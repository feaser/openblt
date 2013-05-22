/****************************************************************************************
|  Description: bootloader application source file
|    File Name: main.c
|
|----------------------------------------------------------------------------------------
|                          C O P Y R I G H T
|----------------------------------------------------------------------------------------
|   Copyright (c) 2012  by Feaser    http://www.feaser.com    All rights reserved
|
|----------------------------------------------------------------------------------------
|                            L I C E N S E
|----------------------------------------------------------------------------------------
| This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
| modify it under the terms of the GNU General Public License as published by the Free
| Software Foundation, either version 3 of the License, or (at your option) any later
| version.
|
| OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with OpenBLT.
| If not, see <http://www.gnu.org/licenses/>.
|
| A special exception to the GPL is included to allow you to distribute a combined work 
| that includes OpenBLT without being obliged to provide the source code for any 
| proprietary components. The exception text is included at the bottom of the license
| file <license.html>.
| 
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

/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);


/****************************************************************************************
** NAME:           main
** PARAMETER:      none
** RETURN VALUE:   program return code
** DESCRIPTION:    This is the entry point for the bootloader application and is called 
**                 by the reset interrupt vector after the C-startup routines executed.
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


/****************************************************************************************
** NAME:           Init
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Initializes the microcontroller. The interrupts are disabled, the
**                 clocks are configured and the flash wait states are configured.
**
****************************************************************************************/
static void Init(void)
{
  /* set the clocking to run at 50MHz from the PLL */
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
#if (BOOT_COM_UART_ENABLE > 0)
  #if (BOOT_COM_UART_CHANNEL_INDEX == 0)
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
