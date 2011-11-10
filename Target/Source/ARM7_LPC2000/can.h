/****************************************************************************************
|  Description: bootloader CAN communication interface header file
|    File Name: can.h
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
#ifndef CAN_H
#define CAN_H

#if (BOOT_COM_CAN_ENABLE > 0)
/****************************************************************************************
* Function prototypes
****************************************************************************************/
void     CanInit(void);
void     CanTransmitPacket(blt_int8u *data, blt_int8u len);
blt_bool CanReceivePacket(blt_int8u *data);
#endif /* BOOT_COM_CAN_ENABLE > 0 */


#endif /* CAN_H */
/*********************************** end of can.h **************************************/
