// Author: Nissa Absalom
// Last modified: 26/3/2022
// Project: ENCE361 project
// Description: This is a main file used for testing the "Display" module
//              as well as providing the beginnings of milestone 1.
#include <stdint.h>
#include <stdbool.h>
#include "../OrbitOLED/lib_OrbitOled/delay.h"
#include "headers/Display.h"
#include "headers/buttons4.h"
#include "headers/acc.h"

static int32_t mean_x;
static int32_t mean_y;
static int32_t mean_z;

int main(void)
  {
    initAccl();
    DelayInit();
    InitDisplay(&mean_x, &mean_y, &mean_z);
    initButtons();
    initClock ();
    initSysTick();

    while (1)
    {

        // use up and down buttons to switch to the next and previous views, respectivly
        if (checkButton(0) == PUSHED)
        {
            NextView();
        }
        else if (checkButton(1) == PUSHED)
        {
            PrevView();
        }

        // update the display and set max loop speed

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

