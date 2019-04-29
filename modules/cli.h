/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	cli.h
* MODULENAME.:	CLI (static class)
* DOCS.......:	https://git.io/fjJau
* VERSION....:	1.2.1
*
* DESCRIPTION:	Interaction with computer CLI using UART, including message
				logging (send) and command parsing (receieve).
*
****************************************************************************/

#pragma once

/***************************** Include files *******************************/

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include "uart.h"

/*****************************   Constants   *******************************/

#define	CLI_MAX_TYPES		4
#define	CLI_MAX_ACTIONS		12
#define	CLI_MSG_MAXLEN		257

/*****************************    Defines    *******************************/

typedef struct	CLI_ACTION		CLI_ACTION;
typedef struct	CLI_TYPE		CLI_TYPE;

typedef struct	UART_FRAME		UART_FRAME;
typedef enum	UART_FRAME_TYPE	UART_FRAME_TYPE;
typedef enum	CMD_ID			CMD_ID;

#define			CMD_TABLE		cli.commands = (CLI_TYPE[CLI_MAX_TYPES])
#define 		CLI_LAMBDA(_x)	({ void _func(const uint8_t * args) _x _func; })

/***********************     External Variables     ************************/

/*****************************    Constructs   *****************************/

// should match across MCU & CLI
enum CMD_ID
{
	// UART_DO
	DO_ECHO,
	DO_SAMPLE,

	// UART_GET
	GET_ECHO,
	GET_ENC,
	GET_HAL,

	// UART_SET
	SET_MODE,
	SET_POS,
	SET_GUI,
	SET_MSG,
	SET_PWM,
	SET_FREQ,
	SET_SLEW,
	SET_BOUND,
	SET_PID,
};

struct CLI_ACTION
{
	CMD_ID id;
	void(*callback)(const uint8_t* payload);
};

struct CLI_TYPE
{
	UART_FRAME_TYPE type;
	CLI_ACTION actions[CLI_MAX_ACTIONS];
};

/*************************    Class Functions    ***************************/

extern struct CLI_CLASS
{
	CLI_TYPE* 	commands;
	UART*		uart_module;

	bool 		allow_msg;

	void 		(* const init)(CLI_TYPE* commands, UART* uart_module); // optional
	void 		(* const check)(void);
	void 		(* const msg)(const char* str);
	void 		(* const msgf)(const char* str, ...);
	void 		(* const parse_frame)(UART_FRAME* frame);
} cli;

/****************************** End Of Module ******************************/
