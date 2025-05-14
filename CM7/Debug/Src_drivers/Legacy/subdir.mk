################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/ben/Graduate_Courses/CSE-237D-EmbSysD/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/Legacy/stm32h7xx_hal_eth.c \
/Users/ben/Graduate_Courses/CSE-237D-EmbSysD/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/Legacy/stm32h7xx_hal_eth_ex.c 

C_DEPS += \
./Src_drivers/Legacy/stm32h7xx_hal_eth.d \
./Src_drivers/Legacy/stm32h7xx_hal_eth_ex.d 

OBJS += \
./Src_drivers/Legacy/stm32h7xx_hal_eth.o \
./Src_drivers/Legacy/stm32h7xx_hal_eth_ex.o 


# Each subdirectory must supply rules for building sources it contributes
Src_drivers/Legacy/stm32h7xx_hal_eth.o: /Users/ben/Graduate_Courses/CSE-237D-EmbSysD/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/Legacy/stm32h7xx_hal_eth.c Src_drivers/Legacy/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I../Core/Inc -I"/Users/ben/Graduate_Courses/CSE-237D-EmbSysD/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Device/ST/STM32H7xx/Include" -I"/Users/ben/Graduate_Courses/CSE-237D-EmbSysD/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Include" -I"/Users/ben/Graduate_Courses/CSE-237D-EmbSysD/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy" -I"/Users/ben/Graduate_Courses/CSE-237D-EmbSysD/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/" -I"/Users/ben/Graduate_Courses/CSE-237D-EmbSysD/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src_drivers/Legacy/stm32h7xx_hal_eth_ex.o: /Users/ben/Graduate_Courses/CSE-237D-EmbSysD/acoustic-species-hardware/Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/Legacy/stm32h7xx_hal_eth_ex.c Src_drivers/Legacy/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I../Core/Inc -I"/Users/ben/Graduate_Courses/CSE-237D-EmbSysD/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Device/ST/STM32H7xx/Include" -I"/Users/ben/Graduate_Courses/CSE-237D-EmbSysD/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Include" -I"/Users/ben/Graduate_Courses/CSE-237D-EmbSysD/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy" -I"/Users/ben/Graduate_Courses/CSE-237D-EmbSysD/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/" -I"/Users/ben/Graduate_Courses/CSE-237D-EmbSysD/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src_drivers-2f-Legacy

clean-Src_drivers-2f-Legacy:
	-$(RM) ./Src_drivers/Legacy/stm32h7xx_hal_eth.cyclo ./Src_drivers/Legacy/stm32h7xx_hal_eth.d ./Src_drivers/Legacy/stm32h7xx_hal_eth.o ./Src_drivers/Legacy/stm32h7xx_hal_eth.su ./Src_drivers/Legacy/stm32h7xx_hal_eth_ex.cyclo ./Src_drivers/Legacy/stm32h7xx_hal_eth_ex.d ./Src_drivers/Legacy/stm32h7xx_hal_eth_ex.o ./Src_drivers/Legacy/stm32h7xx_hal_eth_ex.su

.PHONY: clean-Src_drivers-2f-Legacy

