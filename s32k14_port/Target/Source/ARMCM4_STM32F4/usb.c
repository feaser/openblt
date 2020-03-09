/************************************************************************************//**
* \file         Source/ARMCM4_STM32F4/usb.c
* \brief        Bootloader USB communication interface source file.
* \ingroup      Target_ARMCM4_STM32F4
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
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
#include "boot.h"                                     /* bootloader generic header     */
#if (BOOT_COM_USB_ENABLE > 0)
#include "usb.h"                                      /* USB bootloader driver         */
#include "usbd_core.h"                                /* USB driver core               */
#include "usbd_desc.h"                                /* USB driver descriptor         */
#include "usbd_bulk.h"                                /* USB driver bulk device        */


/****************************************************************************************
* Configuration macros
****************************************************************************************/
/* For a USB bootloader, the backdoor needs to stay open long enough for the USB device
 * to enumerate on the host PC. Therefore the default backdoor open time needs to be
 * extended. Note that this won't be long enough for a first time USB driver install
 * on the host PC. In this case the bootloader should be started with the backup backdoor
 * that uses, for example, a digital input to force the bootloader to stay active. This
 * can be implemented in CpuUserProgramStartHook(). Feel free to shorten/lengthen this
 * time for finetuning. Note that adding this configuration macro to blt_conf.h overrides
 * the value here.
 */
#ifndef BOOT_COM_USB_BACKDOOR_EXTENSION_MS
#define BOOT_COM_USB_BACKDOOR_EXTENSION_MS   (2000)
#endif


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Total number of fifo buffers. */
#define FIFO_MAX_BUFFERS         (2)
/** \brief Invalid value for a fifo buffer handle. */
#define FIFO_ERR_INVALID_HANDLE  (255)
/** \brief Number of bytes that fit in the fifo pipe. */
#define FIFO_PIPE_SIZE           (64)


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Structure type for fifo control. */
typedef struct t_fifo_ctrl
{
  blt_int8u          *startptr;                    /**< pointer to start of buffer     */
  blt_int8u          *endptr;                      /**< pointer to end of buffer       */
  blt_int8u          *readptr;                     /**< pointer to next read location  */
  blt_int8u          *writeptr;                    /**< pointer to next free location  */
  blt_int8u           length;                      /**< number of buffer elements      */
  blt_int8u           entries;                     /**< # of full buffer elements      */
  blt_int8u           handle;                      /**< handle of the buffer           */
  struct t_fifo_ctrl *fifoctrlptr;                 /**< pointer to free buffer control */
} tFifoCtrl;

/** \brief Structure type for a fifo pipe. */
typedef struct
{
  blt_int8u handle;                                /**< fifo handle                    */
  blt_int8u data[FIFO_PIPE_SIZE];                  /**< fifo data buffer               */
} tFifoPipe;                                       /**< USB pipe fifo type             */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static blt_bool  UsbReceiveByte(blt_int8u *data);
static blt_bool  UsbTransmitByte(blt_int8u data);
static void      UsbFifoMgrInit(void);
static blt_int8u UsbFifoMgrCreate(blt_int8u *buffer, blt_int8u length);
static blt_bool  UsbFifoMgrWrite(blt_int8u handle, blt_int8u data);
static blt_bool  UsbFifoMgrRead(blt_int8u handle, blt_int8u *data);
static blt_int8u UsbFifoMgrScan(blt_int8u handle);


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Local variable that holds the fifo control structures. */
static tFifoCtrl  fifoCtrl[FIFO_MAX_BUFFERS];
/** \brief Local pointer that points to the next free fifo control structure. */
static tFifoCtrl *fifoCtrlFree;
/** \brief Fifo pipe used for the bulk in endpoint. */
static tFifoPipe  fifoPipeBulkIN;
/** \brief Fifo pipe used for the bulk out endpoint. */
static tFifoPipe  fifoPipeBulkOUT;
/** \brief USB device handle. */
static USBD_HandleTypeDef hUsbDeviceFS;


/************************************************************************************//**
** \brief     Initializes the USB communication interface.
** \return    none.
**
****************************************************************************************/
void UsbInit(void)
{
  /* initialize the FIFO manager */
  UsbFifoMgrInit();
  /* place 2 buffers under FIFO management */
  fifoPipeBulkIN.handle  = UsbFifoMgrCreate(fifoPipeBulkIN.data,  FIFO_PIPE_SIZE);
  fifoPipeBulkOUT.handle = UsbFifoMgrCreate(fifoPipeBulkOUT.data, FIFO_PIPE_SIZE);
  /* validate fifo handles */
  ASSERT_RT((fifoPipeBulkIN.handle  != FIFO_ERR_INVALID_HANDLE) && \
            (fifoPipeBulkOUT.handle != FIFO_ERR_INVALID_HANDLE));
  /* initialize the USB device libary */
  USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);
  /* register the bootloader's custom USB Bulk based class */
  USBD_RegisterClass(&hUsbDeviceFS, &USBD_Bulk);
  /* inform application about the connect event */
  UsbConnectHook(BLT_TRUE);
  /* start the USB device */
  USBD_Start(&hUsbDeviceFS);
  /* perform low level connect of the device */
  HAL_PCD_DevConnect((PCD_HandleTypeDef *)hUsbDeviceFS.pData);
  /* extend the time that the backdoor is open in case the default timed backdoor
   * mechanism is used.
   */
#if (BOOT_BACKDOOR_HOOKS_ENABLE == 0)
  if (BackDoorGetExtension() < BOOT_COM_USB_BACKDOOR_EXTENSION_MS)
  {
    BackDoorSetExtension(BOOT_COM_USB_BACKDOOR_EXTENSION_MS);
  }
#endif /* BOOT_BACKDOOR_HOOKS_ENABLE == 0 */
} /*** end of UsbInit ***/


/************************************************************************************//**
** \brief     Releases the USB communication interface.
** \return    none.
**
****************************************************************************************/
void UsbFree(void)
{
  /* perform low level disconnect of the device */
  HAL_PCD_DevDisconnect((PCD_HandleTypeDef *)hUsbDeviceFS.pData);
  /* uninitialize the device */
  USBD_DeInit(&hUsbDeviceFS);
  /* inform application about the disconnect event */
  UsbConnectHook(BLT_FALSE);
} /*** end of UsbFree ***/


/************************************************************************************//**
** \brief     Transmits a packet formatted for the communication interface.
** \param     data Pointer to byte array with data that it to be transmitted.
** \param     len  Number of bytes that are to be transmitted.
** \return    none.
**
****************************************************************************************/
void UsbTransmitPacket(blt_int8u *data, blt_int8u len)
{
  blt_int16u data_index;
  blt_bool result;

  /* verify validity of the len-paramenter */
  ASSERT_RT(len <= BOOT_COM_USB_TX_MAX_DATA);

  /* first transmit the length of the packet */
  result = UsbTransmitByte(len);
  ASSERT_RT(result == BLT_TRUE);

  /* transmit all the packet bytes one-by-one */
  for (data_index = 0; data_index < len; data_index++)
  {
    /* keep the watchdog happy */
    CopService();
    /* write byte */
    result = UsbTransmitByte(data[data_index]);
    ASSERT_RT(result == BLT_TRUE);
  }
} /*** end of UsbTransmitPacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface packet if one is present.
** \param     data Pointer to byte array where the data is to be stored.
** \param     len Pointer where the length of the packet is to be stored.
** \return    BLT_TRUE if a packet was received, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool UsbReceivePacket(blt_int8u *data, blt_int8u *len)
{
  static blt_int8u xcpCtoReqPacket[BOOT_COM_USB_RX_MAX_DATA+1];  /* one extra for length */
  static blt_int8u xcpCtoRxLength;
  static blt_bool  xcpCtoRxInProgress = BLT_FALSE;

  /* poll USB interrupt flags to process USB related events */
  HAL_PCD_IRQHandler((PCD_HandleTypeDef *)hUsbDeviceFS.pData);

  /* start of cto packet received? */
  if (xcpCtoRxInProgress == BLT_FALSE)
  {
    /* store the message length when received */
    if (UsbReceiveByte(&xcpCtoReqPacket[0]) == BLT_TRUE)
    {
      if ( (xcpCtoReqPacket[0] > 0) &&
           (xcpCtoReqPacket[0] <= BOOT_COM_USB_RX_MAX_DATA) )
      {
        /* indicate that a cto packet is being received */
        xcpCtoRxInProgress = BLT_TRUE;
        /* reset packet data count */
        xcpCtoRxLength = 0;
      }
    }
  }
  else
  {
    /* store the next packet byte */
    if (UsbReceiveByte(&xcpCtoReqPacket[xcpCtoRxLength+1]) == BLT_TRUE)
    {
      /* increment the packet data count */
      xcpCtoRxLength++;

      /* check to see if the entire packet was received */
      if (xcpCtoRxLength == xcpCtoReqPacket[0])
      {
        /* copy the packet data */
        CpuMemCopy((blt_int32u)data, (blt_int32u)&xcpCtoReqPacket[1], xcpCtoRxLength);
        /* done with cto packet reception */
        xcpCtoRxInProgress = BLT_FALSE;
        /* set the packet length */
        *len = xcpCtoRxLength;
        /* packet reception complete */
        return BLT_TRUE;
      }
    }
  }
  /* packet reception not yet complete */
  return BLT_FALSE;
} /*** end of UsbReceivePacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface byte if one is present.
** \param     data Pointer to byte where the data is to be stored.
** \return    BLT_TRUE if a byte was received, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool UsbReceiveByte(blt_int8u *data)
{
  blt_bool result;

  /* obtain data from the fifo */
  result = UsbFifoMgrRead(fifoPipeBulkOUT.handle, data);
  return result;
} /*** end of UsbReceiveByte ***/


/************************************************************************************//**
** \brief     Transmits a communication interface byte.
** \param     data Value of byte that is to be transmitted.
** \return    BLT_TRUE if the byte was transmitted, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool UsbTransmitByte(blt_int8u data)
{
  blt_bool result;

  /* write data from to fifo */
  result = UsbFifoMgrWrite(fifoPipeBulkIN.handle, data);
  return result;
} /*** end of UsbTransmitByte ***/


/************************************************************************************//**
** \brief     Checks if there is still data left to transmit and if so submits it
**            for transmission with the USB endpoint.
** \return    none.
**
****************************************************************************************/
void UsbTransmitPipeBulkIN(void)
{
  /* USB_Tx_Buffer is static for run-time optimalization */
  static uint8_t USB_Tx_Buffer[BULK_DATA_MAX_PACKET_SIZE];
  blt_int8u nr_of_bytes_for_tx_endpoint;
  blt_int8u byte_counter;
  blt_int8u byte_value;
  blt_bool  result;

  /* read how many bytes should be transmitted */
  nr_of_bytes_for_tx_endpoint = UsbFifoMgrScan(fifoPipeBulkIN.handle);
  /* only continue if there is actually data left to transmit */
  if (nr_of_bytes_for_tx_endpoint == 0)
  {
    return;
  }
  /* make sure to not transmit more than the USB endpoint can handle */
  if (nr_of_bytes_for_tx_endpoint > BULK_DATA_MAX_PACKET_SIZE)
  {
    nr_of_bytes_for_tx_endpoint = BULK_DATA_MAX_PACKET_SIZE;
  }
  /* copy the transmit data to the transmit buffer */
  for (byte_counter=0; byte_counter < nr_of_bytes_for_tx_endpoint; byte_counter++)
  {
    /* obtain data from the fifo */
    result = UsbFifoMgrRead(fifoPipeBulkIN.handle, &byte_value);
    ASSERT_RT(result == BLT_TRUE);
    /* store it in the endpoint's RAM */
    USB_Tx_Buffer[byte_counter] = byte_value;
  }
  /* copy data to endpoint's RAM and start the transmission */
  USBD_LL_Transmit(&hUsbDeviceFS, BULK_IN_EP, &USB_Tx_Buffer[0],
                   nr_of_bytes_for_tx_endpoint);
} /*** end of UsbTransmitPipeBulkIN ***/


/************************************************************************************//**
** \brief     Stores data that was received on the Bulk OUT pipe in the fifo.
** \return    none.
**
****************************************************************************************/
void UsbReceivePipeBulkOUT(blt_int8u epnum)
{
  blt_int16u USB_Rx_Cnt=0;
  blt_int8u *usbRxBufferPtr;
  blt_int16u byte_counter;
  blt_bool result;

  /* Get the received data buffer and the number of received bytes */
  usbRxBufferPtr = USBD_Bulk_GetRxBufferPtr();
  USB_Rx_Cnt = USBD_LL_GetRxDataSize(&hUsbDeviceFS, epnum);

  /* USB data will be immediately processed, this allow next USB traffic being
   * NAKed till the end of the USART Xfer
   */
  for (byte_counter=0; byte_counter<USB_Rx_Cnt; byte_counter++)
  {
    /* add the data to the fifo */
    result = UsbFifoMgrWrite(fifoPipeBulkOUT.handle, usbRxBufferPtr[byte_counter]);
    /* verify that the fifo wasn't full */
    ASSERT_RT(result == BLT_TRUE);
  }
  /* Prepare Out endpoint to receive next packet */
  USBD_LL_PrepareReceive(&hUsbDeviceFS,
                         BULK_OUT_EP,
                         USBD_Bulk_GetRxBufferPtr(),
                         BULK_DATA_FS_OUT_PACKET_SIZE);
} /*** end of UsbReceivePipeBulkOUT ***/


/************************************************************************************//**
** \brief     Initializes the fifo manager. Each controlled fifo is assigned a
**            unique handle, which is the same as its index into fifoCtrl[]. Each
**            controlled fifo holds a pointer to the next free fifo control.
**            For the last fifo in fifoCtrl[] this one is set to a null-pointer as
**            an out of fifo control indicator. Function should be called once
**            before any of the other fifo management functions are called.
** \return    none.
**
****************************************************************************************/
static void UsbFifoMgrInit(void)
{
  blt_int8u i;
  tFifoCtrl *pbc1, *pbc2;

  pbc1 = &fifoCtrl[0];
  pbc2 = &fifoCtrl[1];
  /* assign fifo handles and pointer to next free fifo */
  for (i = 0; i < (FIFO_MAX_BUFFERS - 1); i++)
  {
    pbc1->handle = i;
    pbc1->fifoctrlptr = pbc2;
    pbc1++;
    pbc2++;
  }
  /* initialize handle for the last one and use null-pointer for the next free fifo  */
  pbc1->handle = i;
  pbc1->fifoctrlptr = (tFifoCtrl *)0;
  fifoCtrlFree = &fifoCtrl[0];
} /*** end of UsbFifoMgrInit ***/


/************************************************************************************//**
** \brief     Places a data storage array under fifo management control. A handle
**            for identifying the fifo in subsequent fifo management function
**            calls is returned, if successful.
** \param     buffer Pointer to the first element in the data storage fifo.
** \param     length Maximum number of data elements that can be stored in the fifo.
** \return    Fifo handle if successfull, or FIFO_ERR_INVALID_HANDLE.
**
****************************************************************************************/
static blt_int8u UsbFifoMgrCreate(blt_int8u *buffer, blt_int8u length)
{
  tFifoCtrl *pbc;

  /* first determine if these is still a free fifo control available */
  if (fifoCtrlFree == (tFifoCtrl *)0)
  {
    return FIFO_ERR_INVALID_HANDLE;
  }
  /* store pointer to free fifo and update pointer to next free one */
  pbc = fifoCtrlFree;
  fifoCtrlFree = pbc->fifoctrlptr;

  /* initialize the buffer control */
  pbc->length = length;
  pbc->readptr = buffer;
  pbc->writeptr = buffer;
  pbc->entries = 0;
  pbc->startptr = buffer;
  pbc->endptr = (blt_int8u *)(buffer + length - 1);

  /* return the handle to the successfully created fifo control */
  return pbc->handle;
} /*** end of UsbFifoMgrCreate ***/


/************************************************************************************//**
** \brief     Stores data in the fifo.
** \param     handle Identifies the fifo to write data to.
** \param     data   Pointer to the data that is to be written to the fifo.
** \return    BLT_TRUE if the data was successfully stored in the fifo, BLT_FALSE
**            otherwise.
**
****************************************************************************************/
static blt_bool UsbFifoMgrWrite(blt_int8u handle, blt_int8u data)
{
  /* check the validity of the handle parameter */
  ASSERT_RT(handle < FIFO_MAX_BUFFERS);
  /* check if fifo is full */
  if (fifoCtrl[handle].entries == fifoCtrl[handle].length)
  {
    return BLT_FALSE;
  }
  /* copy data to fifo */
  *fifoCtrl[handle].writeptr = data;
  /* data written so update number of entries */
  fifoCtrl[handle].entries++;
  /* update write pointer */
  fifoCtrl[handle].writeptr++;
  /* check end of fifo */
  if (fifoCtrl[handle].writeptr > fifoCtrl[handle].endptr)
  {
    /* set write pointer to start of the cyclic fifo */
    fifoCtrl[handle].writeptr = fifoCtrl[handle].startptr;
  }
  /* still here so all is okay */
  return BLT_TRUE;
} /*** end of UsbFifoMgrWrite ***/


/************************************************************************************//**
** \brief     Retrieves data from the fifo.
** \param     handle Identifies the fifo to read data from.
** \param     data   Pointer to where the read data is to be stored.
** \return    BLT_TRUE if the data was successfully read from the fifo, BLT_FALSE
**            otherwise.
**
****************************************************************************************/
static blt_bool UsbFifoMgrRead(blt_int8u handle, blt_int8u *data)
{
  /* check the validity of the handle parameter */
  ASSERT_RT(handle < FIFO_MAX_BUFFERS);
  /* check if fifo is empty */
  if (fifoCtrl[handle].entries == 0)
  {
    return BLT_FALSE;
  }
  /* read the data */
  *data = *fifoCtrl[handle].readptr;
  /* data read so update number of entries */
  fifoCtrl[handle].entries--;
  /* update read pointer */
  fifoCtrl[handle].readptr++;
  /* check end of fifo */
  if (fifoCtrl[handle].readptr > fifoCtrl[handle].endptr)
  {
    /* set read pointer to start of the cyclic fifo */
    fifoCtrl[handle].readptr = fifoCtrl[handle].startptr;
  }
  /* still here so all is good */
  return BLT_TRUE;
} /*** end of UsbFifoMgrRead ***/


/************************************************************************************//**
** \brief     Returns the number of data entries currently present in the fifo.
** \param     handle Identifies the fifo that is to be scanned.
** \return    Number of data entries in the fifo if successful, otherwise 0.
**
****************************************************************************************/
static blt_int8u UsbFifoMgrScan(blt_int8u handle)
{
  /* check the validity of the handle parameter */
  ASSERT_RT(handle < FIFO_MAX_BUFFERS);
  /* read and return the number of data entries */
  return fifoCtrl[handle].entries;
} /*** end of UsbFifoMgrScan ***/
#endif /* BOOT_COM_USB_ENABLE > 0 */


/*********************************** end of usb.c **************************************/
