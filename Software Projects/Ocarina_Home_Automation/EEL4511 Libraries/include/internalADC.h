#ifndef INTERNALADC_H_
#define INTERNALADC_H_

void internalADC_init() {
	InitAdc(); // TI function for ADC calibration
	System_printf("INITADC\n");
	System_flush();
	EALLOW;
	SysCtrlRegs.HISPCP.all = 0x03; // make high speed clock equal to 25Mh for ADC
	AdcRegs.ADCTRL1.bit.CONT_RUN = 0; // no freerun mode off
	AdcRegs.ADCTRL1.bit.ACQ_PS = 0;  // prescaler is 1 for bits 8-0 all 0 no multiplier
	AdcRegs.ADCTRL1.bit.CPS = 0;
	//AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 0;
	AdcRegs.ADCTRL2.bit.EXT_SOC_SEQ1 = 0;
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 0;
	//AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ = 0; // turn this off //
	//AdcRegs.ADCTRL2.bit.INT_MOD_SEQ2 = 0; // this off too //
	//AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 4; // sequence 4 ADC channels at once
	//AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 1; // conversion begin until after

	// new stuff from e2e T//
	 //AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
	 AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;   // Enable software trigger for SEQ1( In separate instructions)

	 //AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;  // Enable SEQ1 interrupt (every EOS)

	  // AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ2 = 1;// Enable SOCB from ePWM to start SEQ2
	 //AdcRegs.ADCTRL2.bit.RST_SEQ2 = 1;         // Reset SEQ1
	 //AdcRegs.ADCTRL2.bit.SOC_SEQ2 = 1; //  // Enable software trigger for SEQ2 ( in separate instructions)

	 //AdcRegs.ADCTRL2.bit.INT_ENA_SEQ2 = 1;  // Enable SEQ2 interrupt (every EO
	 EDIS;
	 
	 // example to grab data: q = (AdcRegs.ADCRESULT0)


     AdcRegs.ADCTRL1.bit.SUSMOD = 00;  // Emulation Suspend mode ignored
     AdcRegs.ADCTRL1.bit.ACQ_PS = 5;  //  5 ADCCLK periods
     AdcRegs.ADCTRL1.bit.CPS = 0;          //  divide by 1
    // AdcRegs.ADCTRL1.bit.CONT_RUN = 1;  // Start Stop mode
     AdcRegs.ADCTRL1.bit.SEQ_OVRD = 0;  //Sequencer Override Disabled
     AdcRegs.ADCTRL1.bit.SEQ_CASC = 0;   // Cascaded mode enabled

     //AdcRegs.ADCTRL2.bit.INT_ENA_SEQ2 = 1;  // Enable SEQ2 interrupt (every EO


     AdcRegs.ADCTRL3.bit.SMODE_SEL = 1;  //Set to Simultaneous sampling mode

     AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 10;  // set so that A0 BO, A1 B1, A2 B2, A3 B3, A4 B4 are
   //  sampled

     AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0 ;  // set these 5 fields so that the order of ADC channels
     AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 1;  // sampled is A0,B0,A1,B1,A2,B2,A3,B3,A4,B4.
     AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 2;
     AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 3;
     //AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 4;
    }

    #endif /*INTERNALADC_H_ */
