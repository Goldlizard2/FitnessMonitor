// Author: Nissa Absalom And Okoko Anainga
// Last modified: 26/3/2022
// Project: ENCE361 project
// Description: This is a main file used for testing the "Display" module
//              as well as providing the beginnings of milestone 1.
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <driverlib/adc.h>
#include <driverlib/interrupt.h>
#include <driverlib/sysctl.h>
#include <driverlib/systick.h>
#include <headers/acc.h>
#include <headers/ADC.h>
#include <headers/buttons4.h>
#include <headers/Display.h>
#include <inc/hw_memmap.h>
#include <../OrbitOLED/lib_OrbitOled/delay.h>
#include <sys/_stdint.h>

#define SYSTICK_RATE_HZ 100
#define UPPERTHRESHOLD 60
#define LOWERTHRESHOLD 35
uint8_t longPressFlag = 0;
uint8_t shortPressFlag = 0;
uint8_t stepFlag = 0;
uint8_t congratsFLag = 1;

void initClock(void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
    SYSCTL_XTAL_16MHZ);
}

void SysTickIntHandler(void)
{

    ADCProcessorTrigger(ADC0_BASE, 3);
    static int32_t pressCount = 0;
    static uint16_t tickCount = 0;
    uint8_t button;
    tickCount++;
    updateButtons();
    updateSwitches();
    bufferFlag = 1;

    if (tickCount == 50)
    {
        tickCount = 0;
        displayFlag = 1;
    }

    if (tickCount % 5 == 0)
    {
        stepFlag = 1;
    }

    button = checkButton(DOWN);
    if (button == PUSHED || pressCount != 0)
    {
        pressCount++;
    }
    if (button == RELEASED && pressCount < 100)
    {
        shortPressFlag = 1;
        pressCount = 0;
    }
    if (pressCount >= 100)
    {
        longPressFlag = 1;
    }
    if (button == RELEASED && pressCount >= 100)
    {
        longPressFlag = 0;
        pressCount = 0;

    }
}

void initSysTick(void)
{
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SYSTICK_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

int main(void)
{
    int32_t magnitude = 0;
    initClock();
    initAccl();
    DelayInit();
    InitDisplay();
    initButtons();
    initADC();
    initSysTick();
    referenceorientation(&rollRef, &pitchRef);
    UpdateDisplay();
    uint8_t aboveThreshHold = 0;


    IntMasterEnable();

    while (1)
    {

        if (checkSwitch(RIGHTSW) == SWUP)
        {
            if (checkButton(UP) == PUSHED)
            {
                stepCount += 100;
                SetView(viewState);
            }
            if (shortPressFlag)
            {
                if (stepCount < 500)
                {
                    stepCount = 0;

                }
                else
                {
                    stepCount -= 500;
                }

                SetView(viewState);
                shortPressFlag = 0;
            }
        }
        else
        {
            if (viewState == 2 && shortPressFlag)
            {
                goalStepCount = adccircbuffermeancalculator();
                viewState = SetView(0);
                UpdateDisplay();
                shortPressFlag = 0;
            }
            if (viewState != 2 && longPressFlag)
            {
                LongPressStart();
                LongPressEnd();
                stepCount = 0;
                congratsFLag = 1;
            }
            if (checkButton(UP) == PUSHED)
                SwitchUnits();
            if (stepFlag)
            {
                circbuffermeancalculator(&mean_x, &mean_y, &mean_z);

                magnitude = addStep();
                if (magnitude > UPPERTHRESHOLD && aboveThreshHold == 0)
                {
                    stepCount += 1;
                    aboveThreshHold = 1;
                }

                if (magnitude < LOWERTHRESHOLD)
                    aboveThreshHold = 0;

                stepFlag = 0;


            }
        }

        if (stepCount == goalStepCount && congratsFLag)
        {
            Congratulations();
            congratsFLag = 0;
        }

        if (checkButton(LEFT) == PUSHED)
            PrevView();

        if (checkButton(RIGHT) == PUSHED)
            NextView();

        if (bufferFlag)
        {
            writebuffer();
            bufferFlag = 0;
        }

        if (displayFlag)
        {
            UpdateDisplay();
            displayFlag = 0;
        }
    }
}

