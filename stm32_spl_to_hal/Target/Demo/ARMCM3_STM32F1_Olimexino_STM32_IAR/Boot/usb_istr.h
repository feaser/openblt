/************************************************************************************//**
* \file         Demo\ARMCM3_STM32F1_Olimexino_STM32_IAR\Boot\usb_istr.h
* \brief        Bootloader USB device interrupt/event handler header file.
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
#ifndef USB_ISTR_H
#define USB_ISTR_H


/****************************************************************************************
* Include files
****************************************************************************************/
#include "usb_conf.h"


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void USB_Istr(void);
void EP1_IN_Callback(void);
void EP2_IN_Callback(void);
void EP3_IN_Callback(void);
void EP4_IN_Callback(void);
void EP5_IN_Callback(void);
void EP6_IN_Callback(void);
void EP7_IN_Callback(void);
void EP1_OUT_Callback(void);
void EP2_OUT_Callback(void);
void EP3_OUT_Callback(void);
void EP4_OUT_Callback(void);
void EP5_OUT_Callback(void);
void EP6_OUT_Callback(void);
void EP7_OUT_Callback(void);
#ifdef CTR_CALLBACK
void CTR_Callback(void);
#endif
#ifdef DOVR_CALLBACK
void DOVR_Callback(void);
#endif
#ifdef ERR_CALLBACK
void ERR_Callback(void);
#endif
#ifdef WKUP_CALLBACK
void WKUP_Callback(void);
#endif
#ifdef SUSP_CALLBACK
void SUSP_Callback(void);
#endif
#ifdef RESET_CALLBACK
void RESET_Callback(void);
#endif
#ifdef SOF_CALLBACK
void SOF_Callback(void);
#endif
#ifdef ESOF_CALLBACK
void ESOF_Callback(void);
#endif


#endif /* USB_ISTR_H */

/*********************************** end of usb_istr.h *********************************/
