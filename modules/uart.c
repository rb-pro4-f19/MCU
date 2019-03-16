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

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

#define SYSCLK 			16000000
#define RX_TIMEOUT_MS 	1000

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static UART*    UART_new(uint8_t clkdiv);
static void		UART_del(UART* this);

static void 	UART_send(UART* this, UART_FRAME_TYPE type, uint8_t* payload, uint8_t payload_size);
static bool		UART_read(UART* this, UART_FRAME* frame, bool send_ack);

static void		_UART_init();
static bool 	_UART_recieve(uint8_t* byte);
static void		_UART_transmit(uint8_t byte);

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
{
	// allocate memory
	UART* this = malloc(sizeof(UART));

	// initialize variables
	this->clkdiv 		= clkdiv;
	this->baudrate 		= BAUD_9600; // temporary
	this->tp_timeout 	= tp.new();

	// initialize UART0 module
	_UART_init();

	// return pointer to instance
	return this;
}

static void UART_del(UART* this)
{
	tp.del(this->tp_timeout);
	free(this);
}

/*****************************   Functions   *******************************/

static void _UART_init()
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

    UART0_IBRD_R        |= 104;

    // 0.1 * 64 + 0.5 = 7
    UART0_FBRD_R        |= 11;

	//  WLEN IS 8 bit, FIFO mode, 1 stop bit - no parity
	UART0_LCRH_R        =  ( 0x3 << 5) | (1 << 4);

	// use system clock
	UART0_CC_R          |= 0;

	// enable UART, RXE, TXE
	UART0_CTL_R         |= ( 1 << 0 ) | ( 1 << 8 ) | ( 1 << 9 );

}

static void UART_send(UART* this, UART_FRAME_TYPE type, uint8_t* payload, uint8_t payload_size)
{
	// construct array of max size:
	// 1 byte header + 256 byte payload + 1 byte chksum
	uint8_t tx_buffer[258] = {0};

	// insert header
	tx_buffer[0] = (type << 5) | (payload_size << 0);

	// insert payload (offset by one due to header)
	for (int i = 0; i < payload_size; i++)
	{
		tx_buffer[i + 1] = payload[i];
	}

	// insert 8-bit checksum after payload
	// generated from (header + payload)
	tx_buffer[payload_size + 1] = chksum.gen_8bit(tx_buffer, payload_size + 1);

	// transmit tx_frame
	for (int i = 0; i < (payload_size + 2); i++)
	{
		_UART_transmit(tx_buffer[i]);
	}
}

static bool UART_read(UART* this, UART_FRAME* frame, bool send_ack)
{
	// check if RX FIFO has data
	if ((UART0_FR_R & (1 << 4)))
	{
		return false;
	}

	// initialize variables
	static uint8_t 	rx_buffer[16];
	uint8_t 		rx_counter = 0;
	bool 			rx_success = false;

	// clear rx_buffer
	for (int i = 0; i < 16; i++)
	{
		rx_buffer[i] = 0;
	}

	// try reading header byte
	if (!_UART_recieve(&rx_buffer[rx_counter++]))
	{
		return false;
	}

	// construct frame header
	frame->type 		= (rx_buffer[0] & 0b11100000) >> 5;
	frame->payload_size = (rx_buffer[0] & 0b00011111) >> 0;

	// start timeout timer
	tp.set(this->tp_timeout, tp.now());

	// +2 (1 byte header & 1 byte chksum)
	while (tp.delta_now(this->tp_timeout, ms) < RX_TIMEOUT_MS && rx_counter < (frame->payload_size + 2))
	{
		// if rx_FIFO is empty
		if (!(UART0_FR_R & (1 << 4)))
		{
			rx_success = _UART_recieve(&rx_buffer[rx_counter++]);
		}
	}

	// check that no errors occured
	if (!rx_success) { return false; }

	// construct frame payload and checksum
	frame->payload	= &rx_buffer[1];
	frame->chksum	= rx_buffer[frame->payload_size + 1];

	// validate recieved checksum
	// generated from (header + payload)
	if (!chksum.val_8bit(rx_buffer, frame->payload_size + 1, frame->chksum))
	{
		return false;
	}

	// send acknowledge with same data if required
	if (send_ack)
	{
		uart.send(this, UART_ACK, frame->payload, frame->payload_size);
	}

	// everything is okay
	return true;
}
/*************************   Private Functions   ***************************/

static void _UART_transmit(uint8_t byte)
{
	// spinlock while transmission FIFO is full
	while (UART0_FR_R & (1 << 5));

	// send byte to FIFO
	UART0_DR_R = byte;
}

static bool _UART_recieve(uint8_t* byte)
{
	// read byte from FIFO
	uint32_t rx_data = UART0_DR_R;

	// test for UART overrun, break error, parity error and framing error
	if (rx_data & (1 << 11) || rx_data & (1 << 10) || rx_data & (1 << 9) || rx_data & (1 << 8))
	{
		return false;
	}

	// write value to byte pointer
	*byte = (uint8_t)rx_data;
	return true;
}

/****************************** End Of Module ******************************/
