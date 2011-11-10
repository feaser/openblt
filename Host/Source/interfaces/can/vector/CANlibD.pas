unit CANlibD;

(*----------------------------------------------------------------------------
| File:
|   CANlibD.pas
| Project:
|   Unit for Delphi32 samples (V2.0)
|
|-----------------------------------------------------------------------------
| Ported from cantrace.c by Torsten Lang
|-----------------------------------------------------------------------------
| Copyright (c) 1998 BSK Datentechnik GmbH, Kiesacker 14, 35418 Buseck /
| 1998 by Vector Informatik GmbH, Friolzheimer Str. 6, 70499 Stuttgart
| All rights reserved.
|
| BSK Datentechnik räumt der Vector Informatik GmbH das nicht ausschließliche Recht
| ein, CANlibD.pas für eigene Zwecke zu nutzen. Vector ist es gestattet, die
| Software zu kopieren, abzuändern, zu erweitern, weiterzugeben und in Software von
| Vector zu integrieren. Im Quelltext enthaltene Copyright-Hinweise dürfen hierbei
| nicht entfernt oder geändert werden.
| Vector darf aus von ihm vorgenommenen Anpassungen und/oder Erweiterungen der
| CANlibD.pas keine Rechte an irgendwelchen Teilen der Software gegenüber BSK
| Datentechnik geltend machen.
 ----------------------------------------------------------------------------*)

{ environment switches }
(*******************************************************************************
Attention:
1. All functions that use pointers to structures (either explicitly or by using
   var parameters) may change the contents of the structures right after the
   funtction returns.
*******************************************************************************)
{$ifdef WIN32}
{$define CanLib4Delphi32}
{$else}
{$ifdef CONSOLE}
{$define CanLib4Delphi32}
{$endif}
{$endif}

{ public interface }

interface

type
  ncdStringType = PChar;
  Vstatus = Word;

const
  MAX_APPNAME                 =  32;

  VCAN_WAIT                   =   0;
  VCAN_POLL                   =   1;
  {$ifdef CanLib4Delphi32}
  { Attention: This exists only under Win32 }
  VCAN_NOTIFY                 =   2;
  {$endif}

  VSUCCESS                    =   0;
  VPENDING                    =   1;
  VERROR                      = 255;
  VERR_QUEUE_IS_EMPTY         =  10;
  VERR_QUEUE_IS_FULL          =  11;
  VERR_TX_NOT_POSSIBLE        =  12;
  VERR_NO_LICENSE             =  14;
  VERR_WRONG_PARAMETER        = 101;
  VERR_TWICE_REGISTER         = 110;
  VERR_INVALID_CHAN_INDEX     = 111;
  VERR_INVALID_ACCESS         = 112;
  VERR_PORT_IS_OFFLINE        = 113;
  VERR_CHAN_IS_ONLINE         = 116;
  VERR_INVALID_PORT           = 118;
  VERR_HW_NOT_READY           = 120;
  VERR_CMD_TIMEOUT            = 121;
  VERR_HW_NOT_PRESENT         = 129;
  VERR_NOTIFY_ALREADY_ACTIVE  = 131;
  VERR_CANNOT_OPEN_DRIVER     = 201;

{
//------------------------------------------------------------------------------
// accessmask
}
type
  Vaccess = LongInt; { unsigned long doesn't exist for Delphi32 / Borland Pascal 7! }

{
//------------------------------------------------------------------------------
// porthandle
}

const
  INVALID_PORTHANDLE  = -1;

type
  VportHandle = LongInt;

{
//------------------------------------------------------------------------------
// acceptance filter
}

type
  VsetAcceptance = packed record
    code : LongInt; {unsigned long doesn't exist!}
    mask : LongInt; {unsigned long doesn't exist!}
  end;
  PVsetAcceptance = ^VsetAcceptance;

{
//------------------------------------------------------------------------------
// bit timing
}

type
  VchipParams = packed record
    bitRate : LongInt; {unsigned long doesn't exist!}
    sjw     : Byte;
    tseg1   : Byte;
    tseg2   : Byte;
    sam     : Byte;    { 1 or 3                             }
  end;
  PVchipParams = ^VchipParams;

{
//------------------------------------------------------------------------------
// definitions for the events and commands used by the driver
}

const
  V_RECEIVE_MSG           =  1;
  V_CHIP_STATE            =  4;
  V_CLOCK_OVERFLOW        =  5;
  V_TRIGGER               =  6;
  V_TIMER                 =  8;
  V_TRANSCEIVER           =  9;
  V_TRANSMIT_MSG          = 10;

type
  VeventTag = Byte;

{
//------------------------------------------------------------------------------
// events
}

{
//------------------------------------------------------------------------------
// structure for V_RECEIVE_MSG
}

const
  MAX_MSG_LEN                 = 8;
  EXT_MSG                     = $80000000; { signs an extended identifier      }

  MSGFLAG_ERROR_FRAME         = $01;       { Msg is a bus error                }
  MSGFLAG_OVERRUN             = $02;       { Msgs following this has been lost }
  MSGFLAG_NERR                = $04;       { NERR active during this msg       }
  MSGFLAG_WAKEUP              = $08;       { Msg rcv'd in wakeup mode          }
  MSGFLAG_REMOTE_FRAME        = $10;       { Msg is a remote frame             }
  MSGFLAG_RESERVED_1          = $20;       { Reserved for future usage         }
  MSGFLAG_TX                  = $40;       { TX acknowledge                    }
  MSGFLAG_TXRQ                = $80;       { TX request                        }

type
  _Vmsg = packed record
    id    : LongInt; {unsigned long doesn't exist!}
    flags : Byte;
    dlc   : Byte;
    data  : array [0..MAX_MSG_LEN-1] of Byte;
  end; { 14 Bytes }
  _PVmsg = ^_Vmsg;

{
// structure for V_CHIP_STATE
}

const
  CHIPSTAT_BUSOFF        = $01;
  CHIPSTAT_ERROR_PASSIVE = $02;
  CHIPSTAT_ERROR_WARNING = $04;
  CHIPSTAT_ERROR_ACTIVE  = $08;

type
  _VchipState = packed record
    busStatus      : Byte;
    txErrorCounter : Byte;
    rxErrorCounter : Byte;
  end;
  _PVchipState = ^_VchipState;

{
// structure for V_TRANSCEIVER
}

const
  TRANSCEIVER_EVENT_ERROR   = 1;
  TRANSCEIVER_EVENT_CHANGED = 2;

  TRANSCEIVER_TYPE_NONE     = 0;
  TRANSCEIVER_TYPE_251      = 1;
  TRANSCEIVER_TYPE_252      = 2;
  TRANSCEIVER_TYPE_DNOPTO   = 3;
  TRANSCEIVER_TYPE_W210     = 4;

  TRANSCEIVER_LINEMODE_NA       = 0;
  TRANSCEIVER_LINEMODE_TWO_LINE = 1;
  TRANSCEIVER_LINEMODE_CAN_H    = 2;
  TRANSCEIVER_LINEMODE_CAN_L    = 3;

  TRANSCEIVER_RESNET_NA          = 0;
  TRANSCEIVER_RESNET_MASTER      = 1;
  TRANSCEIVER_RESNET_MASTER_STBY = 2;
  TRANSCEIVER_RESNET_SLAVE       = 3;

type
  _Vtransceiver = packed record
    event : Byte;       { TRANSCEIVER_EVENT_xxx }
  end;
  _PVtransceiver = ^_Vtransceiver;

  Vevent = packed record
    tag : VeventTag;        { 1                          }
    chanIndex : Byte;       { 1                          }
    _transId : Byte;        { 1 not implemented yet !!!! }
    portHandle : Byte;      { 1 internal use only !!!!   }
    timeStamp : LongInt;    { 4                          } { unsigned long doesn't exist! }
    case {tagData:}Byte of
      0 : (msg : _Vmsg);
      1 : (chipState : _VchipState);
      2 : (transceiver : _Vtransceiver);
                            { 14 Bytes (_VMessage)       }
  end;
                            { --------                   }
                            { 22 Bytes                   }
  PVevent = ^Vevent;

{
//------------------------------------------------------------------------------
// structure for SET_OUTPUT_MODE
}

const
  OUTPUT_MODE_SILENT = 0;
  OUTPUT_MODE_NORMAL = 1;

{
//------------------------------------------------------------------------------
// configuration
}

{
// defines for the supported hardware
}
const
  HWTYPE_NONE        = 0;
  HWTYPE_VIRTUAL     = 1;
  HWTYPE_CANCARDX    = 2;
  HWTYPE_CANPARI     = 3;
  HWTYPE_CANDONGLE   = 4;
  HWTYPE_CANAC2      = 5;
  HWTYPE_CANAC2PCI   = 6;
  HWTYPE_CANCARD     = 7;
  HWTYPE_CANCARDY    = 12;
  HWTYPE_CANCARDXL   = 15;
  HWTYPE_CANCARD2    = 17;
  HWTYPE_EDICCARD    = 19;
  HWTYPE_CANCASEXL          = 21;
  HWTYPE_CANBOARDXL         = 25;
  HWTYPE_CANBOARDXL_COMPACT = 27;
  MAX_HWTYPE                = 27;

{
// defines for the tranceiver type
}
const
  (*
  TRANSCEIVER_TYPE_NONE    = 0;
  TRANSCEIVER_TYPE_251     = 1;
  TRANSCEIVER_TYPE_252     = 2;
  TRANSCEIVER_TYPE_DNOPTO  = 3;
  TRANSCEIVER_TYPE_W210    = 4;
  *) { These have already been defined above }
  MAX_TRANSCEIVER_TYPE     = 4;

  MAX_CHAN_NAME   = 31;
  MAX_DRIVER_NAME = 31;

type
  VChannelConfig = packed record
    name           : array [0..MAX_CHAN_NAME] of Char;
    hwType         : Byte;          { HWTYPE_xxxx (see above)                        }
    hwIndex        : Byte;          { Index of the hardware (same type) (0,1,...)    }
    hwChannel      : Byte;          { Index of the channel (same hardware) (0,1,...) }
    tranceiverType : Byte;          { TRANCEIVER_TYPE_xxxx (see above)               }
    channelIndex   : Byte;          { Global channel index (0,1,...)                 }
    channelMask    : LongInt;       { Global channel mask (=1<<channelIndex)         }
                                    { Sorry, no unsigned long here!                  }
    {
    // Channel
    }
    isOnBus          : Byte;        { The channel is on bus                          }
    chipParams       : VchipParams; { Actual bittiming                               }
    outputMode       : Byte;        { Actual output mode                             }
    flags            : Byte;        { Actual options MSGFLAG_TX,MSGFLAG_TXRQ         }
  end;
  PVChannelConfig = ^VChannelConfig;

  VDriverConfig = packed record
    driverName       : array [0..MAX_DRIVER_NAME] of Char;
    driverVersion    : Word;
    dispatcher       : Pointer;
    channelCount     : Byte;        { total number of channels                       }
    channel : array [0..0] of VChannelConfig;
                                    { [channelCount]                                 }
  end;
  PVDriverConfig = ^VDriverConfig;

function ncdOpenDriver                                                             : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdGetDriverConfig     (var pChanCount : LongInt;
                                 pDriverConfig : PVDriverConfig)                   : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdGetChannelIndex     (hwType     : LongInt; hwIndex : LongInt;
                                 hwChannel  : LongInt)                             : LongInt;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdGetChannelMask      (hwType     : LongInt; hwIndex : LongInt;
                                 hwChannel  : LongInt)                             : Vaccess;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdOpenPort            (var portHandle : VportHandle; userName : PChar;
                                 accessMask : Vaccess; initMask : Vaccess;
                                 var permissionMask : Vaccess;
                                 rxQueueSize : LongInt)                            : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdSetChannelMode      (portHandle : VportHandle; accessMask : Vaccess;
                                 tx, txrq   : LongInt)                             : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdSetChannelOutput    (portHandle : VportHandle; accessMask : Vaccess;
                                 mode       : LongInt)                             : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdSetChannelTransceiver(portHandle : VportHandle; accessMask : Vaccess;
                                 typ : LongInt; lineMode  : LongInt; resNet  : LongInt) : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdSetChannelParams    (portHandle : VportHandle; accessMask : Vaccess;
                                 var pChipParams : VchipParams)                    : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdSetChannelParamsC200(portHandle : VportHandle; accessMask : Vaccess;
                                 btr0, btr1 : Byte)                                : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdSetChannelBitrate   (portHandle : VportHandle; accessMask : Vaccess;
                                 bitrate    : LongInt)                             : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdSetChannelAcceptance(portHandle : VportHandle; accessMask : Vaccess;
                                 var filter : VsetAcceptance)                      : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdSetTimerRate        (portHandle : VportHandle; timerRate  : LongInt)   : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdResetClock          (portHandle : VportHandle)                         : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
{$ifdef CanLib4Delphi32}
function ncdSetNotification     (portHandle : VportHandle; var handle : LongInt;
                                 queueLevel : LongInt)                             : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
{$endif}
function ncdTransmit            (portHandle : VportHandle; accessMask : Vaccess;
                                 var pEvent : Vevent)                              : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdReceive1            (portHandle : VportHandle; var ppEvent : PVevent)  : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
{$ifdef CanLib4Delphi32}
{ Attention: This function works only under Win32! }
function ncdReceive             (portHandle : VportHandle; receiveMode : LongInt;
                                 waitHandle : Word; var pEventCount : LongInt;
                                 var pEventList : Vevent)                          : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
{$endif}
function ncdRequestChipState    (portHandle : VportHandle; accessMask : Vaccess)   : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdFlushTransmitQueue  (portHandle : VportHandle; accessMask : Vaccess)   : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdFlushReceiveQueue   (portHandle : VportHandle)                         : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdGetReceiveQueueLevel(portHandle : VportHandle; var level  : LongInt)   : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdGetState            (portHandle : VportHandle)                         : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdActivateChannel     (portHandle : VportHandle; accessMask : Vaccess)   : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdDeactivateChannel   (portHandle : VportHandle; accessMask : Vaccess)   : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdClosePort           (portHandle : VportHandle)                         : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdCloseDriver                                                            : VStatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdGetErrorString      (err        : Vstatus)                             : ncdStringType;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdGetEventString      (var ev     : Vevent)                              : ncdStringType;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};

function ncdGetApplConfig        (appName        : PChar;    appChannel     : LongInt;
                                  var hwType     : LongInt;    var hwIndex    : LongInt;
                                  var hwChannel  : LongInt  )                      : VStatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdSetApplConfig        (appName     : PChar;    appChannel  : LongInt;
                                  hwType      : LongInt;    hwIndex     : LongInt;
                                  hwChannel   : LongInt )                          : VStatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdGetChannelVersion    (ChannelIndex     : LongInt;    var FwVersion    : LongInt;
                                  var HwVersion    : LongInt;    var SerialNumber : LongInt ) : VStatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};
function ncdSetReceiveMode       (Port          : VportHandle;    ErrorFrame    : Byte;
                                  ChipState    : Byte  )                          : VStatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};

function ncdAddAcceptanceRange   (Port          : VportHandle;    accessMask    : Vaccess;
                                  first_id      : LongInt;        last_id       : LongInt  ) : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};

function ncdRemoveAcceptanceRange(Port          : VportHandle;    accessMask    : Vaccess;
                                  first_id      : LongInt;        last_id       : LongInt  ) : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};

function ncdResetAcceptance      (Port          : VportHandle;    accessMask    : Vaccess;
                                  extended      : LongInt  ) : Vstatus;
                                 {$ifdef CanLib4Delphi32} stdcall {$else} far {$endif};

function CHANNEL_MASK           (val        : Byte)                                : LongInt;
                                 {$ifndef CanLib4Delphi32} far; {$endif}
function SIZEOF_VDRIVERCONFIG   (n          : Byte)                                : LongInt;
                                 {$ifndef CanLib4Delphi32} far; {$endif}

implementation

const
  {$ifdef CanLib4Delphi32}
  OrdinalOffset = 0;
  DLL_Name      = 'VCanD32.DLL'; { '.DLL' seems to be NOT added by NT when not searching in the }
  {$else}                        { actual directory.  ===                                       }
  OrdinalOffset = 1;
  DLL_Name      = 'VCAND16';
  {$endif}

(*******************************************************************
ncdOpenDriver():

The Application calls this function to get access to the driver.
*)
//function ncdOpenDriver : Vstatus; external DLL_Name index 1+OrdinalOffset;
function ncdOpenDriver : Vstatus; external DLL_Name name 'ncdOpenDriver';


(********************************************************************
ncdGetDriverConfig():

The application gets the information, which
CAN channels are available in the system. The user
must provide the memory, pChanCount is pointing to
and the size of pDriverConfig. Passing NULL
for pDriverConfig only the ChanCount is given back.

Attention: We use a pointer instead of a var for the second parameter
since we need the possibility to pass a nil pointer here.
*)
function ncdGetDriverConfig
  (
    var pChanCount : LongInt;
    pDriverConfig  : PVDriverConfig
  ) : Vstatus; external DLL_Name name 'ncdGetDriverConfig';


(********************************************************************
ncdGetChannelIndex():
ncdGetChannelMask():

Get the channel index for a channel of a certain hardware.
Parameter -1 means "don't care"
Result -1 (ncdGetChannelIndex) or 0 (ncdGetChannelMask) means "not found"
*)
function ncdGetChannelIndex
  (
    hwType    : LongInt;    { [-1,HWTYPE_CANCARDX,HWTYPE_VIRTUAL,...] }
    hwIndex   : LongInt;    { [-1,0,1]                                }
    hwChannel : LongInt     { [-1,0,1]                                }
  ) : LongInt; external DLL_Name name 'ncdGetChannelIndex';

function ncdGetChannelMask
  (
    hwType    : LongInt;    { [-1,HWTYPE_CANCARDX,HWTYPE_VIRTUAL,...] }
    hwIndex   : LongInt;    { [-1,0,1]                                }
    hwChannel : LongInt     { [-1,0,1]                                }
  ) : Vaccess; external DLL_Name name 'ncdGetChannelMask';



(********************************************************************
ncdOpenPort():

The application tells the driver to which channels
it wants to get access to and which of these channels
it wants to get the permission to initialize the channel.
Only one port can get the permission to initialize a channel.
The permitted init access is returned.
*)
function ncdOpenPort
  (
    var portHandle     : VportHandle;
    userName           : PChar;
    accessMask         : Vaccess;
    initMask           : Vaccess;
    var permissionMask : Vaccess;
    rxQueueSize        : LongInt
  ) : Vstatus; external DLL_Name name 'ncdOpenPort';


(********************************************************************
ncdSetChannelMode():

For the CAN channels defined by AccessMask is set
whether the caller will get a TX and/or a TXRQ
receipt for transmitted messages.
The port must have init access to the channels.
*)
function ncdSetChannelMode
  (
    portHandle : VportHandle;
    accessMask : Vaccess;
    tx, txrq   : LongInt
  ) : Vstatus; external DLL_Name name 'ncdSetChannelMode';


(********************************************************************
ncdSetChannelOutput():

The output mode for the CAN chips of the channels defined by accessMask, is set
to OUTPUT_MODE_NORMAL or OUTPUT_MODE_SILENT.
The port must have init access to the channels.
*)
function ncdSetChannelOutput
  (
    portHandle : VportHandle;
    accessMask : Vaccess;
    mode       : LongInt
  ) : Vstatus; external DLL_Name name 'ncdSetChannelOutput';

(********************************************************************
ncdSetChannelTransceiver():

The transceiver mode for the channels defined by accessMask, is set.
The port must have init access to the channels.
*)
function ncdSetChannelTransceiver
  (
    portHandle : VportHandle;
    accessMask : Vaccess;
    typ        : LongInt;
    lineMode   : LongInt;
    resNet     : LongInt
  ) : Vstatus; external DLL_Name name 'ncdSetChannelTransceiver';

(********************************************************************
ncdSetChannelParams():
ncdSetChannelParamsC200():
ncdSetChannelBitrate():

The channels defined by accessMask will be initialized with the
given parameters.
The port must have init access to the channels.
*)
function ncdSetChannelParams
  (
    portHandle      : VportHandle;
    accessMask      : Vaccess;
    var pChipParams : VchipParams
  ) : Vstatus; external DLL_Name name 'ncdSetChannelParams';

function ncdSetChannelParamsC200
  (
    portHandle : VportHandle;
    accessMask : Vaccess;
    btr0, btr1 : Byte
  ) : Vstatus; external DLL_Name name 'ncdSetChannelParamsC200';


function ncdSetChannelBitrate
  (
    portHandle : VportHandle;
    accessMask : Vaccess;
    bitrate    : LongInt      { unsigned long doesn't exist! }
  ) : Vstatus; external DLL_Name name 'ncdSetChannelBitrate';


(********************************************************************
ncdSetAcceptance():

Set the acceptance filter
Filters for std and ext ids are handled independant in the driver.
Use mask=0xFFFF,code=0xFFFF or mask=0xFFFFFFFF,code=0xFFFFFFFF to fully close
the filter.
*)
function ncdSetChannelAcceptance
  (
    portHandle : VportHandle;
    accessMask : Vaccess;
    var filter : VsetAcceptance
  ) : Vstatus; external DLL_Name name 'ncdSetChannelAcceptance';


(********************************************************************
ncdSetTimerRate():

The timer of the port will be activated/deacticated and the
rate for cyclic timer events is set (10us resolution).
*)
function ncdSetTimerRate
  (
    portHandle : VportHandle;
    timerRate  : LongInt      { unsigned long gibt's hier leider nicht! }
  ) : Vstatus; external DLL_Name name 'ncdSetTimerRate';


(********************************************************************
ncdResetClock():

The clock generating timestamps for the port will be reset

*)
function ncdResetClock
  (
    portHandle : VportHandle
  ) : Vstatus; external DLL_Name name 'ncdResetClock';


(********************************************************************
ncdSetNotification():

Setup a event to notify the application if there are messages in the
ports receive queue.
queueLevel specifies the number of messages that triggeres the event.
Note that the event is triggered only once, when the queueLevel is
reached. An application should read all available messages by ncdReceive
to be sure to reenable the event.

Attention: This function doesn't work in 16 bit environments so we
disable it here!
*)
{$ifdef CanLib4Delphi32}
function ncdSetNotification
  (
    portHandle : VportHandle;
    var handle : LongInt; { unsigned long gibt's hier leider nicht! }
    queueLevel : LongInt
  ) : Vstatus; external DLL_Name name 'ncdSetNotification';
{$endif}

(********************************************************************
ncdTransmit():

A CAN message will be put to the designated channels to be transmitted.

*)
function ncdTransmit
  (
    portHandle : VportHandle;
    accessMask : Vaccess;
    var pEvent : Vevent
  ) : Vstatus; external DLL_Name name 'ncdTransmit';


(********************************************************************
ncdReceive1():

The driver is asked to retrieve a single Events from the
application's receive queue. This function is optimized
for speed. It returns a pointer to the received event.
Lifetime of the data is until the next call of ncdReceive1.
It returns VERR_QUEUE_IS_EMPTY and *ppEvent=NULL if no event
is available.

*)
function ncdReceive1
  (
    portHandle  : VportHandle;
    var ppEvent : PVevent
  ) : Vstatus; external DLL_Name name 'ncdReceive1';


(********************************************************************
ncdReceive():

The driver is asked to retrieve Events from the
application's receive queue. You can choose wether
it will be polled or waited for an incoming event.
Its possible to read multiple events at a time, but the
caller must provide the memory. In pEventCount the actual
number of received events will be returned.

Attention: This function doesn't work in 16 bit environments so we
disable it here!
*)
{$ifdef CanLib4Delphi32}
function ncdReceive
  (
    portHandle      : VportHandle;
    receiveMode     : LongInt;
    waitHandle      : Word;
    var pEventCount : LongInt;
    var pEventList  : Vevent
  ) : Vstatus; external DLL_Name name 'ncdReceive';
{$endif}


(********************************************************************
ncdRequestChipState():

The state of the selected channels is requested.
The answer will be received as an event (V_CHIP_STATE).
*)
function ncdRequestChipState
  (
    portHandle : VportHandle;
    accessMask : Vaccess
  ) : Vstatus; external DLL_Name name 'ncdRequestChipState';


(********************************************************************
ncdFlushTransmitQueue():

The transmit queue of the selected channel will be flushed.
*)
function ncdFlushTransmitQueue
  (
    portHandle : VportHandle;
    accessMask : Vaccess
  ) : Vstatus; external DLL_Name name 'ncdFlushTransmitQueue';


(********************************************************************
ncdFlushReceiveQueue():

The receive queue of the port will be flushed.
*)
function ncdFlushReceiveQueue
  (
    portHandle : VportHandle
  ) : Vstatus; external DLL_Name name 'ncdFlushReceiveQueue';


(********************************************************************
ncdGetReceiveQueueLevel():

The count of event´s in the receive queue of the port will be returned.
*)
function ncdGetReceiveQueueLevel
  (
    portHandle : VportHandle;
    var level  : LongInt
  ) : Vstatus; external DLL_Name name 'ncdGetReceiveQueueLevel';


(********************************************************************
ncdGetState():

The state of the port is returned.
*)
function ncdGetState
  (
    portHandle : VportHandle
  ) : Vstatus; external DLL_Name name 'ncdGetState';


(********************************************************************
ncdActivateChannel():

The selected channels go 'on the bus'.
*)
function ncdActivateChannel
  (
    portHandle : VportHandle;
    accessMask : Vaccess
  ) : Vstatus; external DLL_Name name 'ncdActivateChannel';


(********************************************************************
DeactivateChannel():

The selected channels go 'off the bus'.
Its now possible to initialize

*)
function ncdDeactivateChannel
  (
    portHandle : VportHandle;
    accessMask : Vaccess
  ) : Vstatus; external DLL_Name name 'ncdDeactivateChannel';


(********************************************************************
ncdClosePort():

The port is closed, channels are deactivated.

*)
function ncdClosePort
  (
    portHandle : VportHandle
  ) : Vstatus; external DLL_Name name 'ncdClosePort';


(********************************************************************
ncdCloseDriver ():

The driver is closed.
This is used to unload the driver, if no more application is useing it.
Does not close the open ports !!!
*)
function ncdCloseDriver : VStatus; external DLL_Name name 'ncdCloseDriver';


(********************************************************************

ncdGetErrorString()
ncdGetEventString()

Utility Functions
*)

function ncdGetErrorString
  (
    err : Vstatus
  ) : ncdStringType; external DLL_Name name 'ncdGetErrorString';
function ncdGetEventString
  (
    var ev : Vevent
  ) : ncdStringType; external DLL_Name name 'ncdGetEventString';


(********************************************************************
ncdGetApplConfig ():

Returns the hwIndex, hwChannel and hwType for a specific Application and application channel.
This gives the ability to register own applications into the Vector
CAN driver configuration.
AppName: Zero terminated string containing the Name of the Application.
AppChannel: Channel of the application
hwType, hwIndex, hwChannel: contains the the hardware information on success.
This values can be used in a subsequent call to ncdGetChannelMask or ncdGetChannelIndex.
*)
function ncdGetApplConfig
  (
    appName        : PChar;
    appChannel     : LongInt;
    var hwType     : LongInt;
    var hwIndex    : LongInt;
    var hwChannel  : LongInt
  ) : Vstatus; external DLL_Name name 'ncdGetApplConfig';

(********************************************************************
ncdSetApplConfig ():

Correspondig to ncdGetApplConfig this function sets the Application
Configuration, but it can also done in the CAN Configuration.
If the Applicationname does not exist a new is generated.
*)
function ncdSetApplConfig
  (
    appName     : PChar;
    appChannel  : LongInt;
    hwType      : LongInt;
    hwIndex     : LongInt;
    hwChannel   : LongInt
  ) : Vstatus; external DLL_Name name 'ncdSetApplConfig';

(********************************************************************
ncdGetChannelVersion ():

Get Version Information if available *)
function ncdGetChannelVersion
  (
    ChannelIndex     : LongInt;
    var FwVersion    : LongInt;
    var HwVersion    : LongInt;
    var SerialNumber : LongInt
  ) : Vstatus; external DLL_Name name 'ncdGetChannelVersion';

(********************************************************************
ncdSetReceiveMode ():

Suppress Error Frames and ChipState Events *)
function ncdSetReceiveMode
  (
    Port          : VportHandle;
    ErrorFrame    : Byte;
    ChipState     : Byte
  ) : Vstatus; external DLL_Name name 'ncdSetReceiveMode';

(********************************************************************
ncdAddAcceptanceRange ():

Opens the Acceptance filter for a range of IDs *)
function ncdAddAcceptanceRange
  (
    Port          : VportHandle;
    accessMask    : Vaccess;
    first_id      : LongInt;
    last_id       : LongInt
  ) : Vstatus; external DLL_Name name 'ncdAddAcceptanceRange';

(********************************************************************
ncdRemoveAcceptanceRange ():

Closes the Acceptance filter for a range of IDs *)
function ncdRemoveAcceptanceRange
  (
    Port          : VportHandle;
    accessMask    : Vaccess;
    first_id      : LongInt;
    last_id       : LongInt
  ) : Vstatus; external DLL_Name name 'ncdRemoveAcceptanceRange';

(********************************************************************
ncdResetAcceptance ():

Resets the Acceptance filter. Filter is closed. *)
function ncdResetAcceptance
  (
    Port          : VportHandle;
    accessMask    : Vaccess;
    extended      : LongInt
  ) : Vstatus; external DLL_Name name 'ncdResetAcceptance';


(********************************************************************
This function had been defined as macros originally (C header)
*)

function CHANNEL_MASK
  (
    val : Byte
  ) : LongInt;
begin
  CHANNEL_MASK := 1 shl val;
end;

(********************************************************************
SIZEOF_VDRIVERCONFIG() returns the amount of memory needed for the
VDriverConfig record with n channels. Use this function to allocate
memory for the data ncdDriverConfig returns.
*)
function SIZEOF_VDRIVERCONFIG
  (
    n : Byte
  ) : LongInt;
begin
  SIZEOF_VDRIVERCONFIG := sizeof(VDriverConfig)+(n-1)*sizeof(VChannelConfig);
end;

{$ifdef CanLib4Delphi32}
{$undef CanLib4Delphi32}
{$endif}
end.
