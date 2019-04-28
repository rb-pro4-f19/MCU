/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	sampler.h
* MODULENAME.:	SAMPLER
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

#include "uart.h"

/*****************************    Defines    *******************************/

typedef struct  SAMPLE_DATA 		SAMPLE_DATA;
typedef struct  SAMPLE_DATAPOINT	SAMPLE_DATAPOINT;

/***********************     External Variables     ************************/

/*****************************   Constants   *******************************/

/*****************************    Constructs   *****************************/

struct SAMPLE_DATAPOINT
{
	uint32_t	time;
	float 		value;
};

struct SAMPLE_DATA
{
	uint32_t 			init_timestamp;
	uint32_t			num_samples;
	uint32_t			duration_ms;

	SAMPLE_DATAPOINT*	samples;
};

/*************************    Class Functions    ***************************/

extern struct SAMPLER_CLASS
{
	UART* 		uart_module;
	bool 		is_sampling;
	uint32_t 	index;
	size_t	 	max_size;

	SAMPLE_DATA data;
	void* 		target_var;
	uint32_t 	target_dur_ms;
	TIMEPOINT* 	tp_sampler;

	void 		(* const init)(UART* uart_module, uint32_t max_samples);
	void 		(* const reset)(void);
	void 		(* const sample)(void* variable, uint32_t dur_ms);
	void 		(* const export)(void);
	void 		(* const operate)(void);

} sampler;

/****************************** End Of Module ******************************/
