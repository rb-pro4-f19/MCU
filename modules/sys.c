/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	sys.c
* MODULENAME.:	SYSTEM
*
* For an API and DESCRIPTION, please refer to the  module
* specification file (.h-file).
*
****************************************************************************/

/***************************** Include files *******************************/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include "sys.h"

/*****************************    Defines    *******************************/

//#define abs(X,Y,Z) ( (X > Y) ? (X - Y) > Z : (Y - X) > Z )

/*****************************   Constants   *******************************/

#define SYSTICK_DUR_US				500		// us
#define DEFAULT_MOT_FREQ			80		// kHz

#define CAL_PAN_SEEK_BOUNDARY_DUR	300		// ms
#define CAL_PAN_SEEK_BOUNDARY_PWM	30
#define CAL_PAN_SEEK_HAL_PWM		-20
#define CAL_TILT_SEEK_HAL_PWM		30
#define CAL_TILT_FINETUNE_PWM		-10
#define CAL_TILT_FINETUNE_DUR		200		// ms

/*****************************   Variables   *******************************/

SPI*		spi_main;
UART* 		uart_main;
MOTOR* 		mot0;
MOTOR* 		mot1;
HAL*		hal0;
HAL*		hal1;

/************************  Function Declarations ***************************/

static void 		SYSTEM_init(void);
static void 		SYSTEM_operate(void);
static void			SYSTEM_echo(void);

static void 		SYSTEM_set_mode(SYS_MODE mode);
static void 		SYSTEM_set_pwm(SPI_ADDR mot_addr, int8_t pwm);
static void 		SYSTEM_set_freq(SPI_ADDR mot_addr, uint8_t freq_khz);
//static void 		SYSTEM_set_pos(uint8_t theta);
//static void 		SYSTEM_set_enc(uint8_t ticks);

static void 		SYSTEM_get_enc(SPI_ADDR enc_addr);
static void 		SYSTEM_get_hal(SPI_ADDR hal_addr);

static void 		_SYSTEM_MODE_calibration(void);

/****************************   Class Struct   *****************************/

struct SYSTEM_CLASS sys =
{
	.state			= SYS_IDLE,
	.is_init		= false,
	.is_cal			= false,
	.tp_cal			= NULL,

	.init			= &SYSTEM_init,
	.operate		= &SYSTEM_operate,
	.echo 			= &SYSTEM_echo,

	.set_pwm		= &SYSTEM_set_pwm,
	.set_freq 		= &SYSTEM_set_freq,
	.set_mode		= NULL,
	.set_pos		= NULL,
	.set_enc		= NULL,

	.get_enc 		= &SYSTEM_get_enc,
	.get_hal 		= &SYSTEM_get_hal,
};

/*****************************   Functions   *******************************/

static void SYSTEM_init(void)
{
	// disable interrupts
	__disable_irq();

	// init SysTick
	sys_tick_init(SYSTICK_DUR_US);
	tp.init_systick(SYSTICK_DUR_US, us);

	// init UART @ 9600 baud
	uart_main = uart.new(BAUD_9600);

	// init SPI @ 1 MHz (16 000 000 / 16)
	spi_main = spi.new(16);

	// assign UART module to CLI class
	cli.uart_module = uart_main;

	// assign SPI to MOTOR class
	mot.spi_module = spi_main;

	// assign SPI to HALL class
	hal.spi_module = spi_main;

	// init MOT0 w/ ENC0 to 10 kHz
	mot0 = mot.new(MOT0, ENC0, DEFAULT_MOT_FREQ);

	// init MOT1 w/ ENC0 to 10 kHz
	mot1 = mot.new(MOT1, ENC1, DEFAULT_MOT_FREQ);

	// init HALL0
	hal0 = hal.new(HAL0);

	// init HALL1
	hal1 = hal.new(HAL1);

	// setup TIMEPOINTS
	sys.tp_cal = tp.new();

	// update variables and state
	sys.is_init = true;
	sys.state = SYS_IDLE;

	// enable interrupts
	__enable_irq();
}

/********************************   FSM   **********************************/

static void SYSTEM_operate(void)
{
	switch (sys.state)
	{

		case SYS_IDLE:
		{
			break;
		}

		case SYS_CALIBRATION:
		{

			_SYSTEM_MODE_calibration();

			// control motors (feed the watchdog, woof woof)
			mot.operate(mot0);
			mot.operate(mot1);

			break;
		}

		case SYS_TUNING:
		{
			break;
		}

		case SYS_OPERATION:
		{
			// control motors (feed the watchdog, woof woof)
			mot.operate(mot0);
			mot.operate(mot1);

			break;
		}

		default:
		{
			sys.state = SYS_IDLE;
			break;
		}

	}
}

/***************************   System Calls   ******************************/

static void	SYSTEM_echo(void)
{
	cli.log("Got a frame, echoing back.");
}

static void SYSTEM_set_mode(SYS_MODE mode)
{
	sys.mode = mode;
	cli.logf("System mode set to %d.", mode);
}

static void SYSTEM_set_pwm(SPI_ADDR mot_addr, int8_t pwm)
{
	mot.set_pwm(mot_addr == MOT1 ? mot1 : mot0, pwm);
	cli.logf("PWM of MOT%u was set to %d.", mot_addr - 1, pwm);
}

static void SYSTEM_set_freq(SPI_ADDR mot_addr, uint8_t freq_khz)
{
	mot.set_freq(mot_addr == MOT1 ? mot1 : mot0, freq_khz);
	cli.logf("FRQ of MOT%u was set to %d kHz.", mot_addr - 1, freq_khz);
}

static void SYSTEM_get_enc(SPI_ADDR enc_addr)
{
	int16_t enc_dat =  mot.get_enc(enc_addr == ENC1 ? mot1 : mot0);
	cli.logf("Delta of ENC%u is %d ticks.", enc_addr - 3, enc_dat);
}

static void SYSTEM_get_hal(SPI_ADDR hal_addr)
{
	bool hal_dat = hal.read(hal_addr == HAL1 ? hal1 : hal0);
	cli.logf("Sensor HAL%u is %d.", hal_addr - 5, hal_dat);
}

/****************************   System Modes   *****************************/

static void _SYSTEM_MODE_calibration(void)
{
	// static variables 
  // why are they not declared?
	static int16_t	mot1_enc_cur, mot1_enc_prev;
	static CAL_MODE	cal_state = CAL_RESET;

	// calibration state machine
	switch (cal_state)
	{

		case CAL_RESET:
		{
			// stop motors
			// PAN  = mot1
			// TILT = mot0
			mot.set_pwm(mot0, 0);
			mot.set_pwm(mot1, 0);

			// wait for motor stop to take effect
			if ((mot0->pwm & mot1->pwm) != 0) { break; }

			// reset variables
			sys.is_cal = false;
			mot1_enc_cur = 0;
			mot1_enc_prev = 0;

			// temporary disable slew rate
			mot0->slew = false;
			mot1->slew = false;

			// begin calibration
			cal_state = CAL_PAN_INIT;

			break;
		}

		case CAL_PAN_INIT:
		{
			// set initial PWM of PAN motor and switch to seeking
			// for boundary; begin sampling encoder.

			mot.set_pwm(mot1, CAL_PAN_SEEK_BOUNDARY_PWM);
			mot1_enc_cur = mot.get_enc(mot1);
			cal_state = CAL_PAN_SEEK_BOUNDARY;

			break;
		}

		case CAL_PAN_SEEK_BOUNDARY:
		{
			// sample rate of encoder boundary check
			if (tp.delta_now(sys.tp_cal, ms) < CAL_PAN_SEEK_BOUNDARY_DUR ) { break; }

			// should be fast enough so that the motor encoder changes
			// at least every 10 ms.

			// update sampling TIMEPOINT
			tp.set(sys.tp_cal, tp.now());

			mot1_enc_prev = mot1_enc_cur;
			mot1_enc_cur  = mot.get_enc(mot1);

			// check if motor is stuck; break if false
			if (abs(mot1_enc_cur - mot1_enc_prev) != 0) { break; }

			// !!!!
			// implement redundancy check.. counter or shiftreg
      

      // save the encoder value here, since this is the "best reference"
      // possible 
      // mot1_enc_cur   = 0;

			// reverse motor direction and begin seeking for hall sensor
			mot.set_pwm(mot1, CAL_PAN_SEEK_HAL_PWM);
			cal_state = CAL_PAN_SEEK_HAL;

			break;
		}

		case CAL_PAN_SEEK_HAL:
		{
			// check if hall sensor is reached; break if not
			if (hal.read(hal1) != HAL_HIGH) { break; }

			// stop PAN motor and switch to calibrating TILT motor
			mot.set_pwm(mot1, 0);
			cal_state = CAL_TILT_INIT;

			break;
		}

		case CAL_TILT_INIT:
		{
			mot.set_pwm(mot0, CAL_TILT_SEEK_HAL_PWM);
			cal_state = CAL_TILT_SEEK_HAL;

			break;
		}

		case CAL_TILT_SEEK_HAL:
		{
			// check if hall sensor is reached; break if not
			if (hal.read(hal0) != HAL_HIGH) { break; }

			// enable slew rate
			// due to sharp switch, i.e. 30 pwm to -10 pwm
			mot0->slew = true;
			mot1->slew = true;

			// set PWM to finetune the position of TILT motor
			// reset TIMEPOINT and begin
			mot.set_pwm(mot0, CAL_TILT_FINETUNE_PWM);
			tp.set(sys.tp_cal, tp.now());
			cal_state = CAL_TILT_FINETUNE;

			break;
		}

		case CAL_TILT_FINETUNE:
		{
			// !!!!
			// should only be called if delta ticks > x

			// check if desired finetune duration has passed
			if (tp.delta_now(sys.tp_cal, ms) < CAL_TILT_FINETUNE_DUR ) { break; }

			// stop motor
			mot.set_pwm(mot0, 0);

			break;
		}

		case CAL_FINISH:
		{
			// finish it up, Bob!
      // for this to be an actual calibration the encoder values should be read
      // and saved here. 
			sys.is_cal = true;
			sys.state = SYS_IDLE;

			return;
		}

	};
}

/****************************** End Of Module ******************************/
