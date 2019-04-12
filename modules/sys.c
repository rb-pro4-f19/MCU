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

#define SYSTICK_DUR_MS		1000	// ms
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

TIMEPOINT*	tp_test;

/************************  Function Declarations ***************************/

static void 		SYSTEM_init(void);
static void 		SYSTEM_operate(void);
static void			SYSTEM_echo(void);

static void 		SYSTEM_set_mode(SYS_MODE mode);
static void 		SYSTEM_set_pwm(SPI_ADDR mot_addr, int8_t pwm);
static void 		SYSTEM_set_freq(SPI_ADDR mot_addr, uint8_t freq_khz);
static void 		SYSTEM_set_pos(uint8_t theta);
static void 		SYSTEM_set_enc(uint8_t ticks);

static void 		SYSTEM_get_enc(SPI_ADDR mot_addr);

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

	.set_pwm		= &SYSTEM_set_pwm,
	.set_freq 		= &SYSTEM_set_freq,
	.set_mode		= NULL,
	.set_pos		= NULL,
	.set_enc		= NULL,

	.get_enc 		= &SYSTEM_get_enc,
};

/*****************************   Functions   *******************************/

static void SYSTEM_init(void)
{
	// disable interrupts
	__disable_irq();

	// init SysTick
	sys_tick_init(SYSTICK_DUR_MS);
	tp.init_systick(SYSTICK_DUR_MS, us);

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

	// init HALL1
	hal1 = hal.new(HAL1);

	// update variables and state
	sys.is_init = true;
	sys.state = OPERATION;
	sys.tp_sys = tp.new();

	// setup test timepoint
	tp_test = tp.new();

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

			sys.is_cal = 1;

			static int16_t
				temp0_mot1,
				temp1_mot1 = 0;

			static CALI_MODES
				cur_cali = CALI_TURN_OFF;

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
						temp1_mot1 = mot.get_enc(mot1);
						cur_cali = CALI_STOP_MOT1;

						break;

					case CALI_STOP_MOT1:

						temp0_mot1 = temp1_mot1;
						temp1_mot1 = mot.get_enc(mot1);

						// The if basically make sure that if it is 3 tick different then motor also stops
						// due to might work as a spring

						if ( abs(temp0_mot1, temp1_mot1, ALLOW_EOR_CALI) )
						{

							temp1_mot1 = 0;
							mot.set_pwm(mot1, -30);
							cur_cali = CALI_MOT1;

						}

						break;

					case CALI_MOT1:

						temp1_mot1 += mot.get_enc(mot1);

						// Idk which hall is which.

						if ( hal.read(hal1) == HAL_HIGH )
						{

							mot.set_pwm(mot1, 0);
							mot.set_pwm(mot0, 30);
							cur_cali = CALI_STOP_MOT0;
							sys.is_cal = 0;

						}

						break;

					case CALI_STOP_MOT0:

						// Idk which hall is which.

						if ( hal.read(hal0) == HAL_LOW )
						{

							mot.set_pwm(mot0, 0);
							sys.state = IDLE;
							sys.is_cal = 0;

						}

						break;

				};



			};

			mot.operate(mot0);
			mot.operate(mot1);

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

			// if ((tp.delta_now(tp_test, ms) > 800) && (mot1->pwm != 0))
			// {
			// 	static int8_t pwm = 31;
			// 	pwm = ~pwm;
			// 	mot.set_pwm(mot1, pwm);
			// 	tp.set(tp_test, tp.now());
			// }

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

static void SYSTEM_get_enc(SPI_ADDR mot_addr)
{
	int16_t enc_dat =  mot.get_enc(mot_addr == ENC1 ? mot1 : mot0);
	cli.logf("Delta of ENC%u is %d ticks.", mot_addr - 3, enc_dat);
}

/****************************** End Of Module ******************************/
