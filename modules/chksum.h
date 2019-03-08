/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	chksum.h
* MODULENAME.:	CHECKSUM
* API........:	https://goo.gl/cRHMYG
* VERSION....:	1.0.0
*
* DESCRIPTION:	4 bit BSD checksum on a 16 bit input, of which the least
				signifigant nibble is masked out.
*
****************************************************************************/

#pragma once

/***************************** Include files *******************************/

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

/*****************************    Defines    *******************************/

typedef struct	SPI_FRAME SPI_FRAME;

/***********************     External Variables     ************************/

/*****************************   Constants   *******************************/

/*************************    Class Functions    ***************************/

// copied from spi.h
struct SPI_FRAME
{
	uint8_t addr : 4;
	uint8_t data : 8;
	uint8_t chksum : 4;
};

extern const struct CHECKSUM_CLASS
{
	uint8_t		(*generate)(SPI_FRAME* frame);
	bool 		(*validate)(SPI_FRAME* frame);

} chksum;

/****************************** End Of Module ******************************/
