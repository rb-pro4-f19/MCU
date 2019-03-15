/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	uart.c
* MODULENAME.:	UART
*
* For an API and DESCRIPTION, please refer to the  module
* specification file (.h-file).
*
****************************************************************************/

/***************************** Include files *******************************/

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include "uart.h"
#include "chksum.h"
#include "tp.h"

/*****************************    Defines    *******************************/
#define MINIMALFRAMESIZE 3
#define RX_TIMEOUT_MS 50

/*****************************   Constants   *******************************/

#define SYSCLK 			16000000


/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static UART*    UART_new(uint8_t clkdiv);
static void		UART_del(UART* this);

static bool 	UART_send(UART* this, UART_TYPE type, uint8_t *data, uint8_t size);
static bool		UART_read(UART* this, UART_FRAME* frame);

static void		_UART_init();
static bool 	_UART_recieve(uint8_t * byte);
static void		_UART_transmit(uint8_t data);
/****************************   Class Struct   *****************************/

const struct UART_CLASS uart =
{
	.new			= &UART_new,
	.del			= &UART_del,

	.send			= &UART_send,
	.read			= &UART_read
};

/***********************   Constructive Functions   ************************/

static UART* UART_new(uint8_t clkdiv)
/****************************************************************************
*   Input    : type = desired EXM_TYPE for the instance.
			 : init_val = desired value for `var`.
*   Output   : Pointer to a new EXAMPLE instance.
*   Function : Constructor of a EXAMPLE instance.
****************************************************************************/
{
	// allocate memory
	UART* this = malloc(sizeof(UART));

	// initialize variables
	this->clkdiv = clkdiv;
	_UART_init();	// Initiate UART0 module

	// return pointer to instance
	return this;
}

static void UART_del(UART* this)
/****************************************************************************
*   Input    : this = pointer to a EXAMPLE instance.
*   Function : Destructor of a EXAMPLE instance.
****************************************************************************/
{
	free(this);
}

/*****************************   Functions   *******************************/

static void _UART_init()
/****************************************************************************
*   Input    : this = pointer to a EXAMPLE instance.
*   Function : Sets `is_set` of an EXAMPLE instance to false, such that
*			   something interesting may happen. Maybe.
****************************************************************************/
{
//// TX0 = PA0 and RX0 = PA1

	//// configure ports

	// enable clock to GPIO Port A
	SYSCTL_RCGCGPIO_R	|= ( 1 << 0 );

	// enable alternative function for the UART0 pins of PortA PA'x' (to be controlled by a peripheral)
	GPIO_PORTA_AFSEL_R	|= (1 << 0) | (1 << 1);

	// set currentlvl
	GPIO_PORTA_DR2R_R	|= (1 << 0) | (1 << 1); 	// GPIO pin A0 & A1 has 2-mA drive

	// configure port-mux-control to SSI0
	GPIO_PORTA_PCTL_R 	|= (1 << 0) | (1 << 4);

	// enable the pin's digital function
	GPIO_PORTA_DEN_R 	|= ( 1 << 0 ) | ( 1 <<  1 );

	//// UART0-module config

	//Enable UART module 0
	SYSCTL_RCGCUART_R 	|= (1 << 0);	// enable UART0 Clock Gating Control (PA0 & PA1)

	// provide clock to UART0
	SYSCTL_RCGC1_R 		|= SYSCTL_RCGC1_UART0;

	// disable UART
    UART0_CTL_R         = 0;

	// eq: uartclk / (16 * Baudrate) = BRD (Baud rate Divisor)
	// 16 000 000 / (16 * 10 000 000) = 0.1

    UART0_IBRD_R        |= 0;

    // 0.1 * 64 + 0.5 = 7
    UART0_FBRD_R        |= 7;

	//  WLEN IS 8 bit, FIFO mode, 1 stop bit - no parity
	UART0_LCRH_R        =  ( 0x3 << 5) | (1 << 4);

	// use system clock
	UART0_CC_R          |= 0;

	// enable UART, RXE, TXE
	UART0_CTL_R         |= ( 1 << 0 ) | ( 1 << 8 ) | ( 1 << 9 );


    // delay function for 1ms should be here
//    for(int i = 0; i < 10000000; i++);
}

static bool UART_send(UART* this, UART_TYPE type, uint8_t *data, uint8_t size)
/****************************************************************************
*   Input    : this = pointer to a UART instance
*   Function : Send a frame and will wait for a acknolendge or not
****************************************************************************/
{
	uint8_t tx_buffer[258];		// construct array max size (1 byte header + 256 byte data + 1 byte chksum)

	tx_buffer[0] = (type << 5) | (size << 0);		// insert hedaer

	for(int i = 0; i < size + 1; i++)				// insert data / payload
	{
		tx_buffer[i + 1] = data[i];
	}

	tx_buffer[size + 2] = chksum.gen_8bit(tx_buffer, size);	// insert chksum

	for(int i = 0; i < size + 2; i++)
	{
		_UART_transmit(tx_buffer[i]);			// transmit frame
	}
}

static bool UART_read(UART* this, UART_FRAME* frame)
/****************************************************************************
*   Input    : this = pointer to a UART instance
*   Function : recieve a frame and check checksum
****************************************************************************/
{
	static uint8_t	rx_buffer[16];
	uint8_t rx_counter = 0;
	uint8_t* rx_payload;
	bool rx_error = false;
	TIMEPOINT*	tp_timeout 	= tp.new();


	for(int i = 0; i < 16; i++)
	{
		rx_buffer[i] = 0;
	}

	tp.set(tp_timeout, tp.now());

	while(UART0_FR_R & (1<<4) == 0 && tp.delta_now(tp_timeout, ms) < RX_TIMEOUT_MS)
	{
		rx_error = !(_UART_recieve(&rx_buffer[rx_counter++])); // empty rx_FIFO to rx_buffer
	}

	if(rx_counter == 0 || rx_error )
	{
		return false;
	}

	rx_payload = &rx_buffer[1];

	frame->type = rx_buffer[0] & 0b11100000;
	frame->size = rx_buffer[0] & 0b00011111;
	frame->payload = rx_payload;
	frame->chksum = rx_buffer[rx_counter];

	if(chksum.val_8bit(rx_buffer, rx_counter - 1, frame->chksum))
	{
		return true;
	}
}
/*************************   Private Functions   ***************************/

static void _UART_transmit(uint8_t data)
/****************************************************************************
*	Input    : data transmitting
*   Function :
****************************************************************************/
{
	while(UART0_FR_R & (1 << 5) == 1); // while Tx_FIFO is full
	UART0_DR_R = data;
}

static bool _UART_recieve(uint8_t* byte)
/****************************************************************************
*   Function : move recived data to rx_buffer
****************************************************************************/
{
	uint32_t rx_data = UART0_DR_R;
		// test for UART overrun, Break error, Parity Error and Framing Error
		if(rx_data & (1 << 11) || rx_data & (1 << 10) || rx_data & (1 << 9) || rx_data & (1 << 8))
		{
			return false;
		}
		byte = (uint8_t) rx_data;
		return true;
}

/****************************** End Of Module ******************************/
