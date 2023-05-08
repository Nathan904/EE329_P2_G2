/*
 * timer.h
 *
 *  Created on: Apr 24, 2023
 *      Author: nathan
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "stm32l4xx_hal.h"

void initTIM3(void);
void initTIM4(uint32_t reloadValue);
void delay_ms(uint32_t ms);
void initTIM2(uint32_t reloadCount, uint32_t ccr1Val);

#endif /* INC_TIMER_H_ */
