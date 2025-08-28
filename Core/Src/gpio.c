/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SWD_LED_SEL_GPIO_Port, SWD_LED_SEL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ENC_FUNCTION_CH_A_Pin ENC_LEVEL_CH_A_Pin ENC_LEVEL_CH_B_Pin BTN_LEVEL_Pin
                           ENC_FUNCTION_CH_B_Pin BTN_FUNCTION_Pin BTN_3_Pin BTN_CAMERA_Pin */
  GPIO_InitStruct.Pin = ENC_FUNCTION_CH_A_Pin|ENC_LEVEL_CH_A_Pin|ENC_LEVEL_CH_B_Pin|BTN_LEVEL_Pin
                          |ENC_FUNCTION_CH_B_Pin|BTN_FUNCTION_Pin|BTN_3_Pin|BTN_CAMERA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN_1_Pin BTN_2_Pin */
  GPIO_InitStruct.Pin = BTN_1_Pin|BTN_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ONOFF_Pin */
  GPIO_InitStruct.Pin = ONOFF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ONOFF_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SWD_LED_SEL_Pin */
  GPIO_InitStruct.Pin = SWD_LED_SEL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SWD_LED_SEL_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
