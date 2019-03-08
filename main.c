#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define __embedded__

#include "driver.h"
#include "tp.h"
#include "spi.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

#define SYSTICK_DUR_MS 1

/*****************************   Variables   *******************************/

/************************    External Methods    ***************************/

extern void tp_test(void);
extern void chksum_test(void);

/************************   Interrupt Handlers   ***************************/

void ISR_SYSTICK(void)
{
    tp.systick();
}

/*******************************   Main   **********************************/

int main(void)
{

    // disable interrupts
    __disable_irq();

    // init SYSTICK
    sys_tick_init(SYSTICK_DUR_MS);
    tp.init_systick(SYSTICK_DUR_MS, ms);

    // enable interrupts
    __enable_irq();

    // test methods
	tp_test();
	//chksum_test();

	SPI* spi_test = spi.new(2);

	spi.send(spi_test, CTRL, 0x0F);

	spi.request(spi_test, ENC1, 12);

	// super-loop
	for(;;)
	{
		for (int i = 0; i < 1000; i++);
	}

	return 0;
}
