################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/Libraries/STM32CubeH7/Utilities/Lcd_Trace/lcd_trace.c 

OBJS += \
./Utilities/Lcd_Trace/lcd_trace.o 

C_DEPS += \
./Utilities/Lcd_Trace/lcd_trace.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/Lcd_Trace/lcd_trace.o: /Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/Libraries/STM32CubeH7/Utilities/Lcd_Trace/lcd_trace.c Utilities/Lcd_Trace/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I../Core/Inc -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Device/ST/STM32H7xx/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/Inc" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Utilities/lcd" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/BSP/Components/Common" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/BSP/STM32H747I-DISCO" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Utilities/Fonts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Utilities-2f-Lcd_Trace

clean-Utilities-2f-Lcd_Trace:
	-$(RM) ./Utilities/Lcd_Trace/lcd_trace.cyclo ./Utilities/Lcd_Trace/lcd_trace.d ./Utilities/Lcd_Trace/lcd_trace.o ./Utilities/Lcd_Trace/lcd_trace.su

.PHONY: clean-Utilities-2f-Lcd_Trace

