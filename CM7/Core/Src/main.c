/*
 * STM32H7 Audio Reception via CN10 (Line In) using WM8994 codec
 * Modified to use SAI1 and WM8994 instead of SAI4 PDM
 */

#include "main.h"
#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <string.h>

// Audio buffer size
#define BUFFER_LEN 256

// Audio buffer for reception
uint16_t audio_buffer[BUFFER_LEN];

// Buffer processing flags
volatile uint8_t buffer_half_ready = 0;
volatile uint8_t buffer_full_ready = 0;
volatile uint32_t half_transfer_count = 0;
volatile uint32_t full_transfer_count = 0;

// Handle definitions for SAI1 (not SAI4)
DMA_HandleTypeDef hdma_sai1_a_rx;
SAI_HandleTypeDef hsai_BlockA1;
I2C_HandleTypeDef hi2c4; // For WM8994 codec control

// WM8994 I2C address
#define WM8994_I2C_ADDRESS 0x34U

// Function prototypes
void SystemClock_Config(void);
static void GPIO_Init(void);
static void SAI1_Init(void);
static void DMA1_Init(void);
static void I2C4_Init(void);
static void WM8994_Init(void);
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
    GPIO_Init();
    DMA1_Init();   // Using DMA1 for SAI1, not BDMA
    I2C4_Init();   // Initialize I2C for codec control
    SAI1_Init();   // Using SAI1, not SAI4
    WM8994_Init(); // Initialize the audio codec

    // Enable debug LED
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Start SAI reception using DMA
    HAL_StatusTypeDef status =
        HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t *)audio_buffer, BUFFER_LEN);
    if (status != HAL_OK)
    {
        printf("SAI init failed with error: %d\r\n", status);
        Error_Handler();
    }

    while (1)
    {
        // Process buffers when ready
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

        if (current_time - last_check > 2000) // Check every 2 seconds
        {
            last_check = current_time;

            if (half_transfer_count == 0 && full_transfer_count == 0)
            {
                printf("No transfers detected! Restarting SAI...\r\n");

                HAL_SAI_DMAStop(&hsai_BlockA1);
                HAL_Delay(100);

                HAL_StatusTypeDef status = HAL_SAI_Receive_DMA(
                    &hsai_BlockA1, (uint8_t *)audio_buffer, BUFFER_LEN);

                if (status != HAL_OK)
                {
                    printf("SAI restart failed with error: %d\r\n", status);
                }
            }

            half_transfer_count = 0;
            full_transfer_count = 0;
        }
    }
}

// System clock configuration remains the same
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }

    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
    __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);

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

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 |
                                  RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
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

    // Configure the SAI PLL for audio clock (SAI1)
    // For 48kHz audio: MCLK = 256 * Fs = 12.288 MHz
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
    PeriphClkInitStruct.PLL2.PLL2M = 25;  // Input = 25MHz
    PeriphClkInitStruct.PLL2.PLL2N = 344; // VCO = 344 MHz
    PeriphClkInitStruct.PLL2.PLL2P = 7;   // P output = 49.14 MHz (for 256*Fs at 48kHz)
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
static void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();

    /* Configure WM8994 reset pin (PG10) */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* Reset WM8994 */
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);
    HAL_Delay(20);
}

// SAI1 initialization for line input
static void SAI1_Init(void)
{
    /* SAI1 configuration for audio line input with WM8994 */
    hsai_BlockA1.Instance = SAI1_Block_A;
    hsai_BlockA1.Init.Protocol = SAI_FREE_PROTOCOL;
    hsai_BlockA1.Init.AudioMode = SAI_MODESLAVE_RX; // Slave mode (codec is master)
    hsai_BlockA1.Init.DataSize = SAI_DATASIZE_16;
    hsai_BlockA1.Init.FirstBit = SAI_FIRSTBIT_MSB;
    hsai_BlockA1.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE; // Changed for I2S
    hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
    hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
    hsai_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_DISABLE; // Slave mode
    hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_HF;  // Half full
    hsai_BlockA1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_48K;
    hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
    hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
    hsai_BlockA1.Init.TriState = SAI_OUTPUT_NOTRELEASED; // Add this

    // Frame configuration for I2S standard
    hsai_BlockA1.FrameInit.FrameLength = 64; // 32 bits per channel
    hsai_BlockA1.FrameInit.ActiveFrameLength = 32;
    hsai_BlockA1.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
    hsai_BlockA1.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
    hsai_BlockA1.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT; // I2S standard

    // Slot configuration
    hsai_BlockA1.SlotInit.FirstBitOffset = 0;
    hsai_BlockA1.SlotInit.SlotSize = SAI_SLOTSIZE_32B; // 32-bit slots
    hsai_BlockA1.SlotInit.SlotNumber = 2;              // Stereo
    hsai_BlockA1.SlotInit.SlotActive = SAI_SLOTACTIVE_ALL;

    if (HAL_SAI_Init(&hsai_BlockA1) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_NVIC_SetPriority(SAI1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(SAI1_IRQn);
}

// DMA1 initialization for SAI1
static void DMA1_Init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* Configure DMA request hdma_sai1_a_rx for SAI1_A */
    hdma_sai1_a_rx.Instance = DMA1_Stream1;
    hdma_sai1_a_rx.Init.Request = DMA_REQUEST_SAI1_A;
    hdma_sai1_a_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sai1_a_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sai1_a_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sai1_a_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_sai1_a_rx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_sai1_a_rx.Init.Mode = DMA_CIRCULAR;
    hdma_sai1_a_rx.Init.Priority = DMA_PRIORITY_HIGH;
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

    /* DMA interrupt Init */
    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
}

// I2C4 initialization for WM8994 codec control
static void I2C4_Init(void)
{
    hi2c4.Instance = I2C4;
    hi2c4.Init.Timing = 0x10707DBC; // Calculated for 100KHz at 400MHz
    hi2c4.Init.OwnAddress1 = 0;
    hi2c4.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c4.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c4.Init.OwnAddress2 = 0;
    hi2c4.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c4.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c4.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c4) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure Analogue filter */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c4, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure Digital filter */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c4, 0) != HAL_OK)
    {
        Error_Handler();
    }
}

// WM8994 register addresses (16-bit addressing)
#define WM8994_WRITE_SEQUENCER_0 0x00
#define WM8994_SOFTWARE_RESET 0x00
#define WM8994_POWER_MANAGEMENT_1 0x01
#define WM8994_POWER_MANAGEMENT_2 0x02
#define WM8994_POWER_MANAGEMENT_3 0x03
#define WM8994_POWER_MANAGEMENT_4 0x04
#define WM8994_POWER_MANAGEMENT_5 0x05
#define WM8994_POWER_MANAGEMENT_6 0x06
#define WM8994_INPUT_MIXER_2 0x1B
#define WM8994_INPUT_MIXER_3 0x1C
#define WM8994_INPUT_MIXER_4 0x1D
#define WM8994_LEFT_LINE_INPUT_1_2_VOLUME 0x18
#define WM8994_RIGHT_LINE_INPUT_1_2_VOLUME 0x1A
#define WM8994_LEFT_OUTPUT_VOLUME 0x1C
#define WM8994_RIGHT_OUTPUT_VOLUME 0x1D
#define WM8994_LINE_OUTPUTS_VOLUME 0x1E
#define WM8994_ANTIPOP_2 0x39
#define WM8994_CHARGE_PUMP_1 0x4C
#define WM8994_CLASS_W_1 0x51
#define WM8994_DC_SERVO_1 0x54
#define WM8994_DC_SERVO_4 0x57
#define WM8994_ANALOGUE_HP_1 0x60
#define WM8994_CHIP_REVISION 0x100
#define WM8994_CONTROL_INTERFACE 0x101
#define WM8994_CLOCKING_1 0x208
#define WM8994_CLOCKING_2 0x209
#define WM8994_AIF1_CLOCKING_1 0x200
#define WM8994_AIF1_CLOCKING_2 0x201
#define WM8994_AIF1_CONTROL_1 0x300
#define WM8994_AIF1_CONTROL_2 0x301
#define WM8994_AIF1_MASTER_SLAVE 0x302
#define WM8994_AIF1_RATE 0x210
#define WM8994_AIF1_DAC1_LEFT_VOLUME 0x402
#define WM8994_AIF1_DAC1_RIGHT_VOLUME 0x403
#define WM8994_AIF1_DAC1_LEFT_MIXER_ROUTING 0x601
#define WM8994_AIF1_DAC1_RIGHT_MIXER_ROUTING 0x602
#define WM8994_AIF1_ADC1_LEFT_VOLUME 0x400
#define WM8994_AIF1_ADC1_RIGHT_VOLUME 0x401
#define WM8994_GPIO_1 0x700

// WM8994 I2C helper functions
static HAL_StatusTypeDef WM8994_WriteRegister(uint16_t reg, uint16_t value)
{
    uint8_t data[4];

    data[0] = (reg >> 8) & 0xFF;   // Register address high byte
    data[1] = reg & 0xFF;          // Register address low byte
    data[2] = (value >> 8) & 0xFF; // Value high byte
    data[3] = value & 0xFF;        // Value low byte

    return HAL_I2C_Master_Transmit(&hi2c4, WM8994_I2C_ADDRESS, data, 4, 100);
}

static HAL_StatusTypeDef WM8994_ReadRegister(uint16_t reg, uint16_t *value)
{
    uint8_t addr[2];
    uint8_t data[2];
    HAL_StatusTypeDef status;

    addr[0] = (reg >> 8) & 0xFF;
    addr[1] = reg & 0xFF;

    status = HAL_I2C_Master_Transmit(&hi2c4, WM8994_I2C_ADDRESS, addr, 2, 100);
    if (status != HAL_OK)
        return status;

    status = HAL_I2C_Master_Receive(&hi2c4, WM8994_I2C_ADDRESS, data, 2, 100);
    if (status != HAL_OK)
        return status;

    *value = (data[0] << 8) | data[1];
    return HAL_OK;
}

// Complete WM8994 initialization for line input
static void WM8994_Init(void)
{
    uint16_t id;

    // Software reset
    WM8994_WriteRegister(WM8994_SOFTWARE_RESET, 0x0000);
    HAL_Delay(50);

    // Read device ID to verify communication
    if (WM8994_ReadRegister(WM8994_CHIP_REVISION, &id) != HAL_OK)
    {
        printf("Failed to read WM8994 ID\r\n");
        Error_Handler();
    }
    printf("WM8994 ID: 0x%04X\r\n", id);

    // Power up sequence
    WM8994_WriteRegister(WM8994_ANTIPOP_2, 0x0068); // Anti-pop configuration

    // Enable VMID and BIAS
    WM8994_WriteRegister(WM8994_POWER_MANAGEMENT_1, 0x0003);
    HAL_Delay(50); // Wait for VMID to ramp up

    // Enable Line inputs
    WM8994_WriteRegister(WM8994_POWER_MANAGEMENT_2, 0x6110); // Enable IN1L/R

    // Enable ADCs
    WM8994_WriteRegister(WM8994_POWER_MANAGEMENT_4, 0x0303); // Enable ADC1L/R

    // Configure Line input path
    WM8994_WriteRegister(WM8994_INPUT_MIXER_2, 0x0040); // IN1LP to IN1L
    WM8994_WriteRegister(WM8994_INPUT_MIXER_3, 0x0040); // IN1RP to IN1R

    // Set input volumes (0dB gain)
    WM8994_WriteRegister(WM8994_LEFT_LINE_INPUT_1_2_VOLUME, 0x000B);
    WM8994_WriteRegister(WM8994_RIGHT_LINE_INPUT_1_2_VOLUME, 0x000B);

    // Configure clocking - codec as master
    WM8994_WriteRegister(WM8994_CLOCKING_1, 0x000F);      // AIF1 clock enable
    WM8994_WriteRegister(WM8994_AIF1_CLOCKING_1, 0x0010); // AIF1CLK = MCLK1

    // Configure AIF1 interface - 16bit, I2S format
    WM8994_WriteRegister(WM8994_AIF1_CONTROL_1, 0x4010);    // 16bit, I2S
    WM8994_WriteRegister(WM8994_AIF1_MASTER_SLAVE, 0x4000); // Master mode

    // Set sample rate to 48kHz
    WM8994_WriteRegister(WM8994_AIF1_RATE, 0x0003); // 48kHz

    // Enable AIF1 ADC paths
    WM8994_WriteRegister(WM8994_AIF1_ADC1_LEFT_VOLUME, 0x00C0);  // Enable and 0dB
    WM8994_WriteRegister(WM8994_AIF1_ADC1_RIGHT_VOLUME, 0x00C0); // Enable and 0dB

    // Route ADC to AIF1
    WM8994_WriteRegister(0x606, 0x0002); // ADC1L to AIF1ADC1L
    WM8994_WriteRegister(0x607, 0x0002); // ADC1R to AIF1ADC1R

    // Enable AIF1
    WM8994_WriteRegister(WM8994_POWER_MANAGEMENT_5, 0x0303); // Enable AIF1 ADC

    // Final power up
    WM8994_WriteRegister(WM8994_POWER_MANAGEMENT_1, 0x3003); // Enable all required blocks

    HAL_Delay(100); // Wait for all blocks to stabilize

    printf("WM8994 initialized for line input\r\n");
}

// HAL callbacks remain the same
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    buffer_half_ready = 1;
    half_transfer_count++;

    if (half_transfer_count % 100 == 0)
    {
        printf("Half transfer #%lu\r\n", half_transfer_count);
    }
}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
    buffer_full_ready = 1;
    full_transfer_count++;

    if (full_transfer_count % 100 == 0)
    {
        printf("Full transfer #%lu\r\n", full_transfer_count);
    }
}

void ProcessAudioData(uint16_t *buffer, uint16_t length)
{
    uint8_t has_data = 0;
    uint16_t max_value = 0;
    uint16_t avg_value = 0;
    uint32_t sum = 0;

    for (uint16_t i = 0; i < length; i++)
    {
        if (buffer[i] != 0)
        {
            has_data = 1;
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

    // Add your processing code here
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

// SAI1 MSP Init
void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (hsai->Instance == SAI1_Block_A)
    {
        /* SAI1 clock enable */
        __HAL_RCC_SAI1_CLK_ENABLE();

        /* Enable GPIO clocks */
        __HAL_RCC_GPIOE_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();

        /* SAI1 GPIO Configuration
         * PG7  ------> SAI1_MCLK_A  (Master Clock)
         * PE2  ------> SAI1_MCLK_A  (Alternate)
         * PE4  ------> SAI1_FS_A     (Frame Sync/Word Select)
         * PE5  ------> SAI1_SCK_A    (Bit Clock)
         * PE6  ------> SAI1_SD_A     (Serial Data)
         */

        /* Configure SAI1_FS_A, SAI1_SCK_A, SAI1_SD_A */
        GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF6_SAI1;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        /* Configure SAI1_MCLK_A */
        GPIO_InitStruct.Pin = GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF6_SAI1;
        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

        /* Configure PE2 as alternate MCLK output */
        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF6_SAI1;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    }
}

// I2C4 MSP Init
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (hi2c->Instance == I2C4)
    {
        __HAL_RCC_I2C4_CLK_ENABLE();

        /* I2C4 GPIO Configuration
         * PD12 ------> I2C4_SCL
         * PD13 ------> I2C4_SDA
         */
        GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C4;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    }
}

// SAI1 MSP DeInit
void HAL_SAI_MspDeInit(SAI_HandleTypeDef *hsai)
{
    if (hsai->Instance == SAI1_Block_A)
    {
        /* Peripheral clock disable */
        __HAL_RCC_SAI1_CLK_DISABLE();

        /* DeInit GPIO pins */
        HAL_GPIO_DeInit(GPIOE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
        HAL_GPIO_DeInit(GPIOG, GPIO_PIN_7);
    }
}

// Interrupt handlers
void DMA1_Stream1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_sai1_a_rx);
}

void SAI1_IRQHandler(void)
{
    HAL_SAI_IRQHandler(&hsai_BlockA1);
}

void I2C4_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c4);
}

void I2C4_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c4);
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