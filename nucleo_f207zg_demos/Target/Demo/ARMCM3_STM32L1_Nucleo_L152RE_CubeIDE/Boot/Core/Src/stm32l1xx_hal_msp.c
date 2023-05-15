/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32l1xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */
#include "stm32l1xx_ll_rcc.h"
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

  __HAL_RCC_COMP_CLK_ENABLE();
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/* USER CODE BEGIN 1 */
/**
  * De-Initializes the Global MSP.
  */
void HAL_MspDeInit(void)
{
  /* Reset the RCC clock configuration to the default reset state. */
  LL_RCC_DeInit();

  /* Deinit used GPIOs. */
  LL_GPIO_DeInit(GPIOH);
  LL_GPIO_DeInit(GPIOC);
  LL_GPIO_DeInit(GPIOA);

  /* UART clock disable. */
  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART2);

  /* GPIO ports clock disable. */
  LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
  LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

  /* COMP, SYSCFG and PWR clock disable. */
  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_COMP);
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
