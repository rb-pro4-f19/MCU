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
****************************************************************************

/***************************** Include files *******************************/

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include "spi.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

static int32_t		_global_var = 0;

/************************  Function Declarations ***************************/

static SPI*			SPI_new(int16_t baudrate, int16_t timeout_ms);
static void 		SPI_del(SPI* this);

static void 		SPI_send(SPI* this, int8_t data, SPI_ADDR addr, bool ack);
static int16_t 		SPI_request(SPI* this, int32_t timeout_ms);

static void 		_SPI_transmit(SPI_FRAME* frame);
static SPI_FRAME	_SPI_recieve();
static int8_t		_SPI_checksum_generate(SPI_FRAME* frame);
static bool			_SPI_checksum_validate(SPI_FRAME* frame);

/****************************   Class Struct   *****************************/

const struct SPI_CLASS spi =
{
	.new			= &SPI_new,
	.del			= &SPI_del,

	.send			= &SPI_send,
	.request		= &SPI_request
};

/***********************   Constructive Functions   ************************/

static SPI* SPI_new(int16_t baudrate, int16_t timeout_ms)
/****************************************************************************
*   Input    : type = desired EXM_TYPE for the instance.
			 : init_val = desired value for `var`.
*   Output   : Pointer to a new EXAMPLE instance.
*   Function : Constructor of a EXAMPLE instance.
****************************************************************************/
{
	// allocate memory
	SPI* this = malloc(sizeof(SPI));

	// initialize variables
	this->baudrate 		= init_val;
	this->timeout_ms	= 0;
	_SPI_checksum_generate(&((SPI_FRAME){10, 10, 0}))
	// return pointer to instance
	return this;
}

static void SPI_del(SPI* this)
/****************************************************************************
*   Input    : this = pointer to a EXAMPLE instance.
*   Function : Destructor of a EXAMPLE instance.
****************************************************************************/
{
	free(this);
}

/*****************************   Functions   *******************************/

static void SPI_send(SPI* this, int8_t data, SPI_ADDR addr, bool ack)
/****************************************************************************
*   Input    : this = pointer to a EXAMPLE instance.
*   Function : Sets `is_set` of an EXAMPLE instance to false, such that
			   something interesting may happen. Maybe.
****************************************************************************/
{
	SPI_FRAME frame= {addr, data, 0};
	frame.chksum = _SPI_checksum_generate(&frame);
	_SPI_transmit(&frame);// skal skrives
}

static int32_t _EXAMPLE_private_func(void)
/****************************************************************************
*   Function : Some arithmetic expression.
****************************************************************************/
{
	int32_t temp = 20 * ((7 + 4) / (-5));
	return temp;
}

/****************************** End Of Module ******************************/
