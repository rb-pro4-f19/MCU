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
	.operate        = &PID_operate

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
	this->V[0] = 0;
	this->V[1] = 0;
	this->E[0] = 0;
	this->E[1] = 0;
	this->U  = 0;
	this->Y  = 0;
	this->R  = 0;
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
	this->Y = m_this->enc;

	// prev error is updated
	this->E[1] = this->E[0];

	// new error is calculated
	this->E[0] = (this->R) - (this->Y);

	// prev signal is updated
	this->V[1] = this->V[0];


	// is antiwindup set?
	if (!(this->antiwindup))
	{
		// update new signal without antiwindup
		this->V[0] = (this->E[0])*((this->Kp)+(this->Ki*(this->Ts)*0.5)) + this->E[1]*(((this->Ki)*(this->Ts)*0.5)-(this->Kp))+(this->V[1]);
	}
	else
	{
		// update old signal with antiwindup
		this->V[0] = (this->E[0])*(this->Kp)+(this->E[1])*(-(this->Kp))+(this->V[1]);
	};

	// is pwm saturated? there is two saturation limits 127 and -127
	if ((this->V[0]) > PWM_MAX)
	{
		this->V[0] = PWM_MAX;
		this->antiwindup = 1;
	}
	else if ((this->V[0]) < PWM_MIN )
	{
		this->V[0] = PWM_MIN;
		this->antiwindup = 1;
	}
	else this->antiwindup = 0;

	mot.set_pwm(m_this,(int8_t)(this->V[0]));

}

static void PID_operate_v2(PID* this, MOTOR* m_this)
{
    // currently is only a PID_operate_v2 regulator

	float Kp1, Kp2, Kp3;
	float Ki1, Ki2, Ki3;
	float Kd3;
	float Uk1, Uk2;

    // calculate new error
    mot.get_enc(m_this);

	Kp1 = this->Kp * ( this->b * this->R[0] - this->Y[0] );

	Kp2 = -1 * ( this->Tf * this->Kp * 4 ) / ( 2 * this->Tf + this->Ts ) * ( this->b * this->R[1] - this->Y[1] );

	Kp3 = this->Kp * ( 2 * this->Tf - this->Ts ) / ( 2 * this->Tf + this->Ts ) * ( this->b * this->R[2]-this->Y[2] );

	Ki1 = this->Ki * this->Ts * ( this->R[0] - this->Y[0] );

	Ki2 = this->Ts * this->Ts * this->Ki / ( 2 * this->Tf + this->Ts ) * ( this->R[1] - this->Y[1] );

	Ki3 = ( ( this->Ts * this->Ts * this->Ki ) - ( 2 * this->Tf * this->Ki * this->Ts ) ) / ( 2 * ( 2 * this->Tf + this->Ts ) ) * ( this->R[2] - this->Y[2] );

	Kd3 = this->Kd * 2 / ( 2 * this->Tf + this->Ts ) * ( this->c * this->R[2] - this->Y[2] );

	Uk1 = 4 * this->Tf / ( 2 * this->Tf + this->Ts ) * this->V[1];

	Uk2 = -1 * ( 2 * this->Tf - this->Ts ) / ( 2 * this->Tf + this->Ts ) * this->V[2];

	this->V[0] = Kp1 + Kp2 + Kp3 + Ki1 + Ki2 + Ki3 + Kd3 + Uk1 + Uk2;


}

/****************************** End Of Module ******************************/
