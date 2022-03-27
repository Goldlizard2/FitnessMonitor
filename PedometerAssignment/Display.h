// Author: Nissa Absalom
// Last modified: 26/3/2022
// Project: ENCE361 project
// Description: Displays data for milestone 1.
#ifndef DISPLAY_H
#define DISPLAY_H

void InitDisplay(int16_t* XPtr, int16_t* YPtr, int16_t* ZPtr);
int NextView();
int PrevView();
int SetView(uint8_t newView);
void UpdateDisplay();

#endif
