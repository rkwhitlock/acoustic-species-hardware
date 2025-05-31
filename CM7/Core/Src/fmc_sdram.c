/*
 * FMC SDRAM Initialization for STM32H747I-DISCO
 * External SDRAM: IS42S32800J-6BLI (32MB)
 * Base Address: 0xD0000000
 */

#include "stm32h7xx_hal.h"

// SDRAM handle
SDRAM_HandleTypeDef hsdram1;

// FMC handle
static FMC_SDRAM_TimingTypeDef SdramTiming = {0};

/**
 * @brief  Initialize FMC SDRAM
 * @retval HAL Status
 */
HAL_StatusTypeDef MX_FMC_Init(void)
{
    FMC_SDRAM_CommandTypeDef Command = {0};
    HAL_StatusTypeDef status;

    // Enable FMC clock
    __HAL_RCC_FMC_CLK_ENABLE();

    // Configure GPIO pins for FMC
    MX_FMC_GPIO_Init();

    // SDRAM device configuration
    hsdram1.Instance = FMC_SDRAM_DEVICE;
    hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
    hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
    hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
    hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_32;
    hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
    hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_2;
    hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
    hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
    hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
    hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;

    // SDRAM timing configuration for 100MHz SDRAM clock
    SdramTiming.LoadToActiveDelay = 2;
    SdramTiming.ExitSelfRefreshDelay = 7;
    SdramTiming.SelfRefreshTime = 4;
    SdramTiming.RowCycleDelay = 7;
    SdramTiming.WriteRecoveryTime = 2;
    SdramTiming.RPDelay = 2;
    SdramTiming.RCDDelay = 2;

    // Initialize SDRAM controller
    status = HAL_SDRAM_Init(&hsdram1, &SdramTiming);
    if (status != HAL_OK)
    {
        return status;
    }

    // SDRAM initialization sequence
    // Step 1: Configure a clock configuration enable command
    Command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;

    status = HAL_SDRAM_SendCommand(&hsdram1, &Command, 1000);
    if (status != HAL_OK)
    {
        return status;
    }

    // Step 2: Insert 100 us minimum delay
    HAL_Delay(1);

    // Step 3: Configure a PALL (precharge all) command
    Command.CommandMode = FMC_SDRAM_CMD_PALL;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;

    status = HAL_SDRAM_SendCommand(&hsdram1, &Command, 1000);
    if (status != HAL_OK)
    {
        return status;
    }

    // Step 4: Configure an Auto Refresh command
    Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command.AutoRefreshNumber = 8;
    Command.ModeRegisterDefinition = 0;

    status = HAL_SDRAM_SendCommand(&hsdram1, &Command, 1000);
    if (status != HAL_OK)
    {
        return status;
    }

    // Step 5: Program the external memory mode register
    Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command.AutoRefreshNumber = 1;
    // CAS Latency = 2, Burst Length = 1
    Command.ModeRegisterDefinition = 0x0020;

    status = HAL_SDRAM_SendCommand(&hsdram1, &Command, 1000);
    if (status != HAL_OK)
    {
        return status;
    }

    // Step 6: Set the refresh rate counter
    // Refresh period = 64ms / 8192 rows = 7.8125 us
    // Counter = (7.8125 us × 100MHz) - 20 = 761
    HAL_SDRAM_ProgramRefreshRate(&hsdram1, 761);

    return HAL_OK;
}

/**
 * @brief  Configure FMC GPIO pins
 * @retval None
 */
static void MX_FMC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIO clocks
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();

    // Common GPIO configuration
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    // Configure address pins A0-A12
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                          GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_12 | GPIO_PIN_13 |
                          GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    // Configure data pins D0-D31
    GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_0 | GPIO_PIN_1 |
                          GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                          GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |
                          GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                          GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10 |
                          GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |
                          GPIO_PIN_15;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                          GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    // Configure control pins
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_15; // SDNWE, SDNRAS, SDNCAS
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6; // SDNE1
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5; // SDCKE1, SDCLK
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    // Bank address pins
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5; // BA0, BA1
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    // Data mask pins
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1; // NBL0, NBL1
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5; // NBL2, NBL3
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
}

/**
 * @brief  Test SDRAM functionality
 * @retval 0 if success, -1 if error
 */
int SDRAM_Test(void)
{
    volatile uint32_t *sdram_addr = (uint32_t *)0xD0000000;
    uint32_t test_value = 0xDEADBEEF;

    // Write test value
    *sdram_addr = test_value;

    // Read back and verify
    if (*sdram_addr != test_value)
    {
        return -1; // Test failed
    }

    // Test different locations
    for (int i = 0; i < 1000; i++)
    {
        sdram_addr[i] = 0x12345678 + i;
    }

    for (int i = 0; i < 1000; i++)
    {
        if (sdram_addr[i] != (0x12345678 + i))
        {
            return -1; // Test failed
        }
    }

    return 0; // Test passed
}