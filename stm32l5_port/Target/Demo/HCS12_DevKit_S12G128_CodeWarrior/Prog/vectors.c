/************************************************************************************//**
* \file         Demo/HCS12_DevKit_S12G128_CodeWarrior/Prog/vectors.c
* \brief        Demo program interrupt vectors source file.
* \ingroup      Prog_HCS12_DevKit_S12G128_CodeWarrior
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2018  by Feaser    http://www.feaser.com    All rights reserved
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
#include "header.h"                                    /* generic header               */


/****************************************************************************************
* External functions
****************************************************************************************/
extern void near _Startup(void);


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Type for vector table entries. */
typedef void (*near tIsrFunc)(void);


#pragma CODE_SEG __NEAR_SEG NON_BANKED
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
  for (;;) 
  { 
    ; 
  }
} /*** end of UnusedISR ***/
#pragma CODE_SEG DEFAULT


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
  (tIsrFunc)UnusedISR,                                /* Spurious interrupt            */
  (tIsrFunc)UnusedISR,                                /* Port AD interrupt             */
  (tIsrFunc)UnusedISR,                                /* ADC compare interrupt         */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFF86               */
  (tIsrFunc)UnusedISR,                                /* Autonomous periodical int.    */
  (tIsrFunc)UnusedISR,                                /* Low voltage interrupt         */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFF8C               */
  (tIsrFunc)UnusedISR,                                /* PortP interrupt               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFF90               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFF92               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFF94               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFF96               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFF98               */
  (tIsrFunc)UnusedISR,                                /* Reseved 0xFF9A                */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFF9C               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFF9E               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFFA0               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFFA2               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFFA4               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFFA6               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFFA8               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFFAA               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFFAC               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFFAE               */
  (tIsrFunc)UnusedISR,                                /* CAN transmit                  */
  (tIsrFunc)UnusedISR,                                /* CAN receive                   */
  (tIsrFunc)UnusedISR,                                /* CAN errors                    */
  (tIsrFunc)UnusedISR,                                /* CAN wakeup                    */
  (tIsrFunc)UnusedISR,                                /* Flash command                 */
  (tIsrFunc)UnusedISR,                                /* Flash error                   */
  (tIsrFunc)UnusedISR,                                /* SPI2                          */
  (tIsrFunc)UnusedISR,                                /* SPI1                          */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFFC0               */
  (tIsrFunc)UnusedISR,                                /* SCI2                          */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFFC4               */
  (tIsrFunc)UnusedISR,                                /* PLL lock                      */
  (tIsrFunc)UnusedISR,                                /* Oscillator status interrupt   */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFFCA               */
  (tIsrFunc)UnusedISR,                                /* ACMP                          */
  (tIsrFunc)UnusedISR,                                /* PortJ interrupt               */
  (tIsrFunc)UnusedISR,                                /* Reserved 0xFFD0               */
  (tIsrFunc)UnusedISR,                                /* ADC                           */
  (tIsrFunc)UnusedISR,                                /* SCI1                          */
  (tIsrFunc)UnusedISR,                                /* SCI0                          */
  (tIsrFunc)UnusedISR,                                /* SPI0                          */
  (tIsrFunc)UnusedISR,                                /* TIM pulse acc input edge      */
  (tIsrFunc)UnusedISR,                                /* TIM pulse acc A overflow      */
  (tIsrFunc)UnusedISR,                                /* TIM timer overflow            */
  (tIsrFunc)UnusedISR,                                /* TIM timer channel 7           */ 
  (tIsrFunc)UnusedISR,                                /* TIM timer channel 6           */
  (tIsrFunc)UnusedISR,                                /* TIM timer channel 5           */
  (tIsrFunc)UnusedISR,                                /* TIM timer channel 4           */
  (tIsrFunc)UnusedISR,                                /* TIM timer channel 3           */
  (tIsrFunc)UnusedISR,                                /* TIM timer channel 2           */
  (tIsrFunc)UnusedISR,                                /* TIM timer channel 1           */
  (tIsrFunc)TimerISRHandler,                          /* TIM timer channel 0           */
  (tIsrFunc)UnusedISR,                                /* RTI time-out interrupt        */
  (tIsrFunc)UnusedISR,                                /* IRQ                           */
  (tIsrFunc)UnusedISR,                                /* XIRQ                          */
  (tIsrFunc)UnusedISR,                                /* SWI                           */
  (tIsrFunc)UnusedISR,                                /* Unimplemented trap interrupt  */
  (tIsrFunc)UnusedISR,                                /* COP watchdog reset            */
  (tIsrFunc)UnusedISR,                                /* Clock monitor reset           */
  (tIsrFunc)_Startup                                  /* Reset                         */
};


/************************************ end of vectors.c *********************************/


