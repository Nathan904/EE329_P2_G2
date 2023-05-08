/**
 * @file square.h
 * @brief
 *
 * Detailed Description
 *
 * @author Nathan Vaniman
 * @date May 8, 2023
 */

#ifndef INC_SQUARE_H_
#define INC_SQUARE_H_

#include "stm32l4xx_hal.h"



void squareWave(uint32_t frequency, float dutyCycle);
uint32_t getDutyCycle(float dutyCycle);


#endif /* INC_SQUARE_H_ */
