/**
 * @file ramp.c
 * @brief
 *
 * Detailed Description
 *
 * @author Nathan Vaniman
 * @date May 8, 2023
 */

#include "ramp.h"
#include "timer.h"

#include "stm32l4xx_hal.h"

uint16_t rampSize;
void rampWave(uint16_t frequency) {

	uint32_t reloadVal = (SystemCoreClock) / (RAMP_SIZE * 100);

	/**
	 * @TODO add adjustment factors to fine tune frequencies
	 * can do this in sine.c too
	 */
	switch (frequency) {
		case 100:
			//reloadVal += 1;
			break;
		case 200:
			reloadVal -= 1;
			break;
		case 300:
			reloadVal -= 1;
			break;
		case 400:
			reloadVal -= 1;
			break;
		case 500:
			break;
		default:
			break;
	}
	initTIM2(reloadVal);
}

uint16_t* createRampArray(uint16_t frequency, Polarity p) {
	rampSize = (100 * RAMP_SIZE) / frequency;
	// Allocate memory for the ramp array
	uint16_t *rampArray = (uint16_t*) malloc(rampSize * sizeof(uint16_t));

	// Calculate the step size for the ramp using header macros
	double step = (((double) RAMP_MAX) - RAMP_MIN) / (rampSize - 1);

	// Build array with direction depending on polarity
	switch (p) {
		case POSITIVE:

			// Fill the array with ramp values
			for (uint32_t i = 0; i < rampSize; i++) {
				// 0.5 added to fix rounding
				rampArray[i] = (RAMP_MIN + (uint16_t) (step * i + 0.5))
						| 0x1000U;
			}

			return rampArray;
			break;
		case NEGATIVE:
			for (uint32_t i = 0; i < rampSize; i++) {
				// 0.5 added to fix rounding
				rampArray[i] = (RAMP_MAX - (uint16_t) (step * i + 0.5))
						| 0x1000U;
			}
			return rampArray;
			break;
		default:
			break;
	}
}
