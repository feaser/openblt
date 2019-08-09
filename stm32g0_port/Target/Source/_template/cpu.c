/************************************************************************************//**
* \file         Source/_template/cpu.c
* \brief        Bootloader cpu module source file.
* \ingroup      Target__template_cpu
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019  by Feaser    http://www.feaser.com    All rights reserved
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

/************************************************************************************//**
* \defgroup   Target__template_cpu CPU driver of a port
* \brief      This module implements the CPU driver of a microcontroller port. 
* \ingroup    Target__template
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Pointer to the user program's vector table. */
#define CPU_USER_PROGRAM_VECTABLE_OFFSET  ((blt_addr)NvmGetUserProgBaseAddress())


/****************************************************************************************
* Hook functions
****************************************************************************************/
#if (BOOT_CPU_USER_PROGRAM_START_HOOK > 0)
extern blt_bool CpuUserProgramStartHook(void);
#endif


/************************************************************************************//**
** \brief     Initializes the CPU module.
** \return    none.
**
****************************************************************************************/
void CpuInit(void)
{
  /* bootloader runs in polling mode so disable the global interrupts. this is done for
   * safety reasons. if the bootloader was started from a running user program, it could 
   * be that the user program did not properly disable the interrupt generation of 
   * peripherals. */
  CpuIrqDisable();
} /*** end of CpuInit ***/


/************************************************************************************//**
** \brief     Starts the user program, if one is present. In this case this function
**            does not return.
** \return    none.
**
****************************************************************************************/
void CpuStartUserProgram(void)
{
  void (*pProgResetHandler)(void);

  /* check if a user program is present by verifying the checksum */
  if (NvmVerifyChecksum() == BLT_FALSE)
  {
#if (BOOT_COM_DEFERRED_INIT_ENABLE > 0) && (BOOT_COM_ENABLE > 0)
    /* bootloader will stay active so perform deferred initialization to make sure
     * the communication interface that were not yet initialized are now initialized.
     * this is needed to make sure firmware updates via these communication interfaces
     * will be possible.
     */
    ComDeferredInit();
#endif
    /* not a valid user program so it cannot be started */
    return;
  }
#if (BOOT_CPU_USER_PROGRAM_START_HOOK > 0)
  /* invoke callback */
  if (CpuUserProgramStartHook() == BLT_FALSE)
  {
  #if (BOOT_COM_DEFERRED_INIT_ENABLE > 0) && (BOOT_COM_ENABLE > 0)
    /* bootloader will stay active so perform deferred initialization to make sure
     * the communication interface that were not yet initialized are now initialized.
     * this is needed to make sure firmware updates via these communication interfaces
     * will be possible.
     */
    ComDeferredInit();
  #endif
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
  
  /* TODO ##Port Prepare to start the user program. This typically consists of remapping
   * the base address of the vector table, since the user program is typically moved
   * forward to make space for the bootloader itself. 
   * Some microcontrollers to not support changing the base address of the vector 
   * table. In this the bootloader would need to reroute all interrupt vectors, except 
   * the reset vector, to the location in memory where the user program has its vector
   * table. This was done in the HCS12 port.
   * If the microcontroller does not support remapping the vector table base address in
   * flash, it might support remapping it to RAM. In this case you would not only need
   * to do the remapping, but also copy the user program's vector table to this area
   * in RAM. This was done in the STM32F0 port.
   */

  /* TODO ##Port Enable the global interrupts by calling function CpuIrqEnable(). Note
   * that this should only be done if the microcontroller normally has global interrupts
   * enabled after a reset event. Otherwise, you can skip this part.
   */
  CpuIrqEnable();

  /* TODO ##Port Start the user program. This is achieved by reading out the address
   * of the user program's reset handler from its vector table and jumping to it.
   * The following example implementation shows how this is done in case the reset
   * handler is located in the first entry of the interrupt vector table and the
   * interrupt vector table is at the start of the user program.
   * Note that for a lot of ARM Cortex CPUs, the first entry is the stackpointer and the
   * second entry is the reset handler. In this case an extra 4 bytes need to be added
   * to get to the address of where the reset handler pointer is located. In this case
   * the user program should also explicitly initialize the stackpointer as the first
   * thing in the reset handler.
   */
   
  /* set the address where the bootloader needs to jump to. this is the address of
   * the 1st entry in the user program's vector table. this address points to the
   * user program's reset handler.
   */
  pProgResetHandler = (void(*)(void))(*((blt_addr *)NvmGetUserProgBaseAddress()));
  /* start the user program by calling its reset interrupt service routine */
  pProgResetHandler();
#if (BOOT_COM_DEFERRED_INIT_ENABLE > 0) && (BOOT_COM_ENABLE > 0)
  /* theoretically, the code never gets here because the user program should now be
   * running and the previous function call should not return. In case it did return
   * for whatever reason, make sure all communication interfaces are initialized so that
   * firmware updates can be started.
   */
  ComDeferredInit();
#endif
} /*** end of CpuStartUserProgram ***/


/************************************************************************************//**
** \brief     Copies data from the source to the destination address.
** \param     dest Destination address for the data.
** \param     src  Source address of the data.
** \param     len  length of the data in bytes.
** \return    none.
**
****************************************************************************************/
void CpuMemCopy(blt_addr dest, blt_addr src, blt_int16u len)
{
  blt_int8u *from, *to;

  /* TODO ##Port Implements similar functionality as the C library's memcpy() function.
   * For most ports you can simply leave this function as is. If desired you can optimize
   * the implementation, for example by copying 32-bits at a time for 32-bit CPU
   * architectures. Alternativly, you could just use memcpy().
   */

  /* set casted pointers */
  from = (blt_int8u *)src;
  to = (blt_int8u *)dest;

  /* copy all bytes from source address to destination address */
  while (len-- > 0)
  {
    /* store byte value from source to destination */
    *to++ = *from++;
    /* keep the watchdog happy */
    CopService();
  }
} /*** end of CpuMemCopy ***/


/************************************************************************************//**
** \brief     Sets the bytes at the destination address to the specified value.
** \param     dest Destination address for the data.
** \param     value Value to write.
** \param     len  Number of bytes to write.
** \return    none.
**
****************************************************************************************/
void CpuMemSet(blt_addr dest, blt_int8u value, blt_int16u len)
{
  blt_int8u *to;

  /* TODO ##Port Implements similar functionality as the C library's memset() function.
   * For most ports you can simply leave this function as is. If desired you can optimize
   * the implementation, for example by setting 32-bits at a time for 32-bit CPU
   * architectures. Alternativly, you could just use memset().
   */

  /* set casted pointer */
  to = (blt_int8u *)dest;

  /* set all bytes at the destination address to the specified value */
  while (len-- > 0)
  {
    /* set byte value */
    *to++ = value;
    /* keep the watchdog happy */
    CopService();
  }
} /*** end of CpuMemSet ***/


/*********************************** end of cpu.c **************************************/
