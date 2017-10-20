/*
 * SCI for senior
 */
#include <DSP28x_Project.h>
#include "OneToOneI2CDriver.h"
#include <DSP2833x_Xintf.h>
#include <DSP2833x_XIntrupt.h>
#include "timer.h"
#include "analogToDigitalConverter.h"
#include "digitalToAnalogConverter.h"
#include "DSP2833x_CpuTimers.h"
#include <DSP2833x_SysCtrl.h>
#include "audioCntrl.h"
#include "Sram.h"
#include "digitalToAnalogConverter.h"
#include "hammingWindow.h"
#include "USARTinit.h"
#include <DSP2833x_GlobalPrototypes.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>



//function prototypes
void init_ButtonInterrupt(void);
void Init_Timer1(void);

// methodically guess sizes

//global variables

volatile char rxBuffer[100];
Uint16 rxBufferIndex = 0;

//void USARTenable(){
//
//  //__interrupt void inCharISR();
//  //interruptInit();
//  timerInit();
	//startUSART();
//
//}

__interrupt void Timer1_ISR(void)
{
    EALLOW;
    GpioDataRegs.GPADAT.bit.GPIO0 ^= 1;

}


int main(void){

    DisableDog();
    InitPll(10,2);
    //InitPll(10,2);
    InitSysCtrl();//wtf... check what clocks we actually need
    //GpioDataRegs.GPADAT.bit.GPIO10 = 1; // reset FTDI
    //analogToDigitalConverter_init();
    //digitalToAnalogConverter_init();
    //useGpio();
	EDIS;
	EALLOW;


    startUSART();

    for (Uint32 var  = 0;  var  < 100; var++ )
		rxBuffer[var] = 0x0000;

    outWord("SCI is working?");

    while(1)
    {



    }
}



