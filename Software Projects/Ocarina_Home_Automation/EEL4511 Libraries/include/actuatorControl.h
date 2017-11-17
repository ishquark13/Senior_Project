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
void HRPWM1_Config( int period, float dutyDiv)
{
// ePWM1 register configuration with HRPWM
// ePWM1A toggle low/high with MEP control on Rising edge

	EPwm1Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;	        // set Immediate load
	EPwm1Regs.TBPRD = period-1;		                    // PWM frequency = 1 / period
	EPwm1Regs.CMPA.half.CMPA = period / dutyDiv;	    	// set duty 50% initially
    EPwm1Regs.CMPA.half.CMPAHR = (1 << 8);              // initialize HRPWM extension
	EPwm1Regs.CMPB = period / dutyDiv;	                    // set duty 50% initially
	EPwm1Regs.TBPHS.all = 0;
	EPwm1Regs.TBCTR = 0;

	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;		        // EPWM1 is the Master
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
	EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;

	EPwm1Regs.AQCTLA.bit.ZRO = AQ_CLEAR;                // PWM toggle low/high
	EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;
	EPwm1Regs.AQCTLB.bit.ZRO = AQ_CLEAR;
	EPwm1Regs.AQCTLB.bit.CBU = AQ_SET;

	EALLOW;
	EPwm1Regs.HRCNFG.all = 0x0;
	EPwm1Regs.HRCNFG.bit.EDGMODE = HR_REP;				//MEP control on Rising edge
	EPwm1Regs.HRCNFG.bit.CTLMODE = HR_CMP;
	EPwm1Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;
	EDIS;
}

void enableGpio(void) {

	EALLOW;
	GpioCtrlRegs.GPAPUD.bit.GPIO24 = 1;
	GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO24 = 1;
	GpioDataRegs.GPADAT.bit.GPIO24 = 1;
	EDIS;
}




#endif /* ACTUATORCONTROL_H_ */
