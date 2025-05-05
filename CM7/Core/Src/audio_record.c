/**
 ******************************************************************************
 * @file    BSP/CM7/Src/audio_record.c
 * @author  MCD Application Team
 * @brief   This example describes how to use DFSDM HAL API to realize
 *          audio recording.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "math.h"
#include <stdint.h>
#include <stdio.h>

/** @addtogroup BSP_Examples
 * @{
 */

#define REFERENCE_VOLTAGE 0.00002f
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Audio frequency */
extern AUDIO_ErrorTypeDef AUDIO_Start(uint32_t audio_start_address, uint32_t audio_file_size);
#define AUDIO_FREQUENCY 16000U
#define AUDIO_IN_PDM_BUFFER_SIZE (uint32_t)(128 * AUDIO_FREQUENCY / 16000 * 2)
#define AUDIO_NB_BLOCKS ((uint32_t)4)
#define AUDIO_BLOCK_SIZE ((uint32_t)0xFFFE)
/* Size of the recorder buffer */
#define RECORD_BUFFER_SIZE 4096
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Define record Buf at D3SRAM @0x38000000 since the BDMA for SAI4 use only this memory */
#if defined(__CC_ARM) /* !< ARM Compiler */
ALIGN_32BYTES(uint16_t recordPDMBuf[AUDIO_IN_PDM_BUFFER_SIZE]) __attribute__((section(".RAM_D3")));

#elif defined(__ICCARM__) /* !< ICCARM Compiler */
#pragma location = 0x38000000
ALIGN_32BYTES(uint16_t recordPDMBuf[AUDIO_IN_PDM_BUFFER_SIZE]);
#elif defined(__GNUC__) /* !< GNU Compiler */
ALIGN_32BYTES(uint16_t recordPDMBuf[AUDIO_IN_PDM_BUFFER_SIZE]) __attribute__((section(".RAM_D3")));
#endif
static uint32_t AudioFreq[9] = {8000, 11025, 16000, 22050, 32000, 44100, 48000, 96000, 192000};
ALIGN_32BYTES(uint16_t RecPlayback[2 * RECORD_BUFFER_SIZE]);
ALIGN_32BYTES(uint16_t PlaybackBuffer[2 * RECORD_BUFFER_SIZE]);
uint32_t VolumeLevel = 80;
uint32_t InState = 0;
uint32_t OutState = 0;
uint32_t *AudioFreq_ptr;
uint16_t playbackBuf[RECORD_BUFFER_SIZE * 2];
BSP_AUDIO_Init_t AudioInInit;
BSP_AUDIO_Init_t AudioOutInit;
/* Pointer to record_data */
uint32_t playbackPtr;
uint32_t AudioBufferOffset;
/* Private function prototypes -----------------------------------------------*/
typedef enum
{
    BUFFER_OFFSET_NONE = 0,
    BUFFER_OFFSET_HALF,
    BUFFER_OFFSET_FULL,
} BUFFER_StateTypeDef;
/* Private functions ---------------------------------------------------------*/

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
 * @brief Test Audio record.
 *   The main objective of this test is to check the hardware connection of the
 *   Audio peripheral.
 * @param  None
 * @retval None
 */
void AudioRecord_demo(void)
{
    uint32_t channel_nbr = 2;

    AudioFreq_ptr = AudioFreq + 2; /* AUDIO_FREQUENCY_16K; */

    AudioOutInit.Device = AUDIO_OUT_DEVICE_AUTO;
    AudioOutInit.ChannelsNbr = channel_nbr;
    AudioOutInit.SampleRate = *AudioFreq_ptr;
    AudioOutInit.BitsPerSample = AUDIO_RESOLUTION_16B;
    AudioOutInit.Volume = VolumeLevel;

    AudioInInit.Device = AUDIO_IN_DEVICE_DIGITAL_MIC;
    AudioInInit.ChannelsNbr = channel_nbr;
    AudioInInit.SampleRate = *AudioFreq_ptr;
    AudioInInit.BitsPerSample = AUDIO_RESOLUTION_16B;
    AudioInInit.Volume = VolumeLevel;

    /* Initialize Audio Recorder with 2 channels to be used */
    BSP_AUDIO_IN_Init(1, &AudioInInit);
    BSP_AUDIO_IN_GetState(1, &InState);

    BSP_AUDIO_OUT_Init(0, &AudioOutInit);

    BSP_AUDIO_OUT_SetDevice(0, AUDIO_OUT_DEVICE_HEADPHONE);

    /* Start Recording */
    BSP_AUDIO_IN_RecordPDM(1, (uint8_t *)&recordPDMBuf, 2 * AUDIO_IN_PDM_BUFFER_SIZE);

    /* Play the recorded buffer*/
    BSP_AUDIO_OUT_Play(0, (uint8_t *)&RecPlayback[0], 2 * RECORD_BUFFER_SIZE);
    while (1)
    {

        for (int i = 0; i < 10; i++)
        {
            printf("[%d]: %hd\r\n", i, recordPDMBuf[i]);
        }

        /* Calculate and display sound pressure level */
        float decibel_level = calculate_decibel(recordPDMBuf, AUDIO_IN_PDM_BUFFER_SIZE);
        // if (CheckForUserInput() > 0)
        // {
        //     BSP_AUDIO_OUT_Stop(0);
        //     BSP_AUDIO_OUT_DeInit(0);
        //     BSP_AUDIO_IN_Stop(1);
        //     BSP_AUDIO_IN_DeInit(1);
        //     return;
        // }
    }
}

/**
 * @brief Calculates the remaining file size and new position of the pointer.
 * @param  None
 * @retval None
 */
void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance)
{
    if (Instance == 1U)
    {
        /* Invalidate Data Cache to get the updated content of the SRAM*/
        SCB_InvalidateDCache_by_Addr((uint32_t *)&recordPDMBuf[AUDIO_IN_PDM_BUFFER_SIZE / 2],
                                     AUDIO_IN_PDM_BUFFER_SIZE * 2);

        BSP_AUDIO_IN_PDMToPCM(Instance, (uint16_t *)&recordPDMBuf[AUDIO_IN_PDM_BUFFER_SIZE / 2],
                              &RecPlayback[playbackPtr]);

        /* Clean Data Cache to update the content of the SRAM */
        SCB_CleanDCache_by_Addr((uint32_t *)&RecPlayback[playbackPtr],
                                AUDIO_IN_PDM_BUFFER_SIZE / 4);

        playbackPtr += AUDIO_IN_PDM_BUFFER_SIZE / 4 / 2;
        if (playbackPtr >= RECORD_BUFFER_SIZE)
            playbackPtr = 0;
    }
    else
    {
        AudioBufferOffset = BUFFER_OFFSET_FULL;
    }
}

/**
 * @brief  Manages the DMA Half Transfer complete interrupt.
 * @param  None
 * @retval None
 */
void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance)
{
    if (Instance == 1U)
    {
        /* Invalidate Data Cache to get the updated content of the SRAM*/
        SCB_InvalidateDCache_by_Addr((uint32_t *)&recordPDMBuf[0], AUDIO_IN_PDM_BUFFER_SIZE * 2);

        BSP_AUDIO_IN_PDMToPCM(Instance, (uint16_t *)&recordPDMBuf[0], &RecPlayback[playbackPtr]);

        /* Clean Data Cache to update the content of the SRAM */
        SCB_CleanDCache_by_Addr((uint32_t *)&RecPlayback[playbackPtr],
                                AUDIO_IN_PDM_BUFFER_SIZE / 4);

        playbackPtr += AUDIO_IN_PDM_BUFFER_SIZE / 4 / 2;
        if (playbackPtr >= RECORD_BUFFER_SIZE)
        {
            playbackPtr = 0;
        }
    }
    else
    {
        AudioBufferOffset = BUFFER_OFFSET_HALF;
    }
}

/**
 * @brief  Audio IN Error callback function
 * @param  None
 * @retval None
 */
void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance)
{
    /* Stop the program with an infinite loop */
    // Error_Handler();
}
/**
 * @}
 */
