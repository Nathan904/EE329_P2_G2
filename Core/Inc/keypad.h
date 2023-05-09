/*
 * keypad.h
 *
 *  Created on: Apr 10, 2023
 * @authors		   : Lin Knudsen
 * 				   : Nathan Vaniman
 **************************************************************************
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#define ROWS 4
#define COLS 4

#define KP_PORT GPIOE

uint8_t keypadRead(void);
uint8_t getKeypad();
void setupKeypad(void);
void storeCurrentTime(void);
uint32_t getElapsedTime(void);
void TIM5_Init(void);
#define NO_KEY_PRESSED 0x10

#endif /* KEYPAD_H_ */
