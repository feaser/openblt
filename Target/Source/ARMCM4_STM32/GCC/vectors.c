/************************************************************************************//**
* \file         Source\ARMCM4_STM32\GCC\vectors.c
* \brief        Bootloader interrupt vector table source file.
* \ingroup      Target_ARMCM4_STM32
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
* You should have received a copy of the GNU General Public License along with OpenBLT.
* If not, see <http://www.gnu.org/licenses/>.
*
* A special exception to the GPL is included to allow you to distribute a combined work 
* that includes OpenBLT without being obliged to provide the source code for any 
* proprietary components. The exception text is included at the bottom of the license
* file <license.html>.
* 
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                    /* bootloader generic header      */


/****************************************************************************************
* External data declarations
****************************************************************************************/
extern blt_int32u _estack;                            /* stack end address (memory.x)  */


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
extern void reset_handler(void);                      /* implemented in cstart.c       */
/** \brief Structure type for vector table entries. */
typedef union
{
  void (*func)(void);                                 /**< for ISR function pointers   */
  blt_int32u ptr;                                     /**< for stack pointer entry     */
}tIsrFunc;

/** \brief Interrupt vector table. */
__attribute__ ((section(".isr_vector")))
const tIsrFunc _vectab[] =
{
  { .ptr = (blt_int32u)&_estack },                    /* the initial stack pointer     */
  { reset_handler               },                    /* the reset handler             */
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
  { UnusedISR                   },                    /* Window Watchdog               */
  { UnusedISR                   },                    /* PVD through EXTI Line detect  */
  { UnusedISR                   },                    /* Tamper                        */
  { UnusedISR                   },                    /* RTC                           */
  { UnusedISR                   },                    /* Flash                         */
  { UnusedISR                   },                    /* RCC                           */
  { UnusedISR                   },                    /* EXTI Line 0                   */
  { UnusedISR                   },                    /* EXTI Line 1                   */
  { UnusedISR                   },                    /* EXTI Line 2                   */
  { UnusedISR                   },                    /* EXTI Line 3                   */
  { UnusedISR                   },                    /* EXTI Line 4                   */
  { UnusedISR                   },                    /* DMA1 Channel 0                */
  { UnusedISR                   },                    /* DMA1 Channel 1                */
  { UnusedISR                   },                    /* DMA1 Channel 2                */
  { UnusedISR                   },                    /* DMA1 Channel 3                */
  { UnusedISR                   },                    /* DMA1 Channel 4                */
  { UnusedISR                   },                    /* DMA1 Channel 5                */
  { UnusedISR                   },                    /* DMA1 Channel 6                */
  { UnusedISR                   },                    /* ADC1 and ADC2, ADC3s          */
  { UnusedISR                   },                    /* CAN1 TX                       */
  { UnusedISR                   },                    /* CAN1 RX0                      */
  { UnusedISR                   },                    /* CAN1 RX1                      */
  { UnusedISR                   },                    /* CAN1 SCE                      */
  { UnusedISR                   },                    /* EXTI Line 9..5                */
  { UnusedISR                   },                    /* TIM1 Break and TIM9           */
  { UnusedISR                   },                    /* TIM1 Update and TIM10         */
  { UnusedISR                   },                    /* TIM1 Trigger/Comm. and TIM11  */
  { UnusedISR                   },                    /* TIM1 Capture Compare          */
  { UnusedISR                   },                    /* TIM2                          */
  { UnusedISR                   },                    /* TIM3                          */
  { UnusedISR                   },                    /* TIM4                          */
  { UnusedISR                   },                    /* I2C1 Event                    */
  { UnusedISR                   },                    /* I2C1 Error                    */
  { UnusedISR                   },                    /* I2C2 Event                    */
  { UnusedISR                   },                    /* I2C1 Error                    */
  { UnusedISR                   },                    /* SPI1                          */
  { UnusedISR                   },                    /* SPI2                          */
  { UnusedISR                   },                    /* USART1                        */
  { UnusedISR                   },                    /* USART2                        */
  { UnusedISR                   },                    /* USART3                        */
  { UnusedISR                   },                    /* EXTI Line 15..10              */
  { UnusedISR                   },                    /* RTC alarm through EXTI line   */
  { UnusedISR                   },                    /* USB OTG FS Wakeup             */
  { UnusedISR                   },                    /* TIM8 Break and TIM12          */
  { UnusedISR                   },                    /* TIM8 Update and TIM13         */
  { UnusedISR                   },                    /* TIM8 Trigger/Comm. and TIM14  */
  { UnusedISR                   },                    /* TIM8 Capture Compare          */
  { UnusedISR                   },                    /* DMA1 Stream7                  */
  { UnusedISR                   },                    /* FSMC                          */
  { UnusedISR                   },                    /* SDIO                          */
  { UnusedISR                   },                    /* TIM5                          */
  { UnusedISR                   },                    /* SPI3                          */
  { UnusedISR                   },                    /* UART4                         */
  { UnusedISR                   },                    /* UART5                         */
  { UnusedISR                   },                    /* TIM6 and DAC1&2 underrun err. */
  { UnusedISR                   },                    /* TIM7                          */
  { UnusedISR                   },                    /* DMA2 Stream 0                 */
  { UnusedISR                   },                    /* DMA2 Stream 1                 */
  { UnusedISR                   },                    /* DMA2 Stream 2                 */
  { UnusedISR                   },                    /* DMA2 Stream 3                 */
  { UnusedISR                   },                    /* DMA2 Stream 4                 */
  { UnusedISR                   },                    /* Ethernet                      */
  { UnusedISR                   },                    /* Ethernet Wakeup               */
  { UnusedISR                   },                    /* CAN2 TX                       */
  { UnusedISR                   },                    /* CAN2 RX0                      */
  { UnusedISR                   },                    /* CAN2 RX1                      */
  { UnusedISR                   },                    /* CAN2 SCE                      */
  { UnusedISR                   },                    /* USB OTG FS                    */
  { UnusedISR                   },                    /* DMA2 Stream 5                 */
  { UnusedISR                   },                    /* DMA2 Stream 6                 */
  { UnusedISR                   },                    /* DMA2 Stream 7                 */
  { UnusedISR                   },                    /* USART6                        */
  { UnusedISR                   },                    /* I2C3 event                    */
  { UnusedISR                   },                    /* I2C3 error                    */
  { UnusedISR                   },                    /* USB OTG HS End Point 1 Out    */
  { UnusedISR                   },                    /* USB OTG HS End Point 1 In     */
  { UnusedISR                   },                    /* USB OTG HS Wakeup through EXTI*/
  { UnusedISR                   },                    /* USB OTG HS                    */
  { UnusedISR                   },                    /* DCMI                          */
  { UnusedISR                   },                    /* CRYP crypto                   */
  { UnusedISR                   },                    /* Hash and Rng                  */
  { UnusedISR                   }                     /* FPU                           */
};


/************************************ end of vectors.c *********************************/


