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
#define laps_initval 0
#define laperror 3
#define runout 3
/*****************************   Variables   *******************************/
// extern uint16_t ticks;
int8_t laps = laps_initval;

/************************  Function Declarations ***************************/

static SPI*			SPI_new(uint8_t clkdiv, uint16_t timeout_ms);// done
static void 		SPI_del(SPI* this);	// done

static void 		SPI_send(SPI* this, SPI_ADDR addr, uint8_t data);
static SPI_FRAME* 	SPI_request(SPI* this, SPI_ADDR addr, uint8_t size);

static void 		_SPI_init(uint8_t clkdiv);
static void 		_SPI_transmit(SPI_FRAME* frame);
static SPI_FRAME*	_SPI_recieve(void);
static uint8_t		_SPI_checksum_generate(SPI_FRAME* frame);
static bool			_SPI_checksum_validate(SPI_FRAME* frame);
void				printf_binary(uint16_t v);
uint8_t				bsd_chksum_4bit(uint16_t frame, int num_of_nibbles);
uint8_t				extract_nibble(uint16_t data, int pos);
uint8_t				ror_nibble(uint8_t data);
uint8_t				rol_nibble(uint8_t data);
uint16_t*			_reverse_bitstream(SPI_FRAME* frame);

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
	this->timeout_ms	= timeout_ms;
/*	disable_global_int(); //Disable global interrupt
	init_systick(); //Initiate systick
	enable_global_int(); //Enable global interrupt*/

	_SPI_init(clkdiv);	// Initiate SSI0 module

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
	SPI_FRAME frame= {addr, data, 0};
	frame.chksum = _SPI_checksum_generate(&frame);
	_SPI_transmit(&frame);		// Will Not wait until FIFO transmit is empty
}

static SPI_FRAME* SPI_request(SPI* this, SPI_ADDR addr, uint8_t size)
/****************************************************************************
*	Input    : frame sending
*   Function :
****************************************************************************/
{
	laps=laps_initval;
	while(laps < laperror)
	{
		SPI_FRAME frame = { addr, size, 0};
		frame.chksum = _SPI_checksum_generate(&frame);
		_SPI_transmit(&frame);								// send request
		while((SSI0_SR_R & (1<<0)) == 0);					// wait until SSI0_DR_R tx is empty
		SPI_FRAME emptyframe = { 0, 0, 0 };
		_SPI_transmit(&emptyframe);
//		ticks = 0;
		while((SSI0_SR_R & (1<<3)) == 0); 		//  recive FIFO full = 1			ticks < runout ||
		if((SSI0_SR_R & (1<<3)) == 0 && _SPI_checksum_validate(_SPI_recieve()))
		{

			return _SPI_recieve();							// recive requested data
		}
		laps++;
	}
}
/****************' Private Functions ***************************************/

static void _SPI_transmit(SPI_FRAME* frame)
/****************************************************************************
*	Input    : frame transmitting
*   Function :
****************************************************************************/
{

	SSI0_DR_R = (frame->addr<<12) | (frame->data<<4) |(frame->chksum<<0);
	//while((SSI0_SR_R & (1<<0)) == 0);
}

static SPI_FRAME* _SPI_recieve(void)
{
	uint32_t data;
	data = SSI0_DR_R;
	SPI_FRAME recivedframe = *(SPI_FRAME *)&data;
	return &recivedframe;
}

static uint8_t _SPI_checksum_generate(SPI_FRAME * frame)
{
    // variables
    uint8_t checksum = 0;

    checksum = bsd_chksum_4bit(frame,4);
	return checksum;
}

static bool _SPI_checksum_validate(SPI_FRAME * frame)
{
	SPI_FRAME testchksum;
	testchksum.addr = frame->addr;
	testchksum.data = frame->data;
    testchksum.chksum =0;
    testchksum.chksum = _SPI_checksum_generate(&testchksum);
	if(testchksum.chksum == frame->chksum)
	{
		return true;
	}
	return false;
}

uint8_t bsd_chksum_4bit(uint16_t data, int num_of_nibbles)
{
	// https://en.wikipedia.org/wiki/BSD_checksum

	// variables
	uint8_t checksum = 0;

	// algorithm starting from Most Signifigant Nibble
	for (int i = num_of_nibbles; i >= 0; --i)
	{
		checksum = ror_nibble(checksum);
		checksum = checksum + extract_nibble(data, i);
		checksum = checksum & 0xF;
	}

	return checksum;
}

void printf_binary(uint16_t v)
{
	unsigned int mask = 1 << ((sizeof(uint16_t) << 3) - 1);

	printf("0x%04x : ", v);

	while (mask) {
		printf("%d", (v&mask ? 1 : 0));
		mask >>= 1;
	}

	printf("\n");
}

uint8_t extract_nibble(uint16_t data, int pos)
{
	// extract nibble at position n
	// test = 0101 1111 1010 0000 -> extract_nibble(test, 2) = 1111

	return ((data & (0xF << pos * 4)) >> (pos * 4));
}

uint8_t rol_nibble(uint8_t data)
{
	return ((data << 1) | (data >> 3) & 0xF);
}

uint8_t ror_nibble(uint8_t data)
{
	return ((data >> 1) | (data << 3) & 0xF);
}

/*
uint16_t* _reverse_bitstream(uint16_t frame)			//
{
	uint16_t bitframe = (frame->addr<<12) | (frame->data<<4) |(frame->chksum<<0);

	uint16_t temp;
	for (int i = 0; i <= 15; i++)
	{
		if ((bitframe >> i) & 1)
		{
			temp |= (1 << (15 - i));
		};
	}
	return *temp;
}*/

/****************************** End Of Module ******************************/
