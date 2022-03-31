// Author: Nissa Absalom
// Last modified: 26/3/2022
// Project: ENCE361 project
// Description: Displays data for milestone 1.
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <headers/acc.h>
#include "utils/ustdlib.h"
#include "../OrbitOLED/lib_OrbitOled/delay.h"
#include "../OrbitOled/OrbitOLEDInterface.h"
#include "headers/Display.h"

#define VIEW_COUNT 3
#define LSB_PER_G 256
#define GEE 9.81

// current view number and state
uint8_t viewNumber;
bool showOrientation;

// pointers to accelerometer data
int32_t* XDataPtr;
int32_t* YDataPtr;
int32_t* ZDataPtr;
int32_t* PitchDataPtr;
int32_t* RollDataPtr;
int32_t* PitchRefPtr;
int32_t* RollRefPtr;

// sets pointers, draws static display elements and inits OLED
void InitDisplay(int32_t* XPtr, int32_t* YPtr, int32_t* ZPtr, int32_t* pitchPtr, int32_t* rollPtr, int32_t* pitchRefPtr, int32_t* rollRefPtr)
{
    viewNumber = 0;
    showOrientation = false;
    OLEDInitialise();

    XDataPtr = XPtr;
    YDataPtr = YPtr;
    ZDataPtr = ZPtr;
    PitchDataPtr = pitchPtr;
    RollDataPtr = rollPtr;
    PitchRefPtr = pitchRefPtr;
    RollRefPtr = rollRefPtr;
}

// goes to next view
int NextView()
{
    return SetView(viewNumber + 1);
}

// goes to previous view
int PrevView()
{
    if (viewNumber == 0)
    {
        return SetView(VIEW_COUNT - 1);
    }
    else
    {
        return SetView(viewNumber - 1);
    }
}

// goes to the requested view and blanks the changing parts of the
// display to avoid chars from previous view showing on the current one
int SetView(uint8_t newView)
{
    viewNumber = newView % VIEW_COUNT;

    return viewNumber;
}

// displays the data in raw units from the accelerometer
void displayRaw()
{
    char str[10];

    OLEDStringDraw("raw  ", 11, 0);

    usnprintf(str, sizeof(str), "%d", *XDataPtr);
    OLEDStringDraw(str, 3, 1);

    usnprintf(str, sizeof(str), "%d", *YDataPtr);
    OLEDStringDraw(str, 3, 2);

    usnprintf(str, sizeof(str), "%d", *ZDataPtr);
    OLEDStringDraw(str, 3, 3);
}

// displays the data in gees, due to "usnprintf" not taking floats
// the data is stored in an upper part that is the units place and upwards
// and a lower part that is the thousandths place to the tens place
void displayGees()
{
    char str[10];
    int32_t* data[3];
    data[0] = XDataPtr;
    data[1] = YDataPtr;
    data[2] = ZDataPtr;

    OLEDStringDraw("g    ", 11, 0);

    int8_t i;
    for (i = 0; i < 3; i++)
    {
        uint32_t GeeUpper = abs(*data[i] / LSB_PER_G);
        uint32_t GeeLower = abs(*data[i] * 1000 / LSB_PER_G % 1000);

        if (*data[i] >= 0)
        {
            usnprintf(str, sizeof(str), "%d.%03d", GeeUpper, GeeLower);
        }
        else
        {
            usnprintf(str, sizeof(str), "-%d.%03d", GeeUpper, GeeLower);
        }

        OLEDStringDraw(str, 3, i + 1);
    }
}

// displays the data in m/s^2, due to "usnprintf" not taking floats
// the data is stored in an upper part that is the units place and upwards
// and a lower part that is the thousandths place to the tens place
void displaySI()
{
    char str[10];
    int32_t* data[3];
    data[0] = XDataPtr;
    data[1] = YDataPtr;
    data[2] = ZDataPtr;

    OLEDStringDraw("m/s^2", 11, 0);

    int8_t i;
    for (i = 0; i < 3; i++)
    {
        uint32_t SIUpper = abs(*data[i] * GEE / LSB_PER_G);
        uint32_t SILower = abs((uint32_t)(*data[i] * 9.81 * 1000) / LSB_PER_G % 1000);

        if (*data[i] >= 0)
        {
            usnprintf(str, sizeof(str), "%d.%03d", SIUpper, SILower);
        }
        else
        {
            usnprintf(str, sizeof(str), "-%d.%03d", SIUpper, SILower);
        }

        OLEDStringDraw(str, 3, i + 1);
    }
}

void displayPitchRoll()
{
    char str[10];
    int32_t* data[4];
    data[0] = RollRefPtr;
    data[1] = PitchRefPtr;
    //data[2] = RollRefPtr;
    //data[3] = RollDataPtr;

    int8_t i;
    for (i = 0; i < 2; i++)
    {
        uint32_t angleUpper = abs(*data[i] / 1000);
        uint32_t angleLower = abs(*data[i] % 1000);

        if (*data[i] >= 0)
        {
            usnprintf(str, sizeof(str), "%d.%03d", angleUpper, angleLower);
        }
        else
        {
            usnprintf(str, sizeof(str), "-%d.%03d", angleUpper, angleLower);
        }

        OLEDStringDraw(str, 7, i+1);
    }
}

// dpending on the current viewnumber display the appropriate format
void UpdateDisplay()
{
    if (showOrientation)
    {
        OLEDStringDraw("Ref Orintation: ", 0, 0);
        OLEDStringDraw("Ref R:   ", 0, 1);
        OLEDStringDraw("Ref P:   ", 0, 2);
        OLEDStringDraw("                ", 0, 3);
        // OLEDStringDraw("Cur R:         ", 0, 3);
        displayPitchRoll();
        DelayMs(1100);
        showOrientation = 0;

    }
    else
    {
        OLEDStringDraw("Acc units: ", 0, 0);
        OLEDStringDraw("X:           ", 0, 1);
        OLEDStringDraw("Y:           ", 0, 2);
        OLEDStringDraw("Z:           ", 0, 3);

        switch (viewNumber)
            {
            case 0:
                displayRaw();
                break;
            case 1:
                displayGees();
                break;
            default:
                displaySI();
                break;
        }
    }
}
