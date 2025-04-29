# Project settings
PROJECT = Project1

# MCU flags (adjust depending on STM32 chip)
CPU = -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb

# Paths
CORE_DIR = Core
CUBE_DIR = ../Libraries/STM32CubeH7

SRC = $(wildcard $(CORE_DIR)/Src/*.c) \
      $(wildcard $(CUBE_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/*.c)

STARTUP = $(CORE_DIR)/Startup/startup_stm32h743xx.s

INCLUDES = \
    -I$(CORE_DIR)/Inc \
    -I$(CUBE_DIR)/Drivers/STM32H7xx_HAL_Driver/Inc \
    -I$(CUBE_DIR)/Drivers/CMSIS/Include \
    -I$(CUBE_DIR)/Drivers/CMSIS/Device/ST/STM32H7xx/Include

# Compiler and flags
CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc -x assembler-with-cpp
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = $(CPU) -Wall -O2 -g -std=gnu11 $(INCLUDES)
LDFLAGS = $(CPU) -TSTM32H743ZI_FLASH.ld -Wl,-Map=$(PROJECT).map

# Outputs
OUT_ELF = $(PROJECT).elf
OUT_BIN = $(PROJECT).bin

all: $(OUT_BIN)

$(OUT_ELF): $(SRC) $(STARTUP)
	$(CC) $(CFLAGS) $(SRC) $(STARTUP) -o $@ $(LDFLAGS)

$(OUT_BIN): $(OUT_ELF)
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(OUT_ELF) $(OUT_BIN) $(PROJECT).map

flash: $(OUT_BIN)
	st-flash write $(OUT_BIN) 0x8000000

.PHONY: all clean flash
