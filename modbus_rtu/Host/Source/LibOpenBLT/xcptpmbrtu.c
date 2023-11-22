/************************************************************************************//**
* \file         xcptpmbrtu.c
* \brief        XCP Modbus RTU transport layer source file.
* \ingroup      XcpTpMbRtu
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2022  by Feaser    http://www.feaser.com    All rights reserved
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
#include <assert.h>                         /* for assertions                          */
#include <stdint.h>                         /* for standard integer types              */
#include <stddef.h>                         /* for NULL declaration                    */
#include <stdbool.h>                        /* for boolean type                        */
#include <stdlib.h>                         /* for standard library                    */
#include <string.h>                         /* for string library                      */
#include "session.h"                        /* Communication session module            */
#include "xcploader.h"                      /* XCP loader module                       */
#include "xcptpmbrtu.h"                     /* XCP UART transport layer                */
#include "util.h"                           /* Utility module                          */
#include "serialport.h"                     /* Serial port module                      */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief This module embeds the bootloader's XCP communication packets inside Modbus
 *         RTU communication packets. As this is a non-standard functionality of Modbus,
 *         a user-defined function code is used to extend the Modbus functionality for
 *         the purpose of embedded XCP packets. User-defined function codes are allowed,
 *         as long as they are in the range 65..72 or 100..110. By default, the user-
 *         defined function code 109 is selected. Note that some other Modbus device on
 *         the network might also already use this. In this case set this macro to
 *         another unique user-defined function code.
 */
#define XCP_TP_MBRTU_FCT_CODE_USER_XCP  (109u)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void XcpTpMbRtuInit(void const * settings);
static void XcpTpMbRtuTerminate(void);
static bool XcpTpMbRtuConnect(void);
static void XcpTpMbRtuDisconnect(void);
static bool XcpTpMbRtuSendPacket(tXcpTransportPacket const * txPacket, 
                                 tXcpTransportPacket * rxPacket, uint16_t timeout);
static uint16_t XcpTpMbRtuCrcCalculate(uint8_t const * data, uint16_t len);

 
/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief XCP transport layer structure filled with Modbus RTU specifics. */
static const tXcpTransport mbRtuTransport =
{
  XcpTpMbRtuInit,
  XcpTpMbRtuTerminate,
  XcpTpMbRtuConnect,
  XcpTpMbRtuDisconnect,
  XcpTpMbRtuSendPacket
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The settings to use in this transport layer. */
static tXcpTpMbRtuSettings tpMbRtuSettings;

/** \brief Modbus RTU 3.5 character time in milliseconds. In the baudrate is > 19200
 *         bits/sec the T3_5 can be fixed at 1750 us. This is 2 ms when rounded to
 *         milliseconds. Use this as the initial value and adjust later on in case of a
 *         baudrate <= 19200 bps. Make sure to add 1 ms to adjust for millisecond 
 *         resolution inaccuracy.
 */
static uint32_t tpMbRtuT3_5Ms = 2 + 1;


/***********************************************************************************//**
** \brief     Obtains a pointer to the transport layer structure, so that it can be 
**            linked to the XCP protocol module.
** \return    Pointer to transport layer structure.
**
****************************************************************************************/
tXcpTransport const * XcpTpMbRtuGetTransport(void)
{
  return &mbRtuTransport;
} /*** end of XcpTpMbRtuGetTransport ***/


/************************************************************************************//**
** \brief     Initializes the transport layer.
** \param     settings Pointer to settings structure.
** \return    None.
**
****************************************************************************************/
static void XcpTpMbRtuInit(void const * settings)
{
  char * mbRtuPortName;

  /* Reset transport layer settings. */
  tpMbRtuSettings.portname = NULL;
  tpMbRtuSettings.baudrate = 0;
  tpMbRtuSettings.stopbits = 1;   /* Default 1 stopbit according to Modbus protocol.   */
  tpMbRtuSettings.parity = 2;     /* Default even parity according to Modbus protocol. */
  tpMbRtuSettings.destinationAddr = 1;
  
  /* Check parameters. */
  assert(settings != NULL);

  /* Only continue with valid parameters. */
  if (settings != NULL) /*lint !e774 */
  {
    /* Shallow copy the transport layer settings for layer usage. */
    tpMbRtuSettings = *((tXcpTpMbRtuSettings *)settings);
    /* The portname is a pointer and it is not guaranteed that it stays valid so we need
     * to deep copy this one. note the +1 for '\0' in malloc.
     */
    assert(((tXcpTpMbRtuSettings *)settings)->portname != NULL);
    if (((tXcpTpMbRtuSettings *)settings)->portname != NULL) /*lint !e774 */
    {
      mbRtuPortName = malloc(strlen(((tXcpTpMbRtuSettings *)settings)->portname) + 1);
      assert(mbRtuPortName != NULL);
      if (mbRtuPortName != NULL) /*lint !e774 */
      {
        strcpy(mbRtuPortName, ((tXcpTpMbRtuSettings *)settings)->portname);
        tpMbRtuSettings.portname = mbRtuPortName;
      }
    }
  }
  /* Initialize the serial port. */
  SerialPortInit();
  /* Recalculate the 3.5 character time, if needed. */
  if (tpMbRtuSettings.baudrate <= 19200)
  {
    /* T3_5 [ms] = 3.5 * Tchar = 3.5 * 11 * 1000 / baudrate = 38500 / baudrate.
     * Make sure to do integer round up though. Make sure to add 1 ms to adjust for
     * millisecond resolution inaccuracy.
     */
    tpMbRtuT3_5Ms = ((38500 + (tpMbRtuSettings.baudrate - 1)) / tpMbRtuSettings.baudrate) + 1;
  }
} /*** end of XcpTpMbRtuInit ***/


/************************************************************************************//**
** \brief     Terminates the transport layer.
**
****************************************************************************************/
static void XcpTpMbRtuTerminate(void)
{
  /* Terminate the serial port. */
  SerialPortTerminate();
  /* Release memory that was allocated for storing the port name. */
  if (tpMbRtuSettings.portname != NULL)
  {
    free((char *)tpMbRtuSettings.portname);
  }
  /* Reset transport layer settings. */
  tpMbRtuSettings.portname = NULL;
  tpMbRtuSettings.baudrate = 0;
  tpMbRtuSettings.stopbits = 1;   /* Default 1 stopbit according to Modbus protocol.   */
  tpMbRtuSettings.parity = 2;     /* Default even parity according to Modbus protocol. */
  tpMbRtuSettings.destinationAddr = 1;
} /*** end of XcpTpUartTerminate ***/


/************************************************************************************//**
** \brief     Connects to the transport layer.
** \return    True is connected, false otherwise.
**
****************************************************************************************/
static bool XcpTpMbRtuConnect(void)
{
  bool result = false;
  bool baudrateSupported;
  tSerialPortBaudrate baudrate;
  uint32_t idleTimeoutTime;
  uint32_t waitTimeoutTime;
  uint32_t currentTime;
  uint8_t rxByte = 0;
  tSerialPortParity parity;
  tSerialPortStopbits stopbits;
  
  /* Check if the specified baudrate is supported by the serial port driver. */
  baudrateSupported = (tpMbRtuSettings.baudrate == 9600) || 
                      (tpMbRtuSettings.baudrate == 19200) ||
                      (tpMbRtuSettings.baudrate == 38400) ||
                      (tpMbRtuSettings.baudrate == 57600) ||
                      (tpMbRtuSettings.baudrate == 115200);
                      
  /* Check transport layer settings. */
  assert(tpMbRtuSettings.portname != NULL);
  assert(baudrateSupported);
  assert(tpMbRtuSettings.parity <= 2);
  assert((tpMbRtuSettings.stopbits >= 1) && (tpMbRtuSettings.stopbits <= 2));
  assert((tpMbRtuSettings.destinationAddr >= 1) && (tpMbRtuSettings.destinationAddr <= 247));

  /* Only continue if the transport layer settings are valid. */
  if ( (tpMbRtuSettings.portname != NULL) && (baudrateSupported) && 
       (tpMbRtuSettings.parity <= 2) &&
       ((tpMbRtuSettings.stopbits >= 1) && (tpMbRtuSettings.stopbits <= 2)) &&
       ((tpMbRtuSettings.destinationAddr >= 1) && (tpMbRtuSettings.destinationAddr <= 247)) ) /*lint !e774 */
  {
    /* Convert the numeric baudrate to the one supported by the serial port driver. */
    switch (tpMbRtuSettings.baudrate) 
    {
      case 115200:
        baudrate = SERIALPORT_BR115200;
        break;
      case 57600:
        baudrate = SERIALPORT_BR57600;
        break;
      case 38400:
        baudrate = SERIALPORT_BR38400;
        break;
      case 19200:
        baudrate = SERIALPORT_BR19200;
        break;
      case 9600:
      default:
        baudrate = SERIALPORT_BR9600;
        break;
    }

    /* Set parity value. */
    switch (tpMbRtuSettings.parity)
    {
      case 0:
        parity = SERIALPORT_PARITY_NONE;
        break;
      case 1:
        parity = SERIALPORT_PARITY_ODD;
        break;
      case 2:
      default:
        parity = SERIALPORT_PARITY_EVEN;
        break;
    }

    /* Set stopbits value. */
    switch (tpMbRtuSettings.stopbits)
    {
    case 1:
    default:
      stopbits = SERIALPORT_STOPBITS1;
      break;
    case 2:
      stopbits = SERIALPORT_STOPBITS2;
      break;
    }

    /* Connect to the serial port. */
    result = SerialPortOpen(tpMbRtuSettings.portname, baudrate, parity, stopbits);
  }

  /* Only continue if successfully connected to the serial port. */
  if (result)
  {
    /* On Modbus RTU you need to guarantee that there is T3_5 idle time before starting
     * a new packet transmission or reception. Therefore we need to wait for an idle
     * state on the communication line here.
     */
    currentTime = UtilTimeGetSystemTimeMs();
    idleTimeoutTime = currentTime + tpMbRtuT3_5Ms;
    /* Add a second timeout to make sure this logic doesn't hang, in case the serial
     * line is flooded with data for some weird unexpected reason. It really shouldn't
     * take more a few hundred milliseconds for the idle line to occur.
     */
    waitTimeoutTime = currentTime + 500;

    /* Poll for T3_5 timeout, but restart when a character was received. */
    do
    {
      /* Get the current system time. */
      currentTime = UtilTimeGetSystemTimeMs();
      /* Check if a byte was received. */
      if (SerialPortRead(&rxByte, 1))
      {
        /* Transmission line not idle, so restart the 3.5 character timeout. */
        idleTimeoutTime = currentTime + tpMbRtuT3_5Ms;
      }
      /* Check if an overall timeout occurred. */
      if (currentTime >= waitTimeoutTime)
      {
        /* Could not detect an idle communication line. Flag the error, disconnected
         * the serial port again and stop the loop.
         */
        result = false;
        SerialPortClose();
        break;
      }
    } 
    while (currentTime < idleTimeoutTime);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of XcpTpMbRtuConnect ***/


/************************************************************************************//**
** \brief     Disconnects from the transport layer.
**
****************************************************************************************/
static void XcpTpMbRtuDisconnect(void)
{
  /* Disconnect from the serial port. */
  SerialPortClose();
} /*** end of XcpTpMbRtuDisconnect ***/


/************************************************************************************//**
** \brief     Transmits an XCP packet on the transport layer and attempts to receive the
**            response packet within the specified timeout.
** \param     txPacket Pointer to the packet to transmit.
** \param     rxPacket Pointer where the received packet info is stored.
** \param     timeout Maximum time in milliseconds to wait for the reception of the
**            response packet.
** \return    True is successful and a response packet was received, false otherwise.
**
****************************************************************************************/
static bool XcpTpMbRtuSendPacket(tXcpTransportPacket const * txPacket, 
                                tXcpTransportPacket * rxPacket, uint16_t timeout)
{
  bool     result = false;
  uint32_t idleTimeoutTime;
  uint32_t waitTimeoutTime = 0;
  uint32_t currentTime;
  uint16_t byteIdx;
  uint16_t checksumCalculated;
  uint16_t checksumReceived;
  uint16_t rxModbusPacketLen = 0;

  /* static to lower the stack load. +4 because of Modbus RTU packet overhead for
   * slave address, function code and 16-bit CRC. 
   */
  static uint8_t mbRtuBuffer[XCPLOADER_PACKET_SIZE_MAX + 5];

  /* On Modbus RTU, there must always be a T3_5 time separation between packets. Note
   * that an idle line detection was already performed, right after connecting to
   * the serial port.
   *
   * This bootloader uses XCP packets embedded in Modbus RTU packets. XCP is used in a
   * request / response manner, where no new request is sent before either a response
   * was received or a timeout occurred, waiting for the response to come it. 
   *
   * Theoretically both scenarios already guarantee the T3_5 idle time on the
   * communication link. However, the detection of the T3_5 end-of-packet time on a
   * non real-time OS such as Windows and Linux proves a bit troublesome. To get
   * this reliable, you would have to extend this time significantly (50 - 100 ms),
   * because the standard T3_5 is sometimes already detected between bytes of 
   * the packet itself. Simple because the SerialPort driver doesn't give you a
   * real-time timestamp of a received byte.
   * 
   * For this reason, this module does not implement the T3_5 end-of-packet time event.
   * Instead if uses length information embedded in the XCP on Modbus RTU packet to
   * determine if the packet was fully received.
   * 
   * Long story short: Before a new packet transmission, the T3_5 time should be added
   * to guarantee the T3_5 separation time between the last successful packet reception
   * and the next transmission.
   */

  /* Check parameters. */
  assert(txPacket != NULL);
  assert(rxPacket != NULL);

  /* Only continue with valid parameters. */
  if ((txPacket != NULL) && (rxPacket != NULL)) /*lint !e774 */
  {
    /* Set result value to okay and only change it from now on if an error occurred. */
    result = true;

    /* ------------ Guarantee idle time ---------------------------------------------- */
    /* Poll for T3_5 timeout. */
    idleTimeoutTime = UtilTimeGetSystemTimeMs() + tpMbRtuT3_5Ms;
    do
    {
      /* Get the current system time. */
      currentTime = UtilTimeGetSystemTimeMs();
    }
    while (currentTime < idleTimeoutTime);

    /* ------------ Packet transmission ---------------------------------------------- */
    /* Construct the Modbus RTU packet. Start by adding the slave address. */
    mbRtuBuffer[0] = tpMbRtuSettings.destinationAddr;
    /* Add the user-defined function code for embedding XCP packets. */
    mbRtuBuffer[1] = XCP_TP_MBRTU_FCT_CODE_USER_XCP;
    /* add the XCP packet length. */
    mbRtuBuffer[2] = txPacket->len;

    /* Copy the XCP packet data. */
    for (byteIdx = 0; byteIdx < txPacket->len; byteIdx++)
    {
      mbRtuBuffer[byteIdx + 3] = txPacket->data[byteIdx];
    }
    /* Calculate the checksum for the packet, including slave address, function code and
     * extra XCP length.
     */
    checksumCalculated = XcpTpMbRtuCrcCalculate(&mbRtuBuffer[0], txPacket->len + 3);
    /* add the checksum at the end of the packet */
    mbRtuBuffer[txPacket->len + 3] = (uint8_t)(checksumCalculated & 0xff);
    mbRtuBuffer[txPacket->len + 4] = (uint8_t)(checksumCalculated >> 8);

    /* Transmit the packet. */
    if (!SerialPortWrite(&mbRtuBuffer[0], txPacket->len + 5))
    {
      result = false;
    }

    /* ------------ Packet reception ------------------------------------------------- */
    /* Only continue if the transmission was successful. */
    if (result)
    {
      /* Determine timeout time for the response packet. */
      waitTimeoutTime = UtilTimeGetSystemTimeMs() + timeout;
      /* Reset the indexer used for storing the received data. */
      rxModbusPacketLen = 0;
      /* Poll with timeout detection to receive the header of the XCP on Modbus RTU
       * response packet. This is everything up to the actual XCP packet data. The
       * header includes the slave address, function code and extra XCP length. 
       */
      while ((UtilTimeGetSystemTimeMs() < waitTimeoutTime) && (rxModbusPacketLen < 3))
      {
        /* Attempt to receive a new byte. */
        if (SerialPortRead(&mbRtuBuffer[rxModbusPacketLen], 1))
        {
          /* Continue with the next byte. */
          rxModbusPacketLen++;
        }
      }

      /* Check if the entire packet header was received. */
      if (rxModbusPacketLen != 3)
      {
        /* Packet header not completely received, so a timeout must have occured. */
        result = false;
      }
    }

    /* Only continue if a complete packet header was received. */
    if (result)
    {
      /* Validate the header and make sure the buffer is large enough to store the entire packet. */
      if ( (mbRtuBuffer[0] != tpMbRtuSettings.destinationAddr) ||
           (mbRtuBuffer[1] != XCP_TP_MBRTU_FCT_CODE_USER_XCP) ||
           (mbRtuBuffer[2] == 0) ||
           (((uint16_t)mbRtuBuffer[2] + 5U) > (sizeof(mbRtuBuffer) / sizeof(mbRtuBuffer[0]))) )
      {
        /* The header is either invalid or it is not an XCP response packet for us. */
        result = false;
      }
    }

    /* Only continue with reception if a valid valid packet header was received. */
    if (result)
    {
      /* Sanity check. */
      assert(rxModbusPacketLen == 3);
      
      /* Store the XCP packet length since we know that the packet header is valid. */
      rxPacket->len = mbRtuBuffer[2];
      /* Poll with timeout detection to receive the remainder of the response
       * packet. This is the XCP packet data and 16-byte CRC.
       */
      while ((UtilTimeGetSystemTimeMs() < waitTimeoutTime) && (rxModbusPacketLen < (3 + rxPacket->len + 2)))
      {
        /* Attempt to receive a new byte. */
        if (SerialPortRead(&mbRtuBuffer[rxModbusPacketLen], 1))
        {
          /* Continue with the next byte. */
          rxModbusPacketLen++;
        }
      }

      /* Check if the entire packet remainder was received. */
      if (rxModbusPacketLen != (3 + rxPacket->len + 2))
      {
        /* packet remainder not completely received, so a timeout must have occured. */
        result = false;
      }
    }

    /* Now validate the complete packet. */
    if (result)
    {
      /* Calculate the packet checksum. */
      checksumCalculated = XcpTpMbRtuCrcCalculate(&mbRtuBuffer[0], rxModbusPacketLen - 2);
      /* Extract the checksum received with the packet. */
      checksumReceived = mbRtuBuffer[rxModbusPacketLen - 2] | (mbRtuBuffer[rxModbusPacketLen - 1] << 8);
      /* Check if the CRC is correct. */
      if (checksumCalculated != checksumReceived)
      {
        result = false;
      }
    }

    /* The packet was for us, is valid and contains an XCP response packet. As a final
     * step, copy the XCP packet data.
     */
    if (result)
    {
      for (byteIdx = 0; byteIdx < rxPacket->len; byteIdx++)
      {
        rxPacket->data[byteIdx] = mbRtuBuffer[byteIdx + 3];
      }
    }
  }


  /* Give the result back to the caller. */
  return result;
} /*** end of XcpTpMbRtuSendPacket ***/


/****************************************************************************************
*   C R C   G E N E R A T I O N
****************************************************************************************/

/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief Table of CRC values for high-order byte. */
static uint8_t CRCHi[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

/** \brief Table of CRC values for low-order byte.  */
static uint8_t CRCLo[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};


/************************************************************************************//**
** \brief     Calculates the 16-bit CRC according to the Modbus RTU specification.
** \param     data Pointer to a byte array with values of which to compute the CRC.
** \param     len Number of bytes in the data array.
** \return    the 16-bit CRC value.
**
****************************************************************************************/
static uint16_t XcpTpMbRtuCrcCalculate(uint8_t const * data, uint16_t len)
{
  uint16_t result = 0;
  uint8_t  locCRCHi = 0xFF; /* high byte of CRC initialized */
  uint8_t  locCRCLo = 0xFF; /* low byte of CRC initialized */
  uint16_t idx; /* will index into CRC lookup table */

  /* Check parameters. */
  assert((data != NULL) && (len > 0));

  /* Only continue with valid parameters. */
  if ((data != NULL) && (len > 0)) /*lint !e774 */
  {
    /* Loop through message buffer. */
    while (len--)
    {
      /* Calculate the CRC. */
      idx = locCRCLo ^ *data++;
      locCRCLo = locCRCHi ^ CRCHi[idx];
      locCRCHi = CRCLo[idx];
    }
    result = (locCRCHi << 8 | locCRCLo);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpTpMbRtuCrcCalculate ***/


/*********************************** end of xcptpmbrtu.c *******************************/
