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
	/*	float _t = ((float) frequency) * 0.99;
	 frequency = (uint32_t) _t;*/
	uint32_t reloadVal = (SystemCoreClock) / (SINE_SIZE * frequency);
	//updateTIM2(reloadVal);
	initTIM2(reloadVal);
}
