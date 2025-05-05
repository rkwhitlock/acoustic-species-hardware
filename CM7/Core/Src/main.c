/*
 * STM32H7 SAI Audio Reception with DMA
 * Improved implementation with proper audio handling
 */

#include "main.h"
#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <string.h>

// Increase buffer size for better audio processing
// Using double the size to easily manage half-transfer
#define BUFFER_LEN 256 // Size of the audio buffer (adjust based on your needs)

// Audio buffer for reception - using a double buffer approach
uint16_t audio_buffer[BUFFER_LEN];

// Buffer processing flags
volatile uint8_t buffer_half_ready = 0;
volatile uint8_t buffer_full_ready = 0;
volatile uint32_t half_transfer_count = 0;
volatile uint32_t full_transfer_count = 0;

// DMA handle for SAI1
DMA_HandleTypeDef hdma_sai1_a_rx;
SAI_HandleTypeDef hsai_BlockA4;

// SAI handle for SAI1 Block A (receiver)
SAI_HandleTypeDef hsai_BlockA1;

// Function prototypes
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SAI1_Init(void);
static void MX_DMA_Init(void);
void ProcessAudioData(uint16_t *buffer, uint16_t length);
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai);
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai);
void Error_Handler(void);

// Main function
int main(void)
{
    // HAL initialization
    HAL_Init();

    // Initialize system
    SystemClock_Config();
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_SAI1_Init();

    // Set up debug LED or pin
    // Example:
    // GPIO_InitTypeDef GPIO_InitStruct = {0};
    // __HAL_RCC_GPIOB_CLK_ENABLE();
    // GPIO_InitStruct.Pin = GPIO_PIN_0;
    // GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    // GPIO_InitStruct.Pull = GPIO_NOPULL;
    // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    // HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Start SAI reception using DMA in circular mode
    HAL_StatusTypeDef status =
        HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t *)audio_buffer, BUFFER_LEN);
    if (status != HAL_OK)
    {
        // Indicate specific error with LED pattern or debug output
        Error_Handler();
    }

    // Main processing loop
    while (1)
    {
        // Process buffers as before
        if (buffer_half_ready)
        {
            ProcessAudioData(&audio_buffer[0], BUFFER_LEN / 2);
            buffer_half_ready = 0;
        }

        if (buffer_full_ready)
        {
            ProcessAudioData(&audio_buffer[BUFFER_LEN / 2], BUFFER_LEN / 2);
            buffer_full_ready = 0;
        }

        // Check SAI status periodically
        static uint32_t last_check = 0;
        uint32_t current_time = HAL_GetTick();
        if (current_time - last_check > 1000) // Check every second
        {
            last_check = current_time;

            // Check transfer counts and SAI state
            if (half_transfer_count == 0 && full_transfer_count == 0)
            {
                // No transfers happening - try restarting
                HAL_SAI_DMAStop(&hsai_BlockA1);
                HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t *)audio_buffer, BUFFER_LEN);
            }

            // Reset counters to check if new transfers happen
            half_transfer_count = 0;
            full_transfer_count = 0;
        }
    }
}

// System clock configuration - optimized for audio applications
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Supply configuration update enable
     */
    HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

    /** Configure the main internal regulator output voltage
     */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }

    /** Configure LSE Drive Capability
     */
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    /** Macro to configure the PLL clock source
     */
    __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 5;
    RCC_OscInitStruct.PLL.PLLN = 160;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
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
                                  RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 |
                                  RCC_CLOCKTYPE_D1PCLK1;
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

    /** Configure the SAI PLL for audio clock
     */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
    PeriphClkInitStruct.PLL2.PLL2M = 25;
    PeriphClkInitStruct.PLL2.PLL2N = 344;
    PeriphClkInitStruct.PLL2.PLL2P = 7; // PLL2P = 344/7 = 49.142MHz for 16kHz * 256 = ~12.58MHz
    PeriphClkInitStruct.PLL2.PLL2Q = 2;
    PeriphClkInitStruct.PLL2.PLL2R = 2;
    PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
    PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
    PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
    PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}

// GPIO initialization
static void MX_GPIO_Init(void)
{
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    // Add any specific GPIO pin configurations here if needed
    // for example, LEDs or debug pins
}

// SAI1 initialization with proper audio configuration
static void MX_SAI1_Init(void)
{
    /* SAI peripheral configuration - simplified */
    hsai_BlockA1.Instance = SAI1_Block_A;
    hsai_BlockA1.Init.Protocol = SAI_FREE_PROTOCOL;
    hsai_BlockA1.Init.AudioMode = SAI_MODEMASTER_RX;
    hsai_BlockA1.Init.DataSize = SAI_DATASIZE_16;
    hsai_BlockA1.Init.FirstBit = SAI_FIRSTBIT_LSB;
    hsai_BlockA1.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
    hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
    hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
    hsai_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_DISABLE;
    hsai_BlockA1.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE;
    hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
    hsai_BlockA1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_48K;
    hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
    hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
    hsai_BlockA1.Init.PdmInit.Activation = ENABLE;
    hsai_BlockA1.Init.PdmInit.MicPairsNbr = 1;
    hsai_BlockA1.Init.PdmInit.ClockEnable = SAI_PDM_CLOCK1_ENABLE;
    hsai_BlockA1.FrameInit.FrameLength = 16;
    hsai_BlockA1.FrameInit.ActiveFrameLength = 1;
    hsai_BlockA1.FrameInit.FSDefinition = SAI_FS_STARTFRAME;
    hsai_BlockA1.FrameInit.FSPolarity = SAI_FS_ACTIVE_HIGH;
    hsai_BlockA1.FrameInit.FSOffset = SAI_FS_FIRSTBIT;
    hsai_BlockA1.SlotInit.FirstBitOffset = 0;
    hsai_BlockA1.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
    hsai_BlockA1.SlotInit.SlotNumber = 1;
    hsai_BlockA1.SlotInit.SlotActive = SAI_SLOTACTIVE_ALL;

    if (HAL_SAI_Init(&hsai_BlockA1) != HAL_OK)
    {
        Error_Handler();
    }
}
// DMA initialization for SAI
static void MX_DMA_Init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* Configure DMA request for SAI1 RX */
    hdma_sai1_a_rx.Instance = DMA1_Stream1;
    hdma_sai1_a_rx.Init.Request = DMA_REQUEST_SAI1_A;
    hdma_sai1_a_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sai1_a_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sai1_a_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sai1_a_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_sai1_a_rx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_sai1_a_rx.Init.Mode = DMA_CIRCULAR;
    hdma_sai1_a_rx.Init.Priority = DMA_PRIORITY_HIGH;

    // Consider enabling FIFO mode for more stable transfers with bursts
    hdma_sai1_a_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sai1_a_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sai1_a_rx.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_sai1_a_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;

    if (HAL_DMA_Init(&hdma_sai1_a_rx) != HAL_OK)
    {
        Error_Handler();
    }

    /* Associate the DMA handle with the SAI handle */
    __HAL_LINKDMA(&hsai_BlockA1, hdmarx, hdma_sai1_a_rx);

    /* Configure NVIC for DMA interrupt */
    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
}

/**
 * @brief  Half completed reception callback
 * @param  hsai: pointer to a SAI_HandleTypeDef structure
 * @retval None
 */
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    buffer_half_ready = 1;
    half_transfer_count++; // Debug counter
}

/**
 * @brief  Completed reception callback
 * @param  hsai: pointer to a SAI_HandleTypeDef structure
 * @retval None
 */
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
    buffer_full_ready = 1;
    full_transfer_count++; // Debug counter
}

/**
 * @brief  Process audio data in the specified buffer
 * @param  buffer: pointer to buffer containing audio data
 * @param  length: length of the buffer to process
 * @retval None
 */
void ProcessAudioData(uint16_t *buffer, uint16_t length)
{
    // Debug: Check if buffer contains non-zero values
    uint8_t has_data = 0;
    uint16_t max_value = 0;
    uint16_t avg_value = 0;
    uint32_t sum = 0;

    printf("ayay data\r\n");

    for (uint16_t i = 0; i < length; i++)
    {
        if (buffer[i] != 0)
        {
            has_data = 1;
            printf("ayay data\r\n");
        }

        if (buffer[i] > max_value)
        {
            max_value = buffer[i];
        }

        sum += buffer[i];
    }

    if (length > 0)
    {
        avg_value = sum / length;
    }

    // TODO: Add your debug output mechanism here
    // For example, toggle LEDs based on audio detection
    // or output to debug UART

    // Example LED debug - assuming you have LEDs connected:
    if (has_data)
    {

        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); // Turn on LED
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // Turn off LED
    }

    // Add your processing code here
}
/**
 * @brief  DMA1 Stream1 IRQ handler (for SAI RX)
 * @retval None
 */
void DMA1_Stream1_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_sai1_a_rx); }

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* User can add his own implementation to report the HAL error */
    __disable_irq();
    while (1)
    {
        // Toggle an LED here if available for debugging
    }
}

/**
 * @brief  SAI MSP Initialization
 * @param  hsai: pointer to a SAI_HandleTypeDef structure
 * @retval None
 */
void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (hsai->Instance == SAI1_Block_A)
    {
        /* SAI1 clock enable */
        __HAL_RCC_SAI1_CLK_ENABLE();

        /* Enable GPIO clocks */
        __HAL_RCC_GPIOE_CLK_ENABLE();

        /* Configure GPIO pins for SAI1 */
        /* SAI1_SD_A pin (data input) - Configure with pull-up for better noise immunity */
        GPIO_InitStruct.Pin = GPIO_PIN_2; // Adjust based on your board schematic
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF6_SAI1;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        /* SAI1_SCK_A, SAI1_FS_A, SAI1_MCLK_A pins */
        GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5; // Adjust based on your board
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    }
}

/**
 * @brief  SAI MSP De-Initialization
 * @param  hsai: pointer to a SAI_HandleTypeDef structure
 * @retval None
 */
void HAL_SAI_MspDeInit(SAI_HandleTypeDef *hsai)
{
    if (hsai->Instance == SAI1_Block_A)
    {
        /* Peripheral clock disable */
        __HAL_RCC_SAI1_CLK_DISABLE();

        /* DeInit GPIO pins - Adjust based on your specific hardware */
        HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    }
}

int _write(int file, char *ptr, int len)
{
    int i = 0;
    for (i = 0; i < len; i++)
    {
        ITM_SendChar(*ptr++);
    }
    return len;
}
