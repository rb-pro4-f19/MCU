#include <stdint.h>
#include <stdbool.h>

#include "uart.h"
#include "chksum.h"

void uart_test(void)
{
    UART* uart1 = uart.new(2);

    uint8_t payloadarr[14] = {0};

    UART_FRAME  ngt =
    {
     0,
     0,
     payloadarr,
     0
    };

    if(uart.read(uart1, &ngt))       // read UART FRAME
    {
        uart.send(uart1, ACK, payloadarr, ngt.size);
    }




}
