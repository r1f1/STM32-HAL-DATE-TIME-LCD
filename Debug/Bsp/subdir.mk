################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Bsp/keypad.c \
../Bsp/lcd.c \
../Bsp/rtc.c 

OBJS += \
./Bsp/keypad.o \
./Bsp/lcd.o \
./Bsp/rtc.o 

C_DEPS += \
./Bsp/keypad.d \
./Bsp/lcd.d \
./Bsp/rtc.d 


# Each subdirectory must supply rules for building sources it contributes
Bsp/%.o Bsp/%.su Bsp/%.cyclo: ../Bsp/%.c Bsp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/rafae/Documents/Embedded-C/STM32-HAL-DATE-TIME-LCD/Bsp" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Bsp

clean-Bsp:
	-$(RM) ./Bsp/keypad.cyclo ./Bsp/keypad.d ./Bsp/keypad.o ./Bsp/keypad.su ./Bsp/lcd.cyclo ./Bsp/lcd.d ./Bsp/lcd.o ./Bsp/lcd.su ./Bsp/rtc.cyclo ./Bsp/rtc.d ./Bsp/rtc.o ./Bsp/rtc.su

.PHONY: clean-Bsp

