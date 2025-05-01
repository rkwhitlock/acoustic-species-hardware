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
#include <stdint.h>
#include <stdio.h>
#include <string.h> // Added for memset

/* Configuration defines */
#define BUFFER_SIZE 1024 // Size of the audio buffer
#define REFERENCE_VOLTAGE 0.00002f

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif

/* Hardware handles */
SAI_HandleTypeDef hsai_BlockA4;
DMA_HandleTypeDef hdma_sai4_a;
UART_HandleTypeDef huart1; // For printf statements for debugging

/* Function prototypes */
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_BDMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SAI4_Init(void);

float calculate_decibel(int16_t *buffer, size_t size);

/* Global variables */
int16_t audio_buffer[BUFFER_SIZE];

/**
 * @brief  Calculates the sound pressure level (SPL) in decibels from audio samples
 * @param  buffer: Pointer to audio sample buffer
 * @param  size: Number of samples in the buffer
 * @return Sound pressure level in decibels (dB)
 */
float calculate_decibel(int16_t *buffer, size_t size)
{
    float sum = 0.0f;

    for (size_t i = 0; i < size; i++)
    {
        float voltage = (float)buffer[i] / 32768.0f; // Normalize 16-bit value
        sum += voltage * voltage;
    }

    float rms = sqrtf(sum / size);
    float spl = 20.0f * log10f(rms / REFERENCE_VOLTAGE);

    return spl;
}

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

    memset(audio_buffer, 0xAA, sizeof(audio_buffer)); // Initialize buffer with known pattern

    /* Wait until CPU2 (CM4) boots and enters in stop mode or timeout*/
    int32_t timeout = 0xFFFF;
    while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0))
        ;
    if (timeout < 0)
    {
        Error_Handler();
    }

    /* Hardware initialization sequence */
    MPU_Config();
    HAL_Init();                 // Reset peripherals
    SystemClock_Config();       // System clock configuration
    PeriphCommonClock_Config(); // Peripheral clocks configuration

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

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_BDMA_Init();
    MX_USART1_UART_Init();
    MX_SAI4_Init();

    /* Start audio capture using DMA */
    printf("Starting SAI DMA...\r\n");
    if (HAL_SAI_Receive_DMA(&hsai_BlockA4, (uint8_t *)audio_buffer, BUFFER_SIZE) != HAL_OK)
    {
        printf("SAI DMA initialization failed! Error: %ld\r\n", hsai_BlockA4.ErrorCode);
    }
    else
    {
        printf("SAI DMA started successfully.\r\n");
    }

    /* Main application loop */
    while (1)
    {
        printf("Audio Buffer Data:\r\n");
        for (int i = 0; i < 10; i++)
        {
            printf("[%d]: %h\r\n", i, audio_buffer[i]);
        }

        /* Calculate and display sound pressure level */
        float decibel_level = calculate_decibel(audio_buffer, BUFFER_SIZE);
        printf("SPL: %.2f dB\r\n", decibel_level);

        /* Wait one second before next update */
        HAL_Delay(1000);
    }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Supply configuration update enable
     */
    HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

    /** Configure the main internal regulator output voltage
     */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }

    /** Macro to configure the PLL clock source
     */
    __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSI);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                  RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 |
                                  RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief Peripherals Common Clock Configuration
 * @retval None
 */
void PeriphCommonClock_Config(void)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Initializes the peripherals clock
     */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI4A | RCC_PERIPHCLK_USART1;
    PeriphClkInitStruct.PLL3.PLL3M = 4;
    PeriphClkInitStruct.PLL3.PLL3N = 10;
    PeriphClkInitStruct.PLL3.PLL3P = 10;
    PeriphClkInitStruct.PLL3.PLL3Q = 4;
    PeriphClkInitStruct.PLL3.PLL3R = 2;
    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_3;
    PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOMEDIUM;
    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_PLL3;
    PeriphClkInitStruct.Sai4AClockSelection = RCC_SAI4ACLKSOURCE_PLL3;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief SAI4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SAI4_Init(void)
{

    /* USER CODE BEGIN SAI4_Init 0 */

    /* USER CODE END SAI4_Init 0 */

    /* USER CODE BEGIN SAI4_Init 1 */

    /* USER CODE END SAI4_Init 1 */
    hsai_BlockA4.Instance = SAI4_Block_A;
    hsai_BlockA4.Init.Protocol = SAI_FREE_PROTOCOL;
    hsai_BlockA4.Init.AudioMode = SAI_MODEMASTER_RX;
    hsai_BlockA4.Init.DataSize = SAI_DATASIZE_16;
    hsai_BlockA4.Init.FirstBit = SAI_FIRSTBIT_MSB;
    hsai_BlockA4.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
    hsai_BlockA4.Init.Synchro = SAI_ASYNCHRONOUS;
    hsai_BlockA4.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
    hsai_BlockA4.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
    hsai_BlockA4.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
    hsai_BlockA4.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_192K;
    hsai_BlockA4.Init.MonoStereoMode = SAI_STEREOMODE;
    hsai_BlockA4.Init.CompandingMode = SAI_NOCOMPANDING;
    hsai_BlockA4.Init.PdmInit.Activation = ENABLE;
    hsai_BlockA4.Init.PdmInit.MicPairsNbr = 1;
    hsai_BlockA4.Init.PdmInit.ClockEnable = SAI_PDM_CLOCK1_ENABLE;
    hsai_BlockA4.FrameInit.FrameLength = 16;
    hsai_BlockA4.FrameInit.ActiveFrameLength = 1;
    hsai_BlockA4.FrameInit.FSDefinition = SAI_FS_STARTFRAME;
    hsai_BlockA4.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
    hsai_BlockA4.FrameInit.FSOffset = SAI_FS_FIRSTBIT;
    hsai_BlockA4.SlotInit.FirstBitOffset = 0;
    hsai_BlockA4.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
    hsai_BlockA4.SlotInit.SlotNumber = 1;
    hsai_BlockA4.SlotInit.SlotActive = 0x00000000;
    if (HAL_SAI_Init(&hsai_BlockA4) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN SAI4_Init 2 */

    /* USER CODE END SAI4_Init 2 */
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{

    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN USART1_Init 2 */

    /* USER CODE END USART1_Init 2 */
}

/**
 * Enable DMA controller clock
 */
static void MX_BDMA_Init(void)
{

    /* DMA controller clock enable */
    __HAL_RCC_BDMA_CLK_ENABLE();

    /* DMA interrupt init */
    /* BDMA_Channel0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(BDMA_Channel0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(BDMA_Channel0_IRQn);
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    /* USER CODE BEGIN MX_GPIO_Init_1 */
    /* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* USER CODE BEGIN MX_GPIO_Init_2 */
    /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct = {0};

    /* Disables the MPU */
    HAL_MPU_Disable();

    /** Initializes and configures the Region and the memory to be protected
     */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.BaseAddress = 0x0;
    MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
    MPU_InitStruct.SubRegionDisable = 0x87;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    /* Enables the MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
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
