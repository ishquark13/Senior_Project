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

#include "fpu_rfft.h"

/*
 * ======= Defines =======
 */
#define RFFT_STAGES     10
#define RFFT_SIZE       (1 << RFFT_STAGES) //512 FFT Bins
#define F_PER_SAMPLE    6000.0L/(float)RFFT_SIZE  //Internal sampling rate is 6kHz
#define FFT_THRESHOLD 30000    //TODO: EDIT THIS LATER
#define D5 60
#define F5 72
#define A5 90
#define B5 101
#define C6 107
#define D6 120

/*
 * ======== Pragmas =======
 */
#pragma DATA_SECTION( gRawADCBuffer , "PINGPONG" )
#pragma DATA_SECTION( gAudioBuffer , "PINGPONG" )
#pragma DATA_ALIGN( gRawADCBuffer, 2048 )

#pragma DATA_SECTION( SongofTime , ".econst" )
#pragma DATA_SECTION( WindsRequiem , ".econst" )
#pragma DATA_SECTION( SariasSong , ".econst" )
#pragma DATA_SECTION( SongofStorms , ".econst" )
#pragma DATA_SECTION( SongofPassing , ".econst" )
#pragma DATA_SECTION( hamming,".econst")


/*
 * ======= Global Variables ========
 */
volatile Uint32 gRawADCBuffer[BUFFER_SIZE] = {0}; //contains raw 32bit data from ADC->McBSPb via DMA (data is shifted left) (Initialized to zero)
volatile float gAudioBuffer[BUFFER_SIZE] = {0};  //will contain correctly shifted ADC data after processing occurs (Initialized to zero)
const Uint32 gGarbage = 0x0BADFADE;
volatile Uint16 gNoteDetected = 0;
volatile Uint16 hwicount = 0;
volatile float hamming[BUFFER_SIZE];            //added hamming window vector

RFFT_F32_STRUCT rfft;
float RFFToutBuff[RFFT_SIZE];                   //Calculated FFT result
float RFFTF32Coef[RFFT_SIZE];                   //Coefficient table buffer
float RFFTmagBuff[RFFT_SIZE/2+1];               //Magnitude of frequency spectrum
struct FLAGS_STRUCT
{
    enum SwapSignal{READY, BLOCKED}BufferSwapSignal;
    enum NoteDetected{SILENCE, DETECTED}soundFlag;
    enum RepeatNote{NEW,SAME}noteFlag;
}gFlags;

const Uint16 SongofTime[6] = {A5, D5, F5, A5, D5, F5};
const Uint16 WindsRequiem[6] = {C6, D5, A5 , C6, D5, A5};
const Uint16 SariasSong[6] = {F5, A5, B5 , F5, A5, B5};
const Uint16 SongofStorms[6] = {D5, F5, D6 , D5, F5, D6};
const Uint16 SongofPassing[6] = {A5, F5, D6 , A5, F5, D6};

volatile Uint16 noteBuffer[6]= {0};

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
    //hwicount++;

    if ( gFlags.BufferSwapSignal == BLOCKED )
        gFlags.BufferSwapSignal = READY;
    else if (gFlags.BufferSwapSignal == READY)
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

    //char adcval[10];

    while(1)
    {
        //TODO: Copy & Convert 32 bit data to 16 bit and Implement Correct Scheduling technique
        //pend on  semaphore post()
        Semaphore_pend(BufferSwapSemaphore, BIOS_WAIT_FOREVER);

        System_printf("BufferSwapTask() running \n");
        System_flush();

        Hwi_disable();

        for(int i = 0; i < BUFFER_SIZE; i++)
            {
                gAudioBuffer[i] = (float) ( (gRawADCBuffer[i] >> 2) & 0xFFFF); /** hamming[i];*/
            }

        //TODO: Uncomment Hamming Window

        gFlags.BufferSwapSignal = BLOCKED;

        Hwi_enable();
        Semaphore_post(BufferProcessingSemaphore);

        //sprintf(adcval, "%10u", gAudioBuffer[512]);
        //LCD_string(adcval)  ;
        //LCD_home();
        //System_flush();


    }
}

/* ======= BufferProcessingTask ========
 * This Function implements the FFT processing on the data Buffer and signals the BufferSwapTask to run when finished.
 */

void BufferProcessingTask(UArg a0, UArg a1)
{

    System_printf("enter BufferProcessingTask()\n");
    System_flush();

    //fft setup

    char noteVal[10] = {0};
    Uint16 fftMaxMagBin = 0;
    float fftMaxMag = 0;
    float fftMaxMagFreq = 0;



    RFFT_f32_sincostable(&rfft);                    //Calculate twiddle factor

    //Clean up output buffer
    for (int i = 0; i < RFFT_SIZE; i++)
       {
            RFFToutBuff[i] = 0;
       }

   //Clean up magnitude buffer
   for (int i = 0; i < RFFT_SIZE/2 + 1; i++)
       {
            RFFTmagBuff[i] = 0;
       }


   while(1)
   {
       Semaphore_pend(BufferProcessingSemaphore, BIOS_WAIT_FOREVER);
       System_printf("BufferProcessingTask() running\n");
       System_flush();

       RFFT_f32u(&rfft);                 //calculate rfft on unaligned data (CHECK FOR OPTIMIZATION USING ALIGHT RFFT FUNCTION)
       RFFT_f32_mag(&rfft);                 //calculate rfft

       fftMaxMagBin = 1;
       fftMaxMag = RFFTmagBuff[1];

       for(int i=2;i<RFFT_SIZE/2;i++)
       {
               if(RFFTmagBuff[i] > fftMaxMag)
               {
                   fftMaxMagBin = i;
                   fftMaxMag = RFFTmagBuff[i];
               }
       }

       fftMaxMagFreq =  F_PER_SAMPLE * (float)fftMaxMagBin; //Convert normalized digital frequency to analog frequency

       //TODO: what happens if same note is played for a long time????
       if(fftMaxMag > FFT_THRESHOLD && gFlags.soundFlag==SILENCE)
       {
           gFlags.soundFlag = DETECTED;
           gFlags.noteFlag = NEW;
       }
       else if(fftMaxMag > FFT_THRESHOLD && gFlags.soundFlag == DETECTED)
       {
           if(fftMaxMagBin == noteBuffer[0])
               gFlags.noteFlag = SAME;
           else
               gFlags.noteFlag = NEW;

       }
       else if(fftMaxMag < FFT_THRESHOLD )
       {
           LCD_clear();
           sprintf(noteVal, " No Note");
           LCD_string(noteVal);
           gFlags.soundFlag = SILENCE;
       }

       if(gFlags.noteFlag == NEW && gFlags.soundFlag == DETECTED)
       {
           noteBuffer[5] = noteBuffer[4];
           noteBuffer[4] = noteBuffer[3];
           noteBuffer[3] = noteBuffer[2];
           noteBuffer[2] = noteBuffer[1];
           noteBuffer[1] = noteBuffer[0];
           noteBuffer[0] = fftMaxMagBin;
           sprintf(noteVal, "%10f", fftMaxMagFreq);
           LCD_home();
           LCD_string(noteVal);
           LCD_pos(1, 0);
           sprintf(noteVal, "%3u ,", fftMaxMagBin);
           LCD_string(noteVal);
           sprintf(noteVal, " %3f", fftMaxMag);
           LCD_string(noteVal);

           Semaphore_post(SongCompareSemaphore);
       }

       if ( gFlags.BufferSwapSignal == BLOCKED )
           gFlags.BufferSwapSignal = READY;
       else if (gFlags.BufferSwapSignal == READY)
           Semaphore_post(BufferSwapSemaphore);

   }


}

void SongCompareTask(UArg a0, UArg a1)
{
    System_printf("Starting SongCompare Task\n");
    System_flush();

    while(1)
    {
        Semaphore_pend(SongCompareSemaphore,BIOS_WAIT_FOREVER);

        System_printf("Running SongCompare Task\n");

        System_flush();


        //TODO: Compare


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

    //Initialize DMACH1&2
    Init_DMA();

    System_printf("enter DMA init\n");
    System_flush();

    //Initialize Timer0
    Init_Timer0();

    //Init_FFT();
    rfft.FFTSize   = RFFT_SIZE;                     //Real FFT size
    rfft.FFTStages = RFFT_STAGES;                   //Real FFT stages
    rfft.InBuf     = (float*)&gAudioBuffer[0];      //Input buffer
    rfft.OutBuf    = &RFFToutBuff[0];               //Output buffer
    rfft.MagBuf    = &RFFTmagBuff[0];               //Magnitude output buffer
    rfft.CosSinBuf = &RFFTF32Coef[0];               //Twiddle factor

    System_printf("Finished inits, entering BIOS_start()\n");
    System_flush();

    //Hwi_enableInterrupt(38); //Timer0 Overflow
    //Hwi_enableInterrupt(74); //McBSPb Recieve Interrupt

    gFlags.BufferSwapSignal = READY;


    BIOS_start();    /* does not return */
    return(0);
}

void Init_Timer0(void)
{
    EALLOW;
    InitCpuTimers();

    ConfigCpuTimer( &CpuTimer0 , 150 , 166); //150MHz SYSCLK and 100us Timer Period (6k Sa/s)

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
