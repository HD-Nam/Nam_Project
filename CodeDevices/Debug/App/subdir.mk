################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/FP_session.c \
../App/Sleep_session.c \
../App/mainApp.c 

OBJS += \
./App/FP_session.o \
./App/Sleep_session.o \
./App/mainApp.o 

C_DEPS += \
./App/FP_session.d \
./App/Sleep_session.d \
./App/mainApp.d 


# Each subdirectory must supply rules for building sources it contributes
App/%.o App/%.su App/%.cyclo: ../App/%.c App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"F:/IoT Challenge 2024/idji/middlelayers/lo_ptc" -I"F:/IoT Challenge 2024/idji/middlelayers/crc" -I"F:/IoT Challenge 2024/idji/middlelayers/buffer" -I"F:/IoT Challenge 2024/idji/App" -I"F:/IoT Challenge 2024/idji/middlelayers/acp" -I"F:/IoT Challenge 2024/idji/modules/uart" -I"F:/IoT Challenge 2024/idji/middlelayers/device/DHT22" -I"F:/IoT Challenge 2024/idji/middlelayers/device/MQ135" -I"F:/IoT Challenge 2024/idji/middlelayers/device/PM25" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-App

clean-App:
	-$(RM) ./App/FP_session.cyclo ./App/FP_session.d ./App/FP_session.o ./App/FP_session.su ./App/Sleep_session.cyclo ./App/Sleep_session.d ./App/Sleep_session.o ./App/Sleep_session.su ./App/mainApp.cyclo ./App/mainApp.d ./App/mainApp.o ./App/mainApp.su

.PHONY: clean-App

