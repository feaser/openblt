/************************************************************************************//**
* \file         session.h
* \brief        Communication session module header file.
* \ingroup      Session
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
* \defgroup   Session Communication Session Module
* \brief      Module with functionality to communicate with the bootloader on the target
*             system.
* \ingroup    Library
* \details
* The Communication Session module handles the communication with the bootloader during
* firmware updates on the target system. It contains an interface to link the desired
* communication protocol that should be used for the communication. For example the XCP
* protocol.
****************************************************************************************/
#ifndef SESSION_H
#define SESSION_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Session communication protocol interface. */
typedef struct t_session_protocol
{
  /** \brief Initializes the protocol module. */
  void (* Init) (void const * settings);
  /** \brief Terminates the protocol module. */
  void (* Terminate) (void);
  /** \brief Starts the firmware update session. This is where the connection with the
   *         target is made and the bootloader on the target is activated.
   */
  bool (* Start) (void);
  /** \brief Stops the firmware update. This is where the bootloader starts the user
   *         program on the target if a valid one is present. After this the connection
   *         with the target is severed.
   */
  void (* Stop) (void);
  /** \brief Requests the bootloader to erase the specified range of memory on the
   *         target. The bootloader aligns this range to hardware specified erase blocks.
   */
  bool (* ClearMemory) (uint32_t address, uint32_t len);
  /** \brief Requests the bootloader to program the specified data to memory. In case of
   *         non-volatile memory, the application needs to make sure the memory range
   *         was erased beforehand.
   */
  bool (* WriteData) (uint32_t address, uint32_t len, uint8_t const * data);  
  /** \brief Request the bootloader to upload the specified range of memory. The data is
   *         stored in the data byte array to which the pointer was specified.
   */
  bool (* ReadData) (uint32_t address, uint32_t len, uint8_t * data);
  /** \brief Request information from the bootloader about the info table start address
   *         and length. Using this information, the info table is extracted from the
   *         firmware file selected for the firmware update. The extracted info table
   *         is then downloaded to a RAM buffer in the bootloader. Afterwards, the 
   *         bootloader is requested to compare this info table contents with the one
   *         available in the currently programmed firmware (if any) and to check if
   *         it is okay for the session to proceed with the firmware update. If the 
   *         supported parameter is set to false by this function, the target indicated
   *         that the info table feature is either not supported or not enabled.
   */
  bool (* CheckInfoTable) (bool * supported, bool * okay);
} tSessionProtocol;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void SessionInit(tSessionProtocol const * protocol, void const * protocolSettings);
void SessionTerminate(void);
bool SessionStart(void);
void SessionStop(void);
bool SessionClearMemory(uint32_t address, uint32_t len);
bool SessionWriteData(uint32_t address, uint32_t len, uint8_t const * data);  
bool SessionReadData(uint32_t address, uint32_t len, uint8_t * data);
bool SessionCheckInfoTable(bool * supported, bool * okay);


#ifdef __cplusplus
}
#endif

#endif /* SESSION_H */
/********************************* end of session.h ************************************/

 
 
