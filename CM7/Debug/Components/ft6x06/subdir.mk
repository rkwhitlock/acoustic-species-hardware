################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/BSP/Components/ft6x06/ft6x06.c \
/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/BSP/Components/ft6x06/ft6x06_reg.c 

OBJS += \
./Components/ft6x06/ft6x06.o \
./Components/ft6x06/ft6x06_reg.o 

C_DEPS += \
./Components/ft6x06/ft6x06.d \
./Components/ft6x06/ft6x06_reg.d 


# Each subdirectory must supply rules for building sources it contributes
Components/ft6x06/ft6x06.o: /Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/BSP/Components/ft6x06/ft6x06.c Components/ft6x06/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I../Core/Inc -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Device/ST/STM32H7xx/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/Inc" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Utilities/lcd" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/BSP/Components/Common" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/BSP/STM32H747I-DISCO" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Utilities/Fonts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Components/ft6x06/ft6x06_reg.o: /Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/BSP/Components/ft6x06/ft6x06_reg.c Components/ft6x06/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I../Core/Inc -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Device/ST/STM32H7xx/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/Inc" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Utilities/lcd" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/BSP/Components/Common" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/BSP/STM32H747I-DISCO" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Utilities/Fonts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Components-2f-ft6x06

clean-Components-2f-ft6x06:
	-$(RM) ./Components/ft6x06/ft6x06.cyclo ./Components/ft6x06/ft6x06.d ./Components/ft6x06/ft6x06.o ./Components/ft6x06/ft6x06.su ./Components/ft6x06/ft6x06_reg.cyclo ./Components/ft6x06/ft6x06_reg.d ./Components/ft6x06/ft6x06_reg.o ./Components/ft6x06/ft6x06_reg.su

.PHONY: clean-Components-2f-ft6x06

