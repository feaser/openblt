/************************************************************************************//**
* \file         main.c
* \brief        BootCommander program source file.
* \ingroup      BootCommander
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdio.h>                          /* Standard I/O functions.                 */
#include "openblt.h"                        /* OpenBLT library.                        */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* Program return codes. */
#define RESULT_OK                           (0u)
#define RESULT_ERROR_GENERIC                (1u)


/************************************************************************************//**
** \brief     This is the program entry point.
** \param     argc Number of program arguments.
** \param     argv Array with program arguments.
** \return    Program return code. 0 for success, error code otherwise.
**
****************************************************************************************/
int main(int argc, char *argv[])
{
  int result = RESULT_OK;
  volatile uint32_t waitLoopCnt;
  
  (void)argc;
  (void)argv;
  
  /* Initialize the firmware data module using the S-record parser. */
  BltFirmwareInit(BLT_FIRMWARE_PARSER_SRECORD);
  /* Configure session to use the XCP protocol. */
  tBltSessionSettingsXcpV10 sessionSettings =
  {
    .timeoutT1 = 1000,
    .timeoutT3 = 2000,
    .timeoutT4 = 10000,
    .timeoutT5 = 1000,
    .timeoutT7 = 2000,
    .seedKeyFile = NULL
  };
  /* Configure transport layer to use XCP on UART. */
  tBltTransportSettingsXcpV10Rs232 transportSettings =
  {
    .portName = "/dev/ttyUSB0",
    .baudrate = 57600
  };
  /* Initialize the session. */
  BltSessionInit(BLT_SESSION_XCP_V10, &sessionSettings, 
                 BLT_TRANSPORT_XCP_V10_RS232, &transportSettings);
  /* Start the session. */
  if (BltSessionStart() != BLT_RESULT_OK)
  {
    result = RESULT_ERROR_GENERIC;
  }
  
  /* Do a little dummy delay check LED blink rate on the board to verify that the
   * bootloader got activated.
   */
  for (waitLoopCnt = 0; waitLoopCnt < 500000000; waitLoopCnt++) 
  {
    ;
  }
  
  /* Stop the session. */
  BltSessionStop();
  /* Terminate the session. */
  BltSessionTerminate();
  /* Terminate the firmware data module. */
  BltFirmwareTerminate();
  
  /* Give result back. */
  return result;
} /*** end of main ***/


/*********************************** end of main.c *************************************/

 
