/************************************************************************************//**
* \file         Source/ARMCM0_XMC1/can.c
* \brief        Bootloader CAN communication interface source file.
* \ingroup      Target_ARMCM0_XMC1
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2016  by Feaser    http://www.feaser.com    All rights reserved
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
#include "xmc_can.h"                             /* CAN driver header                  */
#include "xmc_gpio.h"                            /* GPIO driver header                 */


#if (BOOT_COM_CAN_ENABLE > 0)
/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout for transmitting a CAN message in milliseconds. */
#define CAN_MSG_TX_TIMEOUT_MS          (50u)

/** \brief Macro for accessing the CAN channel handle in the format that is expected
 *         by the XMClib CAN driver.
 */
#define CAN_CHANNEL ((CAN_NODE_TypeDef *)(canChannelMap[BOOT_COM_CAN_CHANNEL_INDEX]))

/** \brief Message object dedicated to message transmission. */
#define CAN_TX_MSBOBJ            (CAN_MO0)

/** \brief Index of the message object dedicated to message transmission */
#define CAN_TX_MSBOBJ_IDX        (0)

/** \brief Message object dedicated to message reception. */
#define CAN_RX_MSBOBJ            (CAN_MO1)

/** \brief Index of the message object dedicated to message reception. */
#define CAN_RX_MSBOBJ_IDX        (1)


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief Helper array to quickly convert the channel index, as specific in the boot-
 *         loader's configuration header, to the associated channel handle that the
 *         XMClib's CAN driver requires.
 */
static const CAN_NODE_TypeDef *canChannelMap[] =
{
  CAN_NODE0, /* BOOT_COM_CAN_CHANNEL_INDEX = 0 */
  CAN_NODE1  /* BOOT_COM_CAN_CHANNEL_INDEX = 1 */
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Transmit message object data structure. */
static XMC_CAN_MO_t transmitMsgObj;

/** \brief Receive message object data structure. */
static XMC_CAN_MO_t receiveMsgObj;


/************************************************************************************//**
** \brief     Initializes the CAN controller and synchronizes it to the CAN bus.
** \return    none.
**
****************************************************************************************/
void CanInit(void)
{
  blt_int8u byteIdx;
  blt_int32u canModuleFreqHz;
  XMC_CAN_NODE_NOMINAL_BIT_TIME_CONFIG_t baud;
  blt_int32u transmitId;
  blt_int32u receiveId;

  /* the current implementation supports CAN_NODE0 to CAN_NODE5. throw an assertion error
   * in case a different CAN channel is configured.
   */
  ASSERT_CT((BOOT_COM_CAN_CHANNEL_INDEX >= 0) && (BOOT_COM_CAN_CHANNEL_INDEX <= 1));

  /* decide on fCAN frequency. it should be in the 5-120MHz range. according to the
   * datasheet, it must be at least 12MHz if 1 node (channel) is used with up to
   * 16 message objects. This is sufficient for this CAN driver.
   */
  canModuleFreqHz = XMC_SCU_CLOCK_GetPeripheralClockFrequency();
  /* increase if too low */
  while (canModuleFreqHz < 12000000)
  {
    canModuleFreqHz *= 2;
    /* keep the watchdog happy */
    CopService();
  }
  /* decrease if too high */
  while (canModuleFreqHz > 120000000)
  {
    canModuleFreqHz /= 2;
    /* keep the watchdog happy */
    CopService();
  }

  /* configure CAN module*/
  XMC_CAN_Init(CAN, XMC_CAN_CANCLKSRC_MCLK, canModuleFreqHz);

  /* configure CAN node baudrate */
  baud.can_frequency = canModuleFreqHz;
  baud.baudrate = BOOT_COM_CAN_BAUDRATE;
  baud.sample_point = 8000;
  baud.sjw = 1,
  XMC_CAN_NODE_NominalBitTimeConfigure(CAN_CHANNEL, &baud);

  /* set CCE and INIT bit NCR for node configuration */
  XMC_CAN_NODE_EnableConfigurationChange(CAN_CHANNEL);
  XMC_CAN_NODE_SetInitBit(CAN_CHANNEL);

  /* configure the transmit message object */
  transmitMsgObj.can_mo_ptr = CAN_TX_MSBOBJ;
  transmitMsgObj.can_priority = XMC_CAN_ARBITRATION_MODE_IDE_DIR_BASED_PRIO_2;
  /* set the transmit CAN identifier and negate the bit that configures it as a
   * 29-bit extended CAN identifier.
   */
  transmitId = BOOT_COM_CAN_TX_MSG_ID;
  transmitId &= ~0x80000000;
  if ((BOOT_COM_CAN_TX_MSG_ID & 0x80000000) == 0)
  {
    /* 11-bit standard CAN identifier */
    transmitMsgObj.can_identifier = transmitId;
    transmitMsgObj.can_id_mask = transmitId;
    transmitMsgObj.can_id_mode = XMC_CAN_FRAME_TYPE_STANDARD_11BITS;
    XMC_CAN_MO_AcceptOnlyMatchingIDE(&transmitMsgObj);
  }
  else
  {
    /* 29-bit extended CAN identifier */
    transmitMsgObj.can_identifier = transmitId;
    transmitMsgObj.can_id_mask = transmitId;
    transmitMsgObj.can_id_mode = XMC_CAN_FRAME_TYPE_EXTENDED_29BITS;
    XMC_CAN_MO_AcceptOnlyMatchingIDE(&transmitMsgObj);
  }
  transmitMsgObj.can_data_length = BOOT_COM_CAN_TX_MAX_DATA;
  for (byteIdx=0; byteIdx<transmitMsgObj.can_data_length; byteIdx++)
  {
    transmitMsgObj.can_data_byte[byteIdx] = 0;
  }
  transmitMsgObj.can_mo_type = XMC_CAN_MO_TYPE_TRANSMSGOBJ;
  XMC_CAN_MO_Config(&transmitMsgObj);

  /* allocate transmit message object to the channel */
  XMC_CAN_AllocateMOtoNodeList(CAN, BOOT_COM_CAN_CHANNEL_INDEX, CAN_TX_MSBOBJ_IDX);

  /* configure the receive message object */
  receiveMsgObj.can_mo_ptr = CAN_RX_MSBOBJ;
  receiveMsgObj.can_priority = XMC_CAN_ARBITRATION_MODE_IDE_DIR_BASED_PRIO_2;
  /* set the receive CAN identifier and negate the bit that configures it as a
   * 29-bit extended CAN identifier.
   */
  receiveId = BOOT_COM_CAN_RX_MSG_ID;
  receiveId &= ~0x80000000;
  
  if ((BOOT_COM_CAN_RX_MSG_ID & 0x80000000) == 0)
  {
    /* 11-bit standard CAN identifier */
    receiveMsgObj.can_identifier = receiveId;
    receiveMsgObj.can_id_mask = receiveId;
    receiveMsgObj.can_id_mode = XMC_CAN_FRAME_TYPE_STANDARD_11BITS;
    XMC_CAN_MO_AcceptOnlyMatchingIDE(&receiveMsgObj);
  }
  else
  {
    /* 29-bit extended CAN identifier */
    receiveMsgObj.can_identifier = receiveId;
    receiveMsgObj.can_id_mask = receiveId;
    receiveMsgObj.can_id_mode = XMC_CAN_FRAME_TYPE_EXTENDED_29BITS;
    XMC_CAN_MO_AcceptOnlyMatchingIDE(&receiveMsgObj);
  }
  receiveMsgObj.can_data_length = BOOT_COM_CAN_RX_MAX_DATA;
  for (byteIdx=0; byteIdx<receiveMsgObj.can_data_length; byteIdx++)
  {
    receiveMsgObj.can_data_byte[byteIdx] = 0;
  }
  receiveMsgObj.can_mo_type = XMC_CAN_MO_TYPE_RECMSGOBJ;
  XMC_CAN_MO_Config(&receiveMsgObj);

  /* allocate receive message object to the channel */
  XMC_CAN_AllocateMOtoNodeList(CAN, BOOT_COM_CAN_CHANNEL_INDEX, CAN_RX_MSBOBJ_IDX);

  /* reset CCE and INIT bit NCR for node configuration */
  XMC_CAN_NODE_DisableConfigurationChange(CAN_CHANNEL);
  XMC_CAN_NODE_ResetInitBit(CAN_CHANNEL);
} /*** end of CanInit ***/


/************************************************************************************//**
** \brief     Transmits a packet formatted for the communication interface.
** \param     data Pointer to byte array with data that it to be transmitted.
** \param     len  Number of bytes that are to be transmitted.
** \return    none.
**
****************************************************************************************/
void CanTransmitPacket(blt_int8u *data, blt_int8u len)
{
  blt_int8u byteIdx;
  blt_int32u timeout;

  /* copy message data */
  transmitMsgObj.can_data_length = len;
  for (byteIdx=0; byteIdx<transmitMsgObj.can_data_length; byteIdx++)
  {
    transmitMsgObj.can_data_byte[byteIdx] = data[byteIdx];
  }
  /* update the message object */
  XMC_CAN_MO_UpdateData(&transmitMsgObj);
  /* reset the message object's transmit pending flag */
  XMC_CAN_MO_ResetStatus(&transmitMsgObj, XMC_CAN_MO_RESET_STATUS_TX_PENDING);
  /* submit message for transmission */
  XMC_CAN_MO_Transmit(&transmitMsgObj);
  /* set timeout time to wait for transmission completion */
  timeout = TimerGet() + CAN_MSG_TX_TIMEOUT_MS;
  /* wait for transmit completion */
  while ((XMC_CAN_MO_GetStatus(&transmitMsgObj) & XMC_CAN_MO_STATUS_TX_PENDING) != 0)
  {
    /* keep the watchdog happy */
    CopService();
    /* break loop upon timeout. this would indicate a hardware failure or no other
     * nodes connected to the bus.
     */
    if (TimerGet() > timeout)
    {
      break;
    }
  }
} /*** end of CanTransmitPacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface packet if one is present.
** \param     data Pointer to byte array where the data is to be stored.
** \param     len Pointer where the length of the packet is to be stored.
** \return    BLT_TRUE is a packet was received, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool CanReceivePacket(blt_int8u *data, blt_int8u *len)
{
  blt_int8u byteIdx;
  blt_bool result = BLT_FALSE;

  /* check if a new message was received */
  if ((XMC_CAN_MO_GetStatus(&receiveMsgObj) & XMC_CAN_MO_STATUS_RX_PENDING) != 0)
  {
    /* read out and process the newly received data */
    if (XMC_CAN_MO_Receive(&receiveMsgObj) == XMC_CAN_STATUS_SUCCESS)
    {
      *len = receiveMsgObj.can_data_length;
      for (byteIdx=0; byteIdx<receiveMsgObj.can_data_length; byteIdx++)
      {
        data[byteIdx] = receiveMsgObj.can_data_byte[byteIdx];
        result = BLT_TRUE;
      }
    }
    /* reset the message received flag */
    XMC_CAN_MO_ResetStatus(&receiveMsgObj, XMC_CAN_MO_RESET_STATUS_RX_PENDING);
  }

  return result;
} /*** end of CanReceivePacket ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of can.c **************************************/
