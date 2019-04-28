/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	sampler.c
* MODULENAME.:	SAMPLER
*
* For an API and DESCRIPTION, please refer to the  module
* specification file (.h-file).
*
****************************************************************************/

/***************************** Include files *******************************/

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>

#include "sampler.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static void 		SAMPLER_init(UART* uart_module, uint32_t max_samples);
static void 		SAMPLER_reset(void);
static void 		SAMPLER_sample(void* variable, uint32_t dur_ms);
static void 		SAMPLER_export(void);
static inline void	SAMPLER_operate(void);

/****************************   Class Struct   *****************************/

struct SAMPLER_CLASS sampler =
{
	.uart_module		= NULL,
	.is_sampling		= false,
	.index				= 0,
	.max_size			= 0,

	.tp_sampler 		= NULL,
	.target_var 		= NULL,
	.target_dur_ms 		= 0,

	.init				= &SAMPLER_init,
	.reset				= &SAMPLER_reset,
	.sample				= &SAMPLER_sample,
	.export				= &SAMPLER_export,
	.operate			= &SAMPLER_operate,
};

/*****************************   Functions   *******************************/

static void SAMPLER_init(UART* uart_module, uint32_t max_samples)
{
	// UART module must exist
	assert(uart_module != NULL);

	// allocate memory for samples
	sampler.data.samples = malloc(sizeof(SAMPLE_DATAPOINT) * max_samples);

	// initialize variables
	sampler.uart_module = uart_module;
	sampler.max_size = max_samples;

	// create timepoint
	sampler.tp_sampler = tp.new();

	// reset sampler variables
	sampler.reset();
}

static void SAMPLER_reset(void)
{
	// data memory must be allocated
	assert(sampler.data.samples != NULL);

	// disable sampler (safety)
	sampler.is_sampling = false;

	// reset data samples memory to 0
	memset(sampler.data.samples, 0, sizeof(SAMPLE_DATAPOINT) * sampler.max_size);

	// reset data variables
	sampler.data.init_timestamp	= 0;
	sampler.data.num_samples	= 0;
	sampler.data.duration_ms	= 0;

	// reset variables
	sampler.index = 0;
	sampler.is_sampling = false;
}

static void SAMPLER_sample(void* variable, uint32_t dur_ms)
{
	// set target variables
	sampler.target_var = variable;
	sampler.target_dur_ms = dur_ms;

	// reset sampler
	sampler.reset();

	// reset timepoint
	tp.set(sampler.tp_sampler, tp.now());

	// get systime
	sampler.data.init_timestamp = 0;

	// commence sampling
	sampler.is_sampling = true;
}

static void SAMPLER_export(void)
{
	return;
}

static inline void SAMPLER_operate(void)
{
	// check if sampling is required
	if(!sampler.is_sampling) { return; }

	// check if more space available
	if (sampler.data.num_samples >= sampler.max_size) { goto end_sampling; }

	// add sample from taret variable w/ system time
	uint32_t index = sampler.data.num_samples++;

	sampler.data.samples[index].time	= index;
	sampler.data.samples[index].value	= *((float*)(sampler.target_var));

	// check if sampling time is overrun
	if (tp.delta_now(sampler.tp_sampler, ms) > sampler.target_dur_ms) { goto end_sampling; }

end_sampling:
	sampler.is_sampling = false;
	sampler.export();

}

/****************************** End Of Module ******************************/
