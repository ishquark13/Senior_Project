/*
 * actuatorControl.c
 *
 *  Created on: Oct 14, 2017
 *      Author: Ish's Master Race PC
												 *         /\
														  /  \
														 /    \
														/      \
													   /        \
													  /__________\
													 /\__________/\
													/  \        /  \
												   /    \      /    \
												  /      \    /      \
												 /        \  /        \
												/__________\/__________\
												\__________/\__________/
 */

#include <DSP28x_Project.h>
#include <DSP2833x_SysCtrl.h>
#include <DSP2833x_GlobalPrototypes.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "actuatorControl.h"
#include "DSP2833x_EPwm_defines.h"

int main(void) {
	/*
	 * remember to push the ePWM defines into the github fldr
	 */

	DisableDog();
	InitPll(10,2);
	InitSysCtrl();

// using the EPWM module on GPIO 0 and GPIO 1
	InitEPwm1Gpio();
// disable CPU interrupts
	DINT;
// initialize the PIE vector table to default settings
	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
//  populate table to all interrupts vectors
	InitPieVectTable();
// initialize the public variables
	int updateCycle = 1;
	int dutyCycle   = 0;
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0; // sysclk /10 to pwm
	EDIS;

/*
 *  Enable the configuration for the PWM peripherals
 */
	HRPWM1_Config(10000, 2); // duty cycle at 2 divides for 50%, 3 is 66%, 4 75% etc.. or float between 1-2 for inverted cycle
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;

	while (updateCycle ==1)
	   {
	        for(dutyCycle =1; dutyCycle <256 ;dutyCycle ++)
	        {
	            // Example, write to the HRPWM extension of CMPA
	            EPwm1Regs.CMPA.half.CMPAHR = dutyCycle << 8;     // Left shift by 8 to write into MSB bits
	            EPwm2Regs.CMPA.half.CMPAHR = dutyCycle << 8;     // Left shift by 8 to write into MSB bits

	            // Example, 32-bit write to CMPA:CMPAHR
	            EPwm3Regs.CMPA.all = ((Uint32)EPwm3Regs.CMPA.half.CMPA << 16) + (dutyCycle << 8);
	            EPwm4Regs.CMPA.all = ((Uint32)EPwm4Regs.CMPA.half.CMPA << 16) + (dutyCycle << 8);

	            DELAY_US(20000); // Dummy delay between MEP changes
			}
	   }
	}

