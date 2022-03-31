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
    initAccl();
    DelayInit();
    InitDisplay(&mean_x, &mean_y, &mean_z, &pitch, &roll, &pitchRef, &rollRef);
    initButtons();
    initClock ();
    initSysTick();
    showOrientation = 1;
    referenceorientation(&rollRef, &pitchRef);
    UpdateDisplay();
    while (1)
    {
        // use up button to switch to the next state and down button to set orientation and display for 3 seconds
        if (checkButton(0) == PUSHED)
        {
            NextView();
        }

        else if (checkButton(1) == PUSHED)
        {
            showOrientation = 1;
            referenceorientation(&rollRef, &pitchRef);
            UpdateDisplay();
        }

        if (bufferFlag)
        {
            writebuffer();
            bufferFlag = 0;
        }

        circbuffermeancalculator(&mean_x, &mean_y, &mean_z);

        if (displayFlag)
        {
           UpdateDisplay();
           displayFlag = 0;
        }
    }
}

