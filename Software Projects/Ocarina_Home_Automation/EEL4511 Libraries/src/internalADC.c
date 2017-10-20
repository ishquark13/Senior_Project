#include <DSP28x_Project.h>
#include "../include/internalADC.h"



void internalADC_init() {
	InitAdc();
	EALLOW;
	SysCtrlRegs.HISPCP.all = 0x03; // make high speed clock equal to 25Mh for ADC
	AdcRegs.ADCTRL1.bit.CONT_RUN = 1; // no freerun mode off
	AdcRegs.ADCTRL1.bit.ACQ_PS = 0;  // prescaler is 1 for bits 8-0 all 0 no multiplier
	AdcRegs.ADCTRL1.bit.CPS = 0;
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 0;
	AdcRegs.ADCTRL2.bit.EXT_SOC_SEQ1 = 0;
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 0;
	//AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ = 0; // turn this off //
	//AdcRegs.ADCTRL2.bit.INT_MOD_SEQ2 = 0; // this off too //
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0;
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0; // conversion begin until after

	// new stuff from e2e T//
	 AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
	 AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;   // Enable software trigger for SEQ1( In separate instructions)

	 AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;  // Enable SEQ1 interrupt (every EOS)

	  // AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ2 = 1;// Enable SOCB from ePWM to start SEQ2
	 AdcRegs.ADCTRL2.bit.RST_SEQ2 = 1;         // Reset SEQ1
	 //AdcRegs.ADCTRL2.bit.SOC_SEQ2 = 1; //  // Enable software trigger for SEQ2 ( in separate instructions)

	 AdcRegs.ADCTRL2.bit.INT_ENA_SEQ2 = 1;  // Enable SEQ2 interrupt (every EO
	 EDIS;
}

Uint16 getAdcData() {
EALLOW;	
return (AdcRegs.ADCRESULT0);
EDIS;
}