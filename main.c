#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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

void echo()
{
	cli.log("Got a frame, echoing back.");
}

int main(void)
{

	// disable interrupts
	__disable_irq();

	// init SysTick
	sys_tick_init(SYSTICK_DUR_MS);
	tp.init_systick(SYSTICK_DUR_MS, ms);

	// init UART and assign to CLI
	UART* uart1 = uart.new(2);
	cli.uart_module = uart1;

	// init SPI
	SPI* spi_test = spi.new(2);

	while(1)
	{
		spi.send(spi_test, PWM1, 110);
		for (int i = 0; i < 1000000; i++);
		spi.send(spi_test, PWM2, 126);
		for (int i = 0; i < 1000000; i++);
		spi.send(spi_test, PWM2, 10);
		for (int i = 0; i < 1000000; i++);
		spi.send(spi_test, PWM2, 200);
		for (int i = 0; i < 1000000; i++);
	}

	// init commands
	// max 8 types and 8 actions per type (#defined in cli.h)
	CMD_TABLE
	{
		{ UART_GET, {
			{ 0x00, &echo },        // echo
			{ 0x01, NULL }          // n/a
		}},

		{ UART_SET, {
			{ 0x00, NULL },         // n/a
			{ 0x01, NULL }          // n/a
		}}
	};

	// enable interrupts
	__enable_irq();

	// super-loop
	for(;;)
	{
		// check incoming commands
		cli.check();

		// sleep a little bit
		for(int i = 0; i < 1000; i++);
	}

	return 0;
}
