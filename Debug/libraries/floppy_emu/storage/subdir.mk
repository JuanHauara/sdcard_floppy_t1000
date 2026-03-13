################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libraries/floppy_emu/storage/fixed_image.c 

OBJS += \
./libraries/floppy_emu/storage/fixed_image.o 

C_DEPS += \
./libraries/floppy_emu/storage/fixed_image.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/floppy_emu/storage/%.o libraries/floppy_emu/storage/%.su libraries/floppy_emu/storage/%.cyclo: ../libraries/floppy_emu/storage/%.c libraries/floppy_emu/storage/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../libraries/board_gpio -I../libraries/debug_log -I../libraries/sd_card -I../libraries/utils -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I../libraries/floppy_emu -I"../libraries/floppy_emu/core" -I"../libraries/floppy_emu/iface" -I"../libraries/floppy_emu/storage" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-libraries-2f-floppy_emu-2f-storage

clean-libraries-2f-floppy_emu-2f-storage:
	-$(RM) ./libraries/floppy_emu/storage/fixed_image.cyclo ./libraries/floppy_emu/storage/fixed_image.d ./libraries/floppy_emu/storage/fixed_image.o ./libraries/floppy_emu/storage/fixed_image.su

.PHONY: clean-libraries-2f-floppy_emu-2f-storage

