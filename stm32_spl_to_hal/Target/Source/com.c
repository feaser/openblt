/************************************************************************************//**
* \file         Source\com.c
* \brief        Bootloader communication interface source file.
* \ingroup      Core
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
#include "boot.h"                                /* bootloader generic header          */
#if (BOOT_COM_CAN_ENABLE > 0)
#include "can.h"                                    /* can driver module             */
#endif
#if (BOOT_COM_UART_ENABLE > 0)
#include "uart.h"                                   /* uart driver module            */
#endif
#if (BOOT_COM_USB_ENABLE > 0)
#include "usb.h"                                    /* usb driver module             */
#endif
#if (BOOT_COM_NET_ENABLE > 0)
#include "net.h"                                    /* tcp/ip driver module          */
#endif


#if (BOOT_COM_ENABLE > 0)
/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Holds the communication interface of the currently active interface. */
static tComInterfaceId comActiveInterface = COM_IF_OTHER;


/************************************************************************************//**
** \brief     Initializes the communication module including the hardware needed for
**            the communication.
** \return    none
**
****************************************************************************************/
void ComInit(void)
{
  /* initialize the XCP communication protocol */
  XcpInit();
#if (BOOT_COM_CAN_ENABLE > 0)
  /* initialize the CAN controller */
  CanInit();
  /* set it as active */
  comActiveInterface = COM_IF_CAN;
#endif
#if (BOOT_COM_UART_ENABLE > 0)
  /* initialize the UART interface */
  UartInit();
  /* set it as active */
  comActiveInterface = COM_IF_UART;
#endif
#if (BOOT_COM_USB_ENABLE > 0)
  /* initialize the USB interface */
  UsbInit();
  /* set it as active */
  comActiveInterface = COM_IF_USB;
#endif
#if (BOOT_COM_NET_ENABLE > 0)
  /* initialize the TCP/IP interface */
  NetInit();
  /* set it as active */
  comActiveInterface = COM_IF_NET;
#endif
} /*** end of ComInit ***/


/************************************************************************************//**
** \brief     Updates the communication module by checking if new data was received
**            and submitting the request to process newly received data.
** \return    none
**
****************************************************************************************/
void ComTask(void)
{
  blt_int8u xcpPacketLen;
  /* make xcpCtoReqPacket static for runtime efficiency */
  static blt_int8u xcpCtoReqPacket[BOOT_COM_RX_MAX_DATA];

#if (BOOT_COM_CAN_ENABLE > 0)
  if (CanReceivePacket(&xcpCtoReqPacket[0], &xcpPacketLen) == BLT_TRUE)
  {
    /* make this the active interface */
    comActiveInterface = COM_IF_CAN;
    /* process packet */
    XcpPacketReceived(&xcpCtoReqPacket[0], xcpPacketLen);
  }
#endif
#if (BOOT_COM_UART_ENABLE > 0)
  if (UartReceivePacket(&xcpCtoReqPacket[0], &xcpPacketLen) == BLT_TRUE)
  {
    /* make this the active interface */
    comActiveInterface = COM_IF_UART;
    /* process packet */
    XcpPacketReceived(&xcpCtoReqPacket[0], xcpPacketLen);
  }
#endif
#if (BOOT_COM_USB_ENABLE > 0)
  if (UsbReceivePacket(&xcpCtoReqPacket[0], &xcpPacketLen) == BLT_TRUE)
  {
    /* make this the active interface */
    comActiveInterface = COM_IF_USB;
    /* process packet */
    XcpPacketReceived(&xcpCtoReqPacket[0], xcpPacketLen);
  }
#endif
#if (BOOT_COM_NET_ENABLE > 0)
  if (NetReceivePacket(&xcpCtoReqPacket[0], &xcpPacketLen) == BLT_TRUE)
  {
    /* make this the active interface */
    comActiveInterface = COM_IF_NET;
    /* process packet */
    XcpPacketReceived(&xcpCtoReqPacket[0], xcpPacketLen);
  }
#endif
} /*** end of ComTask ***/


/************************************************************************************//**
** \brief     Releases the communication module.
** \return    none
**
****************************************************************************************/
void ComFree(void)
{
#if (BOOT_COM_USB_ENABLE > 0)
  /* disconnect the usb device from the usb host */
  UsbFree();
#endif
} /*** end of ComFree ***/


/************************************************************************************//**
** \brief     Transmits the packet using the xcp transport layer.
** \param     data Pointer to the byte buffer with packet data.
** \param     len  Number of data bytes that need to be transmitted.
** \return    none
**
****************************************************************************************/
void ComTransmitPacket(blt_int8u *data, blt_int16u len)
{
#if (BOOT_COM_CAN_ENABLE > 0)
  /* transmit the packet. note that len is limited to 8 in the plausibility check,
   * so cast is okay.
   */
  if (comActiveInterface == COM_IF_CAN)
  {
    CanTransmitPacket(data, (blt_int8u)len);
  }
#endif
#if (BOOT_COM_UART_ENABLE > 0)
  /* transmit the packet. note that len is limited to 255 in the plausibility check,
   * so cast is okay.
   */
  if (comActiveInterface == COM_IF_UART)
  {
    UartTransmitPacket(data, (blt_int8u)len);
  }
#endif
#if (BOOT_COM_USB_ENABLE > 0)
  /* transmit the packet */
  if (comActiveInterface == COM_IF_USB)
  {
    UsbTransmitPacket(data, len);
  }
#endif
#if (BOOT_COM_NET_ENABLE > 0)
  if (comActiveInterface == COM_IF_NET)
  {
    /* transmit the packet */
    NetTransmitPacket(data, len);
  }
#endif

  /* send signal that the packet was transmitted */
  XcpPacketTransmitted();
} /*** end of ComTransmitPacket ***/


/************************************************************************************//**
** \brief     Obtains the maximum number of bytes that can be received on the specified
**            communication interface.
** \return    Maximum number of bytes that can be received.
**
****************************************************************************************/
blt_int16u ComGetActiveInterfaceMaxRxLen(void)
{
  blt_int16u result;

  /* filter on communication interface identifier */
  switch (comActiveInterface)
  {
    case COM_IF_UART:
      result = BOOT_COM_UART_RX_MAX_DATA;
      break;

    case COM_IF_CAN:
      result = BOOT_COM_CAN_RX_MAX_DATA;
      break;

    case COM_IF_USB:
      result = BOOT_COM_USB_RX_MAX_DATA;
      break;

    case COM_IF_NET:
      result = BOOT_COM_NET_RX_MAX_DATA;
      break;

    default:
      result = BOOT_COM_RX_MAX_DATA;
      break;
  }

  return result;
} /*** end of ComGetActiveInterfaceMaxRxLen ***/


/************************************************************************************//**
** \brief     Obtains the maximum number of bytes that can be transmitted on the
**            specified communication interface.
** \return    Maximum number of bytes that can be received.
**
****************************************************************************************/
blt_int16u ComGetActiveInterfaceMaxTxLen(void)
{
  blt_int16u result;

  /* filter on communication interface identifier */
  switch (comActiveInterface)
  {
    case COM_IF_UART:
      result = BOOT_COM_UART_TX_MAX_DATA;
      break;

    case COM_IF_CAN:
      result = BOOT_COM_CAN_TX_MAX_DATA;
      break;

    case COM_IF_USB:
      result = BOOT_COM_USB_TX_MAX_DATA;
      break;

    case COM_IF_NET:
      result = BOOT_COM_NET_TX_MAX_DATA;
      break;

    default:
      result = BOOT_COM_TX_MAX_DATA;
      break;
  }

  return result;
} /*** end of ComGetActiveInterfaceMaxTxLen ***/


/************************************************************************************//**
** \brief     This function obtains the XCP connection state.
** \return    BLT_TRUE when an XCP connection is established, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool ComIsConnected(void)
{
  return XcpIsConnected();
} /*** end of ComIsConnected ***/

#endif /* BOOT_COM_ENABLE > 0 */

/*********************************** end of com.c **************************************/
