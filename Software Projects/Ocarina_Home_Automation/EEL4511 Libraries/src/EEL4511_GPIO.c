/*
 * EEL4511_GPIO.c
 *
 *  Created on: Oct 18, 2016
 *      Author: Adrian
 */
#include<DSP28x_Project.h>
#include "../include/EEL4511_GPIO.h"

void Init_LED(void){
	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO0  = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO1  = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO2  = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO3  = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO4  = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO5  = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO6  = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO7  = 1;

	EDIS;
}

void Init_SWITCH(void){
	EALLOW;

	GpioCtrlRegs.GPAMUX1.bit.GPIO8  = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO9  = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO8   = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO9   = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO10  = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO11  = 0;

	EDIS;
}

Uint16 SWITCH_read(void){

	return ( ((Uint16) GpioDataRegs.GPADAT.all & 0x0F00) >> 8 ) ;

}
