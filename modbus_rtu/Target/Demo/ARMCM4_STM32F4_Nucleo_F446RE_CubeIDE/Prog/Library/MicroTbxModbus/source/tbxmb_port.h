/************************************************************************************//**
* \file         tbxmb_port.h
* \brief        Modbus hardware specific port header file.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2023 by Feaser     www.feaser.com     All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This file is part of MicroTBX-Modbus. MicroTBX-Modbus is free software: you can
* redistribute it and/or modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* MicroTBX-Modbus is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
* PARTICULAR PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with MicroTBX-Modbus.
* If not, see www.gnu.org/licenses/.
*
* \endinternal
****************************************************************************************/
#ifndef TBXMB_PORT_H
#define TBXMB_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Function prototypes
****************************************************************************************/
/* UART hardware port functions. */
void     TbxMbPortUartInit     (tTbxMbUartPort             port, 
                                tTbxMbUartBaudrate         baudrate,
                                tTbxMbUartDatabits         databits, 
                                tTbxMbUartStopbits         stopbits,
                                tTbxMbUartParity           parity);

uint8_t  TbxMbPortUartTransmit (tTbxMbUartPort             port, 
                                uint8_t            const * data, 
                                uint16_t                   len);

/* Timer hardware port functions. */
uint16_t TbxMbPortTimerCount(void);

#ifdef __cplusplus
}
#endif

#endif /* TBXMB_PORT_H */
/*********************************** end of tbxmb_port.h *******************************/
