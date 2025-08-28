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
#include "stm32f0xx_hal.h"

    /* Private includes ----------------------------------------------------------*/
    /* USER CODE BEGIN Includes */

    /* USER CODE END Includes */

    /* Exported types ------------------------------------------------------------*/
    /* USER CODE BEGIN ET */

    /* USER CODE END ET */

    /* Exported constants --------------------------------------------------------*/
    /* USER CODE BEGIN EC */

    /* USER CODE END EC */

    /* Exported macro ------------------------------------------------------------*/
    /* USER CODE BEGIN EM */
    // #define DEBUG
    /* USER CODE END EM */

    /* Exported functions prototypes ---------------------------------------------*/
    void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ENC_FUNCTION_CH_A_Pin GPIO_PIN_1
#define ENC_FUNCTION_CH_A_GPIO_Port GPIOA
#define ENC_LEVEL_CH_A_Pin GPIO_PIN_2
#define ENC_LEVEL_CH_A_GPIO_Port GPIOA
#define ENC_LEVEL_CH_B_Pin GPIO_PIN_3
#define ENC_LEVEL_CH_B_GPIO_Port GPIOA
#define BTN_LEVEL_Pin GPIO_PIN_4
#define BTN_LEVEL_GPIO_Port GPIOA
#define ENC_FUNCTION_CH_B_Pin GPIO_PIN_5
#define ENC_FUNCTION_CH_B_GPIO_Port GPIOA
#define BTN_FUNCTION_Pin GPIO_PIN_6
#define BTN_FUNCTION_GPIO_Port GPIOA
#define BTN_3_Pin GPIO_PIN_7
#define BTN_3_GPIO_Port GPIOA
#define LED_GRN_Pin GPIO_PIN_0
#define LED_GRN_GPIO_Port GPIOB
#define LED_RED_Pin GPIO_PIN_1
#define LED_RED_GPIO_Port GPIOB
#define BTN_CAMERA_Pin GPIO_PIN_8
#define BTN_CAMERA_GPIO_Port GPIOA
#define BTN_1_Pin GPIO_PIN_9
#define BTN_1_GPIO_Port GPIOA
#define BTN_2_Pin GPIO_PIN_10
#define BTN_2_GPIO_Port GPIOA
#define ONOFF_Pin GPIO_PIN_3
#define ONOFF_GPIO_Port GPIOB
#define SWD_LED_SEL_Pin GPIO_PIN_4
#define SWD_LED_SEL_GPIO_Port GPIOB

    /* USER CODE BEGIN Private defines */

    /* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
