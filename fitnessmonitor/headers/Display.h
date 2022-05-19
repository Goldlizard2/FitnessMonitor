// Author: Nissa Absalom and Okoko Anainga
// Last modified: 19/5/2022
// Project: ENCE361 project
// Description: Header file for display module
#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdio.h>
#include <stdbool.h>

// Public vars
int32_t goalStepCount;
uint8_t viewState;
int32_t stepCount;

// Public functions
void InitDisplay();
int SetView(uint8_t newView);
int NextView();
int PrevView();
void SwitchUnits();
void LongPressStart();
void LongPressEnd();
void UpdateDisplay();
void Congratulations();

#endif
