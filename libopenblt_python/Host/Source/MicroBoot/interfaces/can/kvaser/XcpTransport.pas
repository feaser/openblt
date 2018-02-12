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
  Windows, Messages, SysUtils, Classes, Forms, IniFiles, canlib;


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
    kvaserHandle : canHandle;
    canHardware  : TKvaserHardware; { KVASER_xxx }
    canChannel   : Word; { currently supported is 1..1 }
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
  // invalidate the handle
  kvaserHandle := canINVALID_HANDLE;
  // initialize the library
  canInitializeLibrary;
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
  // unload the library
  canUnloadLibrary;
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
var
  openFlags: Integer;
  frequency: Integer;
begin
  // init result value
  result := false;

  // disconnect first if still connected
  if connected then
    Disconnect;

  // the current version only supports the leaf light v2
  if canHardware = KVASER_LEAFLIGHT_V2 then
  begin
    // open the CAN channel if valid
    if canChannel > 0 then
    begin
      // set the open flags
      openFlags := canOPEN_REQUIRE_INIT_ACCESS;
      if extendedId then
      begin
        openFlags := openFlags or canOPEN_REQUIRE_EXTENDED;
      end;
      kvaserHandle := canOpenChannel(canChannel - 1, openFlags);
      // only continue if the channel was opened and the handle is not valid
      if kvaserHandle >= 0 then
      begin
        case canBaudrate of
          1000000: frequency := canBITRATE_1M;
           500000: frequency := canBITRATE_500K;
           250000: frequency := canBITRATE_250K;
           125000: frequency := canBITRATE_125K;
           100000: frequency := canBITRATE_100K;
            83333: frequency := canBITRATE_83K;
            50000: frequency := canBITRATE_50K;
            10000: frequency := canBITRATE_10K;
        else
          frequency := canBITRATE_500K;
        end;
        // configure the baudrate
        if canSetBusParams(kvaserHandle, frequency, 0, 0, 0, 0, 0) = canOK then
        begin
          // configure output control to the default normal mode
          if canSetBusOutputControl(kvaserHandle, canDRIVER_NORMAL) = canOK then
          begin
            // go on the bus
            if canBusOn(kvaserHandle) = canOK then
            begin
              // connection was established
              connected := true;
              result := true;
            end;
          end;
        end;
      end;
    end;
  end;
end; //*** end of Connect ***


//***************************************************************************************
// NAME:           IsComError
// PARAMETER:      none
// RETURN VALUE:   True if in error state, False otherwise.
// DESCRIPTION:    Determines if the communication interface is in an error state.
//
//***************************************************************************************
function TXcpTransport.IsComError: Boolean;
var
  statusFlags: Cardinal;
begin
  // init result to no error.
  result := false;

  // do not check if the handle is invalid
  if kvaserHandle <= canINVALID_HANDLE then
  begin
    Exit;
  end;

  // check for bus off error or error passive if connected
  if connected then
  begin
    if canReadStatus(kvaserHandle, statusFlags) = canOK then
    begin
      // check for bus off or error passive bits
      if (statusFlags and (canSTAT_BUS_OFF or canSTAT_ERROR_PASSIVE)) > 0 then
      begin
        result := true;
      end;
    end
    else
    begin
      // could not read the status which is also an indicator that something is wrong
      result := true
    end;
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
  txId: LongInt;
  txData: array[0..kMaxPacketSize-1] of Byte;
  txFlags: Cardinal;
  rxId: LongInt;
  rxData: array[0..kMaxPacketSize-1] of Byte;
  rxFlags: Cardinal;
  rxLen: Cardinal;
  rxTime: Cardinal;
  byteIdx: Byte;
  status: canStatus;
  idTypeOk: Boolean;
begin
  // initialize the result value
  result := false;

  // do not send data when the packet length is invalid or when not connected
  // to the CAN hardware
  if (packetLen > kMaxPacketSize) or (not connected) then
  begin
    Exit;
  end;

  // do not send if the handle is invalid
  if kvaserHandle <= canINVALID_HANDLE then
  begin
    Exit;
  end;

  // prepare the packet for transmission in a CAN message
  txId := packetTxId;
  for byteIdx := 0 to (packetLen - 1) do
  begin
    txData[byteIdx] := packetData[byteIdx];
  end;
  if extendedId then
    txFlags := canMSG_EXT
  else
    txFlags := canMSG_STD;

  // submit the packet for transmission via the CAN bus
  if canWrite(kvaserHandle, txId, @txData[0], packetLen, txFlags) <> canOK then
  begin
    Exit;
  end;

  // reset flag and set the reception timeout time
  responseReceived := false;
  timeoutTime := GetTickCount + timeOutms;

  // attempt to receive the packet response within the timeout time
  repeat
    // prepare message reception
    rxId := packetRxId;
    // attempt to read the packet response from the reception queue
    status := canReadSpecificSkip(kvaserHandle, rxId, @rxData[0], rxLen, rxFlags, rxTime);
    // check if an error was detected
    if (status <> canOK) and (status <> canERR_NOMSG) then
    begin
      // error detected. stop loop.
      Break;
    end;
    // no error, now check if a message was actually received
    if status = canOK then
    begin
      // a message with the identifier of the response packet was received. now check
      // that the identifier type also matches
      idTypeOk := false;
      if extendedId then
      begin
        if (rxFlags and canMSG_EXT) > 0 then
          idTypeOk := true;
      end
      else
      begin
        if (rxFlags and canMSG_STD) > 0 then
          idTypeOk := true;
      end;
      if idTypeOk then
      begin
        // response received. set flag
        responseReceived := true;
      end;
    end;
    // give the application a chance to use the processor
    Application.ProcessMessages;
  until (GetTickCount > timeoutTime) or (responseReceived);

  // check if the response was correctly received
  if responseReceived then
  begin
    // copy the received response packet
    packetLen := rxLen;
    for byteIdx := 0 to (packetLen - 1) do
    begin
      packetData[byteIdx] := rxData[byteIdx];
    end;
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
    // only disconnect if the handle is valid
    if kvaserHandle > canINVALID_HANDLE then
    begin
      // take the channel from the bus
      canBusOff(kvaserHandle);
      // close the channel
      canClose(kvaserHandle);
    end;
  end;
  kvaserHandle := canINVALID_HANDLE;
  connected := false;
end; //*** end of Disconnect ***

end.
//******************************** end of XcpTransport.pas ******************************

