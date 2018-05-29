/************************************************************************************//**
* \file         Source\ARMCM3_STM32F2\can.c
* \brief        Bootloader CAN communication interface source file.
* \ingroup      Target_ARMCM3_STM32F2
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


#if (BOOT_COM_CAN_ENABLE > 0)
/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout for entering/leaving CAN initialization mode in milliseconds. */
#define CAN_INIT_TIMEOUT_MS            (250u)

/** \brief Timeout for transmitting a CAN message in milliseconds. */
#define CAN_MSG_TX_TIMEOUT_MS          (50u)


/************************************************************************************//**
** \brief     Initializes the CAN controller and synchronizes it to the CAN bus.
** \return    none.
**
****************************************************************************************/
void CanInit(void)
{
  /* the current implementation supports CAN1 and 2. throw an assertion error in case a
   * different CAN channel is configured.
   */
  ASSERT_CT((BOOT_COM_CAN_CHANNEL_INDEX == 0 || BOOT_COM_CAN_CHANNEL_INDEX == 1));

  /* TODO ##Vg Implement CanInit. Keep in mind that CAN1 need to be enabled for using the
   *           CAN2 reception filters.
   */
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
  /* TODO ##Vg Implement CanTransmitPacket */
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
  blt_bool result = BLT_FALSE;

  /* TODO ##Vg Implement CanReceivePacket */

  /* give the result back to the caller */
  return result;
} /*** end of CanReceivePacket ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of can.c **************************************/
