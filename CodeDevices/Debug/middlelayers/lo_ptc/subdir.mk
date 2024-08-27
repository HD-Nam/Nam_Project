################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../middlelayers/lo_ptc/lo_ptc.c 

OBJS += \
./middlelayers/lo_ptc/lo_ptc.o 

C_DEPS += \
./middlelayers/lo_ptc/lo_ptc.d 


# Each subdirectory must supply rules for building sources it contributes
middlelayers/lo_ptc/%.o middlelayers/lo_ptc/%.su middlelayers/lo_ptc/%.cyclo: ../middlelayers/lo_ptc/%.c middlelayers/lo_ptc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"F:/IoT Challenge 2024/idji/middlelayers/lo_ptc" -I"F:/IoT Challenge 2024/idji/middlelayers/crc" -I"F:/IoT Challenge 2024/idji/middlelayers/buffer" -I"F:/IoT Challenge 2024/idji/App" -I"F:/IoT Challenge 2024/idji/middlelayers/acp" -I"F:/IoT Challenge 2024/idji/modules/uart" -I"F:/IoT Challenge 2024/idji/middlelayers/device/DHT22" -I"F:/IoT Challenge 2024/idji/middlelayers/device/MQ135" -I"F:/IoT Challenge 2024/idji/middlelayers/device/PM25" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-middlelayers-2f-lo_ptc

clean-middlelayers-2f-lo_ptc:
	-$(RM) ./middlelayers/lo_ptc/lo_ptc.cyclo ./middlelayers/lo_ptc/lo_ptc.d ./middlelayers/lo_ptc/lo_ptc.o ./middlelayers/lo_ptc/lo_ptc.su

.PHONY: clean-middlelayers-2f-lo_ptc

