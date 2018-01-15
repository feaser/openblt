/************************************************************************************//**
* \file         Demo\ARMCM4_STM32F3_Nucleo_F303K8_TrueStudio\Boot\main.c
* \brief        Bootloader application source file.
* \ingroup      Boot_ARMCM4_STM32F3_Nucleo_F303K8_TrueStudio
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2017  by Feaser    http://www.feaser.com    All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option) any later
* version.
*
* OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with OpenBLT. It
* should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
*
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */
#include "stm32f3xx.h"                           /* STM32 CPU and HAL header           */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);
static void SystemClock_Config(void);


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called
**            by the reset interrupt vector after the C-startup routines executed.
** \return    Program return code.
**
****************************************************************************************/
int main(void)
{
  /* initialize the microcontroller */
  Init();
  /* initialize the bootloader */
  BootInit();

  /* start the infinite program loop */
  while (1)
  {
    /* run the bootloader task */
    BootTask();
  }

  /* program should never get here */
  return 0;
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  /* HAL library initialization */
  HAL_Init();
  /* configure system clock */
  SystemClock_Config();
} /*** end of Init ***/


/************************************************************************************//**
** \brief     System Clock Configuration. This code was created by CubeMX and configures
**            the system clock to match the configuration in the bootloader's
**            configuration (blt_conf.h), specifically the macros:
**            BOOT_CPU_SYSTEM_SPEED_KHZ and BOOT_CPU_XTAL_SPEED_KHZ.
** \return    none.
**
****************************************************************************************/
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /* Initializes the CPU, AHB and APB busses clocks. */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization error. */
    ASSERT_RT(BLT_FALSE);
  }

  /* Initializes the CPU, AHB and APB busses clocks. */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    /* Initialization error. */
    ASSERT_RT(BLT_FALSE);
  }
} /*** end of SystemClock_Config ***/


/************************************************************************************//**
** \brief     Initializes the Global MSP. This function is called from HAL_Init()
**            function to perform system level initialization (GPIOs, clock, DMA,
**            interrupt).
** \return    none.
**
****************************************************************************************/
void HAL_MspInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO ports clock enable. */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure GPIO pin for the LED. */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);

  /* Configure GPIO pin for (optional) backdoor entry input. */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
} /*** end of HAL_MspInit ***/


/************************************************************************************//**
** \brief     DeInitializes the Global MSP. This function is called from HAL_DeInit()
**            function to perform system level de-initialization (GPIOs, clock, DMA,
**            interrupt).
** \return    none.
**
****************************************************************************************/
void HAL_MspDeInit(void)
{
  /* Deconfigure GPIO pin for (optional) backdoor entry input. */
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);

  /* Deconfigure GPIO pin for the LED. */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);

  /* GPIO ports clock disable. */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
} /*** end of HAL_MspDeInit ***/


/************************************************************************************//**
** \brief     UART MSP Init.
** \param     huart Pointer to a UART_HandleTypeDef structure that contains the
**            configuration information for the specified UART module.
** \return    none.
**
****************************************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
#if (BOOT_COM_UART_ENABLE > 0)
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Peripheral clock enable. */
  __HAL_RCC_USART2_CLK_ENABLE();
  /* UART TX and RX GPIO pin configuration. */
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif /* BOOT_COM_UART_ENABLE > 0 */
} /*** end of HAL_UART_MspInit ***/


/************************************************************************************//**
** \brief     UART MSP DeInit.
** \param     huart Pointer to a UART_HandleTypeDef structure that contains the
**            configuration information for the specified UART module.
** \return    none.
**
****************************************************************************************/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
#if (BOOT_COM_UART_ENABLE > 0)
  /* Reset UART GPIO pin configuration. */
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_15);
  /* Peripheral clock disable. */
  __HAL_RCC_USART2_CLK_DISABLE();
#endif /* BOOT_COM_UART_ENABLE > 0 */
} /*** end of HAL_UART_MspDeInit ***/


/************************************************************************************//**
** \brief     CAN MSP Init.
** \param     hcan Pointer to a CAN_HandleTypeDef structure that contains the
**            configuration information for the specified CAN module.
** \return    none.
**
****************************************************************************************/
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
#if (BOOT_COM_CAN_ENABLE > 0)
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Peripheral clock enable. */
  __HAL_RCC_CAN1_CLK_ENABLE();
  /* CAN TX and RX GPIO pin configuration. */
  GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_CAN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif /* BOOT_COM_CAN_ENABLE > 0 */
} /*** end of HAL_CAN_MspInit ***/


/************************************************************************************//**
** \brief     CAN MSP DeInit.
** \param     hcan Pointer to a CAN_HandleTypeDef structure that contains the
**            configuration information for the specified CAN module.
** \return    none.
**
****************************************************************************************/
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{
#if (BOOT_COM_CAN_ENABLE > 0)
  /* Reset CAN GPIO pin configuration. */
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11 | GPIO_PIN_12);
  /* Peripheral clock enable. */
  __HAL_RCC_CAN1_CLK_DISABLE();
#endif /* BOOT_COM_CAN_ENABLE > 0 */
} /*** end of HAL_CAN_MspDeInit ***/


/*********************************** end of main.c *************************************/
