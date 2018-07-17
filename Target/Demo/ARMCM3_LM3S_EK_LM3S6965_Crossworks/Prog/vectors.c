/************************************************************************************//**
* \file         Demo/ARMCM3_LM3S_EK_LM3S6965_Crossworks/Prog/vectors.c
* \brief        Demo program interrupt vectors source file.
* \ingroup      Prog_ARMCM3_LM3S_EK_LM3S6965_Crossworks
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2012  by Feaser    http://www.feaser.com    All rights reserved
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
extern void reset_handler(void);                      /* implemented in cstart.s       */


/****************************************************************************************
* External data declarations
****************************************************************************************/
/** \brief Stack end address (memory.x)  */
extern unsigned long __stack_end__;


/************************************************************************************//**
** \brief     Catch-all for unused interrrupt service routines.
** \return    none.
**
****************************************************************************************/
void UnusedISR(void)
{
  /* unexpected interrupt occured, so halt the system */
  while (1) { ; }
} /*** end of UnusedISR ***/


/****************************************************************************************
*                I N T E R R U P T     V E C T O R     T A B L E
****************************************************************************************/
/** \brief Structure type for vector table entries. */
typedef union
{
  void (*func)(void);                                 /**< for ISR function pointers   */
  unsigned long ptr;                                  /**< for stack pointer entry     */
}tIsrFunc;

/** \brief Interrupt vector table. */
__attribute__ ((section(".vectors")))
const tIsrFunc _vectors[] =
{
  { .ptr = (unsigned long)&__stack_end__ },           /* the initial stack pointer     */
  { reset_handler                  },                 /* the reset handler             */
  { UnusedISR                      },                 /* NMI Handler                   */
  { UnusedISR                      },                 /* Hard Fault Handler            */
  { UnusedISR                      },                 /* MPU Fault Handler             */
  { UnusedISR                      },                 /* Bus Fault Handler             */
  { UnusedISR                      },                 /* Usage Fault Handler           */
  { UnusedISR                      },                 /* Reserved                      */
  { UnusedISR                      },                 /* Reserved                      */
  { UnusedISR                      },                 /* Reserved                      */
  { UnusedISR                      },                 /* Reserved                      */
  { UnusedISR                      },                 /* SVCall Handler                */
  { UnusedISR                      },                 /* Debug Monitor Handler         */
  { UnusedISR                      },                 /* Reserved                      */
  { UnusedISR                      },                 /* PendSV Handler                */
  { TimerISRHandler                },                 /* SysTick Handler               */
  { UnusedISR                      },                 /* GPIO Port A                   */
  { UnusedISR                      },                 /* GPIO Port B                   */
  { UnusedISR                      },                 /* GPIO Port C                   */
  { UnusedISR                      },                 /* GPIO Port D                   */
  { UnusedISR                      },                 /* GPIO Port E                   */
  { UnusedISR                      },                 /* UART0 Rx and Tx               */
  { UnusedISR                      },                 /* UART1 Rx and Tx               */
  { UnusedISR                      },                 /* SSI Rx and Tx                 */
  { UnusedISR                      },                 /* I2C Master and Slave          */
  { UnusedISR                      },                 /* PWM Fault                     */
  { UnusedISR                      },                 /* PWM Generator 0               */
  { UnusedISR                      },                 /* PWM Generator 1               */
  { UnusedISR                      },                 /* PWM Generator 2               */
  { UnusedISR                      },                 /* Quadrature Encoder            */
  { UnusedISR                      },                 /* ADC Sequence 0                */
  { UnusedISR                      },                 /* ADC Sequence 1                */
  { UnusedISR                      },                 /* ADC Sequence 2                */
  { UnusedISR                      },                 /* ADC Sequence 3                */
  { UnusedISR                      },                 /* Watchdog timer                */
  { UnusedISR                      },                 /* Timer 0 subtimer A            */
  { UnusedISR                      },                 /* Timer 0 subtimer B            */
  { UnusedISR                      },                 /* Timer 1 subtimer A            */
  { UnusedISR                      },                 /* Timer 1 subtimer B            */
  { UnusedISR                      },                 /* Timer 2 subtimer A            */
  { UnusedISR                      },                 /* Timer 2 subtimer B            */
  { UnusedISR                      },                 /* Analog Comparator 0           */
  { UnusedISR                      },                 /* Analog Comparator 1           */
  { UnusedISR                      },                 /* Analog Comparator 2           */
  { UnusedISR                      },                 /* System Control (PLL, OSC, BO) */
  { UnusedISR                      },                 /* FLASH Control                 */
  { UnusedISR                      },                 /* GPIO Port F                   */
  { UnusedISR                      },                 /* GPIO Port G                   */
  { UnusedISR                      },                 /* GPIO Port H                   */
  { UnusedISR                      },                 /* UART2 Rx and Tx               */
  { UnusedISR                      },                 /* SSI1 Rx and Tx                */
  { UnusedISR                      },                 /* Timer 3 subtimer A            */
  { UnusedISR                      },                 /* Timer 3 subtimer B            */
  { UnusedISR                      },                 /* I2C1 Master and Slave         */
  { UnusedISR                      },                 /* Quadrature Encoder 1          */
  { UnusedISR                      },                 /* CAN0                          */
  { UnusedISR                      },                 /* CAN1                          */
  { UnusedISR                      },                 /* CAN2                          */
  { UnusedISR                      },                 /* Ethernet                      */
  { UnusedISR                      },                 /* Hibernate                     */
  { (void*)0x55AA11EE              }                  /* Reserved for OpenBLT checksum */
};


/************************************ end of vectors.c *********************************/


