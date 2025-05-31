/**
  ******************************************************************************
  * @file    tinyaudiocnn_data_params.h
  * @author  AST Embedded Analytics Research Platform
  * @date    2025-05-31T16:52:42-0700
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#ifndef TINYAUDIOCNN_DATA_PARAMS_H
#define TINYAUDIOCNN_DATA_PARAMS_H

#include "ai_platform.h"

/*
#define AI_TINYAUDIOCNN_DATA_WEIGHTS_PARAMS \
  (AI_HANDLE_PTR(&ai_tinyaudiocnn_data_weights_params[1]))
*/

#define AI_TINYAUDIOCNN_DATA_CONFIG               (NULL)


#define AI_TINYAUDIOCNN_DATA_ACTIVATIONS_SIZES \
  { 197184, }
#define AI_TINYAUDIOCNN_DATA_ACTIVATIONS_SIZE     (197184)
#define AI_TINYAUDIOCNN_DATA_ACTIVATIONS_COUNT    (1)
#define AI_TINYAUDIOCNN_DATA_ACTIVATION_1_SIZE    (197184)



#define AI_TINYAUDIOCNN_DATA_WEIGHTS_SIZES \
  { 24344, }
#define AI_TINYAUDIOCNN_DATA_WEIGHTS_SIZE         (24344)
#define AI_TINYAUDIOCNN_DATA_WEIGHTS_COUNT        (1)
#define AI_TINYAUDIOCNN_DATA_WEIGHT_1_SIZE        (24344)



#define AI_TINYAUDIOCNN_DATA_ACTIVATIONS_TABLE_GET() \
  (&g_tinyaudiocnn_activations_table[1])

extern ai_handle g_tinyaudiocnn_activations_table[1 + 2];



#define AI_TINYAUDIOCNN_DATA_WEIGHTS_TABLE_GET() \
  (&g_tinyaudiocnn_weights_table[1])

extern ai_handle g_tinyaudiocnn_weights_table[1 + 2];


#endif    /* TINYAUDIOCNN_DATA_PARAMS_H */
