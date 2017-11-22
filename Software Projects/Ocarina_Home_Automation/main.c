/*
 *  ======== main.c ========
 */

#include <xdc/std.h>

#include <stdio.h>

#include <string.h>

#include <xdc/cfg/global.h>

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/family/c28/Hwi.h>

#include <DSP28x_Project.h>

#include "EEL4511 Libraries/include/EEL4511.h"

#include "EEL4511 Libraries/include/internalADC.h"

#include "EEL4511 Libraries/include/actuatorControl.h"

#include "fpu_rfft.h"

/*
 * ======= Defines =======
 */
#define RFFT_STAGES     10
#define RFFT_SIZE       (1 << RFFT_STAGES) //512 FFT Bins
#define F_PER_SAMPLE    6000.0L/(float)RFFT_SIZE  //Internal sampling rate is 6kHz
#define FFT_THRESHOLD 30000    //TODO: EDIT THIS LATER
#define D5 100
#define F5 119
#define A5 150
#define B5 168
#define C6 178
#define D6 201

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
//#pragma DATA_SECTION( hamming,".econst")


/*
 * ======= Global Variables ========
 */
volatile Uint32 gRawADCBuffer[BUFFER_SIZE] = {0}; //contains raw 32bit data from ADC->McBSPb via DMA (data is shifted left) (Initialized to zero)
volatile float gAudioBuffer[BUFFER_SIZE] = {0};  //will contain correctly shifted ADC data after processing occurs (Initialized to zero)
const Uint32 gGarbage = 0x0BADFADE;
volatile Uint16 gNoteDetected = 0;
volatile Uint16 hwicount = 0;
Uint16 updateCycle  = 0; //changed to 1 when the pwm is to be altered
Uint16 dutyCycle    = 0; //iterates to 256 writing to the registers
float fanPwm = 1.5; // TODO: delete this and replace with global struct

RFFT_F32_STRUCT rfft;
float RFFToutBuff[RFFT_SIZE];                   //Calculated FFT result
float RFFTF32Coef[RFFT_SIZE];                   //Coefficient table buffer
float RFFTmagBuff[RFFT_SIZE/2+1];               //Magnitude of frequency spectrum

/*
 *  FLAGS_STRUCT: class that determines the status of the note detecion pattern/algorithm
 *  with enum types od boolean flags
 */
struct FLAGS_STRUCT
{
    enum SwapSignal{READY, BLOCKED}BufferSwapSignal;
    enum NoteDetected{SILENCE, DETECTED}soundFlag;
    enum RepeatNote{NEW,SAME}noteFlag;
    enum KnownNote{UNKNOWN,KNOWN}knownFlag;
}gFlags;

/*
 * TINGLES_MAP: struct to encompass the individual states of the sensors and actuators
 */
volatile struct TINGLES_MAP
{
    char packetStart[5];   //data initialization for the ESP wifi module
    char noteFound;         //post to pi the current note
    char songFound;         //post to pi the current song
    Uint16 fanState;        //post to pi status of fan
    float fanPwm;           //default begin duty-cycle
    Uint16 fanPercent;      //post the calculated duty-cycle of the fan
    Uint16 tempVal;         //ADCREG0 reading for the temp
    char lightState;        //post to pi the status of the LEDs
    Uint16 cdsVal;          //ADCREG1 reading for light
    char pumpState;         //on or off for pump
    Uint16 pressureVal;     //ADCREG2 for pressure sensor
    char lockState;         //post to pi lock on or off
    char magnetVal;         //ADCREG3 for magnet
    char packetEnd[4] ;     //packet to end the socket cnxn
}gTinglesMap;

const Uint16 SongofTime[3] = {F5, D5, A5};
const Uint16 WindsRequiem[3] = {A5, D5, C6};
const Uint16 SariasSong[3] = {B5, A5, F5};
const Uint16 SongofStorms[3] = {D6, F5, D5};
const Uint16 SongofPassing[3] = {D6, F5, A5};

volatile Uint16 gNoteBuffer[3]= {0};

float hamming[BUFFER_SIZE];

/*
 *  ======= Function Prototypes =======
 */

void Init_Timer0(void);
void Init_DMA(void);
void Init_PWM(void);
void waitAdc(void);

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

        //System_printf("BufferSwapTask() running \n");
        //System_flush();

        Hwi_disable();

        for(int i = 0; i < BUFFER_SIZE; i++)
            {
                gAudioBuffer[i] = (Uint16) ( ( (float) ( (gRawADCBuffer[i] >> 2) & 0xFFFF) ) ); /** hamming[i];*/
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
       //System_printf("BufferProcessingTask() running\n");
       //System_flush();

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

       gFlags.soundFlag = SILENCE;

      /* if(fftMaxMag > FFT_THRESHOLD) && gFlags.soundFlag == SILENCE)
       {
           gFlags.soundFlag = DETECTED;
           gFlags.noteFlag = NEW;
       }
       else*/ if(fftMaxMag > FFT_THRESHOLD)// && gFlags.soundFlag == DETECTED)
       {
           if ( (fftMaxMagBin < (D5 + 2) ) && (fftMaxMagBin > (D5 - 2) ) )
           {
               fftMaxMagBin = D5;
               gFlags.knownFlag = KNOWN;
               fanPwm -= .1;
               HRPWM1_Config(10000,fanPwm);
              // test the while loop for pwm

              for(dutyCycle =1; dutyCycle<256; dutyCycle++) {
                  EPwm1Regs.CMPA.half.CMPAHR = dutyCycle << 8;
                  EPwm2Regs.CMPA.half.CMPAHR = dutyCycle << 8;
              }

           }
           else if ( (fftMaxMagBin < (F5 + 2) ) && (fftMaxMagBin > (F5 - 2) ) )
           {
               fftMaxMagBin = F5;
               gFlags.knownFlag = KNOWN;
           }
           else if ( (fftMaxMagBin < (A5 + 2) ) && (fftMaxMagBin > (A5 - 2) ) )
           {
               fftMaxMagBin = A5;
               gFlags.knownFlag = KNOWN;
           }
           else if ( (fftMaxMagBin < (B5 + 2) ) && (fftMaxMagBin > (B5 - 2) ) )
           {
               fftMaxMagBin = B5;
               gFlags.knownFlag = KNOWN;

           }
           else if ( (fftMaxMagBin < (C6 + 2) ) && (fftMaxMagBin > (C6 - 2) ) )
           {
               fftMaxMagBin = C6;

                gFlags.knownFlag = KNOWN;
                fanPwm += .1;
                HRPWM1_Config(10000,fanPwm);
               // test the while loop for pwm

               for(dutyCycle =1; dutyCycle<256; dutyCycle++) {
                   EPwm1Regs.CMPA.half.CMPAHR = dutyCycle << 8;
                   EPwm2Regs.CMPA.half.CMPAHR = dutyCycle << 8;

               }
           }
           else if ( (fftMaxMagBin < (D6 + 2) ) && (fftMaxMagBin > (D6 - 2) ) )
           {
               fftMaxMagBin = D6;
               gFlags.knownFlag = KNOWN;
               waitAdc();
               System_printf("ADC RAW SAUCE %u \n",AdcRegs.ADCRESULT0);
               System_printf("ADC RAW SAUCE2 %u \n",AdcRegs.ADCRESULT2);
               System_printf("ADC RAW SAUCE3 %u \n",AdcRegs.ADCRESULT4);
               System_flush();
           }

           if(fftMaxMagBin == gNoteBuffer[0])
               gFlags.noteFlag = SAME;
           else
               gFlags.noteFlag = NEW;

           gFlags.soundFlag = DETECTED;

       }

       if(gFlags.noteFlag == NEW && gFlags.knownFlag == KNOWN && gFlags.soundFlag == DETECTED)
       {
           gNoteBuffer[2] = gNoteBuffer[1];
           gNoteBuffer[1] = gNoteBuffer[0];
           gNoteBuffer[0] = fftMaxMagBin;
          // sprintf(noteVal, "%10f", fftMaxMagFreq);
          // LCD_home();
         // LCD_string(noteVal);
         //  LCD_pos(1, 0);
         //  sprintf(noteVal, "%3u ,", fftMaxMagBin);
        //   LCD_string(noteVal);
        //   sprintf(noteVal, " %3f", fftMaxMag);
        //   LCD_string(noteVal);

           Semaphore_post(SongCompareSemaphore);
       }

       gFlags.knownFlag = UNKNOWN;

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

        //compare gNoteBuffer against all known songs

        if(memcmp( SongofTime, (const void *) gNoteBuffer , sizeof(SongofTime) ) == 0 )
        {
            System_printf("Song of Time found!\n");
            System_flush();

            //code to shut everything down

            memset( (void *) gNoteBuffer, 0, sizeof(gNoteBuffer));
        }
        else if(memcmp( WindsRequiem, (const void *) gNoteBuffer , sizeof(WindsRequiem) ) == 0 )
        {
            System_printf("Winds Requiem found!\n");
            System_flush();

            //code to activate fan + read temp sensor

            memset( (void *) gNoteBuffer, 0, sizeof(gNoteBuffer));
        }
        else if(memcmp( SariasSong, (const void *)  gNoteBuffer , sizeof(SariasSong) ) == 0 )
        {
            System_printf("Sarias Song found!\n");
            System_flush();

            System_printf("ADC RAW SAUCE %u \n",AdcRegs.ADCRESULT0 >> 4);
            System_printf("ADC RAW SAUCE2 %u \n",AdcRegs.ADCRESULT1 >> 4);
            System_printf("ADC RAW SAUCE3 %u \n",AdcRegs.ADCRESULT2 >>4);
            System_flush();



            if (AdcRegs.ADCRESULT0 > 2000) {

                System_flush();
                HRPWM1_Config(10000,fanPwm);
                // test the while loop for pwm

                for(dutyCycle =1; dutyCycle<256; dutyCycle++) {
                    EPwm1Regs.CMPA.half.CMPAHR = dutyCycle << 8;
                    EPwm2Regs.CMPA.half.CMPAHR = dutyCycle << 8;

                }
            }


            //code to open door and check mag sensor

            memset( (void *) gNoteBuffer, 0, sizeof(gNoteBuffer));
        }
        else if(memcmp( SongofStorms, (const void *)  gNoteBuffer , sizeof(SongofStorms) ) == 0 )
        {
            System_printf("Song of Storms found!\n");
            System_flush();

            //code to water plants and check pressure sensor


            memset( (void *) gNoteBuffer, 0, sizeof(gNoteBuffer));
        }
        else if(memcmp( SongofPassing, (const void *)  gNoteBuffer , sizeof(SongofPassing) ) == 0 )
        {
            System_printf("Song of Passing found!\n");
            System_flush();

            //code to toggle leds and check CdS cell

            memset( (void *) gNoteBuffer, 0, sizeof(gNoteBuffer));
        }
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

    //System_printf("enter lcd init\n");
    //System_flush();

    //Initialize LCD over I2C
   // Init_LCD();

    System_printf("enter DMA init\n");
    System_flush();

    //Initialize DMACH1&2
    Init_DMA();

    System_printf("enter timer init\n");
    System_flush();

    //Initialize Timer0
    Init_Timer0();

    System_printf("enter EPWM init\n");
    System_flush();

    //Initialize ePWM1
    Init_PWM();

    System_printf("enter ADC init\n");
    System_flush();

    //Initialize internal ADC
    internalADC_init();

    System_printf("Initialize variables\n");
    System_flush();

    rfft.FFTSize   = RFFT_SIZE;                     //Real FFT size
    rfft.FFTStages = RFFT_STAGES;                   //Real FFT stages
    rfft.InBuf     = (float*)&gAudioBuffer[0];      //Input buffer
    rfft.OutBuf    = &RFFToutBuff[0];               //Output buffer
    rfft.MagBuf    = &RFFTmagBuff[0];               //Magnitude output buffer
    rfft.CosSinBuf = &RFFTF32Coef[0];               //Twiddle factor

    /*initialize TinglesMap
    gTinglesMap.packetStart[0] = 0x01;
    gTinglesMap.packetStart[1] = 0x02;
    gTinglesMap.packetStart[2] = 0x03;
    gTinglesMap.packetStart[3] = 0x04;
    gTinglesMap.packetEnd[0]   = 0x05;
    gTinglesMap.packetEnd[1]   = 0x06;
    gTinglesMap.packetEnd[2]   = 0x07;
    gTinglesMap.packetEnd[3]   = 0x08;*/
    char * val = "start";
    strcpy(&gTinglesMap.packetStart, val);
    val = "stop";
    strcpy(&gTinglesMap.packetEnd, val);

    gFlags.BufferSwapSignal = READY;

    System_printf("Finished inits, entering BIOS_start()\n");
    System_flush();

    //Hwi_enableInterrupt(38); //Timer0 Overflow
    //Hwi_enableInterrupt(74); //McBSPb Recieve Interrupt

    System_printf("size of gTinklesMap = %d\n", sizeof(gTinglesMap));
    System_flush();


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

void Init_PWM(void){
    InitEPwm1Gpio();
       EALLOW;
       SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;
       SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC  = 1;
       //HRPWM1_Config(10000,2); // @params second val changes each time cycle is altered
       EDIS;

       //Initialize GPIO2 and GPIO3
       EALLOW;
       GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;
       GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;
       GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
       GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;
       EDIS;

}

void waitAdc(void) {
    AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;
    while(!AdcRegs.ADCST.bit.SEQ1_BSY);
}

