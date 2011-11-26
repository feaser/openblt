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
  /* configure the button connected to P0.16 as a digital input */
  IO0DIR &= ~(1<<16);
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
  /* button P0.16 has a pullup, so will read high by default. enter backdoor only when
   * this button is pressed. this is the case when it reads low */
  if ((IO0PIN & (1<<16)) == 0)
  {
    return BLT_TRUE;
  }
  return BLT_FALSE;
} /*** end of BackDoorEntryHook ***/
#endif /* BOOT_BACKDOOR_HOOKS_ENABLE > 0 */


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
** RETURN VALUE:   BTL_NVM_OKAY if successful, BTL_NVM_NOT_IN_RANGE if the address is
**                 not within the supported memory range, or BTL_NVM_ERROR is the write
**                 operation failed.
** DESCRIPTION:    Callback that gets called at the start of the NVM driver write 
**                 routine. It allows additional memory to be operated on. If the address
**                 is not within the range of the additional memory, then 
**                 BTL_NVM_NOT_IN_RANGE must be returned to indicate that the data hasn't
**                 been written yet.
**               
**
****************************************************************************************/
blt_int8u NvmWriteHook(blt_addr addr, blt_int32u len, blt_int8u *data)
{
  return BTL_NVM_NOT_IN_RANGE;
} /*** end of NvmWriteHook ***/


/****************************************************************************************
** NAME:           NvmEraseHook
** PARAMETER:      addr start address
**                 len  length in bytes
** RETURN VALUE:   BTL_NVM_OKAY if successful, BTL_NVM_NOT_IN_RANGE if the address is
**                 not within the supported memory range, or BTL_NVM_ERROR is the erase
**                 operation failed.
** DESCRIPTION:    Callback that gets called at the start of the NVM driver erase 
**                 routine. It allows additional memory to be operated on. If the address
**                 is not within the range of the additional memory, then
**                 BTL_NVM_NOT_IN_RANGE must be returned to indicate that the memory
**                 hasn't been erased yet.
**
****************************************************************************************/
blt_bool NvmEraseHook(blt_addr addr, blt_int32u len)
{
  return BTL_NVM_NOT_IN_RANGE;
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
