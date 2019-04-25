/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	mot.h
* MODULENAME.:	MOTOR
* API........:	https://git.io/fjLSM
* VERSION....:	1.2.1
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

extern struct MOTOR_CLASS
{
	SPI* 		spi_module;

	MOTOR*		(* const new)(SPI_ADDR mot_addr, SPI_ADDR enc_addr, uint8_t freq_khz);
	void		(* const del)(MOTOR* this);

	void 		(* const operate)(MOTOR* this);
	void 		(* const feed)(MOTOR* this);

	bool 		(* const set_pwm)(MOTOR* this, int8_t pwm);
	bool 		(* const set_freq)(MOTOR* this, uint8_t freq_khz);
	int16_t		(* const get_enc)(MOTOR* this);
} mot;

/*****************************    Constructs   *****************************/

struct MOTOR
{
	// public
	SPI_ADDR 	mot_addr;
	SPI_ADDR 	enc_addr;

	TIMEPOINT*	tp_watchdog;
	TIMEPOINT*	tp_slewrate;
	TIMEPOINT*	tp_speed;

	uint8_t		freq_khz;
	int8_t		pwm;
	int8_t		pwm_target;
	uint8_t		pwm_data;
	int16_t 	enc;
	float 		speed;

	bool		slew;
	uint16_t	slew_dx;
	uint8_t		slew_dy;

	bool 		bound;
	int16_t 	bound_l;
	int16_t		bound_h;
	uint16_t	bound_max;
};

/****************************** End Of Module ******************************/
