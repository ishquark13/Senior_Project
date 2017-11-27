#ifndef INTERNALADC_H_
#define INTERNALADC_H_

void internalADC_init() {
	System_printf("INITADC\n");
	System_flush();
	EALLOW;
	SysCtrlRegs.HISPCP.all = ADC_MODCLK; // make high speed clock equal to 25Mh for ADC
	InitAdc(); // TI function for ADC calibration

    AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;
    AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 0;        // 1  Cascaded mode
    AdcRegs.ADCMAXCONV.all = 0x0003;
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x1;
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x2;
    AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x3;
     //AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 4;
    AdcRegs.ADCTRL3.bit.SMODE_SEL = 0;  //Set to Simultaneous sampling mode
   	AdcRegs.ADCREFSEL.bit.REF_SEL = 0;  // Internal reference used
   	AdcRegs.ADCTRL2.all = 0x2000; // start the conversion

    }

void adcRead(void) {
	while(Adc.ADCST.bit.INT_SEQ1 == 0);
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
	DELAY_US(100);
	AdcRegs.ADCTRL2.all = 0x2000;
}    
    #endif /*INTERNALADC_H_ */
