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

#include "mot_op.h"

/*****************************    Defines    *******************************/

#define PID_LAMBDA(_x) ({ inline float _func(PID* this) _x _func; })

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

typedef struct	PID			PID;
typedef enum	PID_PARAM	PID_PARAM;

/************************  Function Declarations ***************************/

extern const struct PID_CLASS
{

	PID* 			(* const new)(MOTOR* mot, float Kp, float Ki, float Kd, float Ts);
	void 			(* const del)(PID* this);
	void            (* const operate)(PID* this);
	void            (* const operate_v2)(PID* this);
	void            (* const operate_v3)(PID* this);

} pid;

/*****************************    Constructs   *****************************/

// should match across MCU & CLI
enum PID_PARAM
{
	PID_KP,			// proportional gain
	PID_KI,			// integrator gain
	PID_KD,			// derivative gain
	PID_N,			// filter constant
	PID_I,			// gain index
	PID_CLAMP,		// anti-windup clamp
};

struct PID
{
	MOTOR* mot;

	float Kp;
	float Ki;
	float Kd;
	float Ts;
	float Tf;
	float N;

	float r[3];
	float e[2];
	float v[3];
	float u;
	float y[3];

	float b;
	float c;

	bool  antiwindup;
	bool  clamp;
	bool  Ki_en;
	float sat_max;
	float sat_min;
};

/*****************************    Constructs   *****************************/
