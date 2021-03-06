/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	spi.h
* MODULENAME.:	SPI
* DOCS.......:	https://git.io/fjJa2
* VERSION....:	1.2.2
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
# include "../modules/tp.h"

/*****************************    Defines    *******************************/

typedef struct  SPI SPI;
typedef struct	SPI_FRAME SPI_FRAME;
typedef enum    SPI_ADDR SPI_ADDR;
typedef enum    SPI_FRMPART SPI_FRMPART;

/***********************     External Variables     ************************/

/*****************************   Constants   *******************************/

/*************************    Class Functions    ***************************/

extern const struct SPI_CLASS
{
	SPI*		(*new)(uint8_t clkdiv);
	void		(*del)(SPI* this);

	bool 		(*send)(SPI* this, SPI_ADDR addr, uint8_t data);
	bool		(*request)(SPI* this, SPI_ADDR addr, uint16_t* buffer);
	void		(*flush)(SPI* this);
} spi;

/*****************************    Constructs   *****************************/

enum SPI_ADDR
{
	CTRL 	= 0x00,
	MOT0 	= 0x01,
	MOT1 	= 0x02,
	ENC0 	= 0x03,
	ENC1 	= 0x04,
	HAL0 	= 0x05,
	HAL1 	= 0x06,
	CUR0 	= 0x07,
	CUR1 	= 0x08,
	FREQ	= 0x09,

	RES0	= 0x0A,
	RES1	= 0x0B,
	RES2	= 0x0C,
	RES3	= 0x0D,

	RETX 	= 0x0F
};

enum SPI_FRMPART
{
	ADDR,
	DATA,
	CHKSUM
};

struct SPI_FRAME
{
	uint8_t addr 	: 4;
	uint8_t data 	: 8;
	uint8_t chksum 	: 4;
};

struct SPI
{
	// public
	uint8_t		clkdiv;
	TIMEPOINT* 	tp_timeout;
};

/****************************** End Of Module ******************************/
