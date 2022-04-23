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

void InitDisplay(uint32_t* stepCountIn, uint32_t* goalStepCountIn, uint32_t* newGoalStepCountIn);
int NextView();
int PrevView();
int SetView(uint8_t newView);
void SwitchUnits();
void LongPressStart();
void LongPressEnd();
void UpdateDisplay();

#endif
