/************************************************************************************//**
* \file         Demo/HCS12_DevKit_S12G128_CodeWarrior/Prog/main.c
* \brief        Demo program application source file.
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
* Function prototypes
****************************************************************************************/
static void Init(void);
static void SystemClockInit(void);


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
  /* initialize the bootloader interface */
  BootComInit();

  /* start the infinite program loop */
  for (;;)
  {
    /* toggle LED with a fixed frequency */
    LedToggle();
    /* check for bootloader activation request */
    BootComCheckActivationRequest();
  }
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  volatile unsigned long delayCnt;

  /* ----- initialize the system clock ----- */
  SystemClockInit();
  
  /* ----- configure the CAN Tx and Rx pins ----- */
  /* configure pullup device for the CAN pins */
  PPSM &= ~0x03;
  /* enable the pull device for the CAN pins */
  PERM |= 0x03;
  /* configure the CAN tx pin as a push pull output */
  WOMM |= 0x02;
  
  /* ---- configure the SPI pins for the SBC with CAN transceiver ---- */
  /* configure a pullup device for SPI MISO, MOSI, CLK and a pulldown for CS */
  PPSJ &= ~0x7;
  PPSJ |= 0x10;
  /* enabled the pull device for the SPI pins */
  PERJ |= 0x0F;
  /* disable interrupt on the SPI CS pin */
  PIEJ &= ~0x10;
  /* configure the SPI CS pin as a digital output and deselect the chip */
  PTJ |= 0x10;
  DDRJ |= 0x10;
  
  /* ---- initialize the SPI peripheral ---- */  
  /* disable the SPI1 module and clearing flags in SPISR register */   
  SPI1CR1 = 0x00;
  /* set configuration in control register 2
   *  XFRW=1,MODFEN=1, BIDIROE=0, SPISWAI=0, SPC0=0
   */ 
  SPI1CR2 = 0x50;
  /* configure the communication speed */
  SPI1BR = 0x42;
  /* set the configuration in control register 1
   *   SPIE=0, SPE=1, SPTIE=0, MSTR=1, CPOL=0, CPHA=1, SSOE=1, LSBFE=0
   */
  SPI1CR1 = 0x56;
  
  /* ---- communicate with SBC via SPI to enable CAN communication ---- */
  /* delay a bit to make sure the SBC is ready to receive new data */
  delayCnt = 300;
  while(delayCnt-- > 0);
  /* read Vreg register H */    
  SPI1DRH = 0xDF;
  SPI1DRL = 0x80;
  /* wait for the SPI transmit data register to be empty */
  while(SPI1SR_SPTEF == 0);

  /* delay a bit to make sure the SBC is ready to receive new data */
  delayCnt = 300;
  while(delayCnt-- > 0);
  /* enter in "normal mode" */    
  SPI1DRH = 0x5A;
  SPI1DRL = 0x00;  
  /* wait for the SPI transmit data register to be empty */
  while(SPI1SR_SPTEF == 0);

  /* delay a bit to make sure the SBC is ready to receive new data */
  delayCnt = 300;
  while(delayCnt-- > 0);
  /* enable 5V-CAN and Vaux */
  SPI1DRH = 0x5E;
  SPI1DRL = 0x90;
  /* wait for the SPI transmit data register to be empty */
  while(SPI1SR_SPTEF == 0);

  /* delay a bit to make sure the SBC is ready to receive new data */
  delayCnt = 300;
  while(delayCnt-- > 0);
  /* set CAN in Tx-Rx mode, fast slew rate */  
  SPI1DRH = 0x60;
  SPI1DRL = 0xC0;
  /* wait for the SPI transmit data register to be empty */
  while(SPI1SR_SPTEF == 0);

  /* ---- init the led driver ---- */
  LedInit();
  
  /* ---- init the timer driver ---- */
  TimerInit();
  
  /* ---- enable interrupts ---- */
  EnableInterrupts;
} /*** end of Init ***/


/************************************************************************************//**
** \brief     Initializes the clock configuration of the microcontroller.
** \return    none.
**
****************************************************************************************/
static void SystemClockInit(void)
{
  /* system clock initialization in PEE mode. the external 8 MHz crystal oscillator is 
   * used to drive the PLL such that the system clock (Fpll) is 24 MHz, with and Fvco
   * of 48 MHz and the bus clock is Fpll/2 = 12 MHz.
   */
  /* disable the protection of the clock configuration registers */
  CPMUPROT = 0x26;
  /* configure the oscillator to be disabled in stop mode */
  CPMUCLKS_PSTP = 0;
  /* enable the PLL to allow write to divider registers */
  CPMUCLKS_PLLSEL = 1;
  /* configure Fref to be 4 MHz. REFDIV = 0, REFCLK = 2 - 6 MHz.
   * Fref = Fosc / (REFDIV + 1)
   */
  CPMUREFDIV = 0x41;
  /* configure Fvco to be 48 MHz. SYNDIV = 11, VCOFRQ = 48 - 50 MHz.
   * Fvco = Fref * (SYNDIV + 1)
   */
  CPMUSYNR = 0x4B;
  /* select Fpll (locked) to be 24 MHz. POSTDIV = 1.
   * Fpll = Fvco / (POSTDIV + 1)
   */
  CPMUPOSTDIV = 0x01;
  /* set Fpll as the source of the system clocks */
  CPMUCLKS_PLLSEL = 1;
  /* wait for hardware handshake, which verifies a correct configuration of CPMUCLKS */
  while(CPMUCLKS_PLLSEL == 0) 
  {
    ;
  }
  /* enabled the external oscillator, since it is used to drive the PLL */
  CPMUOSC_OSCE = 1;
  /* wait for the oscillation to stabilize */
  while(CPMUFLG_UPOSC == 0) 
  {
    ;  
  }
  /* configure the PLL frequency modulation */
  CPMUPLL = 0x00U;
  /* wait for the PLL to lock */
  while(CPMUFLG_LOCK == 0) 
  {
    ;
  }
  /* enable the protection of the clock configuration registers */
  CPMUPROT = 0x00;
} /*** end of SystemClockInit ***/


/*********************************** end of main.c *************************************/
