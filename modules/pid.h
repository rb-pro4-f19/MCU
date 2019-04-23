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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <assert.h>

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/************************  Function Declarations ***************************/

extern struct PID_CLASS
{

	PID* 			(* const new)(float Kp, float Ki, float Kd, float N);
	void 			(* const del)(PID* this);

} pid;

/*****************************    Constructs   *****************************/

struct PID
{

	float Kp;
	flaot Ki;
	float Kd;
	float N;
	float U[2];
	float E[2];

};

/*****************************    Constructs   *****************************/
