#include <stdint.h>
#include <stdbool.h>

#include "../tm4c123gh6pm.h"

extern void     __enable_irq(void);
extern bool     __disable_irq(void);

extern bool  	get_irq(void);
extern void     sys_tick_init(uint16_t tick_period_us);

extern void 	sys_time_period_init();
extern void 	toggle_period(uint8_t state);
extern void 	pin_rise();
extern void 	pin_fall();
