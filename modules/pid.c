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
#define KI		(this->Ki * this->Ki_en)
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
static void         PID_operate_v3(PID* this);
static void         PID_operate_v4(PID* this);
static PID*         PID_new(MOTOR* mot, float Kp, float Ki, float Kd, float Ts);
static void 		PID_del(PID* this);

/****************************   Class Struct   *****************************/

const struct PID_CLASS pid =
{
	.new			= &PID_new,
	.del			= &PID_del,
	.operate		= &PID_operate,
	.operate_v2		= &PID_operate_v2,
	.operate_v3     = &PID_operate_v3,
	.operate_v4     = &PID_operate_v4,

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
	this->N         = 0;

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
	static float Ki1, Ki2, Ki3;
	static float Kd1, Kd2, Kd3;
	static float vk1, vk2;
	static float precalculation;

	// calculate new error
	mot.get_enc(this->mot);

	// save prev positions
	this->y[2] = this->y[1];
	this->y[1] = this->y[0];
	this->y[0] = this->mot->enc;


	// Kp terms are calculated
	vk1 = 8 * this->Tf * this->v[1];
	vk2 = 2 * ( this->Ts - 2 * this->Tf ) * this->v[2];

	Kp1 = 2 * this->Kp * ( ( 2 * this->Tf ) + this->Ts ) * ( this->b * this->r[0] - this->y[0] );
	Ki1 = KI * this->Ts * ( this->Ts + 2 * this->Tf ) * ( this->r[0] - this->y[0] );
	Kd1 = 4 * this->Kd * ( this->c * this->r[0] - this->y[0] );

	Kp2 = -8 * this->Kp * this->Tf * ( this->b * this->r[1] - this->y[1] );
	Ki2 = 2 * KI * this->Ts * this->Ts * ( this->r[1] - this->y[1] );
	Kd2 = -8 * this->Kd * ( this->c * this->r[1] - this->y[1] );

	Kp3 = 2 * this->Kp * ( 2 * this->Tf - this->Ts ) * ( this->b * this->r[2] - this->y[2] );
    Ki3 = KI * this->Ts * ( this->Ts - 2 * this->Tf ) * ( this->r[2] - this->y[2] );
	Kd3 = 4 * this->Kd * ( this->c * this->r[2] - this->y[2] );

	precalculation = Kp1+Kp2+Kp3+Ki1+Ki2+Ki3+Kd1+Kd2+Kd3+vk1+vk2;
	this->v[0] = precalculation / (4 * this->Tf + 2 * this->Ts);

	// prev signal is updated
	this->v[2] = this->v[1];
	this->v[1] = this->v[0];

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

	// prev reference is updated
	this->r[2] = this->r[1];
	this->r[1] = this->r[0];

}

static void PID_operate_v3(PID* this)
{
	// currently is only a PID_operate_v2 regulator

	static float Kp1, Kp2, Kp3;
	static float Ki1, Ki2, Ki3;
	static float Kd1, Kd2, Kd3;
	static float vk1, vk2;
	static float precalculation;

	// calculate new error
	mot.get_enc(this->mot);

	// save prev positions
	this->y[2] = this->y[1];
	this->y[1] = this->y[0];
	this->y[0] = this->mot->enc;


	// Kp terms are calculated
	vk1 = 8 * this->v[1];
	vk2 = (2 * this->N * this->Ts - 4) * this->v[2];

	Kp1 = this->Kp * ( 4 + 2 * this->N * this->Ts ) * ( this->b * this->r[0] - this->y[0] ) ;
	Ki1 = KI * this->Ts * (2 * this->N * this->Ts + 2) * ( this->r[0] - this->y[0] );
	Kd1 = 4 * this->Kd * this->N * ( this->c * this->r[0] - this->y[0] );

	Kp2 = -8 * this->Kp * ( this->b * this->r[1] - this->y[1] );
	Ki2 = 2 * KI * this->Ts * this->Ts * this->N * ( this->r[1] - this->y[1] );
	Kd2 = -8 * this->Kd * this->N * ( this->c * this->r[1] - this->y[1] );

	Kp3 = 2 * this->Kp * ( 2 - this->Ts * this->N ) * ( this->b * this->r[2] - this->y[2] );
    Ki3 = KI * this->Ts * ( this->Ts * this->N - 2 ) * ( this->r[2] - this->y[2] );
	Kd3 = 4 * this->Kd * this->N * ( this->c * this->r[2] - this->y[2] );

	precalculation = Kp1+Kp2+Kp3+Ki1+Ki2+Ki3+Kd1+Kd2+Kd3+vk1+vk2;
	this->v[0] = precalculation / (4 + 2 * this->N * this->Ts);

	// prev signal is updated
	this->v[2] = this->v[1];
	this->v[1] = this->v[0];

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

	// prev reference is updated
	this->r[2] = this->r[1];
	this->r[1] = this->r[0];

}

static void PID_operate_v4(PID * this)
{
    // calculate new error
    mot.get_enc(this->mot);

    this->y[0] = this->mot->enc;

    this->v[0] = (this->r[0]- this->y[0])* this->Kp;

    if ((this->v[0] > SAT_MAX) || (this->v[0] < SAT_MIN))
    {
        // bound output
        this->u = (this->v[0] > SAT_MAX) ? SAT_MAX : SAT_MIN;
    }
    else
    {
        this->u = this->v[0];
    }
    // output control signal
    // the PWM must be inverted
    mot.set_pwm(this->mot, (int8_t)((-1)*(this->u)));

}


/****************************** End Of Module ******************************/


/****************************** End Of Module ******************************/
