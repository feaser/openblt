unit CANdrvD;
//***************************************************************************************
// Project Name: TCanDriver component for Borland Delphi
//  Description: Encapsulates Vector's CANlib v4.3 into a VCL component
//    File Name: CANdrvD.pas
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
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs, CANlibD;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  TChannel = ( channel0, channel1 );
  THardware = ( Virtual, CANcardX, CANcardXL, CANcaseXL, CANboardXL,
                CANboardXL_Compact, CANac2, CANac2Pci, CANpari, CANdongle,
                CANcard, CANcardY, CANcard2, EDICcard );
  TDirection = ( Tx, Rx );
  TCanMsg = packed record
    id    : LongInt;
    dlc   : Byte;
    data  : array [0..MAX_MSG_LEN-1] of Byte;
    time  : LongInt;
    ext   : Boolean;
  end;

type
  TMessageEvent = procedure( Sender: TObject; Direction: TDirection; Message: TCanMsg ) of object;
  TErrorFrameEvent = procedure( Sender: TObject; time: LongInt ) of object;
  TBusOffEvent = procedure( Sender: TObject; time: LongInt ) of object;

type
  TCanEventThread = class(TThread)
  private
    { Private declarations }
    FMethod: TThreadMethod;
  protected
    FEventHndl: LongInt;
    procedure Execute; override;
  public
    property Method     : TThreadMethod read FMethod write FMethod;
    property EventHandle: LongInt       read FEventHndl write FEventHndl;
  end;

type
  TCanDriver = class(TComponent)
  private
    { Private declarations }
    FPortHandle    : VPortHandle;
    FChannelMask   : Vaccess;
    FPermissionMask: Vaccess;
    FCanEventThread: TCanEventThread;
    FThreadRunning : boolean;
    FEventHandle   : LongInt;
    FBusOffPending : Boolean;
    function IsThreadRunning: boolean;
    procedure ProcessEvents;
    procedure CopyMessage(event: Vevent; var msg: TCanMsg);
  protected
    { Protected declarations }
    FBaudRate    : LongInt;
    FChannel     : TChannel;
    FHardware    : THardware;
    FFilterMask  : LongInt;
    FFilterCode  : LongInt;
    FPriority    : TThreadPriority;
    FExtendedId  : Boolean;
    FOnMessage   : TMessageEvent;
    FOnErrorFrame: TErrorFrameEvent;
    FOnBusOff    : TBusOffEvent;
    procedure SetBaudRate( Value: LongInt );
    procedure SetChannel( Value: TChannel );
    procedure SetHardware( Value: THardware );
    procedure SetFilterMask( Value: LongInt );
    procedure SetFilterCode( Value: LongInt );
    procedure SetPriority( Value: TThreadPriority );
    procedure SetExtendedId( Value: Boolean );
  public
    { Public declarations }
    constructor Create( AOwner: TComponent ); override;
    destructor  Destroy; override;
    function    Connect: boolean; virtual;
    procedure   Disconnect; virtual;
    function    Transmit( Message: TCanMsg): boolean; virtual;
    function    IsConnected: boolean; virtual;
    function    IsComError: boolean; virtual;
  published
    { Published declarations }
    property BaudRate    : LongInt          read FBaudRate     write SetBaudRate   default 500000;
    property Channel     : TChannel         read FChannel      write SetChannel    default channel0;
    property Hardware    : THardware        read FHardware     write SetHardware   default Virtual;
    property FilterMask  : LongInt          read FFilterMask   write SetFilterMask default 0;
    property FilterCode  : LongInt          read FFilterCode   write SetFilterCode default 0;
    property Priority    : TThreadPriority  read FPriority     write SetPriority   default tpNormal;
    property ExtendedId  : Boolean          read FExtendedId   write SetExtendedId default False;
    property OnMessage   : TMessageEvent    read FOnMessage    write FOnMessage;
    property OnErrorFrame: TErrorFrameEvent read FOnErrorFrame write FOnErrorFrame;
    property OnBusOff    : TBusOffEvent     read FOnBusOff     write FOnBusOff;
  end;


//***************************************************************************************
// Prototypes
//***************************************************************************************
procedure Register;

implementation
//***************************************************************************************
// Constants
//***************************************************************************************
const
  Channels: array[channel0..channel1] of integer = ( 0, 1 );
  HardwareTypes: array[Virtual..EDICcard] of integer = (
    HWTYPE_VIRTUAL, HWTYPE_CANCARDX, HWTYPE_CANCARDXL, HWTYPE_CANCASEXL, HWTYPE_CANBOARDXL,
    HWTYPE_CANBOARDXL_COMPACT, HWTYPE_CANAC2, HWTYPE_CANAC2PCI, HWTYPE_CANPARI,
    HWTYPE_CANDONGLE, HWTYPE_CANCARD, HWTYPE_CANCARDY, HWTYPE_CANCARD2, HWTYPE_EDICCARD);


//***************************************************************************************
// NAME:           Execute
// PRECONDITIONS:  none
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Overriden Execute function for the CanEventThread. Calls and synchro-
//                 nizes with the TCanDriver.ProcessEvents function.
//
//***************************************************************************************
procedure TCanEventThread.Execute;
begin
  while not Terminated do
  begin
    if FEventHndl <> 0 then // make sure event is configured
    begin
      // wait for receive event
      WaitForSingleObject(FEventHndl, 1000);

      if Assigned(Method) then // make sure TCanDriver.ProcessEvents is set
        Synchronize(Method); // call and synchronize
    end;
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
constructor TCanDriver.Create( AOwner: TComponent );
begin
  // call inherited constructor
  inherited Create( AOwner );

  // set defaults for internal variables
  FPortHandle    := INVALID_PORTHANDLE;
  FChannelMask   := 0;
  FPermissionMask:= 0;
  FThreadRunning := False;
  FEventHandle   := 0;
  FBusOffPending := False;

  // set defaults for properties
  FBaudRate := 500000;
  FChannel := channel0;
  FHardware := Virtual;
  FFilterMask := 0;
  FFilterCode := 0;
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
destructor TCanDriver.Destroy;
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
function TCanDriver.IsConnected: boolean;
begin
  if FPortHandle <> INVALID_PORTHANDLE then
    Result := True
  else
    Result := False;
end; //*** end of IsConnected ***


//***************************************************************************************
// NAME:           IsComError
// PRECONDITIONS:  none
// PARAMETER:      none
// RETURN VALUE:   True if the communication interface is in error state, False otherwise
// DESCRIPTION:    Determines whether or not the CAN controller is in error state.
//
//***************************************************************************************
function TCanDriver.IsComError: boolean;
begin
  result := FBusOffPending;
end; //*** end of IsComError ***


//***************************************************************************************
// NAME:           IsThreadRunning
// PRECONDITIONS:  none
// PARAMETER:      none
// RETURN VALUE:   True or False
// DESCRIPTION:    Determines whether or not the CAN event thread is running
//
//***************************************************************************************
function TCanDriver.IsThreadRunning: boolean;
begin
  if FThreadRunning = True then
    Result := True
  else
    Result := False;
end; //*** end of IsThreadRunning ***


//***************************************************************************************
// NAME:           CopyMessage
// PRECONDITIONS:  none
// PARAMETER:      event: msg tx or rx event information (source)
//                 msg: buffer to copy message to (destination)
// RETURN VALUE:   none
// DESCRIPTION:    Copies a CAN message from an event type to a TCanMsg type.
//
//***************************************************************************************
procedure TCanDriver.CopyMessage(event: Vevent; var msg: TCanMsg);
var
  cnt: integer;
begin
  if (event.msg.id and EXT_MSG) = EXT_MSG then // 29-bit id?
  begin
    msg.id := (event.msg.id and not EXT_MSG); // reset ext bit
    msg.ext := True; // this is an 29-bit id
  end
  else
  begin
    msg.id := event.msg.id; // store id
    msg.ext := False; // this is an 11-bit id
  end;
  msg.dlc := event.msg.dlc;
  msg.time := event.timeStamp;

  // copy the data bytes
  for cnt :=0 to MAX_MSG_LEN-1 do
  begin
    if cnt < event.msg.dlc then
      msg.data[cnt] := event.msg.data[cnt]
    else
      msg.data[cnt] := 0;
  end;
end; //*** end of CopyMessage ***


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
procedure TCanDriver.SetBaudRate( Value: LongInt );
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
procedure TCanDriver.SetChannel( Value: TChannel );
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
procedure TCanDriver.SetHardware( Value: THardware );
begin
  FHardware := Value;
end; //*** end of SetHardware ***


//***************************************************************************************
// NAME:           SetFilterMask
// PRECONDITIONS:  none
// PARAMETER:      Value : acceptance filter mask
// RETURN VALUE:   none
// DESCRIPTION:    Configures the acceptance filter mask for the CAN channel
//
//  |------------------------------------------------------------------------------------
//  | Update filter mask value
//  |------------------------------------------------------------------------------------
//***************************************************************************************
procedure TCanDriver.SetFilterMask( Value: LongInt );
begin
  FFilterMask := Value;
end; //*** end of SetFilterMask ***


//***************************************************************************************
// NAME:           SetFilterCode
// PRECONDITIONS:  none
// PARAMETER:      Value : acceptance filter code
// RETURN VALUE:   none
// DESCRIPTION:    Configures the acceptance filter code for the CAN channel
//
//  |------------------------------------------------------------------------------------
//  | Update filter code value
//  |------------------------------------------------------------------------------------
//***************************************************************************************
procedure TCanDriver.SetFilterCode( Value: LongInt );
begin
  FFilterCode := Value;
end; //*** end of SetFilterCode ***


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
procedure TCanDriver.SetPriority( Value: TThreadPriority );
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
procedure TCanDriver.SetExtendedId( Value: Boolean );
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
//  |------------------------------------------------------------------------------------
//  |  y\                    Connected?                                              /n
//  |------------------------------------------------------------------------------------
//  | Open the driver (ncdOpenDriver)
//  | Obtain mask to channel (ncdGetChannelMask)
//  | Open the port using this mask (ncdOpenPort)
//  |------------------------------------------------------------------------------------
//  |  y\                    Permission to change settings?                          /n
//  |------------------------------------------------------------------------------------
//  | Configure baudrate (ncdSetChannelBitrate)                   |
//  |------------------------------------------------------------------------------------
//  | Configure acceptance filter (ncdSetChannelAcceptance)
//  | Enable error frames and chipstate events (ncdSetReceiveMode)
//  | Create synchronizatio object (ncdSetNotification)
//  | Reset internal clock (ncdResetClock)
//  | Sync to the CAN bus (ncdActivateChannel)
//  | Empty transmit and receive queue's (ncdFlushXxxQueue)
//  |------------------------------------------------------------------------------------
//  |  y\                    Errors occurred during init?                            /n
//  |------------------------------------------------------------------------------------
//  |  y\          Port opened?                               /n  |
//  |-------------------------------------------------------------| Start CAN event thread
//  | Close port (ncdClosePort)                        |          | Return TRUE
//  |-------------------------------------------------------------|
//  | Return FALSE                                                |
//  |------------------------------------------------------------------------------------
//***************************************************************************************
function TCanDriver.Connect: boolean;
var
  vErr : Vstatus;
  acc  : VsetAcceptance;
label
  error;
begin
  // reset internal variables
  FPortHandle    := INVALID_PORTHANDLE;
  FChannelMask   := 0;
  FPermissionMask:= 0;
  FThreadRunning := False;
  FEventHandle   := 0;
  FBusOffPending := False;

  //-------------------------- open the driver ------------------------------------------
  vErr := ncdOpenDriver;
  if vErr <> VSUCCESS then goto error;

  //-------------------------- select a channel -----------------------------------------
  FChannelMask := ncdGetChannelMask(HardwareTypes[FHardware], 0, Channels[FChannel]);
  if FChannelMask=0 then goto error;

  //-------------------------- open a port ----------------------------------------------
  FPermissionMask := FChannelMask;
  vErr := ncdOpenPort(FPortHandle, 'TCanDriver0', FChannelMask, FPermissionMask,
                      FPermissionMask, 1024);
  if vErr <> VSUCCESS then goto error;

  //-------------------------- set baudrate ---------------------------------------------
  if FPermissionMask<>0 then
  begin
    vErr := ncdSetChannelBitrate(FPortHandle, FPermissionMask, FBaudRate);
    if vErr <> VSUCCESS then goto error;
  end;

  //-------------------------- set the acceptance filter --------------------------------
  acc.mask := FFilterMask;
  acc.code := FFilterCode;
  if FExtendedId = True then // 29-bit id used?
  begin
    acc.mask := acc.mask or LongInt(EXT_MSG);
    acc.code := acc.code or LongInt(EXT_MSG);
  end;
  vErr := ncdSetChannelAcceptance(FPortHandle, FChannelMask, acc);
  if vErr <> VSUCCESS then goto error;

  //-------------------------- enable error frames and chipstate events -----------------
  vErr := ncdSetReceiveMode(FPortHandle, 0, 0);
  if vErr <> VSUCCESS then goto error;

  //-------------------------- create synchronisation object ----------------------------
  FEventHandle := CreateEvent(nil, FALSE, FALSE, nil);
  if FEventHandle = 0 then goto error;
  vErr := ncdSetNotification(FPortHandle, FEventHandle, 1);
  if vErr<>VSUCCESS then goto error;

  //-------------------------- reset the clock ------------------------------------------
  vErr := ncdResetClock(FPortHandle);
  if vErr <> VSUCCESS then goto error;

  //-------------------------- sync with bus --------------------------------------------
  vErr := ncdActivateChannel(FPortHandle, FChannelMask);
  if vErr <> VSUCCESS then goto error;

  //-------------------------- flush queue's --------------------------------------------
  vErr := ncdFlushReceiveQueue(FPortHandle);
  if vErr <> VSUCCESS then goto error;
  vErr := ncdFlushTransmitQueue(FPortHandle, FChannelMask);
  if vErr <> VSUCCESS then goto error;

  //-------------------------- start CAN event thread -----------------------------------
  FCanEventThread := TCanEventThread.Create(True); // create and suspend
  FCanEventThread.FreeOnTerminate := True; // auto free on termination
  FCanEventThread.Method := ProcessEvents; // set method
  FCanEventThread.FEventHndl := FEventHandle; // set event handle
  FCanEventThread.Resume; // start
  FThreadRunning := True;

  //-------------------------- success --------------------------------------------------
  Result := True; // successfully initialized the driver
  exit; // stop here

  //-------------------------- error occurred -------------------------------------------
  error:
  if FEventHandle <> 0 then
    CloseHandle(FEventHandle);
  if FPortHandle <> INVALID_PORTHANDLE then
  begin
    ncdClosePort(FPortHandle);
    FPortHandle := INVALID_PORTHANDLE;
  end;
  Result := False;
end; //*** end of Connect ***


//***************************************************************************************
// NAME:           Disconnect
// PRECONDITIONS:  none
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Disconnects the CAN driver
//
//  |------------------------------------------------------------------------------------
//  |  y\                    Connected?                                              /n
//  |------------------------------------------------------------------------------------
//  | Deactivate the channel (ncdDeactivateChannel)               |
//  | Close port (ncdClosePort)                                   |
//  |------------------------------------------------------------------------------------
//  | Close the driver (ncdCloseDriver)
//  |------------------------------------------------------------------------------------
//  |  y\                    CAN event thread active?                                /n
//  |------------------------------------------------------------------------------------
//  | Stop CAN event thread                                       |
//  |------------------------------------------------------------------------------------
//***************************************************************************************
procedure TCanDriver.Disconnect;
begin
  if IsConnected = True then begin // are we connected?
    ncdDeactivateChannel(FPortHandle, FChannelMask); // deactivate channel
    if FEventHandle <> 0 then
      CloseHandle(FEventHandle);
    ncdClosePort(FPortHandle); // close the port
    FPortHandle := INVALID_PORTHANDLE; // invalidate handle
  end;
  ncdCloseDriver; // close the driver
  if IsThreadRunning then
  begin
    FCanEventThread.FEventHndl := 0; // reset event handle
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
//  |------------------------------------------------------------------------------------
//  |  y\                    Connected?                                              /n
//  |------------------------------------------------------------------------------------
//  | Transmit message using ncdTransmit           |
//  |----------------------------------------------| Return FALSE
//  | Return TRUE                                  |
//  |------------------------------------------------------------------------------------
//***************************************************************************************
function TCanDriver.Transmit( Message: TCanMsg): boolean;
var
  vErr : Vstatus;
  event : Vevent;
  cnt : integer;
begin
  // make sure the CAN driver is connected
  if not IsConnected then
  begin
    Result := False; // can't transmit it not connected
    exit; // no need to continue
  end;

  // configure message as tx with acknowledge
  event.tag := V_TRANSMIT_MSG;
  event.msg.flags := MSGFLAG_TX;

  // set the message identifier
  if Message.ext = True then
    event.msg.id := Message.id or LongInt(EXT_MSG)
  else
    event.msg.id := Message.id;

  // set the data length
  event.msg.dlc := Message.dlc;

  // store the data bytes
  for cnt :=0 to MAX_MSG_LEN-1 do
  begin
    event.msg.data[cnt] := Message.data[cnt];
  end;

  vErr := ncdTransmit(FPortHandle, FChannelMask, event);

  if vErr <> VSUCCESS then
    Result := False
  else
    Result := True;
end; //*** end of Transmit ***


//***************************************************************************************
// NAME:           ProcessEvents
// PRECONDITIONS:  thread running
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Called by the CAN event tread. This function traps and processes CAN
//                 events for OnMessage, OnBusOff, and OnErrorFrame.
//
//***************************************************************************************
procedure TCanDriver.ProcessEvents;
var
  vErr   : Vstatus;
  pEvent : PVEvent;
  msg    : TCanMsg;
begin
  while True do
  begin
    vErr := ncdReceive1(FPortHandle, pEvent);

    if (vErr<>VSUCCESS) and (vErr<>VERR_QUEUE_IS_EMPTY) then break;
    if vErr=VERR_QUEUE_IS_EMPTY then break;

    case pEvent^.tag of
      V_RECEIVE_MSG, V_TRANSMIT_MSG:
      begin
        if (pEvent^.msg.flags and MSGFLAG_ERROR_FRAME) = MSGFLAG_ERROR_FRAME then
        begin
          //---------------- process errorframe -----------------------------------------
          if Assigned( FOnErrorFrame ) then
          begin
            FOnErrorFrame( Self, pEvent^.timeStamp ); // call application's event handler
          end;
        end
        else if pEvent^.msg.flags = 0 then // msg rx indication
        begin
          //---------------- process reception indication -------------------------------
          CopyMessage(pEvent^, msg);
          if Assigned( FOnMessage ) then
          begin
            FOnMessage( Self, Rx, msg );  // call application's event handler
          end;
        end
        else if (pEvent^.msg.flags and MSGFLAG_TX) = MSGFLAG_TX then // msg tx confirmation
        begin
          //---------------- process transmission confirmation --------------------------
          CopyMessage(pEvent^, msg);
          if Assigned( FOnMessage ) then
          begin
            FOnMessage( Self, Tx, msg );  // call application's event handler
          end;
        end;
      end;
      V_CHIP_STATE:
      begin
        if (pEvent^.chipState.busStatus and CHIPSTAT_BUSOFF) = CHIPSTAT_BUSOFF then
        begin
          //---------------- process bus off event --------------------------------------
          FBusOffPending := True;
          if Assigned( FOnBusOff ) then
          begin
            FOnBusOff( Self, pEvent^.timeStamp );  // call application's event handler
          end;
        end;
      end;
    end;
  end;
end; //*** end of ProcessEvents ***


//***************************************************************************************
// NAME:           Register
// PRECONDITIONS:  none
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Registers the TCanDriver component into Borland Delphi's IDE.
//
//***************************************************************************************
procedure Register;
begin
  RegisterComponents('Feaser', [TCanDriver]);
end; //*** end of Register ***


end.
//********************************** end of CANdrvD.pas *********************************


