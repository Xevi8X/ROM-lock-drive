################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/STM32F3xx_Nucleo_32/stm32f3xx_nucleo_32.c 

OBJS += \
./Utilities/STM32F3xx_Nucleo_32/stm32f3xx_nucleo_32.o 

C_DEPS += \
./Utilities/STM32F3xx_Nucleo_32/stm32f3xx_nucleo_32.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/STM32F3xx_Nucleo_32/%.o: ../Utilities/STM32F3xx_Nucleo_32/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F3 -DSTM32F30 -DSTM32F303K8Tx -DNUCLEO_F303K8 -DDEBUG -DSTM32F303x8 -DUSE_HAL_DRIVER -I"C:/Users/Wojtek/Desktop/Praca przejsciowa/F303v2/HAL_Driver/Inc/Legacy" -I"C:/Users/Wojtek/Desktop/Praca przejsciowa/F303v2/inc" -I"C:/Users/Wojtek/Desktop/Praca przejsciowa/F303v2/CMSIS/device" -I"C:/Users/Wojtek/Desktop/Praca przejsciowa/F303v2/CMSIS/core" -I"C:/Users/Wojtek/Desktop/Praca przejsciowa/F303v2/Utilities/STM32F3xx_Nucleo_32" -I"C:/Users/Wojtek/Desktop/Praca przejsciowa/F303v2/HAL_Driver/Inc" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


