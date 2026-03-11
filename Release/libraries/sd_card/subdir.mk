################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libraries/sd_card/sd_card.c \
../libraries/sd_card/sd_spi.c 

OBJS += \
./libraries/sd_card/sd_card.o \
./libraries/sd_card/sd_spi.o 

C_DEPS += \
./libraries/sd_card/sd_card.d \
./libraries/sd_card/sd_spi.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/sd_card/%.o libraries/sd_card/%.su libraries/sd_card/%.cyclo: ../libraries/sd_card/%.c libraries/sd_card/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../libraries/board_gpio -I../libraries/debug_log -I../libraries/sd_card -I../libraries/utils -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-libraries-2f-sd_card

clean-libraries-2f-sd_card:
	-$(RM) ./libraries/sd_card/sd_card.cyclo ./libraries/sd_card/sd_card.d ./libraries/sd_card/sd_card.o ./libraries/sd_card/sd_card.su ./libraries/sd_card/sd_spi.cyclo ./libraries/sd_card/sd_spi.d ./libraries/sd_card/sd_spi.o ./libraries/sd_card/sd_spi.su

.PHONY: clean-libraries-2f-sd_card

