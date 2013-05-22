/****************************************************************************************
|  Description: bootloader cpu module source file
|    File Name: cpu.c
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
* Macro definitions
****************************************************************************************/
#define CPU_USER_PROG_VECTORS_START_ADDR   ((blt_addr)0x00002000)
#define CPU_RAM_VECTORS_START_ADDR         ((blt_addr)0x40000000)
#define CPU_VECTORS_TABLE_SIZE             (64)


/****************************************************************************************
* Register definitions
****************************************************************************************/
#define MEMMAP          (*((volatile blt_int32u *) 0xE01FC040))


/****************************************************************************************
* Hook functions
****************************************************************************************/
#if (BOOT_CPU_USER_PROGRAM_START_HOOK > 0)
extern blt_bool CpuUserProgramStartHook(void);
#endif


/****************************************************************************************
* External functions
****************************************************************************************/
extern void Reset_Handler(void);                 /* reset service routine in cstart.s  */


/****************************************************************************************
** NAME:           CpuStartUserProgram
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Starts the user program, if one is present. In this case this function
**                 does not return. 
**
****************************************************************************************/
void CpuStartUserProgram(void)
{
  void (*pProgResetHandler)(void);
  
  /* check if a user program is present by verifying the checksum */
  if (NvmVerifyChecksum() == BLT_FALSE)
  {
    /* not a valid user program so it cannot be started */
    return;
  }
  #if (BOOT_CPU_USER_PROGRAM_START_HOOK > 0)
  /* invoke callback */
  if (CpuUserProgramStartHook() == BLT_FALSE)
  {
    /* callback requests the user program to not be started */
    return;
  }
  #endif
  #if (BOOT_COM_ENABLE > 0)
  /* release the communication interface */
  ComFree();
  #endif
  /* reset the timer */
  TimerReset();
  /* copy the user program's interrupt vector table to RAM */
  CpuMemCopy(CPU_RAM_VECTORS_START_ADDR, CPU_USER_PROG_VECTORS_START_ADDR, \
             CPU_VECTORS_TABLE_SIZE);
  
  /* select RAM vector table */
  MEMMAP = 0x02;
  
  /* set the address where the bootloader needs to jump to */
  pProgResetHandler = (void*)CPU_RAM_VECTORS_START_ADDR;

  /* start the user program by activating its reset interrupt service routine */
  pProgResetHandler();
} /*** end of CpuStartUserProgram ***/


/****************************************************************************************
** NAME:           CpuMemCopy
** PARAMETER:      dest destination address for the data.
**                 src  source address of the data.
**                 len  length of the data in bytes.
** RETURN VALUE:   none
** DESCRIPTION:    Copies data from the source to the destination address.
**
****************************************************************************************/
void CpuMemCopy(blt_addr dest, blt_addr src, blt_int16u len)
{
  blt_int8u *from, *to;

  /* set casted pointers */
  from = (blt_int8u *)src;
  to = (blt_int8u *)dest;

  /* copy all bytes from source address to destination address */
  while(len-- > 0)
  {
    /* store byte value from source to destination */
    *to++ = *from++;
    /* keep the watchdog happy */
    CopService();
  }
} /*** end of CpuMemCopy ***/


/****************************************************************************************
** NAME:           CpuReset
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Perform a soft reset of the microcontroller by starting from the reset
**                 ISR.
**
****************************************************************************************/
void CpuReset(void)
{
  /* perform a software reset by calling the reset ISR routine. note that this requires
   * access to the processor status registers (PSRs), which works because the entire
   * bootloader runs in supervisor mode.
   */ 
  Reset_Handler();
} /*** end of CpuReset ***/


/*********************************** end of cpu.c **************************************/
