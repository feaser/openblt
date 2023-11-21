/************************************************************************************//**
* \file         candriver.h
* \brief        Generic CAN driver header file.
* \ingroup      CanDriver
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2017  by Feaser    http://www.feaser.com    All rights reserved
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
/************************************************************************************//**
* \defgroup   CanDriver CAN driver
* \brief      This module implements a generic CAN driver.
* \ingroup    Session
****************************************************************************************/
#ifndef CANDRIVER_H
#define CANDRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Maximum number of data bytes in a CAN message. */
#define CAN_MSG_MAX_LEN     (8u)

/** Bit mask that configures a CAN message identifier as 29-bit extended as opposed to
 *  11-bit standard. Whenever this bit is set in the CAN identifier field of tCanMsg, 
 *  then the CAN identifier is configured for 29-bit CAN extended.
 */
#define CAN_MSG_EXT_ID_MASK (0x80000000u)


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Enumeration of the supported baudrates. */
typedef enum
{
  CAN_BR10K   = 0,                  /**< 10 kbits/sec                                  */
  CAN_BR20K   = 1,                  /**< 20 kbits/sec                                  */
  CAN_BR50K   = 2,                  /**< 50 kbits/sec                                  */
  CAN_BR100K  = 3,                  /**< 100 kbits/sec                                 */
  CAN_BR125K  = 4,                  /**< 125 kbits/sec                                 */
  CAN_BR250K  = 5,                  /**< 250 kbits/sec                                 */
  CAN_BR500K  = 6,                  /**< 500 kbits/sec                                 */
  CAN_BR800K  = 7,                  /**< 800 kbits/sec                                 */
  CAN_BR1M    = 8                   /**< 1 Mbits/sec                                   */
} tCanBaudrate;

/** \brief Layout of a CAN message. Note that \ref CAN_MSG_EXT_ID_MASK can be used to
 *         configure the CAN message identifier as 29-bit extended.         
 */
typedef struct t_can_msg
{
  uint32_t id;                         /**< CAN message identifier.                    */
  uint8_t dlc;                         /**< CAN message data length code.              */
  uint8_t data[CAN_MSG_MAX_LEN];       /**< Array with CAN message data.               */
} tCanMsg;

/** \brief Type to group of CAN interface related settings. The device name specifies
 *         the name of the CAN interface device. For some CAN interfaces this is don't
 *         care, but for other absolutely necessar, for example Linux SocketCAN. The 
 *         channel specifies the channel on the CAN interface, in case it has multiple
 *         CAN channels. The baudrate specifies the communication speed on the CAN 
 *         network. The code and mask values configure the message reception acceptance
 *         filter. A mask bit value of 0 means don't care. The code part of the filter 
 *         determines what bit values to match in the received message identifier.
 *           Example 1: Receive all CAN identifiers
 *                      .code = 0x00000000
 *                      .mask = 0x00000000
 *           Example 2: Receive only CAN identifier 0x124 (11-bit or 29-bit)
 *                      .code = 0x00000124
 *                      .mask = 0x1fffffff
 *           Example 3: Receive only CAN identifier 0x124 (11-bit)
 *                      .code = 0x00000124
 *                      .mask = 0x9fffffff
 *           Example 4: Receive only CAN identifier 0x124 (29-bit)
 *                      .code = 0x80000124
 *                      .mask = 0x9fffffff
 */
typedef struct t_can_settings
{
  char const * devicename;             /**< CAN interface device name (pcanusb, vcan0).*/  
  uint32_t channel;                    /**< Zero based CAN channel index.              */
  tCanBaudrate baudrate;               /**< Communication speed.                       */
  uint32_t code;                       /**< Code of the reception acceptance filter.   */
  uint32_t mask;                       /**< Mask of the reception acceptance filter.   */
} tCanSettings;

/** \brief Structure with CAN event callback functions. */
typedef struct t_can_events
{
  /** \brief Event function that should be called when a message was transmitted. */
  void (*MsgTxed) (tCanMsg const * msg);
  /** \brief Event function that should be called when a message was received. */
  void (*MsgRxed) (tCanMsg const * msg);
} tCanEvents;

/** \brief CAN interface type. */
typedef struct t_can_interface
{
  /** \brief Initialization of the CAN interface. */
  void (*Init) (tCanSettings const * settings);
  /** \brief Terminates the CAN interface. */
  void (*Terminate) (void);
  /** \brief Connects the CAN interface to the CAN bus. */
  bool (*Connect) (void);
  /** \brief Disconnects the CAN interface from the CAN bus. */
  void (*Disconnect) (void);
  /** \brief Submits a CAN message for transmission. */
  bool (*Transmit) (tCanMsg const * msg);
  /** \brief Check if a bus off and/or bus heavy situation occurred. */
  bool (*IsBusError) (void);
  /** \brief Registers the event callback functions. */
  void (*RegisterEvents) (tCanEvents const * events);
} tCanInterface;


/***************************************************************************************
* Function prototypes
****************************************************************************************/
void CanInit(tCanSettings const * settings);
void CanTerminate(void);
bool CanConnect(void);
void CanDisconnect(void);
bool CanIsConnected(void);
bool CanTransmit(tCanMsg const * msg);
bool CanIsBusError(void);
void CanRegisterEvents(tCanEvents const * events);

#ifdef __cplusplus
}
#endif

#endif /* CANDRIVER_H */
/*********************************** end of candriver.h ********************************/
