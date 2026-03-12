#include "board_gpio.h"

#include "debug_log.h"
#include "stm32f4xx_hal.h"


// Digital pins
bool board_gpio_read_di(board_gpio_di_pin_t pin)
{
	switch (pin)
	{
		case FDD_FDSELA:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI0, BOARD_GPIO_PIN_DI0);
		case FDD_MONA:		return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI1, BOARD_GPIO_PIN_DI1);
		case FDD_LOWDNS:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI2, BOARD_GPIO_PIN_DI2);
		case FDD_FDCDRC:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI3, BOARD_GPIO_PIN_DI3);
		case FDD_STEP:		return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI4, BOARD_GPIO_PIN_DI4);
		case FDD_WDATA:		return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI5, BOARD_GPIO_PIN_DI5);
		case FDD_WGATE:		return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI6, BOARD_GPIO_PIN_DI6);
		case FDD_SIDE:		return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI7, BOARD_GPIO_PIN_DI7);

		default:
			return false;
	}
}

bool board_gpio_read_do(board_gpio_do_pin_t pin)
{
	switch (pin)
	{
		case FDD_INDEX:		return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO0, BOARD_GPIO_PIN_DO0);
		case FDD_DSKCHG:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO1, BOARD_GPIO_PIN_DO1);
		case FDD_READY:		return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO2, BOARD_GPIO_PIN_DO2);
		case FDD_MEDIA:		return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO3, BOARD_GPIO_PIN_DO3);
		case FDD_TRACK0:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO4, BOARD_GPIO_PIN_DO4);
		case FDD_WPROTC:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO5, BOARD_GPIO_PIN_DO5);
		case FDD_RDDA:		return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO6, BOARD_GPIO_PIN_DO6);

		default:
			return false;
	}
}

void board_gpio_write_do(board_gpio_do_pin_t pin, bool pin_state)
{
	switch (pin)
	{
		case FDD_INDEX:		HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO0, BOARD_GPIO_PIN_DO0, (GPIO_PinState)pin_state);		break;
		case FDD_DSKCHG:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO1, BOARD_GPIO_PIN_DO1, (GPIO_PinState)pin_state);		break;
		case FDD_READY:		HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO2, BOARD_GPIO_PIN_DO2, (GPIO_PinState)pin_state);		break;
		case FDD_MEDIA:		HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO3, BOARD_GPIO_PIN_DO3, (GPIO_PinState)pin_state);		break;
		case FDD_TRACK0:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO4, BOARD_GPIO_PIN_DO4, (GPIO_PinState)pin_state);		break;
		case FDD_WPROTC:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO5, BOARD_GPIO_PIN_DO5, (GPIO_PinState)pin_state);		break;
		case FDD_RDDA:		HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO6, BOARD_GPIO_PIN_DO6, (GPIO_PinState)pin_state);		break;

		case STATUS_LED:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO_STATUS_LED, BOARD_GPIO_PIN_DO_STATUS_LED, (GPIO_PinState)pin_state);			break;
		case SDCARD_SPI_CS:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO_SDCARD_SPI_CS, BOARD_GPIO_PIN_DO_SDCARD_SPI_CS, (GPIO_PinState)pin_state);		break;
		case SDCARD_ON:		HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO_SDCARD_ON, BOARD_GPIO_PIN_DO_SDCARD_ON, (GPIO_PinState)pin_state);				break;

		default:
		break;
	}
}

void board_gpio_toggle_do(board_gpio_do_pin_t pin)
{
	switch (pin)
	{
		case FDD_INDEX:		HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO0, BOARD_GPIO_PIN_DO0);		break;
		case FDD_DSKCHG:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO1, BOARD_GPIO_PIN_DO1);		break;
		case FDD_READY:		HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO2, BOARD_GPIO_PIN_DO2);		break;
		case FDD_MEDIA:		HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO3, BOARD_GPIO_PIN_DO3);		break;
		case FDD_TRACK0:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO4, BOARD_GPIO_PIN_DO4);		break;
		case FDD_WPROTC:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO5, BOARD_GPIO_PIN_DO5);		break;
		case FDD_RDDA:		HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO6, BOARD_GPIO_PIN_DO6);		break;

		case STATUS_LED:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO_STATUS_LED, BOARD_GPIO_PIN_DO_STATUS_LED);			break;
		case SDCARD_SPI_CS:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO_SDCARD_SPI_CS, BOARD_GPIO_PIN_DO_SDCARD_SPI_CS);		break;
		case SDCARD_ON:		HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO_SDCARD_ON, BOARD_GPIO_PIN_DO_SDCARD_ON);				break;
	
		default:
		break;
	}
}

