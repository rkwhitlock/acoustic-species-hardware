/**
 * @file       main.c
 * @brief      Audio sampling and SPL calculation for STM32H7
 * @details    This program configures an STM32H7 to capture audio via SAI peripheral,
 *            calculate sound pressure level (SPL), and output debug info via UART.
 */

/* Includes */
#include "main.h"
#include "math.h"
#include "stm32h7xx_hal.h"
// #include "fmc_sdram.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h> // Added for memset

/* Configuration defines */
#define REFERENCE_VOLTAGE 0.00002f

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif

/* Hardware handles */
SAI_HandleTypeDef hsai_BlockA4;
DMA_HandleTypeDef hdma_sai4_a;
UART_HandleTypeDef huart1; // For printf statements for debugging

/* Function prototypes */
static void SystemClock_Config(void);
static void MPU_Config(void);
static void CPU_CACHE_Enable(void);

// float calculate_decibel(int16_t *buffer, size_t size);

/**
 * @brief  Calculates the sound pressure level (SPL) in decibels from audio samples
 * @param  buffer: Pointer to audio sample buffer
 * @param  size: Number of samples in the buffer
 * @return Sound pressure level in decibels (dB)
 */
// float calculate_decibel(uint16_t *buffer, size_t size)
// {
//     float sum = 0.0f;

//     for (size_t i = 0; i < size; i++)
//     {
//         float voltage = (float)buffer[i] / 65536.0f; // Normalize 16-bit value
//         sum += voltage * voltage;
//     }

//     float rms = sqrtf(sum / size);
//     float spl = 20.0f * log10f(rms / REFERENCE_VOLTAGE);

//     return spl;
// }

/**
 * @brief  Redirects printf output to UART1
 */
// int _write(int file, char *ptr, int len)
// {
//     HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
//     return len;
// }
// int _write(int file, char *ptr, int len)
// {
//     HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
//     return len;
// }

/**
 * @brief  Redirects printf output to SWV ITM Data Console
 */
int _write(int file, char *ptr, int len)
{
    int i = 0;
    for (i = 0; i < len; i++)
    {
        ITM_SendChar(*ptr++);
    }
    return len;
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

    // memset(audio_buffer, 0xAA, sizeof(audio_buffer)); // Initialize buffer with known pattern

    /* Wait until CPU2 (CM4) boots and enters in stop mode or timeout*/
    int32_t timeout = 0xFFFF;
    while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0))
        ;
    if (timeout < 0)
    {
        Error_Handler();
    }

    /* Configure the MPU attributes as Write Through */
    MPU_Config();

    /* Enable the CPU Cache */
    CPU_CACHE_Enable();

    /* STM32H7xx HAL library initialization:
         - Systick timer is configured by default as source of time base, but user
           can eventually implement his proper time base source (a general purpose
           timer for example or other time source), keeping in mind that Time base
           duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
           handled in milliseconds basis.
         - Set NVIC Group Priority to 4
         - Low Level Initialization
       */
    HAL_Init();

    /* Configure the system clock to 400 MHz */
    SystemClock_Config();

    // if (MX_FMC_Init() != HAL_OK)
    // {
    //     Error_Handler();
    // }

    // // Optional: Test SDRAM
    // if (SDRAM_Test() != 0)
    // {
    //     Error_Handler();
    // }

    /* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
    HSEM notification */
    __HAL_RCC_HSEM_CLK_ENABLE();    // Enable semaphore clock
    HAL_HSEM_FastTake(HSEM_ID_0);   // Take HSEM
    HAL_HSEM_Release(HSEM_ID_0, 0); // Release HSEM to notify CM4

    /* Wait until CPU2 wakes up from stop mode */
    timeout = 0xFFFF;
    while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0))
        ;
    if (timeout < 0)
    {
        Error_Handler();
    }

    /* Main application loop */
    while (1)
    {
        AudioRecord();
        // printf("Audio Buffer Data:\r\n");
        // for (int i = 0; i < 10; i++)
        // {
        //     printf("[%d]: %h\r\n", i, audio_buffer[i]);
        // }

        // /* Calculate and display sound pressure level */
        // float decibel_level = calculate_decibel(audio_buffer, BUFFER_SIZE * 2);
        // printf("SPL: %.2f dB\r\n", decibel_level);

        /* Wait one second before next update */
        HAL_Delay(1000);
    }
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 400000000 (Cortex-M7 CPU Clock)
 *            HCLK(Hz)                       = 200000000 (Cortex-M4 CPU, Bus matrix Clocks)
 *            AHB Prescaler                  = 2
 *            D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
 *            D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
 *            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
 *            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
 *            HSE Frequency(Hz)              = 25000000
 *            PLL_M                          = 5
 *            PLL_N                          = 160
 *            PLL_P                          = 2
 *            PLL_Q                          = 4
 *            PLL_R                          = 2
 *            VDD(V)                         = 3.3
 *            Flash Latency(WS)              = 4
 * @param  None
 * @retval None
 */
static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    HAL_StatusTypeDef ret = HAL_OK;

    /* Enable Power Control clock */
    // __PWR_CLK_ENABLE();
    // __HAL_RCC_SYSCFG_CLK_ENABLE();
    // PWR->CR3 |= PWR_CR3_SMPSEN;

    /* The voltage scaling allows optimizing the power consumption when the device is
       clocked below the maximum system frequency, to update the voltage scaling value
       regarding system frequency refer to product datasheet.  */
    // __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    // {
    // }

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    RCC_OscInitStruct.PLL.PLLM = 5;
    RCC_OscInitStruct.PLL.PLLN = 160;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLQ = 4;

    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
    if (ret != HAL_OK)
    {
        Error_Handler();
    }

    /* Select PLL as system clock source and configure  bus clocks dividers */
    RCC_ClkInitStruct.ClockType =
        (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 |
         RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1);

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
    if (ret != HAL_OK)
    {
        Error_Handler();
    }

    /*
     Note : The activation of the I/O Compensation Cell is recommended with communication interfaces
             (GPIO, SPI, FMC, QSPI ...)  when  operating at  high frequencies(please refer to
     product datasheet) The I/O Compensation Cell activation  procedure requires :
           - The activation of the CSI clock
           - The activation of the SYSCFG clock
           - Enabling the I/O Compensation Cell : setting bit[0] of register SYSCFG_CCCSR
    */

    /*activate CSI clock mondatory for I/O Compensation Cell*/
    __HAL_RCC_CSI_ENABLE();

    /* Enable SYSCFG clock mondatory for I/O Compensation Cell */
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    /* Enables the I/O Compensation Cell */
    HAL_EnableCompensationCell();
}

/**
 * @brief  Configure the MPU attributes as Write Through for SDRAM.
 * @note   The Base Address is SDRAM_DEVICE_ADDR.
 *         The Region Size is 32MB.
 * @param  None
 * @retval None
 */
static void MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct;

    /* Disable the MPU */
    HAL_MPU_Disable();

    /* Configure the MPU as Strongly ordered for not defined regions */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x00;
    MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
    MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x87;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Configure the MPU attributes as WT for SDRAM */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = SDRAM_DEVICE_ADDR;
    MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER1;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Configure the MPU QSPI flash */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x90000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_128MB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER2;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x0;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Enable the MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
 * @brief  CPU L1-Cache enable.
 * @param  None
 * @retval None
 */
static void CPU_CACHE_Enable(void)
{
    /* Enable I-Cache */
    SCB_EnableICache();

    /* Enable D-Cache */
    SCB_EnableDCache();
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
