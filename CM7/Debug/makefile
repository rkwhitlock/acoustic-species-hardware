################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include lcd/subdir.mk
-include X-CUBE-AI/App/subdir.mk
-include Src/Legacy/subdir.mk
-include Src/subdir.mk
-include STM32H747I-DISCO/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include Components/wm8994/subdir.mk
-include Components/ov9655/subdir.mk
-include Components/ov5640/subdir.mk
-include Components/otm8009a/subdir.mk
-include Components/nt35510/subdir.mk
-include Components/mt25tl01g/subdir.mk
-include Components/is42s32800j/subdir.mk
-include Components/ft6x06/subdir.mk
-include Components/adv7533/subdir.mk
-include Common/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
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
	arm-none-eabi-gcc -o "decible_meter_CM7.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/STM32H747XIHX_FLASH.ld" --specs=nosys.specs -Wl,-Map="decible_meter_CM7.map" -Wl,--gc-sections -static -L"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/../Libraries/STM32CubeH7/Middlewares/ST/AI/Lib" -L"/Users/reesewhitlock/AcousticSpecies/acoustic-species-hardware/CM7/Lib" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
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
