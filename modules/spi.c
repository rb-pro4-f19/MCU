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

/*****************************   Constants   *******************************/
#define SYSCLK 16000000
#define SSI_CR0_DSS_16 0xF
#define SSI0_CR0_SCR 8
#define SSI0_CR0_SPH 7
#define SSI0_CR0_SPO 6
#define SSI0_CR0_FRF 4
#define SSI0_CR0_DSS 0
#define LAP_ERROR 3
#define TIMEOUT_RX_ms 50
/*****************************   Variables   *******************************/
// extern uint16_t ticks;

/************************  Function Declarations ***************************/

static SPI*			SPI_new(uint8_t clkdiv);
static void 		SPI_del(SPI* this);

static void 		SPI_send(SPI* this, SPI_ADDR addr, uint8_t data);
static uint16_t 	SPI_request(SPI* this, SPI_ADDR addr, uint8_t size);

static void 		_SPI_init(uint8_t clkdiv);
static void 		_SPI_transmit(SPI_FRAME* frame, bool block);
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
{			// SSI0clk PA2, SSIoFSS PA3, SSI0Rx PA4, SSI0TX PA5;

	SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R0;					// enables SSI module 0
	SYSCTL_RCGCGPIO_R |=  (1<<0); 							// Enable and provide a clock to GPIO Port A in Run mode.
	GPIO_PORTA_AFSEL_R |= (1<<2)|(1<<3)|(1<<4)|(1<<5);		// PortA PA'x' controlled by peripheral signals
	GPIO_PORTA_PCTL_R |= (2<<8)|(2<<12)|(2<<16)|(2<<20);	// Config portmux control to SSI module 0
	GPIO_PORTA_DEN_R |= (1<<2)|(1<<3)|(1<<4)|(1<<5);		// enable pinA2-A3-A4 and pinA5

	SSI0_CR1_R &= ~(1<<1);		// disable SPI0
	SSI0_CR1_R = 0x00000000;	// SPi Master
	SSI0_CC_R |= 0x0;			// clk source ->sysclk;
	SSI0_CPSR_R = clkdiv;		// sysclk/ clkdiv

	SSI0_CR0_R = (0<<SSI0_CR0_SCR) | (0<<SSI0_CR0_SPH) | (0<<SSI0_CR0_SPO) | (SSI_CR0_DSS_16<<SSI0_CR0_DSS);

	SSI0_CR1_R |= (1<<1);		// Enable SPI0
}

static void SPI_send(SPI* this, SPI_ADDR addr, uint8_t data)
/****************************************************************************
*   Input    : this = pointer to a SPI instance
*   Function : Send a frame and will wait for a acknolendge or not
****************************************************************************/
{
	SPI_FRAME frame = {addr, data, 0};
	frame.chksum = chksum.generate(&frame);
	_SPI_transmit(&frame, true);		// Will wait until FIFO transmit is empty
}

static uint16_t SPI_request(SPI* this, SPI_ADDR addr, uint8_t size)
/****************************************************************************
*	Input    : frame sending
*   Function :
****************************************************************************/
{
	uint8_t	laps = 0;
	while(laps < LAP_ERROR)
	{
		SPI_FRAME frame_tx = { addr, size, 0};						// is size redundant? 
		frame_tx.chksum = chksum.generate(&frame_tx);
		_SPI_transmit(&frame_tx,true);								// send request
		SPI_FRAME frame_null = { 0, 0, 0 };
		_SPI_transmit(&frame_null,false);
		SPI_FRAME frame_rx = _SPI_recieve();
		if(chksum.validate(&frame_rx))
		{
			uint16_t data_recived = ((frame_rx.data << 0) | (frame_rx.addr << 8));
			return data_recived;							// recive requested data
		}
		laps++;
	}
}
/****************' Private Functions ***************************************/

static void _SPI_transmit(SPI_FRAME* frame, bool block)
/****************************************************************************
*	Input    : frame transmitting
*   Function :
****************************************************************************/
{

	SSI0_DR_R = (frame->addr<<12) | (frame->data<<4) |(frame->chksum<<0);
	while(block && ((SSI0_SR_R & (1<<0)) == 0));
}

static SPI_FRAME _SPI_recieve(void)
{
    TIMEPOINT* tp_begin = tp.new();
    tp.set(tp_begin,tp.now());
	uint16_t recivedframe;
	while((SSI0_SR_R & (1<<3)) == 0 && tp.delta_now(tp_begin,ms) < TIMEOUT_RX_ms);       //  recive FIFO full = 1
	if((SSI0_SR_R & (1<<3)) == 0)
	{
        recivedframe = (uint16_t) SSI0_DR_R;
        SPI_FRAME datafrom_DR_R = {(recivedframe >> 12), ((recivedframe >> 4)& 0x00FF), (recivedframe & 0x000F)};
        return datafrom_DR_R;
	}
	else
	{
        SPI_FRAME frame_error = { 1, 0, 0};
        return frame_error;
	}
}

/****************************** End Of Module ******************************/
