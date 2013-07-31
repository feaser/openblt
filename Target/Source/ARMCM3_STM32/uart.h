/************************************************************************************//**
* \file         Source\ARMCM3_STM32\uart.h
* \brief        Bootloader UART communication interface header file.
* \ingroup      Target_ARMCM3_STM32
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
#ifndef UART_H
#define UART_H

#if (BOOT_COM_UART_ENABLE > 0)
/****************************************************************************************
* Function prototypes
****************************************************************************************/
void     UartInit(void);
void     UartTransmitPacket(blt_int8u *data, blt_int8u len);
blt_bool UartReceivePacket(blt_int8u *data);
#endif /* BOOT_COM_UART_ENABLE > 0 */


#endif /* UART_H */
/*********************************** end of uart.h *************************************/
