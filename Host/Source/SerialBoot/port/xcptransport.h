/************************************************************************************//**
* \file         port\xcptransport.h
* \brief        XCP transport layer interface header file.
* \ingroup      SerialBoot
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2014  by Feaser    http://www.feaser.com    All rights reserved
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
#ifndef XCPTRANSPORT_H
#define XCPTRANSPORT_H

/****************************************************************************************
* Type definitions
****************************************************************************************/
typedef struct
{
  sb_uint8 data[XCP_MASTER_RX_MAX_DATA];
  sb_uint8 len;
} tXcpTransportResponsePacket;


/****************************************************************************************
* EFunction prototypes
****************************************************************************************/
sb_uint8 XcpTransportInit(sb_char *device, sb_uint32 baudrate);
sb_uint8 XcpTransportSendPacket(sb_uint8 *data, sb_uint8 len, sb_uint16 timeOutMs);
tXcpTransportResponsePacket *XcpTransportReadResponsePacket(void);
void XcpTransportClose(void);


#endif /* XCPTRANSPORT_H */
/*********************************** end of xcptransport.h *****************************/
