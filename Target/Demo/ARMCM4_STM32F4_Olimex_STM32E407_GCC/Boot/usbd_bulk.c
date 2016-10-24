/**
  ******************************************************************************
  * @file    usbd_bulk.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides the high layer firmware functions to manage a 
  *          USB bulk device.
  *                                  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
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
#include "usbd_req.h"


extern void UsbReceivePipeBulkOUT(uint8_t epnum);
extern void UsbTransmitPipeBulkIN(void);


/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup usbd_bulk 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup usbd_bulk_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup usbd_bulk_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup usbd_bulk_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup usbd_bulk_Private_FunctionPrototypes
  * @{
  */

/*********************************************
   BULK Device library callbacks
 *********************************************/
static uint8_t  usbd_bulk_Init        (void  *pdev, uint8_t cfgidx);
static uint8_t  usbd_bulk_DeInit      (void  *pdev, uint8_t cfgidx);
static uint8_t  usbd_bulk_Setup       (void  *pdev, USB_SETUP_REQ *req);
static uint8_t  usbd_bulk_DataIn      (void *pdev, uint8_t epnum);
static uint8_t  usbd_bulk_DataOut     (void *pdev, uint8_t epnum);
static uint8_t  usbd_bulk_SOF         (void *pdev);

/*********************************************
   BULK specific management functions
 *********************************************/
static uint8_t  *USBD_bulk_GetCfgDesc (uint8_t speed, uint16_t *length);
/**
  * @}
  */ 


/** @defgroup usbd_bulk_Private_Variables
  * @{
  */ 
__ALIGN_BEGIN static volatile uint32_t  usbd_bulk_AltSet  __ALIGN_END = 0;

__ALIGN_BEGIN uint8_t USB_Rx_Buffer[BULK_DATA_MAX_PACKET_SIZE] __ALIGN_END ;


/* Bulk interface class callbacks structure */
USBD_Class_cb_TypeDef  USBD_bulk_cb = 
{
  usbd_bulk_Init,
  usbd_bulk_DeInit,
  usbd_bulk_Setup,
  NULL,                 
  NULL,
  usbd_bulk_DataIn,
  usbd_bulk_DataOut,
  usbd_bulk_SOF,
  NULL,
  NULL,     
  USBD_bulk_GetCfgDesc
};


/* USB CDC device Configuration Descriptor */
__ALIGN_BEGIN uint8_t usbd_bulk_CfgDesc[USB_BULK_CONFIG_DESC_SIZ]  __ALIGN_END =
{
  /*Configuration Descriptor*/
  0x09,   /* bLength: Configuration Descriptor size */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* bDescriptorType: Configuration */
  USB_BULK_CONFIG_DESC_SIZ,       /* wTotalLength:no of returned bytes */
  0x00,
  0x01,   /* bNumInterfaces: 1 interface */
  0x01,   /* bConfigurationValue: Configuration value */
  0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
  0xC0,   /* bmAttributes: self powered */
  0x32,   /* MaxPower 100 mA */
  /*---------------------------------------------------------------------------*/
  /*Interface Descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints: Two endpoints used */
  0xFF,   /* bInterfaceClass: vendor specific */
  0x00,   /* bInterfaceSubClass */
  0x00,   /* bInterfaceProtocol */
  0x00,   /* iInterface: */
  /*Endpoint 1 IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
  BULK_IN_EP,   /* bEndpointAddress: (IN1) */
  0x02,   /* bmAttributes: Bulk */
  LOBYTE(BULK_DATA_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(BULK_DATA_MAX_PACKET_SIZE),
  0xFF,    /* bInterval: ignore for Bulk transfer */
  /*Endpoint 1 OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
  BULK_OUT_EP,   /* bEndpointAddress: (OUT1) */
  0x02,   /* bmAttributes: Bulk */
  LOBYTE(BULK_DATA_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(BULK_DATA_MAX_PACKET_SIZE),
  0xFF   /* bInterval: ignore for Bulk transfer */
} ;


/**
  * @}
  */ 

/** @defgroup usbd_bulk_Private_Functions
  * @{
  */ 

/**
  * @brief  usbd_bulk_Init
  *         Initilaize the Bulk interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  usbd_bulk_Init (void  *pdev, 
                               uint8_t cfgidx)
{
  /* Open EP IN */
  DCD_EP_Open(pdev,
              BULK_IN_EP,
              BULK_DATA_IN_PACKET_SIZE,
              USB_OTG_EP_BULK);
  
  /* Open EP OUT */
  DCD_EP_Open(pdev,
              BULK_OUT_EP,
              BULK_DATA_OUT_PACKET_SIZE,
              USB_OTG_EP_BULK);
  
  
  /* Prepare Out endpoint to receive next packet */
  DCD_EP_PrepareRx(pdev,
                   BULK_OUT_EP,
                   (uint8_t*)(USB_Rx_Buffer),
                   BULK_DATA_OUT_PACKET_SIZE);
  
  return USBD_OK;
}

/**
  * @brief  usbd_bulk_DeInit
  *         DeInitialize the layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  usbd_bulk_DeInit (void  *pdev, 
                                 uint8_t cfgidx)
{
  /* Open EP IN */
  DCD_EP_Close(pdev, BULK_IN_EP);
  
  /* Open EP OUT */
  DCD_EP_Close(pdev, BULK_OUT_EP);

  return USBD_OK;
}

/**
  * @brief  usbd_bulk_Setup
  *         Handle the bulk specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  usbd_bulk_Setup (void  *pdev, 
                                USB_SETUP_REQ *req)
{
  uint16_t len=USB_BULK_DESC_SIZ;
  uint8_t  *pbuf=usbd_bulk_CfgDesc + 9;
  
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    /* Standard Requests -------------------------------*/
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_DESCRIPTOR: 
      if( (req->wValue >> 8) == BULK_DESCRIPTOR_TYPE)
      {
        pbuf = usbd_bulk_CfgDesc + 9 + (9 * USBD_ITF_MAX_NUM);
        len = MIN(USB_BULK_DESC_SIZ , req->wLength);
      }
      
      USBD_CtlSendData (pdev, 
                        pbuf,
                        len);
      break;
      
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        (uint8_t *)&usbd_bulk_AltSet,
                        1);
      break;
      
    case USB_REQ_SET_INTERFACE :
      if ((uint8_t)(req->wValue) < USBD_ITF_MAX_NUM)
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
  }
  return USBD_OK;
}


/**
  * @brief  usbd_bulk_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  usbd_bulk_DataIn (void *pdev, uint8_t epnum)
{
  /* endpoint finished the previous transmission so see if more data is left */
  UsbTransmitPipeBulkIN();
  return USBD_OK;
}

/**
  * @brief  usbd_bulk_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  usbd_bulk_DataOut (void *pdev, uint8_t epnum)
{      
  /* read the data from the bulk OUT pipe */
  UsbReceivePipeBulkOUT(epnum);
  return USBD_OK;
}


/**
  * @brief  usbd_buld_get_rx_buffer_ptr
  *         Get pointer to the USB rx buffer
  * @retval Buffer pointer
  */
uint8_t *usbd_bulk_get_rx_buffer_ptr(void)
{
  return USB_Rx_Buffer;
}


/**
  * @brief  usbd_bulk_SOF
  *         Start Of Frame event management
  * @param  pdev: instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  usbd_bulk_SOF (void *pdev)
{      
  if (((USB_OTG_CORE_HANDLE*)pdev)->dev.device_status == USB_OTG_CONFIGURED)
  {
    /* Check the data to be sent through IN pipe */
    UsbTransmitPipeBulkIN();
  }
  return USBD_OK;
}


/**
  * @brief  USBD_bulk_GetCfgDesc 
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_bulk_GetCfgDesc (uint8_t speed, uint16_t *length)
{
  *length = sizeof (usbd_bulk_CfgDesc);
  return usbd_bulk_CfgDesc;
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
