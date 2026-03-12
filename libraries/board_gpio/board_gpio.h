#ifndef BOARD_GPIO_H
#define BOARD_GPIO_H


#include <stdint.h>
#include <stdbool.h>

#include "board_pinout.h"


typedef enum 
{
	FDD_FDSELA = 0,
	FDD_MONA,
	FDD_LOWDNS,
	FDD_FDCDRC,
	FDD_STEP,
	FDD_WDATA,
	FDD_WGATE,
	FDD_SIDE,
} board_gpio_di_pin_t;

typedef enum 
{
	/* FDD interface output pins */
	FDD_INDEX = 0,
	FDD_DSKCHG,
	FDD_READY,
	FDD_MEDIA,
	FDD_TRACK0,
	FDD_WPROTC,
	FDD_RDDA,

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
