/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	spi.c
* MODULENAME.:	SPI
*
* For an API and DESCRIPTION, please refer to the  module
* specification file (.h-file).
*
****************************************************************************/

/***************************** Include files *******************************/

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include "spi.h"
#include "chksum.h"
#include "tp.h"

/*****************************    Defines    *******************************/

#define SSI_CR0_DSS_16 	0xF
#define SSI0_CR0_SCR	8
#define SSI0_CR0_SPH	7
#define SSI0_CR0_SPO	6
#define SSI0_CR0_FRF	4
#define SSI0_CR0_DSS	0

#define FRAME_EMPTY		(SPI_FRAME){0, 0, 0}
#define FRAME_ERROR		(SPI_FRAME){1, 0, 0}
#define FRAME_DATA(x)	(((x.addr << 8) | x.data) & 0x0FFF)

/*****************************   Constants   *******************************/

#define SYSCLK 			16000000

#define RX_MAX_ATTEMPTS	3
#define RX_TIMEOUT_MS	50

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static SPI*			SPI_new(uint8_t clkdiv);
static void 		SPI_del(SPI* this);

static bool 		SPI_send(SPI* this, SPI_ADDR addr, uint8_t data);
static bool		 	SPI_request(SPI* this, SPI_ADDR addr, uint16_t* buffer);

static void 		_SPI_init(uint8_t clkdiv);
static void 		_SPI_transmit(SPI_FRAME* frame, bool spinlock);
static SPI_FRAME	_SPI_recieve(void);

/****************************   Class Struct   *****************************/

const struct SPI_CLASS spi =
{
	.new			= &SPI_new,
	.del			= &SPI_del,

	.send			= &SPI_send,
	.request		= &SPI_request
};

/***********************   Constructive Functions   ************************/

static SPI* SPI_new(uint8_t clkdiv)
/****************************************************************************
*   Input    : type = desired EXM_TYPE for the instance.
			 : init_val = desired value for `var`.
*   Output   : Pointer to a new EXAMPLE instance.
*   Function : Constructor of a EXAMPLE instance.
****************************************************************************/
{
	// allocate memory
	SPI* this = malloc(sizeof(SPI));

	// initialize variables
	this->clkdiv 		= clkdiv;

	_SPI_init(clkdiv);	// Initiate SSI0 module

	// return pointer to instance
	return this;
}

static void SPI_del(SPI* this)
/****************************************************************************
*   Input    : this = pointer to a EXAMPLE instance.
*   Function : Destructor of a EXAMPLE instance.
****************************************************************************/
{
	free(this);
}

/*****************************   Functions   *******************************/

static void _SPI_init(uint8_t clkdiv)
/****************************************************************************
*   Input    : this = pointer to a EXAMPLE instance.
*   Function : Sets `is_set` of an EXAMPLE instance to false, such that
			   something interesting may happen. Maybe.
****************************************************************************/
{

	//// configure ports
	//// SSI0clk = PA2, SSIoFSS = PA3, SSI0Rx = PA4, SSI0TX = PA5

	// enable SSI0 module
	SYSCTL_RCGCSSI_R 	|= SYSCTL_RCGCSSI_R0;

	// enable clock to GPIO Port A
	SYSCTL_RCGCGPIO_R 	|= (1 << 0);

	// enable alternative function for the SSI0 pins of PortA PA'x' (to be controlled by a peripheral)
	GPIO_PORTA_AFSEL_R 	|= (1 << 2) | (1 <<  3) | (1 <<  4) | (1 <<  5);

	// configure port-mux-control to SSI0
	GPIO_PORTA_PCTL_R 	|= (2 << 8) | (2 << 12) | (2 << 16) | (2 << 20);

	// enable the pin's digital function
	GPIO_PORTA_DEN_R 	|= (1 << 2) | (1 <<  3) | (1 <<  4) | (1 <<  5);

	//// configure SPI module

	// disable SPI0
	SSI0_CR1_R 			&= ~(1 << 1);

	// set SPI modules to master
	SSI0_CR1_R 			= 0x00000000;

	// set CLK source to SYSCLK
	SSI0_CC_R 			|= 0x0;

	// set clock divider, keep SCR = 0
	// bps = SYSCLK / (CPSDVSR * (1 + SCR))
	SSI0_CPSR_R 		= clkdiv;

	// set SPI mode and frame size to 16 bit
	SSI0_CR0_R 			= (0 << SSI0_CR0_SCR) | (0 << SSI0_CR0_SPH) | (0 << SSI0_CR0_SPO) | (SSI_CR0_DSS_16 << SSI0_CR0_DSS);

	// enable SPI0
	SSI0_CR1_R 			|= (1 << 1);
}

static bool SPI_send(SPI* this, SPI_ADDR addr, uint8_t data)
/****************************************************************************
*   Input    : this = pointer to a SPI instance
*   Function : Send a frame and will wait for a acknolendge or not
****************************************************************************/
{
	// construct frame & generate checksum
	// checksum is generated from 12 bits (addr & data), e.g. 0000|addr|dataxxxx
	SPI_FRAME frm_send = { addr, data, 0 };
	//frm_send.chksum = chksum.gen_4bit(((frm_send.addr << 8) | frm_send.data) & 0x0FFF, 3);
	frm_send.chksum = chksum.gen_4bit(FRAME_DATA(frm_send), 3);

	// transmit frame
	_SPI_transmit(&frm_send, true);

	// try receiving acknowledge
	SPI_FRAME frm_recived = _SPI_recieve();

	// validate checksum
	if (!chksum.val_4bit(FRAME_DATA(frm_send), 3, frm_recived.chksum))
	{
		return false;
	}

	// everything okay
	return true;
}

static bool SPI_request(SPI* this, SPI_ADDR addr, uint16_t* buffer)
/****************************************************************************
*	Input    : frame sending
*   Function :
****************************************************************************/
{
	// construct frame and generate checksum
	SPI_FRAME frm_request	= { addr, 0, 0 };
	frm_request.chksum = chksum.gen_4bit(FRAME_DATA(frm_request), 3);

	// attempt to send request and get response
	for (int i = 0; i < RX_MAX_ATTEMPTS; i++)
	{
		// send request frame
		_SPI_transmit(&frm_request, true);

		// poll for response frame
		SPI_FRAME frm_response = _SPI_recieve();

		// validate recieved frame and return 12 bit data
		if(chksum.val_4bit(FRAME_DATA(frm_request), 3, frm_response.chksum))
		{
			*buffer = FRAME_DATA(frm_request);
			return true;
		}
	}

	// force error, since no data was recieved
	//assert(0);
	return false;

	// maybe return data via buffer* as argument and return bool on
	// whether function succeeded or not?
}

/*************************   Private Functions   ***************************/

static void _SPI_transmit(SPI_FRAME* frame, bool spinlock)
/****************************************************************************
*	Input    : frame transmitting
*   Function :
****************************************************************************/
{
	SSI0_DR_R = (frame->addr << 12) | (frame->data << 4) | (frame->chksum << 0);
	while(spinlock && ((SSI0_SR_R & (1 << 0)) == 0));
}

static SPI_FRAME _SPI_recieve(void)
{
	TIMEPOINT*	tp_timeout 	= tp.new();

	// transmit empty frame and start timeout timer
	_SPI_transmit(&FRAME_EMPTY, false);
	tp.set(tp_timeout, tp.now());

	// wait until FIFO register full or timedout passed
	while((SSI0_SR_R & (1 << 3)) == 0 && tp.delta_now(tp_timeout, ms) < RX_TIMEOUT_MS);

	// cleanup TIMEPOINT
	// should be moved to be part of struct
	tp.del(tp_timeout);

	// return frame from MISO register if any data
	// HUGE BUG!!!!!
	// !!!!! ... chehcking for 0??
	if((SSI0_SR_R & (1 << 3)) == 0)
	{
		volatile uint32_t rx_data;
		rx_data = SSI0_DR_R;

		return (SPI_FRAME){
			(rx_data >> 12),
			(rx_data >> 4 & 0x00FF),
			(rx_data & 0x000F)
		};
	}
	// return error frame
	else
	{
		return FRAME_ERROR;
	}
}

/****************************** End Of Module ******************************/
