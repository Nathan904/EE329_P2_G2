/*
 * keypad.c
 *
 * Created on: Apr 10, 2023
 *
 * @authors		   : Lin Knudsen
 * 				   : Nathan Vaniman
 ******************************************************************************
 */
#include "stm32l4xx.h"
#include "keypad.h"

#define BRR_OFFSET 0x10U
#define MASK (rowPins[0] | rowPins[1] | rowPins[2] | rowPins[3])
#define DELAY_FOR_LOOP(count) for(int idx=0;idx<count;idx++);
#define TURN_COLUMNS_ON KP_PORT->BSRR = (colPins[0] | colPins[1] | colPins[2] | colPins[3])
#define TURN_COLUMNS_OFF KP_PORT->BRR = (colPins[0] | colPins[1] | colPins[2] | colPins[3])
#define TURN_COLUMN_OFF(val) KP_PORT->BRR = (colPins[val])
#define DEBOUNCE_DELAY 250UL	//Number of ms before returning another valid keypad press
#define TIM5_PRESCALER_1 400000UL // Prescaler value for TIM2 sets each tick to 1ms
#define PRESCALAR_1US_TICK (SystemCoreClock/100000UL)
#define TIM5_ARR_VAL 10000U // Amount of time (in us) before timer resets
// Global variables
uint32_t tim2CounterAtReference = 0; // Variable to store the TIM2 counter value at the reference time point

// Global variables needed for both keypad functions to access
uint8_t col = 0;
uint8_t row = 0;
uint8_t debounce = 1;
/**
 * @brief  Checks for keypad press
 * @retval uint8_t : Most recent keypad button pressed
 */
uint8_t keypadRead(void) {

	uint32_t idr;

	//Enable COLUMN Output pins
	TURN_COLUMNS_ON;

	// Column Loop
	// Disables column c and gets input pin data
	for (uint8_t c = 0; c < COLS; c++) {
		TURN_COLUMN_OFF(c);
		//DELAY_FOR_LOOP(100)
		idr = KP_PORT->IDR;
		for (uint8_t r = 0; r < ROWS; r++) {
			if ((~idr) & rowPins[r]) {
				col = c;
				row = r;
				return (keypadButtons[row][col]);
			}
		}
		TURN_COLUMNS_ON;
	}
	TURN_COLUMNS_OFF;
	return (0x10U);
}

/**
 * @brief  Checks for keypad press and verifies
 * @retval uint8_t : Most recent keypad button pressed
 */
uint8_t getKeypad(void) {
	uint8_t _kp = keypadRead();
	if ((_kp != 0x10U) && (debounce)) {
			debounce = 0;
		startTIM5();
		return (_kp);
		}
	return (0x10U);
}


/**
 * @brief  Sets up KP_PORT registers for keypad i/o
 * @retval void
 */
void setupKeypad(void) {
	//Enable Clock for KP_PORT
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOEEN);

	//Setup Keypad I/O pins
	KP_PORT->MODER = 0x11104000UL;
	KP_PORT->PUPDR = 0x4450000UL;
	KP_PORT->OTYPER &= ~(GPIO_OTYPER_OT7 | GPIO_OTYPER_OT10 | GPIO_OTYPER_OT12
			| GPIO_OTYPER_OT14);
	KP_PORT->OSPEEDR |= ((GPIO_OSPEEDR_OSPEED7_Pos)
			| (GPIO_OSPEEDR_OSPEED10_Pos) | (GPIO_OSPEEDR_OSPEED12_Pos)
			| (GPIO_OSPEEDR_OSPEED14_Pos));
	TIM5_Init();
}
void startTIM5(void) {
	//TIM5->CNT = 0;
	TIM5->DIER |= TIM_DIER_UIE;
	//TIM5->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief  Initializes TIM2 for keypad debounce
 * @retval void
 */
void TIM5_Init(void) {
	// Enable TIM2 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;

	// Enable Interrupt
	TIM5->DIER |= (TIM_DIER_UIE);

	// Set the prescaler value to generate 1 millisecond timebase
	TIM5->PSC = PRESCALAR_1US_TICK;

	// Set the auto-reload value
	TIM5->ARR = TIM5_ARR_VAL;

	TIM5->SR &= ~(TIM_SR_UIF); // Clear flags

	NVIC_EnableIRQ(TIM5_IRQn);
	__enable_irq();

	// Start the timer
	TIM5->CR1 |= TIM_CR1_CEN;
}
