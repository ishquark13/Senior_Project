/*
 *  ======== main.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>

#include <DSP28x_Project.h>

#include "EEL4511 Libraries/include/EEL4511.h"

/*
 * ======== Pragmas =======
 */

#pragma DATA_SECTION( gPingBuf  , "DMARAML4" )
#pragma DATA_SECTION( gPongBuf  , "DMARAML4" )
#pragma DATA_SECTION( gGarbage , "DMARAML5" )


/*
 * ======= Global Variables ========
 */
volatile Uint32 gPingBuf[BUFFER_SIZE] = {0};
volatile Uint32 gPongBuf[BUFFER_SIZE] = {0};
volatile Uint32 gGarbage= 0x0BADFADE;

/*
 *  ======= Function Prototypes =======
 */

void Init_Timer0(void);
void Init_DMA(void);


/*
 *  ======== taskFxn ========
 */
Void taskFxn(UArg a0, UArg a1)
{
    System_printf("enter taskFxn()\n");
	
    Task_sleep(10);
	
    System_printf("exit taskFxn()\n");

    System_flush(); /* force SysMin output to console */
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

    // Initialize System Control:
    // PLL(150MHz), WatchDog, enable Peripheral Clocks
    // This example function is found in the DSP2833x_SysCtrl.c file.
    InitSysCtrl();

    // Initialize External ADC via McBSPb
    Init_McBSPb_ADC();
	
    //Initialize LCD over I2C
    Init_LCD();

    //Initialize Timer0
    Init_Timer0();

    BIOS_start();    /* does not return */
    return(0);
}

void Init_Timer0(void)
{
    EALLOW;
    InitCpuTimers();

    ConfigCpuTimer( &CpuTimer0 , 150 , 22); //150MHz SYSCLK and 22us Timer Period

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
    DMACH1AddrConfig( (volatile Uint16*) &McbspbRegs.DXR2 , (volatile Uint16*) &garbage + 1 );
    DMACH1BurstConfig( 1, -1, 1 ); //send 2 words per burst, decrement source addr, increment dest addr by 1 (32 bits little endian)
    DMACH1TransferConfig( 0, 0, 0 );
    DMACH1WrapConfig( 0, 0, 0, 0 );
    DMACH1ModeConfig( DMA_TINT0, 1, 0, 1, 0, 0, 0, 0, 0, 0 );

    //setup DMA2 (triggered on McBSPB RRDY flag)
    DMACH2AddrConfig( (volatile Uint16*) &pong_1[0] + 1 , (volatile Uint16*) &McbspbRegs.DRR2);
    DMACH2BurstConfig( 1, 1, -1 ); //send 2 words per burst, increment source addr, decrement dest addr by 1 (32 bits little endian)
    DMACH2TransferConfig( BUFFER_SIZE - 1, 0, 3 );  //get N = BUFFER_SIZE ADC values, wrap to same McBSP vals, increment Dest by 3
    DMACH2WrapConfig( 0, 0, BUFFER_SIZE - 1, 0 );
    DMACH2ModeConfig(DMA_MREVTB, 1, 0, 1, 0, 0, 0, 0, 1, 1 );

/*  //setup DMA3 ( DAC transfer triggered on Timer1 overflow)
    DMACH3AddrConfig( (volatile Uint16*) &McbspaRegs.DXR1, (volatile Uint16*) &pong_2[0] );
    DMACH3BurstConfig( 0, 0, 0 );
    DMACH3TransferConfig( BUFFER_SIZE - 1, 1, 0 );
    DMACH3WrapConfig( 0xFFFF, 0, 0, 0 );
    DMACH3ModeConfig( DMA_TINT0, 1, 0, 1, 0, 0, 0, 0, 1, 1 );


    EALLOW;
    PieCtrlRegs.PIEIER7.bit.INTx1 = 1; //enable McBSPB RRDY int
    PieCtrlRegs.PIEIER7.bit.INTx2 = 1; //enable DMACH2 int
    PieCtrlRegs.PIEIER7.bit.INTx3 = 1; //enable DMACH2 int
    PieCtrlRegs.PIEIER6.bit.INTx3 = 1; //enable McBSPB RRDY int
    PieCtrlRegs.PIEIFR6.bit.INTx3 = 1;

    PieVectTable.MRINTB = MRB_ISR;
    PieVectTable.DINTCH2 = DMA_CH2;
    PieVectTable.DINTCH3 = DMA_CH3;

    IER |= M_INT6;   //enable McBSPB RRDY interrupt
    IER |= M_INT7;  //enable "buffer full" interrupt DMACH2

*/

    StartDMACH1();
    StartDMACH2();
    StartDMACH3();
}
