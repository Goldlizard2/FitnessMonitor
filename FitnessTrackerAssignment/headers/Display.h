// Author: Nissa Absalom
// Last modified: 26/3/2022
// Project: ENCE361 project
// Description: Displays data for milestone 1.
#ifndef DISPLAY_H
#define DISPLAY_H

bool showOrientation;

void InitDisplay(int32_t* XPtr, int32_t* YPtr, int32_t* ZPtr, int32_t* pitchPtr, int32_t* rollPtr, int32_t* pitchRefPtr, int32_t* rollRefPtr);
int NextView();
int PrevView();
int SetView(uint8_t newView);
void UpdateDisplay();

#endif
