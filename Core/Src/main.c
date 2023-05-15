/**
 ***************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ***************************************************************************

 */

#include "main.h"
#include "square.h"
#include "mcp4821.h"
#include "lcd.h"
#include "keypad.h"
#include "timer.h"
#include "ramp.h"
#include "sine.h"
#include <stdlib.h>
#define V_HIGH ((2910U) | 0x1000U) // Square wave high DAC_write value
#define V_LOW 0x1000U // Square wave low DAC_write value

// System config functions (ide pre-generated)
void SystemClock_Config(void);

/***************************************************************************
 * Begin Global Variables
 ***************************************************************************
 **/

/**
 * @enum State
 * @brief enumeration for the square wave
 *
 */
typedef enum {
	LOW, /**< LOW *//**< LOW */
	HIGH,/**< HIGH *//**< HIGH */
	WAIT /**< WAIT *//**< WAIT */
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

/**
 * @enum Freq
 * @brief enumeration of frequencies to use for 2d array
 * e.g. for 100 Hz: (ONE = 0) --> first column vector in array is used
 *
 */
typedef enum {
	ONE, /**< ONE HUNDRED*/
	TWO, /**< TWO HUNDRED*/
	THREE,/**< THREE HUNDRED*/
	FOUR, /**< FOUR HUNDRED*/
	FIVE /**< FIVE HUNDRED*/
} Freq;

Freq freq = ONE; // Initialize with default frequency

// Initialize default mode (100 Hz square wave 50% duty)
Mode currentMode = SQUARE;

uint16_t frequency = 100U; // global frequency
float dutyCycle = 0.5f; // fp duty cycle 0<=dutyCycle<=1
uint32_t idx = 0; // Global index for ramp/sine arrays
char dutyCycleLCD[2] = { 5 + '0', 0 + '0' }; // char array for duty cycle
uint8_t kpLast = 0x0U; // Global last keypad button pressed
uint16_t **sineArrays; // 2d sine array global pointer
uint16_t *rampArray;
uint8_t latch = 0;
Polarity polarity = POSITIVE;
uint16_t sineSizes[FREQUENCY_COUNT];
/***************************************************************************
 * End Global Variables
 ***************************************************************************
 **/

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	//Pregenerated init functions
	HAL_Init();
	SystemClock_Config();

	/* Create arrays for sine/ramp functions */
	sineArrays = createSmallArrays(SINE_DATA);
	rampArray = createRampArray(frequency, polarity);

	lcdInit(); // initialize LCD
	updateLCD(); // add initial LCD text
	DAC_init(); // init SPI1 and DAC
	setupKeypad(); // init keypad
	setupTIM2(); // initial setup for TIM2

	// start program at 100 Hz 50% duty cycle square wave
	squareWave(frequency, dutyCycle);
	for (int i = 0; i < FREQUENCY_COUNT; i++) {
		sineSizes[i] = (SINE_SIZE / i) + 0.5;
	}
	updateWave();
	DAC_latch();
	currentMode = RAMP;
	updateWave();
	while (1)
	{
		/**
		 * @brief
		 * Switch calls corresponding function for each mode
		 */
			switch (currentMode) {
			case SQUARE:
				square();
				break;
			case RAMP:
				ramp();
				break;
			case SINE:
				sine();
				break;
			default:
				break;
			}
		// Check Keypad for user input
		checkUserInput();
	}
}

/***************************************************************************
 * DAC_write functions
 ***************************************************************************
 */

/**
 * @fn void square(void)
 * @brief square wave manager handles latch and writes to DAC
 * @note sequence used for the square wave is as follows (assume start low):
 * (TIM2 CCR1IF)->Enables latch->(square() clears latch and writes high)
 * (TIM2    UIF)->Enables latch->(square() clears latch and writes low)
 * This repeats indefinitely
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
 * @fn void ramp(void)
 * @brief disables latch and writes next ramp value to DAC
 *
 */
void ramp(void) {
	//DAC_latch_off(); // Set LDAC high to disable latch
	// Reset idx to beginning of array if it has sent final value
	idx = (idx >= rampSize) ? (0) : (idx);
	if (latch) {
		idx++;
	DAC_write(rampArray[idx]); // Prepare high value in DAC
		latch = 0;
	}

}

/**
 * @fn void sine(void)
 * @brief disables latch and writes next sine value to DAC
 *
 */
void sine(void) {
	//DAC_latch_off();
	// Reset idx to beginning of array if it has sent final value
	idx = (idx > (SINE_SIZE / (freq + 1))) ? (0) : (idx);
	if (latch) {
		idx++;
		latch = 0;
		DAC_write(sineArrays[freq][idx]); // Prepare high value in DAC
	}
}

/***************************************************************************
 * User interface functions
 ***************************************************************************
 */

/**
 * @fn void checkUserInput(void)
 * @brief keypad user input handler
 * switch with case for each possible user input
 *
 */
void checkUserInput(void) {
	uint8_t kp = getKeypad();
	switch (kp) {
		case 0x10:
			break;
		case 0x1:
			kpLast = kp;
			freq = ONE;
			frequency = 100U;
			updateWave();
			break;
		case 0x2:
			kpLast = kp;
			freq = TWO;
			frequency = 200U;
			updateWave();
			break;
		case 0x3:
			kpLast = kp;
			freq = THREE;
			frequency = 300U;
			updateWave();
			break;
		case 0x4:
			kpLast = kp;
			freq = FOUR;
			frequency = 400U;
			updateWave();
			break;
		case 0x5:
			kpLast = kp;
			freq = FIVE;
			frequency = 500U;
			updateWave();
			break;
		case 0x0:
			///Set Duty cycle if using square wave output
			if (currentMode == SQUARE) {
				kpLast = kp;
				dutyCycle = 0.5f;
				updateWave();
			}
			else if (currentMode == RAMP) {
				polarity = 1 - polarity;
				kpLast = kp;
				updateWave();
			}
			break;
		case 0xa:
			///Set Duty cycle if using square wave output
			if (currentMode == SQUARE) {
				kpLast = kp;
				dutyCycle += 0.1f;
				dutyCycle = (dutyCycle > 0.9f) ? (0.9f) : (dutyCycle);
				updateWave();
			}
			break;
		case 0xb:
			///Set Duty cycle if using square wave output
			if (currentMode == SQUARE) {
				kpLast = kp;
				dutyCycle -= 0.1f;
				dutyCycle = (dutyCycle < 0.1f) ? (0.1f) : (dutyCycle);
				updateWave();
				kpLast = kp;
			}
			break;
		case 0x6:
			kpLast = kp;
			currentMode = SINE;
			//freq = ONE;
			//frequency = 100;
			updateWave();
			break;
		case 0x7:
			kpLast = kp;
			currentMode = RAMP;
			updateWave();
			break;
		case 0x8:
			kpLast = kp;
			currentMode = SQUARE;
			updateWave();
			break;
		case 0x9:
			kpLast = kp;
			currentMode = SQUARE;
			frequency = 100;
			dutyCycle = 0.5f;
			updateWave();
			break;
		default:
			break;
	}
}

/***************************************************************************
 * LCD Waveform Screen Functions
 ***************************************************************************
 */

/**
 * @fn void updateLCD(void)
 * @brief updates LCD to match current mode/state
 *
 */
void updateLCD(void) {
	switch (currentMode) {
		case SQUARE:
			squareWvScreen(frequency, dutyCycle);
			break;
		case SINE:
			sineWvScreen(frequency);
			break;
		case RAMP:
			rampWvScreen(frequency);
			break;
		default:
			break;
	}
}

/**
 * @fn void rampWvScreen(int)
 * @brief updates displays ramp screen information
 * text displayed:
 * "	SAW ###Hz LAST
 * 		POSITIVE   '#'	"
 *
 * @param frequency
 */
void rampWvScreen(int freq) {
	lcdClearDisplay();

	lcdSetCursor(0, 0);
	lcdWriteString("SAW ");
	lcdWriteFreq(freq);
	lcdWriteString("  LAST");

	lcdSetCursor(1,0);
	if (polarity == POSITIVE){
		lcdWriteString("POSITIVE");
	} else {
	 	lcdWriteString("NEGATIVE");
	}

	lcdSetCursor(1, 13);
	lcdWriteKey(kpLast);
}

/**
 * @fn void sineWvScreen(int)
 * @brief updates displays sine screen information
 * text displayed:
 * "	SIN ###Hz LAST
 * 		#### PTS   '#'	"
 *
 * @param frequency
 */
void sineWvScreen(int freq) {
	lcdClearDisplay();

	lcdSetCursor(0, 0);
	lcdWriteString("SIN ");
	lcdWriteFreq(freq);
	lcdWriteString("  LAST");

	lcdSetCursor(1, 0);
	lcdWritePTS(((100*SINE_SIZE) / (freq + 1)));

	lcdSetCursor(1, 13);
	lcdWriteKey(kpLast);
}

/**
 * @fn void squareWvScreen(int , float )
 * @brief updates displays square wave screen information
 * text displayed:
 * "	SQR ###Hz LAST
 * 		##.#% DUTY '#'	"
 *
 * @param frequency, duty cycle (as a decimal)
 */
void squareWvScreen(int freq, float duty) {
	lcdClearDisplay();

	lcdSetCursor(0, 0);
	lcdWriteString("SQU ");
	lcdWriteFreq(freq);
	lcdWriteString("  LAST");

	lcdSetCursor(1, 0);
	lcdWriteDuty(duty * 100);

	lcdSetCursor(1, 13);
	lcdWriteKey(kpLast);
}

/**
 * @fn void squareWvScreen(int freq)
 * @brief displays the current frequency on the LCD
 *      at the current cursor position
 *
 *  @param frequency
 */
void lcdWriteFreq(int freq) {
	switch (freq) {
		case 100:
			lcdWriteString("100 Hz");
			break;
		case 200:
			lcdWriteString("200 Hz");
			break;
		case 300:
			lcdWriteString("300 Hz");
			break;
		case 400:
			lcdWriteString("400 Hz");
			break;
		case 500:
			lcdWriteString("500 Hz");
			break;
		default:
			lcdWriteString("ERR");
			break;
	}
}

/**
 * @fn void lcdWriteDuty(float)
 * @brief displays the duty cycle on the LCD at the current cursor position
 *
 * @param duty (as percent)
 */
void lcdWriteDuty(float duty) {
	lcdSendChar(((int) duty / 10) + '0');	// isolating each digit
	lcdSendChar(((int) duty % 10) + '0');	// to display individually
	lcdSendChar('.');
	lcdSendChar(((int) (duty * 10)) % 10 + '0');
	lcdSendChar('%');
	lcdWriteString(" Duty");
}

/**
 * @fn void lcdWriteKey(uint8_t)
 * @brief displays the last pressed key on the LCD at the current
 * cursor position
 *
 * @param key (in hex)
 */
void lcdWriteKey(uint8_t key) {
	lcdSendChar('\'');

	if (kpLast <= 0x9) {
		lcdSendChar(key + '0');
	}
	else {
		lcdSendChar(key + 0x37); //if one of the letters is pressed
							//offset to proper spot in ascii table
	}

	lcdSendChar('\'');
}

/**
 * @fn void lcdWritePTS(int)
 * @brief displays the number of points in the sine wave on the LCD
 * at the current cursor position
 *
 * @param points (from 0>9999)
 */
void lcdWritePTS(int points) {
	lcdSendChar((points/1000) + '0');		//isolating each digit to display
	lcdSendChar(((points%1000)/100) + '0');	//individually
	lcdSendChar(((points%100)/10) + '0');
	lcdSendChar((points%10) + '0');
	lcdWriteString(" PTS");
}

/***************************************************************************
 * Timing functions
 ***************************************************************************
 */

/**
 * @fn void updateWave()
 * @brief updates TIM2 parameters and LCD output based on
 * currentMode, frequency, dutyCycle
 *
 */
void updateWave() {
	switch (currentMode) {
		case SQUARE:
			stopTIM2();
			updateLCD();
			squareWave(frequency, dutyCycle);
			break;
		case SINE:
			stopTIM2();
			updateLCD();
			DAC_latch();
			sineWave(frequency);
			break;
		case RAMP:
			stopTIM2();
			updateLCD();
			DAC_latch();
			free(rampArray); // Remove old ramp array (free malloc)
			rampArray = createRampArray(frequency, polarity);
			rampWave(frequency);
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
	TIM2->SR &= ~TIM_SR_UIF; // clear the update interrupt flag
	latch = 1;
	/*	switch (currentMode) {
		case SQUARE:
			if (TIM2->SR & TIM_SR_UIF) {
				DAC_latch(); // Latch latest value to output on DAC
				TIM2->SR &= ~TIM_SR_UIF; // clear the update interrupt flag
				state = HIGH;
			}
			else //(TIM2->SR & TIM_SR_CC1IF)
			{
				DAC_latch(); // Latch latest value to output on DAC
				TIM2->SR &= ~TIM_SR_CC1IF; // clear the cc1 interrupt flag
				state = LOW;
			}
			square();
			break;
		case SINE:
			TIM2->SR &= ~TIM_SR_UIF; // clear the update interrupt flag
			idx++;
			//sine();
			latch = 1;
			break;
		case RAMP:
			TIM2->SR &= ~TIM_SR_UIF; // clear the update interrupt flag
			idx++;
			//ramp();
			latch = 1;
			break;
		default:
			break;
	 }*/
	if (currentMode == SQUARE) {
		DAC_latch();
		state = HIGH;
		if (TIM2->SR & TIM_SR_CC1IF) {
			TIM2->SR &= ~TIM_SR_CC1IF;
			state = LOW;
		}
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

  /** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK)
			{
		Error_Handler();
	}

  /** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_10;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
			{
		Error_Handler();
	}

  /** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
			{
		Error_Handler();
	}
}

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
