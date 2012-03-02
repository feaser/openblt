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
#include "efm32.h"                                    /* EFM32 registers               */
#include "efm32_chip.h"                               /* EFM32 chip initialization     */
#include "efm32_cmu.h"                                /* EFM32 clock management        */
#include "efm32_gpio.h"


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
  /* initialize the system and its clocks */
  SystemInit();
  /* handle chip errate workarounds */
  CHIP_Init();
  /* enable the low frequency crystal oscillator */
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
  /* turn on clocking of all the modules */
  CMU->HFCORECLKEN0 |= 0x0F;
  CMU->HFPERCLKEN0 |= 0xFFFF;
  /* disable clocking of the modules that are not in use */
  CMU_ClockEnable(cmuClock_AES | cmuClock_DMA | cmuClock_EBI | cmuClock_PRS, false);
  CMU_ClockEnable(cmuClock_USART0 | cmuClock_USART1 | cmuClock_USART2, false);
  CMU_ClockEnable(cmuClock_UART0 | cmuClock_ACMP0 | cmuClock_ACMP1, false);
  CMU_ClockEnable(cmuClock_DAC0 | cmuClock_ADC0 | cmuClock_I2C0 | cmuClock_VCMP, false);
#if (BOOT_COM_UART_ENABLE > 0)
  /* enable power to U2 (RS232_PWR_E) */
  GPIO_PinModeSet(gpioPortB, 9, gpioModePushPullDrive, 1);
  /* set port B outputs to drive up to 20 mA */
  GPIO_DriveModeSet(gpioPortB, gpioDriveModeHigh);
#endif
} /*** end of Init ***/


/*********************************** end of main.c *************************************/
