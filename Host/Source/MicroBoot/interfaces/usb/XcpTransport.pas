unit XcpTransport;
//***************************************************************************************
//  Description: XCP transport layer for USB.
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
  Windows, Messages, SysUtils, Classes, Forms, UsbBulkLib, IniFiles;


//***************************************************************************************
// Global Constants
//***************************************************************************************
const kMaxPacketSize = 256;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  TXcpTransport = class(TObject)
  private
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


//***************************************************************************************
// Constant data declarations
//***************************************************************************************
const
  deviceGuid: tguid = '{807999C3-E4E0-40EA-8188-48E852B54F2B}';
               

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

  // the DLL for UsbBulkLib is automatically loaded, so nothing to be done here

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
  // the DLL for UsbBulkLib is automatically unloaded, so nothing to be done here

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
begin
  // there are no communication specific settings for USB
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
  if UblOpen(Addr(deviceGuid)) <> UBL_OKAY then
    result := false;
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
  result := false;
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
  msgData : array of Byte;
  resLen  : byte;
  cnt     : byte;
  dwEnd   :DWord;
begin
  // init the return value
  result := false;

  // prepare the packet. length goes in the first byte followed by the packet data
  SetLength(msgData, packetLen+1);
  msgData[0] := packetLen;
  for cnt := 0 to packetLen-1 do
  begin
    msgData[cnt+1] := packetData[cnt];
  end;

  // submit the packet transmission request
  if  UblTransmit(@msgData[0], packetLen+1) <> UBL_OKAY then
  begin
    // unable to submit tx request
    Exit;
  end;

  // give application the opportunity to process the messages
  Application.ProcessMessages;

  // compute timeout time
  dwEnd := GetTickCount + timeOutms;

  // configure timeout for first byte
  //sciDriver.InputTimeout := timeOutms;

  // receive the first byte which holds the packet length
  if UblReceive(Addr(resLen), 1, timeOutms) = UBL_OKAY then
  begin
    timeOutms := GetTickCount;
    if timeOutms >= dwEnd then
    begin
      Exit; // timed out
    end;

    // receive the actual packet data
    if UblReceive(Addr(packetData[0]), resLen, dwEnd - timeOutms) = UBL_OKAY then
    begin
      packetLen := resLen;
      result := true;
    end;
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
  UblClose;
end; //*** end of Disconnect ***


end.
//******************************** end of XcpTransport.pas ******************************

