/**
 * @file mcp4821.h
 * @brief
 *
 * MCP4821 Digital-to-analog converter API
 * Pinout:
 * CS	: PA4
 * SCK	: PA5
 * MISO	: PA6
 * MOSI	: PA7
 *
 * @author Nathan Vaniman
 * @date May 7, 2023
 */

#ifndef INC_MCP4821_H_
#define INC_MCP4821_H_

#include "stm32l4xx.h"

#define DAC_GAIN_1x_MASK 0x3000U
#define DAC_GAIN_2x_MASK 0x1000U
#define DAC_SHTDN_MASK 	 0x2000U

void DAC_init(void);
uint16_t DAC_volt_conv(uint16_t mv);
void DAC_write(uint16_t word);
void DAC_latch(void);
void DAC_latch_off(void);

#endif /* INC_MCP4821_H_ */
