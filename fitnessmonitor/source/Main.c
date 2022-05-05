// Author: Nissa Absalom And Okoko Anainga
// Last modified: 26/3/2022
// Project: ENCE361 project
// Description: This is a main file used for testing the "Display" module
//              as well as providing the beginnings of milestone 1.

#include <driverlib/adc.h>
#include <driverlib/interrupt.h>
#include <driverlib/sysctl.h>
#include <driverlib/systick.h>
#include <headers/acc.h>
#include <headers/ADC.h>
#include <headers/buttons4.h>
#include <headers/Display.h>
#include <inc/hw_memmap.h>
#include <lib_OrbitOled/delay.h>
#include <sys/_stdint.h>

#define SYSTICK_RATE_HZ 100

void
initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}

void
SysTickIntHandler (void)
{

    ADCProcessorTrigger(ADC0_BASE, 3);

    static uint16_t tickCount = 0;
    tickCount ++;
    updateButtons();
    // updateSwitches();
    if (tickCount == 50)
    {

        tickCount = 0;
        displayFlag = 1;

    }
    if (tickCount%10 == 0)
    {
        bufferFlag = 1;
    }

}

void
initSysTick (void)
{
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet (SysCtlClockGet () / SYSTICK_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister (SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable ();
    SysTickEnable ();
}


int main(void)
{
    initClock ();
    initAccl();
    DelayInit();
    InitDisplay();
    initButtons();
    initADC();
    initSysTick();
    referenceorientation(&rollRef, &pitchRef);
    UpdateDisplay();

    IntMasterEnable();

    while (1)
    {
        /*switch (checkButton(DOWN))
        {

        case PUSHED:
            LongPressStart();
            break;
        case RELEASED:
            LongPressEnd();
            break;
        default:
            break;
        }*/
        if (checkSwitch(RIGHTSW) == SWUP){
            if (checkButton(UP) == PUSHED)
                stepCount += 100;
            if (checkButton(DOWN) == PUSHED)
                stepCount -= 500;
        } else {
            if (viewState == 2 && checkButton(DOWN) == PUSHED){
                        goalStepCount = adccircbuffermeancalculator();
                        viewState = 0;
                        UpdateDisplay();
                    }
            if (checkButton(UP) == PUSHED)
                        SwitchUnits();
        }

        if (checkButton(LEFT) == PUSHED)
            PrevView();

        if (checkButton(RIGHT) == PUSHED)
            NextView();

        if (bufferFlag)
        {
            stepCount++;
            bufferFlag = 0;
        }


        if (displayFlag)
        {
           UpdateDisplay();
           displayFlag = 0;
        }
    }
}

