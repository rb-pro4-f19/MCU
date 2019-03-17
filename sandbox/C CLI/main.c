#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdio.h>

#include "cli.h"

void some_func(void)
{
	printf("WOOHO!");
}

int main(void)
{
	CLI_ACTION act1 = { 0x10, &some_func };
	CLI_ACTION act2 = { 0x11, &some_func };

	cli.init((CLI_TYPE[8])
	{
		{ UART_GET,	{
			act1,
			act2
		}},

		{ UART_SET,	{
			{ 0x10, &some_func },
			{ 0x11, &some_func }
		}}
	});

	cli.commands = (CLI_TYPE[8])
	{
		{ UART_GET, {
			act1,
			act2
		}},

		{ UART_SET,	{
			{ 0x10, &some_func },
			{ 0x11, &some_func }
		} }
	};

	CMD_TABLE
	{
		{ UART_CONNECT,	{
			act1,
			act2
		}},

		{ UART_SET,	{
			{ 0x10, &some_func },
			{ 0x11, &some_func }
		}},

		{ UART_GET,	{
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

	getchar();
	
}