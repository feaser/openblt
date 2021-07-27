/************************************************************************************//**
* \file         Demo/ARMCM0_XMC1_XMC1400_Boot_Kit_GCC/Boot/main.c
* \brief        Bootloader application source file.
* \ingroup      Boot_ARMCM0_XMC1_XMC1400_Boot_Kit_GCC
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
#include "xmc_gpio.h"                            /* GPIO module                        */
#include "xmc_uart.h"                            /* UART driver header                 */
#include "xmc_can.h"                             /* CAN driver header                  */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void PostInit(void);


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called
**            by the reset interrupt vector after the C-startup routines executed.
** \return    Program exit code.
**
****************************************************************************************/
int main(void)
{
  /* initialize the bootloader */
  BootInit();
  /* post initialization of the microcontroller */
  PostInit();
  
  /* start the infinite program loop */
  while (1)
  {
    /* run the bootloader task */
    BootTask();
  }
  return 0;
} /*** end of main ***/


/************************************************************************************//**
** \brief     Post initialization of the microcontroller. Contains all initialization
**            code that should run after the bootloader's core was initialized.
** \return    none.
**
****************************************************************************************/
static void PostInit(void)
{
#if (BOOT_COM_RS232_ENABLE > 0)
  XMC_GPIO_CONFIG_t rx_rs232_config;
  XMC_GPIO_CONFIG_t tx_rs232_config;
#endif
#if (BOOT_COM_CAN_ENABLE > 0)
  XMC_GPIO_CONFIG_t rx_can_config;
  XMC_GPIO_CONFIG_t tx_can_config;
#endif
  
#if (BOOT_COM_RS232_ENABLE > 0)
  /* initialize UART Rx pin */
  rx_rs232_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
  XMC_GPIO_Init(P1_3, &rx_rs232_config);
  /* initialize UART Tx pin */
  tx_rs232_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT7;
  XMC_GPIO_Init(P1_2, &tx_rs232_config);
  /* set input source path to DXnA to connect P1_3 to ASC Receive. note that this
   * function must be called after XMC_UART_CH_Init(), which is called when initializing
   * the bootloader core with BootInit().
  */
  XMC_UART_CH_SetInputSource(XMC_UART0_CH1, XMC_UART_CH_INPUT_RXD, USIC0_C1_DX0_P1_3);
#endif
  
#if (BOOT_COM_CAN_ENABLE > 0)
  /* configure CAN receive pin */
  rx_can_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
  XMC_GPIO_Init(P4_8, &rx_can_config);
  /* configure CAN transmit pin */
  tx_can_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT9;
  XMC_GPIO_Init(P4_9, &tx_can_config);
  /* select CAN Receive Input C (N1_RXDC) to map P4_8 to CAN_NODE1 */
  XMC_CAN_NODE_EnableConfigurationChange(CAN_NODE1);
  XMC_CAN_NODE_SetReceiveInput(CAN_NODE1, XMC_CAN_NODE_RECEIVE_INPUT_RXDCC);
  XMC_CAN_NODE_DisableConfigurationChange(CAN_NODE1);
#endif
} /*** end of PostInit ***/


/*********************************** end of main.c *************************************/
