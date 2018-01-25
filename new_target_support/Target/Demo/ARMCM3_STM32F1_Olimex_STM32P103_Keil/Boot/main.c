/************************************************************************************//**
* \file         Demo\ARMCM3_STM32F1_Olimex_STM32P103_Keil\Boot\main.c
* \brief        Bootloader application source file.
* \ingroup      Boot_ARMCM3_STM32F1_Olimex_STM32P103_Keil
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
#include "stm32f10x.h"                           /* microcontroller registers          */
#if (BOOT_FILE_LOGGING_ENABLE > 0) && (BOOT_COM_UART_ENABLE == 0)
#include "stm32f10x_conf.h"                      /* STM32 peripheral drivers           */
#endif


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
	/* code should never get here */
	return 0;
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller. 
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
#if (BOOT_FILE_LOGGING_ENABLE > 0) 
  USART_InitTypeDef USART_InitStruct;  
#endif

#if (BOOT_COM_CAN_ENABLE > 0)
  /* GPIO clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  /* Configure CAN pin: RX */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  /* Configure CAN pin: TX */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  /* Remap CAN1 pins to PortB */
  GPIO_PinRemapConfig(GPIO_Remap1_CAN1 , ENABLE);
  /* CAN1 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
#endif
#if (BOOT_COM_UART_ENABLE > 0) || (BOOT_FILE_LOGGING_ENABLE > 0)
  /* enable UART peripheral clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  /* enable GPIO peripheral clock for transmitter and receiver pins */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  /* configure USART Tx as alternate function push-pull */
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  /* Configure USART Rx as alternate function input floating */
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  #if (BOOT_FILE_LOGGING_ENABLE > 0)
  /* configure UART communcation parameters */  
  USART_InitStruct.USART_BaudRate = BOOT_COM_UART_BAUDRATE;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStruct);
  /* enable UART */
  USART_Cmd(USART2, ENABLE);
	#endif
#endif
} /*** end of Init ***/


/*********************************** end of main.c *************************************/
