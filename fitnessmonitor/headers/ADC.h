#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdbool.h>
#include "headers/CircularBuffer.h"
circBuf_t buffer_adc;
int32_t mean_adc;

void
ADCIntHandler(void);

void
initADC (void);

int32_t adccircbuffermeancalculator();


#endif /*ADC_H*/
