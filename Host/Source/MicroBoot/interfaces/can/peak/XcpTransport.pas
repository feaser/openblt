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
// You should have received a copy of the GNU General Public License along with OpenBLT.
// If not, see <http://www.gnu.org/licenses/>.
//
// A special exception to the GPL is included to allow you to distribute a combined work
// that includes OpenBLT without being obliged to provide the source code for any
// proprietary components. The exception text is included at the bottom of the license
// file <license.html>.
//
//***************************************************************************************
interface


//***************************************************************************************
// Includes
//***************************************************************************************
uses
  Windows, Messages, SysUtils, Classes, Forms, IniFiles, PCANdrvD;


//***************************************************************************************
// Global Constants
//***************************************************************************************
const kMaxPacketSize = 256;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  TXcpTransportInfo = (kNone, kResponse, kError);


type
  TXcpTransport = class(TObject)
  private
    comEventInfo : TXcpTransportInfo;
    comEvent     : THandle;
    packetTxId   : LongWord;
    packetRxId   : Longword;
    extendedId   : Boolean;
    procedure OnCanMessage(Sender: TObject; Direction: TPCanDirection; Message: TPCanMessage);
    function  MsgWaitForSingleObject(hHandle: THandle; dwMilliseconds: DWORD): DWORD;
  public
    packetData   : array[0..kMaxPacketSize-1] of Byte;
    packetLen    : Word;
    pcanDriver   : TPCanDriver;
    constructor Create;
    procedure   Configure(iniFile : string);
    function    Connect: Boolean;
    function    SendPacket(timeOutms: LongWord): Boolean;
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

  // reset can event info
  comEventInfo := kNone;

  // create the event that requires manual reset
  comEvent := CreateEvent(nil, True, False, nil);

  if comEvent = 0 then
    Application.MessageBox( 'Could not obtain event placeholder.',
                            'Error', MB_OK or MB_ICONERROR );

  // create a pcan driver instance
  pcanDriver := TPCanDriver.Create(nil);

  // set can driver event handlers
  pcanDriver.OnMessage := OnCanMessage;

  // reset the packet ids
  packetTxId := 0;
  packetRxId := 0;

  // use standard id's by default
  extendedId := false;

  // reset packet length
  packetLen := 0;
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
  // release can driver instance
  pcanDriver.Free;

  // release event handle
  CloseHandle(comEvent);

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
  hwIndex     : integer;
begin
	// read XCP configuration from INI
  if FileExists(iniFile) then
  begin
    // create ini file object
    settingsIni := TIniFile.Create(iniFile);

    // set message configuration
    packetTxId := settingsIni.ReadInteger('can', 'txid', $667);
    packetRxId := settingsIni.ReadInteger('can', 'rxid', $7e1);
    extendedId := settingsIni.ReadBool('can', 'extended', false);

    // configure can hardware
    hwIndex := settingsIni.ReadInteger('can', 'hardware', 0);
    pcanDriver.Hardware := PCAN_USB1CH; // init to PCAN_USB1CH
    case hwIndex of
      0 : pcanDriver.Hardware := PCAN_USB1CH;
    end;

    // configure baudrate
    pcanDriver.BaudRate := settingsIni.ReadInteger('can', 'baudrate', 500) * 1000;

    // only 1 channel on PCAN USB 1CH
    pcanDriver.Channel := pcanchannel0;

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
  result := true;
  if not pcanDriver.Connect then
    result := false;
end; //*** end of Connect ***


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
  pcanmsg : TPCanMessage;
  cnt : byte;
  waitResult: Integer;
begin
  // prepare the packet
  pcanmsg.id      := LongInt(PacketTxId);
  pcanmsg.dlc     := packetLen;
  pcanmsg.ext     := extendedId;
  for cnt := 0 to packetLen-1 do
  begin
    pcanmsg.data[cnt] := packetData[cnt];
  end;

  // make sure the event is reset
  ResetEvent(comEvent);
  comEventInfo := kNone;

  // submit the packet transmission request
  if not pcanDriver.Transmit(pcanmsg) then
  begin
    // unable to submit tx request
    result := False;
    Exit;
  end;

  // packet is being transmitted. Now wait for the response to come in
  waitResult := MsgWaitForSingleObject(comEvent, timeOutms);

  if waitResult <> WAIT_OBJECT_0 then
  begin
    // no com event triggered so either a timeout or internal error occurred
    result := False;
    Exit;
  end;

  // com event was triggered. now check if the reponse was correctly received
  if comEventInfo <> kResponse then
  begin
    result := False;
    Exit;
  end;

  // packet successfully transmitted and response packet received
  result := True;
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
  pcanDriver.Disconnect;
end; //*** end of Disconnect ***


//***************************************************************************************
// NAME:           OnCanMessage
// PRECONDITIONS:  none
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Can message event handler
//
//***************************************************************************************
procedure TXcpTransport.OnCanMessage(Sender: TObject; Direction: TPCanDirection; Message: TPCanMessage);
var
  cnt : integer;
begin
  // the event we are interested in is the reception of the command response from
  // slave.
  if Direction = PCanRx then
  begin
    if Message.id = LongInt(PacketRxId) then
    begin
      // store response data
      for cnt := 0 to Message.dlc-1 do
      begin
        packetData[cnt] := Message.data[cnt];
      end;

      // store response length
      packetLen := Message.dlc;

      // set event flag
      comEventInfo := kResponse;

      // trigger the event
      SetEvent(comEvent);
    end;
  end;
end; //*** end of OnCanMessage ***


//***************************************************************************************
// NAME:           MsgWaitForSingleObject
// PRECONDITIONS:  none
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Improved version of WaitForSingleObject. This version actually
//                 processes messages in the queue instead of blocking them.
//
//***************************************************************************************
function TXcpTransport.MsgWaitForSingleObject(hHandle: THandle; dwMilliseconds: DWORD): DWORD;
var
  dwEnd:DWord;
begin
  // compute the time when the WaitForSingleObject is supposed to time out
  dwEnd := GetTickCount + dwMilliseconds;

  repeat
    // wait for an event to happen or a message to be in the queue
    result := MsgWaitForMultipleObjects(1, hHandle, False, dwMilliseconds, QS_ALLINPUT);

    // a message was in the queue?
    if result = WAIT_OBJECT_0 + 1 then
    begin
      // process these messages
      Application.ProcessMessages;

      // check for timeout manually because if a message in the queue occurred, the
      // MsgWaitForMultipleObjects will be called again and the timer will start from
      // scratch. we need to make sure the correct timeout time is used.
      dwMilliseconds := GetTickCount;
      if dwMilliseconds < dwEnd then
      begin
        dwMilliseconds := dwEnd - dwMilliseconds;
      end
      else
      begin
        // timeout occured
        result := WAIT_TIMEOUT;
        Break;
      end;
    end
    else
    // the event occured?
    begin
      // we can stop
      Break;
    end;
  until True = False;
end; //*** end of MsgWaitForSingleObject ***


end.
//******************************** end of XcpTransport.pas ******************************

