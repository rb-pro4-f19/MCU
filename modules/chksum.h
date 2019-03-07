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

/***********************     External Variables     ************************/

/*****************************   Constants   *******************************/

/*************************    Class Functions    ***************************/

extern const struct CHECKSUM_CLASS
{
	uint8_t		(*generate)(uint16_t frame);
	bool 		(*validate)(uint16_t frame, uint8_t checksum);

} chksum;

/****************************** End Of Module ******************************/
