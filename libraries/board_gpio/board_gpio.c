#include "board_gpio.h"

#include "debug_log.h"
#include "stm32f4xx_hal.h"


// Digital pins
bool board_gpio_read_di(board_gpio_di_pin_t pin)
{
	switch (pin)
	{
		case DI0:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI0, BOARD_GPIO_PIN_DI0);
		case DI1:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI1, BOARD_GPIO_PIN_DI1);
		case DI2:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI2, BOARD_GPIO_PIN_DI2);
		case DI3:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI3, BOARD_GPIO_PIN_DI3);
		case DI4:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI4, BOARD_GPIO_PIN_DI4);
		case DI5:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI5, BOARD_GPIO_PIN_DI5);
		case DI6:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI6, BOARD_GPIO_PIN_DI6);
		case DI7:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI7, BOARD_GPIO_PIN_DI7);
		case DI8:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI8, BOARD_GPIO_PIN_DI8);
		case DI9:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI9, BOARD_GPIO_PIN_DI9);
		case DI10:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI10, BOARD_GPIO_PIN_DI10);
		case DI11:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI11, BOARD_GPIO_PIN_DI11);
		case DI12:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI12, BOARD_GPIO_PIN_DI12);
		case DI13:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI13, BOARD_GPIO_PIN_DI13);
		case DI14:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI14, BOARD_GPIO_PIN_DI14);
		case DI15:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DI15, BOARD_GPIO_PIN_DI15);

		default:
			return false;
	}
}

bool board_gpio_read_do(board_gpio_do_pin_t pin)
{
	switch (pin)
	{
		case DO0:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO0, BOARD_GPIO_PIN_DO0);
		case DO1:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO1, BOARD_GPIO_PIN_DO1);
		case DO2:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO2, BOARD_GPIO_PIN_DO2);
		case DO3:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO3, BOARD_GPIO_PIN_DO3);
		case DO4:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO4, BOARD_GPIO_PIN_DO4);
		case DO5:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO5, BOARD_GPIO_PIN_DO5);
		case DO6:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO6, BOARD_GPIO_PIN_DO6);
		case DO7:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO7, BOARD_GPIO_PIN_DO7);
		case DO8:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO8, BOARD_GPIO_PIN_DO8);
		case DO9:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO9, BOARD_GPIO_PIN_DO9);
		case DO10:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO10, BOARD_GPIO_PIN_DO10);
		case DO11:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO11, BOARD_GPIO_PIN_DO11);
		case DO12:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO12, BOARD_GPIO_PIN_DO12);
		case DO13:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO13, BOARD_GPIO_PIN_DO13);
		case DO14:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO14, BOARD_GPIO_PIN_DO14);
		case DO15:	return (bool)HAL_GPIO_ReadPin(BOARD_GPIO_PORT_DO15, BOARD_GPIO_PIN_DO15);

		default:
			return false;
	}
}

void board_gpio_write_do(board_gpio_do_pin_t pin, bool pin_state)
{
	switch (pin)
	{
		case DO0:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO0, BOARD_GPIO_PIN_DO0, (GPIO_PinState)pin_state);		break;
		case DO1:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO1, BOARD_GPIO_PIN_DO1, (GPIO_PinState)pin_state);		break;
		case DO2:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO2, BOARD_GPIO_PIN_DO2, (GPIO_PinState)pin_state);		break;
		case DO3:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO3, BOARD_GPIO_PIN_DO3, (GPIO_PinState)pin_state);		break;
		case DO4:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO4, BOARD_GPIO_PIN_DO4, (GPIO_PinState)pin_state);		break;
		case DO5:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO5, BOARD_GPIO_PIN_DO5, (GPIO_PinState)pin_state);		break;
		case DO6:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO6, BOARD_GPIO_PIN_DO6, (GPIO_PinState)pin_state);		break;
		case DO7:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO7, BOARD_GPIO_PIN_DO7, (GPIO_PinState)pin_state);		break;
		case DO8:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO8, BOARD_GPIO_PIN_DO8, (GPIO_PinState)pin_state);		break;
		case DO9:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO9, BOARD_GPIO_PIN_DO9, (GPIO_PinState)pin_state);		break;
		case DO10:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO10, BOARD_GPIO_PIN_DO10, (GPIO_PinState)pin_state);		break;
		case DO11:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO11, BOARD_GPIO_PIN_DO11, (GPIO_PinState)pin_state);		break;
		case DO12:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO12, BOARD_GPIO_PIN_DO12, (GPIO_PinState)pin_state);		break;
		case DO13:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO13, BOARD_GPIO_PIN_DO13, (GPIO_PinState)pin_state);		break;
		case DO14:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO14, BOARD_GPIO_PIN_DO14, (GPIO_PinState)pin_state);		break;
		case DO15:	HAL_GPIO_WritePin(BOARD_GPIO_PORT_DO15, BOARD_GPIO_PIN_DO15, (GPIO_PinState)pin_state);		break;

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
		case DO0:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO0, BOARD_GPIO_PIN_DO0);		break;
		case DO1:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO1, BOARD_GPIO_PIN_DO1);		break;
		case DO2:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO2, BOARD_GPIO_PIN_DO2);		break;
		case DO3:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO3, BOARD_GPIO_PIN_DO3);		break;
		case DO4:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO4, BOARD_GPIO_PIN_DO4);		break;
		case DO5:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO5, BOARD_GPIO_PIN_DO5);		break;
		case DO6:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO6, BOARD_GPIO_PIN_DO6);		break;
		case DO7:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO7, BOARD_GPIO_PIN_DO7);		break;
		case DO8:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO8, BOARD_GPIO_PIN_DO8);		break;
		case DO9:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO9, BOARD_GPIO_PIN_DO9);		break;
		case DO10:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO10, BOARD_GPIO_PIN_DO10);		break;
		case DO11:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO11, BOARD_GPIO_PIN_DO11);		break;
		case DO12:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO12, BOARD_GPIO_PIN_DO12);		break;
		case DO13:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO13, BOARD_GPIO_PIN_DO13);		break;
		case DO14:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO14, BOARD_GPIO_PIN_DO14);		break;
		case DO15:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO15, BOARD_GPIO_PIN_DO15);		break;

		case STATUS_LED:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO_STATUS_LED, BOARD_GPIO_PIN_DO_STATUS_LED);			break;
		case SDCARD_SPI_CS:	HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO_SDCARD_SPI_CS, BOARD_GPIO_PIN_DO_SDCARD_SPI_CS);		break;
		case SDCARD_ON:		HAL_GPIO_TogglePin(BOARD_GPIO_PORT_DO_SDCARD_ON, BOARD_GPIO_PIN_DO_SDCARD_ON);				break;
	
		default:
		break;
	}
}

