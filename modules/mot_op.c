/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	mot.c
* MODULENAME.:	MOTOR
*
* For an API and DESCRIPTION, please refer to the  module
* specification file (.h-file).
*
****************************************************************************/

/***************************** Include files *******************************/

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include "assert.h"

#include "mot.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

#define WATCHDOG_TIMEOUT_US 800		// us

#define SLEWRATE_EN			true
#define SLEWRATE_DY			1
#define SLEWRATE_DX_US		10000	// us

#define SPEED_EN			true
#define SPEED_DX_MS			1		// ms

#define BOUNDARY_TH			30

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static MOTOR*		MOTOR_new(SPI_ADDR mot_addr, SPI_ADDR enc_addr, uint8_t freq_khz);
static void 		MOTOR_del(MOTOR* this);

static void 		MOTOR_operate(MOTOR* this);
static inline void 	MOTOR_feed(MOTOR* this);
static inline void 	_MOTOR_calc_speed(MOTOR* this);

static bool 		MOTOR_set_pwm(MOTOR* this, int8_t pwm);
static bool 		MOTOR_set_freq(MOTOR* this, uint8_t freq_khz);
static int16_t 		MOTOR_get_enc(MOTOR* this);

//static MOTOR* 		_MOTOR_get_handler(SPI_ADDR mot_addr);

/****************************   Class Struct   *****************************/

struct MOTOR_CLASS mot =
{
	.spi_module		= NULL,

	.new			= &MOTOR_new,
	.del			= &MOTOR_del,

	.operate		= &MOTOR_operate,
	.feed			= &MOTOR_feed,

	.set_pwm 		= &MOTOR_set_pwm,
	.set_freq 		= &MOTOR_set_freq,
	.get_enc 		= &MOTOR_get_enc
};

/***********************   Constructive Functions   ************************/

static MOTOR* MOTOR_new(SPI_ADDR mot_addr, SPI_ADDR enc_addr, uint8_t freq_khz)
{
	// check SPI module
	assert(mot.spi_module != NULL);

	// allocate memory
	MOTOR* this = malloc(sizeof(MOTOR));

	// initialize variables
	this->mot_addr 		= mot_addr;
	this->enc_addr 		= enc_addr;

	this->tp_watchdog	= tp.new();
	this->tp_slewrate	= tp.new();
	this->tp_speed		= tp.new();

	this->freq_khz		= 0;
	this->pwm 			= 0;
	this->pwm_target	= 0;
	this->pwm_data		= 0;
	this->enc			= 0;
	this->speed 		= 0.0f;

	this->slew			= SLEWRATE_EN;
	this->slew_dy		= SLEWRATE_DY;
	this->slew_dx		= SLEWRATE_DX_US;

	this->bound 		= false;
	this->bound_l 		= 0;
	this->bound_h 		= 0;
	this->bound_max		= 0;

	mot.set_freq(this, freq_khz);

	// return pointer to instance
	return this;
}

static void MOTOR_del(MOTOR* this)
{
	tp.del(this->tp_watchdog);
	tp.del(this->tp_slewrate);
	free(this);
}

/*****************************   Functions   *******************************/

static void MOTOR_operate(MOTOR* this)
{
	// check WATCHDOG timeout
	if (tp.delta_now(this->tp_watchdog, us) > WATCHDOG_TIMEOUT_US)
	{
		mot.feed(this);
	}

	// check boundary safety if bounded
	if (this->bound)
	{
		// upper boundary must be higher than lower
		assert(this->bound_h > this->bound_l);

		// check that encoder is within boundaries; break motors if not
		// bound_l < enc < bound_h
		if (!(this->enc > (this->bound_l + BOUNDARY_TH) && this->enc < (this->bound_h - BOUNDARY_TH)))
		{
			mot.set_pwm(this, 0);
		}
	}

	// update slewrate pwm
	if (this->pwm != this->pwm_target)
	{
		mot.set_pwm(this, this->pwm_target);
	}
}

static inline void MOTOR_feed(MOTOR* this)
{
	spi.send(mot.spi_module, this->mot_addr, this->pwm_data);
}

static bool	MOTOR_set_pwm(MOTOR* this, int8_t pwm)
{
	// convert pwm to the FPGA format; MSB = direction
	//  31 = ‭00011111‬ -> ‭00011111‬
	// -31 = ‭11100001‬ -> 1‭0011111‬
	this->pwm_data = (pwm < 0) ? ((~pwm) + 1) | 0b10000000 : pwm;
	this->pwm = pwm;
	this->pwm_target = pwm;

	// send data to FPGA; update current PWM on success + reset timers
	spi.send(mot.spi_module, this->mot_addr, this->pwm_data);

	// return
	return true;
}

static bool MOTOR_set_freq(MOTOR* this, uint8_t freq_khz)
{
	// clamp max frequency
	if (freq_khz > 100) { freq_khz = 100; }

	// format for FPGA; set MSB to motor indicator
	// MOT1 -> 1XXX_XXXX or MOT0 -> 0XXX_XXXX
	uint8_t freq_khz_data = (this->mot_addr == MOT1) ? (0b10000000 | freq_khz) : (freq_khz);

	// send command to set PWM
	if (spi.send(mot.spi_module, FREQ, freq_khz_data))
	{
		this->freq_khz = freq_khz;
		return true;
	}
	else
	{
		// exception handling here
		return false;
	}
}

static int16_t MOTOR_get_enc(MOTOR* this)
{
	static uint16_t enc_buf = 0;
	static int16_t	enc_dat = 0;

	// get data from SPI
	spi.request(mot.spi_module, this->enc_addr, &enc_buf);

	// replace most-significant nibble with 0 or 1 according to MSB of 12th bit
	// 0000_1000_0101_0000 -> 1111_1000_0101_0000
	enc_dat = (enc_buf & (1 << 11)) ? (enc_buf | 0xF000) : (enc_buf);

	// update stored encoder value
	this->enc += enc_dat;

	// return the read delta encoder value
	return enc_dat;
}

static inline void _MOTOR_calc_speed(MOTOR* this)
{
	static int16_t prev_enc;

	// sample time
	if (tp.delta_now(this->tp_speed, ms) < SPEED_DX_MS) { return; }

	// save previous encoder value
	prev_enc = this->enc;

	// calculate speed (dy/dx)
	this->speed = abs(this->enc - prev_enc) / SPEED_DX_MS;
}

/****************************** End Of Module ******************************/
