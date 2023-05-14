/*
 * timer.h
 *
 *  Created on: Apr 24, 2023
 *      Author: nathan
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "stm32l4xx_hal.h"
#define TIM2_CLK_SPEED 100000UL // Clock speed [Hz] of TIM2 (0.1us/tick)
#define RAMP_PSC SystemCoreClock/100000UL

void initTIM3(void);
void initTIM4(uint32_t reloadValue);
void delay_ms(uint32_t ms);
void initTIM2(uint32_t reloadCount);
void updateTIM2(uint32_t reloadCount);
void initSquareTIM2(uint32_t reloadCount, uint32_t ccr1Val);
void updateSquareTIM2(uint32_t reloadCount, uint32_t ccr1Val);
void setupTIM2(void);
void stopTIM2(void);
#endif /* INC_TIMER_H_ */
