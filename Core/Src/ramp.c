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




void rampWave(uint32_t frequency) {

	uint32_t reloadVal = (SystemCoreClock) / (RAMP_SIZE * frequency);

	initTIM2(reloadVal);
}
