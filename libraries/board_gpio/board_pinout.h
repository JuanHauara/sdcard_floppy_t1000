#ifndef BOARD_PINOUT_H
#define BOARD_PINOUT_H


#include "stm32f4xx_hal.h"


/*---------------------------------------------------------------*/
/*-- Board GP input pins                                      -- */
/*---------------------------------------------------------------*/
#define BOARD_GPIO_PORT_DI0		GPIOB
#define BOARD_GPIO_PIN_DI0		GPIO_PIN_10

#define BOARD_GPIO_PORT_DI1		GPIOE
#define BOARD_GPIO_PIN_DI1		GPIO_PIN_10

#define BOARD_GPIO_PORT_DI2		GPIOE
#define BOARD_GPIO_PIN_DI2		GPIO_PIN_9

#define BOARD_GPIO_PORT_DI3		GPIOE
#define BOARD_GPIO_PIN_DI3		GPIO_PIN_8

#define BOARD_GPIO_PORT_DI4		GPIOD
#define BOARD_GPIO_PIN_DI4		GPIO_PIN_8

#define BOARD_GPIO_PORT_DI5		GPIOD
#define BOARD_GPIO_PIN_DI5		GPIO_PIN_9

#define BOARD_GPIO_PORT_DI6		GPIOD
#define BOARD_GPIO_PIN_DI6		GPIO_PIN_10

#define BOARD_GPIO_PORT_DI7		GPIOD
#define BOARD_GPIO_PIN_DI7		GPIO_PIN_11

#define BOARD_GPIO_PORT_DI8		GPIOD
#define BOARD_GPIO_PIN_DI8		GPIO_PIN_12

#define BOARD_GPIO_PORT_DI9		GPIOD
#define BOARD_GPIO_PIN_DI9		GPIO_PIN_13

#define BOARD_GPIO_PORT_DI10	GPIOD
#define BOARD_GPIO_PIN_DI10		GPIO_PIN_14

#define BOARD_GPIO_PORT_DI11	GPIOD
#define BOARD_GPIO_PIN_DI11		GPIO_PIN_15

#define BOARD_GPIO_PORT_DI12	GPIOC
#define BOARD_GPIO_PIN_DI12		GPIO_PIN_8

#define BOARD_GPIO_PORT_DI13	GPIOC
#define BOARD_GPIO_PIN_DI13		GPIO_PIN_9

#define BOARD_GPIO_PORT_DI14	GPIOA
#define BOARD_GPIO_PIN_DI14		GPIO_PIN_8

#define BOARD_GPIO_PORT_DI15	GPIOA
#define BOARD_GPIO_PIN_DI15		GPIO_PIN_10


/*---------------------------------------------------------------*/
/*-- Board GP output pins                                     -- */
/*---------------------------------------------------------------*/
#define BOARD_GPIO_PORT_DO0		GPIOA
#define BOARD_GPIO_PIN_DO0		GPIO_PIN_5

#define BOARD_GPIO_PORT_DO1		GPIOA
#define BOARD_GPIO_PIN_DO1		GPIO_PIN_4

#define BOARD_GPIO_PORT_DO2		GPIOA
#define BOARD_GPIO_PIN_DO2		GPIO_PIN_3

#define BOARD_GPIO_PORT_DO3		GPIOA
#define BOARD_GPIO_PIN_DO3		GPIO_PIN_2

#define BOARD_GPIO_PORT_DO4		GPIOA
#define BOARD_GPIO_PIN_DO4		GPIO_PIN_1

#define BOARD_GPIO_PORT_DO5		GPIOA
#define BOARD_GPIO_PIN_DO5		GPIO_PIN_0

#define BOARD_GPIO_PORT_DO6		GPIOC
#define BOARD_GPIO_PIN_DO6		GPIO_PIN_3

#define BOARD_GPIO_PORT_DO7		GPIOC
#define BOARD_GPIO_PIN_DO7		GPIO_PIN_2

#define BOARD_GPIO_PORT_DO8		GPIOC
#define BOARD_GPIO_PIN_DO8		GPIO_PIN_1

#define BOARD_GPIO_PORT_DO9		GPIOC
#define BOARD_GPIO_PIN_DO9		GPIO_PIN_0

#define BOARD_GPIO_PORT_DO10	GPIOC
#define BOARD_GPIO_PIN_DO10		GPIO_PIN_13

#define BOARD_GPIO_PORT_DO11	GPIOE
#define BOARD_GPIO_PIN_DO11		GPIO_PIN_6

#define BOARD_GPIO_PORT_DO12	GPIOE
#define BOARD_GPIO_PIN_DO12		GPIO_PIN_5

#define BOARD_GPIO_PORT_DO13	GPIOE
#define BOARD_GPIO_PIN_DO13		GPIO_PIN_4

#define BOARD_GPIO_PORT_DO14	GPIOE
#define BOARD_GPIO_PIN_DO14		GPIO_PIN_3

#define BOARD_GPIO_PORT_DO15	GPIOE
#define BOARD_GPIO_PIN_DO15		GPIO_PIN_2


/*---------------------------------------------------------------*/
/*-- Board internal pins                                      -- */
/*---------------------------------------------------------------*/
#define BOARD_GPIO_PORT_DO_STATUS_LED			GPIOA
#define BOARD_GPIO_PIN_DO_STATUS_LED			GPIO_PIN_3

// SD card
#define BOARD_GPIO_PORT_DO_SDCARD_SPI_CS		GPIOA
#define BOARD_GPIO_PIN_DO_SDCARD_SPI_CS			GPIO_PIN_4

#define BOARD_GPIO_PORT_DO_SDCARD_ON			GPIOC
#define BOARD_GPIO_PIN_DO_SDCARD_ON			    GPIO_PIN_4


#endif  // BOARD_PINOUT_H