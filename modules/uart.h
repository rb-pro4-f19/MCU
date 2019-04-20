/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	uart.h
* MODULENAME.:	UART
* API........:	https://git.io/fjODi
* VERSION....:	1.2.0
*
* DESCRIPTION:	UART serial communication module.
*
****************************************************************************/

#pragma once

/***************************** Include files *******************************/

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include "../tm4c123gh6pm.h"

#include "chksum.h"
#include "tp.h"

/*****************************    Defines    *******************************/

typedef struct  UART UART;
typedef struct	UART_FRAME UART_FRAME;
typedef enum    UART_FRAME_TYPE UART_FRAME_TYPE;
typedef enum    UART_BAUDRATE UART_BAUDRATE;
typedef enum	STREAM_FRAME_SIZE STREAM_FRAME_SIZE;

/***********************     External Variables     ************************/

/*****************************   Constants   *******************************/

/*************************    Class Functions    ***************************/

extern const struct UART_CLASS
{
	UART*		(*new)(UART_BAUDRATE baudrate);
	void		(*del)(UART* this);

	UART_FRAME*	(*newframe)();
	void 		(*delframe)(UART_FRAME* frame);

	void 		(*send)(UART* this, UART_FRAME_TYPE type, const uint8_t* payload, size_t payload_size);
	void 		(*stream)(UART* this, const void* obj, size_t obj_size);
	bool		(*read)(UART* this, UART_FRAME* frame, bool send_ack);
} uart;

/*****************************    Constructs   *****************************/

enum UART_BAUDRATE
{
	BAUD_1200,
	BAUD_9600,
	BAUD_14400,
	BAUD_115200,
	BAUD_921600
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

struct UART_FRAME
{
	UART_FRAME_TYPE	type : 3;
	uint8_t 		payload_size : 5;
	uint8_t* 		payload;
	uint8_t 		chksum;
};

enum STREAM_FRAME_SIZE
{
	STREAM_DAT_FRAME = 1,
	STREAM_CMD_FRAME = 2
};

struct UART
{
	UART_BAUDRATE	baudrate;
	TIMEPOINT*		tp_timeout;
};

/****************************** End Of Module ******************************/
