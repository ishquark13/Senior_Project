/*
 * EEL4511_MISC.c
 *
 *  Created on: Oct 17, 2016
 *      Author: Adrian
 */

#include<DSP28x_Project.h>
#include "../include/EEL4511_MISC.h"

Uint16 float2word(float number)
{
	return ( (Uint16) ( (number / 3.3) * (0xFFFF) ) );
}

float word2float(Uint16 value)
{
	return ( 3.3 * (((float) value) / 0xFFFF ));

}

void keypad_init(void)
{
    EALLOW;

    //set as gpio
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;

    //I/O
    GpioCtrlRegs.GPADIR.bit.GPIO12 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO13 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO14 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO15 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO16 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO17 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO18 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO19 = 0;

    //pull up disable
    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO13 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO15 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;


}

char getKey(void)
{
    EALLOW;
    char key = 0x00;

    Uint16 value = 0;

    GpioDataRegs.GPASET.all = 0x000F000; //clear gpio bits
    GpioDataRegs.GPACLEAR.bit.GPIO12 = 1;
    DELAY_US(1);
    value = ((GpioDataRegs.GPADAT.all >> 12) & 0xFF);
    if( value == 0x7E) {
        key ='*';
    }
    else if( value == 0xBE) {
        key ='0';
    }
    else if( value == 0xDE) {
        key ='#';
    }
    else if( value == 0xEE) {
        key ='D';
    }

    GpioDataRegs.GPASET.all = 0x000F000; //clear gpio bits
    GpioDataRegs.GPACLEAR.bit.GPIO13 = 1;
    DELAY_US(1);
    value = ((GpioDataRegs.GPADAT.all >> 12) & 0xFF);
    if( value == 0x7D) {
        key ='7';
        }
    else if( value == 0xBD) {
        key ='8';
    }
    else if( value == 0xDD) {
        key ='9';
    }
    else if( value == 0xED) {
        key ='C';
        }

    GpioDataRegs.GPASET.all = 0x000F000; //clear gpio bits
    GpioDataRegs.GPACLEAR.bit.GPIO14 = 1;
    DELAY_US(1);
    value = ((GpioDataRegs.GPADAT.all >> 12) & 0xFF);
    if( value == 0x7B) {
        key ='4';
    }
    else if( value == 0xBB) {
        key ='5';
    }
    else if( value == 0xDB) {
        key ='6';
    }
    else if( value == 0xEB) {
        key ='B';
    }

    GpioDataRegs.GPASET.all = 0x000F000; //clear gpio bits
    GpioDataRegs.GPACLEAR.bit.GPIO15 = 1;
    DELAY_US(1);
    value = ((GpioDataRegs.GPADAT.all >> 12) & 0xFF);
    if( value == 0x77) {
        key ='1';
    }
    else if( value == 0xB7) {
        key ='2';
    }
    else if( value == 0xD7) {
        key ='3';
    }
    else if( value == 0xE7) {
        key ='A';
    }

    GpioDataRegs.GPASET.all = 0x000F000;
    return key;
}
