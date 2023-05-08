/*
 * lcd.h
 *
 *  Created on: Apr 16, 2023
 * @authors		   : Lin Knudsen
 * 				   : Nathan Vaniman
 ******************************************************************************
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

// Pin assignments for GPIO-D
#define LCD_PORT GPIOD
#define LCD_D4 GPIO_PIN_0
#define LCD_D5 GPIO_PIN_1
#define LCD_D6 GPIO_PIN_2
#define LCD_D7 GPIO_PIN_3
#define LCD_RS_PIN GPIO_PIN_4
#define LCD_RW_PIN GPIO_PIN_5
#define LCD_E_PIN  GPIO_PIN_6
#define LCD_TIMING_TEST_PIN GPIO_PIN_7

#define LCD_DATA_BITS (LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7)

// LCD Initialization
#define LCD_INIT_1 0x3U
#define LCD_FCN_SET_4_BIT 0x2U
#define LCD_CMD_CLR_DISP 0x01U
#define LCD_CMD_RET_HOME 0x02U
#define LCD_CMD_ENTRY_MODE_SET 0x2U
#define LCD_CMD_DISP_ON 0x8U///0xFU
#define LCD_CMD_DISP_OFF 0x8U
#define LCD_CMD_FCN_SET 0x28U //DL: 4-bit, N: 2-line, F: 5x8 dots

// LCD Delay Values
#define LCD_DELAY_CLEAR_DISPLAY 2000U
#define LCD_DELAY_ENTRY_MODE_SET 3900U
#define LCD_DELAY_DISPLAY_OFF 3900U
#define LCD_DELAY_FUNCTION_SET 3900U
#define LCD_DELAY_INIT_1 4200U


void lcdInit(void);
void lcdWriteString(const char word[]);
void lcdSendChar(uint8_t letter);
void lcdSendCommand(uint8_t command);
void lcdSendHalfCommand(uint8_t command);
void lcdEnablePulse(void);
void lcdPinSetup(void);
void lcdClearDisplay(void);
void lcdSetCursor(uint8_t line, uint8_t column);
void putDwordDecimalValue(uint32_t value);


#endif /* INC_LCD_H_ */
