/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	hal.c
* MODULENAME.:	HAL
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

#include "hal.h"
#include "cli.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static HAL*			HAL_new(SPI_ADDR hal_addr);
static void 		HAL_del(HAL* this);

static HAL_VAL		HAL_read(HAL* this);

/****************************   Class Struct   *****************************/

struct HAL_CLASS hal =
{
	.spi_module		= NULL,

	.new			= &HAL_new,
	.del			= &HAL_del,
	.read			= &HAL_read
};

/***********************   Constructive Functions   ************************/

static HAL* HAL_new(SPI_ADDR hal_addr)
{
	// check SPI module
	assert(hal.spi_module != NULL);

	// allocate memory
	HAL* this = malloc(sizeof(HAL));

	// initialize variables
	this->hal_addr 	= hal_addr;
	this->val 		= HAL_LOW;

	// return pointer to instances
	return this;
}

static void HAL_del(HAL* this)
{
	free(this);
}

/*****************************   Functions   *******************************/

static HAL_VAL HAL_read(HAL* this)
{
	static uint16_t hal_buf = 0;

	if (spi.request(hal.spi_module, this->hal_addr, &hal_buf))
	{
		// hall sensor is active low
		this->val = ((hal_buf >> 0) & 1U) ? HAL_LOW : HAL_HIGH;
		return this->val;
	}

	return HAL_FAULT;
}
