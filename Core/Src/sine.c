/**
 * @file sine.c
 * @brief
 *
 * Detailed Description
 *
 * @author Nathan Vaniman
 * @date May 8, 2023
 */


#include "sine.h"
#include "timer.h"

void sineWave(uint32_t frequency) {
	//uint32_t reloadVal = 0xFFFF;
	uint32_t reloadVal = ((TIM2_CLK_SPEED * 10)
			/ (frequency * (SINE_SIZE - 1)));
	//updateTIM2(reloadVal);
	initTIM2(reloadVal);
}
