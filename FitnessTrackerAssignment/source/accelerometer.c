/**********************************************************
*
* readAcc.c
*
* Example code which reads acceleration in
* three dimensions and displays the reulting data on
* the Orbit OLED display.
*
*    C. P. Moore
*    11 Feb 2020
*
**********************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers/buttons4.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "headers/CircularBuffer.h"
#include "inc/hw_i2c.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "../OrbitOLED/OrbitOLEDInterface.h"
#include "utils/ustdlib.h"
#include "headers/acc.h"
#include "headers/i2c_driver.h"



/**********************************************************
* Constants
**********************************************************/
// Systick configuration
#define SYSTICK_RATE_HZ 100
#define BUF_SIZE 10


/*******************************************
*      Local prototypes
*******************************************/
void SysTickIntHandler (void);
void initClock (void);
void initDisplay (void);
void displayUpdate (char *str1, char *str2, int16_t num, uint8_t charLine);
void initAccl (void);





/***********************************************************
* Initialisation functions: clock, SysTick, PWM
***********************************************************
* Clock
***********************************************************/
void
initSysTick (void)
{
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet (SysCtlClockGet () / SYSTICK_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister (SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable ();
    SysTickEnable ();
}

void
SysTickIntHandler (void)
{
    static uint16_t tickCount = 0;
    tickCount ++;
    updateButtons();
    if (tickCount == 50)
    {

        tickCount = 0;
        displayFlag = 1;

    }
    if (tickCount%10 == 0)
    {
        bufferFlag = 1;
    }

}


void
initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}

/*********************************************************
* initDisplay
*********************************************************/

/*********************************************************
* initAccl
*********************************************************/
void
initAccl (void)
{
    initCircBuf (&buffer_x, BUF_SIZE);
    initCircBuf (&buffer_y, BUF_SIZE);
    initCircBuf (&buffer_z, BUF_SIZE);
    char    toAccl[] = {0, 0};  // parameter, value

    /*
     * Enable I2C Peripheral
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    /*
     * Set I2C GPIO pins
     */
    GPIOPinTypeI2C(I2CSDAPort, I2CSDA_PIN);
    GPIOPinTypeI2CSCL(I2CSCLPort, I2CSCL_PIN);
    GPIOPinConfigure(I2CSCL);
    GPIOPinConfigure(I2CSDA);

    /*
     * Setup I2C
     */
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);

    GPIOPinTypeGPIOInput(ACCL_INT2Port, ACCL_INT2);

    //Initialize ADXL345 Acceleromter

    // set +-2g, 13 bit resolution, active low interrupts
    toAccl[0] = ACCL_DATA_FORMAT;
    toAccl[1] = (ACCL_RANGE_2G | ACCL_FULL_RES);
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_PWR_CTL;
    toAccl[1] = ACCL_MEASURE;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);


    toAccl[0] = ACCL_BW_RATE;
    toAccl[1] = ACCL_RATE_100HZ;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_INT;
    toAccl[1] = 0x00;       // Disable interrupts from accelerometer.
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_OFFSET_X;
    toAccl[1] = 0x00;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_OFFSET_Y;
    toAccl[1] = 0x00;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_OFFSET_Z;
    toAccl[1] = 0x00;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);
}

/********************************************************
* Function to read accelerometer
********************************************************/
void
writebuffer(void)
{


    uint8_t bytesToRead = 6;

    char    fromAccl[] = {0, 0, 0, 0, 0, 0, 0};

    fromAccl[0] = ACCL_DATA_X0;
    I2CGenTransmit(fromAccl, bytesToRead, READ, ACCL_ADDR);

    writeCircBuf (&buffer_x, (fromAccl[2] << 8) | fromAccl[1]);
    writeCircBuf (&buffer_y, (fromAccl[4] << 8) | fromAccl[3]);
    writeCircBuf (&buffer_z, (fromAccl[6] << 8) | fromAccl[5]);

}

void
circbuffermeancalculator(int32_t *mean_x, int32_t *mean_y, int32_t *mean_z)
{
    uint16_t j ;
    int32_t sum_x;
    int32_t sum_y;
    int32_t sum_z;
    sum_x = 0;
    sum_y = 0;
    sum_z = 0;
    *mean_x = 0;
    *mean_y = 0;
    *mean_z = 0;
    j = 0;
    for (j = 0; j < BUF_SIZE; j++)
    {
        sum_x = sum_x + readCircBuf (&buffer_x);
        sum_y = sum_y + readCircBuf (&buffer_y);
        sum_z = sum_z + readCircBuf (&buffer_z);
    }
    *mean_x = sum_x/BUF_SIZE;
    *mean_y = sum_y/BUF_SIZE;
    *mean_z = sum_z/BUF_SIZE;
}



