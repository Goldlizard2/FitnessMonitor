// Author: Nissa Absalom
// Last modified: 26/3/2022
// Project: ENCE361 project
// Description: This is a main file used for testing the "Display" module
//              as well as providing the beginnings of milestone 1.
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "utils/ustdlib.h"
#include "../OrbitOLED/lib_OrbitOled/delay.h"
#include "headers/Display.h"
#include "headers/buttons4.h"
#include "headers/acc.h"


int main(void)
{
    uint32_t stepCount = 0;
    uint32_t goalStepCount = 1000 * 2;
    uint32_t newGoalStepCount = 42069;

    initAccl();
    DelayInit();
    InitDisplay(&stepCount, &goalStepCount, &newGoalStepCount);
    initButtons();
    initClock ();
    initSysTick();
    referenceorientation(&rollRef, &pitchRef);
    UpdateDisplay();

    while (1)
    {
        switch (checkButton(0))
        {
        case PUSHED:
            LongPressStart();
            break;
        case RELEASED:
            LongPressEnd();
            break;
        default:
            break;
        }

        if (checkButton(1) == PUSHED)
            SwitchUnits();

        if (checkButton(2) == PUSHED)
            PrevView();

        if (checkButton(3) == PUSHED)
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

