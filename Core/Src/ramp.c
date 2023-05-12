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
	//uint32_t reloadVal = 0xFFFF;
	uint32_t reloadVal = ((TIM2_CLK_SPEED * 10)
			/ (frequency * (RAMP_SIZE - 1)));
	//updateTIM2(reloadVal);
	initTIM2(reloadVal);
}
