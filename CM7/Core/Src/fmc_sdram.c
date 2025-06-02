/*
 * SDRAM Initialization for STM32H747I-DISCO (Based on BSP Example)
 * External SDRAM: IS42S32800J-6BLI (32MB)
 * Base Address: 0xD0000000
 */

#include "stm32h7xx_hal.h"
#include "main.h"
#include "fmc_sdram.h"

/*
 * SDRAM Helper Functions for STM32H747I-DISCO
 * Uses existing BSP SDRAM functions
 * External SDRAM: IS42S32800J-6BLI (32MB)
 * Base Address: 0xD0000000 (defined in BSP)
 */

#include "stm32h7xx_hal.h"
#include "stm32h747i_discovery_sdram.h" // BSP SDRAM header

/**
 * @brief  Initialize SDRAM using BSP functions
 * @retval 0 if success, -1 if error
 */
int SDRAM_BSP_Init(void)
{
    // Initialize SDRAM using BSP function
    if (BSP_SDRAM_Init(0) != BSP_ERROR_NONE)
    {
        return -1; // SDRAM initialization failed
    }

    return 0; // Success
}

/**
 * @brief  Test SDRAM functionality using BSP functions
 * @retval 0 if success, -1 if error
 */
int SDRAM_Test(void)
{
#define TEST_BUFFER_SIZE 1000
#define TEST_WRITE_ADDR 0x0800

    static uint32_t test_tx_buffer[TEST_BUFFER_SIZE];
    static uint32_t test_rx_buffer[TEST_BUFFER_SIZE];

    // Get SDRAM handle from BSP
    extern SDRAM_HandleTypeDef hsdram[];

    // Fill test buffer with known pattern
    for (uint32_t i = 0; i < TEST_BUFFER_SIZE; i++)
    {
        test_tx_buffer[i] = 0x12345678 + i;
    }

    // Write data to SDRAM
    if (HAL_SDRAM_Write_32b(&hsdram[0],
                            (uint32_t *)(SDRAM_DEVICE_ADDR + TEST_WRITE_ADDR),
                            test_tx_buffer,
                            TEST_BUFFER_SIZE) != HAL_OK)
    {
        return -1; // Write failed
    }

    // Read data back from SDRAM
    if (HAL_SDRAM_Read_32b(&hsdram[0],
                           (uint32_t *)(SDRAM_DEVICE_ADDR + TEST_WRITE_ADDR),
                           test_rx_buffer,
                           TEST_BUFFER_SIZE) != HAL_OK)
    {
        return -1; // Read failed
    }

    // Compare buffers
    for (uint32_t i = 0; i < TEST_BUFFER_SIZE; i++)
    {
        if (test_tx_buffer[i] != test_rx_buffer[i])
        {
            return -1; // Data mismatch
        }
    }

    return 0; // Test passed
}

/**
 * @brief  Simple SDRAM write test
 * @retval 0 if success, -1 if error
 */
int SDRAM_QuickTest(void)
{
    volatile uint32_t *sdram_addr = (uint32_t *)SDRAM_DEVICE_ADDR;
    uint32_t test_value = 0xDEADBEEF;

    // Write test value
    *sdram_addr = test_value;

    // Read back and verify
    if (*sdram_addr != test_value)
    {
        return -1; // Test failed
    }

    // Test a few more locations
    for (int i = 0; i < 100; i++)
    {
        sdram_addr[i] = 0xA5A5A5A5 + i;
    }

    for (int i = 0; i < 100; i++)
    {
        if (sdram_addr[i] != (0xA5A5A5A5 + i))
        {
            return -1; // Test failed
        }
    }

    return 0; // Test passed
}

/**
 * @brief  Initialize mel filter data in SDRAM
 * @param  mel_filters: pointer to mel filter array
 * @param  size: size of the filter array in bytes
 * @retval 0 if success, -1 if error
 */
int SDRAM_InitMelFilters(float *mel_filters, uint32_t size)
{
    // Clear the mel filter memory area
    if (mel_filters != NULL && size > 0)
    {
        // Zero out the filter bank memory
        memset(mel_filters, 0, size);
        return 0;
    }
    return -1;
}

/**
 * @brief  Get pointer to SDRAM for mel filters
 * @param  offset: offset from SDRAM base address
 * @retval Pointer to SDRAM location
 */
float *SDRAM_GetMelFilterPointer(uint32_t offset)
{
    return (float *)(SDRAM_DEVICE_ADDR + offset);
}