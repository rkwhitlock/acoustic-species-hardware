################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/BSP/Components/otm8009a/otm8009a.c \
/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/BSP/Components/otm8009a/otm8009a_reg.c 

OBJS += \
./Components/otm8009a/otm8009a.o \
./Components/otm8009a/otm8009a_reg.o 

C_DEPS += \
./Components/otm8009a/otm8009a.d \
./Components/otm8009a/otm8009a_reg.d 


# Each subdirectory must supply rules for building sources it contributes
Components/otm8009a/otm8009a.o: /Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/BSP/Components/otm8009a/otm8009a.c Components/otm8009a/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I../Core/Inc -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Device/ST/STM32H7xx/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/Inc" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Utilities/lcd" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/BSP/Components/Common" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/BSP/STM32H747I-DISCO" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Utilities/Fonts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Components/otm8009a/otm8009a_reg.o: /Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/BSP/Components/otm8009a/otm8009a_reg.c Components/otm8009a/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I../Core/Inc -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Device/ST/STM32H7xx/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/Inc" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Utilities/lcd" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/BSP/Components/Common" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/BSP/STM32H747I-DISCO" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Utilities/Fonts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Components-2f-otm8009a

clean-Components-2f-otm8009a:
	-$(RM) ./Components/otm8009a/otm8009a.cyclo ./Components/otm8009a/otm8009a.d ./Components/otm8009a/otm8009a.o ./Components/otm8009a/otm8009a.su ./Components/otm8009a/otm8009a_reg.cyclo ./Components/otm8009a/otm8009a_reg.d ./Components/otm8009a/otm8009a_reg.o ./Components/otm8009a/otm8009a_reg.su

.PHONY: clean-Components-2f-otm8009a

