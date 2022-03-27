// Author: Nissa Absalom
// Last modified: 26/3/2022
// Project: ENCE361 project
// Description: This is a main file used for testing the "Display" module
//              as well as providing the beginnings of milestone 1.
#include <stdint.h>
#include "lib_OrbitOled/delay.h"
#include "Display.h"
#include "buttons4.h"

// Stand-ins for actual accelerometer data
int16_t X;
int16_t Y;
int16_t Z;

int main(void)
  {
    // init vars
    X = 0;
    Y = 0;
    Z = 0;

    // init modules
    DelayInit();
    InitDisplay(&X, &Y, &Z);
    initButtons();

    // init interrupt that drives the buttons
    SysTickIntRegister(updateButtons);
    SysTickPeriodSet(SysCtlClockGet() / 100); // set systick to trigger at 100 Hz
    SysTickIntEnable();
    SysTickEnable();

    while (1)
    {
        // provide changing data to make spotting bugs easier
        Y -= 1;
        Z += 3;

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
        UpdateDisplay();
        DelayMs(10);
    }
}
