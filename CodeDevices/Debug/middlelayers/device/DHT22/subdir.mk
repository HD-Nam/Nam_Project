################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../middlelayers/device/DHT22/DHT22.c 

OBJS += \
./middlelayers/device/DHT22/DHT22.o 

C_DEPS += \
./middlelayers/device/DHT22/DHT22.d 


# Each subdirectory must supply rules for building sources it contributes
middlelayers/device/DHT22/%.o middlelayers/device/DHT22/%.su middlelayers/device/DHT22/%.cyclo: ../middlelayers/device/DHT22/%.c middlelayers/device/DHT22/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"F:/IoT Challenge 2024/idji/middlelayers/lo_ptc" -I"F:/IoT Challenge 2024/idji/middlelayers/crc" -I"F:/IoT Challenge 2024/idji/middlelayers/buffer" -I"F:/IoT Challenge 2024/idji/App" -I"F:/IoT Challenge 2024/idji/middlelayers/acp" -I"F:/IoT Challenge 2024/idji/modules/uart" -I"F:/IoT Challenge 2024/idji/middlelayers/device/DHT22" -I"F:/IoT Challenge 2024/idji/middlelayers/device/MQ135" -I"F:/IoT Challenge 2024/idji/middlelayers/device/PM25" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-middlelayers-2f-device-2f-DHT22

clean-middlelayers-2f-device-2f-DHT22:
	-$(RM) ./middlelayers/device/DHT22/DHT22.cyclo ./middlelayers/device/DHT22/DHT22.d ./middlelayers/device/DHT22/DHT22.o ./middlelayers/device/DHT22/DHT22.su

.PHONY: clean-middlelayers-2f-device-2f-DHT22

