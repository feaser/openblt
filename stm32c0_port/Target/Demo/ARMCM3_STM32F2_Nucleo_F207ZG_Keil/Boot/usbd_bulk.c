/**
  ******************************************************************************
  * @file    usbd_bulk.c
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   This file provides the high layer firmware functions to manage a
  *          USB bulk device.
  *                                  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_bulk.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"


/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_Bulk
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_Bulk_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_Bulk_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_Bulk_Private_Macros
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_Bulk_Private_FunctionPrototypes
  * @{
  */
extern void UsbReceivePipeBulkOUT(uint8_t epnum);
extern void UsbTransmitPipeBulkIN(void);


static uint8_t  USBD_Bulk_Init (USBD_HandleTypeDef *pdev,
                                uint8_t cfgidx);

static uint8_t  USBD_Bulk_DeInit (USBD_HandleTypeDef *pdev,
                                  uint8_t cfgidx);

static uint8_t  USBD_Bulk_Setup (USBD_HandleTypeDef *pdev,
                                 USBD_SetupReqTypedef *req);

static uint8_t  USBD_Bulk_DataIn (USBD_HandleTypeDef *pdev,
                                  uint8_t epnum);

static uint8_t  USBD_Bulk_DataOut (USBD_HandleTypeDef *pdev,
                                  uint8_t epnum);

static uint8_t USBD_Bulk_SOF (USBD_HandleTypeDef *pdev);

static uint8_t  *USBD_Bulk_GetFSCfgDesc (uint16_t *length);

uint8_t  *USBD_Bulk_GetDeviceQualifierDescriptor (uint16_t *length);

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_Bulk_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x10,
  0x01,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @}
  */ 

/** @defgroup USBD_Bulk_Private_Variables
  * @{
  */ 
#if defined ( __ICCARM__ ) /* IAR Compiler */
  #pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
__ALIGN_BEGIN static volatile uint32_t  usbd_bulk_AltSet  __ALIGN_END = 0;
__ALIGN_BEGIN uint8_t USB_Rx_Buffer[BULK_DATA_MAX_PACKET_SIZE] __ALIGN_END ;


/* Bulk interface class callbacks structure */
USBD_ClassTypeDef  USBD_Bulk =
{
  USBD_Bulk_Init,
  USBD_Bulk_DeInit,
  USBD_Bulk_Setup,
  NULL,
  NULL,
  USBD_Bulk_DataIn,
  USBD_Bulk_DataOut,
  USBD_Bulk_SOF,
  NULL,
  NULL,     
  NULL,
  USBD_Bulk_GetFSCfgDesc,
  NULL,
  USBD_Bulk_GetDeviceQualifierDescriptor,
};

/* USB Bulk device Configuration Descriptor */
__ALIGN_BEGIN uint8_t USBD_Bulk_CfgFSDesc[USB_BULK_CONFIG_DESC_SIZ] __ALIGN_END =
{
  /*Configuration Descriptor*/
  0x09,   /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,      /* bDescriptorType: Configuration */
  USB_BULK_CONFIG_DESC_SIZ,                /* wTotalLength:no of returned bytes */
  0x00,
  0x01,   /* bNumInterfaces: 1 interface */
  0x01,   /* bConfigurationValue: Configuration value */
  0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
  0xC0,   /* bmAttributes: self powered */
  0x32,   /* MaxPower 100 mA */
  
  /*---------------------------------------------------------------------------*/
  
  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
  /* Interface descriptor type */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints: One endpoints used */
  0xFF,   /* bInterfaceClass: vendor specific */
  0x00,   /* bInterfaceSubClass  */
  0x00,   /* bInterfaceProtocol */
  0x00,   /* iInterface: */

  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
  BULK_IN_EP,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(BULK_DATA_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(BULK_DATA_MAX_PACKET_SIZE),
  0xFF,                               /* bInterval: ignore for Bulk transfer */
  
  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
  BULK_OUT_EP,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(BULK_DATA_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(BULK_DATA_MAX_PACKET_SIZE),
  0xFF                              /* bInterval: ignore for Bulk transfer */
} ;


#if (USBD_WINUSB_ENABLED == 1)
/* USB Bulk device Microsoft OS 2.0 Descriptor Set. */
#define USB_LEN_MSOS20_DESC   (0x9E)

__ALIGN_BEGIN uint8_t USBD_Bulk_MSOS20Desc[USB_LEN_MSOS20_DESC] __ALIGN_END =
{
  0x0A, 0x00,                 /* Descriptor size (10 bytes) */
  0x00, 0x00,                 /* MS OS 2.0 descriptor set header */
  0x00, 0x00, 0x03, 0x06,     /* Windows version (8.1) (0x06030000) */
  0x9E, 0x00,                 /* Size, MS OS 2.0 descriptor set (158 bytes) */

  /* Microsoft OS 2.0 compatible ID descriptor */
  0x14, 0x00,                 /* Descriptor size (20 bytes) */
  0x03, 0x00,                 /* MS OS 2.0 compatible ID descriptor */
  0x57, 0x49, 0x4E, 0x55, 0x53, 0x42, 0x00, 0x00,     /* WINUSB string */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* Sub-compatible ID */

  /* Registry property descriptor */
  0x80, 0x00,                 /* Descriptor size (130 bytes) */
  0x04, 0x00,                 /* Registry Property descriptor */
  0x01, 0x00,                 /* Strings are null-terminated Unicode */
  0x28, 0x00,                 /* Size of Property Name (40 bytes) */

  /* Property Name ("DeviceInterfaceGUID") */
  0x44, 0x00, 0x65, 0x00, 0x76, 0x00, 0x69, 0x00, 0x63, 0x00, 0x65, 0x00,
  0x49, 0x00, 0x6E, 0x00, 0x74, 0x00, 0x65, 0x00, 0x72, 0x00, 0x66, 0x00,
  0x61, 0x00, 0x63, 0x00, 0x65, 0x00, 0x47, 0x00, 0x55, 0x00, 0x49, 0x00,
  0x44, 0x00, 0x00, 0x00,

  0x4E, 0x00,                 /* Size of Property Data (78 bytes) */

  /* Vendor-defined Property Data: "{807999C3-E4E0-40EA-8188-48E852B54F2B}\0" */
  0x7B, 0x00, 0x38, 0x00, /* {8 */
  0x30, 0x00, 0x37, 0x00, /* 07 */
  0x39, 0x00, 0x39, 0x00, /* 99 */
  0x39, 0x00, 0x43, 0x00, /* 9C */
  0x33, 0x00, 0x2D, 0x00, /* 3- */
  0x45, 0x00, 0x34, 0x00, /* E4 */
  0x45, 0x00, 0x30, 0x00, /* E0 */
  0x2D, 0x00, 0x34, 0x00, /* -4 */
  0x30, 0x00, 0x45, 0x00, /* 0E */
  0x41, 0x00, 0x2D, 0x00, /* A- */
  0x38, 0x00, 0x31, 0x00, /* 81 */
  0x38, 0x00, 0x38, 0x00, /* 88 */
  0x2D, 0x00, 0x34, 0x00, /* -4 */
  0x38, 0x00, 0x45, 0x00, /* 8E */
  0x38, 0x00, 0x35, 0x00, /* 85 */
  0x32, 0x00, 0x42, 0x00, /* 2B */
  0x35, 0x00, 0x34, 0x00, /* 54 */
  0x46, 0x00, 0x32, 0x00, /* F2 */
  0x42, 0x00, 0x7D, 0x00, /* B} */
  0x00, 0x00  /* \0 */
};
#endif /* (USBD_WINUSB_ENABLED == 1) */


/**
  * @}
  */ 

/** @defgroup USBD_Bulk_Private_Functions
  * @{
  */ 

/**
  * @brief  USBD_Bulk_Init
  *         Initialize the Bulk interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_Bulk_Init (USBD_HandleTypeDef *pdev,
                                uint8_t cfgidx)
{
  uint8_t ret = 0;
  
  /* Open EP IN */
  USBD_LL_OpenEP(pdev,
                 BULK_IN_EP,
                 USBD_EP_TYPE_BULK,
                 BULK_DATA_FS_IN_PACKET_SIZE);

  /* Open EP OUT */
  USBD_LL_OpenEP(pdev,
                 BULK_OUT_EP,
                 USBD_EP_TYPE_BULK,
                 BULK_DATA_FS_OUT_PACKET_SIZE);

  /* Prepare Out endpoint to receive next packet */
  USBD_LL_PrepareReceive(pdev,
                         BULK_OUT_EP,
                         &USB_Rx_Buffer[0],
                         BULK_DATA_FS_OUT_PACKET_SIZE);
  
  return ret;
}

/**
  * @brief  USBD_Bulk_DeInit
  *         DeInitialize the Bulk layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_Bulk_DeInit (USBD_HandleTypeDef *pdev,
                                  uint8_t cfgidx)
{
  uint8_t ret = 0;
  
  /* Close EP IN */
  USBD_LL_CloseEP(pdev, BULK_IN_EP);
  
  /* lose EP OUT */
  USBD_LL_CloseEP(pdev, BULK_OUT_EP);
  
  return ret;
}

/**
  * @brief  USBD_Bulk_Setup
  *         Handle the bulk specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_Bulk_Setup (USBD_HandleTypeDef *pdev,
                                 USBD_SetupReqTypedef *req)
{
  uint16_t len=USB_BULK_DESC_SIZ;
  uint8_t  *pbuf=USBD_Bulk_CfgFSDesc + 9;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
#if (USBD_WINUSB_ENABLED == 1)
  case USB_REQ_TYPE_VENDOR:
    switch (req->bRequest)
    {
    case USB_BULK_MS_VENDORCODE: /* bMS_VendorCode */
      if (req->wIndex == 7) /*This is defined by the MS spec (Table 6)*/
      {
        pbuf = USBD_Bulk_MSOS20Desc;
        len = USB_LEN_MSOS20_DESC;
        USBD_CtlSendData(pdev, pbuf, len);
        break;
      }

    default:
      break;
    }
    break;
#endif /* (USBD_WINUSB_ENABLED == 1) */

  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_DESCRIPTOR :
      if ( (req->wValue >> 8) == BULK_DESCRIPTOR_TYPE)
      {
        pbuf = USBD_Bulk_CfgFSDesc + 9 + (9 * USBD_MAX_NUM_INTERFACES);
        len = MIN(USB_BULK_DESC_SIZ , req->wLength);
        
      }
      USBD_CtlSendData (pdev, pbuf, len);
      break;

    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        (uint8_t *)&usbd_bulk_AltSet,
                        1);
      break;
      
    case USB_REQ_SET_INTERFACE :
      if ((uint8_t)(req->wValue) < USBD_MAX_NUM_INTERFACES)
      {
        usbd_bulk_AltSet = (uint8_t)(req->wValue);
      }
      else
      {
        /* Call the error management function (command will be nacked */
        USBD_CtlError (pdev, req);
      }
      break;
    }
 
  default: 
    break;
  }
  return USBD_OK;
}

/**
  * @brief  USBD_Bulk_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  USBD_Bulk_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  /* endpoint finished the previous transmission so see if more data is left */
  UsbTransmitPipeBulkIN();
  return USBD_OK;
}

/**
  * @brief  USBD_Bulk_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  USBD_Bulk_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  /* read the data from the bulk OUT pipe */
  UsbReceivePipeBulkOUT(epnum);
  return USBD_OK;
}

/**
  * @brief  USBD_Bulk_SOF
  *         Start Of Frame event management
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_Bulk_SOF (USBD_HandleTypeDef *pdev)
{
  if (pdev->dev_state == USBD_STATE_CONFIGURED )
  {
    /* Check the data to be sent through IN pipe */
    UsbTransmitPipeBulkIN();
  }
  return USBD_OK;

}

/**
  * @brief  USBD_Bulk_GetFSCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_Bulk_GetFSCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_Bulk_CfgFSDesc);
  return USBD_Bulk_CfgFSDesc;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_Bulk_GetDeviceQualifierDescriptor (uint16_t *length)
{
  *length = sizeof (USBD_Bulk_DeviceQualifierDesc);
  return USBD_Bulk_DeviceQualifierDesc;
}

/**
  * @brief  USBD_Bulk_GetRxBufferPtr
  * @retval Pointer to reception buffer.
  */
uint8_t * USBD_Bulk_GetRxBufferPtr(void)
{
  return &USB_Rx_Buffer[0];
}
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
