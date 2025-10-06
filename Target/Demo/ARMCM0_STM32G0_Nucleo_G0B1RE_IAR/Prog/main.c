/************************************************************************************//**
* \file         Demo/ARMCM0_STM32G0_Nucleo_G0B1RE_IAR/Prog/main.c
* \brief        Demo program application source file.
* \ingroup      Prog_ARMCM0_STM32G0_Nucleo_G0B1RE_IAR
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2021  by Feaser    http://www.feaser.com    All rights reserved
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
#include "header.h"                                    /* generic header               */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);
static void SystemClock_Config(void);
static void VectorBase_Config(void);


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called 
**            by the reset interrupt vector after the C-startup routines executed.
** \return    none.
**
****************************************************************************************/
void main(void)
{
  /* initialize the microcontroller */
  Init();
  /* initialize the bootloader interface */
  BootComInit();

  /* start the infinite program loop */
  while (1)
  {
    /* toggle LED with a fixed frequency */
    LedToggle();
    /* check for bootloader activation request */
    BootComCheckActivationRequest();
  }
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  /* configure the vector table base address. */
  VectorBase_Config();
  /* reset of all peripherals, initializes the Flash interface and the Systick. */
  HAL_Init();
  /* configure the system clock. */
  SystemClock_Config();
  /* initialize the timer driver */
  TimerInit();
  /* initialize the led driver */
  LedInit();
} /*** end of Init ***/


/************************************************************************************//**
** \brief     Vector base address configuration. It should no longer be at the start of
**            flash memory but moved forward because the first part of flash is
**            reserved for the bootloader. Note that this is already done by the
**            bootloader before starting this program. Unfortunately, function
**            SystemInit() overwrites this change again. 
** \return    none.
**
****************************************************************************************/
static void VectorBase_Config(void)
{
  /* The constant array with vectors of the vector table is declared externally in the
   * c-startup code.
   */
  extern const unsigned long __vector_table[];

  /* Remap the vector table to where the vector table is located for this program. */
  SCB->VTOR = (unsigned long)&__vector_table[0];
} /*** end of VectorBase_Config ***/


/************************************************************************************//**
** \brief     System Clock Configuration. This code was created by CubeMX and configures
**            the system clock.
** \return    none.
**
****************************************************************************************/
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /* Configure the main internal regulator output voltage. */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /* Initialize the RCC Oscillators. */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 15;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV8;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Clock configuration incorrect or hardware failure. Hang the system to prevent
     * damage.
     */
    while(1);
  }

  /* Initializes the CPU, AHB and APB busses clocks. */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  /* Set the flash latency. */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    /* Flash latency configuration incorrect or hardware failure. Hang the system to
     * prevent damage.
     */
    while(1);
  }
  /* Configure the peripheral clocks */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN | RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    /* USART2 clock configuration incorrect or hardware failure. Hang the system to
     * prevent damage.
     */
    while(1);
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

  /* SYSCFG and PWR clock enable. */
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* GPIO ports clock enable. */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral. */
  HAL_SYSCFG_StrobeDBattpinsConfig(SYSCFG_CFGR1_UCPD1_STROBE | SYSCFG_CFGR1_UCPD2_STROBE);
    
#if (BOOT_COM_RS232_ENABLE > 0)
  /* Peripheral clock enable. */
  __HAL_RCC_USART2_CLK_ENABLE();
#endif /* BOOT_COM_RS232_ENABLE > 0 */
#if (BOOT_COM_CAN_ENABLE > 0)
  /* Peripheral clock enable. */
  __HAL_RCC_FDCAN_CLK_ENABLE();
#endif /* BOOT_COM_CAN_ENABLE > 0 */

  /* SVC_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SVC_IRQn, 0, 0);
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

  /* Configure the LED GPIO pin. */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#if (BOOT_COM_RS232_ENABLE > 0)
  /* UART TX and RX GPIO pin configuration. */
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif /* BOOT_COM_RS232_ENABLE > 0 */
#if (BOOT_COM_CAN_ENABLE > 0)
  /* CAN TX and RX GPIO pin configuration. */
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF3_FDCAN1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif /* BOOT_COM_CAN_ENABLE > 0 */
} /*** end of HAL_MspInit ***/


/************************************************************************************//**
** \brief     Deinitializes the Global MSP. This function is called from HAL_DeInit()
**            function to perform system level Deinitialization (GPIOs, clock, DMA,
**            interrupt).
** \return    none.
**
****************************************************************************************/
void HAL_MspDeInit(void)
{
#if (BOOT_COM_CAN_ENABLE > 0)
  /* Reset CAN GPIO pin configuration. */
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8 | GPIO_PIN_9);
#endif /* BOOT_COM_CAN_ENABLE > 0 */
#if (BOOT_COM_RS232_ENABLE > 0)
  /* Reset UART GPIO pin configuration. */
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);
#endif /* BOOT_COM_RS232_ENABLE > 0 */
  /* Deconfigure GPIO pin for the LED. */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5);

#if (BOOT_COM_CAN_ENABLE > 0)
  /* Peripheral clock disable. */
  __HAL_RCC_FDCAN_CLK_DISABLE();
#endif /* BOOT_COM_CAN_ENABLE > 0 */
#if (BOOT_COM_RS232_ENABLE > 0)
  /* Peripheral clock disable. */
  __HAL_RCC_USART2_CLK_DISABLE();
#endif /* BOOT_COM_RS232_ENABLE > 0 */
  /* GPIO ports clock disable. */
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOA_CLK_DISABLE();
  /* SYSCFG and PWR clock disable. */
  __HAL_RCC_PWR_CLK_DISABLE();
  __HAL_RCC_SYSCFG_CLK_DISABLE();
} /*** end of HAL_MspDeInit ***/


/*********************************** end of main.c *************************************/
