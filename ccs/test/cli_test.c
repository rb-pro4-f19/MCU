#include "cli.h"

void cli_test(void)
{

	// cli.h
	// version 1.0.0

	// define callback functions (can be anywhere, not just here)
	void some_func(void) 		{ return; }
	void some_other_func(void) 	{ return; }

	// define some actions manually
	// undesired, just an example!
	CLI_ACTION act1 = { 0x10, &some_func };
	CLI_ACTION act2 = { 0x11, &some_func };

	// initialize UART module and assign to CLI
	UART* uart1 = uart.new(2);
	cli.uart_module = uart1;

	// initialize command table
	// max 8 types and 8 actions per type (#defined in cli.h)
	// undesired types can be omitted
	CMD_TABLE
	{
		{ UART_GET, {
			{ 0x00, &some_func },	// do this
			{ 0x01, NULL }			// n/a
		}},

		{ UART_SET, {
			{ 0x00, NULL },			// n/a
			{ 0x01, NULL }			// n/a
		}}
	};

	// update a callback manually
	// UART_SET -> 0x01 -> some_other_func()
	cli.commands[1].actions[1].callback = &some_other_func;

	// parse some UART_FRAME
	// type is specified by "frame->type" (UART_GET)
	// action identifier is specified by "frame->payload[0]" (123)
	UART_FRAME frm =
	{
	 	 UART_GET,
		 2,
		 (uint8_t[2]){123, 40},
		 0
	};

	// this will call the matching callback
	cli.parse_frame(&frm);

	// log a message to the PC console (max 256 bytes)
	cli.log("Hello world!");

	// check for and parse UART frame
	cli.check();

}
