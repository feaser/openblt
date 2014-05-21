//***************************************************************************************
// 20060225: Updated by Frank Voorburg - Feaser
//
//   - When using ReadXxx the windows messages are now being processed
//   - When using ReadXxx the loop will stop (FCancel) upon disconnect
//***************************************************************************************

//------------------------------------------------------------------------
// UNIT           : CPDrv.pas
// CONTENTS       : TCommPortDriver component
// VERSION        : 2.1
// TARGET         : (Inprise's) Borland Delphi 4.0
// AUTHOR         : Marco Cocco
// STATUS         : Freeware 
// INFOS          : Implementation of TCommPortDriver component:
//                  - non multithreaded serial I/O
// KNOWN BUGS     : none
// COMPATIBILITY  : Windows 95/98/NT/2000
// REPLACES       : TCommPortDriver v2.00    (Delphi 4.0)
//                  TCommPortDriver v1.08/16 (Delphi 1.0)
//                  TCommPortDriver v1.08/32 (Delphi 2.0/3.0)
// BACK/COMPAT.   : partial - a lot of properties have been renamed 
// RELEASE DATE   : 06/06/2000
//                  (Replaces v2.0 released on 30/NOV/1998)
//------------------------------------------------------------------------
// FOR UPDATES    : - sorry, no home page -
// BUGS REPORT    : mail to   : mcocco@libero.it
//                          or: ditrek@tiscalinet.it
//------------------------------------------------------------------------
//
// Copyright (c) 1996-2000 by Marco Cocco. All rights reseved.
// Copyright (c) 1996-2000 by d3k Software Company. All rights reserved.
//
//******************************************************************************
//*   Permission to use, copy,  modify, and distribute this software and its   *
//*        documentation without fee for any purpose is hereby granted,        *
//*   provided that the above copyright notice appears on all copies and that  *
//*     both that copyright notice and this permission notice appear in all    *
//*                         supporting documentation.                          *
//*                                                                            *
//* NO REPRESENTATIONS ARE MADE ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY *
//*    PURPOSE. IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.    *
//*   NEITHER MARCO COCCO OR D3K SHALL BE LIABLE FOR ANY DAMAGES SUFFERED BY   *
//*                          THE USE OF THIS SOFTWARE.                         *
//******************************************************************************

unit CPDrv;

interface

uses
  // Delphi units
  Windows, Messages, SysUtils, Forms, Classes
  // ComDrv32 units
  ;

//------------------------------------------------------------------------
// Property types
//------------------------------------------------------------------------

type
  // Baud Rates (custom or 110...256k bauds)
  TBaudRate = ( brCustom,
                br110, br300, br600, br1200, br2400, br4800,
                br9600, br14400, br19200, br38400, br56000,
                br57600, br115200, br128000, br256000 );
  // Port Numbers ( custom or COM1..COM16 )
  TPortNumber = ( pnCustom,
                  pnCOM1, pnCOM2, pnCOM3, pnCOM4, pnCOM5, pnCOM6, pnCOM7,
                  pnCOM8, pnCOM9, pnCOM10, pnCOM11, pnCOM12, pnCOM13,
                  pnCOM14, pnCOM15, pnCOM16 );
  // Data bits ( 5, 6, 7, 8 )
  TDataBits = ( db5BITS, db6BITS, db7BITS, db8BITS );
  // Stop bits ( 1, 1.5, 2 )
  TStopBits = ( sb1BITS, sb1HALFBITS, sb2BITS );
  // Parity ( None, odd, even, mark, space )
  TParity = ( ptNONE, ptODD, ptEVEN, ptMARK, ptSPACE );
  // Hardware Flow Control ( None, None + RTS always on, RTS/CTS )
  THwFlowControl = ( hfNONE, hfNONERTSON, hfRTSCTS );
  // Software Flow Control ( None, XON/XOFF )
  TSwFlowControl = ( sfNONE, sfXONXOFF );
  // What to do with incomplete (incoming) packets ( Discard, Pass )
  TPacketMode = ( pmDiscard, pmPass );

//------------------------------------------------------------------------
// Event types
//------------------------------------------------------------------------

type
  // RX event ( packet mode disabled )
  TReceiveDataEvent = procedure( Sender: TObject; DataPtr: pointer; DataSize: DWORD ) of object;
  // RX event ( packed mode enabled )
  TReceivePacketEvent = procedure( Sender: TObject; Packet: pointer; DataSize: DWORD ) of object;

//------------------------------------------------------------------------
// Other types
//------------------------------------------------------------------------

type
  // Line status ( Clear To Send, Data Set Ready, Ring, Carrier Detect )
  TLineStatus = ( lsCTS, lsDSR, lsRING, lsCD );
  // Set of line status
  TLineStatusSet = set of TLineStatus;

//------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------

const
  RELEASE_NOCLOSE_PORT = HFILE(INVALID_HANDLE_VALUE-1);

//------------------------------------------------------------------------
// TCommPortDriver component
//------------------------------------------------------------------------

type
  TCommPortDriver = class( TComponent )
  protected
    // Device Handle ( File Handle )
    FHandle                    : HFILE;
    // # of the COM port to use, or pnCustom to use custom port name
    FPort                      : TPortNumber;
    // Custom port name ( usually '\\.\COMn', with n = 1..x )
    FPortName                  : string;
    // COM Port speed (brXXX)
    FBaudRate                  : TBaudRate;
    // Baud rate ( actual numeric value )
    FBaudRateValue             : DWORD;
    // Data bits size (dbXXX)
    FDataBits                  : TDataBits;
    // How many stop bits to use (sbXXX)
    FStopBits                  : TStopBits;
    // Type of parity to use (ptXXX)
    FParity                    : TParity;
    // Type of hw handshaking (hw flow control) to use (hfXXX)
    FHwFlow                    : THwFlowControl;
    // Type of sw handshaking (sw flow control) to use (sFXXX)
    FSwFlow                    : TSwFlowControl;
    // Size of the input buffer
    FInBufSize                 : DWORD;
    // Size of the output buffer
    FOutBufSize                : DWORD;
    // Size of a data packet
    FPacketSize                : smallint;
    // ms to wait for a complete packet (<=0 = disabled)
    FPacketTimeout             : integer;
    // What to do with incomplete packets (pmXXX)
    FPacketMode                : TPacketMode;
    // Event to raise on data reception (asynchronous)
    FOnReceiveData             : TReceiveDataEvent;
    // Event to raise on packet reception (asynchronous)
    FOnReceivePacket           : TReceivePacketEvent;
    // ms of delay between COM port pollings
    FPollingDelay              : word;
    // Specifies if the DTR line must be enabled/disabled on connect
    FEnableDTROnOpen           : boolean;
    // Output timeout - milliseconds
    FOutputTimeout             : word;
    // Timeout for ReadData
    FInputTimeout              : DWORD;
    // Set to TRUE to prevent hangs when no device connected or
    // device is OFF
    FCkLineStatus              : boolean;
    // This is used for the timer
    FNotifyWnd                 : HWND;
    // Temporary buffer (RX) - used internally
    FTempInBuffer              : pointer;
    // Time of the first byte of current RX packet
    FFirstByteOfPacketTime     : DWORD;
    // Number of RX polling timer pauses
    FRXPollingPauses           : integer;

    FCancel                    : Boolean;

    // Sets the COM port handle
    procedure SetHandle( Value: HFILE );
    // Selects the COM port to use
    procedure SetPort( Value: TPortNumber );
    // Sets the port name
    procedure SetPortName( Value: string );
    // Selects the baud rate
    procedure SetBaudRate( Value: TBaudRate );
    // Selects the baud rate ( actual baud rate value )
    procedure SetBaudRateValue( Value: DWORD );
    // Selects the number of data bits
    procedure SetDataBits( Value: TDataBits );
    // Selects the number of stop bits
    procedure SetStopBits( Value: TStopBits );
    // Selects the kind of parity
    procedure SetParity( Value: TParity );
    // Selects the kind of hardware flow control
    procedure SetHwFlowControl( Value: THwFlowControl );
    // Selects the kind of software flow control
    procedure SetSwFlowControl( Value: TSwFlowControl );
    // Sets the RX buffer size
    procedure SetInBufSize( Value: DWORD );
    // Sets the TX buffer size
    procedure SetOutBufSize( Value: DWORD );
    // Sets the size of incoming packets
    procedure SetPacketSize( Value: smallint );
    // Sets the timeout for incoming packets
    procedure SetPacketTimeout( Value: integer );
    // Sets the delay between polling checks 
    procedure SetPollingDelay( Value: word );
    // Applies current settings to open COM port 
    function ApplyCOMSettings: boolean;
    // Polling proc 
    procedure TimerWndProc( var msg: TMessage );
  public
    // Constructor 
    constructor Create( AOwner: TComponent ); override;
    // Destructor
    destructor Destroy; override;

    // Opens the COM port and takes of it. Returns false if something
    // goes wrong.
    function Connect: boolean;
    // Closes the COM port and releases control of it
    procedure Disconnect;
    // Returns true if COM port has been opened
    function Connected: boolean;
    // Returns the current state of CTS, DSR, RING and RLSD (CD) lines.
    // The function fails if the hardware does not support the control-register
    // values (that is, returned set is always empty).
    function GetLineStatus: TLineStatusSet;
    // Returns true if polling has not been paused 
    function IsPolling: boolean;
    // Pauses polling 
    procedure PausePolling;
    // Re-starts polling (after pause) 
    procedure ContinuePolling;
    // Flushes the rx/tx buffers
    function FlushBuffers( inBuf, outBuf: boolean ): boolean;
    // Returns number of received bytes in the RX buffer 
    function CountRX: integer;
    // Returns the output buffer free space or 65535 if not connected 
    function OutFreeSpace: word;
    // Sends binary data 
    function SendData( DataPtr: pointer; DataSize: DWORD ): DWORD;
    // Sends binary data. Returns number of bytes sent. Timeout overrides
    // the value specifiend in the OutputTimeout property
    function SendDataEx( DataPtr: pchar; DataSize, Timeout: DWORD ): DWORD;
    // Sends a byte. Returns true if the byte has been sent
    function SendByte( Value: byte ): boolean;
    // Sends a char. Returns true if the char has been sent
    function SendChar( Value: char ): boolean;
    // Sends a pascal string (NULL terminated if $H+ (default))
    function SendString( s: string ): boolean;
    // Sends a C-style strings (NULL terminated) 
    function SendZString( s: pchar ): boolean;
    // Reads binary data. Returns number of bytes read 
    function ReadData( DataPtr: pchar; MaxDataSize: DWORD ): DWORD;
    // Reads a byte. Returns true if the byte has been read 
    function ReadByte( var Value: byte ): boolean;
    // Reads a char. Returns true if char has been read 
    function ReadChar( var Value: char ): boolean;
    // Set DTR line high (onOff=TRUE) or low (onOff=FALSE).
    // You must not use HW handshaking.
    procedure ToggleDTR( onOff: boolean );
    // Set RTS line high (onOff=TRUE) or low (onOff=FALSE).
    // You must not use HW handshaking.
    procedure ToggleRTS( onOff: boolean );

    // Make the Handle of the COM port public (for TAPI...) [read/write]
    property Handle: HFILE read FHandle write SetHandle;
  published
    // # of the COM Port to use ( or pnCustom for port by name )
    property Port: TPortNumber read FPort write SetPort default pnCOM2;
    // Name of COM port
    property PortName: string read FPortName write SetPortName;
    // Speed ( Baud Rate )
    property BaudRate: TBaudRate read FBaudRate write SetBaudRate default br9600;
    // Speed ( Actual Baud Rate value )
    property BaudRateValue: DWORD read FBaudRateValue write SetBaudRateValue default 9600;
    // Data bits to use (5..8, for the 8250 the use of 5 data bits with 2 stop
    // bits is an invalid combination, as is 6, 7, or 8 data bits with 1.5 stop
    // bits)
    property DataBits: TDataBits read FDataBits write SetDataBits default db8BITS;
    // Stop bits to use (1, 1.5, 2)
    property StopBits: TStopBits read FStopBits write SetStopBits default sb1BITS;
    // Kind of Parity to use (none,odd,even,mark,space)
    property Parity: TParity read FParity write SetParity default ptNONE;
    // Kind of Hardware Flow Control to use:
    //   hfNONE          none
    //   hfNONERTSON     no flow control but keep RTS line on
    //   hfRTSCTS        Request-To-Send/Clear-To-Send
    property HwFlow: THwFlowControl read FHwFlow write SetHwFlowControl default hfNONERTSON;
    // Kind of Software Flow Control to use:
    //   sfNONE          none
    //   sfXONXOFF       XON/XOFF 
    property SwFlow: TSwFlowControl read FSwFlow write SetSwFlowControl default sfNONE;
    // Input Buffer size ( suggested - driver might ignore this setting ! )
    property InBufSize: DWORD read FInBufSize write SetInBufSize default 2048;
    // Output Buffer size ( suggested - driver usually ignores this setting ! )
    property OutBufSize: DWORD read FOutBufSize write SetOutBufSize default 2048;
    // RX packet size ( this value must be less than InBufSize )
    // A value <= 0 means "no packet mode" ( i.e. standard mode enabled )
    property PacketSize: smallint read FPacketSize write SetPacketSize default -1;
    // Timeout (ms) for a complete packet (in RX) 
    property PacketTimeout: integer read FPacketTimeout write SetPacketTimeout default -1;
    // What to do with incomplete packets (in RX)
    property PacketMode: TPacketMode read FPacketMode write FPacketMode default pmDiscard;
    // ms of delay between COM port pollings  
    property PollingDelay: word read FPollingDelay write SetPollingDelay default 50;
    // Set to TRUE to enable DTR line on connect and to leave it on until disconnect.
    // Set to FALSE to disable DTR line on connect. 
    property EnableDTROnOpen: boolean read FEnableDTROnOpen write FEnableDTROnOpen default true;
    // Output timeout (milliseconds)
    property OutputTimeout: word read FOutputTimeOut write FOutputTimeout default 500;
    // Input timeout (milliseconds)
    property InputTimeout: DWORD read FInputTimeOut write FInputTimeout default 200;
    // Set to TRUE to prevent hangs when no device connected or device is OFF
    property CheckLineStatus: boolean read FCkLineStatus write FCkLineStatus default false;
    // Event to raise when there is data available (input buffer has data)
    // (called only if PacketSize <= 0)
    property OnReceiveData: TReceiveDataEvent read FOnReceiveData write FOnReceiveData;
    // Event to raise when there is data packet available (called only if PacketSize > 0)
    property OnReceivePacket: TReceivePacketEvent read FOnReceivePacket write FOnReceivePacket;
  end;

function BaudRateOf( bRate: TBaudRate ): DWORD;
function DelayForRX( bRate: TBaudRate; DataSize: DWORD ): DWORD;

implementation

const
  Win32BaudRates: array[br110..br256000] of DWORD =
    ( CBR_110, CBR_300, CBR_600, CBR_1200, CBR_2400, CBR_4800, CBR_9600,
      CBR_14400, CBR_19200, CBR_38400, CBR_56000, CBR_57600, CBR_115200,
      CBR_128000, CBR_256000 );

const
  dcb_Binary              = $00000001;
  dcb_ParityCheck         = $00000002;
  dcb_OutxCtsFlow         = $00000004;
  dcb_OutxDsrFlow         = $00000008;
  dcb_DtrControlMask      = $00000030;
    dcb_DtrControlDisable   = $00000000;
    dcb_DtrControlEnable    = $00000010;
    dcb_DtrControlHandshake = $00000020;
  dcb_DsrSensivity        = $00000040;
  dcb_TXContinueOnXoff    = $00000080;
  dcb_OutX                = $00000100;
  dcb_InX                 = $00000200;
  dcb_ErrorChar           = $00000400;
  dcb_NullStrip           = $00000800;
  dcb_RtsControlMask      = $00003000;
    dcb_RtsControlDisable   = $00000000;
    dcb_RtsControlEnable    = $00001000;
    dcb_RtsControlHandshake = $00002000;
    dcb_RtsControlToggle    = $00003000;
  dcb_AbortOnError        = $00004000;
  dcb_Reserveds           = $FFFF8000;

function GetWinPlatform: string;
var ov: TOSVERSIONINFO;
begin
  ov.dwOSVersionInfoSize := sizeof(ov);
  if GetVersionEx( ov ) then
  begin
    case ov.dwPlatformId of
      VER_PLATFORM_WIN32s: // Win32s on Windows 3.1
        Result := 'W32S';
      VER_PLATFORM_WIN32_WINDOWS: // Win32 on Windows 95/98
        Result := 'W95';
      VER_PLATFORM_WIN32_NT: //	Windows NT
        Result := 'WNT';
    end;
  end
  else
    Result := '??';
end;

function GetWinVersion: DWORD;
var ov: TOSVERSIONINFO;
begin
  ov.dwOSVersionInfoSize := sizeof(ov);
  if GetVersionEx( ov ) then
    Result := MAKELONG( ov.dwMinorVersion, ov.dwMajorVersion )
  else
    Result := $00000000;
end;

function BaudRateOf( bRate: TBaudRate ): DWORD;
begin
  if bRate = brCustom then
    Result := 0
  else
    Result := Win32BaudRates[ bRate ];
end;

function DelayForRX( bRate: TBaudRate; DataSize: DWORD ): DWORD;
begin
  Result := round( DataSize / (BaudRateOf(bRate) / 10) * 1000 );
end;

constructor TCommPortDriver.Create( AOwner: TComponent );
begin
  inherited Create( AOwner );
  // Initialize to default values -----------------------
  // not canceled
  FCancel := false;
  // Not connected
  FHandle                    := INVALID_HANDLE_VALUE;
  // COM 2
  FPort                      := pnCOM2;
  FPortName                  := '\\.\COM2';
  // 9600 bauds
  FBaudRate                  := br9600;
  FBaudRateValue             := BaudRateOf( br9600 );
  // 8 data bits
  FDataBits                  := db8BITS;
  // 1 stop bit
  FStopBits                  := sb1BITS;
  // no parity
  FParity                    := ptNONE;
  // No hardware flow control but RTS on
  FHwFlow                    := hfNONERTSON;
  // No software flow control
  FSwFlow                    := sfNONE;
  // Input buffer of 2048 bytes
  FInBufSize                 := 2048;
  // Output buffer of 2048 bytes
  FOutBufSize                := 2048;
  // Don't pack data
  FPacketSize                := -1;
  // Packet timeout disabled 
  FPacketTimeout             := -1;
  // Discard incomplete packets
  FPacketMode                := pmDiscard;
  // Poll COM port every 50ms
  FPollingDelay              := 50;
  // Output timeout of 500ms
  FOutputTimeout             := 500;
  // Timeout for ReadData(), 200ms
  FInputTimeout              := 200;
  // DTR high on connect
  FEnableDTROnOpen           := true;
  // Time not valid ( used by the packing routines )
  FFirstByteOfPacketTime     := DWORD(-1);
  // Don't check of off-line devices
  FCkLineStatus              := false;
  // Init number of RX polling timer pauses - not paused
  FRXPollingPauses := 0;
  // Temporary buffer for received data 
  FTempInBuffer := AllocMem( FInBufSize );
  // Allocate a window handle to catch timer's notification messages
  if not (csDesigning in ComponentState) then
    FNotifyWnd := AllocateHWnd( TimerWndProc );
end;

destructor TCommPortDriver.Destroy;
begin
  // Be sure to release the COM port
  Disconnect;
  // Free the temporary buffer
  FreeMem( FTempInBuffer, FInBufSize );
  // Destroy the timer's window
  if not (csDesigning in ComponentState) then
    DeallocateHWnd( FNotifyWnd );
  // Call inherited destructor
  inherited Destroy;
end;

// The COM port handle made public and writeable.
// This lets you connect to external opened com port.
// Setting ComPortHandle to INVALID_PORT_HANDLE acts as Disconnect.
procedure TCommPortDriver.SetHandle( Value: HFILE );
begin
  // If same COM port then do nothing
  if FHandle = Value then
    exit;
  // If value is RELEASE_NOCLOSE_PORT then stop controlling the COM port
  // without closing in
  if Value = RELEASE_NOCLOSE_PORT then
  begin
    // Stop the timer
    if Connected then
      KillTimer( FNotifyWnd, 1 );
    // No more connected 
    FHandle := INVALID_HANDLE_VALUE;
  end
  else
  begin
    // Disconnect
    Disconnect;
    // If Value is INVALID_HANDLE_VALUE then exit now
    if Value = INVALID_HANDLE_VALUE then
      exit;
    // Set COM port handle
    FHandle := Value;
    // Start the timer ( used for polling )
    SetTimer( FNotifyWnd, 1, FPollingDelay, nil );
  end;
end;

// Selects the COM port to use
procedure TCommPortDriver.SetPort( Value: TPortNumber );
begin
  // Be sure we are not using any COM port
  if Connected then
    exit;
  // Change COM port
  FPort := Value;
  // Update the port name
  if FPort <> pnCustom then
    FPortName := Format( '\\.\COM%d', [ord(FPort)] );
end;

// Sets the port name
procedure TCommPortDriver.SetPortName( Value: string );
begin
  // Be sure we are not using any COM port
  if Connected then
    exit;
  // Change COM port
  FPort := pnCustom;
  // Update the port name
  FPortName := Value;
end;

// Selects the baud rate
procedure TCommPortDriver.SetBaudRate( Value: TBaudRate );
begin
  // Set new COM speed
  FBaudRate := Value;
  if FBaudRate <> brCustom then
    FBaudRateValue := BaudRateOf( FBaudRate );
  // Apply changes
  if Connected then
    ApplyCOMSettings;
end;

// Selects the baud rate ( actual baud rate value )
procedure TCommPortDriver.SetBaudRateValue( Value: DWORD );
begin
  // Set new COM speed
  FBaudRate := brCustom;
  FBaudRateValue := Value;
  // Apply changes
  if Connected then
    ApplyCOMSettings;
end;

// Selects the number of data bits
procedure TCommPortDriver.SetDataBits( Value: TDataBits );
begin
  // Set new data bits
  FDataBits := Value;
  // Apply changes 
  if Connected then
    ApplyCOMSettings;
end;

// Selects the number of stop bits
procedure TCommPortDriver.SetStopBits( Value: TStopBits );
begin
  // Set new stop bits
  FStopBits := Value;
  // Apply changes 
  if Connected then
    ApplyCOMSettings;
end;

// Selects the kind of parity
procedure TCommPortDriver.SetParity( Value: TParity );
begin
  // Set new parity
  FParity := Value;
  // Apply changes 
  if Connected then
    ApplyCOMSettings;
end;

// Selects the kind of hardware flow control
procedure TCommPortDriver.SetHwFlowControl( Value: THwFlowControl );
begin
  // Set new hardware flow control
  FHwFlow := Value;
  // Apply changes 
  if Connected then
    ApplyCOMSettings;
end;

// Selects the kind of software flow control
procedure TCommPortDriver.SetSwFlowControl( Value: TSwFlowControl );
begin
  // Set new software flow control
  FSwFlow := Value;
  // Apply changes 
  if Connected then
    ApplyCOMSettings;
end;

// Sets the RX buffer size
procedure TCommPortDriver.SetInBufSize( Value: DWORD );
begin
  // Do nothing if connected
  if Connected then
    exit;
  // Free the temporary input buffer
  FreeMem( FTempInBuffer, FInBufSize );
  // Set new input buffer size
  if Value > 8192 then
    Value := 8192
  else if Value < 128 then
    Value := 128;
  FInBufSize := Value;
  // Allocate the temporary input buffer
  FTempInBuffer := AllocMem( FInBufSize );
  // Adjust the RX packet size
  SetPacketSize( FPacketSize );
end;

// Sets the TX buffer size
procedure TCommPortDriver.SetOutBufSize( Value: DWORD );
begin
  // Do nothing if connected
  if Connected then
    exit;
  // Set new output buffer size
  if Value > 8192 then
    Value := 8192
  else if Value < 128 then
    Value := 128;
  FOutBufSize := Value;
end;

// Sets the size of incoming packets
procedure TCommPortDriver.SetPacketSize( Value: smallint );
begin
  // PackeSize <= 0 if data isn't to be 'packetized'
  if Value <= 0 then
    FPacketSize := -1
  // If the PacketSize if greater than then RX buffer size then
  // increase the RX buffer size
  else if DWORD(Value) > FInBufSize then
  begin
    FPacketSize := Value;
    SetInBufSize( FPacketSize );
  end;
end;

// Sets the timeout for incoming packets
procedure TCommPortDriver.SetPacketTimeout( Value: integer );
begin
  // PacketTimeout <= 0 if packet timeout is to be disabled
  if Value < 1 then
    FPacketTimeout := -1
  // PacketTimeout cannot be less than polling delay + some extra ms 
  else if Value < FPollingDelay then
    FPacketTimeout := FPollingDelay + (FPollingDelay*40) div 100;
end;

// Sets the delay between polling checks
procedure TCommPortDriver.SetPollingDelay( Value: word );
begin
  // Make it greater than 4 ms
  if Value < 5 then
    Value := 5;
  // If new delay is not equal to previous value...
  if Value <> FPollingDelay then
  begin
    // Stop the timer 
    if Connected then
      KillTimer( FNotifyWnd, 1 );
    // Store new delay value
    FPollingDelay := Value;
    // Restart the timer
    if Connected then
      SetTimer( FNotifyWnd, 1, FPollingDelay, nil );
    // Adjust the packet timeout 
    SetPacketTimeout( FPacketTimeout );
  end;
end;

// Apply COM settings 
function TCommPortDriver.ApplyCOMSettings: boolean;
var dcb: TDCB;
begin
  // Do nothing if not connected
  Result := false;
  if not Connected then
    exit;

  // ** Setup DCB (Device Control Block) fields ******************************

  // Clear all
  fillchar( dcb, sizeof(dcb), 0 );
  // DCB structure size
  dcb.DCBLength := sizeof(dcb);
  // Baud rate
  dcb.BaudRate := FBaudRateValue;
  // Set fBinary: Win32 does not support non binary mode transfers
  // (also disable EOF check) 
  dcb.Flags := dcb_Binary;
  // Enables the DTR line when the device is opened and leaves it on 
  if EnableDTROnOpen then
    dcb.Flags := dcb.Flags or dcb_DtrControlEnable;
  // Kind of hw flow control to use
  case FHwFlow of
    // No hw flow control
    hfNONE:;
    // No hw flow control but set RTS high and leave it high
    hfNONERTSON:
      dcb.Flags := dcb.Flags or dcb_RtsControlEnable;
    // RTS/CTS (request-to-send/clear-to-send) flow control
    hfRTSCTS:
      dcb.Flags := dcb.Flags or dcb_OutxCtsFlow or dcb_RtsControlHandshake;
  end;
  // Kind of sw flow control to use
  case FSwFlow of
    // No sw flow control
    sfNONE:;
    // XON/XOFF sw flow control
    sfXONXOFF:
      dcb.Flags := dcb.Flags or dcb_OutX or dcb_InX;
  end;
  // Set XONLim: specifies the minimum number of bytes allowed in the input
  // buffer before the XON character is sent (or CTS is set). 
  if (GetWinPlatform = 'WNT') and (GetWinVersion >= $00040000) then
  begin
    // WinNT 4.0 + Service Pack 3 needs XONLim to be less than or
    // equal to 4096 bytes. Win95/98 doesn't have such limit. 
    if FInBufSize div 4 > 4096 then
      dcb.XONLim := 4096
    else
      dcb.XONLim := FInBufSize div 4;
  end
  else
    dcb.XONLim := FInBufSize div 4;
  // Specifies the maximum number of bytes allowed in the input buffer before
  // the XOFF character is sent (or CTS is set low). The maximum number of bytes
  // allowed is calculated by subtracting this value from the size, in bytes, of
  // the input buffer.
  dcb.XOFFLim := dcb.XONLim;
  // How many data bits to use
  dcb.ByteSize := 5 + ord(FDataBits);
  // Kind of parity to use
  dcb.Parity := ord(FParity);
  // How many stop bits to use
  dcb.StopBits := ord(FStopbits);
  // XON ASCII char - DC1, Ctrl-Q, ASCII 17
  dcb.XONChar := #17;
  // XOFF ASCII char - DC3, Ctrl-S, ASCII 19
  dcb.XOFFChar := #19;

  // Apply new settings
  Result := SetCommState( FHandle, dcb );
  if not Result then
    exit;
  // Flush buffers
  Result := FlushBuffers( true, true );
  if not Result then
    exit;
  // Setup buffers size
  Result := SetupComm( FHandle, FInBufSize, FOutBufSize );
end;

function TCommPortDriver.Connect: boolean;
var tms: TCOMMTIMEOUTS;
begin
  // not canceled
  FCancel := false;

  // Do nothing if already connected
  Result := Connected;
  if Result then
    exit;
  // Open the COM port
  FHandle := CreateFile( pchar(FPortName),
                         GENERIC_READ or GENERIC_WRITE,
                         0, // Not shared 
                         nil, // No security attributes 
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         0 // No template 
                        ) ;
  Result := Connected;
  if not Result then
    exit;
  // Apply settings
  Result := ApplyCOMSettings;
  if not Result then
  begin
    Disconnect;
    exit;
  end;
  // Set ReadIntervalTimeout: Specifies the maximum time, in milliseconds,
  // allowed to elapse between the arrival of two characters on the
  // communications line.
  // We disable timeouts because we are polling the com port!
  tms.ReadIntervalTimeout := 1;
  // Set ReadTotalTimeoutMultiplier: Specifies the multiplier, in milliseconds,
  // used to calculate the total time-out period for read operations.
  tms.ReadTotalTimeoutMultiplier := 0;
  // Set ReadTotalTimeoutConstant: Specifies the constant, in milliseconds,
  // used to calculate the total time-out period for read operations.
  tms.ReadTotalTimeoutConstant := 1;
  // Set WriteTotalTimeoutMultiplier: Specifies the multiplier, in milliseconds,
  // used to calculate the total time-out period for write operations.
  tms.WriteTotalTimeoutMultiplier := 0;
  // Set WriteTotalTimeoutConstant: Specifies the constant, in milliseconds,
  // used to calculate the total time-out period for write operations.
  tms.WriteTotalTimeoutConstant := 10;
  // Apply timeouts
  SetCommTimeOuts( FHandle, tms );
  // Start the timer (used for polling)
  SetTimer( FNotifyWnd, 1, FPollingDelay, nil );
end;

procedure TCommPortDriver.Disconnect;
begin
    // not canceled
  FCancel := true;


  if Connected then
  begin
    // Stop the timer (used for polling)
    KillTimer( FNotifyWnd, 1 );
    // Release the COM port
    CloseHandle( FHandle );
    // No more connected
    FHandle := INVALID_HANDLE_VALUE;
  end;
end;

// Returns true if connected
function TCommPortDriver.Connected: boolean;
begin
  Result := FHandle <> INVALID_HANDLE_VALUE;
end;

// Returns CTS, DSR, RING and RLSD (CD) signals status
function TCommPortDriver.GetLineStatus: TLineStatusSet;
var dwS: DWORD;
begin
  Result := [];
  if not Connected then
    exit;
  // Retrieves modem control-register values.
  // The function fails if the hardware does not support the control-register
  // values. 
  if not GetCommModemStatus( FHandle, dwS ) then
    exit;
  if dwS and MS_CTS_ON <> 0 then Result := Result + [lsCTS];
  if dwS and MS_DSR_ON <> 0 then Result := Result + [lsDSR];
  if dwS and MS_RING_ON <> 0 then Result := Result + [lsRING];
  if dwS and MS_RLSD_ON <> 0 then Result := Result + [lsCD];
end;

// Returns true if polling has not been paused 
function TCommPortDriver.IsPolling: boolean;
begin
  Result := FRXPollingPauses <= 0;
end;

// Pauses polling 
procedure TCommPortDriver.PausePolling;
begin
  // Inc. RX polling pauses counter 
  inc( FRXPollingPauses );
end;

// Re-starts polling (after pause) 
procedure TCommPortDriver.ContinuePolling;
begin
  // Dec. RX polling pauses counter 
  dec( FRXPollingPauses );
end;

// Flush rx/tx buffers 
function TCommPortDriver.FlushBuffers( inBuf, outBuf: boolean ): boolean;
var dwAction: DWORD;
begin
  // Do nothing if not connected
  Result := false;
  if not Connected then
    exit;
  // Flush the RX data buffer
  dwAction := 0;
  if outBuf then
    dwAction := dwAction or PURGE_TXABORT or PURGE_TXCLEAR;
  // Flush the TX data buffer
  if inBuf then
    dwAction := dwAction or PURGE_RXABORT or PURGE_RXCLEAR;
  Result := PurgeComm( FHandle, dwAction );
  // Used by the RX packet mechanism
  if Result then
    FFirstByteOfPacketTime := DWORD(-1);
end;

// Returns number of received bytes in the RX buffer
function TCommPortDriver.CountRX: integer;
var stat: TCOMSTAT;
    errs: DWORD;
begin
  // Do nothing if port has not been opened 
  Result := 65535;
  if not Connected then
    exit;
  // Get count 
  ClearCommError( FHandle, errs, @stat );
  Result := stat.cbInQue;
end;

// Returns the output buffer free space or 65535 if not connected 
function TCommPortDriver.OutFreeSpace: word;
var stat: TCOMSTAT;
    errs: DWORD;
begin
  if not Connected then
    Result := 65535
  else
  begin
    ClearCommError( FHandle, errs, @stat );
    Result := FOutBufSize - stat.cbOutQue;
  end;
end;

// Sends binary data. Returns number of bytes sent. Timeout overrides
// the value specifiend in the OutputTimeout property
function TCommPortDriver.SendDataEx( DataPtr: pchar; DataSize, Timeout: DWORD ): DWORD;
var nToSend, nSent, t1: DWORD;
begin
  // Do nothing if port has not been opened
  Result := 0;
  if not Connected then
    exit;
  // Current time
  t1 := GetTickCount;
  // Loop until all data sent or timeout occurred
  while DataSize > 0 do
  begin
    // Get TX buffer free space
    nToSend := OutFreeSpace;
    // If output buffer has some free space...
    if nToSend > 0 then
    begin
      // Check signals
      if FCkLineStatus and (GetLineStatus = []) then
        exit;
      // Don't send more bytes than we actually have to send
      if nToSend > DataSize then
        nToSend := DataSize;
      // Send
      WriteFile( FHandle, DataPtr^, nToSend, nSent, nil );
      nSent := abs( nSent );
      if nSent > 0 then
      begin
        // Update number of bytes sent
        Result := Result + nSent;
        // Decrease the count of bytes to send 
        DataSize := DataSize - nSent;
        // Inc. data pointer 
        DataPtr := DataPtr + nSent;
        // Get current time 
        t1 := GetTickCount;
        // Continue. This skips the time check below (don't stop
        // trasmitting if the Timeout is set too low)
        continue;
      end;
    end;
    // Buffer is full. If we are waiting too long then exit 
    if DWORD(GetTickCount-t1) > Timeout then
      exit;
  end;
end;

// Send data (breaks the data in small packets if it doesn't fit in the output
// buffer)
function TCommPortDriver.SendData( DataPtr: pointer; DataSize: DWORD ): DWORD;
begin
  Result := SendDataEx( DataPtr, DataSize, FOutputTimeout );
end;

// Sends a byte. Returns true if the byte has been sent
function TCommPortDriver.SendByte( Value: byte ): boolean;
begin
  Result := SendData( @Value, 1 ) = 1;
end;

// Sends a char. Returns true if the char has been sent
function TCommPortDriver.SendChar( Value: char ): boolean;
begin
  Result := SendData( @Value, 1 ) = 1;
end;

// Sends a pascal string (NULL terminated if $H+ (default))
function TCommPortDriver.SendString( s: string ): boolean;
var len: DWORD;
begin
  len := length( s );
  {$IFOPT H+}  // New syle pascal string (NULL terminated)
  Result := SendData( pchar(s), len ) = len;
  {$ELSE} // Old style pascal string (s[0] = length)
  Result := SendData( pchar(@s[1]), len ) = len;
  {$ENDIF}
end;

// Sends a C-style string (NULL terminated) 
function TCommPortDriver.SendZString( s: pchar ): boolean;
var len: DWORD;
begin
  len := strlen( s );
  Result := SendData( s, len ) = len;
end;

// Reads binary data. Returns number of bytes read 
function TCommPortDriver.ReadData( DataPtr: pchar; MaxDataSize: DWORD ): DWORD;
var nToRead, nRead, t1: DWORD;
begin
  // Do nothing if port has not been opened 
  Result := 0;
  if not Connected then
    exit;
  // Pause polling 
  PausePolling;
  // Current time
  t1 := GetTickCount;
  // Loop until all requested data read or timeout occurred
  while MaxDataSize > 0 do
  begin
    Application.ProcessMessages; // ##Vg process these messages

    if FCancel then exit;

    // Get data bytes count in RX buffer
    nToRead := CountRX;
    // If input buffer has some data... 
    if nToRead > 0 then
    begin
      // Don't read more bytes than we actually have to read 
      if nToRead > MaxDataSize then
        nToRead := MaxDataSize;
      // Read 
      ReadFile( FHandle, DataPtr^, nToRead, nRead, nil );
      // Update number of bytes read 
      Result := Result + nRead;
      // Decrease the count of bytes to read 
      MaxDataSize := MaxDataSize - nRead;
      // Inc. data pointer 
      DataPtr := DataPtr + nRead;
      // Get current time 
      t1 := GetTickCount;
      // Continue. This skips the time check below (don't stop
      // reading if the FInputTimeout is set too low)
      continue;
    end;
    // Buffer is empty. If we are waiting too long then exit 
    if (GetTickCount-t1) > FInputTimeout then
      break;
  end;
  // Continue polling 
  ContinuePolling;
end;

// Reads a byte. Returns true if the byte has been read 
function TCommPortDriver.ReadByte( var Value: byte ): boolean;
begin
  Result := ReadData( @Value, 1 ) = 1;
end;

// Reads a char. Returns true if char has been read 
function TCommPortDriver.ReadChar( var Value: char ): boolean;
begin
  Result := ReadData( @Value, 1 ) = 1;
end;

// Set DTR line high (onOff=TRUE) or low (onOff=FALSE).
// You must not use HW handshaking.
procedure TCommPortDriver.ToggleDTR( onOff: boolean );
const funcs: array[boolean] of integer = (CLRDTR,SETDTR);
begin
  if Connected then
    EscapeCommFunction( FHandle, funcs[onOff] );
end;

// Set RTS line high (onOff=TRUE) or low (onOff=FALSE).
// You must not use HW handshaking.
procedure TCommPortDriver.ToggleRTS( onOff: boolean );
const funcs: array[boolean] of integer = (CLRRTS,SETRTS);
begin
  if Connected then
    EscapeCommFunction( FHandle, funcs[onOff] );
end;

// COM port polling proc
procedure TCommPortDriver.TimerWndProc( var msg: TMessage );
var nRead, nToRead, dummy: DWORD;
    comStat: TCOMSTAT;
begin
  if (msg.Msg = WM_TIMER) and Connected then
  begin
    // Do nothing if RX polling has been paused 
    if FRXPollingPauses > 0 then
      exit;
    // If PacketSize is > 0 then raise the OnReceiveData event only if the RX
    // buffer has at least PacketSize bytes in it.
    ClearCommError( FHandle, dummy, @comStat );
    if FPacketSize > 0 then
    begin
      // Complete packet received ?
      if DWORD(comStat.cbInQue) >= DWORD(FPacketSize) then
      begin
        repeat
          // Read the packet and pass it to the app
          nRead := 0;
          if ReadFile( FHandle, FTempInBuffer^, FPacketSize, nRead, nil ) then
            if (nRead <> 0) and Assigned(FOnReceivePacket) then
              FOnReceivePacket( Self, FTempInBuffer, nRead );
          // Adjust time
          //if comStat.cbInQue >= FPacketSize then
            FFirstByteOfPacketTime := FFirstByteOfPacketTime +
                                      DelayForRX( FBaudRate, FPacketSize );
          comStat.cbInQue := comStat.cbInQue - WORD(FPacketSize);
          if comStat.cbInQue = 0 then
            FFirstByteOfPacketTime := DWORD(-1);
        until DWORD(comStat.cbInQue) < DWORD(FPacketSize);
        // Done
        exit;
      end;
      // Handle packet timeouts
      if (FPacketTimeout > 0) and (FFirstByteOfPacketTime <> DWORD(-1)) and
         (GetTickCount - FFirstByteOfPacketTime > DWORD(FPacketTimeout)) then
      begin
        nRead := 0;
        // Read the "incomplete" packet
        if ReadFile( FHandle, FTempInBuffer^, comStat.cbInQue, nRead, nil ) then
          // If PacketMode is not pmDiscard then pass the packet to the app
          if (FPacketMode <> pmDiscard) and (nRead <> 0) and Assigned(FOnReceivePacket) then
            FOnReceivePacket( Self, FTempInBuffer, nRead );
        // Restart waiting for a packet 
        FFirstByteOfPacketTime := DWORD(-1);
        // Done 
        exit;
      end;
      // Start time 
      if (comStat.cbInQue > 0) and (FFirstByteOfPacketTime = DWORD(-1)) then
        FFirstByteOfPacketTime := GetTickCount;
      // Done 
      exit;
    end;

    // Standard data handling 
    nRead := 0;
    nToRead := comStat.cbInQue;
    if (nToRead > 0) and ReadFile( FHandle, FTempInBuffer^, nToRead, nRead, nil ) then
      if (nRead <> 0) and Assigned(FOnReceiveData) then
        FOnReceiveData( Self, FTempInBuffer, nRead );
  end
  // Let Windows handle other messages 
  else
    Msg.Result := DefWindowProc( FNotifyWnd, Msg.Msg, Msg.wParam, Msg.lParam ) ;
end;

end.
