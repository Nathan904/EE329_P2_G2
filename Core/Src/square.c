/**
 * @file square.c
 * @brief
 *
 * Detailed Description
 *
 * @author Nathan Vaniman
 * @date May 8, 2023
 */
#include "main.h"
#include "square.h"
#include "timer.h"

uint32_t reloadVal;
//#define PRESCALAR_1US_TICK (SystemCoreClock/100000UL)

void squareWave(uint32_t frequency, float dutyCycle) {


	reloadVal = (100000 / frequency);
	uint32_t captureVal = getDutyCycle(dutyCycle);

	initTIM2(reloadVal, captureVal);

}


uint32_t getDutyCycle(float dutyCycle) {


	uint32_t val = ((uint32_t) (reloadVal * dutyCycle));
	return (val);

}

void setDutyCycle(float dutyCycle) {
	TIM5->CCR1 = ((uint32_t) (reloadVal * dutyCycle));
}
