#include "USARTinit.h"
#include <DSP28x_Project.h>

extern volatile char rxBuffer[100];
extern Uint16 rxBufferIndex;

/***********************************INTERRUPTS********************************/

__interrupt void inCharISR(){
  ScibRegs.SCICTL1.bit.SLEEP = 0;
  while(ScibRegs.SCIRXST.bit.RXRDY != 1);//wait until ready

  rxBuffer[rxBufferIndex] = (char)ScibRegs.SCIRXBUF.bit.RXDT;
  rxBufferIndex++;

  PieCtrlRegs.PIEACK.all|=0x100; //a wild interrupt appeared!

}


/***********************************FUNCTIONS*********************************/
void outChar(char c){
  while(ScibRegs.SCICTL2.bit.TXRDY != 1);//wait until ready
  ScibRegs.SCITXBUF = c;
}

void outWord(char* word){
  for(char c = *word; c; c= *(++word))
    outChar(c);
}

void startUSART() {

	EALLOW;

	//SCITXDB - SCI-B transmit (O)
	//SCIRXDB - SCI-B receive (I/O)

	GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 0x3;
	//GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0x2;

	GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 0x3;
	//GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0x2;

	SysCtrlRegs.PCLKCR0.bit.SCIBENCLK = 0x1;

	ScibRegs.SCICCR.all = 0x7; //set 8 bits, 1 stop bit, no parity and idle line mode.
	ScibRegs.SCILBAUD = 0xE7; //Set Baud using Equation
	ScibRegs.SCIHBAUD = 0x01; // 9600 rate


	ScibRegs.SCICTL1.bit.TXENA = 1;
	ScibRegs.SCICTL1.bit.RXENA = 1;
	ScibRegs.SCICTL1.bit.SWRESET = 1;

	SysCtrlRegs.LOSPCP.all = 0x1;

	/* copied */

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
	EINT;


}
