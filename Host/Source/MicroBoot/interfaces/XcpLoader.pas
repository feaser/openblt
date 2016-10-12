unit XcpLoader;
//***************************************************************************************
//  Description: XCP Master Communication Protocol Layer for Bootloader.
//    File Name: XcpLoader.pas
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
  Windows, Messages, SysUtils, Classes, Forms, XcpTransport, XcpProtection, IniFiles;

//***************************************************************************************
// Global Constants
//***************************************************************************************
// XCP command codes
const kCmdCONNECT         = $FF;
const kCmdDISCONNECT      = $FE;
const kCmdGET_STATUS      = $FD;
const kCmdSYNCH           = $FC;
const kCmdGET_ID          = $FA;
const kCmdGET_SEED        = $F8;
const kCmdUNLOCK          = $F7;
const kCmdSET_MTA         = $F6;
const kCmdUPLOAD          = $F5;
const kCmdSHORT_UPLOAD    = $F4;
const kCmdBUILD_CHECKSUM  = $F3;
const kCmdDOWNLOAD        = $F0;
const kCmdDOWNLOAD_MAX    = $EE;
const kCmdSET_CAL_PAGE    = $EB;
const kCmdGET_CAL_PAGE    = $EA;
const kCmdPROGRAM_START   = $D2;
const kCmdPROGRAM_CLEAR   = $D1;
const kCmdPROGRAM         = $D0;
const kCmdPROGRAM_RESET   = $CF;
const kCmdPROGRAM_PREPARE = $CC;
const kCmdPROGRAM_MAX     = $C9;

// XCP command response packet IDs
const kCmdPidRES  = $FF; // positive response packet
const kCmdPidERR  = $FE; // error packet
const kCmdPidEV   = $FD; // event packet
const kCmdPidSERV = $FC; // service request packet

// XCP resources
const kResPGM   = $10; // programming resource

// XCP error codes
const kErrCMD_SYNCH         = $00; // Command processor synchronization
const kErrCMD_BUSY          = $10; // Command was not executed.
const kErrDAQ_ACTIVE        = $11; // Command rejected because DAQ is running.
const kErrPGM_ACTIVE        = $12; // Command rejected because PGM is running.
const kErrCMD_UNKNOWN       = $20; // Unknown command or not implemented optional command.
const kErrCMD_SYNTAX        = $21; // Command syntax invalid
const kErrOUT_OF_RANGE      = $22; // Command syntax valid but command parameter(s) out of range.
const kErrWRITE_PROTECTED   = $23; // The memory location is write protected.
const kErrACCESS_DENIED     = $24; // The memory location is not accessible.
const kErrACCESS_LOCKED     = $25; // Access denied, Seed & Key is required
const kErrPAGE_NOT_VALID    = $26; // Selected page not available
const kErrMODE_NOT_VALID    = $27; // Selected page mode not available
const kErrSEGMENT_NOT_VALID = $28; // Selected segment not valid
const kErrSEQUENCE          = $29; // Sequence error
const kErrDAQ_CONFIG        = $2A; // DAQ configuration not valid
const kErrMEMORY_OVERFLOW   = $30; // Memory overflow error
const kErrGENERIC           = $31; // Generic error
const kErrVERIFY            = $32; // The slave internal program verify routine detects an error.

// Feaser error Codes
const kErrFsrExecuteCmd          = $80; // Could not execute command
const kErrFsrResourceUnavailable = $81; // Resource needed but not available
const kErrFsrSeedKeyDllInvalid   = $82; // Seed/Key DLL is invalid
const kErrFsrKeyAlgoMissing      = $83; // Key computation algorithm is missing

// Start programming session return codes
const kProgSessionStarted        = 0;
const kProgSessionUnlockError    = 1;
const kProgSessionGenericError   = 2;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  TXcpLoader = class(TObject)
  private
    FIsConnected     : Boolean;
    FTimerInterval   : array[1..7] of Word;
    FConnectCmdTimer : Word;
    FIsIntel         : Boolean;
    FCtoPacketLen    : Byte;
    FCtoPGMPacketLen : Byte;
    FDtoPacketLen    : Word;
    FSeedKeyDll      : string;
    FLastError       : Byte;
    FResources       : Byte;
    FProtection      : Byte;
    FMta             : LongWord;
    procedure   WaitT7;
    function    GetOrderedWord(data : PByteArray) : Word;
    procedure   SetOrderedLong(value: LongWord; data : PByteArray);
    function    SendSynchedPacket(timeMs : Word; useMta : Boolean) : Boolean;
    function    CmdSynch(useMta : Boolean) : Boolean;
    function    CmdConnect : Boolean;
    function    CmdDisconnect : Boolean;
    function    CmdProgramStart : Boolean;
    function    CmdGetStatus : Boolean;
    function    CmdGetSeed(seed : PByteArray; resource : Byte; var len : Byte) : Boolean;
    function    CmdUnlock(key : PByteArray; len : Byte) : Boolean;
    function    CmdProgramReset : Boolean;
    function    CmdProgram(data : PByteArray; len : Byte) : Boolean;
    function    CmdProgramMax(data : PByteArray) : Boolean;
    function    CmdSetMta(addr : LongWord) : Boolean;
    function    CmdProgramClear(len : LongWord) : Boolean;
  public
    comDriver        : TXcpTransport;
    constructor Create;
    destructor  Destroy; override;
    function    GetLastError(var info : string) : Byte;
    procedure   Configure(iniFile : string);
    function    Connect : Boolean;
    function    IsComError : Boolean;
    procedure   Disconnect;
    function    StartProgrammingSession : Byte;
    function    StopProgrammingSession : Boolean;
    function    ClearMemory(addr : LongWord; len : LongWord) : Boolean;
    function    WriteData(addr : LongWord; len : LongWord; data : PByteArray) : Boolean;
  end;


implementation
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor
//
//***************************************************************************************
constructor TXcpLoader.Create;
begin
  // call inherited constructor
  inherited Create;

  // reset error
  FLastError := 0;

  // reset memory transfer address
  FMta := 0;

  // not connected upon creation
  FIsConnected := false;

  // reset seed/key dll filename
  FSeedKeyDll := '';

  // set communication defaults
  FIsIntel    := False; // motorola byte order by default
  FResources  := 0;     // no resources available
  FProtection := 0;     // all resources unprotected by default

  // set XCP packet length defaults
  FCtoPacketLen := 8; // must be at least 8 for connect command response
  FDtoPacketLen := 8;
  FCtoPGMPacketLen := 8;

  // set interval time defaults
  FTimerInterval[1] :=  1000; // t1 =  1000ms - standard command timeout
  FTimerInterval[2] :=  2000; // t2 =  2000ms - build checksum timeout
  FTimerInterval[3] :=  2000; // t3 =  2000ms - program start timeout
  FTimerInterval[4] := 10000; // t4 = 10000ms - erase timeout
  FTimerInterval[5] :=  1000; // t5 =  1000ms - write and reset timeout
  FTimerInterval[6] :=  1000; // t6 =  1000ms - user specific connect
  FTimerInterval[7] :=  2000; // t7 =  2000ms - wait timer
  // the connect command does not have a protocol specified timeout value. However, this
  // timeout is important for the OpenBLT timed backdoor feature. The backdoor time should
  // be at least 2.5 times the length of this timeout value.
  FConnectCmdTimer  :=  20;   // 20 ms - connect command

  // create instance of XCP transport layer object
  comDriver := TXcpTransport.Create;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Destroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class destructor
//
//***************************************************************************************
destructor TXcpLoader.Destroy;
begin
  // disconnect the XCP transport layer
  comDriver.Disconnect;

  // release XCP transport layer object
  comDriver.Free;

  // call inherited destructor
  inherited;
end; //*** end of Destroy ***


//***************************************************************************************
// NAME:           WaitRoutine
// PARAMETER:      number of milliseconds to wait
// RETURN VALUE:   none
// DESCRIPTION:    Basic routine that waits for the specified amount of time before
//                 continueing.
//
//***************************************************************************************
procedure TXcpLoader.WaitT7;
begin
  Sleep(FTimerInterval[7]);
end; //*** end of WaitRoutine ***


//***************************************************************************************
// NAME:           GetOrderedWord
// PARAMETER:      pointer to byte array
// RETURN VALUE:   word value
// DESCRIPTION:    Returns the word value from the byte array taking into account Intel
//                 or Motorola byte ordering.
//
//***************************************************************************************
function TXcpLoader.GetOrderedWord(data : PByteArray) : Word;
begin
  result := 0;

  if FIsIntel then
  begin
    result := result or (data[1] shl 8);
    result := result or (data[0]);
  end
  else
  begin
    result := result or (data[0] shl 8);
    result := result or (data[1]);
  end;
end; //*** end of GetOrderedWord ***


//***************************************************************************************
// NAME:           SetOrderedLong
// PARAMETER:      pointer to byte array and 32-bit value
// RETURN VALUE:   none
// DESCRIPTION:    Stores a 32-bit value into a byte buffer taking into account Intel
//                 or Motorola byte ordering.
//
//***************************************************************************************
procedure TXcpLoader.SetOrderedLong(value: LongWord; data : PByteArray);
begin
  if FIsIntel then
  begin
    data[3] := Byte(value shr 24);
    data[2] := Byte(value shr 16);
    data[1] := Byte(value shr  8);
    data[0] := Byte(value);
  end
  else
  begin
    data[0] := Byte(value shr 24);
    data[1] := Byte(value shr 16);
    data[2] := Byte(value shr  8);
    data[3] := Byte(value);
  end;
end; //*** end of SetOrderedLong ***


//***************************************************************************************
// NAME:           GetLastError
// PARAMETER:      destination string from error information
// RETURN VALUE:   error code
// DESCRIPTION:    Return the last error value.
//
//***************************************************************************************
function TXcpLoader.GetLastError(var info : string) : Byte;
begin
  // set info string
  case FLastError of
    kErrCMD_SYNCH             : info := '0x00 -  Command processor synchronization';
    kErrCMD_BUSY              : info := '0x10 -  Command was not executed';
    kErrDAQ_ACTIVE            : info := '0x11 -  Command rejected because DAQ is running';
    kErrPGM_ACTIVE            : info := '0x12 -  Command rejected because PGM is running';
    kErrCMD_UNKNOWN           : info := '0x20 -  Unknown command or not implemented optional command';
    kErrCMD_SYNTAX            : info := '0x21 -  Command syntax invalid';
    kErrOUT_OF_RANGE          : info := '0x22 -  Command syntax valid but command parameter(s) out of range';
    kErrWRITE_PROTECTED       : info := '0x23 -  The memory location is write protected';
    kErrACCESS_DENIED         : info := '0x24 -  The memory location is not accessible';
    kErrACCESS_LOCKED         : info := '0x25 -  Access denied, Seed & Key is required';
    kErrPAGE_NOT_VALID        : info := '0x26 -  Selected page not available';
    kErrMODE_NOT_VALID        : info := '0x27 -  Selected page mode not available';
    kErrSEGMENT_NOT_VALID     : info := '0x28 -  Selected segment not valid';
    kErrSEQUENCE              : info := '0x29 -  Sequence error';
    kErrDAQ_CONFIG            : info := '0x2A -  DAQ configuration not valid';
    kErrMEMORY_OVERFLOW       : info := '0x30 -  Memory overflow error';
    kErrGENERIC               : info := '0x31 -  Generic error';
    kErrVERIFY                : info := '0x32 -  The slave internal program verify routine detects an error';
    kErrFsrExecuteCmd         : info := '0x80 -  Could not execute command';
    kErrFsrResourceUnavailable: info := '0x81 -  Resource needed but not available';
    kErrFsrSeedKeyDllInvalid  : info := '0x82 -  Seed/Key DLL is invalid';
    kErrFsrKeyAlgoMissing     : info := '0x83 -  Key computation algorithm is missing';
  end;

  // return the error code
  result := FLastError;
end; //*** end of GetLastError ***


//***************************************************************************************
// NAME:           Configure
// PARAMETER:      filename of the INI
// RETURN VALUE:   none
// DESCRIPTION:    Configures both this class and the transport layer from the settings
//                 in the INI.
//
//***************************************************************************************
procedure TXcpLoader.Configure(iniFile : string);
var
  settingsIni : TIniFile;
  wasConnected : Boolean;
begin
  // backup connection state
  wasConnected := FIsConnected;

  // disconnect
  if FIsConnected then DisConnect;

  // configure comDriver
  comDriver.Configure(iniFile);

  // read XCP configuration from INI
  if FileExists(iniFile) then
  begin
    // create ini file object
    settingsIni := TIniFile.Create(iniFile);

    FSeedKeyDll := settingsIni.ReadString('xcp', 'seedkey', ExtractFilePath(ParamStr(0))+'FeaserKey.dll');

    // if no path specified, then assume dll is located in the executable's path
    if ExtractFilePath(FSeedKeyDll) = '' then
      FSeedKeyDll := ExtractFilePath(ParamStr(0))+FSeedKeyDll;

    FTimerInterval[1] := settingsIni.ReadInteger('xcp', 't1', 1000);
    FTimerInterval[3] := settingsIni.ReadInteger('xcp', 't3', 2000);
    FTimerInterval[4] := settingsIni.ReadInteger('xcp', 't4', 10000);
    FTimerInterval[5] := settingsIni.ReadInteger('xcp', 't5', 1000);
    FTimerInterval[7] := settingsIni.ReadInteger('xcp', 't7', 2000);
    FConnectCmdTimer := settingsIni.ReadInteger('xcp', 'tconnect', 20);

    // release ini file object
    settingsIni.Free;
  end;

  // restore connection
  if WasConnected then Connect;
end; //*** end of Configure ***


//***************************************************************************************
// NAME:           Connect
// PARAMETER:      none
// RETURN VALUE:   True if connected, False otherwise.
// DESCRIPTION:    Connects the XCP transport layer
//
//***************************************************************************************
function TXcpLoader.Connect : Boolean;
begin
  // connect the XCP transport layer
  if comDriver.Connect = true then
  begin
    FIsConnected := true;
    result := true;
  end
  else
  begin
    FIsConnected := false;
    result := false;
  end;
end; //*** end of Connect ***


//***************************************************************************************
// NAME:           Disconnect
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Disconnects the XCP transport layer
//
//***************************************************************************************
procedure TXcpLoader.Disconnect;
begin
  // disconnect the XCP transport layer
  FIsConnected := false;
  comDriver.Disconnect;
end; //*** end of Disconnect ***


//***************************************************************************************
// NAME:           IsComError
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Determines if a communication error is present in the transport layer.
//
//***************************************************************************************
function TXcpLoader.IsComError : Boolean;
begin
  result := comDriver.IsComError;
end;


//***************************************************************************************
// NAME:           SendSynchedPacket
// PARAMETER:      timeout time in ms and info if mta should be resend
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Sends out the XCP packet using the "2 Retry with SYNCH" method at
//                 outlined in the XCP protocol error handling. in case an error
//                 occurred, a FLastError will be set. If useMta = true then a SET_MTA
//                 packet will be send right after the SYNCH packet.
//
//***************************************************************************************
function TXcpLoader.SendSynchedPacket(timeMs : Word; useMta : Boolean) : Boolean;
var
  dataCpy : array of Byte;
  cnt     : Word;
begin
  // init return value
  Result := false;

  // validate packet length. it must always be > 0
  if comDriver.packetLen = 0 then
    Exit;

  // make a copy of the packet data because the synch command could overwrite it
  SetLength(dataCpy, comDriver.packetLen);
  for cnt := 0 to comDriver.packetLen-1 do
    dataCpy[cnt] := comDriver.packetData[cnt];

  // send out the command with t1 timeout
  if not comDriver.SendPacket(timeMs) then
  begin
    CmdSynch(useMta); // perform pre-action for 1st retry

    // prepare to send the command packet again
    comDriver.packetLen := Length(dataCpy);
    for cnt := 0 to comDriver.packetLen-1 do
      comDriver.packetData[cnt] := dataCpy[cnt];

    if not comDriver.SendPacket(timeMs) then
    begin
      CmdSynch(useMta); // perform pre-action for 2nd and last retry

      // prepare to send the command packet again
      comDriver.packetLen := Length(dataCpy);
      for cnt := 0 to comDriver.packetLen-1 do
        comDriver.packetData[cnt] := dataCpy[cnt];

      if comDriver.SendPacket(timeMs) then
        result := true; // success
    end
    else
      result := true; // success
  end
  else
    result := true; // sucess

  if result = false then
    FLastError := kErrFsrExecuteCmd; // Could not execute command
end; //*** end of SendSynchedPacket ***


//***************************************************************************************
// NAME:           CmdSynch
// PARAMETER:      useMta is a SET_MTA should be included
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Sends out the synchronise command
//
//***************************************************************************************
function TXcpLoader.CmdSynch(useMta : Boolean) : Boolean;
begin
  // init return value
  Result := false;

  // prepare the command packet
  comDriver.packetData[0] := kCmdSYNCH;
  comDriver.packetLen := 1;

  // send out the command with t1 timeout
  if not comDriver.SendPacket(FTimerInterval[1]) then
  begin
    Exit;
  end;

  // is response an error packet as expected?
  if comDriver.packetData[0] = kCmdPidERR then
  begin
    // is it the expected processor synchronization error?
    if comDriver.packetData[1] = kErrCMD_SYNCH then
    begin
      result := true;
    end;
  end;

  // should MTA be resend aswell?
  if (useMta = true) and (result = true) then
  begin
    // prepare the command packet
    comDriver.packetData[0] := kCmdSET_MTA;
    comDriver.packetData[1] := 0; // reserved
    comDriver.packetData[2] := 0; // reserved
    comDriver.packetData[3] := 0; // address extension not supported

    // set address taking into account byte ordering
    SetOrderedLong(FMta, @comDriver.packetData[4]);

    comDriver.packetLen := 8;

    // send packet with SYNCH retry feature
    if not SendSynchedPacket(FTimerInterval[1], false) then
    begin
      result := false;
      Exit;
    end;
  end;
end; //*** end of CmdSynch ***


//***************************************************************************************
// NAME:           CmdConnect
// PARAMETER:      none
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Connects the XCP slave to start the XCP session
//
//***************************************************************************************
function TXcpLoader.CmdConnect : Boolean;
begin
  // init return value
  Result := false;

  // prepare the connect command packet
  comDriver.packetData[0] := kCmdCONNECT;
  comDriver.packetData[1] := 0; // normal mode
  comDriver.packetLen := 2;

  // send out the command with 20ms timeout. note that this timeout is not required at
  // all by the XCP protocol. here it is set quite short to accomodate the OpenBTL
  // bootloader default backdoor entry feature
  if comDriver.SendPacket(FConnectCmdTimer) then
  begin
    // check to see if it was an error packet
    if comDriver.packetData[0] = kCmdPidERR then
    begin
      // store error and stop
      FLastError := comDriver.packetData[1];
      Exit;
    end;

    // store byte order configuration
    if (comDriver.packetData[2] and $01) = $00 then FIsIntel := true;

    // store available resources
    FResources := comDriver.packetData[1];

    // store cto packet length
    FCtoPacketLen := comDriver.packetData[3];
    FCtoPGMPacketLen := FCtoPacketLen;

    // store dto packet length
    FDtoPacketLen := GetOrderedWord(@comDriver.packetData[4]);

    // success
    result := true;
  end
  else
  begin
    FLastError := kErrFsrExecuteCmd; // Could not execute command;
  end;
end; //*** end of CmdConnect ***


//***************************************************************************************
// NAME:           CmdDisconnect
// PARAMETER:      none
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Disconnects the XCP slave to end the XCP session
//
//***************************************************************************************
function TXcpLoader.CmdDisconnect : Boolean;
begin
  // init return value
  Result := false;

  // prepare the disconnect command packet
  comDriver.packetData[0] := kCmdDISCONNECT;
  comDriver.packetLen := 1;

  // send packet with SYNCH retry feature
  if not SendSynchedPacket(FTimerInterval[1], false) then Exit;

  // was the response an error packet?
  if comDriver.packetData[0] = kCmdPidERR then
  begin
    // busy or programming active error received?
    if (comDriver.packetData[1] = kErrCMD_BUSY) or
       (comDriver.packetData[1] = kErrPGM_ACTIVE) then
    begin
      WaitT7; // wait the predescribed time
      result := CmdDisconnect; // repeat this command
      Exit;
    end
    else
    begin
      FLastError := comDriver.packetData[1]; // Store error info
      Exit;
    end;
  end;

  // no error so it must have been a positive response
  result := true;
end; //*** end of CmdDisconnect ***


//***************************************************************************************
// NAME:           CmdProgramStart
// PARAMETER:      none
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Informs the slave that programming of non volatile memory starts.
//
//***************************************************************************************
function TXcpLoader.CmdProgramStart : Boolean;
begin
  // init return value
  Result := false;

  // prepare the command packet
  comDriver.packetData[0] := kCmdPROGRAM_START;
  comDriver.packetLen := 1;

  // send packet with SYNCH retry feature
  if not SendSynchedPacket(FTimerInterval[3], false) then Exit;

  // was the response an error packet?
  if comDriver.packetData[0] = kCmdPidERR then
  begin
    // busy or programming active error received?
    if comDriver.packetData[1] = kErrCMD_BUSY then
    begin
      WaitT7; // wait the predescribed time
      result := CmdProgramStart; // repeat this command
      Exit;
    end
    else
    begin
      FLastError := comDriver.packetData[1]; // Store error info
      Exit;
    end;
  end;

  // no error so it must have been a positive response
  result := true;

  // update max cto packet length in programming mode if supported
  if comDriver.packetData[3] <> 0 then
    FCtoPGMPacketLen := comDriver.packetData[3];
end; //*** end of CmdProgramStart ***


//***************************************************************************************
// NAME:           CmdGetStatus
// PARAMETER:      none
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Obtains the resource protection info from the XCP slave
//
//***************************************************************************************
function TXcpLoader.CmdGetStatus : Boolean;
begin
  // init return value
  result := false;

  // prepare the get status command packet
  comDriver.packetData[0] := kCmdGET_STATUS;
  comDriver.packetLen := 1;

  // send packet with SYNCH retry feature
  if not SendSynchedPacket(FTimerInterval[1], false) then Exit;

  // was the response an error packet?
  if comDriver.packetData[0] = kCmdPidERR then
  begin
    FLastError := comDriver.packetData[1]; // Store error info
    Exit;
  end;

  // no error so it must have been a positive response. this response comes right after
  // the one from the connect command, which might be send out multiple time so make sure
  // that this is really a response to get_status by verifying its length.
  if comDriver.packetLen = 6 then
  begin
    result := true;
  end;

  // store protection info
  FProtection := comDriver.packetData[2];
end; //*** end of CmdGetStatus ***


//***************************************************************************************
// NAME:           CmdGetSeed
// PARAMETER:      seed destination buffer and the resource
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Obtains the seed from the specified resource
//
//***************************************************************************************
function TXcpLoader.CmdGetSeed(seed : PByteArray; resource : Byte; var len : Byte) : Boolean;
var
  cnt : byte;
begin
  // init return value
  result := false;

  // prepare the get seed command packet
  comDriver.packetData[0] := kCmdGET_SEED;
  comDriver.packetData[1] := 0; // seeds of up to six bytes are supported
  comDriver.packetData[2] := resource;
  comDriver.packetLen := 3;

  // send packet with SYNCH retry feature
  if not SendSynchedPacket(FTimerInterval[1], false) then Exit;

  // was the response an error packet?
  if comDriver.packetData[0] = kCmdPidERR then
  begin
    // busy or programming active error received?
    if (comDriver.packetData[1] = kErrCMD_BUSY) or
       (comDriver.packetData[1] = kErrPGM_ACTIVE) then
    begin
      WaitT7; // wait the predescribed time
      result := CmdGetSeed(seed, resource, len); // repeat this command
      Exit;
    end
    else
    begin
      FLastError := comDriver.packetData[1]; // Store error info
      Exit;
    end;
  end;

  // no error so it must have been a positive response
  result := true;

  // now store the seed info
  len := comDriver.packetData[1];
  for cnt := 0 to len-1 do
    seed[cnt] := comDriver.packetData[cnt+2];
end; //*** end of CmdGetSeed ***


//***************************************************************************************
// NAME:           CmdUnlock
// PARAMETER:      key source buffer and key length
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Unlocks the resource by sending the key
//
//***************************************************************************************
function TXcpLoader.CmdUnlock(key : PByteArray; len : Byte) : Boolean;
var
  cnt : byte;
begin
  // init return value
  result := false;

  // prepare the command packet
  comDriver.packetData[0] := kCmdUNLOCK;
  comDriver.packetData[1] := len; // key length
  for cnt := 0 to len-1 do
    comDriver.packetData[cnt+2] := key[cnt];
  comDriver.packetLen := len + 2;

  // send packet with SYNCH retry feature
  if not SendSynchedPacket(FTimerInterval[1], false) then Exit;

  // was the response an error packet?
  if comDriver.packetData[0] = kCmdPidERR then
  begin
    // busy or programming active error received?
    if (comDriver.packetData[1] = kErrCMD_BUSY) or
       (comDriver.packetData[1] = kErrPGM_ACTIVE) then
    begin
      WaitT7; // wait the predescribed time
      result := CmdUnlock(key, len); // repeat this command
      Exit;
    end
    else
    begin
      FLastError := comDriver.packetData[1]; // Store error info
      Exit;
    end;
  end;

  // no error so it must have been a positive response
  result := true;

  // store the new resource protection mask
  FProtection := comDriver.packetData[1];
end; //*** end of CmdUnlock ***


//***************************************************************************************
// NAME:           CmdProgramReset
// PARAMETER:      none
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Requests the ECU to perform a reset.
//
//***************************************************************************************
function TXcpLoader.CmdProgramReset : Boolean;
begin
  // init return value
  result := false;

  // prepare the command packet
  comDriver.packetData[0] := kCmdPROGRAM_RESET;
  comDriver.packetLen := 1;

  // send packet without SYNCH retry feature. ignore negative return value because this
  // command does not require a response
  if not comDriver.SendPacket(FTimerInterval[5]) then
  begin
    result := true; // ok to not have a response
    Exit; // no response to process to stop here
  end;

  // was the response an error packet?
  if comDriver.packetData[0] = kCmdPidERR then
  begin
    // busy or programming active error received?
    if (comDriver.packetData[1] = kErrCMD_BUSY) or
       (comDriver.packetData[1] = kErrPGM_ACTIVE) then
    begin
      WaitT7; // wait the predescribed time
      result := CmdProgramReset; // repeat this command
      Exit;
    end
    else
    begin
      FLastError := comDriver.packetData[1]; // Store error info
      Exit;
    end;
  end;

  // no error so it must have been a positive response
  result := true;
end; //*** end of CmdProgramReset ***


//***************************************************************************************
// NAME:           CmdProgram
// PARAMETER:      data source buffer and data length
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Programs the data into non-volatile memory
//
//***************************************************************************************
function TXcpLoader.CmdProgram(data : PByteArray; len : Byte) : Boolean;
var
  cnt : byte;
begin
  // init return value
  result := false;

  // prepare the command packet
  comDriver.packetData[0] := kCmdPROGRAM;
  comDriver.packetData[1] := len; // key length

  if len > 0 then
  begin
    for cnt := 0 to len-1 do
      comDriver.packetData[cnt+2] := data[cnt];
  end;
  comDriver.packetLen := len + 2;

  // send packet with SYNCH retry feature
  if not SendSynchedPacket(FTimerInterval[5], true) then Exit;

  // was the response an error packet?
  if comDriver.packetData[0] = kCmdPidERR then
  begin
    // busy or programming active error received?
    if (comDriver.packetData[1] = kErrCMD_BUSY) then
    begin
      WaitT7; // wait the predescribed time
      result := CmdProgram(data, len); // repeat this command
      Exit;
    end
    else
    begin
      FLastError := comDriver.packetData[1]; // Store error info
      Exit;
    end;
  end;

  // no error so it must have been a positive response
  result := true;
end; //*** end of CmdProgram ***


//***************************************************************************************
// NAME:           CmdProgramMax
// PARAMETER:      data source buffer
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Programs the data into non-volatile memory
//
//***************************************************************************************
function TXcpLoader.CmdProgramMax(data : PByteArray) : Boolean;
var
  cnt : byte;
begin
  // init return value
  result := false;

  // prepare the command packet
  comDriver.packetData[0] := kCmdPROGRAM_MAX;
  for cnt := 0 to FCtoPGMPacketLen-2 do
    comDriver.packetData[cnt+1] := data[cnt];
  comDriver.packetLen := FCtoPGMPacketLen;

  // send packet with SYNCH retry feature
  if not SendSynchedPacket(FTimerInterval[5], true) then Exit;

  // was the response an error packet?
  if comDriver.packetData[0] = kCmdPidERR then
  begin
    // busy or programming active error received?
    if (comDriver.packetData[1] = kErrCMD_BUSY) then
    begin
      WaitT7; // wait the predescribed time
      result := CmdProgramMax(data); // repeat this command
      Exit;
    end
    else
    begin
      FLastError := comDriver.packetData[1]; // Store error info
      Exit;
    end;
  end;

  // no error so it must have been a positive response
  result := true;
end; //*** end of CmdProgramMax ***


//***************************************************************************************
// NAME:           CmdSetMta
// PARAMETER:      32-bit address
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Sets the memory transfer address
//
//***************************************************************************************
function TXcpLoader.CmdSetMta(addr : LongWord) : Boolean;
begin
  // init return value
  result := false;

  // prepare the command packet
  comDriver.packetData[0] := kCmdSET_MTA;
  comDriver.packetData[1] := 0; // reserved
  comDriver.packetData[2] := 0; // reserved
  comDriver.packetData[3] := 0; // address extension not supported

  // set address taking into account byte ordering
  SetOrderedLong(addr, @comDriver.packetData[4]);

  comDriver.packetLen := 8;

  // send packet with SYNCH retry feature
  if not SendSynchedPacket(FTimerInterval[1], false) then Exit;

  // was the response an error packet?
  if comDriver.packetData[0] = kCmdPidERR then
  begin
    // busy or programming active error received?
    if (comDriver.packetData[1] = kErrCMD_BUSY) or
       (comDriver.packetData[1] = kErrPGM_ACTIVE) then
    begin
      WaitT7; // wait the predescribed time
      result := CmdSetMta(addr); // repeat this command
      Exit;
    end
    else
    begin
      FLastError := comDriver.packetData[1]; // Store error info
      Exit;
    end;
  end;

  // no error so it must have been a positive response
  result := true;

  // store current memory transfer address
  FMta := addr;
end; //*** end of CmdSetMta ***


//***************************************************************************************
// NAME:           CmdProgramClear
// PARAMETER:      number of bytes in memory to clear
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Clears the number of bytes in non-volatile memory starting at the
//                 mta address.
//
//***************************************************************************************
function TXcpLoader.CmdProgramClear(len : LongWord) : Boolean;
begin
  // init return value
  result := false;

  // prepare the command packet
  comDriver.packetData[0] := kCmdPROGRAM_CLEAR;
  comDriver.packetData[1] := 0; // use absolute mode
  comDriver.packetData[2] := 0; // reserved
  comDriver.packetData[3] := 0; // reserved

  // set address taking into account byte ordering
  SetOrderedLong(len, @comDriver.packetData[4]);

  comDriver.packetLen := 8;

  // send packet with SYNCH retry feature
  if not SendSynchedPacket(FTimerInterval[4], true) then Exit;

  // was the response an error packet?
  if comDriver.packetData[0] = kCmdPidERR then
  begin
    // busy or programming active error received?
    if comDriver.packetData[1] = kErrCMD_BUSY then
    begin
      WaitT7; // wait the predescribed time
      result := CmdProgramClear(len); // repeat this command
      Exit;
    end
    else
    begin
      FLastError := comDriver.packetData[1]; // Store error info
      Exit;
    end;
  end;

  // no error so it must have been a positive response
  result := true;
end; //*** end of CmdProgramClear ***


//***************************************************************************************
// NAME:           StartProgrammingSession
// PARAMETER:      none
// RETURN VALUE:   kProgSessionStarted if successful, kProgSessionUnlockError in case
//                 the PGM resource could not be unlocked or kProgSessionGenericError.
// DESCRIPTION:    Starts the programming session using the following XCP command
//                 sequence:
//                   * CONNECT
//                   * GET_STATUS
//                   * GETSEED (if applicable)
//                   * UNLOCK  (if applicable)
//                   * PROGRAM_START
//
//***************************************************************************************
function TXcpLoader.StartProgrammingSession : Byte;
var
  xcpProtection : TXcpProtection;
  supportedRes  : Byte;
  seedData      : array[0..5] of Byte;
  seedLen       : byte;
  keyData       : array[0..5] of Byte;
  keyLen        : byte;
begin
  // send the CONNECT command
  if not CmdConnect then
  begin
    result := kProgSessionGenericError;
    Exit;
  end;

  // make sure the programming resource is supported
  if (FResources and kResPGM) <> kResPGM then
  begin
    FLastError := kErrFsrResourceUnavailable;
    result := kProgSessionGenericError;
    Exit;
  end;

  // send the GET_STATUS command
  if not CmdGetStatus then
  begin
    result := kProgSessionGenericError;
    Exit;
  end;

  // check if we need to unlock the programming resource
  if (FProtection and kResPGM) = kResPGM then
  begin
    // ceate xcp protection object
    xcpProtection := TXcpProtection.Create(FSeedKeyDll);

    // make sure it contains the unlock algorithm for the PGM resource
    if xcpProtection.GetPrivileges(@supportedRes) <> 0 then
    begin
      FLastError := kErrFsrSeedKeyDllInvalid; // error calling DLL function
      result := kProgSessionUnlockError;
      xcpProtection.Free; // release the object
      Exit;
    end;
    if (supportedRes and kResPGM) <> kResPGM then
    begin
      FLastError := kErrFsrKeyAlgoMissing; // key algorithm not present
      result := kProgSessionUnlockError;
      xcpProtection.Free; // release the object
      Exit;
    end;

    // obtain the seed for the programming resource
    if not CmdGetSeed(@seedData, kResPGM, seedLen) then
    begin
      result := kProgSessionUnlockError;
      xcpProtection.Free; // release the object
      Exit;
    end;

    // compute the key
    keyLen := Length(keyData);
    if xcpProtection.ComputKeyFromSeed(kResPGM, seedLen, @seedData, @keyLen, @keyData) <> 0 then
    begin
      FLastError := kErrFsrSeedKeyDllInvalid; // error calling DLL function
      result := kProgSessionUnlockError;
      xcpProtection.Free; // release the object
      Exit;
    end;

    // release the object..no longer needed
    xcpProtection.Free;

    // we have the key so now unlock the resource
    if not CmdUnlock(@keyData, keyLen) then
    begin
      result := kProgSessionUnlockError;
      Exit;
    end;

    // make sure the PGM resource is really unprotected now
    if (FProtection and kResPGM) = kResPGM then
    begin
      FLastError := kErrACCESS_LOCKED;
      result := kProgSessionUnlockError;
      Exit;
    end;
  end;

  // send the PROGRAM_START command
  if not CmdProgramStart then
  begin
    result := kProgSessionGenericError;
    Exit;
  end;

  // successfully started the programming session
  result := kProgSessionStarted;
end; //*** end of StartProgrammingSession ***


//***************************************************************************************
// NAME:           StopProgrammingSession
// PARAMETER:      none
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Stops the programming session using the following XCP command
//                 sequence:
//                   * PROGRAM (size=0)
//                   * PROGRAM_RESET
//
//***************************************************************************************
function TXcpLoader.StopProgrammingSession : Boolean;
begin
  // init return value
  result := false;

  // send the program command with size 0 to indicate end of programming session
  if not CmdProgram(nil, 0) then Exit;

  // finish off by resetting the ECU
  if not CmdProgramReset then Exit;

  // successfully stopped the programming session
  result := true;
end; //*** end of StopProgrammingSession ***


//***************************************************************************************
// NAME:           ClearMemory
// PARAMETER:      start address and the number of bytes to clear
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Clears the specified memory range using the following XCP command
//                 sequence:
//                   * SET_MTA
//                   * PROGRAM_CLEAR
//
//***************************************************************************************
function TXcpLoader.ClearMemory(addr : LongWord; len : LongWord) : Boolean;
begin
  // init return value
  result := false;

  // set the start address for the erase operation
  if not CmdSetMta(addr) then Exit;

  // finish off by resetting the ECU
  if not CmdProgramClear(len) then Exit;

  // successfully cleared the memory
  result := true;
end; //*** end of ClearMemory ***


//***************************************************************************************
// NAME:           WriteData
// PARAMETER:      start address, the number of bytes to program, and the data buffer
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Programs specified memory range using the following XCP command
//                 sequence:
//                   * SET_MTA
//                   * PROGRAM(_MAX)
//
//***************************************************************************************
function TXcpLoader.WriteData(addr : LongWord; len : LongWord; data : PByteArray) : Boolean;
var
  currentWriteCnt : Byte;
  bufferOffset    : LongWord;
begin
  // init return value
  result := false;

  // validate FCtoPGMPacketLen because using it to prevent possible divide by 0
  if FCtoPGMPacketLen = 0 then
    exit;

  // set the start address for the program operation
  if not CmdSetMta(addr) then Exit;

  // init buffer indexer
  bufferOffset := 0;

  while len > 0 do
  begin
    // set the current write length to make optimal use of the available packet data
    currentWriteCnt := Integer(len) mod (Integer(FCtoPGMPacketLen)-1);
    if currentWriteCnt = 0 then currentWriteCnt := FCtoPGMPacketLen-1;

    // prepare the packet data for PROGRAM
    if currentWriteCnt < FCtoPGMPacketLen-1 then
    begin
      if not CmdProgram(@data[bufferOffset], currentWriteCnt) then Exit;
    end
    // prepare the packet data for PROGRAM_MAX
    else
    begin
      if not CmdProgramMax(@data[bufferOffset]) then Exit;
    end;

    // update loop variables
    len := len - currentWriteCnt;
    bufferOffset := bufferOffset + currentWriteCnt;
  end;

  // successfully programmed the memory
  result := true;
end; //*** end of WriteData ***


end.
//******************************** end of XcpLoader.pas *********************************

