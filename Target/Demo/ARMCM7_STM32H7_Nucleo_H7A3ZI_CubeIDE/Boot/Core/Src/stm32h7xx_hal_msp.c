/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : stm32h7xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization
  *                      and de-Initialization codes.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */
#include "stm32h7xx_ll_rcc.h"
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
* @brief PCD MSP Initialization
* This function configures the hardware resources used in this example
* @param hpcd: PCD handle pointer
* @retval None
*/
void HAL_PCD_MspInit(PCD_HandleTypeDef* hpcd)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hpcd->Instance==USB_OTG_HS)
  {
  /* USER CODE BEGIN USB_OTG_HS_MspInit 0 */

  /* USER CODE END USB_OTG_HS_MspInit 0 */
    LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_HSI48);
    LL_PWR_EnableUSBVoltageDetector();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USB_OTG_HS GPIO Configuration
    PA11     ------> USB_OTG_HS_DM
    PA12     ------> USB_OTG_HS_DP
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_USB_OTG_HS_CLK_ENABLE();
  /* USER CODE BEGIN USB_OTG_HS_MspInit 1 */

  /* USER CODE END USB_OTG_HS_MspInit 1 */
  }

}

/**
* @brief PCD MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hpcd: PCD handle pointer
* @retval None
*/
void HAL_PCD_MspDeInit(PCD_HandleTypeDef* hpcd)
{
  if(hpcd->Instance==USB_OTG_HS)
  {
  /* USER CODE BEGIN USB_OTG_HS_MspDeInit 0 */

  /* USER CODE END USB_OTG_HS_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USB_OTG_HS_CLK_DISABLE();

    /**USB_OTG_HS GPIO Configuration
    PA11     ------> USB_OTG_HS_DM
    PA12     ------> USB_OTG_HS_DP
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

  /* USER CODE BEGIN USB_OTG_HS_MspDeInit 1 */

  /* USER CODE END USB_OTG_HS_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */
/**
  * De-Initializes the Global MSP.
  */
void HAL_MspDeInit(void)
{
  /* Reset the RCC clock configuration to the default reset state. */
  LL_RCC_DeInit();

  /* Reset GPIO pin for the LED to turn it off. */
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);

  /* Deinit used GPIOs. */
  LL_GPIO_DeInit(GPIOD);
  LL_GPIO_DeInit(GPIOC);
  LL_GPIO_DeInit(GPIOB);
  LL_GPIO_DeInit(GPIOA);

  /* USB clock disable. */
 // LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_USB2OTGHS);
  /* CAN clock disable. */
  LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_FDCAN);
  /* UART clock disable. */
  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART3);

  /* GPIO ports clock disable. */
  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOD);
  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOC);
  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOB);
  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOA);

  /* SYSCFG clock disable. */
  LL_APB4_GRP1_DisableClock(LL_APB4_GRP1_PERIPH_SYSCFG);
}
/* USER CODE END 1 */
