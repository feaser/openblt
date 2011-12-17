/****************************************************************************************
|  Description: bootloader USB device properties source file
|    File Name: usb_prop.c
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
DEVICE Device_Table =
{
  EP_NUM,
  1
};

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

ONE_DESCRIPTOR Device_Descriptor =
{
  (uint8_t*)Bulk_DeviceDescriptor,
  BULK_SIZ_DEVICE_DESC
};

ONE_DESCRIPTOR Config_Descriptor =
{
  (uint8_t*)Bulk_ConfigDescriptor,
  BULK_SIZ_CONFIG_DESC
};

ONE_DESCRIPTOR Bulk_Descriptor =
{
  (uint8_t*)Bulk_ConfigDescriptor + BULK_OFF_DESC,
  BULK_SIZ_DESC
};

ONE_DESCRIPTOR String_Descriptor[5] =
{
  {(uint8_t*)Bulk_StringLangID, BULK_SIZ_STRING_LANGID},
  {(uint8_t*)Bulk_StringVendor, BULK_SIZ_STRING_VENDOR},
  {(uint8_t*)Bulk_StringProduct, BULK_SIZ_STRING_PRODUCT},
  {(uint8_t*)Bulk_StringSerial, BULK_SIZ_STRING_SERIAL},
  {(uint8_t*)Bulk_StringInterface, BULK_SIZ_STRING_INTERFACE}
};


/****************************************************************************************
** NAME:           Bulk_Init
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    USB device initialization function.
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


/****************************************************************************************
** NAME:           Bulk_Reset
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    USB device reset routine.
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


/****************************************************************************************
** NAME:           Bulk_SetConfiguration
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Update the device state to configured.
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


/****************************************************************************************
** NAME:           Bulk_SetDeviceAddress
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Update the device state to addressed.
**
****************************************************************************************/
void Bulk_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
} /*** end of Bulk_SetDeviceAddress ***/


/****************************************************************************************
** NAME:           Bulk_Status_In
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    USB device status in routine.
**
****************************************************************************************/
void Bulk_Status_In(void)
{
  /* status in not used for USB bulk device */
} /*** end of Bulk_Status_In ***/


/****************************************************************************************
** NAME:           Bulk_Status_Out
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    USB device status out routine.
**
****************************************************************************************/
void Bulk_Status_Out(void)
{
  /* status out not used for USB bulk device */
} /*** end of Bulk_Status_Out ***/


/****************************************************************************************
** NAME:           Bulk_Data_Setup
** PARAMETER:      RequestNo request number.
** RETURN VALUE:   USB_UNSUPPORT or USB_SUCCESS.
** DESCRIPTION:    Handles the data class specific requests.
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


/****************************************************************************************
** NAME:           Bulk_NoData_Setup
** PARAMETER:      RequestNo request number.
** RETURN VALUE:   USB_UNSUPPORT or USB_SUCCESS.
** DESCRIPTION:    Handles the no data class specific requests.
**
****************************************************************************************/
RESULT Bulk_NoData_Setup(uint8_t RequestNo)
{
  return USB_UNSUPPORT;
} /*** end of Bulk_NoData_Setup ***/


/****************************************************************************************
** NAME:           Bulk_GetDeviceDescriptor
** PARAMETER:      Length length of the descriptor in bytes.
** RETURN VALUE:   The address of the device descriptor.
** DESCRIPTION:    Gets the device descriptor.
**
****************************************************************************************/
uint8_t *Bulk_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
} /*** end of Bulk_GetDeviceDescriptor ***/


/****************************************************************************************
** NAME:           Bulk_GetConfigDescriptor
** PARAMETER:      Length length of the descriptor in bytes.
** RETURN VALUE:   The address of the configuration descriptor.
** DESCRIPTION:    Gets the configuration descriptor.
**
****************************************************************************************/
uint8_t *Bulk_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
} /*** end of Bulk_GetConfigDescriptor ***/


/****************************************************************************************
** NAME:           Bulk_GetStringDescriptor
** PARAMETER:      Length length of the descriptor in bytes.
** RETURN VALUE:   The address of the string descriptor.
** DESCRIPTION:    Gets the string descriptor.
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


/****************************************************************************************
** NAME:           Bulk_GetBulkDescriptor
** PARAMETER:      Length length of the descriptor in bytes.
** RETURN VALUE:   The address of the bulk descriptor.
** DESCRIPTION:    Gets the bulk descriptor.
**
****************************************************************************************/
uint8_t *Bulk_GetBulkDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Bulk_Descriptor);
} /*** end of Bulk_GetBulkDescriptor ***/


/****************************************************************************************
** NAME:           Bulk_Get_Interface_Setting
** PARAMETER:      Interface        interface number.
**                 AlternateSetting alternate setting number.
** RETURN VALUE:   USB_UNSUPPORT or USB_SUCCESS.
** DESCRIPTION:    Test the interface and the alternate setting according to the
**                 supported one.
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

