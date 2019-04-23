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

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include "assert.h"

#include "cli.h"
#include "mot.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

static PID*			MOTOR_new(float Kp, float Ki, float Kd, float N);
static void 		MOTOR_del(MOTOR* this);

/****************************   Class Struct   *****************************/

struct PID_CLASS mot =
{

	.new			= &PID_new,
	.del			= &PID_del,

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
	this->U[0] = 0;
	this->U[1] = 0;
	this->E[0] = 0;
	this->E[1] = 0;

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
	


}

/****************************** End Of Module ******************************/
