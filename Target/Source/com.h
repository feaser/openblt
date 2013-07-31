/************************************************************************************//**
* \file         Source\com.h
* \brief        Bootloader communication interface header file.
* \ingroup      Core
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
#ifndef COM_H
#define COM_H


#if (BOOT_COM_ENABLE > 0)
/****************************************************************************************
* Macro definitions
****************************************************************************************/
#if (BOOT_COM_CAN_ENABLE > 0)
  /** \brief Defines the maximum number of bytes for transport transmission on CAN. */
  #define BOOT_COM_TX_MAX_DATA          (BOOT_COM_CAN_TX_MAX_DATA)
  /** \brief Defines the maximum number of bytes for transport reception on CAN. */
  #define BOOT_COM_RX_MAX_DATA          (BOOT_COM_CAN_RX_MAX_DATA)
#endif
#if (BOOT_COM_UART_ENABLE > 0)
  /** \brief Defines the maximum number of bytes for transport transmission on UART. */
  #define BOOT_COM_TX_MAX_DATA          (BOOT_COM_UART_TX_MAX_DATA)
  /** \brief Defines the maximum number of bytes for transport reception on UART. */
  #define BOOT_COM_RX_MAX_DATA          (BOOT_COM_UART_RX_MAX_DATA)
#endif
#if (BOOT_COM_USB_ENABLE > 0)
  /** \brief Defines the maximum number of bytes for transport transmission on USB. */
  #define BOOT_COM_TX_MAX_DATA          (BOOT_COM_USB_TX_MAX_DATA)
  /** \brief Defines the maximum number of bytes for transport reception on USB. */
  #define BOOT_COM_RX_MAX_DATA          (BOOT_COM_USB_RX_MAX_DATA)
#endif


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void     ComInit(void);
void     ComTask(void);
void     ComFree(void);
void     ComTransmitPacket(blt_int8u *data, blt_int16u len);
void     ComSetConnectEntryState(void);
void     ComSetDisconnectEntryState(void);
blt_bool ComIsConnectEntryState(void);
void     ComSetConnected(blt_bool connected);
blt_bool ComIsConnected(void);

#endif /* BOOT_COM_ENABLE > 0 */

#endif /* COM_H */
/*********************************** end of com.h **************************************/
