unit XcpTransport;
//***************************************************************************************
//  Description: XCP transport layer for CAN.
//    File Name: XcpTransport.pas
//
//---------------------------------------------------------------------------------------
//                          C O P Y R I G H T
//---------------------------------------------------------------------------------------
//   Copyright (c) 2011 by Feaser    http://www.feaser.com    All rights reserved
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
  Windows, Messages, SysUtils, Classes, Forms, IniFiles, PCANBasic;


//***************************************************************************************
// Global Constants
//***************************************************************************************
// a CAN message can only have up to 8 bytes
const kMaxPacketSize = 8;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  TPCANhardware = ( PCAN_PCI = $40, PCAN_USB = $50, PCAN_PCC = $60 );

  TXcpTransport = class(TObject)
  private
    packetTxId   : LongWord;
    packetRxId   : Longword;
    extendedId   : Boolean;
    canHardware  : TPCANhardware; { PCAN_xxx }
    canChannel   : Word; { currently supported is 1..8 }
    canBaudrate  : Word; { in bits/sec }
    connected    : Boolean;
    function ConstructPeakHandle(hardware: TPCANhardware; channel: Word): TPCANHandle;
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
begin
	// read XCP configuration from INI
  if FileExists(iniFile) then
  begin
    // create ini file object
    settingsIni := TIniFile.Create(iniFile);

    // set hardware configuration
    case settingsIni.ReadInteger('can', 'hardware', 0) of
      0: canHardware := PCAN_USB;
      1: canHardware := PCAN_PCI;
      2: canHardware := PCAN_PCC;
      else
        canHardware := PCAN_USB;
    end;
    canChannel := settingsIni.ReadInteger('can', 'channel', 0) + 1;

    case settingsIni.ReadInteger('can', 'baudrate', 2) of
      0: canBaudrate  := PCAN_BAUD_1M;
      1: canBaudrate  := PCAN_BAUD_800K;
      2: canBaudrate  := PCAN_BAUD_500K;
      3: canBaudrate  := PCAN_BAUD_250K;
      4: canBaudrate  := PCAN_BAUD_125K;
      5: canBaudrate  := PCAN_BAUD_100K;
      6: canBaudrate  := PCAN_BAUD_83K;
      7: canBaudrate  := PCAN_BAUD_33K;
      8: canBaudrate  := PCAN_BAUD_20K;
      9: canBaudrate  := PCAN_BAUD_10K;
      10: canBaudrate := PCAN_BAUD_5K;
      else
        canBaudrate := PCAN_BAUD_500K;
    end;

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
  status: TPCANStatus;
  iBuffer : Integer;
begin
  // init result value
  result := false;

  // disconnect first if still connected
  if connected then
    Disconnect;

  // attempt to connect to the CAN hardware interface
  status := CAN_Initialize(ConstructPeakHandle(canHardware, canChannel), canBaudrate, 0, 0, 0);

  // process the result
  if status = PCAN_ERROR_OK then
  begin
    // connected. now enable the bus off automatic reset
    iBuffer := PCAN_PARAMETER_ON;
    status := CAN_SetValue(ConstructPeakHandle(canHardware, canChannel), PCAN_BUSOFF_AUTORESET,
                           PLongWord(@iBuffer), sizeof(iBuffer));
    if status = PCAN_ERROR_OK then
    begin
      connected := true;
      result := true;
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
  status: TPCANStatus;
begin
  // init result to no error.
  result := false;

  // check for bus off error if connected
  if connected then
  begin
    status := CAN_GetStatus(ConstructPeakHandle(canHardware, canChannel));
    if (status = PCAN_ERROR_BUSOFF) or (status = PCAN_ERROR_BUSHEAVY) then
    begin
      result := true;
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
  txMsg: TPCANMsg;
  rxMsg: TPCANMsg;
  byteIdx: Byte;
  status: TPCANStatus;
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

  // prepare the packet for transmission in a CAN message
  txMsg.ID := packetTxId;
  if extendedId then
    txMsg.MSGTYPE := PCAN_MESSAGE_EXTENDED
  else
    txMsg.MSGTYPE := PCAN_MESSAGE_STANDARD;
  txMsg.LEN := packetLen;
  for byteIdx := 0 to (packetLen-1) do
  begin
    txMsg.DATA[byteIdx] := packetData[byteIdx];
  end;

  // transmit the packet via CAN
  status := CAN_Write(ConstructPeakHandle(canHardware, canChannel), txMsg);
  if status <> PCAN_ERROR_OK then
  begin
    Exit;

  end;

  // reset flag and set the reception timeout time
  responseReceived := false;
  timeoutTime := GetTickCount + timeOutms;

  // attempt to receive the packet response within the timeout time
  repeat
    // read out the next message in the receive queue
    status := CAN_Read(ConstructPeakHandle(canHardware, canChannel), rxMsg, nil);
    // check if an error occurred
    if (status <> PCAN_ERROR_OK) and (status <> PCAN_ERROR_QRCVEMPTY) then
    begin
      // error detected. stop loop.
      Break;
    end
    // no error occurred, so either a message was received or the queue was
    // empty. check for the latter condition
    else if status = PCAN_ERROR_OK then
    begin
      // was the newly received CAN message the response we are waiting for?
      if rxMsg.ID = packetRxId then
      begin
        // was the id type also a match?
        if ((rxMsg.MSGTYPE = PCAN_MESSAGE_STANDARD) and (not extendedId)) or
           ((rxMsg.MSGTYPE = PCAN_MESSAGE_EXTENDED) and (extendedId)) then
        begin
          // response received. set flag
          responseReceived := true;
        end;
      end;
    end;
    // give the application a chance to use the processor
    Application.ProcessMessages;
  until (GetTickCount > timeoutTime) or (responseReceived);

  // check if the response was correctly received
  if responseReceived then
  begin
    // copy the response for futher processing
    packetLen := rxMsg.LEN;
    for byteIdx := 0 to (packetLen-1) do
    begin
      packetData[byteIdx] := rxMsg.DATA[byteIdx];
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
    CAN_Uninitialize(ConstructPeakHandle(canHardware, canChannel));
  end;
  connected := false;
end; //*** end of Disconnect ***


//***************************************************************************************
// NAME:           ConstructPeakHandle
// PARAMETER:      hardware Peak hardware identifier.
//                 channel Peak channel.
// RETURN VALUE:   Peak hardware channel handle.
// DESCRIPTION:    Converts this class' hardware and channel values into a handle that
//                 can be passed to the Peak API.
//
//***************************************************************************************
function TXcpTransport.ConstructPeakHandle(hardware: TPCANhardware; channel: Word): TPCANHandle;
begin
  result := Word(hardware) + channel;
end; //*** end of ConstructPeakHandle ***

end.
//******************************** end of XcpTransport.pas ******************************

