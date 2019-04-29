/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	sys.c
* MODULENAME.:	SYSTEM
*
* For an API and DESCRIPTION, please refer to the  module
* specification file (.h-file).
*
****************************************************************************/

/***************************** Include files *******************************/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include "sys.h"

/*****************************    Defines    *******************************/

//#define abs(X,Y,Z) ( (X > Y) ? (X - Y) > Z : (Y - X) > Z )
#define LAMBDA(c_) ({ c_ _; })

/*****************************   Constants   *******************************/

#define SYSTICK_DUR_US				100		// us
#define DEFAULT_MOT_FREQ			80		// kHz
#define GUI_UPDATE_DELAY			5	 	// ms

#define CAL_PAN_SEEK_BOUNDARY_DUR	300		// ms
#define CAL_PAN_SEEK_BOUNDARY_PWM	-40
#define CAL_PAN_SEEK_HAL_PWM		43
#define CAL_TILT_SEEK_HAL_PWM		-40
#define CAL_TILT_FINETUNE_PWM		25
#define CAL_TILT_FINETUNE_DUR		1300	// ms
#define CAL_REDUNDANCY_NUM			4

#define MOT0_BOUNDARY_MAX			540		// ticks
#define MOT1_BOUNDARY_H				236		// ticks
#define MOT1_BOUNDARY_L				-243	// ticks

#define PID0_KP						2
#define PID0_KI 					20
#define PID1_KP						2
#define PID1_KI 					20

#define MAX_SAMPLES                 2048

/*****************************   Variables   *******************************/

SPI*		spi_main;
UART* 		uart_main;
MOTOR* 		mot0;
MOTOR* 		mot1;
HAL*		hal0;
HAL*		hal1;
PID* 		pid0;
PID* 		pid1;

/************************  Function Declarations ***************************/

static void 		SYSTEM_init(void);
static void 		SYSTEM_operate(void);

static void			SYSTEM_echo(void);
static void 		SYSTEM_sample(SYSTEM_VAR var, SAMPLE_TYPE type, const uint8_t* dur_ms_arr, const uint8_t* addr_arr);

static void 		SYSTEM_set_mode(SYS_MODE mode);
static void 		SYSTEM_set_pos(SPI_ADDR mot_addr, const uint8_t* flt_array);
static void 		SYSTEM_set_gui(bool option);
static void 		SYSTEM_set_sampler(bool option);
static void 		SYSTEM_set_msg(bool option);

static void 		SYSTEM_set_pwm(SPI_ADDR mot_addr, int8_t pwm);
static void 		SYSTEM_set_freq(SPI_ADDR mot_addr, uint8_t freq_khz);
static void 		SYSTEM_set_slew(TARGET_SLEW target_slew, bool option);
static void 		SYSTEM_set_bound(bool option);

static void 		SYSTEM_set_pid(SPI_ADDR mot_addr, PID_PARAM param, const uint8_t* flt_array);

static void 		SYSTEM_get_enc(SPI_ADDR enc_addr);
static void 		SYSTEM_get_hal(SPI_ADDR hal_addr);

static void 		_SYSTEM_to_gui(void);
static void         _SYSTEM_to_gui_bg(void);
static inline void  _SYSTEM_fill_gui(void);

static void 		_SYSTEM_MODE_calibration(void);

/****************************   Class Struct   *****************************/

struct SYSTEM_CLASS sys =
{
	.mode			= SYS_IDLE,

	.init_done		= false,
	.cal_done		= false,

	.use_gui		= false,
	.use_slew		= true,
	.use_sampler	= true,

	.tp_cal			= NULL,
	.tp_gui			= NULL,
	.tp_tst			= NULL,

	.cal_state 		= CAL_RESET,
	.gui_data		= {},
	.op_time	 	= 0,

	.init			= &SYSTEM_init,
	.operate		= &SYSTEM_operate,

	.echo 			= &SYSTEM_echo,
	.sample			= &SYSTEM_sample,

	.set_mode		= &SYSTEM_set_mode,
	.set_pos		= &SYSTEM_set_pos,
	.set_gui		= &SYSTEM_set_gui,
	.set_msg		= &SYSTEM_set_msg,

	.set_pwm		= &SYSTEM_set_pwm,
	.set_freq 		= &SYSTEM_set_freq,
	.set_slew		= &SYSTEM_set_slew,
	.set_bound		= &SYSTEM_set_bound,

	.set_pid 		= &SYSTEM_set_pid,

	.get_enc 		= &SYSTEM_get_enc,
	.get_hal 		= &SYSTEM_get_hal,
};

/*****************************   Functions   *******************************/

static void SYSTEM_init(void)
{
	// disable interrupts
	__disable_irq();

	// init SysTick
	sys_tick_init(SYSTICK_DUR_US);
	tp.init_systick(SYSTICK_DUR_US, us);

	// init UART @ 921600 baud
	uart_main = uart.new(BAUD_921600);

	// init SPI @ 1 MHz (16 000 000 / >16<)
	spi_main = spi.new(16);

	// assign UART module to CLI class
	cli.uart_module = uart_main;

	// assign SPI to MOTOR class
	mot.spi_module = spi_main;

	// assign SPI to HAL class
	hal.spi_module = spi_main;

	// init SAMPLER w/ UART module and max_samples size
	sampler.init(uart_main, MAX_SAMPLES);

	// init MOT0 w/ ENC0 to 10 kHz (tilt)
	mot0 = mot.new(MOT0, ENC0, DEFAULT_MOT_FREQ);

	// init MOT1 w/ ENC0 to 10 kHz (pan)
	mot1 = mot.new(MOT1, ENC1, DEFAULT_MOT_FREQ);

	// set motor boundaries
	mot0->bound = false;
	mot0->bound_max = MOT0_BOUNDARY_MAX;

	mot1->bound = true;
	mot1->bound_l = MOT1_BOUNDARY_L;
	mot1->bound_h = MOT1_BOUNDARY_H;

	// init HAL0
	hal0 = hal.new(HAL0);

	// init HAL1
	hal1 = hal.new(HAL1);

	// init PID0 (tilt)
	pid0 = pid.new(mot0, 0.1f, 0.0f, 0.0f, 0.002f);
	pid0->Kp = PID0_KP;
	pid0->Ki = PID0_KI;
	pid0->clamp = true;
	pid0->sat_max = 127;
	pid0->sat_min = -127;

	// init PID1 (pan)
	pid1 = pid.new(mot1, 0.1f, 0.1f, 0.0f, 0.002f);
	pid1->Kp = PID1_KP;
	pid1->Ki = PID1_KI;
	pid1->sat_max = 127;
	pid1->sat_min = -127;

	// setup TIMEPOINTS
	sys.tp_cal = tp.new();
	sys.tp_gui = tp.new();
	sys.tp_tst = tp.new();

	// set system variables
	sys.init_done = true;
	sys.cal_done = false;
	sys.mode = SYS_IDLE;

	// initialize GUI data
	_SYSTEM_fill_gui();

	// enable interrupts
	__enable_irq();
}

/********************************   FSM   **********************************/

static void SYSTEM_operate(void)
{

	// send data to GUI if enabled
	if (sys.use_gui)			{ _SYSTEM_to_gui_bg(); }
	if (sys.use_sampler)		{ sampler.operate(); }

	// finite state machine
	switch (sys.mode)
	{

		case SYS_IDLE:
		{
			break;
		}

		case SYS_CALIBRATION:
		{

			_SYSTEM_MODE_calibration();

			// control motors (feed the watchdog, woof woof)
			mot.operate(mot0);
			mot.operate(mot1);

			break;
		}

		case SYS_TUNING:
		{
			sys.op_time = tp.lmeasure(LAMBDA(void _(void)
			{

			// operate controllers
			pid.operate(pid0);
			//pid.operate(pid1);

			// operate motors
			mot.operate(mot0);
			mot.operate(mot1);

			}), ms);

			break;
		}

		case SYS_OPERATION:
		{

			// lambda used to measure execution time of operation loop
			sys.op_time = tp.lmeasure(LAMBDA(void _(void)
			{

			// check if system not calibrated
			if (!sys.cal_done) { return; }

			// control motors (feed the watchdog, woof woof)
			mot.operate(mot0);
			mot.operate(mot1);

			// read encoders
			mot.get_enc(mot0);
			mot.get_enc(mot1);

			// read hall sensors
			hal.read(hal0);
			hal.read(hal1);

			}), ms);

			break;
		}

		default:
		{
			sys.mode = SYS_IDLE;
			break;
		}

	}
}

/***************************   System Calls   ******************************/

static void	SYSTEM_echo(void)
{
	cli.msg("Hello world!");
}

static void SYSTEM_sample(SYSTEM_VAR var, SAMPLE_TYPE type, const uint8_t* dur_ms_arr, const uint8_t* addr_arr)
{
	static void* 	target_var;
	static uint32_t target_addr = 0;
	static uint16_t target_dur_ms = 0;

	// select which variable to sample
	switch (var)
	{
		case SV_ADDR:
		{
			// parse target address from byte array
			memcpy(&target_addr, addr_arr, sizeof(uint32_t));

			// set target variable to parsed address
			target_var = (void*)target_addr;

			break;
		}

		case SV_PID0_U:
		{
			target_var = &(pid0->u);
			break;
		}

		default:
		{
			break;
		}
	}

	// parse target duration from byte array
	memcpy(&target_dur_ms, dur_ms_arr, sizeof(uint16_t));

	// initialize sampler
	sampler.sample(target_var, type, target_dur_ms);
}

static void SYSTEM_set_mode(SYS_MODE mode)
{
	// set mode
	sys.mode = mode;

	// default configuration
	switch(mode)
	{
		case SYS_IDLE:
		{
			break;
		}

		case SYS_TUNING:
		{
			// disable slew
			sys.set_slew(TARGET_SLEW_MOT, false);

			break;
		}

		case SYS_CALIBRATION:
		{
			// reset calibration state
			sys.cal_state = CAL_RESET;

			// disable boundaries
			sys.set_bound(false);

			break;
		}

		case SYS_OPERATION:
		{
			// enable slew
			sys.set_slew(TARGET_SLEW_MOT, true);
			sys.set_slew(TARGET_SLEW_REF, true);

			// enable boundaries
			sys.set_bound(true);

			break;
		}
	}

	// message info
	cli.msgf("Mode set to %d.", (uint8_t)mode);
}

static void SYSTEM_set_pos(SPI_ADDR mot_addr, const uint8_t* flt_array)
{
	PID* target_pid = (mot_addr == MOT1) ? pid1 : pid0;
	static float theta_ang = 0.0f;

	// construct float from float byte array
	memcpy(&theta_ang, flt_array, sizeof(float));

	// 1080 / 360 -> encoder ticks relationship
	target_pid->r[0] = 3 * theta_ang;
}

static void SYSTEM_set_gui(bool option)
{
	sys.use_gui = option;
	cli.msgf("GUI was %s.", option ? "enabled" : "disabled");
}

static void SYSTEM_set_sampler(bool option)
{
	sys.use_sampler = option;
	cli.msgf("SAMPLER was %s.", option ? "enabled" : "disabled");
}

static void SYSTEM_set_msg(bool option)
{
	cli.allow_msg = option;
	cli.msgf("MSG was %s.", option ? "enabled" : "disabled");
}

static void SYSTEM_set_pwm(SPI_ADDR mot_addr, int8_t pwm)
{
	mot.set_pwm(mot_addr == MOT1 ? mot1 : mot0, pwm);
	cli.msgf("PWM of MOT%u was set to %d.", mot_addr - 1, pwm);
}

static void SYSTEM_set_freq(SPI_ADDR mot_addr, uint8_t freq_khz)
{
	mot.set_freq(mot_addr == MOT1 ? mot1 : mot0, freq_khz);
	cli.msgf("FRQ of MOT%u was set to %d kHz.", mot_addr - 1, freq_khz);
}

static void SYSTEM_set_slew(TARGET_SLEW target_slew, bool option)
{
	switch(target_slew)
	{
		case TARGET_SLEW_REF:
		{
			sys.use_slew = option;
			cli.msgf("REF slew was %s.", option ? "enabled" : "disabled");

			break;
		}

		case TARGET_SLEW_MOT:
		{
			mot0->slew = option;
			mot1->slew = option;
			cli.msgf("MOT slew was %s.", option ? "enabled" : "disabled");

			break;
		}
	}

}

static void SYSTEM_set_bound(bool option)
{
	//mot0->bound = option;
	mot1->bound = option;
	cli.msgf("MOT boundary was %s.", option ? "enabled" : "disabled");
}

static void SYSTEM_set_pid(SPI_ADDR mot_addr, PID_PARAM param, const uint8_t* flt_array)
{
	float rx_float      = 0.0f;
	PID*  target_pid 	= (mot_addr == MOT0) ? pid0 : pid1;

	// construct float from float byte array
	memcpy(&rx_float, flt_array, sizeof(float));

	// set PID parameter
	switch (param)
	{
		case PID_KP:
		{
			target_pid->Kp = rx_float;
			break;
		}

		case PID_KI:
		{
			target_pid->Ki = rx_float;
			break;
		}

		case PID_KD:
		{
			target_pid->Kd = rx_float;
			break;
		}

		case PID_CLAMP:
		{
			target_pid->clamp = (bool)rx_float;
			break;
		}

		default:
		{
		    break;
		}
	}

}

static void SYSTEM_get_enc(SPI_ADDR enc_addr)
{
	int16_t enc_dat =  mot.get_enc(enc_addr == ENC1 ? mot1 : mot0);
	cli.msgf("Delta of ENC%u is %d ticks.", enc_addr - 3, enc_dat);
}

static void SYSTEM_get_hal(SPI_ADDR hal_addr)
{
	bool hal_dat = hal.read(hal_addr == HAL1 ? hal1 : hal0);
	cli.msgf("Sensor HAL%u is %d.", hal_addr - 5, hal_dat);
}

/****************************   System Modes   *****************************/

static void _SYSTEM_to_gui(void)
{
	// !!!
	// DEPRECATED METHOD
	// takes too long to transfer data.

	// flow control
	if (tp.delta_now(sys.tp_gui, ms) < GUI_UPDATE_DELAY * 2 ) { return; }

	// populate GUI data
	_SYSTEM_fill_gui();

	// stream data
	// 160 bytes @ 921600 bps = 1.4ms + overhead
	uart.stream(uart_main, &sys.gui_data, sizeof(sys.gui_data));

	// disable streaming
	//sys.to_gui = false;

	// reset timepoint
	tp.set(sys.tp_gui, tp.now());
}

static void _SYSTEM_to_gui_bg(void)
{
	// flow control
	if (tp.delta_now(sys.tp_gui, ms) < GUI_UPDATE_DELAY ) { return; }

	// variables
	static uint8_t* byte;
	static size_t 	obj_size;

	if (obj_size--)
	// iterate bytes of object and send them individually
	{
		uart.send(uart_main, UART_STREAM, byte++, 1);
		//if (obj_size == 0) { sys.to_gui = false; }
	}
	else
	{
		// populate GUI data
		_SYSTEM_fill_gui();

		// reset pointer and size
		byte = (uint8_t*)(&sys.gui_data);
		obj_size = sizeof(sys.gui_data);

		// transmit reset frame
		uart.send(uart_main, UART_STREAM, (uint8_t[2]){0}, 2);
	}

	// reset flowcontrol timepoint
	tp.set(sys.tp_gui, tp.now());
}

static inline void _SYSTEM_fill_gui(void)
{

	// operation data
	sys.gui_data.mode		= sys.mode;
	sys.gui_data.op_time	= sys.op_time;
	sys.gui_data.bound 		= mot1->bound;;
	sys.gui_data.slew_r		= sys.use_slew;
	sys.gui_data.slew_y 	= (mot0->slew) & (mot1->slew);
	sys.gui_data.cal_done	= sys.cal_done;

	// motor0 data
	sys.gui_data.mot0 = (MOT_DATA)
	{
		.pwm		= mot0->pwm,
		.freq		= mot0->freq_khz,
		.enc		= mot0->enc,
		//.spd		= 5.2f,
		.hal		= hal0->val,
		//.pid_i		= 1,
		//.pid_n		= pid0->Tf,
		.pid_kp		= pid0->Kp,
		.pid_ki		= pid0->Ki,
		//.pid_kd		= pid0->Kd,
		.pid_clamp	= pid0->clamp,
	};

	// motor1 data
	sys.gui_data.mot1 = (MOT_DATA)
	{
		.pwm		= mot1->pwm,
		.freq		= mot1->freq_khz,
		.enc		= mot1->enc,
		//.spd		= 0.0f,
		.hal		= hal1->val,
		//.pid_i		= 1,
		//.pid_n		= pid1->Tf,
		.pid_kp		= pid1->Kp,
		.pid_ki		= pid1->Ki,
		//.pid_kd		= pid1->Kd,
		.pid_clamp	= pid1->clamp,
	};
}

static void _SYSTEM_MODE_calibration(void)
{
	// static variables
	static int16_t	mot1_enc_cur, mot1_enc_prev;
	static uint8_t	reduc_check = 0;

	// calibration state machine
	switch (sys.cal_state)
	{

		case CAL_RESET:
		{
			// stop motors
			// PAN  = mot1
			// TILT = mot0
			mot.set_pwm(mot0, 0);
			mot.set_pwm(mot1, 0);

			// wait for motor stop to take effect
			if ((mot0->pwm & mot1->pwm) != 0) { break; }

			// reset variables
			sys.cal_done = false;
			mot1_enc_cur = 0;
			mot1_enc_prev = 0;

			// temporary disable slew rate
			mot0->slew = false;
			mot1->slew = false;

			// disable boundary detection
			mot0->bound = false;
			mot1->bound = false;

			// begin calibration
			sys.cal_state = CAL_PAN_INIT;

			break;
		}

		case CAL_PAN_INIT:
		{
			// set initial PWM of PAN motor and switch to seeking
			// for boundary; begin sampling encoder.

			mot.set_pwm(mot1, CAL_PAN_SEEK_BOUNDARY_PWM);
			mot1_enc_cur = mot.get_enc(mot1);
			sys.cal_state = CAL_PAN_SEEK_BOUNDARY;
			reduc_check = 0;

			break;
		}

		case CAL_PAN_SEEK_BOUNDARY:
		{
			// sample rate of encoder boundary check
			if (tp.delta_now(sys.tp_cal, ms) < CAL_PAN_SEEK_BOUNDARY_DUR ) { break; }

			// should be fast enough so that the motor encoder changes
			// at least every 10 ms.

			// update sampling TIMEPOINT
			tp.set(sys.tp_cal, tp.now());

			mot1_enc_prev = mot1_enc_cur;
			mot1_enc_cur  = mot.get_enc(mot1);

			// check if motor is stuck; break if false
			if (abs(mot1_enc_cur - mot1_enc_prev) != 0) { reduc_check = 0; break; }

			// redundancy check
			if ( ++reduc_check < CAL_REDUNDANCY_NUM ) { break; }

			// reverse motor direction and begin seeking for hall sensor
			mot.set_pwm(mot1, CAL_PAN_SEEK_HAL_PWM);
			sys.cal_state = CAL_PAN_SEEK_HAL;

			break;
		}

		case CAL_PAN_SEEK_HAL:
		{
			// check if hall sensor is reached; break if not

			// !!!
			// can break since disconnected sensors always return HIGH
			if (hal.read(hal1) != HAL_HIGH) { break; }

			// stop PAN motor and switch to calibrating TILT motor
			mot.set_pwm(mot1, 0);
			sys.cal_state = CAL_TILT_INIT;

			break;
		}

		case CAL_TILT_INIT:
		{
			mot.set_pwm(mot0, CAL_TILT_SEEK_HAL_PWM);
			sys.cal_state = CAL_TILT_SEEK_HAL;

			break;
		}

		case CAL_TILT_SEEK_HAL:
		{
			// check if hall sensor is reached; break if not
			if (hal.read(hal0) != HAL_HIGH) { break; }

			// enable slew rate
			// due to sharp switch, i.e. 30 pwm to -10 pwm
			mot0->slew = true;
			mot1->slew = true;

			// set PWM to finetune the position of TILT motor
			// reset TIMEPOINT and begin
			mot.set_pwm(mot0, CAL_TILT_FINETUNE_PWM);
			tp.set(sys.tp_cal, tp.now());
			sys.cal_state = CAL_TILT_FINETUNE;

			break;
		}

		case CAL_TILT_FINETUNE:
		{
			// check if desired finetune duration has passed
			if (tp.delta_now(sys.tp_cal, ms) < CAL_TILT_FINETUNE_DUR ) { break; }

			mot0->slew = false;
			mot1->slew = false;

			// stop motor
			mot.set_pwm(mot0, 0);

			// finish up
			sys.cal_state = CAL_FINISH;

			break;
		}

		case CAL_FINISH:
		{
			// finish it up, Bob!

			// for this to be an actual calibration the encoder values should be read
			// and saved here.

			// clear encoder buffer
			mot.get_enc(mot0);
			mot.get_enc(mot1);

			// reset stored encoder values
			mot0->enc = 0;
			mot1->enc = 0;

			// enable slew
			mot0->slew = true;
			mot1->slew = true;

			// enable bound
			mot0->bound = true;
			mot1->bound = true;

			sys.cal_done = true;
			sys.cal_state = CAL_RESET;
			sys.mode = SYS_IDLE;

			return;
		}

	};
}

/****************************** End Of Module ******************************/
