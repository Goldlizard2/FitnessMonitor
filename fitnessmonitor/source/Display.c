// Author: Nissa Absalom and Okoko Anainga
// Last modified: 19/5/2022
// Project: ENCE361 project
// Description: This is file handles interfacing with the Orbit OLED Display
#include <headers/ADC.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "headers/acc.h"
#include "driverlib/adc.h"
#include "utils/ustdlib.h"
#include "headers/buttons4.h"
#include "../OrbitOLED/lib_OrbitOled/delay.h"
#include "../OrbitOLED/OrbitOLEDInterface.h"
#include "headers/Display.h"

// Constants
#define VIEW_COUNT 3
#define DISPLAY_WIDTH 16
#define CM_PER_STEP 90
#define MILE_PER_DM 62

// Macro of spaces that fully cover the display
#define BLANK_DISPLAY "                                                                "

// Private var used to store which unit mode the display is in
// kM vs miles and steps vs %
bool unitMode;

// Initalises vars and the OLED
void InitDisplay()
{
    viewState = 0;
    unitMode = false;
    goalStepCount = 1000;
    OLEDInitialise();
}

// Goes to the requested view and blanks the display
int SetView(uint8_t newView)
{
    viewState = newView % VIEW_COUNT;

    OLEDStringDraw(BLANK_DISPLAY, 0, 0);

    return viewState;
}

// Goes to next view
int NextView()
{
    return SetView(viewState + 1);
}

// Goes to previous view
int PrevView()
{
    if (viewState == 0)
    {
        return SetView(VIEW_COUNT - 1);
    }
    else
    {
        return SetView(viewState - 1);
    }
}

// Displays Two horizontal lines on the top and bottom lines
// and several vertical lines on the middle two lines
void LongPressStart()
{
    OLEDStringDraw("----------------||||||||||||||||||||||||||||||||----------------", 0, 0);
}

// Blanks the display
void LongPressEnd()
{
    OLEDStringDraw(BLANK_DISPLAY, 0, 0);
}

// In units of steps displays both the total and goal
void displaySteps()
{
    char str[DISPLAY_WIDTH]; // Defines an array of chars the width of the display

    OLEDStringDraw("Unit: Steps", 0, 0); // Displays current unit

    usnprintf(str, sizeof(str), "Total: %d", stepCount); // Creates a char array that displays the total number of steps
    OLEDStringDraw(str, 0, 1); // Displays prev char array

    usnprintf(str, sizeof(str), "Goal : %d ", goalStepCount); // Creates a char array that displays the goal number of steps
    OLEDStringDraw(str, 0, 2); // Displays prev char array
}

// Displays the percentage of goal
void displayPercent()
{
    char str[DISPLAY_WIDTH]; // Defines an array of chars the width of the display

    uint32_t persentSteps2DP = stepCount * (10000 / goalStepCount); // Calculates percent times 100

    OLEDStringDraw("Unit: Percent", 0, 0); // Displays current unit

    usnprintf(str, sizeof(str), "Total: %d.%02d", persentSteps2DP / 100, persentSteps2DP % 100); // Crates a char array that displays the percent, accurate to 2DP
    OLEDStringDraw(str, 0, 1); // Displays prev char array
}

// In units of km displays both the total and goal
void displayKMs()
{
    char str[DISPLAY_WIDTH]; // Defines an array of chars the width of the display

    uint32_t metersSteps = stepCount * CM_PER_STEP / 100; // Calculates total distance in meters
    uint32_t metersGoal = goalStepCount * CM_PER_STEP / 100; // Calculates goal distance in meters

    OLEDStringDraw("Unit: kilometres", 0, 0); // Displays current unit

    usnprintf(str, sizeof(str), "Total: %d.%03d", metersSteps / 1000, metersSteps % 1000); // Crates a char array that displays the total in km, accurate to 3DP
    OLEDStringDraw(str, 0, 1); // Displays prev char array

    usnprintf(str, sizeof(str), "Goal : %d.%03d", metersGoal / 1000, metersGoal % 1000); // Crates a char array that displays the goal in km, accurate to 3DP
    OLEDStringDraw(str, 0, 2); // Displays prev char array
}

// In units of miles displays both the total and goal
void displayMiles()
{
    char str[DISPLAY_WIDTH]; // Defines an array of chars the width of the display

    uint32_t milimilesSteps = stepCount * CM_PER_STEP * MILE_PER_DM / 10000; // Calculates total distance in mili-miles
    uint32_t milimilesGoal = goalStepCount * CM_PER_STEP * MILE_PER_DM / 10000; // Calculates goal distance in mili-miles

    OLEDStringDraw("Unit: miles", 0, 0); // Displays current unit

    usnprintf(str, sizeof(str), "Total: %d.%03d", milimilesSteps / 1000, milimilesSteps % 1000); // Crates a char array that displays the total in miles, accurate to 3DP
    OLEDStringDraw(str, 0, 1); // Displays prev char array

    usnprintf(str, sizeof(str), "Goal : %d.%03d", milimilesGoal / 1000, milimilesGoal % 1000); // Crates a char array that displays the goal in miles, accurate to 3DP
    OLEDStringDraw(str, 0, 2); // Displays prev char array
}

// Displays the set goal UI
void displaySetGoal()
{
    char str[DISPLAY_WIDTH];

    OLEDStringDraw("Set new goal", 0, 0);

    usnprintf(str, sizeof(str), "Current: %d", goalStepCount);
    OLEDStringDraw(str, 0, 1);
    usnprintf(str, sizeof(str), "New    : %d ", adccircbuffermeancalculator());
    OLEDStringDraw(str, 0, 2);
}

// Up dates the "unitMode" var and blanks the display
void SwitchUnits()
{
    unitMode = !unitMode;
    OLEDStringDraw(BLANK_DISPLAY, 0, 0);
}

// Notifies the user they have completed their goal
void Congratulations()
{
    OLEDStringDraw(BLANK_DISPLAY, 0, 0);
    OLEDStringDraw("CONGRATULATIONS", 1, 0);
    OLEDStringDraw("STEP GOAL", 4, 1);
    OLEDStringDraw("ACHIEVED", 4, 2);
    DelayMs(1100);
    OLEDStringDraw(BLANK_DISPLAY, 0, 0);
}

// Depending on the current viewState and unitMode display the appropriate format
void UpdateDisplay()
{
    if (unitMode)
    {
        switch (viewState)
        {
        case 0:
            displayPercent();
            break;
        case 1:
            displayMiles();
            break;
        default:
            displaySetGoal();
            break;
        }
    }
    else
    {
        switch (viewState)
        {
        case 0:
            displaySteps();
            break;
        case 1:
            displayKMs();
            break;
        default:
            displaySetGoal();
            break;
        }
    }
}
