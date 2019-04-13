/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	sys.h
* MODULENAME.:	SYSTEM
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
#include <assert.h>

#include "driver.h"
#include "tp.h"
#include "spi_old.h"
#include "uart.h"
#include "cli.h"
#include "hal.h"
#include "mot.h"

/*****************************    Defines    *******************************/

typedef enum	SYS_MODE SYS_MODE;
typedef enum	CAL_MODE CAL_MODE;

/***********************     External Variables     ************************/

extern SPI*			spi_main;
extern UART*		uart_main;
extern MOTOR*		mot0;
extern MOTOR*		mot1;
extern HAL*			hal0;
extern HAL*			hal1;

/*****************************   Constants   *******************************/

/****************************    Constructs    *****************************/

enum SYS_MODE
{
	SYS_IDLE,
	SYS_CALIBRATION,
	SYS_TUNING,
	SYS_OPERATION
};

enum CAL_MODE
{
	CAL_RESET,

	CAL_PAN_INIT,
	CAL_PAN_SEEK_BOUNDARY,
	CAL_PAN_SEEK_HAL,

	CAL_TILT_INIT,
	CAL_TILT_SEEK_HAL,
	CAL_TILT_FINETUNE,

	CAL_FINISH
};

/*************************    Class Functions    ***************************/

extern struct SYSTEM_CLASS
{
	SYS_MODE	state;

	bool		is_cal;
	bool		is_init;

	TIMEPOINT*	tp_cal;

	void		(* const init)(void);
	void		(* const operate)(void);
	void		(* const echo)(void);

	void 		(* const set_pwm)(SPI_ADDR mot_addr, int8_t pwm);
	void 		(* const set_freq)(SPI_ADDR mot_addr, uint8_t freq_khz);
	void 		(* const set_pos)(uint8_t theta);
	void 		(* const set_enc)(uint8_t ticks);
	void		(* const set_mode)(SYS_MODE mode);

	void 		(* const get_enc)(SPI_ADDR enc_addr);
	void 		(* const get_hal)(SPI_ADDR hal_addr);
} sys;

/****************************** End Of Module ******************************/
