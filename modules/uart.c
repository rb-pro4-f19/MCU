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

#include "UART.h"
#include "chksum.h"
#include "tp.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

#define SYSCLK 			16000000


/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static UART*    (*new)(uint8_t clkdiv);
static void		(*del)(UART* this);

static bool 	(*send)(UART* this, UART_TYPE addr, uint8_t *data);
static uint16_t	(*recieve)(UART* this, UART_TYPE addr);

/****************************   Class Struct   *****************************/

const struct UART_CLASS UART =
{
	.new			= &UART_new,
	.del			= &UART_del,

	.send			= &UART_send,
	.recieve		= &UART_request
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

	_UART_init(clkdiv);	// Initiate SSI0 module

	// return pointer to instance
	return this;
}

static void UART_del(* this)
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
			   something interesting may happen. Maybe.
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
    for(int i = 0; i < 10000000; i++);
}

static bool UART_send(UART* this, UART_TYPE addr, uint8_t *data, uint8_t size)
/****************************************************************************
*   Input    : this = pointer to a UART instance
*   Function : Send a frame and will wait for a acknolendge or not
****************************************************************************/
{
}

/*************************   Private Functions   ***************************/

static void _UART_transmit(UART_FRAME* frame, bool UARTnlock)
/****************************************************************************
*	Input    : frame transmitting
*   Function :
****************************************************************************/
{
}

static UART_FRAME _UART_recieve(void)
{

}

/****************************** End Of Module ******************************/
