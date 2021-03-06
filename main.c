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

		{ UART_DO, {
			{ DO_ECHO, 		CLI_LAMBDA({ sys.echo(); 										}) },
			{ DO_SAMPLE,	CLI_LAMBDA({ sys.sample(args[0], args[1], &args[2], &args[4]); 	}) },
			{ DO_RESEND,	CLI_LAMBDA({ sys.resend();									 	}) },
		}},

		{ UART_GET, {
			{ GET_ENC, 		CLI_LAMBDA({ sys.get_enc(args[0]); 								}) },
			{ GET_HAL, 		CLI_LAMBDA({ sys.get_hal(args[0]); 								}) },
		}},

		{ UART_SET, {
			{ SET_MODE,		CLI_LAMBDA({ sys.set_mode(args[0]); 							}) },
			{ SET_POS,      CLI_LAMBDA({ sys.set_pos(args[0], &args[1]);					}) },
			{ SET_GUI,		CLI_LAMBDA({ sys.set_gui(args[0]);              				}) },
			{ SET_MSG,		CLI_LAMBDA({ sys.set_msg(args[0]);              				}) },
			{ SET_PWM,		CLI_LAMBDA({ sys.set_pwm(args[0], args[1]); 					}) },
			{ SET_FREQ,		CLI_LAMBDA({ sys.set_freq(args[0], args[1]); 					}) },
			{ SET_SLEW,		CLI_LAMBDA({ sys.set_slew(args[0], args[1]); 	         		}) },
			{ SET_BOUND,	CLI_LAMBDA({ sys.set_bound(args[0]);              				}) },
			{ SET_PID,		CLI_LAMBDA({ sys.set_pid(args[0], args[1], &args[2]);			}) },
		}}
	};

	/** Initialization *****************************************************/

	sys.init();

	/** Loop ***************************************************************/

	while(true)
	{
		pin_rise();

		// check CLI for commands
		cli.check();

		// operate system
		sys.operate();

		pin_fall();
	}

	/** Garbage Collection *************************************************/

	return 0;
}
