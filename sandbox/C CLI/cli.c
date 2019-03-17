/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	exm.c
* MODULENAME.:	EXAMPLE
*
* For an API and DESCRIPTION, please refer to the  module
* specification file (.h-file).
*
****************************************************************************/

/***************************** Include files *******************************/

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include <assert.h>

#include "cli.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static void		CLI_init(CLI_TYPE* commands);
static void		CLI_parse_frame(UART_FRAME* frame);

/****************************   Class Struct   *****************************/

struct CLI_CLASS cli =
{
	.commands		= NULL,
	.init			= &CLI_init,
	.parse_frame	= &CLI_parse_frame
};

/***********************   Constructive Functions   ************************/

static void CLI_init(CLI_TYPE* commands)
{
	cli.commands = commands;
}

static void CLI_parse_frame(UART_FRAME* frame)
{

	// command table must be defined
	assert(cli.commands != NULL);

	// find type index
	int type_index = -1;
	while (cli.commands[++type_index].type != frame->type && type_index < CLI_MAX_TYPES);

	// find action index
	// assuming action identifier is located in first byte of payload
	int action_index = -1;
	while (cli.commands[type_index].actions[++action_index].id != frame->payload[0] && action_index < CLI_MAX_ACTIONS);

	// call appropriate callback
	assert(cli.commands[type_index].actions[action_index].callback != NULL);
	cli.commands[type_index].actions[action_index].callback();
}

/****************************** End Of Module ******************************/