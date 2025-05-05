
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file         stm32h7xx_hal_msp.c
 * @brief        This file provides code for the MSP Initialization
 *               and de-Initialization codes.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
 * Initializes the Global MSP.
 */
void HAL_MspInit(void)
{

    /* USER CODE BEGIN MspInit 0 */

    /* USER CODE END MspInit 0 */

    __HAL_RCC_SYSCFG_CLK_ENABLE();

    /* System interrupt init*/

    /* USER CODE BEGIN MspInit 1 */

    /* USER CODE END MspInit 1 */
}

/**
 * @brief UART MSP Initialization
 * This function configures the hardware resources used in this example
 * @param huart: UART handle pointer
 * @retval None
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (huart->Instance == USART1)
    {
        /* USER CODE BEGIN USART1_MspInit 0 */

        /* USER CODE END USART1_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART1 GPIO Configuration
        PA10     ------> USART1_RX
        PA9     ------> USART1_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USER CODE BEGIN USART1_MspInit 1 */

        /* USER CODE END USART1_MspInit 1 */
    }
}

/**
 * @brief UART MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param huart: UART handle pointer
 * @retval None
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        /* USER CODE BEGIN USART1_MspDeInit 0 */

        /* USER CODE END USART1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /**USART1 GPIO Configuration
        PA10     ------> USART1_RX
        PA9     ------> USART1_TX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10 | GPIO_PIN_9);

        /* USER CODE BEGIN USART1_MspDeInit 1 */

        /* USER CODE END USART1_MspDeInit 1 */
    }
}

extern DMA_HandleTypeDef hdma_sai4_a;

// static uint32_t SAI4_client = 0;

// void HAL_SAI_MspInit(SAI_HandleTypeDef* hsai)
// {

//   GPIO_InitTypeDef GPIO_InitStruct;
//   HAL_DMA_MuxSyncConfigTypeDef pSyncConfig;
// /* SAI4 */
//     if(hsai->Instance==SAI4_Block_A)
//     {
//     /* Peripheral clock enable */
//     if (SAI4_client == 0)
//     {
//        __HAL_RCC_SAI4_CLK_ENABLE();

//     /* Peripheral interrupt init*/
//     HAL_NVIC_SetPriority(SAI4_IRQn, 0, 0);
//     HAL_NVIC_EnableIRQ(SAI4_IRQn);
//     }
//     SAI4_client ++;

//     /**SAI4_A_Block_A GPIO Configuration
//     PE2     ------> SAI4_CK1
//     PC1     ------> SAI4_D1
//     */
//     GPIO_InitStruct.Pin = GPIO_PIN_2;
//     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//     GPIO_InitStruct.Alternate = GPIO_AF10_SAI4;
//     HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

//     GPIO_InitStruct.Pin = GPIO_PIN_1;
//     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//     GPIO_InitStruct.Alternate = GPIO_AF10_SAI4;
//     HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//       /* Peripheral DMA init*/

//     hdma_sai4_a.Instance = BDMA_Channel0;
//     hdma_sai4_a.Init.Request = BDMA_REQUEST_SAI4_A;
//     hdma_sai4_a.Init.Direction = DMA_PERIPH_TO_MEMORY;
//     hdma_sai4_a.Init.PeriphInc = DMA_PINC_DISABLE;
//     hdma_sai4_a.Init.MemInc = DMA_MINC_ENABLE;
//     hdma_sai4_a.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
//     hdma_sai4_a.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
//     hdma_sai4_a.Init.Mode = DMA_CIRCULAR;
//     hdma_sai4_a.Init.Priority = DMA_PRIORITY_HIGH;
//     if (HAL_DMA_Init(&hdma_sai4_a) != HAL_OK)
//     {
//       Error_Handler();
//     }

//     pSyncConfig.SyncSignalID = HAL_DMAMUX2_SYNC_EXTI0;
//     pSyncConfig.SyncPolarity = HAL_DMAMUX_SYNC_NO_EVENT;
//     pSyncConfig.SyncEnable = DISABLE;
//     pSyncConfig.EventEnable = ENABLE;
//     pSyncConfig.RequestNumber = 1;
//     if (HAL_DMAEx_ConfigMuxSync(&hdma_sai4_a, &pSyncConfig) != HAL_OK)
//     {
//       Error_Handler();
//     }

//     /* Several peripheral DMA handle pointers point to the same DMA handle.
//      Be aware that there is only one channel to perform all the requested DMAs. */
//     __HAL_LINKDMA(hsai,hdmarx,hdma_sai4_a);

//     __HAL_LINKDMA(hsai,hdmatx,hdma_sai4_a);

//     }
// }

// void HAL_SAI_MspDeInit(SAI_HandleTypeDef *hsai)
// {
//     /* SAI4 */
//     if (hsai->Instance == SAI4_Block_A)
//     {
//         SAI4_client--;
//         if (SAI4_client == 0)
//         {
//             /* Peripheral clock disable */
//             __HAL_RCC_SAI4_CLK_DISABLE();
//             /* SAI4 interrupt DeInit */
//             HAL_NVIC_DisableIRQ(SAI4_IRQn);
//         }

//         /**SAI4_A_Block_A GPIO Configuration
//         PE2     ------> SAI4_CK1
//         PC1     ------> SAI4_D1
//         */
//         HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2);

//         HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1);

//         /* SAI4 DMA Deinit */
//         HAL_DMA_DeInit(hsai->hdmarx);
//         HAL_DMA_DeInit(hsai->hdmatx);
//     }
// }

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
