/************************************************************************************//**
* \file         Demo/ARMCM3_LM3S_EK_LM3S6965_Crossworks/Boot/main.c
* \brief        Bootloader application source file.
* \ingroup      Boot_ARMCM3_LM3S_EK_LM3S6965_Crossworks
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2012  by Feaser    http://www.feaser.com    All rights reserved
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
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#if (BOOT_FILE_LOGGING_ENABLE > 0)
#include "driverlib/uartlib.h"
#endif
#include "shared_params.h"                       /* Shared parameters header           */

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
  /* set the clocking to run at 50MHz from the PLL */
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
  
  /* initialize the status button as a digital input. it is used to override the
   * starting of the user program.
   */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_DIR_MODE_IN);
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  
#if (BOOT_COM_RS232_ENABLE > 0)
  #if (BOOT_COM_RS232_CHANNEL_INDEX == 0)
  /* enable and configure UART0 related peripherals and pins */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  #endif
#elif (BOOT_FILE_LOGGING_ENABLE > 0)
  /* log info strings to UART during firmware updates from local file storage */
  /* enable and configure UART0 related peripherals and pins */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  /* enable the UART0 peripheral */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  /* configure the UART0 baudrate and communication parameters */
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 57600,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | 
                      UART_CONFIG_PAR_NONE));
#endif
} /*** end of Init ***/


/*********************************** end of main.c *************************************/
