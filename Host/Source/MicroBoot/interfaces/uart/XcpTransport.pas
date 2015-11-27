unit XcpTransport;
//***************************************************************************************
//  Description: XCP transport layer for SCI.
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
  Windows, Messages, SysUtils, Classes, Forms, CPDrv, IniFiles;


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
    sciDriver    : TCommPortDriver;
    constructor Create;
    procedure   Configure(iniFile : string);
    function    Connect : Boolean;
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

  // create a sci driver instance
  sciDriver := TCommPortDriver.Create(nil);

  // init sci settings
  sciDriver.DataBits := db8BITS;
  sciDriver.StopBits := sb1BITS;
  sciDriver.Parity := ptNONE;
  sciDriver.SwFlow := sfNONE;
  sciDriver.PollingDelay := 5;

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
  // release sci driver instance
  sciDriver.Free;

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
  configIndex : integer;
begin
	// read XCP configuration from INI
  if FileExists(iniFile) then
  begin
    // create ini file object
    settingsIni := TIniFile.Create(iniFile);

    // configure baudrate
    configIndex := settingsIni.ReadInteger('sci', 'baudrate', 6);
    sciDriver.BaudRate := br38400; // init to default value
    case configIndex of
      0 : sciDriver.BaudRate := br1200;
      1 : sciDriver.BaudRate := br2400;
      2 : sciDriver.BaudRate := br4800;
      3 : sciDriver.BaudRate := br9600;
      4 : sciDriver.BaudRate := br14400;
      5 : sciDriver.BaudRate := br19200;
      6 : sciDriver.BaudRate := br38400;
      7 : sciDriver.BaudRate := br56000;
      8 : sciDriver.BaudRate := br57600;
      9 : sciDriver.BaudRate := br115200;
      10: sciDriver.BaudRate := br128000;
      11: sciDriver.BaudRate := br256000;
    end;

    // configure port
    configIndex := settingsIni.ReadInteger('sci', 'port', 0);
    sciDriver.Port := pnCOM1; // init to default value
    case configIndex of
      0 : sciDriver.Port := pnCOM1;
      1 : sciDriver.Port := pnCOM2;
      2 : sciDriver.Port := pnCOM3;
      3 : sciDriver.Port := pnCOM4;
      4 : sciDriver.Port := pnCOM5;
      5 : sciDriver.Port := pnCOM6;
      6 : sciDriver.Port := pnCOM7;
      7 : sciDriver.Port := pnCOM8;
    end;

    // release ini file object
    settingsIni.Free;
  end;
end; //*** end of Configure ***


//***************************************************************************************
// NAME:           Connect
// PARAMETER:      none
// RETURN VALUE:   True is successful, False otherwise.
// DESCRIPTION:    Connects the transport layer device.
//
//***************************************************************************************
function TXcpTransport.Connect : Boolean;
begin
  result := true;
  if not sciDriver.Connect then
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

  // during high burst I/O the USB/RS232 emulated COM-ports sometimes have problems
  // processing all the data. therefore, add a small delay time between packet I/O.
  // exclude the CONNECT command because of the default small backdoor time of the
  // bootloader
  if packetData[0] <> $FF then
  begin
    Application.ProcessMessages;
    Sleep(5);
  end;

  // prepare the packet. length goes in the first byte followed by the packet data
  SetLength(msgData, packetLen+1);
  msgData[0] := packetLen;
  for cnt := 0 to packetLen-1 do
  begin
    msgData[cnt+1] := packetData[cnt];
  end;

  // submit the packet transmission request
  if sciDriver.SendData(@msgData[0], packetLen+1) <> (packetLen+1)  then
  begin
    // unable to submit tx request
    Exit;
  end;

  // compute timeout time
  dwEnd := GetTickCount + timeOutms;

  // configure timeout for first byte
  sciDriver.InputTimeout := timeOutms;

  // receive the first byte which holds the packet length
  if sciDriver.ReadByte(resLen) = true then
  begin
    timeOutms := GetTickCount;
    if timeOutms < dwEnd then
    begin
      // configure timeout for remaining bytes
      sciDriver.InputTimeout := dwEnd - timeOutms;
    end
    else
    begin
      Exit; // timed out
    end;

    // receive the actual packet data
    if sciDriver.ReadData(@packetData[0], resLen) = resLen then
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
  sciDriver.Disconnect;
end; //*** end of Disconnect ***


end.
//******************************** end of XcpTransport.pas ******************************

