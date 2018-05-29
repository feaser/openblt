/************************************************************************************//**
* \file         Demo\ARMCM3_STM32F1_Olimexino_STM32_IAR\Boot\usb_desc.c
* \brief        Bootloader USB device descriptor source file.
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include "usb_lib.h"
#include "usb_desc.h"


/****************************************************************************************
* Constant data declarations
****************************************************************************************/
/** \brief USB device descriptor. */
const uint8_t Bulk_DeviceDescriptor[] =
  {
    0x12,   /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,     /* bDescriptorType */
    0x10,
    0x01,   /* bcdUSB = 1.1 */
    0x00,   /* bDeviceClass: each interface defines the device class */
    0x00,   /* bDeviceSubClass */
    0x00,   /* bDeviceProtocol */
    0x40,   /* bMaxPacketSize0 */
    0x50,
    0x1d,   /* idVendor = 0x1d50 */
    0xac,
    0x60,   /* idProduct = 0x60ac */
    0x00,
    0x01,   /* bcdDevice = 1.00 */
    1,      /* Index of string descriptor describing manufacturer */
    2,      /* Index of string descriptor describing product */
    3,      /* Index of string descriptor describing the device's serial number */
    0x01    /* bNumConfigurations */
  };

/** \brief USB configuration descriptor. */
const uint8_t Bulk_ConfigDescriptor[] =
  {
    /*Configuration Descriptor*/
    0x09,   /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* bDescriptorType: Configuration */
    BULK_SIZ_CONFIG_DESC,       /* wTotalLength:no of returned bytes */
    0x00,
    0x01,   /* bNumInterfaces: 1 interface */
    0x01,   /* bConfigurationValue: Configuration value */
    0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
    0xC0,   /* bmAttributes: self powered */
    0x32,   /* MaxPower 100 mA */
    /*Interface Descriptor*/
    0x09,   /* bLength: Endpoint Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
    0x00,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x02,   /* bNumEndpoints: Two endpoints used */
    0xFF,   /* bInterfaceClass: vendor specific */
    0x00,   /* bInterfaceSubClass */
    0x00,   /* bInterfaceProtocol */
    4,      /* iInterface: */
    /*Endpoint 1 IN Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    0x81,   /* bEndpointAddress: (IN1) */
    0x02,   /* bmAttributes: Bulk */
    BULK_DATA_SIZE,             /* wMaxPacketSize: */
    0x00,
    0x00,    /* bInterval: ignore for Bulk transfer */
    /*Endpoint 1 OUT Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    0x01,   /* bEndpointAddress: (OUT1) */
    0x02,   /* bmAttributes: Bulk */
    BULK_DATA_SIZE,             /* wMaxPacketSize: */
    0x00,
    0x00   /* bInterval: ignore for Bulk transfer */
  };

/** \brief USB string descriptors. */
const uint8_t Bulk_StringLangID[BULK_SIZ_STRING_LANGID] =
  {
    BULK_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04 /* LangID = 0x0409: U.S. English */
  };

/** \brief Vendor string. */
const uint8_t Bulk_StringVendor[BULK_SIZ_STRING_VENDOR] =
  {
    BULK_SIZ_STRING_VENDOR,     /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,             /* bDescriptorType*/
    /* Manufacturer: "OpenBLT User" */
    'O', 0, 'p', 0, 'e', 0, 'n', 0, 'B', 0, 'L', 0, 'T', 0,
    ' ', 0, 'U', 0, 's', 0, 'e', 0, 'r', 0
  };

/** \brief Product string. */
const uint8_t Bulk_StringProduct[BULK_SIZ_STRING_PRODUCT] =
  {
    BULK_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    /* Product name: "WinUSB Bulk Device" */
    'W', 0, 'i', 0, 'n', 0, 'U', 0, 'S', 0, 'B', 0, ' ', 0, 
    'B', 0, 'u', 0, 'l', 0, 'k', 0, ' ', 0,
    'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0
  };

/** \brief Serial number string. */
uint8_t Bulk_StringSerial[BULK_SIZ_STRING_SERIAL] =
  {
    BULK_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    /* Serial name: "SER1234567890" */
    'S', 0, 'E', 0, 'R', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0,
     '8', 0, '9', 0, '0', 0
  };

/** \brief Interface string. */
const uint8_t Bulk_StringInterface[BULK_SIZ_STRING_INTERFACE] =
  {
    BULK_SIZ_STRING_INTERFACE,
    USB_STRING_DESCRIPTOR_TYPE,
    /* Interface 0: "WinUSB Bulk Interface" */
    'W', 0, 'i', 0, 'n', 0, 'U', 0, 'S', 0, 'B', 0, ' ', 0, 
    'B', 0, 'u', 0, 'l', 0, 'k', 0, ' ', 0,
    'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0
  };

/*********************************** end of usb_desc.h *********************************/
