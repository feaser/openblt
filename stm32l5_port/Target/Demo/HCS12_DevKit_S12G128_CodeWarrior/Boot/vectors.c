/************************************************************************************//**
* \file         Demo/HCS12_DevKit_S12G128_CodeWarrior/Boot/vectors.c
* \brief        Demo program interrupt vectors source file.
* \ingroup      Boot_HCS12_DevKit_S12G128_CodeWarrior
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include <hidef.h>                                   /* common defines and macros      */
#include "boot.h"                                    /* bootloader generic header      */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Start address of the user program's vector table.
 *  \attention This value must be updated if the memory reserved for the bootloader
 *             changes.
 */
#define VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR  (0xE780)


/****************************************************************************************
* External functions
****************************************************************************************/
extern void near _Startup(void);


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Type for vector table entries. */
typedef void (*near tIsrFunc)(void);


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector0_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (0 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector0_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector1_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (1 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector1_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector2_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (2 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector2_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector3_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (3 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector3_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector4_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (4 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector4_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector5_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (5 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector5_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector6_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (6 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector6_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector7_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (7 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector7_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector8_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (8 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector8_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector9_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (9 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector9_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector10_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (10 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector10_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector11_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (11 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector11_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector12_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (12 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector12_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector13_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (13 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector13_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector14_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (14 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector14_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector15_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (15 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector15_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector16_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (16 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector16_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector17_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (17 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector17_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector18_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (18 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector18_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector19_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (19 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector19_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector20_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (20 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector20_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector21_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (21 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector21_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector22_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (22 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector22_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector23_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (23 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector23_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector24_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (24 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector24_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector25_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (25 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector25_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector26_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (26 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector26_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector27_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (27 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector27_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector28_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (28 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector28_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector29_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (29 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector29_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector30_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (30 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector30_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector31_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (31 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector31_handler ***/



/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector32_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (32 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector32_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector33_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (33 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector33_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector34_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (34 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector34_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector35_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (35 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector35_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector36_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (36 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector36_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector37_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (37 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector37_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector38_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (38 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector38_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector39_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (39 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector39_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector40_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (40 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector40_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector41_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (41 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector41_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector42_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (42 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector42_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector43_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (43 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector43_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector44_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (44 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector44_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector45_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (45 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector45_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector46_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (46 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector46_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector47_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (47 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector47_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector48_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (48 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector48_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector49_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (49 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector49_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector50_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (50 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector50_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector51_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (51 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector51_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector52_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (52 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector52_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector53_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (53 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector53_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector54_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (54 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector54_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector55_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (55 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector55_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector56_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (56 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector56_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector57_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (57 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector57_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector58_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (58 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector58_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector59_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (59 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector59_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector60_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (60 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector60_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector61_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (61 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector61_handler ***/


/************************************************************************************//**
** \brief     ISR handler for a specific vector index in the interrupt vector table for
**            linking the actual interrupts vectors to the one in the user program's
**            vector table.
** \return    none.
**
****************************************************************************************/
void Vector62_handler(void)
{
  /* Note that this function is called upon interrupt and therefore all CPU registers
   * (excluding the SP) are already backed up on the stack, so we are free to use them.
   */
  asm
  {
    /* Load the address of the user program's ISR into X. */
    LDX(VCT_USER_PROGRAM_VECTOR_TABLE_STARTADDR + (62 * 2))
    /* Jump there. It is important to use the JMP instruction here as opposed to other
     * branch instruction, because the JMP instruction does not modify the stack by
     * saving a return address for example.
     */
    JMP 0,X
  }
} /*** end of Vector62_handler ***/


/****************************************************************************************
*                I N T E R R U P T     V E C T O R     T A B L E
****************************************************************************************/
/** \brief The interrupt vector table. It contains the reset vector and all other
 *         interrupts vectors are remapped to the user program vector table.
 */
const tIsrFunc _vectab[] @0xff80 =
{
  (tIsrFunc)Vector0_handler,                          /* Reserved 0xFF80               */
  (tIsrFunc)Vector1_handler,                          /* Reserved 0xFF82               */
  (tIsrFunc)Vector2_handler,                          /* Reserved 0xFF84               */
  (tIsrFunc)Vector3_handler,                          /* Reserved 0xFF86               */
  (tIsrFunc)Vector4_handler,                          /* Reserved 0xFF88               */
  (tIsrFunc)Vector5_handler,                          /* Reserved 0xFF8A               */
  (tIsrFunc)Vector6_handler,                          /* PWM Emergency Shutdown 0xFF8C */
  (tIsrFunc)Vector7_handler,                          /* PortP Interrupt 0xFF8E        */
  (tIsrFunc)Vector8_handler,                          /* MSCAN4 Transmit 0xFF90        */
  (tIsrFunc)Vector9_handler,                          /* MSCAN4 Receive 0xFF92         */
  (tIsrFunc)Vector10_handler,                         /* MSCAN4 Errors 0xFF94          */
  (tIsrFunc)Vector11_handler,                         /* MSCAN4 WakeUp 0xFF96          */
  (tIsrFunc)Vector12_handler,                         /* MSCAN3 Transmit 0xFF98        */
  (tIsrFunc)Vector13_handler,                         /* MSCAN3 Receive 0xFF9A         */
  (tIsrFunc)Vector14_handler,                         /* MSCAN3 Errors 0xFF9C          */
  (tIsrFunc)Vector15_handler,                         /* MSCAN3 WakeUp 0xFF9E          */
  (tIsrFunc)Vector16_handler,                         /* MSCAN2 Transmit 0xFFA0        */
  (tIsrFunc)Vector17_handler,                         /* MSCAN2 Receive 0xFFA2         */
  (tIsrFunc)Vector18_handler,                         /* MSCAN2 Errors 0xFFA4          */
  (tIsrFunc)Vector19_handler,                         /* MSCAN2 WakeUp 0xFFA6          */
  (tIsrFunc)Vector20_handler,                         /* MSCAN1 Transmit 0xFFA8        */
  (tIsrFunc)Vector21_handler,                         /* MSCAN1 Receive 0xFFAA         */
  (tIsrFunc)Vector22_handler,                         /* MSCAN1 Errors 0xFFAC          */
  (tIsrFunc)Vector23_handler,                         /* MSCAN1 WakeUp 0xFFAE          */
  (tIsrFunc)Vector24_handler,                         /* MSCAN0 Transmit 0xFFB0        */
  (tIsrFunc)Vector25_handler,                         /* MSCAN0 Receive 0xFFB2         */
  (tIsrFunc)Vector26_handler,                         /* MSCAN0 Errors 0xFFB4          */
  (tIsrFunc)Vector27_handler,                         /* MSCAN0 WakeUp 0xFFB6          */
  (tIsrFunc)Vector28_handler,                         /* Flash 0xFFB8                  */
  (tIsrFunc)Vector29_handler,                         /* Eeprom WakeUp 0xFFBA          */
  (tIsrFunc)Vector30_handler,                         /* SPI2  0xFFBC                  */
  (tIsrFunc)Vector31_handler,                         /* SPI1  0xFFBE                  */
  (tIsrFunc)Vector32_handler,                         /* IIC Bus 0xFFC0                */
  (tIsrFunc)Vector33_handler,                         /* DLC 0xFFC2                    */
  (tIsrFunc)Vector34_handler,                         /* SCME 0xFFC4                   */
  (tIsrFunc)Vector35_handler,                         /* CRG Lock 0xFFC6               */
  (tIsrFunc)Vector36_handler,                         /* Pulse AccB Overflow 0xFFC8    */
  (tIsrFunc)Vector37_handler,                         /* Mod Down Cnt Underflow 0xFFCA */
  (tIsrFunc)Vector38_handler,                         /* PortH Interrupt 0xFFCC        */
  (tIsrFunc)Vector39_handler,                         /* PortJ Interrupt 0xFFCE        */
  (tIsrFunc)Vector40_handler,                         /* ATD1 0xFFD0                   */
  (tIsrFunc)Vector41_handler,                         /* ATD0 0xFFD2                   */
  (tIsrFunc)Vector42_handler,                         /* SCI1 0xFFD4                   */
  (tIsrFunc)Vector43_handler,                         /* SCI0 0xFFD6                   */
  (tIsrFunc)Vector44_handler,                         /* SPI0 0xFFD8                   */
  (tIsrFunc)Vector45_handler,                         /* Pulse AccA Input Edge 0xFFDA  */
  (tIsrFunc)Vector46_handler,                         /* Pulse AccA Overflow 0xFFDC    */
  (tIsrFunc)Vector47_handler,                         /* Timer Overflow 0xFFDE         */
  (tIsrFunc)Vector48_handler,                         /* Timer 7 0xFFE0                */
  (tIsrFunc)Vector49_handler,                         /* Timer 6 0xFFE2                */
  (tIsrFunc)Vector50_handler,                         /* Timer 5 0xFFE4                */
  (tIsrFunc)Vector51_handler,                         /* Timer 4 0xFFE6                */
  (tIsrFunc)Vector52_handler,                         /* Timer 3 0xFFE8                */
  (tIsrFunc)Vector53_handler,                         /* Timer 2 0xFFEA                */
  (tIsrFunc)Vector54_handler,                         /* Timer 1 0xFFEC                */
  (tIsrFunc)Vector55_handler,                         /* Timer 0 0xFFEE                */
  (tIsrFunc)Vector56_handler,                         /* RTI 0xFFF0                    */
  (tIsrFunc)Vector57_handler,                         /* IRQ 0xFFF2                    */
  (tIsrFunc)Vector58_handler,                         /* XIRQ 0xFFF4                   */
  (tIsrFunc)Vector59_handler,                         /* SWI 0xFFF6                    */
  (tIsrFunc)Vector60_handler,                         /* Unimpl Instr Trap 0xFFF8      */
  (tIsrFunc)_Startup,                                 /* COP Failure Reset 0xFFFA      */
  (tIsrFunc)Vector62_handler,                         /* COP Clk Mon Fail 0xFFFC       */
  (tIsrFunc)_Startup                                  /* Reset(N/A) 0xFFFE             */
};

/************************************ end of vectors.c *********************************/


