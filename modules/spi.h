/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	spi.h
* MODULENAME.:	EXAMPLE
* API........:	github.com/rb-pro4-f19/MCU/blob/master/README.md
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

typedef struct  SPI SPI;
typedef struct	SPI_FRAME SPI_FRAME;
typedef enum    SPI_ADDR SPI_ADDR;
typedef enum    SPI_FRMPART SPI_FRMPART;

/***********************     External Variables     ************************/

/*****************************   Constants   *******************************/

#define	EXM_ARRAY_SIZE 8

/*************************    Class Functions    ***************************/

extern const struct SPI_CLASS
{
	SPI*		(*new)(int16_t baudrate, int16_t timeout_ms);
	void		(*del)(SPI* this);

	void 		(*send)(SPI* this, int8_t data, SPI_ADDR addr, bool ack);
	int16_t		(*request)(SPI* this, SPI_ADDR addr, int8_t size, int32_t timeout_ms);
} spi;

/*****************************    Constructs   *****************************/

enum SPI_ADDR
{
	CTRL = 0x00,
	MOT1,
	MOT2,
	ENC1,
	ENC2,
	HAL1,
	HAL2,
	CUR1,
	CUR2
};

enum SPI_FRMPART
{
	ADDR,
	DATA,
	CHKSUM
};

struct SPI_FRAME
{
	int8_t addr : 4;
	int8_t data : 8;
	int8_t chksum : 4;
};

struct SPI
{
	// public
	int16_t		baudrate;
	int32_t		timeout_ms;
};

/****************************** End Of Module ******************************/
