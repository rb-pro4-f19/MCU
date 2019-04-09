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

#include "cli.h"
#include "mot.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

#define WATCHDOG_TIMEOUT_US 800

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static MOTOR*		MOTOR_new(SPI_ADDR mot_addr, SPI_ADDR enc_addr, uint8_t freq_khz);
static void 		MOTOR_del(MOTOR* this);

static void 		MOTOR_operate(MOTOR* this);
static inline void 	MOTOR_feed(MOTOR* this);

static void 		MOTOR_set_pwm(MOTOR* this, uint8_t pwm);
static void 		MOTOR_set_freq(MOTOR* this, uint8_t freq_khz);
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

	this->freq_khz		= 0;
	this->pwm 			= 0;
	this->enc			= 0;

	mot.set_freq(this, freq_khz);

	// return pointer to instance
	return this;
}

static void MOTOR_del(MOTOR* this)
{
	tp.del(this->tp_watchdog);
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
}

static inline void MOTOR_feed(MOTOR* this)
{
	// try feeding the watchdog; reset timer on success
	if (spi.send(mot.spi_module, this->mot_addr, this->pwm))
	{
		tp.set(this->tp_watchdog, tp.now());
	}
}

static void	MOTOR_set_pwm(MOTOR* this, uint8_t pwm)
{
	int8_t pwm_val = pwm;

	if (pwm_val < 0)
	{
		pwm_val = ((~pwm_val) + 1) | 0b10000000;
	}

	if (spi.send(mot.spi_module, this->mot_addr, pwm_val))
	{
		this->pwm = pwm_val;
		cli.logf("PWM of MOT%u was set to %d.", (this->mot_addr - 1), (int8_t)pwm);
	}
	else
	{
		// exception handling here
	}
}

static void MOTOR_set_freq(MOTOR* this, uint8_t freq_khz)
{
	// clamp max frequency
	if (freq_khz > 100) { freq_khz = 100; }

	uint8_t freq_khz_val = freq_khz;

	// set MSB to motor indicator
	// MOT1 -> 1XXX_XXXX
	freq_khz = (this->mot_addr == MOT1) ? (0b10000000 | freq_khz) : (freq_khz);

	// send command to set PWM
	if (spi.send(mot.spi_module, FREQ, freq_khz))
	{
		this->freq_khz = freq_khz;
		cli.logf("FRQ of MOT%u was set to %d kHz.", (this->mot_addr - 1), freq_khz_val);
	}
	else
	{
		// exception handling here
	}
}

static int16_t MOTOR_get_enc(MOTOR* this)
{
	static uint16_t enc_buf = 0;
	static int16_t  enc_dat = 0;

	if (spi.request(mot.spi_module, this->enc_addr, &enc_buf))
	{
		// replace most-significant nibble with 0 or 1 according to MSB of 12th bit
		// 0000_1000_0101_0000 -> 1111_1000_0101_0000
		enc_dat = (enc_buf & (1 << 11)) ? (enc_buf | 0xF000) : (enc_buf);

		cli.logf("Delta of ENC%u is %d ticks.", (this->enc_addr - 3), enc_dat);

		return enc_dat;
	}
	else
	{
		// exception handling here

		return 0;
	}
}

/****************************** End Of Module ******************************/
