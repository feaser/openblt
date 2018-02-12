/************************************************************************************//**
* \file         Demo\ARMCM3_STM32F1_Olimex_STM32H103_GCC\Boot\usb_conf.h
* \brief        Bootloader USB device configuration header file.
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
#ifndef USB_CONF_H
#define USB_CONF_H

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Total number of used endpoints. */
#define EP_NUM              (3)
/** \brief Buffer table base address. */
#define BTABLE_ADDRESS      (0x00)
/** \brief EP0 rx buffer base address. */
#define ENDP0_RXADDR        (0x40)
/** \brief EP0 tx buffer base address. */
#define ENDP0_TXADDR        (0x80)
/** \brief EP1 rx buffer base address */
#define ENDP1_RXADDR        (0xC0)
/** \brief EP1 tx buffer base address */
#define ENDP1_TXADDR        (0x100)
/** \brief Mask defining which events has to be handled by the device application software. */
#define IMR_MSK (CNTR_CTRM  | CNTR_SOFM  | CNTR_RESETM )
/** \brief Enable start of frame callback. */
#define SOF_CALLBACK
/* CTR service routines associated to defined endpoints. keep EP1_IN and EP1_OUT
 * callback uncommented to enable them. They are implemented in usb_endp.c
 */
/*#define  EP1_IN_Callback   NOP_Process*/
#define  EP2_IN_Callback   NOP_Process
#define  EP3_IN_Callback   NOP_Process
#define  EP4_IN_Callback   NOP_Process
#define  EP5_IN_Callback   NOP_Process
#define  EP6_IN_Callback   NOP_Process
#define  EP7_IN_Callback   NOP_Process
/*#define  EP1_OUT_Callback   NOP_Process*/
#define  EP2_OUT_Callback   NOP_Process
#define  EP3_OUT_Callback   NOP_Process
#define  EP4_OUT_Callback   NOP_Process
#define  EP5_OUT_Callback   NOP_Process
#define  EP6_OUT_Callback   NOP_Process
#define  EP7_OUT_Callback   NOP_Process

#endif /* USB_CONF_H */

/*********************************** end of usb_conf.h *********************************/

