/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	spi.h
* MODULENAME.:	SPI
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
	SPI*		(*new)(uint8_t clkdiv, uint16_t timeout_ms);
	void		(*del)(SPI* this);

	void 		(*send)(SPI* this, SPI_ADDR addr, uint8_t data);
	SPI_FRAME	(*request)(SPI* this, SPI_ADDR addr, uint8_t size);
} spi;

/*****************************    Constructs   *****************************/

enum SPI_ADDR
{
	CTRL = 0b0000,
	PWM1 = 0b0001,
	PWM2 = 0b0010,
	ENC1 = 0b0011,
	ENC2 = 0b0100,
	HALL1 = 0b0101,
	HALL2 = 0b0110,
	CUR1 = 0b0111,
	CUR2 = 0b1000
};

enum SPI_FRMPART
{
	ADDR,
	DATA,
	CHKSUM
};

struct SPI_FRAME
{
	uint8_t addr : 4;
	uint8_t data : 8;
	uint8_t chksum : 4;
};

struct SPI
{
	// public
	uint8_t		clkdiv;
	uint32_t	timeout_ms;
};

/****************************** End Of Module ******************************/