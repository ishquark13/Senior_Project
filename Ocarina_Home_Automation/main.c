/*
 *  ======== main.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>

#include <DSP28x_Project.h>

#include <EEL4511.h>

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

    BIOS_start();    /* does not return */
    return(0);
}
