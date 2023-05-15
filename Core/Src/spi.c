/**
 * @file spi.c
 * @brief
 *
 * Detailed Description
 *
 * @author Nathan Vaniman
 * @date May 7, 2023
 */


#include "spi.h"
#include "stm32l4xx.h"

/**
 * @fn void SPI_init(void)
 * @brief Initializes SPI1 interface for DAC
 *
 */
void SPI_init(void) {
	// SPI config as specified @ STM32L4 RM0351 rev.9 p.1459
	// called by or with DAC_init()
	// build control registers CR1 & CR2 for SPI control of peripheral DAC
	// assumes no active SPI xmits & no recv data in process (BSY=0)
	// CR1 (reset value = 0x0000)
	SPI1->CR1 &= ~( SPI_CR1_SPE);             	// disable SPI for config
	SPI1->CR1 &= ~( SPI_CR1_RXONLY);          	// recv-only OFF
	SPI1->CR1 &= ~( SPI_CR1_LSBFIRST);        	// data bit order MSb:LSb
	SPI1->CR1 &= ~( SPI_CR1_CPOL | SPI_CR1_CPHA); // SCLK polarity:phase = 0:0
	SPI1->CR1 |= SPI_CR1_MSTR;              	// MCU is SPI controller
	SPI1->CR1 &= ~SPI_CR1_BR;

	SPI1->CR2 &= ~( SPI_CR2_TXEIE | SPI_CR2_RXNEIE); // disable FIFO intrpts
	SPI1->CR2 &= ~( SPI_CR2_FRF);              	// Moto frame format
	SPI1->CR2 |= SPI_CR2_NSSP;              	// auto-generate NSS pulse
	SPI1->CR2 |= SPI_CR2_DS;                	// 16-bit data
	SPI1->CR2 |= SPI_CR2_SSOE;              	// enable SS output

	SPI1->CR1 |= SPI_CR1_SPE;               	// re-enable SPI for ops
}

/**
 * @fn void setupSPI1(void)
 * @brief sets up GPIO pins for SPI communications
 * Specific to MCP4821 (No MISO)
 *
 */
void setupSPI1(void) {
	// enable clock for GPIOA & SPI1
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);         // GPIOA: DAC NSS/SCK/SDO
	RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);                // SPI1 port
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED4 | GPIO_OSPEEDR_OSPEED5
			| GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7);
	GPIOA->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6
			| GPIO_MODER_MODE7);
	GPIOA->MODER |= (GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1
			| GPIO_MODER_MODE6_0
			| GPIO_MODER_MODE7_1);
	GPIOA->ODR |= (GPIO_PIN_6);

	GPIOA->AFR[0] &= ~((0x000F << GPIO_AFRL_AFSEL4_Pos)
			| (0x000F << GPIO_AFRL_AFSEL5_Pos)
			| (0x000F << GPIO_AFRL_AFSEL7_Pos)); // clear nibble for bit 7 AF
	GPIOA->AFR[0] |= ((0x0005 << GPIO_AFRL_AFSEL4_Pos)
			| (0x0005 << GPIO_AFRL_AFSEL5_Pos)
			| (0x0005 << GPIO_AFRL_AFSEL7_Pos)); // set b7 AF to SPI1 (fcn 5)
}
