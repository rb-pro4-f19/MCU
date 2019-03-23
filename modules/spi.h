/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	spi.h
* MODULENAME.:	SPI
* DOCS.......:	https://git.io/fjJa2
* VERSION....:	1.1.1
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
} spi;

/*****************************    Constructs   *****************************/

enum SPI_ADDR
{
	CTRL 	= 0b0000,
	PWM1 	= 0b0001,
	PWM2 	= 0b0010,
	ENC1 	= 0b0011,
	ENC2 	= 0b0100,
	HAL1 	= 0b0101,
	HAL2 	= 0b0110,
	CUR1 	= 0b0111,
	CUR2 	= 0b1000
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
