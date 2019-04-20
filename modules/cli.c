/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	cli.c
* MODULENAME.:	CLI (static class)
*
* For an API and DESCRIPTION, please refer to the  module
* specification file (.h-file).
*
****************************************************************************/

/***************************** Include files *******************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <assert.h>

#include "cli.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static void		CLI_init(CLI_TYPE* commands, UART* uart_module);
static void		CLI_check();
static void		CLI_msg(const char* str);
static void 	CLI_msgf(const char* str, ...);
static void		CLI_parse_frame(UART_FRAME* frame);

/****************************   Class Struct   *****************************/

struct CLI_CLASS cli =
{
	.commands		= NULL,
	.uart_module	= NULL,

	.allow_msg		= true,

	.init			= &CLI_init,
	.check			= &CLI_check,
	.msg			= &CLI_msg,
	.msgf			= &CLI_msgf,
	.parse_frame	= &CLI_parse_frame
};

/***********************   Constructive Functions   ************************/

static void CLI_init(CLI_TYPE* commands, UART* uart_module)
{
	cli.commands = commands;
	cli.uart_module = uart_module;
}

static void CLI_check()
{
	static UART_FRAME rx_frame = { 0, 0, NULL, 0 };

	if (uart.read(cli.uart_module, &rx_frame, true))
	{
		assert(rx_frame.payload != NULL);
		cli.parse_frame(&rx_frame);
	}
}

static void	CLI_msg(const char* str)
{
	// check if logging is allowed
	if (!cli.allow_msg) { return; }

	// count number of chars
	int16_t payload_size = -1;
	while (str[++payload_size] != '\0' && payload_size < 256 + 1);

	// check for maximum size
	// now handled by the UART; message can be as long as desired, yet bounded by UART

	// send frame
	uart.send(cli.uart_module, UART_MSG, str, (size_t)payload_size);
}

static void CLI_msgf(const char* str, ...)
{

	// http://mylifeforthecode.com/creating-a-custom-printf-function-in-c/
	// http://www.cplusplus.com/reference/cstdio/vsprintf/

	va_list vargs;
	static char str_buffer[CLI_MSG_MAXLEN];

	// construct formatted string from variadic args
	va_start(vargs, str);
	vsprintf(str_buffer, str, vargs);
	va_end(vargs);

	// call regular log method w/ constructed string
	cli.msg(str_buffer);
}

static void CLI_parse_frame(UART_FRAME* frame)
{
	// command table must be defined
	assert(cli.commands != NULL);

	// find type index
	int16_t type_index = -1;
	while (cli.commands[++type_index].type != frame->type && type_index < CLI_MAX_TYPES);

	// find action index
	// assuming action identifier is located in first byte of payload
	int16_t action_index = -1;
	while (cli.commands[type_index].actions[++action_index].id != frame->payload[0]
		&& action_index < CLI_MAX_ACTIONS);

	// assert callback
	assert(cli.commands[type_index].actions[action_index].callback != NULL);

	// extract data (increment payload pointer by one)
	if (frame->payload_size != 1) { frame->payload++; }

	// callback w/ data
	cli.commands[type_index].actions[action_index].callback(frame->payload);
}

/****************************** End Of Module ******************************/
