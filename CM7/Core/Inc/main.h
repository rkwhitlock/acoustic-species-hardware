/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "mel_filterbank.h"
#include "mel_spectrogram.h"
#include "stm32h747i_discovery_audio.h"
#include "stm32h747i_discovery_sdram.h"
#include "stm32h7xx_hal.h"

#include <stdint.h>
#include <string.h>

#define BUFFER_SIZE 4096 // Size of the audio buffer

    /* Exported types ------------------------------------------------------------*/
    typedef enum
    {
        AUDIO_ERROR_NONE = 0,
        AUDIO_ERROR_NOTREADY,
        AUDIO_ERROR_IO,
        AUDIO_ERROR_EOF,
    } AUDIO_ErrorTypeDef;
#define SD_DMA_MODE 0U
#define SD_IT_MODE 1U
#define SD_POLLING_MODE 2U

    /* Exported variables --------------------------------------------------------*/
    extern __IO uint32_t SRAMTest;
#ifndef USE_FULL_ASSERT
    extern uint32_t ErrorCounter;
#endif
    extern __IO uint32_t SdramTest;
    extern __IO uint32_t SdmmcTest;

    /* Global variables */
    extern uint16_t audio_buffer[BUFFER_SIZE * 2];

/* Exported constants --------------------------------------------------------*/
/**
 * @brief  SDRAM Write read buffer start address after CAM Frame buffer
 * Assuming Camera frame buffer is of size 800x480 and format ARGB8888 (32 bits per pixel).
 */
#define SDRAM_WRITE_READ_ADDR_OFFSET ((uint32_t)0x0800)

// TODO: check if sdram write read address offset can be 0

/* SDRAM write address */
#define SDRAM_WRITE_READ_ADDR 0xD0177000
#define AUDIO_REC_START_ADDR SDRAM_WRITE_READ_ADDR
#define AUDIO_REC_TOTAL_SIZE ((uint32_t)0x0000E000)
#define AUDIO_RECPDM_START_ADDR (AUDIO_REC_START_ADDR + AUDIO_REC_TOTAL_SIZE)

#define AUDIO_PLAY_SAMPLE 0
#define AUDIO_PLAY_RECORDED 1

/* Exported macro ------------------------------------------------------------*/
#ifdef USE_FULL_ASSERT
/* Assert activated */
#define ASSERT(__condition__)                                                                      \
    do                                                                                             \
    {                                                                                              \
        if (__condition__)                                                                         \
        {                                                                                          \
            assert_failed(__FILE__, __LINE__);                                                     \
            while (1)                                                                              \
                ;                                                                                  \
        }                                                                                          \
    } while (0)
#else
/* Assert not activated : macro has no effect */
#define ASSERT(__condition__)                                                                      \
    do                                                                                             \
    {                                                                                              \
        if (__condition__)                                                                         \
        {                                                                                          \
            ErrorCounter++;                                                                        \
        }                                                                                          \
    } while (0)
#endif /* USE_FULL_ASSERT */

    /* Exported functions ------------------------------------------------------- */
    void SD_DMA_demo(void);
    void SD_IT_demo(void);
    void SD_POLLING_demo(void);
    void Error_Handler(void);
    void SDRAM_demo(void);
    void SDRAM_DMA_demo(void);
    void AudioRecord(void);
#endif /* __MAIN_H */
