unit OpenBlt;
//***************************************************************************************
//  Description: Unit for accessing the OpenBLT shared library.
//    File Name: openblt.pas
//
//---------------------------------------------------------------------------------------
//                          C O P Y R I G H T
//---------------------------------------------------------------------------------------
//   Copyright (c) 2017 by Feaser    http://www.feaser.com    All rights reserved
//
//   This software has been carefully tested, but is not guaranteed for any particular
// purpose. The author does not offer any warranties and does not guarantee the accuracy,
//   adequacy, or completeness of the software and is not responsible for any errors or
//              omissions or the results obtained from use of the software.
//
//---------------------------------------------------------------------------------------
//                            L I C E N S E
//---------------------------------------------------------------------------------------
// This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. See the GNU General Public License for more details.
//
// You have received a copy of the GNU General Public License along with OpenBLT. It
// should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
//
//***************************************************************************************
{$IFDEF FPC}
{$MODE objfpc}{$H+}
{$ENDIF}

interface
//***************************************************************************************
// Global Includes
//***************************************************************************************
uses
  Classes, SysUtils;

//***************************************************************************************
// Global Constant Declarations
//***************************************************************************************
const
  // Name of the external librarz.
  LIBOPENBLT_LIBNAME = 'openblt';
  // Function return value for when everything went okay.
  BLT_RESULT_OK = 0;
  // Function return value for when a generic error occured.
  BLT_RESULT_ERROR_GENERIC = 1;


//***************************************************************************************
//             V E R S I O N   I N F O R M A T I O N
//***************************************************************************************
function BltVersionGetNumber: LongWord; cdecl; external LIBOPENBLT_LIBNAME;
function BltVersionGetString: PAnsiChar; cdecl; external LIBOPENBLT_LIBNAME;


//***************************************************************************************
//             S E S S I O N   /   T R A N S P O R T   L A Y E R S
//***************************************************************************************
const
  // XCP protocol version 1.0. XCP is a universal measurement and calibration communica-
  // tion protocol. It contains functionality for reading, programming, and erasing
  // (non-volatile) memory making it a good fit for bootloader purposes.
  BLT_SESSION_XCP_V10: LongWord = 0;
  // Transport layer for the XCP v1.0 protocol that uses RS-232 serial communication for
  // data exchange.
  BLT_TRANSPORT_XCP_V10_RS232: LongWord = 0;
  // Transport layer for the XCP v1.0 protocol that uses Controller Area Network (CAN)
  // for data exchange.
  BLT_TRANSPORT_XCP_V10_CAN: LongWord = 1;

type
  // Structure layout of the XCP version 1.0 session settings.
  tBltSessionSettingsXcpV10 = record
    timeoutT1: LongWord;          // Command response timeout in milliseconds.
    timeoutT3: LongWord;          // Start programming timeout in milliseconds.
    timeoutT4: LongWord;          // Erase memory timeout in milliseonds.
    timeoutT5: LongWord;          // Program memory and reset timeout in milliseonds.
    timeoutT7: LongWord;          // Busy wait timer timeout in milliseonds.
    seedKeyFile: PAnsiChar;       // Seed/key algorithm library filename.
  end;

  // Structure layout of the XCP version 1.0 RS232 transport layer settings.
  tBltTransportSettingsXcpV10Rs232 = record
    portName: PAnsiChar;          // Communication port name such as /dev/tty0.
    baudrate: LongWord;           // Communication speed in bits/sec.
  end;

  // Structure layout of the XCP version 1.0 CAN transport layer settings.
  tBltTransportSettingsXcpV10Can = record
    deviceName: PAnsiChar;        // Device name such as /dev/can0.
    deviceChannel: LongWord;      // Channel on the device to use.
    baudrate: LongWord;           // Communication speed in bits/sec.
    transmitId: LongWord;         // Transmit CAN identifier.
    receiveId: LongWord;          // Receive CAN identifier.
    useExtended: LongWord;        // Boolean to configure 29-bit CAN identifiers.
  end;

procedure BltSessionInit(sessionType: LongWord;
                         sessionSettings: Pointer;
                         transportType: LongWord;
                         transportSettings: Pointer);
                         cdecl; external LIBOPENBLT_LIBNAME;
procedure BltSessionTerminate; cdecl; external LIBOPENBLT_LIBNAME;
function  BltSessionStart: LongWord; cdecl; external LIBOPENBLT_LIBNAME;
procedure BltSessionStop; cdecl; external LIBOPENBLT_LIBNAME;
function  BltSessionClearMemory(address: LongWord;
                                len: LongWord): LongWord;
                                cdecl; external LIBOPENBLT_LIBNAME;
function  BltSessionWriteData(address: LongWord;
                              len: LongWord;
                              data: PByte): LongWord;
                              cdecl; external LIBOPENBLT_LIBNAME;
function  BltSessionReadData(address: LongWord;
                             len: LongWord;
                             data: PByte): LongWord;
                             cdecl; external LIBOPENBLT_LIBNAME;


//***************************************************************************************
//             F I R M W A R E   D A T A   C O N T R O L
//***************************************************************************************
const
  // The S-record parser enables writing and reading firmware data to and from file
  // formatted as Motorola S-record. This is a widely known file format and pretty much
  // all microcontroller compiler toolchains included functionality to output or convert
  // the firmware's data as an S-record.
  BLT_FIRMWARE_PARSER_SRECORD: LongWord = 0;

procedure BltFirmwareInit(parserType: LongWord;
                          parserSettings: Pointer);
                          cdecl; external LIBOPENBLT_LIBNAME;
procedure BltFirmwareTerminate; cdecl; external LIBOPENBLT_LIBNAME;
function  BltFirmwareLoadFromFile(firmwareFile: PAnsiChar): LongWord;
                                  cdecl; external LIBOPENBLT_LIBNAME;
function  BltFirmwareSaveToFile(firmwareFile: PAnsiChar): LongWord;
                                cdecl; external LIBOPENBLT_LIBNAME;
function  BltFirmwareGetSegmentCount: LongWord; cdecl; external LIBOPENBLT_LIBNAME;
function  BltFirmwareGetSegment(idx: LongWord;
                                var address: LongWord;
                                var len: LongWord;
                                data: PByte): LongWord;
                                cdecl; external LIBOPENBLT_LIBNAME;
function  BltFirmwareAddData(address: LongWord;
                             len: LongWord;
                             data: PByte): LongWord;
                             cdecl; external LIBOPENBLT_LIBNAME;
function  BltFirmwareRemoveData(address: LongWord;
                               len: LongWord): LongWord;
                               cdecl; external LIBOPENBLT_LIBNAME;


//***************************************************************************************
//             G E N E R I C   U T I L I T I E S
//***************************************************************************************
function BltUtilCrc16Calculate(data: PByte; len: LongWord): Word; cdecl; external 'openblt';
function BltUtilCrc32Calculate(data: PByte; len: LongWord): LongWord; cdecl; external 'openblt';


implementation


end.
//******************************** end of openblt.pas ***********************************

 
