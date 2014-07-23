unit PCANdrvD;
//***************************************************************************************
// Project Name: TPCanDriver component for Borland Delphi
//  Description: Encapsulates PCAN's Light driver into a VCL component for PCANUSB 1CH.
//    File Name: PCANdrvD.pas
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
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs, Pcan_usb;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  TPCanChannel = ( pcanchannel0 );
  TPCanHardware = ( PCAN_USB1CH );
  TPCanDirection = ( PcanTx, PCanRx );
  TPCanMessage = packed record
    id    : LongInt;
    dlc   : Byte;
    data  : array [0..7] of Byte;
    time  : LongInt;
    ext   : Boolean;
  end;

type
  TPCanMessageEvent = procedure( Sender: TObject; Direction: TPCanDirection; Message: TPCanMessage ) of object;

type
  TPCanEventThread = class(TThread)
  private
    { Private declarations }
    FMethod: TThreadMethod;
  protected
    procedure Execute; override;
  public
    property Method     : TThreadMethod read FMethod write FMethod;
  end;

type
  TPCanDriver = class(TComponent)
  private
    { Private declarations }
    FCanEventThread: TPCanEventThread;
    FThreadRunning : boolean;
    FCanConnected : boolean;
    FStartTickCnt : DWORD;
    function IsThreadRunning: boolean;
    procedure ProcessReception;
  protected
    { Protected declarations }
    FBaudRate    : LongInt;
    FChannel     : TPCanChannel;
    FHardware    : TPCanHardware;
    FPriority    : TThreadPriority;
    FExtendedId  : Boolean;
    FOnMessage   : TPCanMessageEvent;
    procedure SetBaudRate( Value: LongInt );
    procedure SetChannel( Value: TPCanChannel );
    procedure SetHardware( Value: TPCanHardware );
    procedure SetPriority( Value: TThreadPriority );
    procedure SetExtendedId( Value: Boolean );
  public
    { Public declarations }
    constructor Create( AOwner: TComponent ); override;
    destructor  Destroy; override;
    function    Connect: boolean; virtual;
    procedure   Disconnect; virtual;
    function    Transmit( Message: TPCanMessage): boolean; virtual;
    function    IsConnected: boolean; virtual;
  published
    { Published declarations }
    property BaudRate    : LongInt           read FBaudRate     write SetBaudRate   default 500000;
    property Channel     : TPCanChannel      read FChannel  write SetChannel    default pcanchannel0;
    property Hardware    : TPCanHardware     read FHardware     write SetHardware   default PCAN_USB1CH;
    property Priority    : TThreadPriority   read FPriority     write SetPriority   default tpNormal;
    property ExtendedId  : Boolean           read FExtendedId   write SetExtendedId default False;
    property OnMessage   : TPCanMessageEvent read FOnMessage    write FOnMessage;
  end;


//***************************************************************************************
// Prototypes
//***************************************************************************************
procedure Register;

implementation
//***************************************************************************************
// NAME:           Execute
// PRECONDITIONS:  none
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Overriden Execute function for the CanEventThread. Calls and synchro-
//                 nizes with the TCanDriver.ProcessEvents function.
//
//***************************************************************************************
procedure TPCanEventThread.Execute;
begin
  while not Terminated do
  begin
    if Assigned(Method) then // make sure TPCanDriver.ProcessEvents is set
      Synchronize(Method); // call and synchronize
  end;
end; //*** end of Execute ***


//***************************************************************************************
// NAME:           Create
// PRECONDITIONS:  none
// PARAMETER:      AOwner : owner of the component
// RETURN VALUE:   none
// DESCRIPTION:    Component constructor. Calls TComponent's constructor and initializes
//                 the private property variables to their default values.
//
//***************************************************************************************
constructor TPCanDriver.Create( AOwner: TComponent );
begin
  // call inherited constructor
  inherited Create( AOwner );

  // set defaults for internal variables
  FThreadRunning := False;
  FCanConnected := False;

  // set defaults for properties
  FBaudRate := 500000;
  FChannel := pcanchannel0;
  FHardware := PCAN_USB1CH;
  FPriority := tpNormal;
  FExtendedId := False;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Destroy
// PRECONDITIONS:  none
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Component destructor. Calls TComponent's destructor
//
//***************************************************************************************
destructor TPCanDriver.Destroy;
begin
  Disconnect; // close the port and driver
  inherited Destroy; // call inherited destructor
end; //*** end of Destroy ***


//***************************************************************************************
// NAME:           IsConnected
// PRECONDITIONS:  none
// PARAMETER:      none
// RETURN VALUE:   True or False
// DESCRIPTION:    Determines whether or not the CAN driver is connected and active
//
//***************************************************************************************
function TPCanDriver.IsConnected: boolean;
begin
  Result := FCanConnected;
end; //*** end of IsConnected ***


//***************************************************************************************
// NAME:           IsThreadRunning
// PRECONDITIONS:  none
// PARAMETER:      none
// RETURN VALUE:   True or False
// DESCRIPTION:    Determines whether or not the CAN event thread is running
//
//***************************************************************************************
function TPCanDriver.IsThreadRunning: boolean;
begin
  if FThreadRunning = True then
    Result := True
  else
    Result := False;
end; //*** end of IsThreadRunning ***


//***************************************************************************************
// NAME:           SetBaudRate
// PRECONDITIONS:  none
// PARAMETER:      Value : new baudrate value [0 - 1000000 bps]
// RETURN VALUE:   none
// DESCRIPTION:    Configures the baudrate
//
//  |------------------------------------------------------------------------------------
//  | Update baudrate configuration
//  |------------------------------------------------------------------------------------
//***************************************************************************************
procedure TPCanDriver.SetBaudRate( Value: LongInt );
begin
  FBaudRate := Value; // update property
end; //*** end of SetBaudRate ***


//***************************************************************************************
// NAME:           SetChannel
// PRECONDITIONS:  none
// PARAMETER:      Value : channel0 or channel1
// RETURN VALUE:   none
// DESCRIPTION:    Configures the used CAN channel
//
//  |------------------------------------------------------------------------------------
//  | Update channel configuration
//  |------------------------------------------------------------------------------------
//***************************************************************************************
procedure TPCanDriver.SetChannel( Value: TPCanChannel );
begin
  FChannel := Value;
end; //*** end of SetChannel ***


//***************************************************************************************
// NAME:           SetHardware
// PRECONDITIONS:  none
// PARAMETER:      Value : type of CAN hardware (Virtual, CANcardXL, etc.)
// RETURN VALUE:   none
// DESCRIPTION:    Configures the used CAN hardware
//
//  |------------------------------------------------------------------------------------
//  | Update hardware configuration
//  |------------------------------------------------------------------------------------
//***************************************************************************************
procedure TPCanDriver.SetHardware( Value: TPCanHardware );
begin
  FHardware := Value;
end; //*** end of SetHardware ***


//***************************************************************************************
// NAME:           SetPriority
// PRECONDITIONS:  none
// PARAMETER:      Value : thread priority
// RETURN VALUE:   none
// DESCRIPTION:    Configures the priority for the CAN event thread
//
//  |------------------------------------------------------------------------------------
//  |  y\                      Is Thread running?                                    /n
//  |------------------------------------------------------------------------------------
//  | Stop Thread                                      |
//  | Update Thread priority                           | Update Thread priority
//  | Restart Thread                                   |
//  |------------------------------------------------------------------------------------
//***************************************************************************************
procedure TPCanDriver.SetPriority( Value: TThreadPriority );
begin
  if IsThreadRunning then
  begin
   FCanEventThread.Suspend; // suspend the thread
   FPriority := Value; // update the priority
   FCanEventThread.Resume; // resume the thread
  end
  else
  begin
   FPriority := Value; // update the priority
  end;
end; //*** end of SetPriority ***


//***************************************************************************************
// NAME:           SetExtendedId
// PRECONDITIONS:  none
// PARAMETER:      Value : true = support only 29-bit id's, false = support only 11-bit
// RETURN VALUE:   none
// DESCRIPTION:    Configures the support of extended 29-bit identifiers
//
//  |------------------------------------------------------------------------------------
//  | Update extended id support selection
//  |------------------------------------------------------------------------------------
//***************************************************************************************
procedure TPCanDriver.SetExtendedId( Value: Boolean );
begin
  FExtendedId := Value;
end; //*** end of SetExtendedId ***/


//***************************************************************************************
// NAME:           Connect
// PRECONDITIONS:  Disconnected from CAN bus
// PARAMETER:      none
// RETURN VALUE:   True or False for succees or error, respectively
// DESCRIPTION:    Initializes the CAN driver and synchronizes the hardware with the CAN
//                 bus.
//
//***************************************************************************************
function TPCanDriver.Connect: boolean;
var
  Baudcode : Word;
  MsgType  : Integer;

begin
  Result := False;
  FThreadRunning := False;
  FCanConnected := False;

  // convert baudrate in bps to supported baudrate code
  Baudcode := CAN_BAUD_500K; // init local
  case FBaudRate of
    5000    : Baudcode := CAN_BAUD_5K;
    10000   : Baudcode := CAN_BAUD_10K;
    20000   : Baudcode := CAN_BAUD_20K;
    33333   : Baudcode := $1D14;
    50000   : Baudcode := CAN_BAUD_50K;
    83333   : Baudcode := $4B14;
    100000  : Baudcode := CAN_BAUD_100K;
    125000  : Baudcode := CAN_BAUD_125K;
    250000  : Baudcode := CAN_BAUD_250K;
    500000  : Baudcode := CAN_BAUD_500K;
    1000000 : Baudcode := CAN_BAUD_1M;
  end;

  // convert extented id info
  if FExtendedId then
    MsgType := 1
  else
    MsgType := 0;

  //-------------------------- open the driver ------------------------------------------
  if CAN_Init(Baudcode, MsgType) <> CAN_ERR_OK then Exit;



  //-------------------------- open the acceptance filter --------------------------------
  if CAN_ResetFilter <> CAN_ERR_OK then
  begin
    CAN_Close;
    Exit;
  end;

  if FExtendedId then
  begin
    if CAN_MsgFilter($000, $1FFFFFFF, MSGTYPE_EXTENDED) <> CAN_ERR_OK then
    begin
      CAN_Close;
      Exit;
    end;
  end
  else
  begin
    if CAN_MsgFilter($000, $7FF, MSGTYPE_STANDARD) <> CAN_ERR_OK then
    begin
      CAN_Close;
      Exit;
    end;
  end;

  //-------------------------- reset message queues -------------------------------------
  if CAN_ResetClient <> CAN_ERR_OK then
  begin
    CAN_Close;
    Exit;
  end;

  //-------------------------- start CAN event thread -----------------------------------
  FCanEventThread := TPCanEventThread.Create(True); // create and suspend
  FCanEventThread.FreeOnTerminate := True; // auto free on termination
  FCanEventThread.Method := ProcessReception; // set method
  FCanEventThread.Resume; // start
  FThreadRunning := True;


  //-------------------------- store start time for time stamps -------------------------
  FStartTickCnt := GetTickCount;

  //-------------------------- success --------------------------------------------------
  FCanConnected := True;
  Result := True; // successfully initialized the driver
end; //*** end of Connect ***


//***************************************************************************************
// NAME:           Disconnect
// PRECONDITIONS:  none
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Disconnects the CAN driver
//
//***************************************************************************************
procedure TPCanDriver.Disconnect;
begin
  if IsConnected = True then // are we connected?
  begin
    FCanConnected := False;
    // close the channel
    CAN_Close;
  end;

  if IsThreadRunning then
  begin
    FCanEventThread.Terminate; // stop
    FThreadRunning := False;
  end;
end; //*** end of Disconnect ***


//***************************************************************************************
// NAME:           Transmit
// PRECONDITIONS:  Driver initialized using 'Connect'
// PARAMETER:      Message: CAN message that is to be transmitted
// RETURN VALUE:   True or False for succees or error, respectively
// DESCRIPTION:    Transmits a CAN message.
//
//***************************************************************************************
function TPCanDriver.Transmit( Message: TPCanMessage): boolean;
var
  cnt    : Byte;
  msg    : TPCANMsg;
  msgcpy : TPCanMessage;
begin
  // make sure the CAN driver is connected
  if not IsConnected then
  begin
    Result := False; // can't transmit it not connected
    exit; // no need to continue
  end;

  // set the message identifier
  msg.ID := Message.id;
  if Message.ext then
    msg.MSGTYPE := MSGTYPE_EXTENDED
  else
    msg.MSGTYPE := MSGTYPE_STANDARD;

  // set the data length
  msg.LEN := Message.dlc;

  // store the data bytes
  for cnt :=0 to Message.dlc do
  begin
    msg.DATA[cnt] := Message.data[cnt];
  end;

  // submit the transmit request
  if CAN_Write(msg) <> CAN_ERR_OK then
  begin
    Result := False;
    exit;
  end;

  //---------------- process transmission confirmation --------------------------
  if Assigned( FOnMessage ) then
  begin
    msgcpy := Message;
    msgcpy.time := GetTickCount - FStartTickCnt;
    FOnMessage( Self, PCanTx, msgcpy );  // call application's event handler
  end;

  Result := True;
end; //*** end of Transmit ***


//***************************************************************************************
// NAME:           ProcessReception
// PRECONDITIONS:  thread running
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Called by the CAN event tread. This function traps and processes CAN
//                 events for OnMessage.
//
//***************************************************************************************
procedure TPCanDriver.ProcessReception;
var
  cnt    : Byte;
  msg    : TPCanMessage;
  msgraw : TPCANMsg;
begin
  //---------------- process reception indication -------------------------------
  // continue only if a new message is present in the queue
  if CAN_Read(msgraw) <> CAN_ERR_OK then
    Exit;

  // only process CAN messages and not the status messages
  if (msgraw.MSGTYPE = MSGTYPE_EXTENDED) or (msgraw.MSGTYPE = MSGTYPE_STANDARD) then
  begin
    // copy the message info
    msg.time := GetTickCount - FStartTickCnt;
    msg.id := msgraw.ID;
    msg.dlc := msgraw.LEN;
    // store the data bytes
    for cnt :=0 to msg.dlc do
    begin
      msg.data[cnt] := msgraw.DATA[cnt];
    end;

    if Assigned( FOnMessage ) then
    begin
      FOnMessage( Self, PCanRx, msg );  // call application's event handler
    end;
  end;
end; //*** end of ProcessReception ***


//***************************************************************************************
// NAME:           Register
// PRECONDITIONS:  none
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Registers the TPCanDriver component into Borland Delphi's IDE.
//
//***************************************************************************************
procedure Register;
begin
  RegisterComponents('Feaser', [TPCanDriver]);
end; //*** end of Register ***


end.
//********************************** end of PCANdrvD.pas ********************************


