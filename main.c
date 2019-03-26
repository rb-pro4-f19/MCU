#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>

#include "driver.h"
#include "tp.h"
#include "spi.h"
#include "uart.h"
#include "cli.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

#define SYSTICK_DUR_MS 1

/*****************************   Variables   *******************************/

/************************    External Methods    ***************************/

extern void tp_test(void);
extern void chksum_test(void);
extern void spi_test(void);
extern void uart_test(void);
extern void cli_test(void);

/************************   Interrupt Handlers   ***************************/

void ISR_SYSTICK(void)
{
	tp.systick();
}

/*******************************   Main   **********************************/

int main(void)
{

	/** Initialization *****************************************************/

	// disable interrupts
	__disable_irq();

	// init SysTick
	sys_tick_init(SYSTICK_DUR_MS);
	tp.init_systick(SYSTICK_DUR_MS, ms);

	// init UART and assign to CLI
	UART* uart1 = uart.new(BAUD_9600);
	cli.uart_module = uart1;

	// init SPI
	SPI* spi_test = spi.new(16);

	/** Callbacks **********************************************************/

	// echo callback
	void echo(const uint8_t* args)
	{
		cli.log("Got a frame, echoing back.");
	}

	// set pwm callback
	void set_pwm(SPI_ADDR motor, int8_t value)
	{
		if (spi.send(spi_test, motor, value))
		{
			cli.logf("PWM of MOT%u was set to %d.", (motor - 1), value);
		}
		else
		{
			cli.logf("Error settting PWM.");
		}
	}

	// get encoder callback
	void get_enc(SPI_ADDR enc)
	{
		uint16_t enc_dat = 0;
		spi.request(spi_test, enc, &enc_dat);

		cli.logf("Delta of ENC%u is %d ticks.", (enc - 3), enc_dat);
	}

	/** Command Table ******************************************************/

	// init commands
	// max 8 types and 8 actions per type (#defined in cli.h)
	CMD_TABLE
	{
		{ UART_GET, {
			{ 0x00, &echo },
			{ 0x01, CLI_LAMBDA({ get_enc(args[0]); }) }
		}},

		{ UART_SET, {
			{ 0x00, NULL },
			{ 0x01, CLI_LAMBDA({ set_pwm(args[0], args[1]); }) }
		}}
	};

	/** Main ***************************************************************/

	// enable interrupts
	    __enable_irq();

	// super-loop
	for(;;)
	{
		// check incoming commands
		cli.check();

		// sleep a little bit
		for(int i = 0; i < 10000; i++);
	}

	/** Garbage Collection *************************************************/

	return 0;
}
