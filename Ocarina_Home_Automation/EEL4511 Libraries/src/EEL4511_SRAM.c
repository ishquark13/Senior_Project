/*
 * EEL4511_SRAM.c
 *
 *  Created on: Oct 17, 2016
 *      Author: Adrian
 */

#include<DSP28x_Project.h>
#include "../include/EEL4511_SRAM.h"


void Init_SRAM(void)
{
	EALLOW;

	XintfRegs.XINTCNF2.bit.XTIMCLK= 0; //150 MHz
	XintfRegs.XINTCNF2.bit.CLKMODE = 0; //XCLK @ SysClk/1

	XintfRegs.XTIMING0.bit.XWRTRAIL = 1;
	XintfRegs.XTIMING0.bit.XWRACTIVE = 0;
	XintfRegs.XTIMING0.bit.XWRLEAD = 1;
	XintfRegs.XTIMING0.bit.XRDTRAIL = 1;
	XintfRegs.XTIMING0.bit.XRDACTIVE = 0;
	XintfRegs.XTIMING0.bit.XRDLEAD = 1;

	GpioCtrlRegs.GPAMUX2.all |= 0xFC000000;
	GpioCtrlRegs.GPBMUX1.all |= 0xFFFFFCF0;
	GpioCtrlRegs.GPCMUX1.all |= 0xFFFFFFFF;
	GpioCtrlRegs.GPCMUX2.all |= 0xFFFF;
	SysCtrlRegs.PCLKCR3.bit.XINTFENCLK = 1;

	EDIS;
}
