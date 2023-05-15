/**
 * @file ramp.h
 * @brief
 *
 * Detailed Description
 *
 * @author Nathan Vaniman
 * @date May 8, 2023
 */

#ifndef INC_RAMP_H_
#define INC_RAMP_H_
#include "stm32l4xx_hal.h"

#define RAMP_SIZE 290 // S
#define RAMP_MAX 2910U
#define RAMP_MIN 0U
void rampWave(uint16_t frequency);

typedef enum {
	POSITIVE,
	NEGATIVE
} Polarity;

uint16_t* createRampArray(uint16_t frequency, Polarity p);
extern uint16_t rampSize;

#endif /* INC_RAMP_H_ */
