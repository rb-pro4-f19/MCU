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

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static void         PID_operate(PID* this, MOTOR* m_this);
static void 		PID_operate_v2(PID* this, MOTOR* m_this);
static PID*         PID_new(float Kp, float Ki, float Kd, float N);
static void 		PID_del(PID* this);

/****************************   Class Struct   *****************************/

const struct PID_CLASS pid =
{

	.new			= &PID_new,
	.del			= &PID_del,
	.operate        = &PID_operate,
	.operate_v2     = &PID_operate_v2

};

/***********************   Constructive Functions   ************************/

static PID* PID_new(float Kp, float Ki, float Kd, float N)
{
	// allocate memory
	PID* this = malloc(sizeof(MOTOR));

	this->Kp = Kp;
	this->Ki = Ki;
	this->Kd = Kd;
	this->Tf = 0;
	this->v[0] = 0;
	this->v[1] = 0;
	this->v[2] = 0;
	this->e[0] = 0;
	this->e[1] = 0;
	this->y[0] = 0;
	this->y[1] = 0;
	this->y[2] = 0;
	this->r[0] = 0;
	this->r[1] = 0;
	this->r[2] = 0;
	this->u  = 0;
	this->Ts = 0;
	this->antiwindup = 0;

	// return pointer to instance
	return this;
}

static void PID_del(PID* this)
{
	free(this);
}

/***********************   Constructive Functions   ************************/

static void PID_operate(PID* this, MOTOR* m_this)
{
	// currently is only a simple PI regulator

	// calculate new error
	mot.get_enc(m_this);

	// measure the current position
	this->y[0] = m_this->enc;

	// prev error is updated
	this->e[1] = this->e[0];

	// new error is calculated
	this->e[0] = (this->r[0]) - (this->y[0]);

	// prev signal is updated
	this->v[1] = this->v[0];


	// is antiwindup set?
	if ( (this->antiwindup) == 1)
	{
		// update new signal without antiwindup
		this->v[0] = (this->e[0])*((this->Kp)+(this->Ki*(this->Ts)*0.5)) + this->e[1]*(((this->Ki)*(this->Ts)*0.5)-(this->Kp))+(this->v[1]);
	}
	else
	{
		// update old signal with antiwindup
		this->v[0] = (this->e[0])*(this->Kp)+(this->e[1])*(-(this->Kp))+(this->v[1]);
	};

	// is pwm saturated? there is two saturation limits 127 and -127
	if ((this->v[0]) > PWM_MAX)
	{
		this->v[0] = PWM_MAX;
		this->antiwindup = 0;
	}
	else if ((this->v[0]) < PWM_MIN )
	{
		this->v[0] = PWM_MIN;
		this->antiwindup = 0;
	}
	else this->antiwindup = 1;

	mot.set_pwm(m_this,(int8_t)(this->v[0]));

}

static void PID_operate_v2(PID* this, MOTOR* m_this)
{
    // currently is only a PID_operate_v2 regulator

	float Kp1, Kp2, Kp3;
	float Ki1 = 0, Ki2 = 0, Ki3 = 0;
	float Kd3;
	float Uk1, Uk2;

    // calculate new error
    mot.get_enc(m_this);

	// save prev positions
	this->y[2] = this->y[1];
	this->y[1] = this->y[0];
	this->y[0] = m_this->enc;

	// prev error is updated
	this->r[2] = this->r[1];
	this->r[1] = this->r[0];
	this->r[0] = this->r[0];

	Kp1 = this->Kp * ( this->b * this->r[0] - this->y[0] );
	Kp2 = -1 * ( this->Tf * this->Kp * 4 ) / ( 2 * this->Tf + this->Ts ) * ( this->b * this->r[1] - this->y[1] );
	Kp3 = this->Kp * ( 2 * this->Tf - this->Ts ) / ( 2 * this->Tf + this->Ts ) * ( this->b * this->r[2]-this->y[2] );

	if ( this->antiwindup == 1 )
	{
		Ki1 = this->Ki * this->Ts * ( this->r[0] - this->y[0] );
		Ki2 = this->Ts * this->Ts * this->Ki / ( 2 * this->Tf + this->Ts ) * ( this->r[1] - this->y[1] );
		Ki3 = ( ( this->Ts * this->Ts * this->Ki ) - ( 2 * this->Tf * this->Ki * this->Ts ) ) / ( 2 * ( 2 * this->Tf + this->Ts ) ) * ( this->r[2] - this->y[2] );
	}

	Kd3 = this->Kd * 2 / ( 2 * this->Tf + this->Ts ) * ( this->c * this->r[2] - this->y[2] );
	Uk1 = 4 * this->Tf / ( 2 * this->Tf + this->Ts ) * this->v[1];
	Uk2 = -1 * ( 2 * this->Tf - this->Ts ) / ( 2 * this->Tf + this->Ts ) * this->v[2];

	// prev signal is updated
	this->v[2] = this->v[1];
	this->v[1] = this->v[0];
	this->v[0] = Kp1 + Kp2 + Kp3 + Ki1 + Ki2 + Ki3 + Kd3 + Uk1 + Uk2;

	// is pwm saturated? there is two saturation limits 127 and -127
	if ( ( this->v[0] ) > PWM_MAX )
	{
		this->v[0] = PWM_MAX;
		this->antiwindup = 0;
	}
	else if ((this->v[0]) < PWM_MIN )
	{
		this->v[0] = PWM_MIN;
		this->antiwindup = 0;
	}
	else this->antiwindup = 1;

	mot.set_pwm(m_this,(int8_t)(this->v[0]));

}

/****************************** End Of Module ******************************/
