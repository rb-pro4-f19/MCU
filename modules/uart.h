/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	uart.h
* MODULENAME.:	UART
* API........:	https://goo.gl/HVfyDq
* VERSION....:	1.0.2
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

#include "../tm4c123gh6pm.h"

/*****************************    Defines    *******************************/

typedef struct  UART UART;
typedef struct	UART_FRAME UART_FRAME;
typedef enum    UART_TYPE UART_TYPE;
typedef enum    UART_FRMPART UART_FRMPART;

/***********************     External Variables     ************************/



/*****************************   Constants   *******************************/

#define	EXM_ARRAY_SIZE 8

// http://dev.ti.com/tirex/content/simplelink_cc26x2_sdk_2_20_00_36/docs/driverlib_cc13xx_cc26xx/cc13x0/register_descriptions/CPU_MMAP/UART0.html //


/*************************    Class Functions    ***************************/

extern const struct UART_CLASS
{
	UART*		(*new)(uint8_t clkdiv);
	void		(*del)(UART* this);

	void 		(*send)(UART* this, UART_TYPE type, uint8_t *data, uint8_t size);
	bool		(*read)(UART* this, UART_FRAME* frame);
} uart;

/*****************************    Constructs   *****************************/

enum UART_TYPE
{
    CONNECT,
    RAW,
    GET,
    SET,
    ACK,
    RESPONSE,
    STREAM,
    MSG
};


struct UART_FRAME
{
	uint8_t type : 3;
	uint8_t size : 5;
	uint8_t* payload;
	uint8_t chksum;
};

struct UART
{
	// public
	uint8_t		clkdiv;
};

/****************************** End Of Module ******************************/
