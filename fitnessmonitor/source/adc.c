#include <headers/ADC.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "headers/Display.h"
#include "../headers/CircularBuffer.h"
#include "../OrbitOLED/OrbitOLEDInterface.h"

//*****************************************************************************
// Constants
//*****************************************************************************
#define BUFFER_SIZE 10
#define SAMPLE_RATE_HZ 10


void
ADCIntHandler(void)
{
    uint32_t ulValue;

    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
    writeCircBuf (&buffer_adc, ulValue);
    ADCIntClear(ADC0_BASE, 3);
}

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void
initADC (void)
{
    initCircBuf (&buffer_adc, BUFFER_SIZE);
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
                             ADC_CTL_END);

    //
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    //
    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);

    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}


int32_t adccircbuffermeancalculator()
{
    uint16_t j ;
    int32_t mean;
    int32_t stepGoal;
    int32_t sum_adc;
    sum_adc = 0;
    j = 0;
    for (j = 0; j < BUFFER_SIZE; j++)
    {
        sum_adc = sum_adc + readCircBuf (&buffer_adc);
    }
    mean = sum_adc/BUFFER_SIZE;
    stepGoal = (mean/35)*100;
    return(1000+stepGoal);
}
