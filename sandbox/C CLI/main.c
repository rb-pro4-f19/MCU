#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdio.h>

#include "..\..\modules\cli.h"

void some_func(void)
{
	printf("WOOHO!");
}

int main(void)
{
	CLI_ACTION act1 = { 0x10, &some_func };
	CLI_ACTION act2 = { 0x11, &some_func };

	#ifndef _MSC_VER
		UART* some_uart = uart.new(2);
		cli.uart_module = some_uart;
	#endif // !_MSC_VER	

	CMD_TABLE
	{
		{ UART_CONNECT, (CLI_ACTION[2]){
			&act1,
			&act2
		}},

		{ UART_SET, (CLI_ACTION[2]) {
			{ 0x10, &some_func },
			{ 0x10, &some_func }
		}},

		{ UART_GET,	(CLI_ACTION[2]) {
			{ 123, &some_func },
			{ 255, &some_func }
		}}
	};

	// testing with some random frame
	UART_FRAME frm =
	{
	 	 UART_GET,
		 2,
		 (uint8_t[2]){255, 0},
		 0
	};

	cli.parse_frame(&frm);

	// log test
	cli.log("This is a test string.");

	// stall
	getchar();
}