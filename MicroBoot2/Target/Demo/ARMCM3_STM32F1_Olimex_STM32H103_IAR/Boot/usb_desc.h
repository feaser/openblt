/************************************************************************************//**
* \file         Demo\ARMCM3_STM32F1_Olimex_STM32H103_IAR\Boot\usb_desc.h
* \brief        Bootloader USB device descriptor header file.
* \ingroup      Boot_ARMCM3_STM32F1_Olimex_STM32H103_IAR
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
#ifndef USB_DESC_H
#define USB_DESC_H

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* descriptor types as defined by the USB specification */
#define USB_DEVICE_DESCRIPTOR_TYPE              (0x01)
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       (0x02)
#define USB_STRING_DESCRIPTOR_TYPE              (0x03)
#define USB_INTERFACE_DESCRIPTOR_TYPE           (0x04)
#define USB_ENDPOINT_DESCRIPTOR_TYPE            (0x05)
/* info regarding the bulk device and application specific descriptor */
#define BULK_DESCRIPTOR_TYPE                    (0x21)
#define BULK_SIZ_DESC                           (0x09)
#define BULK_OFF_DESC                           (0x12)
/* max data transfer length per endpoint */
#define BULK_DATA_SIZE                          (64)
/* length of descriptor tables */
#define BULK_SIZ_DEVICE_DESC                    (18)
#define BULK_SIZ_CONFIG_DESC                    (32)
#define BULK_SIZ_STRING_LANGID                  (4)
#define BULK_SIZ_STRING_VENDOR                  (26)
#define BULK_SIZ_STRING_PRODUCT                 (38)
#define BULK_SIZ_STRING_SERIAL                  (28)
#define BULK_SIZ_STRING_INTERFACE               (44)


/****************************************************************************************
* External data declarations
****************************************************************************************/
extern const uint8_t Bulk_DeviceDescriptor[BULK_SIZ_DEVICE_DESC];
extern const uint8_t Bulk_ConfigDescriptor[BULK_SIZ_CONFIG_DESC];
extern const uint8_t Bulk_StringLangID[BULK_SIZ_STRING_LANGID];
extern const uint8_t Bulk_StringVendor[BULK_SIZ_STRING_VENDOR];
extern const uint8_t Bulk_StringProduct[BULK_SIZ_STRING_PRODUCT];
extern       uint8_t Bulk_StringSerial[BULK_SIZ_STRING_SERIAL];
extern const uint8_t Bulk_StringInterface[BULK_SIZ_STRING_INTERFACE];

#endif /* USB_DESC_H */

/*********************************** end of usb_desc.h *********************************/
