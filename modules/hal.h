/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	hal.h
* MODULENAME.:	HAL
* API........:	https://git.io/fjLSM
* VERSION....:	1.0.0
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

#include "spi_old.h"

/*****************************    Defines    *******************************/

typedef struct  HAL HAL;
typedef	enum	HAL_VAL HAL_VAL;

/***********************     External Variables     ************************/

/*****************************   Constants   *******************************/

/*************************    Class Functions    ***************************/

extern struct HAL_CLASS
{
	SPI* 		spi_module;

	HAL*		(* const new)(SPI_ADDR HAL_addr);
	void		(* const del)(HAL* this);

	HAL_VAL		(* const read)(HAL* this);

} hal;

enum HAL_VAL
{
	HAL_LOW,
	HAL_HIGH,
	HAL_FAULT
};

/*****************************    Constructs   *****************************/

struct HAL
{
	// public
	SPI_ADDR 	hal_addr;
};

/****************************** End Of Module ******************************/
