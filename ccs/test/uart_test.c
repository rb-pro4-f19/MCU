#include <stdint.h>
#include <stdbool.h>

#include "uart.h"
#include "chksum.h"

void uart_test(void)
{

	// uart.h
	// version 1.0.1

	// create new UART instance with BAUDRATE 9600
	// currently not implemented
	UART* uart1 = uart.new(2);

	// construct UART frame
	UART_FRAME frm =
	{
	 	 0,
		 0,
		 (uint8_t[14]){0},
		 0
	};

	// construct and data array
	uint8_t tx_array[4] = {120, 0x22, 0, 'f'};

	// send some data of type 'MSG'
	uart.send(uart1, UART_MSG, tx_array, 4);

	// infinite loop of reading from UART
	for(;;)
	{
		uart.read(uart1, &frm, true);
		for (int i = 0; i < 1000; i++);
	}

}
