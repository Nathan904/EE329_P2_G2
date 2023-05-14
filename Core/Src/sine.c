/**
 * @file sine.c
 * @brief
 *
 * Detailed Description
 *
 * @author Nathan Vaniman
 * @date May 8, 2023
 */


#include "sine.h"
#include "timer.h"

/**
 * @fn void sineWave(uint32_t)
 * @brief initializes TIM2 for sine wave
 * @note frequency parameter now arbitrary since array size is variable
 * @param frequency [Hz]
 */
void sineWave(uint32_t frequency) {
	uint32_t reloadVal = (SystemCoreClock) / (SINE_SIZE * 100);
	initTIM2(reloadVal);
}

/**
 * @fn uint16_t createSmallArrays**(uint16_t*)
 * @brief creates array of vectors for each frequency (FREQUENCY_COUNT)
 * @param sineData : Original SINE_DATA array (max samples @ fmin)
 * @return uint16_t
 */
uint16_t** createSmallArrays(uint16_t *sineData) {
	/**
	 * @brief Allocate memory for arrays
	 * The number of small arrays is equal to FREQUENCY_COUNT + 1
	 * Each array is a pointer to a uint16_t
	 * @note malloc is used because the array sizes are determined at
	 * run-time. This allows for someone to easily modify frequencies used
	 */
	uint16_t **smallArrays = malloc(
			(FREQUENCY_COUNT + 1) * sizeof(uint16_t*));

	// Set first pointer in new array equal to original array's pointer
	smallArrays[0] = sineData;

	// loop for each frequency
	for (size_t i = 0; i < FREQUENCY_COUNT; i++) {
		// Get size of smaller array
		size_t newSize = SINE_SIZE / (i + 2);

		// Allocate memory for smallerArray
		uint16_t *smallerArray = malloc(newSize * sizeof(uint16_t));

		// Fill smallerArray with sineData values
		// Elements used are j*(i+2)
		for (size_t j = 0; j < newSize; j++) {
			smallerArray[j] = sineData[j * (i + 2)];
		}

		// Add smallerArray to 2d array
        smallArrays[i + 1] = smallerArray;
	}
	// Return 2d uint16_t array pointer
    return smallArrays;
}
