unit XcpTransport;
//***************************************************************************************
//  Description: XCP transport layer for CAN.
//    File Name: XcpTransport.pas
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
interface


//***************************************************************************************
// Includes
//***************************************************************************************
uses
  Windows, Messages, SysUtils, Classes, Forms, IniFiles;


//***************************************************************************************
// Global Constants
//***************************************************************************************
// a CAN message can only have up to 8 bytes
const kMaxPacketSize = 8;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  TKvaserHardware = ( KVASER_LEAFLIGHT_V2 = $01 );

  TXcpTransport = class(TObject)
  private
    packetTxId   : LongWord;
    packetRxId   : LongWord;
    extendedId   : Boolean;
    canHardware  : TKvaserHardware; { KVASER_xxx }
    canChannel   : Word; { currently supported is 1..8 }
    canBaudrate  : LongWord; { in bits/sec }
    connected    : Boolean;
  public
    packetData   : array[0..kMaxPacketSize-1] of Byte;
    packetLen    : Word;
    constructor Create;
    procedure   Configure(iniFile : string);
    function    Connect: Boolean;
    function    SendPacket(timeOutms: LongWord): Boolean;
    function    IsComError: Boolean;
    procedure   Disconnect;
    destructor  Destroy; override;
  end;


implementation

//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructore
//
//***************************************************************************************
constructor TXcpTransport.Create;
begin
  // call inherited constructor
  inherited Create;

  // reset the packet ids
  packetTxId := 0;
  packetRxId := 0;

  // use standard id's by default
  extendedId := false;

  // reset packet length
  packetLen := 0;

  // disconnected by default
  connected := false;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Destroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class destructor
//
//***************************************************************************************
destructor TXcpTransport.Destroy;
begin
  // call inherited destructor
  inherited;
end; //*** end of Destroy ***


//***************************************************************************************
// NAME:           Configure
// PARAMETER:      filename of the INI
// RETURN VALUE:   none
// DESCRIPTION:    Configures both this class from the settings in the INI.
//
//***************************************************************************************
procedure TXcpTransport.Configure(iniFile : string);
var
  settingsIni : TIniFile;
  baudrateIdx : Integer;
const
  baudrateLookupTable : array[0..7] of LongWord =
  (
    // list baudrates in the same order as they appear in the combobox on the settings
    // form. this way the combobox's ItemIndex property can be used as an indexer to this
    // array.
    1000000, 500000, 250000, 125000, 100000, 83333, 50000, 10000
  );
begin
	// read XCP configuration from INI
  if FileExists(iniFile) then
  begin
    // create ini file object
    settingsIni := TIniFile.Create(iniFile);

    // set hardware configuration
    case settingsIni.ReadInteger('can', 'hardware', 0) of
      0: canHardware := KVASER_LEAFLIGHT_V2;
      else
        canHardware := KVASER_LEAFLIGHT_V2;
    end;
    // set channel configuration
    canChannel := settingsIni.ReadInteger('can', 'channel', 0) + 1;
    // set baudrate configuration
    baudrateIdx := settingsIni.ReadInteger('can', 'baudrate', 1);
    canBaudrate := 500000;
    if (baudrateIdx >= 0) and (baudrateIdx < Length(baudrateLookupTable)) then
      canBaudrate := baudrateLookupTable[baudrateIdx];
    // set message configuration
    packetTxId := settingsIni.ReadInteger('can', 'txid', $667);
    packetRxId := settingsIni.ReadInteger('can', 'rxid', $7e1);
    extendedId := settingsIni.ReadBool('can', 'extended', false);

    // release ini file object
    settingsIni.Free;
  end;
end; //*** end of Configure ***


//***************************************************************************************
// NAME:           Connect
// PARAMETER:      none
// RETURN VALUE:   True if successful, False otherwise.
// DESCRIPTION:    Connects the transport layer device.
//
//***************************************************************************************
function TXcpTransport.Connect: Boolean;
begin
  // init result value
  result := false;

  // disconnect first if still connected
  if connected then
    Disconnect;

  //##Vg TODO connect to the CAN hardware interface and update the connected flag
  connected := false;
end; //*** end of Connect ***


//***************************************************************************************
// NAME:           IsComError
// PARAMETER:      none
// RETURN VALUE:   True if in error state, False otherwise.
// DESCRIPTION:    Determines if the communication interface is in an error state.
//
//***************************************************************************************
function TXcpTransport.IsComError: Boolean;
begin
  // init result to no error.
  result := false;

  // check for bus off error if connected
  if connected then
  begin
    //##Vg TODO check for bus off and possible bus heavy if available
  end;
end; //*** end of IsComError ***


//***************************************************************************************
// NAME:           SendPacket
// PARAMETER:      the time[ms] allowed for the reponse from the slave to come in.
// RETURN VALUE:   True if response received from slave, False otherwise
// DESCRIPTION:    Sends the XCP packet using the data in 'packetData' and length in
//                 'packetLen' and waits for the response to come in.
//
//***************************************************************************************
function TXcpTransport.SendPacket(timeOutms: LongWord): Boolean;
var
  responseReceived: Boolean;
  timeoutTime: DWORD;
begin
  // initialize the result value
  result := false;

  // do not send data when the packet length is invalid or when not connected
  // to the CAN hardware
  if (packetLen > kMaxPacketSize) or (not connected) then
  begin
    Exit;
  end;

  //##Vg TODO prepare the packet for transmission in a CAN message

  //##Vg transmit the packet via CAN and Exit if not possible

  // reset flag and set the reception timeout time
  responseReceived := false;
  timeoutTime := GetTickCount + timeOutms;

  // attempt to receive the packet response within the timeout time
  repeat
    //##Vg attempt to read a message from the reception buffer, break loop on error
    //##Vg check if it has the correct identifier (packetRxId) and set the
    //     responseReceived flag if so

    // give the application a chance to use the processor
    Application.ProcessMessages;
  until (GetTickCount > timeoutTime) or (responseReceived);

  // check if the response was correctly received
  if responseReceived then
  begin
    //##Vg copy the response for futher processing (packetLen and packetData)
    // success
    result := true;
  end;
end; //*** end of SendPacket ***


//***************************************************************************************
// NAME:           Disconnect
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Disconnects the transport layer device.
//
//***************************************************************************************
procedure TXcpTransport.Disconnect;
begin
  // disconnect CAN interface if connected
  if connected then
  begin
    //##Vg TODO disconnect from the CAN hardware interface
  end;
  connected := false;
end; //*** end of Disconnect ***

end.
//******************************** end of XcpTransport.pas ******************************

