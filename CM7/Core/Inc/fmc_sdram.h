#ifndef FMC_SDRAM_H
#define FMC_SDRAM_H

#include "stm32h7xx_hal.h"
#include <stdint.h>

// SDRAM base address (should match your BSP configuration)
#define SDRAM_DEVICE_ADDR 0xD0000000
#define SDRAM_DEVICE_SIZE 0x2000000 // 32MB

// Function prototypes
int MX_SDRAM_Init(void);
int SDRAM_Test(void);
int SDRAM_QuickTest(void);
int SDRAM_InitMelFilters(float *mel_filters, uint32_t size);

#endif /* FMC_SDRAM_H */