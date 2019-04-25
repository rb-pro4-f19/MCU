/****************************************************************************
* University of Southern Denmark
* RB-PRO4 F19
*
* FILENAME...:	cli.c
* MODULENAME.:	CLI (static class)
*
* For an API and DESCRIPTION, please refer to the  module
* specification file (.h-file).
*
****************************************************************************/

/***************************** Include files *******************************/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <assert.h>
#include "mot.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

typedef struct PID PID;

/************************  Function Declarations ***************************/

extern const struct PID_CLASS
{

	PID* 			(* const new)(float Kp, float Ki, float Kd, float N);
	void 			(* const del)(PID* this);
	void            (* const operate)(PID* this, MOTOR* m_this);


} pid;

/*****************************    Constructs   *****************************/

struct PID
{

    float Kp;
    float Ki;
    float Kd;
    float Tf;
    float V[2];
    float E[2];
    float U;
    float Y;
    float R;
    float Ts;
    bool antiwindup;

};

/*****************************    Constructs   *****************************/
