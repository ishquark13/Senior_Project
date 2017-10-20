/*
 *  ======== main.c ========
 */

#include <xdc/std.h>

#include <stdio.h>

#include <xdc/cfg/global.h>

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/family/c28/Hwi.h>

#include <DSP28x_Project.h>

#include "EEL4511 Libraries/include/EEL4511.h"

/*
 * ======== Pragmas =======
 */

#pragma DATA_SECTION( gRawADCBuffer , "PINGPONG" )
#pragma DATA_SECTION( gAudioBuffer , "PINGPONG" )
#pragma DATA_ALIGN( gRawADCBuffer, 2048 )


/*
 * ======= Global Variables ========
 */
volatile Uint32 gRawADCBuffer[BUFFER_SIZE] = {0}; //contains raw 32bit data from ADC->McBSPb via DMA (data is shifted left) (Initialized to zero)
volatile Uint16 gAudioBuffer[BUFFER_SIZE] = {0};  //will contain correctly shifted ADC data after processing occurs (Initialized to zero)
volatile Uint32 gGarbage = 0x0BADFADE;
volatile Uint16 hwicount = 0;

/*
 *  ======= Function Prototypes =======
 */

void Init_Timer0(void);
void Init_DMA(void);


/* ======= BufferSwapPostFxn_HWI ========
 * This Function Posts a Semaphore for the BufferSwapFxn_SWI to run
 */

void BufferSwapPostFxn_HWI(void)
{
    //TODO: Check BufferSwapStatus flag and Post Semaphore to BufferSwapFxn_SWI if necessary
    //System_flush();
    hwicount++;
    Semaphore_post(BufferSwapSemaphore);

    //EALLOW;
    //PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;
    //StartDMACH2();
}

/* ======= BufferSwapFxn_SWI ========
 * This Function implements the PingPong Buffer Swap pending on the BufferSwapPost_HWI and FFT_Finished semaphore post.
 */

void BufferSwapTask(UArg a0, UArg a1)
{

    System_printf("enter BufferSwapTask()\n");
    System_flush();

    char timerval[10];
    Uint16 adcH;
    Uint16 adcL;
    Uint16 adc;
    char adcval[10];

    while(1)
    {
        //TODO: Copy & Convert 32 bit data to 16 bit and Implement Correct Scheduling technique
        //pend on  semaphore post()
        Semaphore_pend(BufferSwapSemaphore, BIOS_WAIT_FOREVER);

        Hwi_disable();

        for(int i = 0; i < BUFFER_SIZE; i++)
            {
                gAudioBuffer[i] = (Uint16) ((gRawADCBuffer[i] >> 2) & 0xFFFF);
            }

        Hwi_enable();

        EALLOW;
        //sprintf(timerval, "%10u", (Uint16) CpuTimer0Regs.TIM.all);
        adcH = McbspbRegs.DRR2.all;
        adcL = McbspbRegs.DRR1.all;

        adcH = (adcH << 14) & 0xC000 ;
        adcL &= 0xFFFC;
        adcL = adcL >> 2;

        Uint16 adc = (adcH) | (adcL);
        //adc =  ( (Uint16) McbspbRegs.DRR1.all ) >> 2 ;
        sprintf(adcval, "%10u", gAudioBuffer[512]);
        LCD_string(adcval)  ;
        LCD_home();
        //System_printf("Printed to LCD!!!\n");
        System_flush();
        //StartDMACH2();


    }
}

/*
 *  ======== main ========
 */
Int main()
{ 
    /*
     * use ROV->SysMin to view the characters in the circular buffer
     */
    System_printf("enter main()\n");
    System_flush();

    // Initialize System Control:
    // PLL(150MHz), WatchDog, enable Peripheral Clocks
    // This example function is found in the DSP2833x_SysCtrl.c file.
    InitSysCtrl();

    System_printf("enter mcbsp init\n");
    System_flush();

    // Initialize External ADC via McBSPb
    Init_McBSPb_ADC();
	
    System_printf("enter lcd init\n");
    System_flush();

    //Initialize LCD over I2C
    Init_LCD();

    System_printf("enter timer init\n");
    System_flush();

    //Initialize Timer0
    Init_DMA();

    System_printf("enter DMA init\n");
    System_flush();

    //Initialize DMACH1&2
    Init_Timer0();

    System_printf("Finished inits, entering BIOS_start()\n");
    System_flush();

    //Hwi_enableInterrupt(38); //Timer0 Overflow
    //Hwi_enableInterrupt(74); //McBSPb Recieve Interrupt


    BIOS_start();    /* does not return */
    return(0);
}

void Init_Timer0(void)
{
    EALLOW;
    InitCpuTimers();

    ConfigCpuTimer( &CpuTimer0 , 150 , 100); //150MHz SYSCLK and 100us Timer Period (10k Sa/s)

    //Don't modify PIE for BIOS Applications
    //PieVectTable.XINT13 = <function_name>;  //THIS IS FOR TIMER1 ONLY!!!!
    //PieCtrlRegs.PIECTRL.bit.ENPIE = 1;      //THIS IS FOR TIMER1 ONLY!!!!

    //Don't modify IER Register for BIOS Applications
    //IER |= M_INT13;                         //THIS IS FOR TIMER1 ONLY!!!!

    //BIOS_start() will handle global interrupt enable
    //EINT;

    //Enable the timer
    StartCpuTimer0();
}

void Init_DMA(void)
{
    EALLOW;

    SysCtrlRegs.HISPCP.all = 0;                 //Hi-Speed Clk = SYSCLK
    SysCtrlRegs.PCLKCR3.bit.DMAENCLK = 1;       // DMA Clock

    DMAInitialize();

    //setup DMA1 (McBSPb (ADC) transfer triggered on Timer1 overflow)
    DMACH1AddrConfig( (volatile Uint16*) &McbspbRegs.DXR2 , (volatile Uint16*) &gGarbage + 1 );
    DMACH1BurstConfig( 1, -1, 1 ); //send 2 words per burst, decrement source addr, increment dest addr by 1 (32 bits little endian)
    DMACH1TransferConfig( 0, 0, 0 );
    DMACH1WrapConfig( 0, 0, 0, 0 );
    DMACH1ModeConfig( DMA_TINT0, 1, 0, 1, 0, 0, 0, 0, 0, 0 );

    //setup DMA2 (triggered on McBSPB RRDY flag)
    DMACH2AddrConfig( (volatile Uint16*) &gRawADCBuffer[0] + 1 , (volatile Uint16*) &McbspbRegs.DRR2);
    DMACH2BurstConfig( 1, 1, -1 ); //send 2 words per burst, increment source addr, decrement dest addr by 1 (32 bits little endian)
    DMACH2TransferConfig( BUFFER_SIZE - 1 , 0, 3 );  //get N = BUFFER_SIZE ADC values, wrap to same McBSP vals, increment Dest by 3
    DMACH2WrapConfig( 0, 0, BUFFER_SIZE - 1 , 0 );
    DMACH2ModeConfig(DMA_MREVTB, 1, 0, 1, 0, 0, 0, 0, 1, 1 );

/*  //setup DMA3 ( DAC transfer triggered on Timer1 overflow)
    DMACH3AddrConfig( (volatile Uint16*) &McbspaRegs.DXR1, (volatile Uint16*) &pong_2[0] );
    DMACH3BurstConfig( 0, 0, 0 );
    DMACH3TransferConfig( BUFFER_SIZE - 1, 1, 0 );
    DMACH3WrapConfig( 0xFFFF, 0, 0, 0 );
    DMACH3ModeConfig( DMA_TINT0, 1, 0, 1, 0, 0, 0, 0, 1, 1 );
*/

    EALLOW;
    //PieCtrlRegs.PIEIER7.bit.INTx1 = 1; //enable DMACH1  int
    //PieCtrlRegs.PIEIER7.bit.INTx2 = 1; //enable DMACH2 int
    //PieCtrlRegs.PIEIER7.bit.INTx3 = 1; //enable DMACH3 int
    PieCtrlRegs.PIEIER6.bit.INTx3 = 1; //enable McBSPB RRDY int

    //PieVectTable.MRINTB = MRB_ISR;
    //PieVectTable.DINTCH2 = DMA_CH2;
    //PieVectTable.DINTCH3 = DMA_CH3;

    IER |= M_INT6;   //enable McBSPB RRDY interrupt
    //IER |= M_INT7;  //enable "buffer full" interrupt DMACH2



    StartDMACH1();
    StartDMACH2();
    //StartDMACH3();
}
