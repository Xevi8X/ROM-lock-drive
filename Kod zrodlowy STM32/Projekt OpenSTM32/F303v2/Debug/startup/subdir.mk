################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32f303x8.s 

OBJS += \
./startup/startup_stm32f303x8.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -I"C:/Users/Wojtek/Desktop/Praca przejsciowa/F303v2/HAL_Driver/Inc/Legacy" -I"C:/Users/Wojtek/Desktop/Praca przejsciowa/F303v2/inc" -I"C:/Users/Wojtek/Desktop/Praca przejsciowa/F303v2/CMSIS/device" -I"C:/Users/Wojtek/Desktop/Praca przejsciowa/F303v2/CMSIS/core" -I"C:/Users/Wojtek/Desktop/Praca przejsciowa/F303v2/Utilities/STM32F3xx_Nucleo_32" -I"C:/Users/Wojtek/Desktop/Praca przejsciowa/F303v2/HAL_Driver/Inc" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


