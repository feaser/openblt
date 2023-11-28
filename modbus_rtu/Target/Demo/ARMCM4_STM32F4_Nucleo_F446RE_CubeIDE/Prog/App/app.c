/************************************************************************************//**
* \file         Demo/ARMCM4_STM32F4_Nucleo_F446RE_CubeIDE/Prog/App/app.c
* \brief        User program application source file.
* \ingroup      Prog_ARMCM4_STM32F4_Nucleo_F446RE_CubeIDE
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2023  by Feaser    http://www.feaser.com    All rights reserved
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
#include "header.h"                                    /* generic header               */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static tTbxMbServerResult AppReadInputReg(tTbxMbServer channel, uint16_t addr,
                                          uint16_t * value);


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Modbus RTU transport layer handle. */
static tTbxMbTp modbusRtuTransport;

/** \brief Modbus server channel handle. */
static tTbxMbServer modbusServer;


/************************************************************************************//**
** \brief     Initializes the user program application. Should be called once during
**            software program initialization.
** \return    none.
**
****************************************************************************************/
void AppInit(void)
{
  tTbxMbUartPort port;

  /* Initialize the timer driver. */
  TimerInit();
  /* Initialize the led driver. */
  LedInit();
  /* Create a Modbus RTU transport layer object. */
  port = (BOOT_COM_MBRTU_CHANNEL_INDEX == 0) ? TBX_MB_UART_PORT1 : TBX_MB_UART_PORT2;
  modbusRtuTransport = TbxMbRtuCreate(0x01, port, TBX_MB_UART_57600BPS,
                                     TBX_MB_UART_1_STOPBITS, TBX_MB_EVEN_PARITY);
  /* Create a Modbus server channel object and link the RTU transport layer object. */
  modbusServer = TbxMbServerCreate(modbusRtuTransport);
  /* Set the callbacks for accessing the Modbus data tables. */
  TbxMbServerSetCallbackReadInputReg(modbusServer, AppReadInputReg);
} /*** end of AppInit ***/


/************************************************************************************//**
** \brief     Task function of the user program application. Should be called
**            continuously in the program loop.
** \return    none.
**
****************************************************************************************/
void AppTask(void)
{
  /* Toggle LED with a fixed frequency. */
  LedToggle();
  /* Continuously call the Modbus stack event task function. */
  TbxMbEventTask();
} /*** end of AppTask ***/


/************************************************************************************//**
** \brief     Reads a data element from the input registers data table.
** \details   Write the value of the input register in your CPUs native endianess. The
**            MicroTBX-Modbus stack will automatically convert this to the big endianess
**            that the Modbus protocol requires.
**            Note that the element is specified by its zero-based address in the range
**            0 - 65535, not its element number (1 - 65536).
** \param     channel Handle to the Modbus server channel object that triggered the
**            callback.
** \param     addr Element address (0..65535).
** \param     value Pointer to write the value of the input register to.
** \return    TBX_MB_SERVER_OK if successful, TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR if the
**            specific data element address is not supported by this server,
**            TBX_MB_SERVER_ERR_DEVICE_FAILURE otherwise.
**
****************************************************************************************/
static tTbxMbServerResult AppReadInputReg(tTbxMbServer channel, uint16_t addr,
                                          uint16_t * value)
{
  tTbxMbServerResult result = TBX_MB_SERVER_OK;

  TBX_UNUSED_ARG(channel);

  /* Filter on the requested input register address. */
  switch (addr)
  {
  case 30000U:
    /* Store the current value of the millisecond counter. */
    *value = (uint16_t)TimerGet();
    break;

  case 30001U:
    /* Store a constant value. */
    *value = 12345;
    break;

  default:
    /* Unsupported input register address. */
    result = TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR;
    break;
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of ModbusReadInputReg ***/


/*********************************** end of app.c **************************************/
