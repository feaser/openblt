/************************************************************************************//**
* \file         Demo\ARMCM3_STM32F1_Olimex_STM32H103_IAR\Boot\main.c
* \brief        Bootloader application source file.
* \ingroup      Boot_ARMCM3_STM32F1_Olimex_STM32H103_IAR
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
#include "boot.h"                                /* bootloader generic header          */
#include "stm32f10x.h"                           /* microcontroller registers          */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called 
**            by the reset interrupt vector after the C-startup routines executed.
** \return    none.
**
****************************************************************************************/
void main(void)
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
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller. 
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  volatile blt_int32u StartUpCounter = 0, HSEStatus = 0;
  blt_int32u pll_multiplier;

  /* reset the RCC clock configuration to the default reset state (for debug purpose) */
  /* set HSION bit */
  RCC->CR |= (blt_int32u)0x00000001;
  /* reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
  RCC->CFGR &= (blt_int32u)0xF8FF0000;
  /* reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (blt_int32u)0xFEF6FFFF;
  /* reset HSEBYP bit */
  RCC->CR &= (blt_int32u)0xFFFBFFFF;
  /* reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
  RCC->CFGR &= (blt_int32u)0xFF80FFFF;
  /* disable all interrupts and clear pending bits  */
  RCC->CIR = 0x009F0000;
  /* enable HSE */    
  RCC->CR |= ((blt_int32u)RCC_CR_HSEON);
  /* wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } 
  while((HSEStatus == 0) && (StartUpCounter != 1500));
  /* check if time out was reached */
  if ((RCC->CR & RCC_CR_HSERDY) == RESET)
  {
    /* cannot continue when HSE is not ready */
    ASSERT_RT(BLT_FALSE);
  }
  /* enable flash prefetch buffer */
  FLASH->ACR |= FLASH_ACR_PRFTBE;
  /* reset flash wait state configuration to default 0 wait states */
  FLASH->ACR &= (blt_int32u)((blt_int32u)~FLASH_ACR_LATENCY);
#if (BOOT_CPU_SYSTEM_SPEED_KHZ > 48000)
  /* configure 2 flash wait states */
  FLASH->ACR |= (blt_int32u)FLASH_ACR_LATENCY_2;    
#elif (BOOT_CPU_SYSTEM_SPEED_KHZ > 24000)  
  /* configure 1 flash wait states */
  FLASH->ACR |= (blt_int32u)FLASH_ACR_LATENCY_1;    
#endif
  /* HCLK = SYSCLK */
  RCC->CFGR |= (blt_int32u)RCC_CFGR_HPRE_DIV1;
  /* PCLK2 = HCLK/2 */
  RCC->CFGR |= (blt_int32u)RCC_CFGR_PPRE2_DIV2;
  /* PCLK1 = HCLK/2 */
  RCC->CFGR |= (blt_int32u)RCC_CFGR_PPRE1_DIV2;
  /* reset PLL configuration */
  RCC->CFGR &= (blt_int32u)((blt_int32u)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | \
                                          RCC_CFGR_PLLMULL));
  /* assert that the pll_multiplier is between 2 and 16 */
  ASSERT_CT((BOOT_CPU_SYSTEM_SPEED_KHZ/BOOT_CPU_XTAL_SPEED_KHZ) >= 2);
  ASSERT_CT((BOOT_CPU_SYSTEM_SPEED_KHZ/BOOT_CPU_XTAL_SPEED_KHZ) <= 16);
  /* calculate multiplier value */
  pll_multiplier = BOOT_CPU_SYSTEM_SPEED_KHZ/BOOT_CPU_XTAL_SPEED_KHZ;
  /* convert to register value */
  pll_multiplier = (blt_int32u)((pll_multiplier - 2) << 18);
  /* set the PLL multiplier and clock source */
  RCC->CFGR |= (blt_int32u)(RCC_CFGR_PLLSRC_HSE | pll_multiplier);
  /* enable PLL */
  RCC->CR |= RCC_CR_PLLON;
  /* wait till PLL is ready */
  while((RCC->CR & RCC_CR_PLLRDY) == 0)
  {
  }
  /* select PLL as system clock source */
  RCC->CFGR &= (blt_int32u)((blt_int32u)~(RCC_CFGR_SW));
  RCC->CFGR |= (blt_int32u)RCC_CFGR_SW_PLL;    
  /* wait till PLL is used as system clock source */
  while ((RCC->CFGR & (blt_int32u)RCC_CFGR_SWS) != (blt_int32u)0x08)
  {
  }
  
  /* enable clock for PA0 pin peripheral (GPIOA) */
  RCC->APB2ENR |= (blt_int32u)(0x00000004);
  /* configure BUT (GPIOA0) as floating digital input */
  /* first reset the configuration */
  GPIOA->CRL &= ~(blt_int32u)((blt_int32u)0xf << 0);
  /* CNF0[1:0] = %01 and MODE0[1:0] = %00 */
  GPIOA->CRL |= (blt_int32u)((blt_int32u)0x4 << 0);
  
#if (BOOT_COM_UART_ENABLE > 0)
  /* enable clock for USART2 peripheral */
  RCC->APB1ENR |= (blt_int32u)0x00020000;
  /* enable clocks for USART2 transmitter and receiver pins (GPIOA and AFIO) */
  RCC->APB2ENR |= (blt_int32u)(0x00000004 | 0x00000001);
  /* configure USART2 Tx (GPIOA2) as alternate function push-pull */
  /* first reset the configuration */
  GPIOA->CRL &= ~(blt_int32u)((blt_int32u)0xf << 8);
  /* CNF2[1:0] = %10 and MODE2[1:0] = %11 */
  GPIOA->CRL |= (blt_int32u)((blt_int32u)0xb << 8);
  /* configure USART2 Rx (GPIOA3) as alternate function input floating */
  /* first reset the configuration */
  GPIOA->CRL &= ~(blt_int32u)((blt_int32u)0xf << 12);
  /* CNF2[1:0] = %01 and MODE2[1:0] = %00 */
  GPIOA->CRL |= (blt_int32u)((blt_int32u)0x4 << 12);
#endif
#if (BOOT_COM_CAN_ENABLE > 0)
  /* enable clocks for CAN transmitter and receiver pins (GPIOB and AFIO) */
  RCC->APB2ENR |= (blt_int32u)(0x00000008 | 0x00000001);
  /* configure CAN Rx (GPIOB8) as alternate function input pull-up */
  /* first reset the configuration */
  GPIOB->CRH &= ~(blt_int32u)((blt_int32u)0xf << 0);
  /* CNF8[1:0] = %10 and MODE8[1:0] = %00 */
  GPIOB->CRH |= (blt_int32u)((blt_int32u)0x8 << 0);
  /* configure CAN Tx (GPIOB9) as alternate function push-pull */
  /* first reset the configuration */
  GPIOB->CRH &= ~(blt_int32u)((blt_int32u)0xf << 4);
  /* CNF9[1:0] = %10 and MODE9[1:0] = %11 */
  GPIOB->CRH |= (blt_int32u)((blt_int32u)0xb << 4);
  /* remap CAN1 pins to PortB */
  AFIO->MAPR &= ~(blt_int32u)((blt_int32u)0x3 << 13);
  AFIO->MAPR |=  (blt_int32u)((blt_int32u)0x2 << 13);
  /* enable clocks for CAN controller peripheral */
  RCC->APB1ENR |= (blt_int32u)0x02000000;
#endif
#if (BOOT_COM_USB_ENABLE > 0)
  /* divide USB clock by 1.5 to create 48MHz clock */
  RCC->CFGR &= ~(blt_int32u)((blt_int32u)0x1 << 22);
  /* enable the USB clock */
  RCC->APB1ENR |= (blt_int32u)0x00800000;
#endif
} /*** end of Init ***/


/*********************************** end of main.c *************************************/
