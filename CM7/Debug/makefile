################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Src/Legacy/subdir.mk
-include Src-common/subdir.mk
-include Src/subdir.mk
-include Source/TransformFunctions/subdir.mk
-include Source/SupportFunctions/subdir.mk
-include Source/StatisticsFunctions/subdir.mk
-include Source/MatrixFunctions/subdir.mk
-include Source/FilteringFunctions/subdir.mk
-include Source/FastMathFunctions/subdir.mk
-include Source/ControllerFunctions/subdir.mk
-include Source/ComplexMathFunctions/subdir.mk
-include Source/CommonTables/subdir.mk
-include Source/BasicMathFunctions/subdir.mk
-include STM32H747I-DISCO-src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include Components-bsp/wm8994/subdir.mk
-include Components-bsp/ov9655/subdir.mk
-include Components-bsp/ov5640/subdir.mk
-include Components-bsp/otm8009a/subdir.mk
-include Components-bsp/nt35510/subdir.mk
-include Components-bsp/mt25tl01g/subdir.mk
-include Components-bsp/is42s32800j/subdir.mk
-include Components-bsp/ft6x06/subdir.mk
-include Components-bsp/adv7533/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CCM_DEPS)),)
-include $(CCM_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CXXM_DEPS)),)
-include $(CXXM_DEPS)
endif
ifneq ($(strip $(C++M_DEPS)),)
-include $(C++M_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := decible_meter_CM7
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
decible_meter_CM7.elf \

MAP_FILES += \
decible_meter_CM7.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
decible_meter_CM7.list \


# All Target
all: main-build

# Main-build Target
main-build: decible_meter_CM7.elf secondary-outputs

# Tool invocations
decible_meter_CM7.elf decible_meter_CM7.map: $(OBJS) $(USER_OBJS) /Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/STM32H747XIHX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-g++ -o "decible_meter_CM7.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/STM32H747XIHX_FLASH.ld" --specs=nosys.specs -Wl,-Map="decible_meter_CM7.map" -Wl,--gc-sections -static -L"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/Lib/" -u _printf_float --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

decible_meter_CM7.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "decible_meter_CM7.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) decible_meter_CM7.elf decible_meter_CM7.list decible_meter_CM7.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
