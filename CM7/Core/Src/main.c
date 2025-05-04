/**
 * @file       main.c
 * @brief      Audio sampling and SPL calculation for STM32H747XI DISCO
 * @details    This program configures an STM32H747XI to capture audio via SAI peripheral,
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
#define DEBUG_UART huart1         // Use UART1 for debug
#define LED_PORT GPIOI            // Discovery board has LEDs on GPIOI
#define LED_GREEN_PIN GPIO_PIN_12 // Green LED pin
#define LED_RED_PIN GPIO_PIN_13   // Red LED pin

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif

/* Hardware handles */
SAI_HandleTypeDef hsai_BlockA1; // Using SAI1 instead of SAI4 for STM32H747XI DISCO
DMA_HandleTypeDef hdma_sai1_a;  // DMA for SAI1 BlockA
UART_HandleTypeDef huart1;      // For printf statements for debugging

/* Function prototypes */
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SAI1_Init(void);

float calculate_decibel(int16_t *buffer, size_t size);
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai);
void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai);

/* Global variables */
int16_t audio_buffer[BUFFER_SIZE];
volatile uint8_t audio_ready_flag = 0; // Flag to indicate when DMA transfer is complete
uint32_t error_counter = 0;            // Track SAI errors

/**
 * @brief  Calculates the sound pressure level (SPL) in decibels from audio samples
 * @param  buffer: Pointer to audio sample buffer
 * @param  size: Number of samples in the buffer
 * @return Sound pressure level in decibels (dB)
 */
float calculate_decibel(int16_t *buffer, size_t size)
{
    float sum = 0.0f;
    int valid_samples = 0;

    // Verify we have non-zero samples
    for (size_t i = 0; i < size; i++)
    {
        if (buffer[i] != 0 && buffer[i] != 0xAAAA) // Check for initialized or silent values
        {
            float voltage = (float)buffer[i] / 32768.0f; // Normalize 16-bit value
            sum += voltage * voltage;
            valid_samples++;
        }
    }

    if (valid_samples == 0)
        return -100.0f; // Return very low value if no valid samples

    float rms = sqrtf(sum / valid_samples);
    float spl = 20.0f * log10f(rms / REFERENCE_VOLTAGE);

    return spl;
}

/**
 * @brief  DMA completion callback for SAI
 */
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
    audio_ready_flag = 1;
    HAL_GPIO_TogglePin(LED_PORT, LED_GREEN_PIN); // Toggle green LED to visualize DMA activity
}

/**
 * @brief  SAI error callback
 */
void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
    error_counter++;
    HAL_GPIO_TogglePin(LED_PORT, LED_RED_PIN); // Toggle red LED on error

    // Restart SAI in case of error
    HAL_SAI_DMAStop(hsai);
    HAL_SAI_Receive_DMA(hsai, (uint8_t *)audio_buffer, BUFFER_SIZE);
}

/**
 * @brief  Redirects printf output to UART1
 */
int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    // Initialize audio buffer to a known pattern
    memset(audio_buffer, 0xAA, sizeof(audio_buffer));

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
    MX_DMA_Init();
    MX_USART1_UART_Init();
    MX_SAI1_Init();

    printf("\r\n\r\n----- STM32H747XI DISCO Audio Capture Started -----\r\n");
    printf("Buffer size: %d samples\r\n", BUFFER_SIZE);

    /* Start audio capture using DMA */
    printf("Starting SAI DMA...\r\n");
    if (HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t *)audio_buffer, BUFFER_SIZE) != HAL_OK)
    {
        printf("SAI DMA initialization failed! Error: %ld\r\n", hsai_BlockA1.ErrorCode);
        // Visual indication of error - solid red LED
        HAL_GPIO_WritePin(LED_PORT, LED_RED_PIN, GPIO_PIN_SET);
    }
    else
    {
        printf("SAI DMA started successfully.\r\n");
        // Visual indication of success - green LED blink
        HAL_GPIO_WritePin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
    }

    uint32_t sample_counter = 0;

    /* Main application loop */
    while (1)
    {
        // If new audio data is ready
        if (audio_ready_flag)
        {
            audio_ready_flag = 0;
            sample_counter++;

            // Print some samples for debugging
            printf("Audio Buffer Data (Sample #%lu):\r\n", sample_counter);
            for (int i = 0; i < 16; i += 2)
            {
                printf("[%d]: 0x%04X  [%d]: 0x%04X\r\n",
                       i, (uint16_t)audio_buffer[i],
                       i + 1, (uint16_t)audio_buffer[i + 1]);
            }

            // Count non-zero values to detect if we're getting actual data
            int nonzero_count = 0;
            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                if (audio_buffer[i] != 0 && audio_buffer[i] != 0xAAAA)
                {
                    nonzero_count++;
                }
            }
            printf("Non-zero samples: %d/%d\r\n", nonzero_count, BUFFER_SIZE);

            /* Calculate and display sound pressure level */
            float decibel_level = calculate_decibel(audio_buffer, BUFFER_SIZE);
            printf("SPL: %.2f dB, SAI Errors: %lu\r\n\r\n", decibel_level, error_counter);
        }

        /* Wait 500ms before checking for new audio data */
        HAL_Delay(500);
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
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON; // Use HSE (external oscillator) on DISCO board
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 5;
    RCC_OscInitStruct.PLL.PLLN = 160;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                  RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1 | RCC_PERIPHCLK_USART1;

    // Configure PLL3 for audio - better configuration for standard audio rates
    PeriphClkInitStruct.PLL3.PLL3M = 25;  // Input divider
    PeriphClkInitStruct.PLL3.PLL3N = 336; // Multiplier
    PeriphClkInitStruct.PLL3.PLL3P = 2;   // Output divider
    PeriphClkInitStruct.PLL3.PLL3Q = 7;
    PeriphClkInitStruct.PLL3.PLL3R = 2;
    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
    PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;

    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL3;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief SAI1 Initialization Function - Configured for DISCO Board PDM microphones
 * @param None
 * @retval None
 */
static void MX_SAI1_Init(void)
{
    /* USER CODE BEGIN SAI1_Init 0 */

    /* USER CODE END SAI1_Init 0 */

    /* USER CODE BEGIN SAI1_Init 1 */

    /* USER CODE END SAI1_Init 1 */
    hsai_BlockA1.Instance = SAI1_Block_A;
    hsai_BlockA1.Init.AudioMode = SAI_MODEMASTER_RX;
    hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
    hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
    hsai_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
    hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
    hsai_BlockA1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_48K;
    hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
    hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
    hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
    hsai_BlockA1.Init.TriState = SAI_OUTPUT_NOTRELEASED;

    // PDM Microphone configuration for STM32H747XI DISCO
    hsai_BlockA1.Init.Protocol = SAI_FREE_PROTOCOL;
    hsai_BlockA1.Init.DataSize = SAI_DATASIZE_16;
    hsai_BlockA1.Init.FirstBit = SAI_FIRSTBIT_MSB;
    hsai_BlockA1.Init.ClockStrobing = SAI_CLOCKSTROBING_RISINGEDGE;

    // PDM mode configuration
    hsai_BlockA1.Init.PdmInit.Activation = ENABLE;
    hsai_BlockA1.Init.PdmInit.MicPairsNbr = 1;
    hsai_BlockA1.Init.PdmInit.ClockEnable = SAI_PDM_CLOCK1_ENABLE;

    // Frame configuration
    hsai_BlockA1.FrameInit.FrameLength = 16;
    hsai_BlockA1.FrameInit.ActiveFrameLength = 1;
    hsai_BlockA1.FrameInit.FSDefinition = SAI_FS_STARTFRAME;
    hsai_BlockA1.FrameInit.FSPolarity = SAI_FS_ACTIVE_HIGH;
    hsai_BlockA1.FrameInit.FSOffset = SAI_FS_FIRSTBIT;

    // Slot configuration
    hsai_BlockA1.SlotInit.FirstBitOffset = 0;
    hsai_BlockA1.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
    hsai_BlockA1.SlotInit.SlotNumber = 1;
    hsai_BlockA1.SlotInit.SlotActive = 0x00000001;

    if (HAL_SAI_Init(&hsai_BlockA1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN SAI1_Init 2 */

    /* USER CODE END SAI1_Init 2 */
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{
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
}

/**
 * Enable DMA controller clock and configure DMA for SAI1
 */
static void MX_DMA_Init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* Configure DMA request hdma_sai1_a */
    hdma_sai1_a.Instance = DMA1_Stream0;
    hdma_sai1_a.Init.Request = DMA_REQUEST_SAI1_A;
    hdma_sai1_a.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sai1_a.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sai1_a.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sai1_a.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_sai1_a.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_sai1_a.Init.Mode = DMA_CIRCULAR;
    hdma_sai1_a.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_sai1_a.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    if (HAL_DMA_Init(&hdma_sai1_a) != HAL_OK)
    {
        Error_Handler();
    }

    /* Associate the DMA handle */
    __HAL_LINKDMA(&hsai_BlockA1, hdmarx, hdma_sai1_a);

    /* DMA interrupt init */
    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Configure GPIO pin Output Level for LEDs */
    HAL_GPIO_WritePin(LED_PORT, LED_GREEN_PIN | LED_RED_PIN, GPIO_PIN_RESET);

    /* Configure LED pins */
    GPIO_InitStruct.Pin = LED_GREEN_PIN | LED_RED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

    /* Configure SAI1 pins if needed */
    /* The HAL_SAI_MspInit will be called automatically by HAL and should configure the pins */
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

    // Rapid error LED flashing
    while (1)
    {
        HAL_GPIO_TogglePin(LED_PORT, LED_RED_PIN);
        HAL_Delay(200);
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

/**
 * @brief  Initializes the SAI MSP.
 * @param  hsai: pointer to a SAI_HandleTypeDef structure
 * @retval None
 */
void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    if (hsai->Instance == SAI1_Block_A)
    {
        /* Enable SAI1 clock */
        __HAL_RCC_SAI1_CLK_ENABLE();

        /* SAI1_A pins configuration for STM32H747I-DISCO board */
        /* The digital microphone is connected to SAI1_A on PC1 (SAI1_SD_A) and PC4 (SAI1_MCLK_A) */

        __HAL_RCC_GPIOC_CLK_ENABLE();

        /* SAI_MCLK_A pin */
        GPIO_InitStruct.Pin = GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF6_SAI1;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        /* SAI_SD_A pin */
        GPIO_InitStruct.Pin = GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF6_SAI1;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        /* Link the DMA to handle */
        if (hdma_sai1_a.Instance == NULL)
        {
            /* DMA controller clock enable */
            __HAL_RCC_DMA1_CLK_ENABLE();

            /* Configure DMA request hdma_sai1_a */
            hdma_sai1_a.Instance = DMA1_Stream0;
            hdma_sai1_a.Init.Request = DMA_REQUEST_SAI1_A;
            hdma_sai1_a.Init.Direction = DMA_PERIPH_TO_MEMORY;
            hdma_sai1_a.Init.PeriphInc = DMA_PINC_DISABLE;
            hdma_sai1_a.Init.MemInc = DMA_MINC_ENABLE;
            hdma_sai1_a.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            hdma_sai1_a.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
            hdma_sai1_a.Init.Mode = DMA_CIRCULAR;
            hdma_sai1_a.Init.Priority = DMA_PRIORITY_HIGH;
            hdma_sai1_a.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

            if (HAL_DMA_Init(&hdma_sai1_a) != HAL_OK)
            {
                Error_Handler();
            }

            /* Associate the DMA handle */
            __HAL_LINKDMA(hsai, hdmarx, hdma_sai1_a);

            /* DMA interrupt init */
            HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
        }

        /* SAI1 interrupt Init */
        HAL_NVIC_SetPriority(SAI1_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(SAI1_IRQn);
    }
}

/**
 * @brief  Deinitializes the SAI MSP.
 * @param  hsai: pointer to a SAI_HandleTypeDef structure
 * @retval None
 */
void HAL_SAI_MspDeInit(SAI_HandleTypeDef *hsai)
{
    if (hsai->Instance == SAI1_Block_A)
    {
        /* Disable SAI1 clock */
        __HAL_RCC_SAI1_CLK_DISABLE();

        /* SAI1_A pins deinit */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1 | GPIO_PIN_4);

        /* Disable interrupt */
        HAL_NVIC_DisableIRQ(SAI1_IRQn);

        /* Disable DMA */
        HAL_DMA_DeInit(&hdma_sai1_a);
    }
}

/**
 * @brief  DMA1_Stream0 IRQ handler
 * @param  None
 * @retval None
 */
void DMA1_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_sai1_a);
}

/**
 * @brief  SAI1 IRQ handler
 * @param  None
 * @retval None
 */
void SAI1_IRQHandler(void)
{
    HAL_SAI_IRQHandler(&hsai_BlockA1);
}