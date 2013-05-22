/****************************************************************************************
|  Description: bootloader callback source file
|    File Name: hooks.c
|
|----------------------------------------------------------------------------------------
|                          C O P Y R I G H T
|----------------------------------------------------------------------------------------
|   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
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
#include "stm32f10x.h"                           /* microcontroller registers          */


/****************************************************************************************
*   U S B   C O M M U N I C A T I O N   I N T E R F A C E   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_COM_USB_ENABLE > 0)
/****************************************************************************************
** NAME:           UsbConnect
** PARAMETER:      connect BLT_TRUE to connect and BLT_FALSE to disconnected.
** RETURN VALUE:   none
** DESCRIPTION:    Callback that gets called whenever the USB device should be connected
**                 to the USB bus. 
**
****************************************************************************************/
void UsbConnectHook(blt_bool connect)
{
  static blt_bool initialized = BLT_FALSE;

  /* the connection to the USB bus is typically controlled by software through a digital
   * output. the GPIO pin for this must be configured as such.
   */
  if (initialized == BLT_FALSE)
  {
    /* enable clock for PC11 pin peripheral (GPIOC) */
    RCC->APB2ENR |= (blt_int32u)(0x00000010);
    /* configure DIS (GPIOC11) as open drain digital output */
    /* first reset the configuration */
    GPIOC->CRH &= ~(blt_int32u)((blt_int32u)0xf << 12);
    /* CNF11[1:0] = %01 and MODE11[1:0] = %11 */
    GPIOC->CRH |= (blt_int32u)((blt_int32u)0x7 << 12);
    /* set to initialized as this part only has to be done once after reset */
    initialized = BLT_TRUE;
  }

  /* determine if the USB should be connected or disconnected */
  if (connect == BLT_TRUE)
  {
    /* the GPIO has a pull-up so to connect to the USB bus the pin needs to go low */
    GPIOC->BRR = (blt_int32u)((blt_int32u)0x1 << 11);
  }
  else
  {
    /* the GPIO has a pull-up so to disconnect to the USB bus the pin needs to go high */
    GPIOC->BSRR = (blt_int32u)((blt_int32u)0x1 << 11);
  }
} /*** end of UsbConnect ***/


/****************************************************************************************
** NAME:           UsbEnterLowPowerMode
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Callback that gets called whenever the USB host requests the device
**                 to enter a low power mode.
**
****************************************************************************************/
void UsbEnterLowPowerModeHook(void)
{
  /* support to enter a low power mode can be implemented here */
} /*** end of UsbEnterLowPowerMode ***/


/****************************************************************************************
** NAME:           UsbLeaveLowPowerMode
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Callback that gets called whenever the USB host requests the device to
**                 exit low power mode.
**
****************************************************************************************/
void UsbLeaveLowPowerModeHook(void)
{
  /* support to leave a low power mode can be implemented here */
} /*** end of UsbLeaveLowPowerMode ***/
#endif /* BOOT_COM_USB_ENABLE > 0 */


/****************************************************************************************
*   B A C K D O O R   E N T R Y   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_BACKDOOR_HOOKS_ENABLE > 0)
/****************************************************************************************
** NAME:           BackDoorInitHook
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Initializes the backdoor entry option.
**
****************************************************************************************/
void BackDoorInitHook(void)
{
  /* enable clock for PA0 pin peripheral (GPIOA) */
  RCC->APB2ENR |= (blt_int32u)(0x00000004);
  /* configure BUT (GPIOA0) as floating digital input */
  /* first reset the configuration */
  GPIOA->CRL &= ~(blt_int32u)((blt_int32u)0xf << 0);
  /* CNF0[1:0] = %01 and MODE0[1:0] = %00 */
  GPIOA->CRL |= (blt_int32u)((blt_int32u)0x4 << 0);
} /*** end of BackDoorInitHook ***/


/****************************************************************************************
** NAME:           BackDoorEntryHook
** PARAMETER:      none
** RETURN VALUE:   BLT_TRUE if the backdoor entry is requested, BLT_FALSE otherwise.
** DESCRIPTION:    Checks if a backdoor entry is requested.
**
****************************************************************************************/
blt_bool BackDoorEntryHook(void)
{
  /* button PA0 has a pullup, so will read high by default. enter backdoor only when
   * this button is pressed. this is the case when it reads low */

  if ((GPIOA->IDR & ((blt_int32u)0x01)) == 0)
  {
    return BLT_TRUE;
  }
  return BLT_FALSE;
} /*** end of BackDoorEntryHook ***/
#endif /* BOOT_BACKDOOR_HOOKS_ENABLE > 0 */


/****************************************************************************************
*   C P U   D R I V E R   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_CPU_USER_PROGRAM_START_HOOK > 0)
/****************************************************************************************
** NAME:           CpuUserProgramStartHook
** PARAMETER:      none
** RETURN VALUE:   BLT_TRUE if it is okay to start the user program, BLT_FALSE to keep
**                 keep the bootloader active.
** DESCRIPTION:    Callback that gets called when the bootloader is about to exit and
**                 hand over control to the user program. This is the last moment that
**                 some final checking can be performed and if necessary prevent the
**                 bootloader from activiting the user program.
**
****************************************************************************************/
blt_bool CpuUserProgramStartHook(void)
{
  /* okay to start the user program */
  return BLT_TRUE;
} /*** end of CpuUserProgramStartHook ***/
#endif /* BOOT_CPU_USER_PROGRAM_START_HOOK > 0 */


/****************************************************************************************
*   N O N - V O L A T I L E   M E M O R Y   D R I V E R   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_NVM_HOOKS_ENABLE > 0)
/****************************************************************************************
** NAME:           NvmInitHook
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Callback that gets called at the start of the internal NVM driver
**                 initialization routine. 
**
****************************************************************************************/
void NvmInitHook(void)
{
} /*** end of NvmInitHook ***/


/****************************************************************************************
** NAME:           NvmWriteHook
** PARAMETER:      addr start address
**                 len  length in bytes
**                 data pointer to the data buffer.
** RETURN VALUE:   BLT_NVM_OKAY if successful, BLT_NVM_NOT_IN_RANGE if the address is
**                 not within the supported memory range, or BLT_NVM_ERROR is the write
**                 operation failed.
** DESCRIPTION:    Callback that gets called at the start of the NVM driver write 
**                 routine. It allows additional memory to be operated on. If the address
**                 is not within the range of the additional memory, then 
**                 BLT_NVM_NOT_IN_RANGE must be returned to indicate that the data hasn't
**                 been written yet.
**               
**
****************************************************************************************/
blt_int8u NvmWriteHook(blt_addr addr, blt_int32u len, blt_int8u *data)
{
  return BLT_NVM_NOT_IN_RANGE;
} /*** end of NvmWriteHook ***/


/****************************************************************************************
** NAME:           NvmEraseHook
** PARAMETER:      addr start address
**                 len  length in bytes
** RETURN VALUE:   BLT_NVM_OKAY if successful, BLT_NVM_NOT_IN_RANGE if the address is
**                 not within the supported memory range, or BLT_NVM_ERROR is the erase
**                 operation failed.
** DESCRIPTION:    Callback that gets called at the start of the NVM driver erase 
**                 routine. It allows additional memory to be operated on. If the address
**                 is not within the range of the additional memory, then
**                 BLT_NVM_NOT_IN_RANGE must be returned to indicate that the memory
**                 hasn't been erased yet.
**
****************************************************************************************/
blt_int8u NvmEraseHook(blt_addr addr, blt_int32u len)
{
  return BLT_NVM_NOT_IN_RANGE;
} /*** end of NvmEraseHook ***/


/****************************************************************************************
** NAME:           NvmDoneHook
** PARAMETER:      none
** RETURN VALUE:   BLT_TRUE is successful, BLT_FALSE otherwise.
** DESCRIPTION:    Callback that gets called at the end of the NVM programming session.
**
****************************************************************************************/
blt_bool NvmDoneHook(void)
{
  return BLT_TRUE;
} /*** end of NvmDoneHook ***/
#endif /* BOOT_NVM_HOOKS_ENABLE > 0 */


/****************************************************************************************
*   W A T C H D O G   D R I V E R   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_COP_HOOKS_ENABLE > 0)
/****************************************************************************************
** NAME:           CopInitHook
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Callback that gets called at the end of the internal COP driver
**                 initialization routine. It can be used to configure and enable the
**                 watchdog.
**
****************************************************************************************/
void CopInitHook(void)
{
} /*** end of CopInitHook ***/


/****************************************************************************************
** NAME:           CopServiceHook
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Callback that gets called at the end of the internal COP driver
**                 service routine. This gets called upon initialization and during
**                 potential long lasting loops and routine. It can be used to service
**                 the watchdog to prevent a watchdog reset.
**
****************************************************************************************/
void CopServiceHook(void)
{
} /*** end of CopServiceHook ***/
#endif /* BOOT_COP_HOOKS_ENABLE > 0 */


/*********************************** end of hooks.c ************************************/
