/*
 * lcd.c
 *
 *  Created on: Apr 16, 2023
 * @authors		   : Lin Knudsen
 * 				   : Nathan Vaniman
 ******************************************************************************
 */
#include "main.h"
#include "lcd.h"
#include "delay.h"
#define LCD_LINE_LENGTH 0x40
/**
 * @brief  LCD Initialization Sequence
 * @retval void
 */
void lcdInit(void){
	lcdPinSetup();
	SysTick_Init();
	delay_us(40000);

	//FUNCTION SET 1
	lcdSendHalfCommand(LCD_INIT_1);
	delay_us(LCD_DELAY_INIT_1);

	//FUNCTION SET 2 (Same bits as 1)
	lcdSendHalfCommand(LCD_INIT_1);
	delay_us(LCD_DELAY_INIT_1);

	//FUNCTION SET 3 (Same bits as 2)
	lcdSendHalfCommand(LCD_INIT_1);
	delay_us(LCD_DELAY_FUNCTION_SET);

	//FUNCTION SET 4 (Last one before 4 bit mode)
	lcdSendHalfCommand(LCD_FCN_SET_4_BIT);
	delay_us(LCD_DELAY_FUNCTION_SET);

	//FUNCTION SET INIT CMD
	lcdSendCommand(LCD_CMD_FCN_SET);
	delay_us(LCD_DELAY_FUNCTION_SET);

	//DISPLAY OFF INIT CMD
	lcdSendCommand(0x10U);
	delay_us(LCD_DELAY_DISPLAY_OFF);

	//CLR DISP INIT CMD
	lcdSendCommand(0x0CU);
	delay_us(LCD_DELAY_CLEAR_DISPLAY);

	//ENTRY MODE SET INIT CMD
	lcdSendCommand(0x06U);
	delay_us(LCD_DELAY_ENTRY_MODE_SET);
	SysTick_Return();

}

/**
 * @brief  Sets LCD cursor address
 * @param  line	 : Line Number (0 or 1)
 *         column: Column number
 * @retval void
 */
void lcdSetCursor(uint8_t line,uint8_t column){
	//SysTick_Init();
	LCD_PORT->ODR &= ~(LCD_RS_PIN | LCD_RW_PIN);
	uint8_t address = (((line * LCD_LINE_LENGTH) + column) | (0x1U << 7));
	lcdSendCommand(address);
}

/**
 * @brief  Writes string to LCD using lcdSendChar()
 * @param  word: text to send to LCD
 * @retval void
 */
void lcdWriteString(const char* word){
	for(int idx = 0; word[idx] != '\0'; idx++){
		lcdSendChar((uint8_t)word[idx]);
	}

}

/**
 * @fn void putDwordDecimalValue(uint32_t)
 * @brief Converts uint32_t to ascii string for LCD
 * Adapted from:
 * https://stackoverflow.com/users/6912830/xevion
 *
 * @param value
 */
void putDwordDecimalValue(uint32_t value) {

	uint32_t valueCopy;
	valueCopy = value;

	int length = 0;
	while (valueCopy != 0) {
		length++;
		valueCopy /= 10;
	}

	int i;
	uint8_t asciiDigits[10];  // Max size of 4294967295 => 10 digits
	for (i = 0; i < length; i++) {
		uint8_t lastDigit = value % 10;
		value /= 10;
		asciiDigits[length - (i + 1)] = 0x30 + lastDigit; // 0x30 = 0 in ASCII
	}
	for (i = 0; i < length; i++) {
		lcdSendChar(asciiDigits[i]);
	}
}

/**
 * @brief  Writes char to LCD using lcdSendCommand()
 * @param  letter: char to send to LCD
 * @retval void
 */
void lcdSendChar(uint8_t letter){
	SysTick_Init();
	LCD_PORT->ODR   |= (LCD_RS_PIN);       // RS = HI for data to address
	delay_us(5);
	lcdSendCommand(letter);             // character to print
	delay_us(5);
	LCD_PORT->ODR   &= ~(LCD_RS_PIN);      // RS = LO
	delay_us(100);
	SysTick_Return();

}

/**
 * @brief  			: Sends 8-bit command to LCD using 2 4-bit nibbles (MSB sent first)
 * @param			: uint8_t command
 * 					(8-bit instruction)
 * @retval void
 */
void lcdSendCommand(uint8_t command){
	uint32_t command_ = command;
	LCD_PORT->ODR   &= ~(LCD_DATA_BITS);               // isolate cmd bits
	LCD_PORT->ODR   |= ((command_ >> 4) & LCD_DATA_BITS); // HIGH shifted low
	delay_us(10);
	lcdEnablePulse();                                    // latch HIGH NIBBLE
	LCD_PORT->ODR   &= ~(LCD_DATA_BITS);               // isolate cmd bits
	LCD_PORT->ODR   |= (command_ & LCD_DATA_BITS);      // LOW nibble
	delay_us(10);
	lcdEnablePulse();                                    // latch LOW NIBBLE
}


/**
 * @brief  			: Sends 4-bit command to LCD (Used for init before switching to 4-bit mode)
 * @param			: uint8_t command
 * 					(4-bit instruction)
 * @retval void
 */
void lcdSendHalfCommand(uint8_t command){
	// LCD command using 4 MSB
	LCD_PORT->ODR   &= ~(LCD_DATA_BITS); 	// clear DATA bits
	LCD_PORT->ODR   |= ((command) & LCD_DATA_BITS);   // DATA = command
	lcdEnablePulse();
}

/**
 * @brief  : Sends a pulse on LCD_E_PIN to latch data with delays
 * 		   : Refer to LCD datasheet for timing diagram
 * @retval void
 */
void lcdEnablePulse(void){

	LCD_PORT->ODR   |= (LCD_E_PIN);
	delay_us(5);
	LCD_PORT->ODR   &= ~(LCD_E_PIN);
	delay_us(5);
}

/**
 * @brief  : Configures GPIO Pins for LCD and enables clock for GPIOD
 * 		   : GPIOD 0->6
 * 		   : output, push/pull, no pu/pd, very high speed.
 * @retval void
 */
void lcdPinSetup(void){
	RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIODEN);
	GPIOD->MODER   &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
	GPIOD->MODER   |=  (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0);
	GPIOD->OTYPER  &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3 | GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);
	GPIOD->PUPDR   &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3 | GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);
	GPIOD->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED0_Pos) |
			(3 << GPIO_OSPEEDR_OSPEED1_Pos) |
			(3 << GPIO_OSPEEDR_OSPEED2_Pos) |
			(3 << GPIO_OSPEEDR_OSPEED3_Pos) |
			(3 << GPIO_OSPEEDR_OSPEED4_Pos) |
			(3 << GPIO_OSPEEDR_OSPEED5_Pos) |
			(3 << GPIO_OSPEEDR_OSPEED6_Pos) |
			(3 << GPIO_OSPEEDR_OSPEED7_Pos));
	GPIOD->ODR = (0x0U);
}

void lcdClearDisplay(void){
	SysTick_Init();
	lcdSendCommand(LCD_CMD_CLR_DISP);
	delay_us(LCD_DELAY_CLEAR_DISPLAY);
	SysTick_Return();
}
