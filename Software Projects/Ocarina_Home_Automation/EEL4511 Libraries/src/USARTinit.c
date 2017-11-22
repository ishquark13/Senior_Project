#include "../include/USARTinit.h"
#include <DSP28x_Project.h>

/***********************************FUNCTIONS*********************************/


void outChar(char c){
  while(ScibRegs.SCICTL2.bit.TXRDY != 1);//wait until ready
  ScibRegs.SCITXBUF = c;
}

void outWord(char* word){
  for(char c = *word; c; c= *(++word))
    outChar(c);
}

void outPayload(char * payload, int len)
{
    for(int i = 0 ; i != len; i++)
        outChar(payload[i]);
}

void startUSART() {

	EALLOW;

	//SCITXDC - SCI-C transmit (O)
	//SCIRXDC - SCI-C receive (I/O)

	GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 0x3;

	GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 0x3;

	SysCtrlRegs.PCLKCR0.bit.SCICENCLK = 0x1;

	ScicRegs.SCICCR.all = 0x7; //set 8 bits, 1 stop bit, no parity and idle line mode.
	ScicRegs.SCILBAUD = 0xE7; //Set Baud using Equation
	ScicRegs.SCIHBAUD = 0x01; // 9600 rate


	ScicRegs.SCICTL1.bit.TXENA = 1;
	ScicRegs.SCICTL1.bit.RXENA = 1;
	ScicRegs.SCICTL1.bit.SWRESET = 1;

	SysCtrlRegs.LOSPCP.all = 0x1;

	/* copied */

	//Interupt Enabling code
	/*
	DINT;

	ScibRegs.SCICTL2.bit.RXBKINTENA = 1;
	GpioDataRegs.GPADAT.bit.GPIO0 = 1;
	InitPieCtrl();

	IER = 0x0000;
	IFR = 0x0000;

	InitPieVectTable();

	EALLOW;
	PieVectTable.SCIRXINTB = &inCharISR;

	//enable PIE
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;

	PieCtrlRegs.PIEIER9.bit.INTx3=1;     // PIE Group 9, INT3
	IER = 0x100;	// Enable CPU INT
	EINT;*/


}
