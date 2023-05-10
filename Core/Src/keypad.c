/*
 * keypad.c
 *
 * Created on: Apr 10, 2023
 *
 * @authors		   : Lin Knudsen
 * 				   : Nathan Vaniman
 ******************************************************************************
 */
#include "main.h"
#include "keypad.h"

#define BRR_OFFSET 0x10U
#define MASK (rowPins[0] | rowPins[1] | rowPins[2] | rowPins[3])
#define DELAY_FOR_LOOP(count) for(int idx=0;idx<count;idx++);
#define TURN_COLUMNS_ON KP_PORT->BSRR = (colPins[0] | colPins[1] | colPins[2] | colPins[3])
#define TURN_COLUMNS_OFF KP_PORT->BRR = (colPins[0] | colPins[1] | colPins[2] | colPins[3])
#define TURN_COLUMN_OFF(val) KP_PORT->BRR = (colPins[val])
#define DEBOUNCE_DELAY 200U	//Number of ms before returning another valid keypad press
#define KP_TIM_PRESCALER (0xFFFFU) // Prescaler value for TIM2 sets each tick to 1ms
// Global variables
uint32_t tim2CounterAtReference = 0; // Variable to store the TIM2 counter value at the reference time point

// Global variables needed for both keypad functions to access
uint8_t col = 0;
uint8_t row = 0;
uint8_t keypadButtons[ROWS][COLS] =
		{ { 0x1, 0x2, 0x3, 0xa }, {
				0x4, 0x5, 0x6, 0xb }, { 0x7, 0x8, 0x9, 0xc }, { 0xe, 0x0,
				0xf, 0xd } };
uint16_t rowPins[ROWS] = { GPIO_PIN_9, GPIO_PIN_11, GPIO_PIN_13,
GPIO_PIN_8 };
uint16_t colPins[COLS] = { GPIO_PIN_7, GPIO_PIN_10, GPIO_PIN_12,
GPIO_PIN_14 };

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
				col = r;
				row = c;
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
	uint8_t kp = keypadRead();
	if (kp != 0x10U) {
		if (getElapsedTime() >= DEBOUNCE_DELAY) {
			storeCurrentTime();
			return (kp);
		}
	}
	return (0x10U);
}


/**
 * @brief  Sets up KP_PORT registers for keypad i/o
 * @retval void
 */
void setupKeypad(void) {
	TIM5_Init();
	storeCurrentTime();
	//Enable Clock for KP_PORT
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOEEN);

	//KP_PORT->MODER = 0xFFFFFFFFUL;
	//Setup Keypad I/O pins
	KP_PORT->MODER = 0x11104000UL;
	//KP_PORT->MODER |=(GPIO_MODER_MODE0_1)
	KP_PORT->PUPDR = 0x4450000UL;
	KP_PORT->OTYPER &= ~(GPIO_OTYPER_OT7 | GPIO_OTYPER_OT10 | GPIO_OTYPER_OT12
			| GPIO_OTYPER_OT14);
	KP_PORT->OSPEEDR |= ((GPIO_OSPEEDR_OSPEED7_Pos)
			| (GPIO_OSPEEDR_OSPEED10_Pos) | (GPIO_OSPEEDR_OSPEED12_Pos)
			| (GPIO_OSPEEDR_OSPEED14_Pos));
}

/**
 * @brief  Initializes TIM5 for keypad debounce
 * @retval void
 */
void TIM5_Init(void) {
	// Enable TIM5 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;
	TIM5->CR1 |= (0x2U << TIM_CR1_CKD_Pos);
	// Set the prescaler value to generate 1 millisecond timebase
	TIM5->PSC = KP_TIM_PRESCALER;
	TIM5->ARR = 0xFFFF;
	// Start the timer
	TIM5->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief  Function to store the current time in ms
 * @retval void
 */
void storeCurrentTime(void) {
	// Read the current counter value of TIM2
	tim2CounterAtReference = TIM5->CNT;
}

/**
 * @brief  Function to calculate elapsed time since the reference time point in ms
 * @retval void
 */
uint32_t getElapsedTime(void) {
	uint32_t currentTime = TIM5->CNT; // Get the current counter value of TIM2
	if (currentTime >= tim2CounterAtReference) {
		return (currentTime - tim2CounterAtReference); // Calculate elapsed time
	} else {
		return (0xFFFFU - tim2CounterAtReference + currentTime); // Counter overflow handler
	}
}
