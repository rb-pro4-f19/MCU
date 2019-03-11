/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	spi.h
* MODULENAME.:	SPI
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


/*************************    Class Functions    ***************************/

extern const struct UART_CLASS
{
	UART*		(*new)(uint8_t clkdiv);
	void		(*del)(UART* this);

	bool 		(*send)(UART* this, UART_TYPE addr, uint8_t *data, uint8_t size);
	uint16_t	(*recieve)(UART* this, UART_TYPE addr);
} spi;

/*****************************    Constructs   *****************************/

enum UART_TYPE
{
	GET 	= 0x01,
	SET 	= 0x02
};

enum UART_FRMPART
{
	ADDR,
	DATA,
	CHKSUM
};

struct UART_FRAME
{
	uint8_t type       : 8;
	uint8_t payload    : 8;
	uint8_t chksum     : 8;
};

struct UART
{
	// public
};

/****************************** End Of Module ******************************/
