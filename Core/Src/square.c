/**
 * @file square.c
 * @brief Square wave functions
 *
 * Square wave functions to calculate/modify frequency/duty cycle
 *
 * @author Nathan Vaniman
 * @date May 8, 2023
 */
#include "main.h"
#include "square.h"
#include "timer.h"

uint32_t reloadVal; // Timer ARR value (based on desired frequency)



/**
 * @fn void squareWave(uint32_t, float)
 * @brief
 *
 * @param frequency [Hz]
 * @param dutyCycle [%] as float (e.g. 0.5f)
 */
void squareWave(uint32_t frequency, float dutyCycle) {


	reloadVal = (TIM2_CLK_SPEED / frequency);
	uint32_t captureVal = getDutyCycle(dutyCycle);
	setupTIM2();
	updateSquareTIM2(reloadVal, captureVal);

}

/**
 * @fn uint32_t getDutyCycle(float)
 * @brief Calculates timer capture/compare value based on reloadVal &
 * dutyCycle
 *
 * @param dutyCycle [%] as float (e.g. 0.5f)
 * @return uint32_t val : value to use as CCR1 for TIMER
 */
uint32_t getDutyCycle(float dutyCycle) {


	uint32_t val = ((uint32_t) (reloadVal * dutyCycle));
	return (val);


}
