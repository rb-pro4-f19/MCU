/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	chksum.h
* MODULENAME.:	CHECKSUM
* API........:	https://goo.gl/cRHMYG
* VERSION....:	1.1.0
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

#include "spi.h"

/*****************************    Defines    *******************************/

typedef enum CHKSUM_SIZE CHKSUM_SIZE;

/***********************     External Variables     ************************/

/*****************************   Constants   *******************************/

/*************************    Class Functions    ***************************/

extern const struct CHECKSUM_CLASS
{
	uint8_t		(*generate)(SPI_FRAME* frame);
	bool 		(*validate)(SPI_FRAME* frame);

} chksum;

/****************************** End Of Module ******************************/
