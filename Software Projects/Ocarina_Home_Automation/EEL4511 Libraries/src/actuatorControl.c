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
#include "../include/actuatorControl.h"
#include "DSP2833x_EPwm_defines.h"


	/*
	 * remember to push the ePWM defines into the github fldr
	 */


// initialize the public variables

/*
 *  Enable the configuration for the PWM peripherals
 */
	void HRPWM1_Config( int period, float dutyDiv)
	{
	// ePWM1 register configuration with HRPWM
	// ePWM1A toggle low/high with MEP control on Rising edge
	    EALLOW;
	    EPwm1Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;           // set Immediate load
	    EPwm1Regs.TBPRD = period-1;                         // PWM frequency = 1 / period
	    EPwm1Regs.CMPA.half.CMPA = period / dutyDiv;            // set duty 50% initially
	    EPwm1Regs.CMPA.half.CMPAHR = (1 << 8);              // initialize HRPWM extension
	    EPwm1Regs.CMPB = period / dutyDiv;                      // set duty 50% initially
	    EPwm1Regs.TBPHS.all = 0;
	    EPwm1Regs.TBCTR = 0;

	    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;             // EPWM1 is the Master
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
	    EPwm1Regs.HRCNFG.bit.EDGMODE = HR_REP;              //MEP control on Rising edge
	    EPwm1Regs.HRCNFG.bit.CTLMODE = HR_CMP;
	    EPwm1Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;
	    EDIS;
	}


//	while (updateCycle ==1)
//	   {
//	        for(dutyCycle =1; dutyCycle <256 ;dutyCycle ++)
//	        {
//	            // Example, write to the HRPWM extension of CMPA
//	            EPwm1Regs.CMPA.half.CMPAHR = dutyCycle << 8;     // Left shift by 8 to write into MSB bits
//	            EPwm2Regs.CMPA.half.CMPAHR = dutyCycle << 8;     // Left shift by 8 to write into MSB bits
//
//	            // Example, 32-bit write to CMPA:CMPAHR
//	            EPwm3Regs.CMPA.all = ((Uint32)EPwm3Regs.CMPA.half.CMPA << 16) + (dutyCycle << 8);
//	            EPwm4Regs.CMPA.all = ((Uint32)EPwm4Regs.CMPA.half.CMPA << 16) + (dutyCycle << 8);
//
//	            DELAY_US(20000); // Dummy delay between MEP changes
//			}
//	   }
	void enableGpio(void) {

	    EALLOW;
	    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;
	    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;
	    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;
	    GpioDataRegs.GPADAT.bit.GPIO0 = 1;
	    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;
	    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;
	    GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;
	    GpioDataRegs.GPADAT.bit.GPIO1 = 1;
	    EDIS;
	}

