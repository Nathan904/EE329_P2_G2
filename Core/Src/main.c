/**
 *************************************************************************
  * @file           : main.c
  * @brief          : Main program body
 *************************************************************************

  */

#include "main.h"
#include "square.h"
#include "mcp4821.h"
#include "lcd.h"
#include "keypad.h"
#include "timer.h"
#include "ramp.h"
#include "sine.h"
#define V_HIGH ((3000U) | 0x1000U)
#define V_LOW 0x1000U
#define FLOAT_TO_PERCENT_CHAR(f)\
	uint8_t val=(uint8_t)(f*100); \
	dutyCycleLCD[1] = ((val/10)) + '0';\
	dutyCycleLCD[0] = (val%10) + '0';



void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/**
 * @enum State
 * @brief enumeration for the square wave
 *
 */
typedef enum {
	LOW, /**< LOW */
	HIGH,/**< HIGH */
	WAIT /**< WAIT */
} State;
State state = LOW;

/**
 * @enum MODE
 * @brief enumeration for the output wave type
 *
 */
typedef enum {
	SQUARE,/**< SQUARE */
	SINE, /**< SINE */
	RAMP /**< RAMP */
} Mode;

// Initialize default mode (100 Hz square wave 50% duty)
Mode currentMode = SQUARE;
uint16_t frequency = 100U;
float dutyCycle = 0.5f;
uint32_t rampIdx = 0;
char dutyCycleLCD[2] = { 5 + '0', 0 + '0' };
uint8_t kpLast = 0x10U;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();
	lcdInit();
	lcdWriteString("HELLO");
	DAC_init();
	setupKeypad();
	setupTIM2();
	squareWave(frequency, dutyCycle);


	updateLCD();

  while (1)
  {
		switch (currentMode) {
			case SQUARE:
				square();
				break;
			case RAMP:
				ramp();
				break;
			case SINE:
				sine();
			default:
				break;
		}
		checkUserInput();
  }
}

void ramp(void) {
	DAC_latch_off(); // Set LDAC high to disable latch
	rampIdx = (rampIdx > RAMP_SIZE) ? (0) : (rampIdx);
	DAC_write((RAMP_DATA[rampIdx] | 0x1000U)); // Prepare high value in DAC

}
void sine(void) {
	DAC_latch_off(); // Set LDAC high to disable latch
	rampIdx = (rampIdx > SINE_SIZE) ? (0) : (rampIdx);
	DAC_write((SINE_DATA[rampIdx] | 0x1000U)); // Prepare high value in DAC

}

/**
 * @fn void checkUserInput(void)
 * @brief keypad user input handler
 * switch with case for each possible user input
 * @todo add the rest
 *
 */
void checkUserInput(void) {
	uint8_t kp = getKeypad();
	switch (kp) {
		case 0x10:
			break;
		case 0x1:
			frequency = 100U;
			updateWave();
			kpLast = kp;
			break;
		case 0x2:
			frequency = 200U;
			updateWave();
			kpLast = kp;
			break;
		case 0x3:
			frequency = 300U;
			updateWave();
			kpLast = kp;
			break;
		case 0x4:
			frequency = 400U;
			updateWave();
			kpLast = kp;
			break;
		case 0x5:
			frequency = 500U;
			updateWave();
			kpLast = kp;
			break;
		case 0x0:
			dutyCycle = 0.5f;
			updateWave();
			kpLast = kp;
			break;
		case 0xa:
			dutyCycle += 0.1f;
			dutyCycle = (dutyCycle > 0.9f) ? (0.9f) : (dutyCycle);
			updateWave();
			kpLast = kp;
			break;
		case 0xb:
			// @TODO does anyone have a 4x4 keypad with working asterisk?
			dutyCycle -= 0.1f;
			dutyCycle = (dutyCycle < 0.1f) ? (0.1f) : (dutyCycle);
			updateWave();
			kpLast = kp;
			break;
		case 0x6:
			currentMode = SINE;
			frequency = 100;
			updateWave();
			kpLast = kp;
			break;
		case 0x7:
			currentMode = RAMP;
			updateWave();
			kpLast = kp;
			break;
		case 0x8:
			currentMode = SQUARE;
			updateWave();
			kpLast = kp;
			break;
		case 0x9:
			break;
		default:
			break;
	}
}
void updateWave() {
	switch (currentMode) {
		case SQUARE:
			//updateLCD(); @TODO implement LCD
			squareWave(frequency, dutyCycle);
			break;
		case SINE:
			sineWave(frequency);
			break;
		case RAMP:
			rampWave(frequency);
			break;
		default:
			break;
	}
}

/**
 * @fn void updateLCD()
 * @brief updates LCD to match current mode/state
 *
 */
void updateLCD() {
	/**
	 * @TODO Add LCD feature
	 * The tricky part will be converting the float to a char array
	 * Don't use sprintf/fprintf (no string.h)
	 * COMMANDS:
	 * lcdClearDisplay(void);
	 * lcdSetCursor(line,col) @note is 0 or 1
	 * lcdSendChar(uint8_t val)
	 * lcdWriteString("HELLO")
	 */
	switch (currentMode) {
		case SQUARE:
			lcdClearDisplay();
			lcdSetCursor(0,0);
		
			lcdWriteString("SQR ");
			//borrowed this from lcd.c
			putDwordDecimalValue(frequency);
			lcdWriteString(" Hz  LAST");
			lcdSetCursor(1,0);
		
			FLOAT_TO_PERCENT_CHAR(dutyCycle);
			lcdWriteString(dutyCycleLCD);
			lcdWriteString("% DUTY");
			break;
		case SINE:
			lcdClearDisplay();
			lcdSetCursor(0,0);
		
			lcdWriteString("SIN ");
			putDwordDecimalValue(frequency);
			lcdWriteString(" Hz  LAST");
			lcdSetCursor(1,0);
			/**
			*@TODO needs num of pts per cycle
			*putDwordDecimalValue(pointPerCycle);
			*lcdWriteString(" PTS");
			**/
			break;
		case RAMP:
			lcdClearDisplay();
			lcdSetCursor(0,0);
			lcdWriteString("SAW ");
			putDwordDecimalValue(frequency);
			lcdWriteString(" Hz  LAST");
			lcdSetCursor(1,0);
			/**
			* @TODO implement ramp polarity on 0 button
			*if (rampPolarity == 0){
			*	lcdWriteString("NEGATIVE");
			*} else {
			*	lcdWriteString("POSITIVE");
			*}
			**/
			break;
		default:
			break;
	}
	lcdSetCursor(1, 13);
	lcdSendChar(0x27); //sends ' char
	lcdSendChar(kpLast);
	lcdSendChar(0x27);
}



/**
 * @fn void square(void)
 * @brief square wave manager
 * handles latch and writes to DAC
 * @note sequence used for the square wave is as follows (assume start low):
 * (TIM2 CCR1IF)->Enables latch->(square() clears latch and writes high)
 * (TIM2    UIF)->Enables latch->(square() clears latch and writes low)
 * This repeats indefinintely
 */
void square(void) {
	switch (state) {
		case HIGH:
			DAC_latch_off(); // Set LDAC high to disable latch
			DAC_write(V_LOW); // Prepare low value in DAC
			break;
		case LOW:
			DAC_latch_off(); // Set LDAC high to disable latch
			DAC_write(V_HIGH); // Prepare high value in DAC
			break;
		case WAIT:
			break;
		default:
			break;
	}
}

/**
 * @fn void TIM2_IRQHandler(void)
 * @brief Interrupt handler for TIM2 (wave gen timer)
 * Clears flags and latches DAC data (very precise timing)
 * @note a switch case should be added to utilize this for other wave types
 *
 */
void TIM2_IRQHandler(void) {
	DAC_latch();
	if(TIM2->SR & TIM_SR_UIF) {
		TIM2->SR &= ~TIM_SR_UIF; // clear the update interrupt flag
		state = HIGH;
		rampIdx++;
	}
	else //(TIM2->SR & TIM_SR_CC1IF)
	{
		TIM2->SR &= ~TIM_SR_CC1IF; // clear the cc1 interrupt flag
		state = LOW;
	}
}



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0xFFU; //RCC_MSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_10;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
	}

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : STLK_RX_Pin STLK_TX_Pin */
  GPIO_InitStruct.Pin = STLK_RX_Pin|STLK_TX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_SOF_Pin USB_ID_Pin USB_DM_Pin USB_DP_Pin */
  GPIO_InitStruct.Pin = USB_SOF_Pin|USB_ID_Pin|USB_DM_Pin|USB_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
