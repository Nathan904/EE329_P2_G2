/*
 * timer.c
 *
 *  Created on: Apr 24, 2023
 *      Author: nathan
 */
#include "timer.h"
#include "main.h"

#define CLOCK_FREQUENCY SystemCoreClock
#define PRESCALAR_1US_TICK (CLOCK_FREQUENCY/100000UL)

/**
 * @fn void setupTIM2()
 * @brief old TIM2 function (ignore)
 *
 */
void setupTIM2() {
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->DIER |= (TIM_DIER_UIE | TIM_DIER_CC1IE);
	TIM2->CR1 |= (0x2U << TIM_CR1_CKD_Pos);
	TIM2->PSC = PRESCALAR_1US_TICK + 1; // prescaler value
	TIM2->SR &= ~(TIM_SR_UIF | TIM_SR_CC1IF);
	NVIC_EnableIRQ(TIM2_IRQn);
	__enable_irq();
	TIM2->CR1 |= TIM_CR1_CEN;

}

/**
 * @fn void initTIM2(uint32_t, uint32_t)
 * @brief initializes TIM2 for square wave
 *
 * @param reloadCount
 * @param ccr1Val
 */
void initTIM2(uint32_t reloadCount, uint32_t ccr1Val) {
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->DIER |= (TIM_DIER_UIE | TIM_DIER_CC1IE);
	TIM2->PSC = PRESCALAR_1US_TICK + 1; // prescaler value
	TIM2->ARR = reloadCount - 1; // reload value
	TIM2->CCR1 = ccr1Val;
	TIM2->SR &= ~(TIM_SR_UIF | TIM_SR_CC1IF);
	NVIC_EnableIRQ(TIM2_IRQn);
	__enable_irq();
	TIM2->CR1 |= TIM_CR1_CEN;

}

/**
 * @fn void updateTIM2(uint32_t, uint32_t)
 * @brief updates TIM2 CCR1 and ARR registers
 * briefly disables timer to do this
 * @note CNT must be reset or hardfault occurs if ARR is less than CNT
 *
 * @param reloadCount
 * @param ccr1Val
 */
void updateTIM2(uint32_t reloadCount, uint32_t ccr1Val) {
	TIM2->CR1 &= ~(TIM_CR1_CEN);
	TIM2->CNT = 0UL;
	TIM2->ARR = reloadCount - 1; // reload value
	TIM2->CCR1 = ccr1Val;
	TIM2->SR &= ~(TIM_SR_UIF | TIM_SR_CC1IF);
	TIM2->CR1 |= TIM_CR1_CEN;
}


