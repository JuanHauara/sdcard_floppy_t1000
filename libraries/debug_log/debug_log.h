/*
	Minimal and portable debug logging system.

	Usage:
	- Set ENABLE_SERIAL_DEBUG to 1 to enable debug messages, or 0 to disable them.
	- Use DEBUG_SERIAL_LOG(...) in your code like printf to print formatted debug messages.
	- When disabled, DEBUG_SERIAL_LOG(...) expands to ((void)0), generating no code.

	How to use:
	1. In your debug_log.c file, implement:
		static void send(const char *cstr);

		This function is responsible for sending the formatted string to your desired output
		(e.g., USB CDC, UART, etc.).

	Example usage:
		DEBUG_SERIAL_LOG("Sensor value: %d\n", sensor_value);

	Example backend (USB CDC - STM32CubeIDE):
		static void send(const char *cstr)
		{
			CDC_Transmit_FS((uint8_t *)cstr, strlen(cstr));
		}

	Example backend (UART, huart2 - STM32CubeIDE):
		extern UART_HandleTypeDef huart2;
		
		static void send(const char *cstr)
		{
			HAL_UART_Transmit(&huart2, (uint8_t *)cstr, strlen(cstr), HAL_MAX_DELAY);
		}
*/

#ifndef DEBUG_SERIAL_LOG_H
#define DEBUG_SERIAL_LOG_H


#include <stdarg.h>
#include <stdio.h>


#define ENABLE_SERIAL_DEBUG		1		// Global flag to enable/disable debug logging.
#define MAX_LEN_CSTR_BUFFER		512		// Maximum size in Bytes of the debug message to be transmitted.


#if ENABLE_SERIAL_DEBUG
	void debug_log_format(const char *fmt, ...);
	
	#define DEBUG_SERIAL_LOG(...) debug_log_format(__VA_ARGS__)
#else
	// If ENABLE_SERIAL_DEBUG is disabled, no code is generated.
	#define DEBUG_SERIAL_LOG(...) ((void)0)
#endif  // ENABLE_SERIAL_DEBUG


#endif  // DEBUG_SERIAL_LOG_H
