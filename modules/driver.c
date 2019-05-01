#include "driver.h"

// Missing definitions in tm4c123gh6pm.h file
#define NVIC_INT_CTRL_PEND_SYST   0x04000000  // Pend a systick int
#define NVIC_INT_CTRL_UNPEND_SYST 0x02000000  // Unpend a systick int
#define SYSTICK_PRIORITY    0xE

static bool irq_status = true;

void __enable_irq(void)
{
#ifndef _MSC_VER
	// enable global interrupts using ASM
	asm volatile
	(
		"CPSIE i"
	);

	// update status
	irq_status = true;
#else
	return;
#endif
}

bool __disable_irq(void)
{
#ifndef _MSC_VER
	// return false if interrupts are already disabled
	if (!irq_status) return false;

	// disable global interrupts using ASM
	asm volatile
	(
		"CPSID i"
	);

	// update status and return true
	irq_status = false;
	return true;
#else
	return false;
#endif
}

bool get_irq(void)
{
	return irq_status;
}

static int set_systick_reload_value(uint16_t tick_period_us)
{
	return tick_period_us * 16;
}

void sys_tick_init(uint16_t tick_period_us)
{

#ifndef _MSC_VER

	// Note flag is cleared by reading the STRCTL or writing CTCURRENT Register
	NVIC_ST_CTRL_R &=  ~(NVIC_ST_CTRL_ENABLE);

	// tick_period = SYSTICK_RELOAD_VALUE 3199999 // 200 mS
	NVIC_ST_CURRENT_R = set_systick_reload_value(tick_period_us);

	// Set Reload value, Systick reload register
	NVIC_ST_RELOAD_R = set_systick_reload_value(tick_period_us);

	// On a write, removes the pending state from the SysTick exception s. 159
	NVIC_INT_CTRL_R |= NVIC_INT_CTRL_UNPEND_SYST;

	// First Clear Then Set - Low priority, s. 171
	NVIC_SYS_PRI3_R &= ~(NVIC_SYS_PRI3_TICK_M);
	NVIC_SYS_PRI3_R |= (NVIC_SYS_PRI3_TICK_M);

	// Enable systick interrupt
	NVIC_ST_CTRL_R |= (NVIC_ST_CTRL_INTEN) | (NVIC_ST_CTRL_CLK_SRC);

	// Enable and start timer
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE;

#else

	return;

#endif
}

void sys_time_period_init()
{
	volatile int dummy;

	// Enable the GPIO port that is used for the on-board LED.
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;

	// Do a dummy read to insert a few cycles after enabling the peripheral.
	dummy = SYSCTL_RCGC2_R;

	// Set the direction as output (PF0)
	GPIO_PORTF_DIR_R |= 0x02;

	// Enable the GPIO pins for digital function (PF0).
	GPIO_PORTF_DEN_R |= 0x02;

	// GPIO pins output
	GPIO_PORTF_DATA_R &= ~(1 << 1);
}

void pin_rise()
{
	GPIO_PORTF_DATA_R = (0 << 1);
	GPIO_PORTF_DATA_R = (1 << 1);
}

void pin_fall()
{
	GPIO_PORTF_DATA_R = (1 << 1);
	GPIO_PORTF_DATA_R = (0 << 1);
}

void toggle_period(uint8_t state)
{
	// I AM ALIVE
	GPIO_PORTF_DATA_R = (state << 1);
}
