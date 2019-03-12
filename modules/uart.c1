/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	UART.c
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

    //// configure UART module

    // provide clock to UART0
	SYSCTL_RCGCSSI_R 	|= SYSCTL_RCGCUART_R;

	// enable clock to GPIO Port A
	SYSCTL_RCGCGPIO_R 	|= ( 1 << 0 );

    // disable UART
    UART0_CTL_R         |= 0;

    // FORMULA 16Mhz / (16 ) = 1 MHz -> 1Mhz / clkdiv = Desired Baudrate
    // 16.000.000/(16*115200) = 8.680
    UART0_IBRD_R        |= 8;

    // 0.680 * 64 + 0.5 = 44
    UART0_FBRD_R        |= 44;

    // use system clock
	UART0_CC_R          |= 0;

	// 8-bit (WLEN IS 5 & 6 bit), no parity, 1 stop bit - no FIFO buffer
	UART0_LCRH_R        |= ( 1 << 5) | (1 << 6);

	// enable UART, RXE, TXE
	UART0_CTL_R         |= ( 1 << 0 ) | ( 1 << 8 ) | ( 1 << 9 );

    //// configure ports
    //// TX0 = PA0 and RX0 = PA1

	// enable the pin's digital function
	GPIO_PORTA_DEN_R 	|= ( 1 << 0 ) | ( 1 <<  1 );

    // use PA0, PA1 alternate function
    GPIO_PORTA_AFSEL_R  |= ( 1 << 0 ) | ( 1 << 1 );

    // connfigure PA0 and PA1 for UART_TYPE
    GPIO_PORTA_PCTL_R   |= ( 1 << 0 ) | ( 1 << 4 );

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
