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

#include "spi_op.h"
#include "chksum.h"
#include "tp.h"
#include "cli.h"

/*****************************    Defines    *******************************/

#define SSI_CR0_DSS_16 	0xF
#define SSI0_CR0_SCR	8
#define SSI0_CR0_SPH	7
#define SSI0_CR0_SPO	6
#define SSI0_CR0_FRF	4
#define SSI0_CR0_DSS	0

#define BSY				4
#define RFF				3
#define RNE				2
#define TFE				0

#define PORTD			3

#define PD0				0
#define PD1				1
#define PD2				2
#define PD3				3

#define PD0MUX			0
#define PD1MUX			4
#define PD2MUX			8
#define PD3MUX			12

#define FRAME_EMPTY		(SPI_FRAME){0, 0, 0}
#define FRAME_ERROR		(SPI_FRAME){1, 0, 0}
#define FRAME_DATA(x)	(((x.addr << 8) | x.data) & 0x0FFF)

/*****************************   Constants   *******************************/

#define SYSCLK 			16000000

#define RX_MAX_ATTEMPTS	3
#define RX_TIMEOUT_MS	50

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static SPI*				SPI_new(uint8_t clkdiv);
static void 			SPI_del(SPI* this);

static bool 			SPI_send(SPI* this, SPI_ADDR addr, uint8_t data);
static bool		 		SPI_request(SPI* this, SPI_ADDR addr, uint16_t* buffer);
static inline void		SPI_flush(SPI* this);

static void 			_SPI_init(uint8_t clkdiv);
static inline void		_SPI_transmit(SPI_FRAME* frame, bool spinlock);
static inline SPI_FRAME	_SPI_recieve(SPI* this);

/****************************   Class Struct   *****************************/

const struct SPI_CLASS spi =
{
	.new			= &SPI_new,
	.del			= &SPI_del,

	.send			= &SPI_send,
	.request		= &SPI_request,
	.flush 			= &SPI_flush
};

/***********************   Constructive Functions   ************************/

static SPI* SPI_new(uint8_t clkdiv)
{
	// allocate memory
	SPI* this = malloc(sizeof(SPI));

	// initialize variables
	this->clkdiv 		= clkdiv;
	this->tp_timeout	= tp.new();

	_SPI_init(clkdiv);	// Initiate SSI0 module

	// return pointer to instance
	return this;
}

static void SPI_del(SPI* this)
{
	tp.del(this->tp_timeout);
	free(this);
}

/*****************************   Functions   *******************************/

static void _SPI_init(uint8_t clkdiv)
{
	//// configure GPIO

	// enable SSI module
	SYSCTL_RCGCSSI_R 	|= SYSCTL_RCGCSSI_R0;

	// enable clock to GPIO Port D
	SYSCTL_RCGCGPIO_R 	|= (1 << 0);

	// enable alternative function for the SSI0 pins of PortA PA'x' (to be controlled by a peripheral)
	GPIO_PORTA_AFSEL_R 	|= (1 << 2) | (1 <<  3) | (1 <<  4) | (1 <<  5);

	// configure port-mux-control to SSI
	GPIO_PORTA_PCTL_R 	|= (2 << 8) | (2 << 12) | (2 << 16) | (2 << 20);

	// enable the pin's digital function
	GPIO_PORTA_DEN_R 	|= (1 << 2) | (1 <<  3) | (1 <<  4) | (1 <<  5);

	//// configure SPI module

	// disable SPI
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

	// enable SPI
	SSI0_CR1_R 			|= (1 << 1);
}

static bool SPI_send(SPI* this, SPI_ADDR addr, uint8_t data)
{
	// construct frame & generate checksum
	// checksum is generated from 12 bits (addr & data), e.g. 0000|addr|dataxxxx
	SPI_FRAME frm_send = { addr, data, 0 };
	frm_send.chksum = chksum.gen_4bit(FRAME_DATA(frm_send), 3);

	// transmit frame and spinlock
	_SPI_transmit(&frm_send, true);

	// flush buffer
	spi.flush(this);

	// try receiving acknowledge
	SPI_FRAME frm_recived = _SPI_recieve(this);

	// validate checksum
	if (!chksum.val_4bit(FRAME_DATA(frm_recived), 3, frm_recived.chksum))
	{
		return false;
	}

	// everything okay
	return true;
}

static bool SPI_request(SPI* this, SPI_ADDR addr, uint16_t* buffer)
{
	// construct frame and generate checksum
	SPI_FRAME frm_request = { addr, 0, 0 };
	frm_request.chksum = chksum.gen_4bit(FRAME_DATA(frm_request), 3);

	// send request frame
	_SPI_transmit(&frm_request, true);

	// flush buffer
	spi.flush(this);

	// poll for response frame
	SPI_FRAME frm_response = _SPI_recieve(this);

	// validate recieved frame and return 12 bit data
	if(chksum.val_4bit(FRAME_DATA(frm_response), 3, frm_response.chksum))
	{
		*buffer = FRAME_DATA(frm_response);
		return true;
	}
	else
	{
		// force error since no valid data was recieved
		return false;
	}
}

static inline void SPI_flush(SPI* this)
{
	for (volatile uint32_t tmp = 0; SSI0_SR_R & (1 << RNE); tmp = SSI0_DR_R);
}

/*************************   Private Functions   ***************************/

static inline void _SPI_transmit(SPI_FRAME* frame, bool spinlock)
{
	// send data to register
	SSI0_DR_R = ((frame->addr << 12) | (frame->data << 4) | (frame->chksum << 0));

	// if spinlock enabled, wait until SSI BSY flag is cleared
	while (spinlock && (SSI0_SR_R & (1 << BSY)));
}

static inline SPI_FRAME _SPI_recieve(SPI* this)
{
	// construct empty frame
	static SPI_FRAME frm_empty = FRAME_EMPTY;
	uint32_t timeout = 0;

	// transmit empty frame without spinlocking
	_SPI_transmit(&frm_empty, false);

	// wait until SSI not busy or timed out
	while ((SSI0_SR_R & (1 << BSY)) && ((++timeout) < 100000000));

	// read from register
	volatile uint16_t rx_data = (uint16_t)SSI0_DR_R;

	// construct and return frame
	return (SPI_FRAME)
	{
		(rx_data >> 12),
		(rx_data >> 4 & 0x00FF),
		(rx_data & 0x000F)
	};
}

/****************************** End Of Module ******************************/
