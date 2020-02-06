/************************************************************************************//**
* \file         Demo/ARMCM4_XMC4_XMC4700_Relax_Kit_GCC/Boot/main.c
* \brief        Bootloader application source file.
* \ingroup      Boot_ARMCM4_XMC4_XMC4700_Relax_Kit_GCC
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
#include "xmc_gpio.h"                            /* GPIO module                        */
#include "xmc_uart.h"                            /* UART driver header                 */
#include "xmc_can.h"                             /* CAN driver header                  */
#include "shared_params.h"                       /* Shared parameters header           */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);
static void PostInit(void);


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called
**            by the reset interrupt vector after the C-startup routines executed.
** \return    Program return code.
**
****************************************************************************************/
int main(void)
{
  blt_int8u deferredInitRequestFlag = 0;

  /* initialize the microcontroller */
  Init();
  /* initialize the shared parameters module */
  SharedParamsInit();
  /* initialize the bootloader */
  BootInit();
#if (BOOT_COM_DEFERRED_INIT_ENABLE == 1)
  /* the bootloader is configured to NOT initialize the TCP/IP network stack by default
   * to bypass unnecessary delay times before starting the user program. the TCP/IP net-
   * work tack is now only initialized when: (a) no valid user program is detected, or
   * (b) a forced backdoor entry occurred (CpuUserProgramStartHook() returned BLT_FALSE).
   *
   * these demo bootloader and user programs have one extra feature implemented for
   * demonstration purposes. the demo user program can detect firmware update requests
   * from the TCP/IP network in which case it activates the bootloader. But...the
   * TCP/IP network stack will not be initialized in this situation. for this reason
   * the shared parameter module was integrated in both the bootloader and user program.
   * more information about the shared parameter module can be found here:
   *   https://www.feaser.com/en/blog/?p=216
   *
   * the shared parameter at the first index (0) contains a flag. this flag is set to
   * 1, right before the user program activates this bootloader, to explicitly request
   * the bootloader to initialize the TCP/IP network stack. this makes it possible for
   * a firmware update to proceed. the code here reads out this flag and performs the
   * TCP/IP network stack initialization when requested.
   */
  SharedParamsReadByIndex(0, &deferredInitRequestFlag);
  if (deferredInitRequestFlag == 1)
  {
    /* explicitly initialize all communication interface for which the deferred
     * initialization feature was enabled.
     */
    ComDeferredInit();
  }
#endif
  
  /* post initialization of the microcontroller */
  PostInit();

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
#if (BOOT_FILE_LOGGING_ENABLE > 0) && (BOOT_COM_RS232_ENABLE == 0)
  XMC_UART_CH_CONFIG_t rs232_config;
#endif

  /* initialize LED1 on P5.9 as digital output */
  XMC_GPIO_SetMode(P5_9, XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
  /* initialize BUTTON1 as digital input. */
  XMC_GPIO_SetMode(P15_13, XMC_GPIO_MODE_INPUT_TRISTATE);
  XMC_GPIO_EnableDigitalInput(P15_13);

#if (BOOT_FILE_LOGGING_ENABLE > 0) && (BOOT_COM_RS232_ENABLE == 0)
  /* set configuration and initialize UART channel */
  rs232_config.baudrate = BOOT_COM_RS232_BAUDRATE;
  rs232_config.data_bits = 8;
  rs232_config.frame_length = 8;
  rs232_config.stop_bits = 1;
  rs232_config.oversampling = 16;
  rs232_config.parity_mode = XMC_USIC_CH_PARITY_MODE_NONE;
  XMC_UART_CH_Init(XMC_UART0_CH0, &rs232_config);
  /* configure small transmit and receive FIFO */
  XMC_USIC_CH_TXFIFO_Configure(XMC_UART0_CH0, 16U, XMC_USIC_CH_FIFO_SIZE_16WORDS, 1U);
  XMC_USIC_CH_RXFIFO_Configure(XMC_UART0_CH0,  0U, XMC_USIC_CH_FIFO_SIZE_16WORDS, 1U);
  /* start UART */
  XMC_UART_CH_Start(XMC_UART0_CH0);
#endif
} /*** end of Init ***/


/************************************************************************************//**
** \brief     Post initialization of the microcontroller. Contains all initialization
**            code that should run after the bootloader's core was initialized.
** \return    none.
**
****************************************************************************************/
static void PostInit(void)
{
#if (BOOT_COM_RS232_ENABLE > 0) || (BOOT_FILE_LOGGING_ENABLE > 0)
  XMC_GPIO_CONFIG_t rx_rs232_config;
  XMC_GPIO_CONFIG_t tx_rs232_config;
#endif
#if (BOOT_COM_CAN_ENABLE > 0)
  XMC_GPIO_CONFIG_t rx_can_config;
  XMC_GPIO_CONFIG_t tx_can_config;
#endif

#if (BOOT_COM_RS232_ENABLE > 0) || (BOOT_FILE_LOGGING_ENABLE > 0)
  /* initialize UART Rx pin */
  rx_rs232_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
  rx_rs232_config.output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH;
  rx_rs232_config.output_strength  = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
  XMC_GPIO_Init(P1_4, &rx_rs232_config);
  /* initialize UART Tx pin */
  tx_rs232_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2;
  tx_rs232_config.output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH;
  tx_rs232_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
  XMC_GPIO_Init(P1_5, &tx_rs232_config);
  /* set input source path to DXnB to connect P1_4 to ASC Receive. note that this
   * function must be called after XMC_UART_CH_Init(), which is called when initializing
   * the bootloader core with BootInit().
  */
  XMC_USIC_CH_SetInputSource(XMC_UART0_CH0, XMC_USIC_CH_INPUT_DX0, 1U);
#endif

#if (BOOT_COM_CAN_ENABLE > 0)
  /* configure CAN receive pin */
  rx_can_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
  XMC_GPIO_Init(P1_13, &rx_can_config);
  /* configure CAN transmit pin */
  tx_can_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2;
  tx_can_config.output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH;
  tx_can_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE;
  XMC_GPIO_Init(P1_12, &tx_can_config);
  /* select CAN Receive Input C (N1_RXDC) to map P1_13 to CAN_NODE1 */
  XMC_CAN_NODE_EnableConfigurationChange(CAN_NODE1);
  XMC_CAN_NODE_SetReceiveInput(CAN_NODE1, XMC_CAN_NODE_RECEIVE_INPUT_RXDCC);
  XMC_CAN_NODE_DisableConfigurationChange(CAN_NODE1);
#endif
} /*** end of PostInit ***/

/*********************************** end of main.c *************************************/
