/************************************************************************************//**
* \file         Demo\ARMCM3_STM32F1_Olimex_STM32H103_GCC\Boot\usb_pwr.h
* \brief        Bootloader USB device power management header file.
* \ingroup      Boot_ARMCM3_STM32F1_Olimex_STM32H103_GCC
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
#ifndef USB_PWR_H
#define USB_PWR_H


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Enumeration of the supported resume states. */
typedef enum _RESUME_STATE
{
  RESUME_EXTERNAL,
  RESUME_INTERNAL,
  RESUME_LATER,
  RESUME_WAIT,
  RESUME_START,
  RESUME_ON,
  RESUME_OFF,
  RESUME_ESOF
} RESUME_STATE;

/** \brief Enumeration of the supported device states. */
typedef enum _DEVICE_STATE
{
  UNCONNECTED,
  ATTACHED,
  POWERED,
  SUSPENDED,
  ADDRESSED,
  CONFIGURED
} DEVICE_STATE;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void Suspend(void);
void Resume_Init(void);
void Resume(RESUME_STATE eResumeSetVal);
RESULT PowerOn(void);
RESULT PowerOff(void);


/****************************************************************************************
* External data declarations
****************************************************************************************/
/* USB device status */
extern volatile uint32_t bDeviceState; 
/* true when suspend is possible */
extern volatile bool fSuspendEnabled;  

#endif  /* USB_PWR_H*/

/*********************************** end of usb_pwr.h **********************************/
