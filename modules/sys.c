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

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include "sys.h"

/*****************************    Defines    *******************************/

#define abs(X,Y,Z) ( X > Y ? X - Y > Z : Y - Z > Z )

/*****************************   Constants   *******************************/

#define SYSTICK_DUR_MS		1		// ms
#define DEFAULT_MOT_FREQ	80		// kHz

#define UPDATE_TP_CALI		10		// ms
#define ALLOW_EOR_CALI		3		// amount of ticks allowed as error

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
static void 		SYSTEM_set_pos(uint8_t theta);
static void 		SYSTEM_set_enc(uint8_t ticks);

/****************************   Class Struct   *****************************/

struct SYSTEM_CLASS sys =
{
	.state			= IDLE,
	.is_init		= false,
	.is_cal			= false,
	.tp_sys			= NULL,

	.init			= &SYSTEM_init,
	.operate		= &SYSTEM_operate,
	.echo 			= &SYSTEM_echo,

	.set_mode		= NULL,
	.set_pos		= NULL,
	.set_enc		= NULL,
};

/*****************************   Functions   *******************************/

static void SYSTEM_init(void)
{
	// disable interrupts
	__disable_irq();

	// init SysTick
	sys_tick_init(SYSTICK_DUR_MS);
	tp.init_systick(SYSTICK_DUR_MS, ms);

	// init UART @ 9600 baud
	uart_main = uart.new(BAUD_9600);

	// init SPI @ 1 MHz (16 000 000 / 16)
	spi_main = spi.new(16);

	// assign UART module to CLI class
	cli.uart_module = uart_main;

	// assign SPI to MOTOR class
	mot.spi_module = spi_main;

	// init MOT0 w/ ENC0 to 10 kHz
	mot0 = mot.new(MOT0, ENC0, DEFAULT_MOT_FREQ);

	// init MOT1 w/ ENC0 to 10 kHz
	mot1 = mot.new(MOT1, ENC1, DEFAULT_MOT_FREQ);

	// init HALL0
	hal0 = hal.new(HAL0);

	// init HALL0
	hal1 = hal.new(HAL1);

	// update variables and state
	sys.is_init = true;
	sys.state = OPERATION;
	sys.tp_sys	= tp.new();

	// enable interrupts
	__enable_irq();
}

static void SYSTEM_operate(void)
{
	switch (sys.state)
	{

		case IDLE:
		{
			break;
		}

		case CALIBRATION:
		{
			/*
			
			static int16_t
				cur_enc_mot0,
				prev_enc_mot0,
				cur_enc_mot1,
				prev_enc_mot1 = 0;

			static CALI_MODES cur_cali = CALI_TURN_OFF;

			if ( cur_cali == CALI_TURN_OFF || ( tp.delta_now(sys.tp_sys, ms) > UPDATE_TP_CALI ) )
			{

				tp.set(sys.tp_sys, tp.now());

				switch (cur_cali)
				{
					case CALI_TURN_OFF:

						// Safety

						mot.set_pwm(mot0, 0);
						mot.set_pwm(mot1, 0);
						mot.get_enc(mot0);
						mot.get_enc(mot1);
						cur_cali = CALI_GOMAX_MOT1;

						break;

					case CALI_GOMAX_MOT1:

						// Should be fast enough so that the motor encoder changes
						// atleast every 10 ms

						mot.set_pwm(mot1, 30);
						cur_enc_mot1 = mot.get_enc(mot1);
						cur_cali = CALI_STOP_MOT1;

						break;

					case CALI_STOP_MOT1:

						prev_enc_mot1 = cur_enc_mot1;
						cur_enc_mot1 = mot.get_enc(mot1);

						// The if basically make sure that if it is 3 tick different then motor also stops
						// due to might work as a spring

						if ( abs(prev_enc_mot1, cur_enc_mot1, ALLOW_EOR_CALI) )
						{
							mot.set_pwm(mot1, 0);
							cur_cali = CALI_MOT1;
						}

						break;

					case CALI_MOT1:

						prev_enc_mot1 = cur_enc_mot1;
						cur_enc_mot1 = mot.get_enc(mot1);
						// The if basically make sure that if it is one tick different, then motor also stops.

						break;

					case CALI_STOP_MOT0:

						break;

				};

				mot.operate(mot0);
				mot.operate(mot1);

			};

			*/

		}

		case TUNING:
		{
			break;
		}

		case OPERATION:
		{
			// control motors (feed the watchdog, woof woof)
			mot.operate(mot0);
			mot.operate(mot1);

			break;
		}

		default:
		{
			sys.state = IDLE;
			break;
		}

	}
}

static void	SYSTEM_echo(void)
{
	cli.log("Got a frame, echoing back.");
}

/****************************** End Of Module ******************************/
