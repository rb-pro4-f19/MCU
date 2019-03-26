/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	spi.h
* MODULENAME.:	SPI
* DOCS.......:	https://git.io/fjJa2
* VERSION....:	1.1.3
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
	SPI*		(*new)(uint8_t clkdiv, uint8_t spimodule);
	void		(*del)(SPI* this);

	bool 		(*send)(SPI* this, SPI_ADDR addr, uint8_t data);
	bool		(*request)(SPI* this, SPI_ADDR addr, uint16_t* buffer);
} spi;

/*****************************    Constructs   *****************************/

enum SPI_ADDR
{
	CTRL 	= 0x00,
	PWM0 	= 0x01,
	PWM1 	= 0x02,
	ENC0 	= 0x03,
	ENC1 	= 0x04,
	HAL0 	= 0x05,
	HAL1 	= 0x06,
	CUR0 	= 0x07,
	CUR1 	= 0x08
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
