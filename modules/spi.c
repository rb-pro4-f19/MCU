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

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/
#define SYSCLK 16000000
#define SSI_CR0_DSS_16 0xF
#define SSI0_CR0_SCR 8
#define SSI0_CR0_SPH 7
#define SSI0_CR0_SPO 6
#define SSI0_CR0_FRF 4
#define SSI0_CR0_DSS 0
/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static SPI*			SPI_new(uint8_t clkdiv, uint16_t timeout_ms);
static void 		SPI_del(SPI* this);

static void 		SPI_send(SPI* this, SPI_ADDR addr, uint8_t data, bool ack);
static uint16_t 	SPI_request(SPI* this, SPI_ADDR addr, uint8_t size, uint32_t timeout_ms);

static void 		_SPI_init(uint8_t clkdiv);
static void 		_SPI_transmit(SPI_FRAME* frame);
static SPI_FRAME	_SPI_recieve(void);
static uint8_t		_SPI_checksum_generate(SPI_FRAME* frame);
static bool			_SPI_checksum_validate(SPI_FRAME* frame);

/****************************   Class Struct   *****************************/

const struct SPI_CLASS spi =
{
	.new			= &SPI_new,
	.del			= &SPI_del,

	.send			= &SPI_send,
	.request		= &SPI_request
};

/***********************   Constructive Functions   ************************/

static SPI* SPI_new(uint8_t clkdiv, uint16_t timeout_ms)
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
	this->timeout_ms	= 0;
	_SPI_init(clkdiv);
	_SPI_checksum_generate(&((SPI_FRAME){10, 10, 0}));

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


static void SPI_send(SPI* this, SPI_ADDR addr, uint8_t data, bool ack)
/****************************************************************************
*   Input    : this = pointer to a EXAMPLE instance.
*   Function : Sets `is_set` of an EXAMPLE instance to false, such that
			   something interesting may happen. Maybe.
****************************************************************************/
{
	SPI_FRAME frame= {addr, data, 0};
	frame.chksum = _SPI_checksum_generate(&frame);
	_SPI_transmit(&frame);// skal skrives
}

static void _SPI_init(uint8_t clkdiv)
/****************************************************************************
*   Input    : this = pointer to a EXAMPLE instance.
*   Function : Sets `is_set` of an EXAMPLE instance to false, such that
			   something interesting may happen. Maybe.
****************************************************************************/
{			// SSI0clk PA2, SSIoFSS PA3, SSI0Rx PA4, SSI0TX PA5;

	SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R0; // enables SSI module 0
	SYSCTL_RCGCGPIO_R |=  (1<<0); 			// Enable and provide a clock to GPIO Port A in Run mode.
	GPIO_PORTA_AFSEL_R |= (1<<2)|(1<<3)|(1<<4)|(1<<5);	// PortA PA'x' controlled by peripheral signals
	GPIO_PORTA_PCTL_R |= (2<<8)|(2<<12)|(2<<16)|(2<<20); // Config portmux control to SSI module 0
	GPIO_PORTA_DEN_R |= (1<<2)|(1<<3)|(1<<4)|(1<<5);

	SSI0_CR1_R &= ~(1<<1);		// disable SPI0
	SSI0_CR1_R = 0x00000000;		// SPi Master
	SSI0_CC_R |= 0x0;			// clk source ->sysclk;
	SSI0_CPSR_R = clkdiv;		// sysclk/ clkdiv

	SSI0_CR0_R = (0<<SSI0_CR0_SCR) | (0<<SSI0_CR0_SPH) | (0<<SSI0_CR0_SPO) | (SSI_CR0_DSS_16<<SSI0_CR0_DSS);

	SSI0_CR1_R |= (1<<1);		// Enable SPI0
}
static void _SPI_transmit(SPI_FRAME* frame)
/****************************************************************************
*	Input    : frame sending
*   Function :
****************************************************************************/
{
		SSI0_DR_R = (frame->addr<<12) | (frame->data<<4) |(frame->chksum<<0);
	while((SSI0_SR_R & (1<<0)) == 0);
}
static uint16_t SPI_request(SPI * this, SPI_ADDR addr, uint8_t size, uint32_t timeout_ms)
{
	return 0;
}
static SPI_FRAME _SPI_recieve(void)
{
	return (SPI_FRAME){ 0,0,0 };
}
static uint8_t _SPI_checksum_generate(SPI_FRAME * frame)
{
	return 0;
}
static bool _SPI_checksum_validate(SPI_FRAME * frame)
{
	return false;
}
/****************************** End Of Module ******************************/
