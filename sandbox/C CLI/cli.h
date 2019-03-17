/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	exm.h
* MODULENAME.:	EXAMPLE
* API........:	github.com/rb-pro4-f19/MCU/blob/master/README.md
* VERSION....:	1.0.0
*
* DESCRIPTION:	An example module. This might have a lengthy description, in
*				which case, we simply add some tabs.
*
****************************************************************************/

#pragma once

/***************************** Include files *******************************/

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include <stdio.h>

/*****************************   Constants   *******************************/

#define	CLI_MAX_TYPES		8
#define	CLI_MAX_ACTIONS		256

/*****************************    Defines    *******************************/

typedef struct	CLI_ACTION		CLI_ACTION;
typedef struct	CLI_TYPE		CLI_TYPE;

typedef struct	UART_FRAME		UART_FRAME;
typedef enum	UART_FRAME_TYPE	UART_FRAME_TYPE;

#define			CMD_TABLE		cli.commands = (CLI_TYPE[CLI_MAX_TYPES])

/***********************     External Variables     ************************/

/*************************    Class Functions    ***************************/

extern struct CLI_CLASS
{
	CLI_TYPE* commands;

	void(*init)(CLI_TYPE* commands);
	void(*parse_frame)(UART_FRAME* frame);
} cli;

/*****************************    Constructs   *****************************/

struct UART_FRAME
{
	UART_FRAME_TYPE	type : 3;
	uint8_t 		payload_size : 5;
	uint8_t* 		payload;
	uint8_t 		chksum;
};

enum UART_FRAME_TYPE
{
	UART_CONNECT	= 0x00,
	UART_RAW		= 0x01,
	UART_GET		= 0x02,
	UART_SET		= 0x03,
	UART_ACK		= 0x04,
	UART_RESPONSE	= 0x05,
	UART_STREAM		= 0x06,
	UART_MSG		= 0x07
};

struct CLI_ACTION
{
	uint8_t id;
	void(*callback)(void);
};

struct CLI_TYPE
{
	UART_FRAME_TYPE type;
	CLI_ACTION actions[CLI_MAX_ACTIONS];
};

/****************************** End Of Module ******************************/