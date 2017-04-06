#include "DCL.h"
#define PID_DEFAULTS { 1.0f, \
0.0f, \
0.0f, \
1.0f, \
0.0f, \
0.0f, \
0.0f, \
0.0f, \
0.0f, \
0.0f, \
1.0f, \
-1.0f \
}


/*
 * main.c
 */

/*
 * TODO: call the PID CONTROLLER FUNCTION INSIDE A FAST INTERRUPT:
 *
 */
int main(void) {
	PID pid1 = PID_DEFAULTS;
	
	typedef volatile struct {
	float Kp; // proportional gain
	float Ki; // integral gain
	float Kd; // derivative gain
	float Kr; // set point weight
	float c1; // D filter coefficient 1
	float c2; // D filter coefficient 2
	float d2; // D filter storage 1
	float d3; // D filter storage 2
	float i10; // I storage
	float i14; // sat storage
	float Umax; // upper saturation limit
	float Umin; // lower saturation limit
	} PID;



	return 0;
}
