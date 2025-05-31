#ifndef FMC_SDRAM_H
#define FMC_SDRAM_H

#include "stm32h7xx_hal.h"

// SDRAM base address
#define SDRAM_DEVICE_ADDR 0xD0000000
#define SDRAM_DEVICE_SIZE 0x2000000 // 32MB

// Function prototypes
HAL_StatusTypeDef MX_FMC_Init(void);
static void MX_FMC_GPIO_Init(void);
int SDRAM_Test(void);

// External handle
extern SDRAM_HandleTypeDef hsdram1;

#endif /* FMC_SDRAM_H */