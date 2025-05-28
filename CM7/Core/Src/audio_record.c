/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Audio frequency */
extern AUDIO_ErrorTypeDef AUDIO_Start(uint32_t audio_start_address, uint32_t audio_file_size);
#define AUDIO_FREQUENCY 16000U
#define AUDIO_IN_PDM_BUFFER_SIZE (uint32_t)(128 * AUDIO_FREQUENCY / 16000 * 2)
#define AUDIO_NB_BLOCKS ((uint32_t)4)
#define AUDIO_BLOCK_SIZE ((uint32_t)0xFFFE)

// fft hyperparams for audio processing
#define FFT_SIZE 512
#define HOP_LENGTH 256
#define MEL_BANDS 64
#define PCM_SCALING (1.0f / 32768.0f)

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
ALIGN_32BYTES(uint16_t PCMBuffer[2 * BUFFER_SIZE]);
ALIGN_32BYTES(uint16_t PlaybackBuffer[2 * BUFFER_SIZE]);
uint32_t VolumeLevel = 80;
uint32_t InState = 0;
uint32_t OutState = 0;
uint32_t *AudioFreq_ptr;
uint16_t playbackBuf[BUFFER_SIZE * 2];
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
/**
 * @brief Test Audio record.
 *   The main objective of this test is to check the hardware connection of the
 *   Audio peripheral.
 * @param  None
 * @retval None
 */
void AudioRecord(void)
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
    BSP_AUDIO_OUT_Play(0, (uint8_t *)&PCMBuffer[0], 2 * BUFFER_SIZE);

    // IMPLEMENT SOME SEGMENT OR DUTY CYCLING for recording?

    // PCMBuffer is filled with the recorded data

    // CODE HERE TO PROCESS PCMBuffer
    // ALIGN_32BYTES(uint16_t PCMBuffer[2 * BUFFER_SIZE]);

    // USE CMSIS DSP Library to process PCMBuffer

    // define configuration - match trained model
    MelSpectrogramConfig_t config = {.fft_size = 512,
                                     .hop_length = 256,
                                     .n_mels = 64,
                                     .sample_rate = 16000.0f,
                                     .f_min = 0.0f,
                                     .f_max = 8000.0f};

    mel_spectrogram_init(&config);

    // output spectrogram buffer
    // n_mels x n_frames
    static float mel_spec[64 * 64];
    // zero out mel spectrogram buffer
    memset(mel_spec, 0, sizeof(mel_spec));

    // call DSP pipeline for PCMBuffer -> mel_spec
    int n_frames = calculate_mel_spectrogram((const int16_t *)PCMBuffer, BUFFER_SIZE, mel_spec,
                                             64); // max columns

    // normalize to [0, 1]
    normalize_spectrogram(mel_spec, config.n_mels, n_frames);

    // DO STUFF FOR ML INFERENCE
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
                              &PCMBuffer[playbackPtr]);

        /* Clean Data Cache to update the content of the SRAM */
        SCB_CleanDCache_by_Addr((uint32_t *)&PCMBuffer[playbackPtr], AUDIO_IN_PDM_BUFFER_SIZE / 4);

        playbackPtr += AUDIO_IN_PDM_BUFFER_SIZE / 4 / 2;
        if (playbackPtr >= BUFFER_SIZE)
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

        BSP_AUDIO_IN_PDMToPCM(Instance, (uint16_t *)&recordPDMBuf[0], &PCMBuffer[playbackPtr]);

        /* Clean Data Cache to update the content of the SRAM */
        SCB_CleanDCache_by_Addr((uint32_t *)&PCMBuffer[playbackPtr], AUDIO_IN_PDM_BUFFER_SIZE / 4);

        playbackPtr += AUDIO_IN_PDM_BUFFER_SIZE / 4 / 2;
        if (playbackPtr >= BUFFER_SIZE)
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
    Error_Handler();
}
/**
 * @}
 */
