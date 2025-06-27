################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/audio_play.c \
../Core/Src/audio_record.c \
../Core/Src/camera.c \
../Core/Src/joystick.c \
../Core/Src/lcd.c \
../Core/Src/main.c \
../Core/Src/qspi.c \
../Core/Src/sd.c \
../Core/Src/sdram.c \
../Core/Src/stm32h7xx_hal_msp.c \
../Core/Src/stm32h7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/touchscreen.c 

OBJS += \
./Core/Src/audio_play.o \
./Core/Src/audio_record.o \
./Core/Src/camera.o \
./Core/Src/joystick.o \
./Core/Src/lcd.o \
./Core/Src/main.o \
./Core/Src/qspi.o \
./Core/Src/sd.o \
./Core/Src/sdram.o \
./Core/Src/stm32h7xx_hal_msp.o \
./Core/Src/stm32h7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/touchscreen.o 

C_DEPS += \
./Core/Src/audio_play.d \
./Core/Src/audio_record.d \
./Core/Src/camera.d \
./Core/Src/joystick.d \
./Core/Src/lcd.d \
./Core/Src/main.d \
./Core/Src/qspi.d \
./Core/Src/sd.d \
./Core/Src/sdram.d \
./Core/Src/stm32h7xx_hal_msp.d \
./Core/Src/stm32h7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/touchscreen.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -DUSE_PWR_DIRECT_SMPS_SUPPLY -c -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Middlewares/ST/AI/Inc" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Device/ST/STM32H7xx/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/CMSIS/Include" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/Inc" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Utilities/lcd" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/BSP/Components/Common" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/BSP/STM32H747I-DISCO" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Utilities/Fonts" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/Core/Inc" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/X-CUBE-AI" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/X-CUBE-AI/App" -I"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Drivers/BSP/Components" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/audio_play.cyclo ./Core/Src/audio_play.d ./Core/Src/audio_play.o ./Core/Src/audio_play.su ./Core/Src/audio_record.cyclo ./Core/Src/audio_record.d ./Core/Src/audio_record.o ./Core/Src/audio_record.su ./Core/Src/camera.cyclo ./Core/Src/camera.d ./Core/Src/camera.o ./Core/Src/camera.su ./Core/Src/joystick.cyclo ./Core/Src/joystick.d ./Core/Src/joystick.o ./Core/Src/joystick.su ./Core/Src/lcd.cyclo ./Core/Src/lcd.d ./Core/Src/lcd.o ./Core/Src/lcd.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/qspi.cyclo ./Core/Src/qspi.d ./Core/Src/qspi.o ./Core/Src/qspi.su ./Core/Src/sd.cyclo ./Core/Src/sd.d ./Core/Src/sd.o ./Core/Src/sd.su ./Core/Src/sdram.cyclo ./Core/Src/sdram.d ./Core/Src/sdram.o ./Core/Src/sdram.su ./Core/Src/stm32h7xx_hal_msp.cyclo ./Core/Src/stm32h7xx_hal_msp.d ./Core/Src/stm32h7xx_hal_msp.o ./Core/Src/stm32h7xx_hal_msp.su ./Core/Src/stm32h7xx_it.cyclo ./Core/Src/stm32h7xx_it.d ./Core/Src/stm32h7xx_it.o ./Core/Src/stm32h7xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/touchscreen.cyclo ./Core/Src/touchscreen.d ./Core/Src/touchscreen.o ./Core/Src/touchscreen.su

.PHONY: clean-Core-2f-Src

