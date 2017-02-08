/*
 *  Copyright (c) 2013-2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include "app_in8.h"
#include "com_objs.h"
#include "params.h"
#include "fiftyHertz.h"

#include <sblib/eib.h>
#include <sblib/eib/sblib_default_objects.h>
#include <sblib/timeout.h>

// Digital pin for LED
#define PIO_LED PIO2_0

#define DIRECT_IO

// Debouncers for inputs
Debouncer inputDebouncer[NUM_CHANNELS];

const byte* channelParams = userEepromData + (EE_CHANNEL_PARAMS_BASE - USER_EEPROM_START);
const byte* channelTimingParams = userEepromData + (EE_CHANNEL_TIMING_PARAMS_BASE - USER_EEPROM_START);

#ifdef DIRECT_IO
// Input pins
const int inputPins[] =
//    { PIO2_2, PIO0_7, PIO2_10, PIO2_9, PIO0_2, PIO0_8, PIO0_9, PIO2_11 };  // 4TE controller
    { PIO2_2, PIO0_9, PIO2_11, PIO1_1, PIO3_0, PIO3_1, PIO3_2, PIO2_9 };   // TS-ARM

void setupIO(void)
{
    // Configure the input pins and initialize the debouncers with the current
    // value of the pin.
    for (int channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        pinMode(inputPins[channel], INPUT | HYSTERESIS | PULL_DOWN);
        inputDebouncer[channel].init(digitalRead(inputPins[channel]));
    }
}

void scanIO(void)
{

}

int readIO(int channel)
{
    return digitalRead(inputPins[channel]);
}

#else
#include <sblib/spi.h>

SPI spi(SPI_PORT_0);
static unsigned int ioState;

void setupIO(void)
{
   ioState = 0;
   pinMode(PIO0_8,  OUTPUT | SPI_MISO);  // IO6
   pinMode(PIO0_9,  OUTPUT);             // IO7
   pinMode(PIO2_11, OUTPUT | SPI_CLOCK); // IO8

   spi.setClockDivider(128);
   spi.begin();
   spi.setDataSize(SPI_DATA_8BIT);
}

void scanIO(void)
{
    digitalWrite(PIO0_9, 0);
    digitalWrite(PIO0_9, 1);
    ioState = spi.transfer(0x00);
}

int readIO(int channel)
{
    return ioState & (1 << channel);

}
#endif

/**
 * Application setup
 */
void setup()
{
    bcu.begin(4, 0x7054, 2); // We are a "Jung 2118" device, version 0.2

    pinMode(PIO_LED, OUTPUT);
    digitalWrite(PIO_LED, 1);

    setupIO();
    // Handle configured power-up delay
    unsigned int startupTimeout = calculateTime
            ( userEeprom[EE_BUS_RETURN_DELAY_BASE] >> 4
            , userEeprom[EE_BUS_RETURN_DELAY_FACT] &  0x7F
            );
    Timeout delay;
    int debounceTime = userEeprom[EE_INPUT_DEBOUNCE_TIME] >> 1;
    delay.start(startupTimeout);
    while (delay.started() && !delay.expired())
    {   // while we wait for the power on delay to expire we debounce the input channels
        scanIO();
        for (int channel = 0; channel < NUM_CHANNELS; ++channel)
        {
            inputDebouncer[channel].debounce(readIO(channel), debounceTime);
        }
        waitForInterrupt();
    }
    initApplication();

    //provide a 50 Hz output, e.g. for a rebuild of a motion sensor with zero-crossing-detection
    fiftyHertzInit();
}

/**
 * The application's main.
 */
void loop()
{
    int debounceTime = userEeprom[EE_INPUT_DEBOUNCE_TIME] >> 1;
    int objno, channel, value, lastValue;

    scanIO();
    // Handle the input pins
    for (channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        lastValue = inputDebouncer[channel].value();
        value = inputDebouncer[channel].debounce(readIO(channel), debounceTime);

        if (lastValue != value)
            inputChanged(channel, value);
    }

    // Handle updated communication objects
    while ((objno = nextUpdatedObject()) >= 0)
    {
        objectUpdated(objno);
    }

    // Handle timed functions (e.g. periodic update)
    handlePeriodic();

    // Sleep up to 1 millisecond if there is nothing to do
    if (bus.idle())
        waitForInterrupt();
}
