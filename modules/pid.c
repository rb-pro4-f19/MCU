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

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static void         PID_operate(PID* this, MOTOR* m_this);
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
	this->N  = N;
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
	// calculate new error
	mot.get_enc(m_this);
	this->Y = m_this->enc;
	this->E[1] = this->E[0];
	this->E[0] = this->R - this->Y;
	this->V[1] = this->V[0];

	if ( this->antiwindup == 0 )
	{
		this->V[0] = this->E[0]*( this->Kp + ( this->Ki * this->Ts / 2 ) ) + this->E[1]*( ( this->Ki * this->Ts / 2 ) - this->Kp ) + this->V[1];
	}
	else
	{
		this->V[0] = this->E[0]*( this->Kp ) + this->E[1]*( -(this->Kp) ) + this->V[1];
	};

	if ( this->V[0] > 127 )
	{
		this->V[0] = 127;
		this->antiwindup = 1;
	}
	else if ( this->V[0] > -127 )
	{
		this->V[0] = 127;
		this->antiwindup = 1;
	}
	else
	{
		this->antiwindup = 0;
	}

	mot.set_pwm(m_this,(int8_t)(this->V[0]));

}

/****************************** End Of Module ******************************/
