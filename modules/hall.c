/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	exm.c
* MODULENAME.:	EXAMPLE
*
* For an API and DESCRIPTION, please refer to the  module
* specification file (.h-file).
*
****************************************************************************/

/***************************** Include files *******************************/

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include "assert.h"

#include "cli.h"
#include "hall.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static HALL*		HALL_new(SPI_ADDR hall_addr);
static void 		HALL_del(HALL* this);

static bool 		HALL_read(HALL* this);

/****************************   Class Struct   *****************************/

struct MOTOR_CLASS hall =
{
	.spi_module		= NULL,

	.new			= &HALL_new,
	.del			= &HALL_del,
	.get			= &HALL_read
};

/***********************   Constructive Functions   ************************/

static HALL* HALL_new(SPI_ADDR hall_addr)
{
	// check SPI module -- why??
	assert(hall.spi_module != NULL);

	// allocate memory
	HALL* this = malloc(sizeof(MOTOR));

	// initialize variables
	this->hall_addr 	= hall_addr;

	// return pointer to instance
	return this;
}

static void HALL_del(HALL* this)
{
	free(this);
}

/*****************************   Functions   *******************************/

static bool HALL_read (HALL* this)
{
	static uint16_t hall_buf = 0;
	static int16_t  hall_dat = 0;
	/*
	if (spi.request(hall.spi_module, this->hall_addr, &hall_buf))
	{
		// replace most-significant nibble with 0 or 1 according to MSB of 12th bit
		// 0000_1000_0101_0000 -> 1111_1000_0101_0000
		// enc_dat = (enc_buf & (1 << 11)) ? (enc_buf | 0xF000) : (hall_buf);

		// cli.logf("Delta of ENC%u is %d ticks.", (this->enc_addr - 3), enc_dat);

		return enc_dat;
	}
	else
	{
		// exception handling here

		return 0;
	}
	*/
}
