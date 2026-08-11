/*
	Minimal and portable debug logging system.

	Usage:
	- Set DEBUG_LOG_ENABLE_SERIAL_DEBUG to 1 to enable debug messages, or 0 to disable them.
	- Use DEBUG_SERIAL_LOG(...) in your code like printf to print formatted debug messages.
	- When disabled, DEBUG_SERIAL_LOG(...) expands to ((void)0), generating no code.
	- To view debug messages on the PC, enable the "Enable DTR" option in the terminal 
	  application on the PC.

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


#define DEBUG_LOG_ENABLE_SERIAL_DEBUG	1		// Global flag to enable/disable debug logging.
#define DEBUG_LOG_MAX_LEN_CSTR_BUFFER	300		// Maximum size in Bytes of the debug message to be transmitted.
/*
	Maximum time to wait for the USB port to finish sending the previous
	log before sending the next one.
	Increase this value if logs are printed incompletely or are not
	being transmitted.
*/
#define DEBUG_LOG_SEND_TIMEOUT_MS		50


#if DEBUG_LOG_ENABLE_SERIAL_DEBUG
	void debug_log_format(const char *fmt, ...);
	
	#define DEBUG_SERIAL_LOG(...) debug_log_format(__VA_ARGS__)
#else
	// If DEBUG_LOG_ENABLE_SERIAL_DEBUG is disabled, no code is generated.
	#define DEBUG_SERIAL_LOG(...) ((void)0)
#endif  // DEBUG_LOG_ENABLE_SERIAL_DEBUG


#endif  // DEBUG_SERIAL_LOG_H
