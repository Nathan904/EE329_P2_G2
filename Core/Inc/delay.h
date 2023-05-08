/*
 * delay.h
 *
 *  Created on: Apr 16, 2023
 * @authors		   : Lin Knudsen
 * 				   : Nathan Vaniman
 ******************************************************************************
 */
#ifndef INC_DELAY_H_
#define INC_DELAY_H_
#include "stm32l4xx_hal.h"

void SysTick_Init(void);
void SysTick_Return(void);
void delay_us(const uint32_t time_us);

#endif /* INC_DELAY_H_ */
