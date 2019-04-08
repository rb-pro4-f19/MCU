/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	mot.h
* MODULENAME.:	MOTOR
* API........:	https://git.io/fjLSM
* VERSION....:	1.1.0
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

typedef struct  HALL HALL;

/***********************     External Variables     ************************/

/*****************************   Constants   *******************************/

/*************************    Class Functions    ***************************/

extern struct HALL_CLASS
{
	SPI* 		spi_module;

	HALL*		(* const new)(SPI_ADDR hall_addr);
	void		(* const del)(HALL* this);

	bool		(* const read)(HALL* this);
} mot;

/*****************************    Constructs   *****************************/

struct HALL
{
	// public
	SPI_ADDR 	hall_addr;

};

/****************************** End Of Module ******************************/
