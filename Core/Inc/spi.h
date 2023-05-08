/**
 * @file spi.h
 * @brief
 *
 * Detailed Description
 *
 * @author Nathan Vaniman
 * @date May 7, 2023
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#define SPI_PORT GPIOA
#define SPI_PIN_SCK GPIO_PIN_5
#define SPI_PIN_MOSI GPIO_PIN_7
#define SPI_PIN_CS GPIO_PIN_4

void setupSPI1(void);
void SPI_init(void);

#endif /* INC_SPI_H_ */
