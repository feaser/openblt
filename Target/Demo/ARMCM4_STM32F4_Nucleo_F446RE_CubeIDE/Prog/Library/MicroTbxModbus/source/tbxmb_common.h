/************************************************************************************//**
* \file         tbxmb_common.h
* \brief        MicroTBX-Modbus common definitions header file.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2023 by Feaser     www.feaser.com     All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This file is part of MicroTBX-Modbus. MicroTBX-Modbus is free software: you can
* redistribute it and/or modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* MicroTBX-Modbus is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
* PARTICULAR PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with MicroTBX-Modbus.
* If not, see www.gnu.org/licenses/.
*
* \endinternal
****************************************************************************************/
#ifndef TBXMB_COMMON_H
#define TBXMB_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* ------------------------- Function codes ------------------------------------------ */
/** \brief Modbus function code 01 - Read Coils. */
#define TBX_MB_FC01_READ_COILS                        (1U)

/** \brief Modbus function code 02 - Read Discrete Inputs. */
#define TBX_MB_FC02_READ_DISCRETE_INPUTS              (2U)

/** \brief Modbus function code 03 - Read Holding Registers. */
#define TBX_MB_FC03_READ_HOLDING_REGISTERS            (3U)

/** \brief Modbus function code 04 - Read Input Registers. */
#define TBX_MB_FC04_READ_INPUT_REGISTERS              (4U)

/** \brief Modbus function code 05 - Write Single Coil. */
#define TBX_MB_FC05_WRITE_SINGLE_COIL                 (5U)

/** \brief Modbus function code 06 - Write Single Register. */
#define TBX_MB_FC06_WRITE_SINGLE_REGISTER             (6U)

/** \brief Modbus function code 08 - Diagnostics. */
#define TBX_MB_FC08_DIAGNOSTICS                       (8U)

/** \brief Modbus function code 15 - Write Multiple Coils. */
#define TBX_MB_FC15_WRITE_MULTIPLE_COILS              (15U)

/** \brief Modbus function code 16 - Write Multiple Registers. */
#define TBX_MB_FC16_WRITE_MULTIPLE_REGISTERS          (16U)


/* ------------------------- Exception codes ----------------------------------------- */
/** \brief Modbus exception code 01 - Illegal function. */
#define TBX_MB_EC01_ILLEGAL_FUNCTION                  (1U)

/** \brief Modbus exception code 02 - Illegal data address. */
#define TBX_MB_EC02_ILLEGAL_DATA_ADDRESS              (2U)

/** \brief Modbus exception code 03 - Illegal data value. */
#define TBX_MB_EC03_ILLEGAL_DATA_VALUE                (3U)

/** \brief Modbus exception code 04 - Server device failure. */
#define TBX_MB_EC04_SERVER_DEVICE_FAILURE             (4U)


/* ------------------------- Diagnostics sub-function codes -------------------------- */
/** \brief Diagnostics sub-function code - Return Query Data. */
#define TBX_MB_DIAG_SC_QUERY_DATA                     (0U)

/** \brief Diagnostics sub-function code - Clear Counters. */
#define TBX_MB_DIAG_SC_CLEAR_COUNTERS                 (10U)

/** \brief Diagnostics sub-function code - Return Bus Message Count. */
#define TBX_MB_DIAG_SC_BUS_MESSAGE_COUNT              (11U)

/** \brief Diagnostics sub-function code - Return Bus Communication Error Count. */
#define TBX_MB_DIAG_SC_BUS_COMM_ERROR_COUNT           (12U)

/** \brief Diagnostics sub-function code - Return Bus Exception Error Count. */
#define TBX_MB_DIAG_SC_BUS_EXCEPTION_ERROR_COUNT      (13U)

/** \brief Diagnostics sub-function code - Return Server Message Count. */
#define TBX_MB_DIAG_SC_SERVER_MESSAGE_COUNT           (14U)

/** \brief Diagnostics sub-function code - Return Server No Response Count. */
#define TBX_MB_DIAG_SC_SERVER_NO_RESPONSE_COUNT       (15U)


/* ------------------------- Bit masks ----------------------------------------------- */
/** \brief Bit mask to OR to the function code to flag it as an exception response. */
#define TBX_MB_FC_EXCEPTION_MASK                      (0x80U)


/****************************************************************************************
* Inline functions
****************************************************************************************/
/************************************************************************************//**
** \brief     Helper function to extract an unsigned 16-bit value from the data of a 
**            Modbus packet. Note that unsigned 16-bit values are always store in the
**            big endian format, e.g. 0x1234 is stored as:
**            - data[0] = 0x12
**            - data[1] = 0x34
** \param     data Pointer to the byte array that holds the two bytes to extract, stored
**            in the big endian format.
** \return    The 16-bit unsigned integer value.
**
****************************************************************************************/
static inline uint16_t TbxMbCommonExtractUInt16BE(uint8_t const * data)
{
  return ((uint16_t)data[0] << 8U) | data[1];
} /*** end of TbxMbCommonExtractUInt16BE ***/


/************************************************************************************//**
** \brief     Helper function to store an unsigned 16-bit value in the data of a Modbus
**            packet. Note that unsigned 16-bit values are always stored in the
**            big endian format, e.g. 0x1234 is stored as:
**            - data[0] = 0x12
**            - data[1] = 0x34
** \param     value The unsigned 16-bit value to store.
** \param     data Pointer to the byte array where to store the value in the big endian
**            format.
**
****************************************************************************************/
static inline void TbxMbCommonStoreUInt16BE(uint16_t   value,
                                            uint8_t  * data)
{
  data[0] = (uint8_t)(value >> 8U);
  data[1] = (uint8_t)value;
} /*** end of TbxMbCommonExtractUInt16BE ***/




#ifdef __cplusplus
}
#endif

#endif /* TBXMB_COMMON_H */
/*********************************** end of tbxmb_common.h *****************************/
