#include "utils.h"

#include <string.h>
#include <math.h>

#include "stm32f4xx_hal.h"


/* Global variables */
extern TIM_HandleTypeDef htim2;


/* --------- Implement these functions --------- */
void delay_ms(uint32_t ms)
{
	/* --- Implement this function --- */

	HAL_Delay(ms);
}

void delay_us(uint32_t us)
{
	/* --- Implement this function --- */

	/*
		delay_us() generates a blocking delay in microseconds using TIM2.

		TIM2 is configured as a free-running 32-bit timer at 1 MHz (1 tick = 1 µs).
		The function captures the current counter value and waits until the desired
		number of microseconds has elapsed.

		Overflow handling:
		Since TIM2 is 32-bit, it will overflow approximately every 4295 seconds at 1 MHz.
		The subtraction between the current counter and the start value is performed
		using unsigned arithmetic (uint32_t), which naturally handles overflow without
		requiring additional logic.
	*/

	uint32_t ti = __HAL_TIM_GET_COUNTER(&htim2);
	while ( (uint32_t)(__HAL_TIM_GET_COUNTER(&htim2) - ti) < us ) 
	{
	}
}

uint32_t get_ms(void)
{
	/* --- Implement this function --- */

	return HAL_GetTick();
}

uint32_t get_us(void)
{
	/* --- Implement this function --- */

	/*
		get_us() returns the current timestamp in microseconds
		based on TIM2 configured as a free-running 32-bit timer at 1 MHz
		(1 tick = 1 µs).

		Overflow behavior:
		TIM2 overflows approximately every 4295 seconds (~71 minutes).
		The returned value is a 32-bit unsigned integer (uint32_t). When
		measuring elapsed time, always store both the start and end
		timestamps as uint32_t and perform the subtraction in unsigned
		arithmetic:

			uint32_t start = get_us();
			...
			uint32_t elapsed = get_us() - start;

		This approach handles wrap-around automatically without additional logic.
	*/

	return __HAL_TIM_GET_COUNTER(&htim2);
}
/* --------------------------------------------- */

