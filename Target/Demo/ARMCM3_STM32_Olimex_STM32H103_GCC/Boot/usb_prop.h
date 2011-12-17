/****************************************************************************************
|  Description: bootloader USB device properties header file
|    File Name: usb_prop.h
|        Notes: based on an example from STMicroelectronics
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
#ifndef USB_PROP_H
#define USB_PROP_H


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* enable the supported and required functions by commenting them out */
#define Bulk_GetConfiguration          NOP_Process
/*#define Bulk_SetConfiguration          NOP_Process*/
#define Bulk_GetInterface              NOP_Process
#define Bulk_SetInterface              NOP_Process
#define Bulk_GetStatus                 NOP_Process
#define Bulk_ClearFeature              NOP_Process
#define Bulk_SetEndPointFeature        NOP_Process
#define Bulk_SetDeviceFeature          NOP_Process
/*#define Bulk_SetDeviceAddress          NOP_Process*/


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void Bulk_Init(void);
void Bulk_Reset(void);
void Bulk_SetConfiguration(void);
void Bulk_SetDeviceAddress (void);
void Bulk_Status_In (void);
void Bulk_Status_Out (void);
RESULT Bulk_Data_Setup(uint8_t);
RESULT Bulk_NoData_Setup(uint8_t);
RESULT Bulk_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *Bulk_GetDeviceDescriptor(uint16_t );
uint8_t *Bulk_GetConfigDescriptor(uint16_t);
uint8_t *Bulk_GetStringDescriptor(uint16_t);
uint8_t *Bulk_GetBulkDescriptor(uint16_t Length);
#endif /* USB_PROP_H */

/*********************************** end of usb_prop.h *********************************/

