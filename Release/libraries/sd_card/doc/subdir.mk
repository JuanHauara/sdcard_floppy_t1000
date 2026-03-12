################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libraries/sd_card/doc/user_diskio.c 

OBJS += \
./libraries/sd_card/doc/user_diskio.o 

C_DEPS += \
./libraries/sd_card/doc/user_diskio.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/sd_card/doc/%.o libraries/sd_card/doc/%.su libraries/sd_card/doc/%.cyclo: ../libraries/sd_card/doc/%.c libraries/sd_card/doc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../libraries/board_gpio -I../libraries/debug_log -I../libraries/sd_card -I../libraries/utils -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-libraries-2f-sd_card-2f-doc

clean-libraries-2f-sd_card-2f-doc:
	-$(RM) ./libraries/sd_card/doc/user_diskio.cyclo ./libraries/sd_card/doc/user_diskio.d ./libraries/sd_card/doc/user_diskio.o ./libraries/sd_card/doc/user_diskio.su

.PHONY: clean-libraries-2f-sd_card-2f-doc

