#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>

#include "driver.h"
#include "tp.h"
#include "spi_old.h"
#include "uart.h"
#include "cli.h"
#include "mot.h"

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

	// init UART @ 9600 baud
	UART* uart_main = uart.new(BAUD_9600);

	// init SPI @ 1 MHz (16 000 000 / 16)
	SPI* spi_main = spi.new(16);

	// assign UART module to CLI
	cli.uart_module = uart_main;

	// init MOT0 w/ ENC0 to 10 kHz
	MOTOR* mot0 = mot.new(MOT0, ENC0, 10);
	mot0->spi_module = spi_main;

	// init MOT1 w/ ENC0 to 10 kHz
	MOTOR* mot1 = mot.new(MOT1, ENC1, 10);
	mot1->spi_module = spi_main;

	/** Callbacks **********************************************************/

	// echo callback
	void echo(const uint8_t* args)
	{
		cli.log("Got a frame, echoing back.");
	}

	/** Command Table ******************************************************/

	// init commands
	// max 8 types and 8 actions per type (#defined in cli.h)
	CMD_TABLE
	{
		{ UART_GET, {
			{ 0x00, &echo },
			{ 0x01, CLI_LAMBDA({ mot.get_enc(args[0] == MOT1 ? mot1 : mot0); }) }
		}},

		{ UART_SET, {
			{ 0x00, NULL },
			// { 0x01, CLI_LAMBDA({ mot.set_pwm(args[0], args[1]); }) },
			{ 0x01, CLI_LAMBDA({ mot.set_pwm((args[0] == MOT1 ? mot1 : mot0), args[1]); }) },
			{ 0x02, CLI_LAMBDA({ mot.set_freq((args[0] == MOT1 ? mot1 : mot0), args[1]); }) }
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
