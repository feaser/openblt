/************************************************************************************//**
* \file         Demo\ARMCM3_STM32F1_Olimex_STM32H103_Crossworks\Boot\usb_pwr.c
* \brief        Bootloader USB device power management source file.
* \ingroup      Boot_ARMCM3_STM32F1_Olimex_STM32H103_Crossworks
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
#include "stm32f10x.h"
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_pwr.h"
#include "types.h"


/****************************************************************************************
* External functions
****************************************************************************************/
extern void UsbConnectHook(blt_bool connect);
extern void UsbEnterLowPowerMode(void);
extern void UsbLeaveLowPowerMode(void);


/****************************************************************************************
* Global data declarations
****************************************************************************************/
/** \brief USB device status. */
volatile uint32_t bDeviceState = UNCONNECTED; 
/** \brief True when suspend is possible. */
volatile bool fSuspendEnabled = TRUE;  

/** \brief Data structure to group together resume state related information. */
struct
{
  volatile RESUME_STATE eState;
  volatile uint8_t bESOFcnt;
}ResumeS;


/************************************************************************************//**
** \brief     Power on routine for the USB device 
** \return    USB_SUCCESS.
**
****************************************************************************************/
RESULT PowerOn(void)
{
  uint16_t wRegVal;

  /* cable plugged-in */
  UsbConnectHook(BLT_TRUE);
  /* CNTR_PWDN = 0 */
  wRegVal = CNTR_FRES;
  _SetCNTR(wRegVal);
  /* CNTR_FRES = 0 */
  wInterrupt_Mask = 0;
  _SetCNTR(wInterrupt_Mask);
  /* clear pending interrupts */
  _SetISTR(0);
  /* set interrupt mask ***/
  wInterrupt_Mask = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM;
  _SetCNTR(wInterrupt_Mask);
  return USB_SUCCESS;
} /*** end of PowerOn ***/


/************************************************************************************//**
** \brief     Power of routine for the USB device to handle a switch of event.
** \return    USB_SUCCESS.
**
****************************************************************************************/
RESULT PowerOff()
{
  /* disable all interrupts and force USB reset */
  _SetCNTR(CNTR_FRES);
  /* clear interrupt status register */
  _SetISTR(0);
  /* Disable the Pull-Up*/
  UsbConnectHook(BLT_FALSE);
  /* switch-off device */
  _SetCNTR(CNTR_FRES + CNTR_PDWN);
  return USB_SUCCESS;
} /*** end of PowerOff ***/


/************************************************************************************//**
** \brief     Sets suspend mode operating conditions.
** \return    none.
**
****************************************************************************************/
void Suspend(void)
{
  uint16_t wCNTR;

  /* macrocell enters suspend mode */
  wCNTR = _GetCNTR();
  wCNTR |= CNTR_FSUSP;
  _SetCNTR(wCNTR);
  /* ------------------ ONLY WITH BUS-POWERED DEVICES ---------------------- */
  /* power reduction */
  /* force low-power mode in the macrocell */
  wCNTR = _GetCNTR();
  wCNTR |= CNTR_LPMODE;
  _SetCNTR(wCNTR);
  /* switch-off the clocks */
  UsbEnterLowPowerMode();
} /*** end of Suspend ***/


/************************************************************************************//**
** \brief     Handles wake-up restoring normal operations.
** \return    none.
**
****************************************************************************************/
void Resume_Init(void)
{
  uint16_t wCNTR;

  /* ------------------ ONLY WITH BUS-POWERED DEVICES ---------------------- */
  /* CNTR_LPMODE = 0 */
  wCNTR = _GetCNTR();
  wCNTR &= (~CNTR_LPMODE);
  _SetCNTR(wCNTR);

  /* restore full power */
  /* ... on connected devices */
  UsbLeaveLowPowerMode();
  /* reset FSUSP bit */
  _SetCNTR(IMR_MSK);
} /*** end of Resume_Init ***/


/************************************************************************************//**
** \brief     This is the state machine handling resume operations and timing 
**            sequence. The control is based on the Resume structure variables and 
**            on the ESOF interrupt calling this subroutine  without changing 
**            machine state.
** \param     eResumeSetVal a state machine value (RESUME_STATE)
** \return    none.
**
****************************************************************************************/
void Resume(RESUME_STATE eResumeSetVal)
{
  uint16_t wCNTR;

  if (eResumeSetVal != RESUME_ESOF)
    ResumeS.eState = eResumeSetVal;

  switch (ResumeS.eState)
  {
    case RESUME_EXTERNAL:
      Resume_Init();
      ResumeS.eState = RESUME_OFF;
      break;
    case RESUME_INTERNAL:
      Resume_Init();
      ResumeS.eState = RESUME_START;
      break;
    case RESUME_LATER:
      ResumeS.bESOFcnt = 2;
      ResumeS.eState = RESUME_WAIT;
      break;
    case RESUME_WAIT:
      ResumeS.bESOFcnt--;
      if (ResumeS.bESOFcnt == 0)
        ResumeS.eState = RESUME_START;
      break;
    case RESUME_START:
      wCNTR = _GetCNTR();
      wCNTR |= CNTR_RESUME;
      _SetCNTR(wCNTR);
      ResumeS.eState = RESUME_ON;
      ResumeS.bESOFcnt = 10;
      break;
    case RESUME_ON:
      ResumeS.bESOFcnt--;
      if (ResumeS.bESOFcnt == 0)
      {
        wCNTR = _GetCNTR();
        wCNTR &= (~CNTR_RESUME);
        _SetCNTR(wCNTR);
        ResumeS.eState = RESUME_OFF;
      }
      break;
    case RESUME_OFF:
    case RESUME_ESOF:
    default:
      ResumeS.eState = RESUME_OFF;
      break;
  }
} /*** end of Resume ***/


/*********************************** end of usb_pwr.c **********************************/
