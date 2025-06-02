#ifndef FMC_SDRAM_H
#define FMC_SDRAM_H

#include "stm32h7xx_hal.h"
#include "stm32h747i_discovery_sdram.h" // This already defines SDRAM_DEVICE_ADDR

// Function prototypes (renamed to avoid conflicts)
int SDRAM_BSP_Init(void);
int SDRAM_Test(void);
int SDRAM_QuickTest(void);
int SDRAM_InitMelFilters(float *mel_filters, uint32_t size);
float *SDRAM_GetMelFilterPointer(uint32_t offset);

#endif /* FMC_SDRAM_H */