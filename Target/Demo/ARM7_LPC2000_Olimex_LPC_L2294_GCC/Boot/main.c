/************************************************************************************//**
* \file         Demo\ARM7_LPC2000_Olimex_LPC_L2294_GCC\Boot\main.c
* \brief        Bootloader application source file.
* \ingroup      Boot_ARM7_LPC2000_Olimex_LPC_L2294_GCC
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
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
#include "boot.h"                                /* bootloader generic header          */
#include "lpc2294.h"                             /* CPU register definitions           */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called 
**            by the reset interrupt vector after the C-startup routines executed.
** \return    Program return code.
**
****************************************************************************************/
int main(void)
{
  /* initialize the microcontroller */
  Init();
  /* initialize the bootloader */
  BootInit();
  /* start the infinite program loop */
  while (1)
  {
    /* run the bootloader task */
    BootTask();
  }

  /* program should never get here */
  return 0;
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller. The Fpll is set to 60MHz and Fvpb is
**            configured equal to Fpll. The GPIO pin of the status LED is configured
**            as digital output.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  blt_int8u m_sel;                                    /* pll multiplier register value */
  static blt_int8u pll_dividers[] = { 1, 2, 4, 8 };   /* possible pll dividers         */
  blt_int8u p_sel_cnt;                                /* loop counter to find p_sel    */
  blt_int32u f_cco;                                   /* current controller oscillator */
    
  /* check that pll multiplier value will be in the range 1..32 */
  ASSERT_CT((BOOT_CPU_SYSTEM_SPEED_KHZ + ((BOOT_CPU_XTAL_SPEED_KHZ+1)/2)) / \
             BOOT_CPU_XTAL_SPEED_KHZ >= 1);

  ASSERT_CT((BOOT_CPU_SYSTEM_SPEED_KHZ + ((BOOT_CPU_XTAL_SPEED_KHZ+1)/2)) / \
             BOOT_CPU_XTAL_SPEED_KHZ <= 32);

  /* calculate MSEL: M = round(Fcclk / Fosc) */
  m_sel = (BOOT_CPU_SYSTEM_SPEED_KHZ + ((BOOT_CPU_XTAL_SPEED_KHZ+1)/2)) / \
           BOOT_CPU_XTAL_SPEED_KHZ;
  /* value for the PLLCFG register is -1 */
  m_sel--;
  
  /* find PSEL value so that Fcco(= Fcclk * 2 * P) is in the 156000..320000 kHz range. */
  for (p_sel_cnt=0; p_sel_cnt<sizeof(pll_dividers)/sizeof(pll_dividers[0]); p_sel_cnt++)
  {
    /* check f_cco with this pll divider */  
    f_cco = BOOT_CPU_SYSTEM_SPEED_KHZ * 2 * pll_dividers[p_sel_cnt];
    if ( (f_cco >= 156000) && (f_cco <= 320000) )
    {
      /* found a valid pll divider value */
      break;
    }
  }
  /* check that a valid value was found */
  ASSERT_RT(p_sel_cnt < (sizeof(pll_dividers)/sizeof(pll_dividers[0])));

  /* set multiplier and divider values */
  PLLCFG  = (p_sel_cnt << 5) | m_sel;
  PLLFEED = 0xAA;
  PLLFEED = 0x55;
  /* enable the PLL */
  PLLCON  = 0x1;
  PLLFEED = 0xAA;
  PLLFEED = 0x55;
  /* wait for the PLL to lock to set frequency */
  while(!(PLLSTAT & 0x400)) { ; }  
  /* connect the PLL as the clock source */
  PLLCON  = 0x3;
  PLLFEED = 0xAA;
  PLLFEED = 0x55;
  /* enable MAM and set number of clocks used for Flash memory fetch. Recommended:
   *   Fcclk >= 60 MHz: 4 clock cycles  
   *   Fcclk >= 40 MHz: 3 clock cycles  
   *   Fcclk >= 20 MHz: 2 clock cycles  
   *   Fcclk <  20 MHz: 1 clock cycle
   */
  MAMCR   = 0x0;
#if (BOOT_CPU_SYSTEM_SPEED_KHZ >= 60000)
  MAMTIM  = 4;
#elif (BOOT_CPU_SYSTEM_SPEED_KHZ >= 40000)  
  MAMTIM  = 3;
#elif (BOOT_CPU_SYSTEM_SPEED_KHZ >= 20000)  
  MAMTIM  = 2;
#else
  MAMTIM  = 1;
#endif  
  MAMCR   = 0x2;
  /* setting peripheral Clock (pclk) to System Clock (cclk) */
  VPBDIV  = 0x1;
#if (BOOT_NVM_HOOKS_ENABLE > 0)
  /* in this the external memory on the Olimex LPC-L2294 board is used so configure
   * the memory banks for the external flash EEPROM and RAM
   */
  /* external flash EEPROM:
   *   IDCY=3 (idle timing)
   *   WST1=4 (read timing)
   *   RBLE=1 
   *   WST2=6 (write timing)
   *   MW=1   (16-bit data bus)
   */
  BCFG0 = (0x3 << 0) | (0x4 << 5) | (0x1 << 10) | (0x6 << 11) | (0x1 << 28);
  /* external RAM:
   *   IDCY=0 (idle timing)
   *   WST1=0 (read timing)
   *   RBLE=1 
   *   WST2=0 (write timing)
   *   MW=2   (32-bit data bus)
   */
  BCFG1 = (0x0 << 0) | (0x0 << 5) | (0x1 << 10) | (0x0 << 11) | (0x2 << 28);
  /* configure use of data bus and strobe pins for the external memory */
  PINSEL2 = 0x0F000924;
#endif
#if (BOOT_COM_UART_ENABLE > 0)
  /* configure P0.0 for UART0 Tx and P0.1 for UART0 Rx functionality */  
  PINSEL0 |= 0x05;
#endif  
#if (BOOT_COM_CAN_ENABLE > 0)
  /* configure P0.25 for CAN1 Rx functionality */
  PINSEL1 |= 0x00040000L; 
#endif
} /*** end of Init ***/


/*********************************** end of main.c *************************************/
