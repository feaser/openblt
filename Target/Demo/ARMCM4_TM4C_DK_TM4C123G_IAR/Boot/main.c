/************************************************************************************//**
* \file         Demo/ARMCM4_TM4C_DK_TM4C123G_IAR/Boot/main.c
* \brief        Bootloader application source file.
* \ingroup      Boot_ARMCM4_TM4C_DK_TM4C123G_IAR
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2014  by Feaser    http://www.feaser.com    All rights reserved
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
#include <stdbool.h>
#include <stdint.h>
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

/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);


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
  /* initialize the bootloader */
  BootInit();

  /* start the infinite program loop */
  while (1)
  {
    /* run the bootloader task */
    BootTask();
  }
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller. 
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  /* set the clocking to run at 50MHz from the PLL */
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
  /* enable the peripheral for the LED GPIO */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
  /* configure the LED as digital output and turn off the LED */
  GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);
  GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0);
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
#if (BOOT_COM_USB_ENABLE > 0)
  /* enable the GPIO peripheral used for USB, and configure the USB pins */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
  GPIOPinTypeUSBAnalog(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  GPIOPinTypeUSBAnalog(GPIO_PORTL_BASE, GPIO_PIN_6 | GPIO_PIN_7);
  /* reset the USB peripheral */
  SysCtlPeripheralReset(SYSCTL_PERIPH_USB0);
  /* enable the USB peripheral and turn on the USB Phy clock */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
  SysCtlUSBPLLEnable();
#endif  
  /* enable the GPIO port to which the SELECT button is connected */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
  /* configure the SELECT button pin as an input with pull-up */
  GPIODirModeSet(GPIO_PORTM_BASE, GPIO_PIN_4, GPIO_DIR_MODE_IN);
  GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
} /*** end of Init ***/


/*********************************** end of main.c *************************************/
