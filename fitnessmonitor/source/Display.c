// Author: Nissa Absalom
// Last modified: 26/3/2022
// Project: ENCE361 project
// Description: Displays data for milestone 1.
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <headers/acc.h>
#include "utils/ustdlib.h"
#include "lib_OrbitOled/delay.h"
#include "OrbitOLEDInterface.h"
#include "headers/Display.h"

#define VIEW_COUNT 3
#define DISPLAY_WIDTH 16
#define CM_PER_STEP 90
#define MILE_PER_DM 62

bool unitMode;
bool longPress;

// pointers to accelerometer data
uint32_t* stepCount;
uint32_t* goalStepCount;
uint32_t* newGoalStepCount;


// sets pointers, draws static display elements and inits OLED
void InitDisplay(uint32_t* stepCountIn, uint32_t* goalStepCountIn, uint32_t* newGoalStepCountIn)
{
    viewState = 0;
    unitMode = false;
    longPress = false;

    OLEDInitialise();

    stepCount = stepCountIn;
    goalStepCount = goalStepCountIn;
    newGoalStepCount = newGoalStepCountIn;
}

// goes to next view
int NextView()
{
    return SetView(viewState + 1);
}

// goes to previous view
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

// goes to the requested view and blanks the changing parts of the
// display to avoid chars from previous view showing on the current one
int SetView(uint8_t newView)
{
    viewState = newView % VIEW_COUNT;

    OLEDStringDraw("                                                                ", 0, 0);

    return viewState;
}

void LongPressStart()
{
    OLEDStringDraw("----------------||||||||||||||||||||||||||||||||----------------", 0, 0);
    longPress = true;
}

void LongPressEnd()
{
    OLEDStringDraw("                                                                ", 0, 0);
    longPress = false;
}

// displays the data in raw units from the accelerometer
void displaySteps()
{
    char str[DISPLAY_WIDTH];

    OLEDStringDraw("Unit: Steps", 0, 0);

    usnprintf(str, sizeof(str), "Total: %d", *stepCount);
    OLEDStringDraw(str, 0, 1);
    usnprintf(str, sizeof(str), "Goal : %d ", *goalStepCount);
    OLEDStringDraw(str, 0, 2);
}

void displayPercent()
{
    char str[DISPLAY_WIDTH];
    uint32_t persentSteps2DP = *stepCount * (10000 / *goalStepCount);

    OLEDStringDraw("Unit: Percent", 0, 0);

    usnprintf(str, sizeof(str), "Total: %d.%02d", persentSteps2DP / 100, persentSteps2DP % 100);
    OLEDStringDraw(str, 0, 1);
}

void displayKMs()
{
    char str[DISPLAY_WIDTH];
    uint32_t metersSteps = *stepCount * CM_PER_STEP / 100;
    uint32_t metersGoal = *goalStepCount * CM_PER_STEP / 100;

    OLEDStringDraw("Unit: kilometres", 0, 0);

    usnprintf(str, sizeof(str), "Total: %d.%03d", metersSteps / 1000, metersSteps % 1000);
    OLEDStringDraw(str, 0, 1);
    usnprintf(str, sizeof(str), "Goal : %d.%03d", metersGoal / 1000, metersGoal % 1000);
    OLEDStringDraw(str, 0, 2);
}

void displayMiles()
{
    char str[DISPLAY_WIDTH];
    uint32_t milimilesSteps = *stepCount * CM_PER_STEP * MILE_PER_DM / 10000;
    uint32_t milimilesGoal = *goalStepCount * CM_PER_STEP * MILE_PER_DM / 10000;

    OLEDStringDraw("Unit: miles", 0, 0);

    usnprintf(str, sizeof(str), "Total: %d.%03d", milimilesSteps / 1000, milimilesSteps % 1000);
    OLEDStringDraw(str, 0, 1);
    usnprintf(str, sizeof(str), "Goal : %d.%03d", milimilesGoal / 1000, milimilesGoal % 1000);
    OLEDStringDraw(str, 0, 2);
}

void displaySetGoal()
{
    char str[DISPLAY_WIDTH];

    OLEDStringDraw("Set new goal", 0, 0);

    usnprintf(str, sizeof(str), "Current: %d", *goalStepCount);
    OLEDStringDraw(str, 0, 1);
    usnprintf(str, sizeof(str), "New    : %d ", *newGoalStepCount);
    OLEDStringDraw(str, 0, 2);
}

void SwitchUnits()
{
    unitMode = !unitMode;
    OLEDStringDraw("                                                                ", 0, 0);
}

// dpending on the current viewnumber display the appropriate format
void UpdateDisplay()
{
    if (longPress)
        return;

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
