/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	sampler.h
* MODULENAME.:	SAMPLER
* API........:	https://git.io/fjG6L
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

#include "uart.h"

/*****************************    Defines    *******************************/

typedef struct  SAMPLE_DATA 		SAMPLE_DATA;
typedef struct  SAMPLE_DATAPOINT	SAMPLE_DATAPOINT;
typedef enum 	SAMPLE_TYPE			SAMPLE_TYPE;

/***********************     External Variables     ************************/

/*****************************   Constants   *******************************/

/*****************************    Constructs   *****************************/

struct SAMPLE_DATAPOINT
{
	uint16_t	time;
	int16_t		value;
};

struct SAMPLE_DATA
{
	uint32_t 			init_timestamp;
	uint32_t			num_samples;
	uint32_t			duration_ms;

	SAMPLE_DATAPOINT*	samples;
};

enum SAMPLE_TYPE
{
	ST_INTEGER,
	ST_FLOATING,
};

/*************************    Class Functions    ***************************/

extern struct SAMPLER_CLASS
{
	UART* 		uart_module;
	bool 		is_sampling;
	size_t	 	max_size;

	SAMPLE_DATA data;
	void* 		target_var;
	uint32_t 	target_samples;
	SAMPLE_TYPE	target_type;
	TIMEPOINT* 	tp_sampler;

	void 		(* const init)(UART* uart_module, uint32_t max_samples);
	void 		(* const reset)(void);
	void 		(* const sample)(void* variable, SAMPLE_TYPE type, uint32_t target_samples);
	void 		(* const export)(void);
	void 		(* const operate)(void);

} sampler;

/****************************** End Of Module ******************************/
