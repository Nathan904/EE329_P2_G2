/*
 * keypad.h
 *
 *  Created on: Apr 10, 2023
 * @authors		   : Lin Knudsen
 * 				   : Nathan Vaniman
 ******************************************************************************
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_
#include "stm32l4xx.h"
#define ROWS 4
#define COLS 4

#define KP_PORT GPIOE
const static uint8_t keypadButtons[ROWS][COLS] = { { 0x1, 0x2, 0x3, 0xa }, {
		0x4, 0x5, 0x6, 0xb }, { 0x7, 0x8, 0x9, 0xc }, { 0xe, 0x0, 0xf, 0xd } };
const static uint16_t rowPins[ROWS] = { GPIO_PIN_9, GPIO_PIN_11,
		GPIO_PIN_13,
		GPIO_PIN_8 };
const static uint16_t colPins[COLS] = { GPIO_PIN_7, GPIO_PIN_10, GPIO_PIN_12,
		GPIO_PIN_14 };
uint8_t getKeypad(void);
void setupKeypad(void);
void TIM5_Init(void);

extern uint8_t debounce;

#define NO_KEY_PRESSED 0x10

#endif /* KEYPAD_H_ */
