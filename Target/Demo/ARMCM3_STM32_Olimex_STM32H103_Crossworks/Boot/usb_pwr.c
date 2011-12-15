/****************************************************************************************
|  Description: bootloader USB device power management source file
|    File Name: usb_pwr.c
|        Notes: based on an example from STMicroelectronics
|
|----------------------------------------------------------------------------------------
|                          C O P Y R I G H T
|----------------------------------------------------------------------------------------
|   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
|
|----------------------------------------------------------------------------------------
|                            L I C E N S E
|----------------------------------------------------------------------------------------
| This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
| modify it under the terms of the GNU General Public License as published by the Free
| Software Foundation, either version 3 of the License, or (at your option) any later
| version.
|
| OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with OpenBLT.
| If not, see <http://www.gnu.org/licenses/>.
|
| A special exception to the GPL is included to allow you to distribute a combined work 
| that includes OpenBLT without being obliged to provide the source code for any 
| proprietary components. The exception text is included at the bottom of the license
| file <license.html>.
| 
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
/* USB device status */
volatile uint32_t bDeviceState = UNCONNECTED; 
/* true when suspend is possible */
volatile bool fSuspendEnabled = TRUE;  

struct
{
  volatile RESUME_STATE eState;
  volatile uint8_t bESOFcnt;
}ResumeS;


/****************************************************************************************
** NAME:           PowerOn
** PARAMETER:      none
** RETURN VALUE:   USB_SUCCESS
** DESCRIPTION:    Power on routine for the USB device
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


/****************************************************************************************
** NAME:           PowerOff
** PARAMETER:      none
** RETURN VALUE:   USB_SUCCESS
** DESCRIPTION:    Power of routine for the USB device to handle a switch of event.
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


/****************************************************************************************
** NAME:           Suspend
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Sets suspend mode operating conditions.
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


/****************************************************************************************
** NAME:           Resume_Init
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Handles wake-up restoring normal operations.
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


/****************************************************************************************
** NAME:           Resume
** PARAMETER:      eResumeSetVal a state machine value (RESUME_STATE)
** RETURN VALUE:   none
** DESCRIPTION:    This is the state machine handling resume operations and timing 
**                 sequence. The control is based on the Resume structure variables and 
**                 on the ESOF interrupt calling this subroutine  without changing 
**                 machine state.
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
