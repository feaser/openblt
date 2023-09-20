/************************************************************************************//**
* \file         Demo/HCS12_Evbplus_Dragon12p_CodeWarrior/Prog/vectors.c
* \brief        Demo program interrupt vectors source file.
* \ingroup      Prog_HCS12_Evbplus_Dragon12p_CodeWarrior
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2013  by Feaser    http://www.feaser.com    All rights reserved
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
#include "header.h"                                  /* generic header                 */


/****************************************************************************************
* External functions
****************************************************************************************/
extern void near _Startup(void);


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Type for vector table entries. */
typedef void (*near tIsrFunc)(void);


/****************************************************************************************
** NAME:           UnusedISR
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Catch-all for unused interrrupt service routines.
**
****************************************************************************************/
__interrupt void UnusedISR(void)
{
  /* unexpected interrupt occured, so halt the system */
  while (1) { ; }
} /*** end of UnusedISR ***/


/****************************************************************************************
*                I N T E R R U P T     V E C T O R     T A B L E
****************************************************************************************/
/** \brief   Interrupt vector table.
 *  \details Normally, these are at 0xff80-0xffff, but the bootloader occupies 0xe800 - 
 *           0xffff. The bootloader expects the vector table to be at the end of user 
 *           program flash, which is 0xe780 - 0xe7ff. 2 more bytes are reserved for the
 *           checksum that is programmed and verified by the bootloader, so the start 
 *           address ends up being 0xe77e. Note that this needs to be updated when the 
 *           size of the bootloader changes, as defined in the flashLayout[] table in 
 *           flash.c of the bootloader.
 */
#if (BDM_DEBUGGING_ENABLED == 1)
/* for programming and debugging with a BDM device, the vector table should be at
 * its default location.
 */
const tIsrFunc _vectab[] @0xff80 =
#else
const tIsrFunc _vectab[] @0xe77e =
#endif 
{
#if (BDM_DEBUGGING_ENABLED != 1)
  /* for programming and debugging with a BDM device, the checksum should not be
   * programmed because it would be in a reserved flash memory space.
   */
  (tIsrFunc)0xaa55,                                   /* Reserved for OpenBLT checksum */
#endif  
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFF80               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFF82               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFF84               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFF86               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFF88               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFF8A               */
  (tIsrFunc)UnusedISR,                                /* PWM Emergency Shutdown 0xFF8C */
  (tIsrFunc)UnusedISR,                                /* PortP Interrupt 0xFF8E        */
  (tIsrFunc)UnusedISR,                                /* MSCAN4 Transmit 0xFF90        */
  (tIsrFunc)UnusedISR,                                /* MSCAN4 Receive 0xFF92         */
  (tIsrFunc)UnusedISR,                                /* MSCAN4 Errors 0xFF94          */
  (tIsrFunc)UnusedISR,                                /* MSCAN4 WakeUp 0xFF96          */
  (tIsrFunc)UnusedISR,                                /* MSCAN3 Transmit 0xFF98        */
  (tIsrFunc)UnusedISR,                                /* MSCAN3 Receive 0xFF9A         */
  (tIsrFunc)UnusedISR,                                /* MSCAN3 Errors 0xFF9C          */
  (tIsrFunc)UnusedISR,                                /* MSCAN3 WakeUp 0xFF9E          */
  (tIsrFunc)UnusedISR,                                /* MSCAN2 Transmit 0xFFA0        */
  (tIsrFunc)UnusedISR,                                /* MSCAN2 Receive 0xFFA2         */
  (tIsrFunc)UnusedISR,                                /* MSCAN2 Errors 0xFFA4          */
  (tIsrFunc)UnusedISR,                                /* MSCAN2 WakeUp 0xFFA6          */
  (tIsrFunc)UnusedISR,                                /* MSCAN1 Transmit 0xFFA8        */
  (tIsrFunc)UnusedISR,                                /* MSCAN1 Receive 0xFFAA         */
  (tIsrFunc)UnusedISR,                                /* MSCAN1 Errors 0xFFAC          */
  (tIsrFunc)UnusedISR,                                /* MSCAN1 WakeUp 0xFFAE          */
  (tIsrFunc)UnusedISR,                                /* MSCAN0 Transmit 0xFFB0        */
  (tIsrFunc)UnusedISR,                                /* MSCAN0 Receive 0xFFB2         */
  (tIsrFunc)UnusedISR,                                /* MSCAN0 Errors 0xFFB4          */
  (tIsrFunc)UnusedISR,                                /* MSCAN0 WakeUp 0xFFB6          */
  (tIsrFunc)UnusedISR,                                /* Flash 0xFFB8                  */
  (tIsrFunc)UnusedISR,                                /* Eeprom WakeUp 0xFFBA          */
  (tIsrFunc)UnusedISR,                                /* SPI2  0xFFBC                  */
  (tIsrFunc)UnusedISR,                                /* SPI1  0xFFBE                  */
  (tIsrFunc)UnusedISR,                                /* IIC Bus 0xFFC0                */
  (tIsrFunc)UnusedISR,                                /* DLC 0xFFC2                    */
  (tIsrFunc)UnusedISR,                                /* SCME 0xFFC4                   */
  (tIsrFunc)UnusedISR,                                /* CRG Lock 0xFFC6               */
  (tIsrFunc)UnusedISR,                                /* Pulse AccB Overflow 0xFFC8    */
  (tIsrFunc)UnusedISR,                                /* Mod Down Cnt Underflow 0xFFCA */
  (tIsrFunc)UnusedISR,                                /* PortH Interrupt 0xFFCC        */
  (tIsrFunc)UnusedISR,                                /* PortJ Interrupt 0xFFCE        */
  (tIsrFunc)UnusedISR,                                /* ATD1 0xFFD0                   */
  (tIsrFunc)UnusedISR,                                /* ATD0 0xFFD2                   */
  (tIsrFunc)UnusedISR,                                /* SCI1 0xFFD4                   */
  (tIsrFunc)UnusedISR,                                /* SCI0 0xFFD6                   */
  (tIsrFunc)UnusedISR,                                /* SPI0 0xFFD8                   */
  (tIsrFunc)UnusedISR,                                /* Pulse AccA Input Edge 0xFFDA  */
  (tIsrFunc)UnusedISR,                                /* Pulse AccA Overflow 0xFFDC    */
  (tIsrFunc)UnusedISR,                                /* Timer Overflow 0xFFDE         */
  (tIsrFunc)UnusedISR,                                /* Timer 7 0xFFE0                */ 
  (tIsrFunc)UnusedISR,                                /* Timer 6 0xFFE2                */
  (tIsrFunc)UnusedISR,                                /* Timer 5 0xFFE4                */
  (tIsrFunc)UnusedISR,                                /* Timer 4 0xFFE6                */
  (tIsrFunc)UnusedISR,                                /* Timer 3 0xFFE8                */
  (tIsrFunc)UnusedISR,                                /* Timer 2 0xFFEA                */
  (tIsrFunc)UnusedISR,                                /* Timer 1 0xFFEC                */
  (tIsrFunc)TimerISRHandler,                          /* Timer 0 0xFFEE                */
  (tIsrFunc)UnusedISR,                                /* RTI 0xFFF0                    */
  (tIsrFunc)UnusedISR,                                /* IRQ 0xFFF2                    */
  (tIsrFunc)UnusedISR,                                /* XIRQ 0xFFF4                   */
  (tIsrFunc)UnusedISR,                                /* SWI 0xFFF6                    */
  (tIsrFunc)UnusedISR,                                /* Unimpl Instr Trap 0xFFF8      */
  (tIsrFunc)UnusedISR,                                /* COP Failure Reset 0xFFFA      */
  (tIsrFunc)UnusedISR,                                /* COP Clk Mon Fail 0xFFFC       */
  (tIsrFunc)_Startup                                  /* Reset(N/A) 0xFFFE             */
};
#pragma CODE_SEG DEFAULT
/************************************ end of vectors.c *********************************/


