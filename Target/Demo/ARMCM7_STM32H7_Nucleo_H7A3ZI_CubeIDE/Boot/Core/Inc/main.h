/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "stm32h7xx_ll_rcc.h"
#include "stm32h7xx_ll_crs.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_system.h"
#include "stm32h7xx_ll_exti.h"
#include "stm32h7xx_ll_cortex.h"
#include "stm32h7xx_ll_utils.h"
#include "stm32h7xx_ll_pwr.h"
#include "stm32h7xx_ll_dma.h"
#include "stm32h7xx_ll_gpio.h"

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

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

#define BTN0_Pin LL_GPIO_PIN_13
#define BTN0_GPIO_Port GPIOC
#define RGB_B1_Pin LL_GPIO_PIN_8
#define RGB_B1_GPIO_Port GPIOF
#define RGB_B2_Pin LL_GPIO_PIN_9
#define RGB_B2_GPIO_Port GPIOF
#define RGB_B3_Pin LL_GPIO_PIN_10
#define RGB_B3_GPIO_Port GPIOF
#define RGB_B4_Pin LL_GPIO_PIN_11
#define RGB_B4_GPIO_Port GPIOF
#define RGB_R1_Pin LL_GPIO_PIN_12
#define RGB_R1_GPIO_Port GPIOE
#define RGB_R2_Pin LL_GPIO_PIN_13
#define RGB_R2_GPIO_Port GPIOE
#define RGB_R3_Pin LL_GPIO_PIN_14
#define RGB_R3_GPIO_Port GPIOE
#define RGB_R4_Pin LL_GPIO_PIN_15
#define RGB_R4_GPIO_Port GPIOE
#define LD3_Pin LL_GPIO_PIN_14
#define LD3_GPIO_Port GPIOB
#define STLINK_RX_Pin LL_GPIO_PIN_8
#define STLINK_RX_GPIO_Port GPIOD
#define STLINK_TX_Pin LL_GPIO_PIN_9
#define STLINK_TX_GPIO_Port GPIOD
#define RGB_G4_Pin LL_GPIO_PIN_12
#define RGB_G4_GPIO_Port GPIOD
#define RGB_G3_Pin LL_GPIO_PIN_13
#define RGB_G3_GPIO_Port GPIOD
#define RGB_G2_Pin LL_GPIO_PIN_14
#define RGB_G2_GPIO_Port GPIOD
#define RGB_G1_Pin LL_GPIO_PIN_15
#define RGB_G1_GPIO_Port GPIOD
#define SDMMC1_CD_Pin LL_GPIO_PIN_5
#define SDMMC1_CD_GPIO_Port GPIOD
#define DEBUG_PIN_Pin LL_GPIO_PIN_9
#define DEBUG_PIN_GPIO_Port GPIOG
#define JOY_BTN_Pin LL_GPIO_PIN_0
#define JOY_BTN_GPIO_Port GPIOE
#define LD2_Pin LL_GPIO_PIN_1
#define LD2_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
