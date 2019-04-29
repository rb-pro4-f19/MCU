/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	mot.c
* MODULENAME.:	MOTOR
*
* For an API and DESCRIPTION, please refer to the  module
* specification file (.h-file).
*
****************************************************************************/

/***************************** Include files *******************************/

#include "pid.h"

/*****************************    Defines    *******************************/

#define PWM_MAX 127
#define PWM_MIN -127

#define KP		this->Kp
#define KI		this->Ki * this->Ki_en
#define KD		this->Kd
#define TS		this->Ts
#define TF		this->Tf
#define SAT_MAX	this->sat_max
#define SAT_MIN	this->sat_min
/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static void 		PID_operate(PID* this);
static void 		PID_operate_v2(PID* this);
static PID*         PID_new(MOTOR* mot, float Kp, float Ki, float Kd, float Ts);
static void 		PID_del(PID* this);

/****************************   Class Struct   *****************************/

const struct PID_CLASS pid =
{
	.new			= &PID_new,
	.del			= &PID_del,
	.operate		= &PID_operate,
	.operate_v2		= &PID_operate_v2,
};

/***********************   Constructive Functions   ************************/

static PID* PID_new(MOTOR* mot, float Kp, float Ki, float Kd, float Ts)
{
	// allocate memory
	PID* this = malloc(sizeof(PID));

	// initialize values
	this->mot		= mot;

	this->Kp 		= Kp;
	this->Ki 		= Ki;
	this->Kd 		= Kd;

	this->Ts 		= Ts;

	this->r[0]		= 0.0f;
	this->r[1]		= 0.0f;
	this->r[2]		= 0.0f;

	this->e[0]		= 0.0f;
	this->e[1]		= 0.0f;

	this->v[0]		= 0.0f;
	this->v[1]		= 0.0f;
	this->v[2]		= 0.0f;

	this->u 		= 0.0f;

	this->y[0]		= 0.0f;
	this->y[1]		= 0.0f;
	this->y[2]		= 0.0f;

	this->b 		= 1.0f;
	this->c 		= 0.0f;
	this->Tf 		= 20.0f;

	this->clamp 	= false;
	this->antiwindup= false;
	this->Ki_en 	= true;
	this->sat_max 	= 0.0f;
	this->sat_min 	= 0.0f;

	// return pointer to instance
	return this;
}

static void PID_del(PID* this)
{
	free(this);
}

/***********************   Constructive Functions   ************************/

static void PID_operate(PID* this)
{
	// measure the current output
	mot.get_enc(this->mot);
	this->y[0] = this->mot->enc;

	// update previous error
	this->e[1] = this->e[0];

	// calculate current error
	this->e[0] = (this->r[0]) - (this->y[0]);

	// update previous signal
	this->v[1] = this->v[0];

	// differene equation
	// defines abstract the 'this' identifier; e.g. KP = this->Kp
	// anti-windup is multiplied on the Ki gain; KI = this->Ki * this->Ki_en
	//this->v[0] = (this->v[1]) + (this->e[0]) * (KP + KI * TS * 0.5) + (this->e[1]) * ((KI * TS * 0.5) - KP);
	this->v[0] = this->v[1] + this->e[0] * (KP + KI * TS * 0.5) + this->e[1] * ((KI * TS * 0.5) - KP);

	// saturation limits & anti-windup
	if ((this->v[0] > SAT_MAX) || (this->v[0] < SAT_MIN))
	{
		// bound output
		this->u = (this->v[0] > SAT_MAX) ? SAT_MAX : SAT_MIN;

		// anti-windup
		this->Ki_en = (this->clamp) ? false : true;
	}
	else
	{
		this->u = this->v[0];
		this->Ki_en = true;
	}

	// output control signal
	// the PWM must be inverted
	mot.set_pwm(this->mot, (int8_t)((-1)*(this->u)));
}

static void PID_operate_v2(PID* this)
{
	// currently is only a PID_operate_v2 regulator

	static float Kp1, Kp2, Kp3;
	static float Ki1 = 0, Ki2 = 0, Ki3 = 0;
	static float Kd3;
	static float Uk1, Uk2;

	// calculate new error
	mot.get_enc(m_this);

	// save prev positions
	this->y[2] = this->y[1];
	this->y[1] = this->y[0];
	this->y[0] = this->mot->enc;

	// prev error is updated
	this->r[2] = this->r[1];
	this->r[1] = this->r[0];
	this->r[0] = this->r[0];

	// Kp terms are calculated
	Kp1 = KP * ( this->b * this->r[0] - this->y[0] );
	Kp2 = -1 * ( TF * KP * 4 ) / ( 2 * TF + TS ) * ( this->b * this->r[1] - this->y[1] );
	Kp3 = KP * ( 2 * TF - TS ) / ( 2 * TF + TS ) * ( this->b * this->r[2] - this->y[2] );

	if ( this->antiwindup == 1 )
	{
		Ki1 = KI * TS * ( this->r[0] - this->y[0] );
		Ki2 = TS * TS * KI / ( 2 * TF + TS ) * ( this->r[1] - this->y[1] );
		Ki3 = ( ( TS * TS * KI ) - ( 2 * TF * KI * TS ) ) / ( 2 * ( 2 * TF + TS ) ) * ( this->r[2] - this->y[2] );
	}
	else
	{
		Ki1 = 0; Ki2 = 0; Ki3 = 0;
	}

	Kd3 = KD * 2 / ( 2 * TF + TS ) * ( this->c * this->r[2] - this->y[2] );
	Uk1 = 4 * TF / ( 2 * TF + TS ) * this->v[1];
	Uk2 = -1 * ( 2 * TF - TS ) / ( 2 * TF + TS ) * this->v[2];

	// prev signal is updated
	this->v[2] = this->v[1];
	this->v[1] = this->v[0];
	this->v[0] = Kp1 + Kp2 + Kp3 + Ki1 + Ki2 + Ki3 + Kd3 + Uk1 + Uk2;

	// saturation limits & anti-windup
	if ((this->v[0] > SAT_MAX) || (this->v[0] < SAT_MIN))
	{
		// bound output
		this->u = (this->v[0] > SAT_MAX) ? SAT_MAX : SAT_MIN;

		// anti-windup
		this->Ki_en = (this->clamp) ? false : true;
	}
	else
	{
		this->u = this->v[0];
		this->Ki_en = true;
	}


	// output control signal
	// the PWM must be inverted
	mot.set_pwm(this->mot, (int8_t)((-1)*(this->u)));

}

/****************************** End Of Module ******************************/
