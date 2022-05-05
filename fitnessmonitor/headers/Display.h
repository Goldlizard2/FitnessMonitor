// Author: Nissa Absalom
// Last modified: 26/3/2022
// Project: ENCE361 project
// Description: Displays data for milestone 1.
#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdio.h>
#include <stdbool.h>

// current view number and state
uint8_t viewState;
int32_t stepCount;
int32_t goalStepCount;
int32_t newGoalStepCount;


void InitDisplay();
int NextView();
int PrevView();
int SetView(uint8_t newView);
void SwitchUnits();
void LongPressStart();
void LongPressEnd();
void UpdateDisplay();

#endif
