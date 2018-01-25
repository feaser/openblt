unit CanUsb;
//***************************************************************************************
//  Description: Lawicel CANUSB API interface wrapper.
//    File Name: CanUsb.pas
//
//---------------------------------------------------------------------------------------
//                          C O P Y R I G H T
//---------------------------------------------------------------------------------------
//   Copyright (c) 2016 by Feaser    http://www.feaser.com    All rights reserved
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
  Windows, Messages, SysUtils, Classes;


//***************************************************************************************
// Global constant declarations
//***************************************************************************************
const
  // filter mask settings
  CANUSB_ACCEPTANCE_CODE_ALL = $00000000;
  CANUSB_ACCEPTANCE_MASK_ALL = $FFFFFFFF;

  // message flags
  CANMSG_EXTENDED = $80;
  CANMSG_RTR = $40;

  // status bits
  CANSTATUS_RECEIVE_FIFO_FULL = $01;
  CANSTATUS_TRANSMIT_FIFO_FULL = $02;
  CANSTATUS_ERROR_WARNING = $04;
  CANSTATUS_DATA_OVERRUN = $08;
  CANSTATUS_ERROR_PASSIVE = $20;
  CANSTATUS_ARBITRATION_LOST = $40;
  CANSTATUS_BUS_ERROR = $80;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  // CAN handle to the actual hardware adapter
  CANHANDLE = Longint;

  // CAN baudrate identifiers
  CANBaudrate = (  CAN_BAUD_1M   = 0,  //   1   MBit/sec
                   CAN_BAUD_800K = 1,  // 800   kBit/sec
                   CAN_BAUD_500K = 2,  // 500   kBit/sec
                   CAN_BAUD_250K = 3,  // 250   kBit/sec
                   CAN_BAUD_125K = 4,  // 125   kBit/sec
                   CAN_BAUD_100K = 5,  // 100   kBit/sec
                   CAN_BAUD_50K  = 6,  //  50   kBit/sec
                   CAN_BAUD_20K  = 7,  //  20   kBit/sec
                   CAN_BAUD_10K  = 8   //  10   kBit/sec
                 );

  // CAN Frame
  CANMsg = record
    id : Longword;                // message id
    timestamp : Longword;         // timestamp in
    flags : Byte;                 // [extended_id|1][RTR:1][reserver:6]
    len : Byte;                   // frame size (0.8)
    data : array[0..7] of Byte;   // databytes 0..7
  end;

  // DLL interface methods
  TDllCanUsbOpen   = function(szID: PAnsiChar; szBitrate: PAnsiChar; acceptance_code: Longword; acceptance_mask: Longword; flags: Longword): CANHANDLE; stdcall;
  TDllCanUsbClose  = function(h: CANHANDLE): Integer; stdcall;
  TDllCanUsbRead   = function(h: CANHANDLE; var msg: CANMsg): Integer; stdcall;
  TDllCanUsbWrite  = function(h: CANHANDLE; var msg: CANMsg): Integer; stdcall;
  TDllCanUsbStatus = function(h: CANHANDLE): Integer; stdcall;

  // CANUSB API interface wrapper class
  TCanUsb = class(TObject)
  private
    { Private declarations }
    FDllCanUsbOpen: TDllCanUsbOpen;
    FDllCanUsbClose: TDllCanUsbClose;
    FDllCanUsbRead: TDllCanUsbRead;
    FDllCanUsbWrite: TDllCanUsbWrite;
    FDllCanUsbStatus: TDllCanUsbStatus;
    FHCanUsbAdapter: CANHANDLE;
    FHCanUsbLib: THandle;
  protected
    { Protected declarations }
  public
    { Public declarations }
    constructor Create;
    destructor  Destroy; override;
    function    LoadDll: Boolean;
    procedure   UnloadDll;
    function    IsDllLoaded: Boolean;
    function    Connect(baudRate: CANBaudrate; acceptanceCode: Longword; acceptanceMask: Longword): Boolean;
    function    Disconnect: Boolean;
    function    Transmit(msg: CANMsg): Boolean;
    function    Receive(var msg: CANMsg): Boolean;
    function    Status: Integer;
    procedure   FindOptimumSingleRxFilter(id: Longword; ext: Boolean; var code: Longword; var mask: Longword);
  end;


implementation
//***************************************************************************************
// Local constant declarations
//***************************************************************************************
const
  CANBaudrateVals: array[0..8] of AnsiString =
   ( '1000', // CAN_BAUD_1M
     '800',  // CAN_BAUD_800K
     '500',  // CAN_BAUD_500K
     '250',  // CAN_BAUD_250K
     '125',  // CAN_BAUD_125K
     '100',  // CAN_BAUD_100K
     '50',   // CAN_BAUD_50K
     '20' ,  // CAN_BAUD_20K
     '10'    // CAN_BAUD_10K
   ) ;

  // error return codes
  ERROR_CANUSB_OK = 1;
  ERROR_CANUSB_GENERAL = -(1);
  ERROR_CANUSB_OPEN_SUBSYSTEM = -(2);
  ERROR_CANUSB_COMMAND_SUBSYSTEM = -(3);
  ERROR_CANUSB_NOT_OPEN = -(4);
  ERROR_CANUSB_TX_FIFO_FULL = -(5);
  ERROR_CANUSB_INVALID_PARAM = -(6);
  ERROR_CANUSB_NO_MESSAGE = -(7);
  ERROR_CANUSB_MEMORY_ERROR = -(8);
  ERROR_CANUSB_NO_DEVICE = -(9);
  ERROR_CANUSB_TIMEOUT = -(10);
  ERROR_CANUSB_INVALID_HARDWARE = -(11);


//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Object constructor. Calls TObject's constructor and initializes the
//                 private member variables to their default values.
//
//***************************************************************************************
constructor TCanUsb.Create;
begin
  // call inherited constructor
  inherited Create;

  // initialize private members
  FHCanUsbLib := 0;
  FHCanUsbAdapter := 0;
  FDllCanUsbOpen := nil;
  FDllCanUsbClose := nil;
  FDllCanUsbRead := nil;
  FDllCanUsbWrite := nil;
  FDllCanUsbStatus := nil;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Destroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Object destructor. Calls TObject's destructor
//
//***************************************************************************************
destructor TCanUsb.Destroy;
begin
  // clean up by unloading the dll
  UnloadDll;

  // call inherited destructor
  inherited Destroy;
end; //*** end of Destroy ***


//***************************************************************************************
// NAME:           LoadDll
// PARAMETER:      none
// RETURN VALUE:   True if the DLL was successfully loaded, False otherwise.
// DESCRIPTION:    Loads the CANUSB API dll.
//
//***************************************************************************************
function TCanUsb.LoadDll: Boolean;
begin
  // init result
  Result := True;

  // nothing to do if the dll is already loaded
  if IsDllLoaded then
  begin
    Exit;
  end;

  // attempt to load the CANUSB API dll
  FHCanUsbLib := LoadLibrary(PChar('CANUSBDRV.DLL'));
  // check result
  if FHCanUsbLib = 0 then
  begin
    Result := False;
    Exit;
  end;

  // still here so library loaded. attempt to obtain the function pointers
  @FDllCanUsbOpen   := GetProcAddress(FHCanUsbLib, 'canusb_Open');
  @FDllCanUsbClose  := GetProcAddress(FHCanUsbLib, 'canusb_Close');
  @FDllCanUsbRead   := GetProcAddress(FHCanUsbLib, 'canusb_Read');
  @FDllCanUsbWrite  := GetProcAddress(FHCanUsbLib, 'canusb_Write');
  @FDllCanUsbStatus := GetProcAddress(FHCanUsbLib, 'canusb_Status');

  // check if the functions were found in the interface library
  if not Assigned(FDllCanUsbOpen)   then Result := False;
  if not Assigned(FDllCanUsbClose)  then Result := False;
  if not Assigned(FDllCanUsbRead)   then Result := False;
  if not Assigned(FDllCanUsbWrite)  then Result := False;
  if not Assigned(FDllCanUsbStatus) then Result := False;

  // check if functions were all successfully loaded
  if not Result then
  begin
    FreeLibrary(FHCanUsbLib);
    FHCanUsbLib := 0;
  end;
end; //*** end of LoadDll ***


//***************************************************************************************
// NAME:           UnloadDll
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Unloads the CANUSB API dll.
//
//***************************************************************************************
procedure TCanUsb.UnloadDll;
begin
  // only continue if the dll is actually loaded at this point
  if not IsDllLoaded then
  begin
    Exit;
  end;

  // make sure that the connection with the CANUSB adapter is closed
  Disconnect;

  // unload the DLL
  FreeLibrary(FHCanUsbLib);
  FHCanUsbLib := 0;
end; //*** end of UnloadDll ***


//***************************************************************************************
// NAME:           IsDllLoaded
// PARAMETER:      none
// RETURN VALUE:   True if the DLL is loaded, False otherwise.
// DESCRIPTION:    Determines if the CANUSB API dll is currently loaded.
//
//***************************************************************************************
function TCanUsb.IsDllLoaded: Boolean;
begin
  Result := (FHCanUsbLib <> 0);
end; //*** end of IsDllLoaded ***


//***************************************************************************************
// NAME:           Connect
// PARAMETER:      baudRate Baudrate id.
//                 acceptanceCode Code part of the acceptance filter. Set to
//                 CANUSB_ACCEPTANCE_CODE_ALL to get all messages.
//                 acceptanceMask Mask part of the acceptance filter. Set to
//                 CANUSB_ACCEPTANCE_MASk_ALL to get all messages.
// RETURN VALUE:   True if successful, False otherwise.
// DESCRIPTION:    Opens the connection with the first CANUSB hardware adapter found.
//
//***************************************************************************************
function TCanUsb.Connect(baudRate: CANBaudrate; acceptanceCode: Longword; acceptanceMask: Longword): Boolean;
begin
  // initialize the result
  Result := True;

  // do not continue if the DLL is not loaded
  if not IsDllLoaded then
  begin
    Result := False;
    Exit;
  end;

  // make sure the connection is closed before opening
  Disconnect;

  // open the connection
  FHCanUsbAdapter := FDllCanUsbOpen(nil, PAnsiChar(CANBaudrateVals[Ord(baudRate)]), acceptanceCode, acceptanceMask, 0);

  // check the result
  if FHCanUsbAdapter <= 0 then
  begin
    Result := False;
    FHCanUsbAdapter := 0;
  end;
end; //*** end of Connect ***


//***************************************************************************************
// NAME:           Disconnect
// PARAMETER:      none
// RETURN VALUE:   True if successful, False otherwise.
// DESCRIPTION:    Closes the connection with the CANUSB hardware adapter
//
//***************************************************************************************
function TCanUsb.Disconnect: Boolean;
begin
  // initialize the result
  Result := True;

  // only continue if the DLL is loaded
  if IsDllLoaded then
  begin
    // check if the connection with the CANUSB adapter is open
    if FHCanUsbAdapter <> 0 then
    begin
      // close the connection and set the result
      Result := (FDllCanUsbClose(FHCanUsbAdapter) > 0);
      FHCanUsbAdapter := 0;
    end;
  end;
end; //*** end of Disconnect ***


//***************************************************************************************
// NAME:           Transmit
// PARAMETER:      msg CAN message to transmit.
// RETURN VALUE:   True if successful, False otherwise.
// DESCRIPTION:    Submits a CAN message for transmission.
//
//***************************************************************************************
function TCanUsb.Transmit(msg: CANMsg): Boolean;
begin
  // only continue if the DLL is loaded
  if not IsDllLoaded then
  begin
    Result := False;
    Exit;
  end;

  // check if the connection with the CANUSB adapter is open
  if FHCanUsbAdapter = 0 then
  begin
    Result := False;
    Exit;
  end;

  // submit message for transmission and set the result
  Result := (FDllCanUsbWrite(FHCanUsbAdapter, msg) = ERROR_CANUSB_OK);
end; //*** end of Transmit ***


//***************************************************************************************
// NAME:           Receive
// PARAMETER:      msg CAN message to store received message.
// RETURN VALUE:   True if successful, False otherwise.
// DESCRIPTION:    Receives the oldest message from the receive fifo, if one is present.
//
//***************************************************************************************
function TCanUsb.Receive(var msg: CANMsg): Boolean;
begin
  // only continue if the DLL is loaded
  if not IsDllLoaded then
  begin
    Result := False;
    Exit;
  end;

  // check if the connection with the CANUSB adapter is open
  if FHCanUsbAdapter = 0 then
  begin
    Result := False;
    Exit;
  end;

  // extract oldest message from the receive fifo, if one is present
  Result := (FDllCanUsbRead(FHCanUsbAdapter, msg) = ERROR_CANUSB_OK);
end; //*** end of Receive ***


//***************************************************************************************
// NAME:           Status
// PARAMETER:      none
// RETURN VALUE:   Status bits (CANSTATUS_xxx).
// DESCRIPTION:    Obtains status of the CANUSB adapter.
//
//***************************************************************************************
function TCanUsb.Status: Integer;
begin
  // init result
  Result := 0;

  // only continue if the DLL is loaded
  if not IsDllLoaded then
  begin
    Exit;
  end;

  // check if the connection with the CANUSB adapter is open
  if FHCanUsbAdapter = 0 then
  begin
    Exit;
  end;

  // read and return status bits
  Result := FDllCanUsbStatus(FHCanUsbAdapter);
end; //*** end of Status ***


//***************************************************************************************
// NAME:           FindOptimumSingleRxFilter
// PARAMETER:      id CAN message identifier to optimize the filter for.
//                 ext True if the id is 29-bit, False otherwise.
//                 code Buffer for storing the code part of the acceptance filter.
//                 mask Buffer for storing the mask part of the acceptance filter.
// RETURN VALUE:   none
// DESCRIPTION:    Finds the best code and mask values for receiving just a single CAN
//                 message with the reception acceptance filter. For 11-bit identifiers,
//                 this will find a perfect match, for 29-bit identfiers, it will always
//                 still let a group of messages pass because bits 0..12 are always
//                 don't care.
//
//***************************************************************************************
procedure TCanUsb.FindOptimumSingleRxFilter(id: Longword; ext: Boolean; var code: Longword; var mask: Longword);
var
  ACR0, ACR1, ACR2, ACR3: Byte;
  AMR0, AMR1, AMR2, AMR3: Byte;
begin
  // CANUSB's SJA1000 is in dual filter mode. this means it can be set to receive 1 single
  // 11-bit identifier or a small group of 29-bit identifiers.
  if not ext then
  begin
    ACR0 := Byte(id shr 3);
    AMR0 := $00;
    ACR1 := (Byte(id shl 5) or $1f);
    AMR1 := $1F;
    ACR2 := Byte(id shr 3);
    AMR2 := $00;
    ACR3 := (Byte(id shl 5) or $1f);
    AMR3 := $1F;
  end
  else
  begin
    ACR0 := Byte(id shr 21);
    AMR0 := $00;
    ACR1 := Byte(id shr 13);
    AMR1 := $00;
    ACR2 := Byte(id shr 21);
    AMR2 := $00;
    ACR3 := Byte(id shr 13);
    AMR3 := $00;
  end;

  // set the results
  code := (ACR3 shl 24) and $ff000000;
  code := code or ((ACR2 shl 16) and $00ff0000);
  code := code or ((ACR1 shl  8) and $0000ff00);
  code := code or ((ACR0 shl  0) and $000000ff);
  mask := (AMR3 shl 24) and $ff000000;
  mask := mask or ((AMR2 shl 16) and $00ff0000);
  mask := mask or ((AMR1 shl  8) and $0000ff00);
  mask := mask or ((AMR0 shl  0) and $000000ff);
end; //*** end of FindOptimumSingleRxFilter ***


end.
//******************************* end of CanUsb.pas *************************************


