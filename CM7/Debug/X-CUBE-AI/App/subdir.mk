################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../X-CUBE-AI/App/app_x-cube-ai.c \
../X-CUBE-AI/App/tinyaudiocnn.c \
../X-CUBE-AI/App/tinyaudiocnn_data.c \
../X-CUBE-AI/App/tinyaudiocnn_data_params.c 

OBJS += \
./X-CUBE-AI/App/app_x-cube-ai.o \
./X-CUBE-AI/App/tinyaudiocnn.o \
./X-CUBE-AI/App/tinyaudiocnn_data.o \
./X-CUBE-AI/App/tinyaudiocnn_data_params.o 

C_DEPS += \
./X-CUBE-AI/App/app_x-cube-ai.d \
./X-CUBE-AI/App/tinyaudiocnn.d \
./X-CUBE-AI/App/tinyaudiocnn_data.d \
./X-CUBE-AI/App/tinyaudiocnn_data_params.d 


# Each subdirectory must supply rules for building sources it contributes
X-CUBE-AI/App/%.o X-CUBE-AI/App/%.su X-CUBE-AI/App/%.cyclo: ../X-CUBE-AI/App/%.c X-CUBE-AI/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -DUSE_PWR_DIRECT_SMPS_SUPPLY -c -I../X-CUBE-AI/App -I../X-CUBE-AI -I../Core/Inc -I../../Middlewares/ST/AI/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-X-2d-CUBE-2d-AI-2f-App

clean-X-2d-CUBE-2d-AI-2f-App:
	-$(RM) ./X-CUBE-AI/App/app_x-cube-ai.cyclo ./X-CUBE-AI/App/app_x-cube-ai.d ./X-CUBE-AI/App/app_x-cube-ai.o ./X-CUBE-AI/App/app_x-cube-ai.su ./X-CUBE-AI/App/tinyaudiocnn.cyclo ./X-CUBE-AI/App/tinyaudiocnn.d ./X-CUBE-AI/App/tinyaudiocnn.o ./X-CUBE-AI/App/tinyaudiocnn.su ./X-CUBE-AI/App/tinyaudiocnn_data.cyclo ./X-CUBE-AI/App/tinyaudiocnn_data.d ./X-CUBE-AI/App/tinyaudiocnn_data.o ./X-CUBE-AI/App/tinyaudiocnn_data.su ./X-CUBE-AI/App/tinyaudiocnn_data_params.cyclo ./X-CUBE-AI/App/tinyaudiocnn_data_params.d ./X-CUBE-AI/App/tinyaudiocnn_data_params.o ./X-CUBE-AI/App/tinyaudiocnn_data_params.su

.PHONY: clean-X-2d-CUBE-2d-AI-2f-App

