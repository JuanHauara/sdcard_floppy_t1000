#ifndef BOARD_GPIO_H
#define BOARD_GPIO_H


#include <stdint.h>
#include <stdbool.h>

#include "board_pinout.h"


typedef enum 
{
	DI0 = 0,
	DI1, 
	DI2, 
	DI3, 
	DI4, 
	DI5, 
	DI6, 
	DI7, 
	DI8, 
	DI9, 
	DI10, 
	DI11, 
	DI12, 
	DI13, 
	DI14, 
	DI15, 
} board_gpio_di_pin_t;

typedef enum 
{
	/* Board GP output pins */
	DO0 = 0,
	DO1, 
	DO2, 
	DO3, 
	DO4, 
	DO5, 
	DO6, 
	DO7, 
	DO8, 
	DO9, 
	DO10, 
	DO11, 
	DO12, 
	DO13, 
	DO14, 
	DO15, 

	/* Board internal pins */
	STATUS_LED, 
	SDCARD_SPI_CS, 
	SDCARD_ON, 
} board_gpio_do_pin_t;


// Digital pins
bool board_gpio_read_di(board_gpio_di_pin_t pin);
bool board_gpio_read_do(board_gpio_do_pin_t pin);
void board_gpio_write_do(board_gpio_do_pin_t pin, bool pin_state);
void board_gpio_toggle_do(board_gpio_do_pin_t pin);


#endif  // BOARD_GPIO_H