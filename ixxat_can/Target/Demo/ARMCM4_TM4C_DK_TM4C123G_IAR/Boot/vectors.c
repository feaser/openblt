/************************************************************************************//**
* \file         Demo/ARMCM4_TM4C_DK_TM4C123G_IAR/Boot/vectors.c
* \brief        Bootloader interrupt vector table source file.
* \ingroup      Boot_ARMCM4_TM4C_DK_TM4C123G_IAR
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2014  by Feaser    http://www.feaser.com    All rights reserved
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
#include "boot.h"                                    /* bootloader generic header      */


/****************************************************************************************
* External functions
****************************************************************************************/
extern void reset_handler(void);


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Structure type for vector table entries. */
typedef union
{
  void (*func)(void);                                 /**< for ISR function pointers   */
  void *ptr;                                          /**< for stack pointer entry     */
} tIsrFunc;


/************************************************************************************//**
** \brief     Catch-all for unused interrrupt service routines.
** \return    none.
**
****************************************************************************************/
void UnusedISR(void)
{
  /* unexpected interrupt occured, so trigger an assertion to halt the system */
  ASSERT_RT(BLT_FALSE);
} /*** end of UnusedISR ***/


/****************************************************************************************
*                I N T E R R U P T     V E C T O R     T A B L E
****************************************************************************************/
#pragma language=extended                             /* enable IAR extensions         */
#pragma segment="CSTACK"

/** \brief Interrupt vector table. */
__root const tIsrFunc __vector_table[] @ ".intvec" =
{
  { .ptr = __sfe("CSTACK")    },                      /* the initial stack pointer     */
  { &reset_handler              },                    /* the reset handler             */
  { UnusedISR                   },                    /* NMI Handler                   */
  { UnusedISR                   },                    /* Hard Fault Handler            */
  { UnusedISR                   },                    /* MPU Fault Handler             */
  { UnusedISR                   },                    /* Bus Fault Handler             */
  { UnusedISR                   },                    /* Usage Fault Handler           */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* SVCall Handler                */
  { UnusedISR                   },                    /* Debug Monitor Handler         */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* PendSV Handler                */
  { UnusedISR                   },                    /* SysTick Handler               */
  { UnusedISR                   },                    /* GPIO Port A                   */
  { UnusedISR                   },                    /* GPIO Port B                   */
  { UnusedISR                   },                    /* GPIO Port C                   */
  { UnusedISR                   },                    /* GPIO Port D                   */
  { UnusedISR                   },                    /* GPIO Port E                   */
  { UnusedISR                   },                    /* UART0 Rx and Tx               */
  { UnusedISR                   },                    /* UART1 Rx and Tx               */
  { UnusedISR                   },                    /* SSI Rx and Tx                 */
  { UnusedISR                   },                    /* I2C Master and Slave          */
  { UnusedISR                   },                    /* PWM Fault                     */
  { UnusedISR                   },                    /* PWM Generator 0               */
  { UnusedISR                   },                    /* PWM Generator 1               */
  { UnusedISR                   },                    /* PWM Generator 2               */
  { UnusedISR                   },                    /* Quadrature Encoder            */
  { UnusedISR                   },                    /* ADC Sequence 0                */
  { UnusedISR                   },                    /* ADC Sequence 1                */
  { UnusedISR                   },                    /* ADC Sequence 2                */
  { UnusedISR                   },                    /* ADC Sequence 3                */
  { UnusedISR                   },                    /* Watchdog timer                */
  { UnusedISR                   },                    /* Timer 0 subtimer A            */
  { UnusedISR                   },                    /* Timer 0 subtimer B            */
  { UnusedISR                   },                    /* Timer 1 subtimer A            */
  { UnusedISR                   },                    /* Timer 1 subtimer B            */
  { UnusedISR                   },                    /* Timer 2 subtimer A            */
  { UnusedISR                   },                    /* Timer 2 subtimer B            */
  { UnusedISR                   },                    /* Analog Comparator 0           */
  { UnusedISR                   },                    /* Analog Comparator 1           */
  { UnusedISR                   },                    /* Analog Comparator 2           */
  { UnusedISR                   },                    /* System Control (PLL, OSC, BO) */
  { UnusedISR                   },                    /* FLASH Control                 */
  { UnusedISR                   },                    /* GPIO Port F                   */
  { UnusedISR                   },                    /* GPIO Port G                   */
  { UnusedISR                   },                    /* GPIO Port H                   */
  { UnusedISR                   },                    /* UART2 Rx and Tx               */
  { UnusedISR                   },                    /* SSI1 Rx and Tx                */
  { UnusedISR                   },                    /* Timer 3 subtimer A            */
  { UnusedISR                   },                    /* Timer 3 subtimer B            */
  { UnusedISR                   },                    /* I2C1 Master and Slave         */
  { UnusedISR                   },                    /* Quadrature Encoder 1          */
  { UnusedISR                   },                    /* CAN0                          */
  { UnusedISR                   },                    /* CAN1                          */
  { UnusedISR                   },                    /* CAN2                          */
  { UnusedISR                   },                    /* Ethernet                      */
  { UnusedISR                   },                    /* Hibernate                     */
  { UnusedISR                   },                    /* USB0                          */
  { UnusedISR                   },                    /* PWM Generator 3               */
  { UnusedISR                   },                    /* uDMA Software Transfer        */
  { UnusedISR                   },                    /* uDMA Error                    */
  { UnusedISR                   },                    /* ADC1 Sequence 0               */
  { UnusedISR                   },                    /* ADC1 Sequence 1               */
  { UnusedISR                   },                    /* ADC1 Sequence 2               */
  { UnusedISR                   },                    /* ADC1 Sequence 3               */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* GPIO Port J                   */
  { UnusedISR                   },                    /* GPIO Port K                   */
  { UnusedISR                   },                    /* GPIO Port L                   */
  { UnusedISR                   },                    /* SSI2 Rx and Tx                */
  { UnusedISR                   },                    /* SSI3 Rx and Tx                */
  { UnusedISR                   },                    /* UART3 Rx and Tx               */
  { UnusedISR                   },                    /* UART4 Rx and Tx               */
  { UnusedISR                   },                    /* UART5 Rx and Tx               */
  { UnusedISR                   },                    /* UART6 Rx and Tx               */
  { UnusedISR                   },                    /* UART7 Rx and Tx               */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* I2C2 Master and Slave         */
  { UnusedISR                   },                    /* I2C3 Master and Slave         */
  { UnusedISR                   },                    /* Timer 4 subtimer A            */
  { UnusedISR                   },                    /* Timer 4 subtimer B            */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Timer 5 subtimer A            */
  { UnusedISR                   },                    /* Timer 5 subtimer B            */
  { UnusedISR                   },                    /* Wide Timer 0 subtimer A       */
  { UnusedISR                   },                    /* Wide Timer 0 subtimer B       */
  { UnusedISR                   },                    /* Wide Timer 1 subtimer A       */
  { UnusedISR                   },                    /* Wide Timer 1 subtimer B       */
  { UnusedISR                   },                    /* Wide Timer 2 subtimer A       */
  { UnusedISR                   },                    /* Wide Timer 2 subtimer B       */
  { UnusedISR                   },                    /* Wide Timer 3 subtimer A       */
  { UnusedISR                   },                    /* Wide Timer 3 subtimer B       */
  { UnusedISR                   },                    /* Wide Timer 4 subtimer A       */
  { UnusedISR                   },                    /* Wide Timer 4 subtimer B       */
  { UnusedISR                   },                    /* Wide Timer 5 subtimer A       */
  { UnusedISR                   },                    /* Wide Timer 5 subtimer B       */
  { UnusedISR                   },                    /* FPU                           */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* I2C4 Master and Slave         */
  { UnusedISR                   },                    /* I2C5 Master and Slave         */
  { UnusedISR                   },                    /* GPIO Port M                   */
  { UnusedISR                   },                    /* GPIO Port N                   */
  { UnusedISR                   },                    /* Quadrature Encoder 2          */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* Reserved                      */
  { UnusedISR                   },                    /* GPIO Port P (Summary or P0)   */
  { UnusedISR                   },                    /* GPIO Port P1                  */
  { UnusedISR                   },                    /* GPIO Port P2                  */
  { UnusedISR                   },                    /* GPIO Port P3                  */
  { UnusedISR                   },                    /* GPIO Port P4                  */
  { UnusedISR                   },                    /* GPIO Port P5                  */
  { UnusedISR                   },                    /* GPIO Port P6                  */
  { UnusedISR                   },                    /* GPIO Port P7                  */
  { UnusedISR                   },                    /* GPIO Port Q (Summary or Q0)   */
  { UnusedISR                   },                    /* GPIO Port Q1                  */
  { UnusedISR                   },                    /* GPIO Port Q2                  */
  { UnusedISR                   },                    /* GPIO Port Q3                  */
  { UnusedISR                   },                    /* GPIO Port Q4                  */
  { UnusedISR                   },                    /* GPIO Port Q5                  */
  { UnusedISR                   },                    /* GPIO Port Q6                  */
  { UnusedISR                   },                    /* GPIO Port Q7                  */
  { UnusedISR                   },                    /* GPIO Port R                   */
  { UnusedISR                   },                    /* GPIO Port S                   */
  { UnusedISR                   },                    /* PWM 1 Generator 0             */
  { UnusedISR                   },                    /* PWM 1 Generator 1             */
  { UnusedISR                   },                    /* PWM 1 Generator 2             */
  { UnusedISR                   },                    /* PWM 1 Generator 3             */
  { UnusedISR                   }                     /* PWM 1 Fault                   */
};


/************************************ end of vectors.c *********************************/


