/************************************************************************************//**
* \file         tbxmb_server.h
* \brief        Modbus server header file.
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
#ifndef TBXMB_SERVER_H
#define TBXMB_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Handle to a Modbus server channel object, in the format of an opaque
 *         pointer.
 */
typedef void * tTbxMbServer;


/** \brief Enumerated type with all supported return values for the callbacks. */
typedef enum
{
  /* Callback function succeeded. */
  TBX_MB_SERVER_OK = 0U,
  /* Callback function could not perform the request because the address of the specified
   * data element is not supported by the server.
   */
  TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR,
  /* Callback function could not perform the operation on the specified data element. */
  TBX_MB_SERVER_ERR_DEVICE_FAILURE
} tTbxMbServerResult;


/** \brief   Modbus server callback function for reading a discrete input.
 *  \details Note that the element is specified by its zero-based address in the range
 *           0 - 65535, not its element number (1 - 65536).
 *  \param   channel Handle to the Modbus server channel object that triggered the 
 *           callback.
 *  \param   addr Element address (0..65535).
 *  \param   value Pointer to write the value of the input to. Use TBX_ON if the input
 *           is on, TBX_OFF otherwise.
 *  \return  TBX_MB_SERVER_OK if successful, TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR if the
 *           specific data element address is not supported by this server, 
 *           TBX_MB_SERVER_ERR_DEVICE_FAILURE otherwise.
 */
typedef tTbxMbServerResult (* tTbxMbServerReadInput)       (tTbxMbServer    channel, 
                                                            uint16_t        addr, 
                                                            uint8_t       * value);


/** \brief   Modbus server callback function for reading a coil.
 *  \details Note that the element is specified by its zero-based address in the range
 *           0 - 65535, not its element number (1 - 65536).
 *  \param   channel Handle to the Modbus server channel object that triggered the 
 *           callback.
 *  \param   addr Element address (0..65535).
 *  \param   value Pointer to write the value of the coil to. Use TBX_ON if the coils is
 *           on, TBX_OFF otherwise.
 *  \return  TBX_MB_SERVER_OK if successful, TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR if the
 *           specific data element address is not supported by this server, 
 *           TBX_MB_SERVER_ERR_DEVICE_FAILURE otherwise.
 */
typedef tTbxMbServerResult (* tTbxMbServerReadCoil)        (tTbxMbServer    channel, 
                                                            uint16_t        addr, 
                                                            uint8_t       * value);


/** \brief   Modbus server callback function for writing a coil.
 *  \details Note that the element is specified by its zero-based address in the range
 *           0 - 65535, not its element number (1 - 65536).
 *  \param   channel Handle to the Modbus server channel object that triggered the 
 *           callback.
 *  \param   addr Element address (0..65535).
 *  \param   value Coil value. Use TBX_ON to activate the coil, TBX_OFF otherwise.
 *  \return  TBX_MB_SERVER_OK if successful, TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR if the
 *           specific data element address is not supported by this server, 
 *           TBX_MB_SERVER_ERR_DEVICE_FAILURE otherwise.
 */
typedef tTbxMbServerResult (* tTbxMbServerWriteCoil)       (tTbxMbServer    channel, 
                                                            uint16_t        addr, 
                                                            uint8_t         value);


/** \brief   Modbus server callback function for reading an input register.
 *  \details Write the value of the input register in your CPUs native endianess. The
 *           MicroTBX-Modbus stack will automatically convert this to the big endianess
 *           that the Modbus protocol requires.
 *           Note that the element is specified by its zero-based address in the range
 *           0 - 65535, not its element number (1 - 65536).
 *  \param   channel Handle to the Modbus server channel object that triggered the 
 *           callback.
 *  \param   addr Element address (0..65535).
 *  \param   value Pointer to write the value of the input register to.
 *  \return  TBX_MB_SERVER_OK if successful, TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR if the
 *           specific data element address is not supported by this server, 
 *           TBX_MB_SERVER_ERR_DEVICE_FAILURE otherwise.
 */
typedef tTbxMbServerResult (* tTbxMbServerReadInputReg)    (tTbxMbServer    channel, 
                                                            uint16_t        addr, 
                                                            uint16_t      * value);


/** \brief   Modbus server callback function for reading a holding register.
 *  \details Write the value of the holding register in your CPUs native endianess. The
 *           MicroTBX-Modbus stack will automatically convert this to the big endianess
 *           that the Modbus protocol requires.
 *           Note that the element is specified by its zero-based address in the range
 *           0 - 65535, not its element number (1 - 65536).
 *  \param   channel Handle to the Modbus server channel object that triggered the 
 *           callback.
 *  \param   addr Element address (0..65535).
 *  \param   value Pointer to write the value of the holding register to.
 *  \return  TBX_MB_SERVER_OK if successful, TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR if the
 *           specific data element address is not supported by this server, 
 *           TBX_MB_SERVER_ERR_DEVICE_FAILURE otherwise.
 */
typedef tTbxMbServerResult (* tTbxMbServerReadHoldingReg)  (tTbxMbServer    channel, 
                                                            uint16_t        addr, 
                                                            uint16_t      * value);


/** \brief   Modbus server callback function for writing a holding register.
 *  \details Note that the element is specified by its zero-based address in the range
 *           0 - 65535, not its element number (1 - 65536).
 *           The value of the holding register in already in your CPUs native endianess.
 *  \param   channel Handle to the Modbus server channel object that triggered the 
 *           callback.
 *  \param   addr Element address (0..65535).
 *  \param   value Value of the holding register.
 *  \return  TBX_MB_SERVER_OK if successful, TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR if the
 *           specific data element address is not supported by this server, 
 *           TBX_MB_SERVER_ERR_DEVICE_FAILURE otherwise.
 */
typedef tTbxMbServerResult (* tTbxMbServerWriteHoldingReg) (tTbxMbServer    channel, 
                                                            uint16_t        addr, 
                                                            uint16_t        value);


/** \brief   Modbus server callback function for implementing custom function code
 *           handling. Thanks to this functionality, the user can support Modbus function
 *           codes that are either currently not supported or user defined extensions.
 *  \details The "rxPdu" and "txPdu" parameters are pointers to the byte array of the
 *           PDU. The first byte (i.e. rxPdu[0]) contains the function code, followed by
 *           its data bytes. Upon calling the callback, the "len" parameter contains the
 *           length of "rxPdu". When preparing the response, you can write the length
 *           of the "txPdu" response to "len" as well.
 *  \param   channel Handle to the Modbus server channel object that triggered the 
 *           callback.
 *  \param   rxPdu Pointer to a byte array for reading the received PDU.
 *  \param   txPdu Pointer to a byte array for writing the response PDU.
 *  \param   len Pointer to the PDU length, including the function code.
 *  \return  TBX_TRUE if the callback function handled the received function code and 
 *           prepared a response PDU. TBX_FALSE otherwise.
 */
typedef uint8_t            (* tTbxMbServerCustomFunction)  (tTbxMbServer    channel,
                                                            uint8_t const * rxPdu,
                                                            uint8_t       * txPdu,
                                                            uint8_t       * len);


/****************************************************************************************
* Function prototypes
****************************************************************************************/
tTbxMbServer TbxMbServerCreate                    (tTbxMbTp                   transport);

void         TbxMbServerFree                      (tTbxMbServer                channel);

void         TbxMbServerSetCallbackReadInput      (tTbxMbServer                channel,
                                                   tTbxMbServerReadInput       callback);

void         TbxMbServerSetCallbackReadCoil       (tTbxMbServer                channel,
                                                   tTbxMbServerReadCoil        callback);

void         TbxMbServerSetCallbackWriteCoil      (tTbxMbServer                channel,
                                                   tTbxMbServerWriteCoil       callback);

void         TbxMbServerSetCallbackReadInputReg   (tTbxMbServer                channel,
                                                   tTbxMbServerReadInputReg    callback);

void         TbxMbServerSetCallbackReadHoldingReg (tTbxMbServer                channel,
                                                   tTbxMbServerReadHoldingReg  callback);

void         TbxMbServerSetCallbackWriteHoldingReg(tTbxMbServer                channel,
                                                   tTbxMbServerWriteHoldingReg callback);

void         TbxMbServerSetCallbackCustomFunction (tTbxMbServer                channel,
                                                   tTbxMbServerCustomFunction  callback);


#ifdef __cplusplus
}
#endif

#endif /* TBXMB_SERVER_H */
/*********************************** end of tbxmb_server.h *****************************/
