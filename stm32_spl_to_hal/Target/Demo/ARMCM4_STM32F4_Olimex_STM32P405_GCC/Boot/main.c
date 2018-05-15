/************************************************************************************//**
* \file         Demo\ARMCM4_STM32F4_Olimex_STM32P405_GCC\Boot\main.c
* \brief        Bootloader application source file.
* \ingroup      Boot_ARMCM4_STM32F4_Olimex_STM32P405_GCC
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
#include "stm32f4xx.h"                           /* STM32 registers                    */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);

/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called
**            by the reset interrupt vector after the C-startup routines executed.
** \return    Program exit code.
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
  return 0;
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
#if (BOOT_COM_UART_ENABLE > 0)
#elif (BOOT_FILE_SYS_ENABLE > 0)
  USART_InitTypeDef USART_InitStructure;
#endif

  /* initialize the button as a digital input. is used to override the starting of
   * the user program.
   */
  /* enable the GPIO Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* configure the GPIO pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

#if (BOOT_COM_UART_ENABLE > 0)
  /* enable UART peripheral clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  /* enable GPIO peripheral clock for transmitter and receiver pins */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* connect the pin to the peripherals alternate function */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
  /* configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* configure USART Rx as alternate function */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
#elif (BOOT_FILE_SYS_ENABLE > 0)
  /* enable UART peripheral clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  /* enable GPIO peripheral clock for transmitter and receiver pins */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* connect the pin to the peripherals alternate function */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
  /* configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* configure USART Rx as alternate function */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* initialize the uart for the specified communication speed */
  USART_InitStructure.USART_BaudRate = BOOT_COM_UART_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);
  /* enable UART */
  USART_Cmd(USART2, ENABLE);
#endif
#if (BOOT_COM_CAN_ENABLE > 0)
  /* enable clocks for CAN1 transmitter and receiver pins */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  /* select alternate function for the CAN1 pins */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_CAN1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_CAN1);
  /* configure CAN1 RX and TX pins */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* enable CAN1 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
#endif
  /* enable the GPIO_LED Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  /* configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_SetBits(GPIOC, GPIO_Pin_12);
} /*** end of Init ***/


/*********************************** end of main.c *************************************/
