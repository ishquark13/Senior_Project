/*
 * actuatorControl.h
 *
 *  Created on: Oct 17, 2017
 *      Author: Ish's Master Race PC
 */

#ifndef ACTUATORCONTROL_H_
#define ACTUATORCONTROL_H_
#include "DSP2833x_EPwm_defines.h"
#include <DSP28x_Project.h>

// declare prototypes
void HRPWM1_Config(int a, float b);
// declare public variables

extern Uint16 i, j, n, dutyCycle, newCycle;
extern Uint32 tempCycle;

// user functions

void enableGpio(void);




#endif /* ACTUATORCONTROL_H_ */
