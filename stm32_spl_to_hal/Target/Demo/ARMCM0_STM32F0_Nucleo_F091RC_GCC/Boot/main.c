/************************************************************************************//**
* \file         Demo\ARMCM0_STM32F0_Nucleo_F091RC_GCC\Boot\main.c
* \brief        Bootloader application source file.
* \ingroup      Boot_ARMCM0_STM32F0_Nucleo_F091RC_GCC
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2016  by Feaser    http://www.feaser.com    All rights reserved
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
#include "stm32f0xx.h"                           /* STM32 registers and drivers        */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);


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
  GPIO_InitTypeDef GPIO_InitStructure;

  /* enable the GPIO_LED Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA, GPIO_Pin_5);

  /* configure the pushbutton pin as input for backdoor entry */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

#if (BOOT_COM_UART_ENABLE > 0)
  /* enable the USART2 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
  /* enable the USART2 Tx/Rx and pushbutton GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* configure alternate function for the USART2 Tx/Rx pins */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
  /* Configure USART2 pins: Rx (PA2) and Tx (PA3) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif

#if (BOOT_COM_CAN_ENABLE > 0)
  /* enable CAN1 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);
  /* enable clocks for CAN1 transmitter and receiver pins */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  /* select alternate function for the CAN1 pins */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_4);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_4);
  /* configure CAN1 RX and TX pins */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
} /*** end of Init ***/


/*********************************** end of main.c *************************************/
