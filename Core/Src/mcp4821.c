/**
 * @file mcp4821.c
 * @brief
 *
 * Detailed Description
 *
 * @author Nathan Vaniman
 * @date May 7, 2023
 */


#include "mcp4821.h"
#include "stm32l4xx.h"
#include "spi.h"

/**
 * @fn void DAC_init(void)
 * @brief Initializes DAC by calling init functions
 *
 */
void DAC_init(void) {
	setupSPI1();
	SPI_init();
}

/**
 * @fn uint16_t DAC_volt_conv(uint16_t)
 * @brief Converts millivolts to 16 bit DAC message
 *
 * @param mv : millivolts
 * @return uint16_t : _dacData
 */
uint16_t DAC_volt_conv(uint16_t mv) {
	mv = (uint16_t) (((float) mv * 0.993f) - 2.0);
	uint16_t _dacData;
	if (mv < 2048) {
		_dacData = (mv * 2);
		_dacData |= (0x3000U);
	}
	else if (mv <= 3300) {
		_dacData = mv | 0x1000U;
	}
	else if (mv <= 0) {
		_dacData = (0x2000U);
	}
	else {
		_dacData = (0xFFF | 0x1000U);
	}
	return (_dacData);
}

/**
 * @fn void DAC_write(uint16_t)
 * @brief sends 16-bit word to DAC using SPI protocol
 *
 * @param word : 16 bit word to send to DAC
 */
void DAC_write(uint16_t word) {
	SPI1->DR = word;


	//while (!(SPI1->SR & SPI_SR_TXE))
	//; // Wait for data to finish moving to FIFO register
	//while (SPI1->SR & SPI_SR_BSY)
	//; // Wait for the message to send (busy flag cleared)


}
void DAC_latch(void) {
	GPIOA->ODR &= ~GPIO_PIN_6;
}
void DAC_latch_off(void) {
	GPIOA->ODR |= GPIO_PIN_6;
}
