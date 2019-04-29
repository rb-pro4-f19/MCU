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
static void SAMPLER_sample(void* variable, SAMPLE_TYPE type, uint32_t dur_ms);
static void 		SAMPLER_export(void);
static inline void	SAMPLER_operate(void);

/****************************   Class Struct   *****************************/

struct SAMPLER_CLASS sampler =
{
	.uart_module		= NULL,
	.is_sampling		= false,
	.max_size			= 0,

	.tp_sampler 		= NULL,
	.target_var 		= NULL,
	.target_dur_ms 		= 0,
	.target_type        = ST_FLOATING,

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
}

static void SAMPLER_sample(void* variable, SAMPLE_TYPE type, uint32_t dur_ms)
{
	// set target variables
	sampler.target_var = variable;
	sampler.target_dur_ms = dur_ms;
	sampler.target_type = type;

	// reset sampler
	sampler.reset();

	// reset timepoint
	tp.set(sampler.tp_sampler, tp.now());

	// commence sampling
	sampler.is_sampling = true;
}

static void SAMPLER_export(void)
{
	// disable sampling
	sampler.is_sampling = false;

	// enum for header frames
	enum UART_SAMPLEDATA_CMD
	{
		USDC_RESET,
		USDC_DATAPOINT,
		USDC_EOT,
	};

	// a "header frame" specifies how the CLI should handle
	// the incoming data.

	// 0x00 = reset
	// 0x01 = datapoint
	// 0x02 = end of transmission

	// send UART_SAMPLEDATA frame of size 2 w/ USDC_RESET to reset CLI handler
	uart.send(sampler.uart_module, UART_SAMPLEDATA, (uint8_t[2]){USDC_RESET}, 2);

	// iterate all sample datapoints and transmit them individually
	for (int i = 0; i < sampler.data.num_samples; i++)
	{
		// send UART_SAMPLEDATA frame of size 2 w/ USDC_DATAPOINT to prepare CLI handler
		uart.send(sampler.uart_module, UART_SAMPLEDATA, (uint8_t[2]){USDC_DATAPOINT}, 2);

		// transmit the actual data bytes
		uart.send_obj(sampler.uart_module, UART_SAMPLEDATA, &(sampler.data.samples[i]), sizeof(SAMPLE_DATAPOINT));
	}

	// send UART_SAMPLEDATA frame of size 2 w/ USDC_EOT to end transmission
	uart.send(sampler.uart_module, UART_SAMPLEDATA, (uint8_t[2]){USDC_EOT}, 2);
}

static inline void SAMPLER_operate(void)
{
	// static variables
	static float 	value;
	static uint32_t index;

	// check if sampling is required
	if(!sampler.is_sampling) { return; }

	// check if more space available
	if (sampler.data.num_samples >= sampler.max_size) { sampler.export(); return; }

	// check if sampling time is overrun
	if (tp.delta_now(sampler.tp_sampler, ms) > sampler.target_dur_ms) { sampler.export(); return; }

	// read and parse value of target_var (void*) correctly
	if 		(sampler.target_type == ST_FLOATING) { value = *((float*)(sampler.target_var)); }
	else if (sampler.target_type == ST_INTEGER)	 { value = *((int*)(sampler.target_var));   }

	// add sample from taret variable w/ system time
	index = sampler.data.num_samples++;

	sampler.data.samples[index].time	= index;
	sampler.data.samples[index].value	= (float)value;
}

/****************************** End Of Module ******************************/
