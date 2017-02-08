/*
 * fiftyHertz.cpp
 *
 *  Created on: 08.02.2017
 *      Author: Peter Kappelt
 */

#include <sblib/eib.h>
#include <sblib/timer.h>
#include "fiftyHertz.h"

/**
 * This file provides a 50 Hz output.
 * It might be usefull for motion detectors,
 * so you can emulate a 50 Hz power line signal,
 * which is necessary if the sensor uses zero-crossing-detection.
 *
 * The signal is provided @ PIO0_11, which is IO12 on the ARM-"Tasterschnittstelle".
 * Additionally, TIMER32_0 is in use.
 */

Timer fiftyHertzClock(TIMER32_0);

void fiftyHertzInit(void){
    fiftyHertzClock.begin();
    fiftyHertzClock.start();
    fiftyHertzClock.prescaler(1000);						//48MHz / 1000 = 48 kHz

    fiftyHertzClock.matchMode(MAT2, RESET);					//Reset the timer with Channel 2 match
    fiftyHertzClock.match(MAT2, 960);						//Reset the timer at 961 -> 48 kHz / 960 = 50 Hz

    fiftyHertzClock.pwmEnable(MAT3);						//enable pwm for channel 3
    fiftyHertzClock.match(MAT3, (960 / 2));					// 50 % DC
    pinMode(PIO0_11, OUTPUT_MATCH);
}
