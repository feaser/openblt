/************************************************************************************//**
* \file         Demo\ARMCM3_STM32F1_Olimex_STM32H103_IAR\Boot\usb_prop.c
* \brief        Bootloader USB device properties source file.
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


/****************************************************************************************
* Include files
****************************************************************************************/
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"


/****************************************************************************************
* External functions
****************************************************************************************/
extern void UsbGetSerialNum(void);


/****************************************************************************************
* Global data declarations
****************************************************************************************/
/** \brief Device table. */
DEVICE Device_Table =
{
  EP_NUM,
  1
};

/** \brief Device property table. */
DEVICE_PROP Device_Property =
{
  Bulk_Init,
  Bulk_Reset,
  Bulk_Status_In,
  Bulk_Status_Out,
  Bulk_Data_Setup,
  Bulk_NoData_Setup,
  Bulk_Get_Interface_Setting,
  Bulk_GetDeviceDescriptor,
  Bulk_GetConfigDescriptor,
  Bulk_GetStringDescriptor,
  0,
  0x40 /*MAX PACKET SIZE*/
};

/** \brief Supported standard requests. */
USER_STANDARD_REQUESTS User_Standard_Requests =
{
  Bulk_GetConfiguration,
  Bulk_SetConfiguration,
  Bulk_GetInterface,
  Bulk_SetInterface,
  Bulk_GetStatus,
  Bulk_ClearFeature,
  Bulk_SetEndPointFeature,
  Bulk_SetDeviceFeature,
  Bulk_SetDeviceAddress
};

/** \brief Device descriptor wrapper. */
ONE_DESCRIPTOR Device_Descriptor =
{
  (uint8_t*)Bulk_DeviceDescriptor,
  BULK_SIZ_DEVICE_DESC
};

/** \brief Device configuration wrapper. */
ONE_DESCRIPTOR Config_Descriptor =
{
  (uint8_t*)Bulk_ConfigDescriptor,
  BULK_SIZ_CONFIG_DESC
};

/** \brief USB bulk descriptor wrapper. */
ONE_DESCRIPTOR Bulk_Descriptor =
{
  (uint8_t*)Bulk_ConfigDescriptor + BULK_OFF_DESC,
  BULK_SIZ_DESC
};

/** \brief USB string table wrapper. */
ONE_DESCRIPTOR String_Descriptor[5] =
{
  {(uint8_t*)Bulk_StringLangID, BULK_SIZ_STRING_LANGID},
  {(uint8_t*)Bulk_StringVendor, BULK_SIZ_STRING_VENDOR},
  {(uint8_t*)Bulk_StringProduct, BULK_SIZ_STRING_PRODUCT},
  {(uint8_t*)Bulk_StringSerial, BULK_SIZ_STRING_SERIAL},
  {(uint8_t*)Bulk_StringInterface, BULK_SIZ_STRING_INTERFACE}
};


/************************************************************************************//**
** \brief     USB device initialization function.
** \return    none.
**
****************************************************************************************/
void Bulk_Init(void)
{
  /* update the serial number string descriptor with the data from the unique ID*/
  UsbGetSerialNum();
  /* set default configuration */
  pInformation->Current_Configuration = 0;
  /* connect the device */
  PowerOn();
  /* perform basic device initialization operations */
  USB_SIL_Init();
  /* set default state */
  bDeviceState = UNCONNECTED;
} /*** end of Bulk_Init ***/


/************************************************************************************//**
** \brief     USB device reset routine.
** \return    none.
**
****************************************************************************************/
void Bulk_Reset(void)
{
  /* set device as not configured */
  pInformation->Current_Configuration = 0;
  /* current feature initialization */
  pInformation->Current_Feature = Bulk_ConfigDescriptor[7];
  /* set device's the default Interface*/
  pInformation->Current_Interface = 0;
  /* set based address in USB RAM */
  SetBTABLE(BTABLE_ADDRESS);
  /* initialize endpoint 0 CONTROL (mandatory) */
  SetEPType(ENDP0, EP_CONTROL);
  SetEPTxStatus(ENDP0, EP_TX_STALL);
  SetEPRxAddr(ENDP0, ENDP0_RXADDR);
  SetEPTxAddr(ENDP0, ENDP0_TXADDR);
  Clear_Status_Out(ENDP0);
  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPRxValid(ENDP0);
  /* initialize endpoint 1 IN + OUT */
  SetEPType(ENDP1, EP_BULK);
  SetEPTxAddr(ENDP1, ENDP1_TXADDR);
  SetEPRxAddr(ENDP1, ENDP1_RXADDR);
  SetEPTxCount(ENDP1, BULK_DATA_SIZE);
  SetEPRxCount(ENDP1, BULK_DATA_SIZE);
  SetEPRxStatus(ENDP1, EP_RX_VALID);
  SetEPTxStatus(ENDP1, EP_TX_NAK);
  /* set this device to respond on default address */
  SetDeviceAddress(0);
  /* update device state */
  bDeviceState = ATTACHED;
} /*** end of Bulk_Reset ***/


/************************************************************************************//**
** \brief     Update the device state to configured.
** \return    none.
**
****************************************************************************************/
void Bulk_SetConfiguration(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  if (pInfo->Current_Configuration != 0)
  {
    /* device configured */
    bDeviceState = CONFIGURED;
  }
} /*** end of Bulk_SetConfiguration ***/


/************************************************************************************//**
** \brief     Update the device state to addressed.
** \return    none.
**
****************************************************************************************/
void Bulk_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
} /*** end of Bulk_SetDeviceAddress ***/


/************************************************************************************//**
** \brief     USB device status in routine.
** \return    none.
**
****************************************************************************************/
void Bulk_Status_In(void)
{
  /* status in not used for USB bulk device */
} /*** end of Bulk_Status_In ***/


/************************************************************************************//**
** \brief     USB device status out routine.
** \return    none.
**
****************************************************************************************/
void Bulk_Status_Out(void)
{
  /* status out not used for USB bulk device */
} /*** end of Bulk_Status_Out ***/


/************************************************************************************//**
** \brief     Handles the data class specific requests.
** \param     RequestNo Request number.
** \return    USB_UNSUPPORT or USB_SUCCESS.
**
****************************************************************************************/
RESULT Bulk_Data_Setup(uint8_t RequestNo)
{
  uint8_t *(*CopyRoutine)(uint16_t);

  CopyRoutine = NULL;

  if ((RequestNo == GET_DESCRIPTOR)
      && (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
      && (pInformation->USBwIndex0 == 0))
  {

    if (pInformation->USBwValue1 == BULK_DESCRIPTOR_TYPE)
    {
      CopyRoutine = Bulk_GetBulkDescriptor;
    }

  } /* End of GET_DESCRIPTOR */
  if (CopyRoutine == NULL)
  {
    return USB_UNSUPPORT;
  }

  pInformation->Ctrl_Info.CopyData = CopyRoutine;
  pInformation->Ctrl_Info.Usb_wOffset = 0;
  (*CopyRoutine)(0);
  return USB_SUCCESS;
} /*** end of Bulk_Data_Setup ***/


/************************************************************************************//**
** \brief     Handles the no data class specific requests.
** \param     RequestNo Request number.
** \return    USB_UNSUPPORT or USB_SUCCESS.
**
****************************************************************************************/
RESULT Bulk_NoData_Setup(uint8_t RequestNo)
{
  return USB_UNSUPPORT;
} /*** end of Bulk_NoData_Setup ***/


/************************************************************************************//**
** \brief     Gets the device descriptor.
** \param     Length Length of the descriptor in bytes.
** \return    The address of the device descriptor.
**
****************************************************************************************/
uint8_t *Bulk_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
} /*** end of Bulk_GetDeviceDescriptor ***/


/************************************************************************************//**
** \brief     Gets the configuration descriptor.
** \param     Length Length of the descriptor in bytes.
** \return    The address of the configuration descriptor.
**
****************************************************************************************/
uint8_t *Bulk_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
} /*** end of Bulk_GetConfigDescriptor ***/


/************************************************************************************//**
** \brief     Gets the string descriptor.
** \param     Length Length of the descriptor in bytes.
** \return    The address of the string descriptor.
**
****************************************************************************************/
uint8_t *Bulk_GetStringDescriptor(uint16_t Length)
{
  uint8_t wValue0 = pInformation->USBwValue0;
  if (wValue0 > 5)
  {
    return NULL;
  }
  else
  {
    return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
  }
} /*** end of Bulk_GetStringDescriptor ***/


/************************************************************************************//**
** \brief     Gets the bulk descriptor.
** \param     Length Length of the descriptor in bytes.
** \return    The address of the bulk descriptor.
**
****************************************************************************************/
uint8_t *Bulk_GetBulkDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Bulk_Descriptor);
} /*** end of Bulk_GetBulkDescriptor ***/


/************************************************************************************//**
** \brief     Test the interface and the alternate setting according to the
**            supported one.
** \param     Interface        interface number.
** \param     AlternateSetting alternate setting number.
** \return    USB_UNSUPPORT or USB_SUCCESS.
**
****************************************************************************************/
RESULT Bulk_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT;
  }
  else if (Interface > 0)
  {
    return USB_UNSUPPORT;
  }
  return USB_SUCCESS;
} /*** end of Bulk_Get_Interface_Setting ***/


/*********************************** end of usb_prop.h *********************************/

