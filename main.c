#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "sys.h"

/************************   Interrupt Handlers   ***************************/

void ISR_SYSTICK(void)
{
	tp.systick();
}

/*******************************   Main   **********************************/

int main(void)
{
	/** Command Table ******************************************************/

	// init commands
	// max 8 types and 8 actions per type (#defined in cli.h)
	CMD_TABLE
	{
		{ UART_GET, {
			{ 0x00, CLI_LAMBDA({ sys.echo(); }) },
			{ 0x01, CLI_LAMBDA({ sys.get_enc(args[0]); }) },
			{ 0x02, CLI_LAMBDA({ sys.get_hal(args[0]); }) },
		}},

		{ UART_SET, {
			{ 0x00, NULL },
			{ 0x01, CLI_LAMBDA({ sys.set_pwm(args[0], args[1]); }) },
			{ 0x02, CLI_LAMBDA({ sys.set_freq(args[0], args[1]); }) }
		}}
	};

	/** Initialization *****************************************************/

	    sys.init();

	/** Loop ***************************************************************/

	while(true)
	{
		// check CLI for commands
		cli.check();

		// operate system
		sys.operate();
	}

	/** Garbage Collection *************************************************/

	return 0;
}
