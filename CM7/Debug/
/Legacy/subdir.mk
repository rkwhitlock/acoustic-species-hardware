################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/Legacy/stm32h7xx_hal_eth.c \
/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/Legacy/stm32h7xx_hal_eth_ex.c 

C_DEPS += \
./\ /Legacy/stm32h7xx_hal_eth.d \
./\ /Legacy/stm32h7xx_hal_eth_ex.d 

OBJS += \
./\ /Legacy/stm32h7xx_hal_eth.o \
./\ /Legacy/stm32h7xx_hal_eth_ex.o 


# Each subdirectory must supply rules for building sources it contributes
\ /Legacy/stm32h7xx_hal_eth.o: /Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/Legacy/stm32h7xx_hal_eth.c \ /Legacy/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I../Core/Inc -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Device/ST/STM32H7xx/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"
/Legacy/stm32h7xx_hal_eth.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
\ /Legacy/stm32h7xx_hal_eth_ex.o: /Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/Legacy/stm32h7xx_hal_eth_ex.c \ /Legacy/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I../Core/Inc -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Device/ST/STM32H7xx/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"
/Legacy/stm32h7xx_hal_eth_ex.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean--a--2f-Legacy

clean--a--2f-Legacy:
	-$(RM) ./\ /Legacy/stm32h7xx_hal_eth.cyclo ./\ /Legacy/stm32h7xx_hal_eth.d ./\ /Legacy/stm32h7xx_hal_eth.o ./\ /Legacy/stm32h7xx_hal_eth.su ./\ /Legacy/stm32h7xx_hal_eth_ex.cyclo ./\ /Legacy/stm32h7xx_hal_eth_ex.d ./\ /Legacy/stm32h7xx_hal_eth_ex.o ./\ /Legacy/stm32h7xx_hal_eth_ex.su

.PHONY: clean--a--2f-Legacy

