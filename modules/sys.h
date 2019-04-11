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
typedef enum	CALI_MODES CALI_MODES;

/***********************     External Variables     ************************/

extern SPI*			spi_main;
extern UART*		uart_main;
extern MOTOR*		mot0;
extern MOTOR*		mot1;
extern HAL*		hal0;
extern HAL*		hal1;

extern CALI_MODES	cur_cali;

/*****************************   Constants   *******************************/

/****************************    Constructs    *****************************/

enum SYS_MODE
{
	IDLE,
	CALIBRATION,
	TUNING,
	OPERATION
};

enum CALI_MODES
{
	CALI_TURN_OFF,
    CALI_GOMAX_MOT1,
    CALI_STOP_MOT1,
    CALI_MOT1,
    CALI_STOP_MOT0
};

/*************************    Class Functions    ***************************/

extern struct SYSTEM_CLASS
{
	SYS_MODE	state;

	bool		is_cal;
	bool		is_init;

	TIMEPOINT*	tp_sys;

	void		(* const init)(void);
	void		(* const operate)(void);
	void		(* const echo)(void);

	void		(* const set_pos)(uint8_t theta);
	void		(* const set_enc)(uint8_t ticks);
	void		(* const set_mode)(SYS_MODE mode);
} sys;

/****************************** End Of Module ******************************/
