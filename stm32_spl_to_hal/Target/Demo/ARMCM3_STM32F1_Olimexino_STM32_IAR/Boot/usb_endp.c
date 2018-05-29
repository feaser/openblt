/************************************************************************************//**
* \file         Demo\ARMCM3_STM32F1_Olimexino_STM32_IAR\Boot\usb_endp.c
* \brief        Bootloader USB device endpoint routines source file.
* \ingroup      Boot_ARMCM3_STM32F1_Olimexino_STM32_IAR
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
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "usb_istr.h"
#include "usb_pwr.h"


/****************************************************************************************
* External functions
****************************************************************************************/
extern void UsbTransmitPipeBulkIN(void);
extern void UsbReceivePipeBulkOUT(void);


/************************************************************************************//**
** \brief     Endpoint 1 IN callback that gets called each time that data can be
**            transmitted from the USB device to the host on this endpoint.
** \return    none.
**
****************************************************************************************/
void EP1_IN_Callback(void)
{
  /* endpoint finished the previous transmission so see if more data is left */
  UsbTransmitPipeBulkIN();
} /*** end of EP1_IN_Callback ***/


/************************************************************************************//**
** \brief     Endpoint 1 OUT callback that gets called each time that data was
**            received from the USB host on this endpoint.
** \return    none.
**
****************************************************************************************/
void EP1_OUT_Callback(void)
{
  /* read the data from the bulk OUT pipe */
  UsbReceivePipeBulkOUT();
} /*** end of EP1_OUT_Callback ***/


/************************************************************************************//**
** \brief     Start of frame callback that gets called each time a start of frame
**            was received from the USB host, typically each millisecond. Can be
**            used as a trigger to start a transmission to an IN endpoint.
** \return    none.
**
****************************************************************************************/
void SOF_Callback(void)
{
  if(bDeviceState == CONFIGURED)
  {
    /* Check the data to be sent through IN pipe */
    UsbTransmitPipeBulkIN();
  }  
} /*** end of SOF_Callback ***/


/*********************************** end of usb_endp.c *********************************/

