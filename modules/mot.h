/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	mot.h
* MODULENAME.:	MOTOR
* API........:	n/a
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

typedef struct  MOTOR MOTOR;

/***********************     External Variables     ************************/

/*****************************   Constants   *******************************/

/*************************    Class Functions    ***************************/

extern const struct MOTOR_CLASS
{
	MOTOR*		(*new)(SPI_ADDR mot_addr, SPI_ADDR enc_addr, uint8_t freq_khz);
	void		(*del)(MOTOR* this);

	void 		(*set_pwm)(MOTOR* this, uint8_t pwm);
	void 		(*set_freq)(MOTOR* this, uint8_t freq_khz);
	int16_t		(*get_enc)(MOTOR* this);
} mot;

/*****************************    Constructs   *****************************/

struct MOTOR
{
	// public
	SPI*		spi_module;
	SPI_ADDR 	mot_addr;
	SPI_ADDR 	enc_addr;

	uint8_t		freq_khz;
	uint8_t		pwm;
	int16_t 	enc;
};

/****************************** End Of Module ******************************/
