/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	chksum.h
* MODULENAME.:	CHECKSUM
*
* For an API and DESCRIPTION, please refer to the  module
* specification file (.h-file).
*
****************************************************************************/

/***************************** Include files *******************************/

#include <stdint.h>
#include <stdbool.h>

#include "chksum.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

#define NUM_OF_NIBBLES 4

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static uint8_t		CHECKSUM_generate(uint16_t frame);
static bool 		CHECKSUM_validate(uint16_t frame, uint8_t checksum);

static uint8_t		_CHECKSUM_extract_nibble(uint16_t data, int pos);
static uint8_t 		_CHECKSUM_ror_nibble(uint8_t data);
static uint8_t 		_CHECKSUM_rol_nibble(uint8_t data);

/****************************   Class Struct   *****************************/

const struct CHECKSUM_CLASS chksum =
{
	.generate		= &CHECKSUM_generate,
	.validate		= &CHECKSUM_validate
};

/*****************************   Functions   *******************************/

static uint8_t CHECKSUM_generate(uint16_t frame)
{
	// https://en.wikipedia.org/wiki/BSD_checksum

	// variables
	uint8_t checksum = 0;
	frame = frame & 0xFFF0;

	// algorithm starting from Most Signifigant Nibble
	for (int i = NUM_OF_NIBBLES; i >= 0; --i)
	{
		checksum = _CHECKSUM_ror_nibble(checksum);
		checksum = checksum + _CHECKSUM_extract_nibble(frame, i);
		checksum = checksum & 0xF;
	}

	return checksum;
}

static bool CHECKSUM_validate(uint16_t frame, uint8_t checksum)
{
	frame = frame & 0xFFF0;
	return (chksum.generate(frame) == checksum);
}

static uint8_t _CHECKSUM_extract_nibble(uint16_t data, int pos)
{
	// extract nibble at position n
	// test = 0101 1111 1010 0000 -> extract_nibble(test, 2) = 1111

	return ((data & (0xF << pos * 4)) >> (pos * 4));
}

static uint8_t _CHECKSUM_ror_nibble(uint8_t data)
{
	return ((data >> 1) | (data << 3) & 0xF);
}

static uint8_t _CHECKSUM_rol_nibble(uint8_t data)
{
	return ((data << 1) | (data >> 3) & 0xF);
}

/****************************** End Of Module ******************************/
