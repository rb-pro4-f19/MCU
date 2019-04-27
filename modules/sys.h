/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	sys.h
* MODULENAME.:	SYSTEM
* API........:	https://git.io/fjsVH
* VERSION....:	1.4.0
*
* DESCRIPTION:	An example module. This might have a lengthy description, in
*				which case, we simply add some tabs.
*
****************************************************************************/

#pragma once

/***************************** Include files *******************************/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

#include "driver.h"
#include "tp.h"
#include "spi_old.h"
#include "uart.h"
#include "cli.h"
#include "hal.h"
#include "mot.h"
#include "pid.h"

/*****************************    Defines    *******************************/

typedef enum	SYS_MODE SYS_MODE;
typedef enum	CAL_MODE CAL_MODE;
typedef enum    PID_PARAM PID_PARAM;
typedef struct	MOT_DATA MOT_DATA;
typedef struct	GUI_DATA GUI_DATA;

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
	SYS_IDLE			= 0,
	SYS_CALIBRATION		= 1,
	SYS_TUNING			= 2,
	SYS_OPERATION		= 3
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

struct MOT_DATA
{
	int8_t		pwm;
	uint8_t		freq;
	int16_t		enc;
	float		spd;
	uint8_t		hal;

	uint8_t		pid_i;
	uint8_t		pid_n;
	float		pid_kp;
	float		pid_ki;
	float		pid_kd;
};

struct GUI_DATA
{
	uint8_t		mode;

	MOT_DATA	mot0;
	MOT_DATA	mot1;
};

/*************************    Class Functions    ***************************/

extern struct SYSTEM_CLASS
{
	SYS_MODE	mode;

	bool		cal_done;
	bool		init_done;
	bool		to_gui;

	TIMEPOINT*	tp_cal;
	TIMEPOINT*	tp_gui;
	TIMEPOINT*	tp_tst;

	GUI_DATA 	gui_data;
	uint16_t 	op_time;

	void		(* const init)(void);
	void		(* const operate)(void);
	void		(* const echo)(void);

	void		(* const set_mode)(SYS_MODE mode);
	void 		(* const set_pos)(SPI_ADDR mot_addr, uint8_t* flt_array);
	void		(* const set_gui)(bool option);
	void		(* const set_msg)(bool option);

	void 		(* const set_pwm)(SPI_ADDR mot_addr, int8_t pwm);
	void 		(* const set_freq)(SPI_ADDR mot_addr, uint8_t freq_khz);
	void		(* const set_slew)(bool option);
	void		(* const set_bound)(bool option);

	void 		(* const set_pid)(SPI_ADDR mot_addr, PID_PARAM param, uint8_t* flt_array);

	void 		(* const get_enc)(SPI_ADDR enc_addr);
	void 		(* const get_hal)(SPI_ADDR hal_addr);
} sys;

/****************************** End Of Module ******************************/
