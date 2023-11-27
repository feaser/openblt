/************************************************************************************//**
* \file         tbxmb_uart_private.h
* \brief        Modbus UART interface private header file.
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
#ifndef TBXMB_UART_PRIVATE_H
#define TBXMB_UART_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Transport layer callback function to signal the completion of a data transmit
 *         transfer.
 */
typedef void (* tTbxMbUartTransmitComplete)(tTbxMbUartPort         port);


/** \breif Transport layer callback function to signal the reception of new data. */
typedef void (* tTbxMbUartDataReceived)    (tTbxMbUartPort         port, 
                                            uint8_t        const * data, 
                                            uint8_t                len);


/****************************************************************************************
* Function prototypes
****************************************************************************************/
/* cppcheck-suppress [misra-c2012-8.2,unmatchedSuppression] 
 * MISRA false positive: Function types shall be in prototype form with named parameters.
 * This is the case for function TbxMbUartInit(). The cppcheck message can therefore be
 * ignored.
 */
void    TbxMbUartInit    (tTbxMbUartPort                     port, 
                          tTbxMbUartBaudrate                 baudrate,
                          tTbxMbUartDatabits                 databits, 
                          tTbxMbUartStopbits                 stopbits,
                          tTbxMbUartParity                   parity,
                          tTbxMbUartTransmitComplete         transmitCompleteFcn,
                          tTbxMbUartDataReceived             dataReceivedFcn);

uint8_t TbxMbUartTransmit(tTbxMbUartPort                     port, 
                          uint8_t                    const * data, 
                          uint16_t                           len);


#ifdef __cplusplus
}
#endif

#endif /* TBXMB_UART_PRIVATE_H */
/*********************************** end of tbxmb_uart_private.h ***********************/
