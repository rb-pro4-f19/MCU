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

void echo(const uint8_t* payload)
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
	UART* uart1 = uart.new(BAUD_9600);
	cli.uart_module = uart1;

	// init SPI
	SPI* spi_test = spi.new(2);

	// set pwm callback
	void set_pwm(const uint8_t* payload)
	{
		SPI_ADDR	motor = payload[1] + 1;
		uint8_t		value = payload[2];

		spi.send(spi_test, motor, value);

		cli.log("Set PWM!");
	}

	// get encoder callback
	void get_enc(const uint8_t* payload)
	{
		SPI_ADDR	enc = payload[1] + 3;

		uint16_t buf = 0;
		spi.request(spi_test, enc, &buf);

		cli.log("Got encoder!");
	}

	// init commands
	// max 8 types and 8 actions per type (#defined in cli.h)
	CMD_TABLE
	{
		{ UART_GET, {
			{ 0x00, &echo },        // echo
			{ 0x01, &get_enc }      // n/a
		}},

		{ UART_SET, {
			{ 0x00, NULL },         // n/a
			{ 0x01, &set_pwm }      // n/a
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
		for(int i = 0; i < 10000; i++);
	}

	return 0;
}
