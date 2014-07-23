///////////////////////////////////////////////////////////////////////////////
//	PCAN-Light
//  PCAN_USB.pas
//
//  Version 2.x
//
//  ~~~~~~~~~~
//
//  Definition of the PCAN-Light API. 
//	The Driver support a Hardware and a Software who want to communicate with CAN-busses 
//
//  ~~~~~~~~~~~~
//
//  PCAN-Light-API
//
//  ~~~~~~~~~~~~
//
//	- CAN_Init(wBTR0BTR1: Word; CANMsgType: Integer)
//  - CAN_Close()
//  - CAN_Status()
//  - CAN_Write(var MsgBuff: TPCANMsg)
//  - CAN_Read(var MsgBuff: TPCANMsg)
//  - CAN_ReadEx(var MsgBuff: TPCANMsg; var RcvTime: TPCANTimestamp)
//  - CAN_VersionInfo(lpszTextBuff: PChar)
//  - CAN_DLLVersionInfo(lpszTextBuff: PChar)
//  - CAN_SpecialFunktion(distributorcode: LongWord; codenumber: Integer)
//  - CAN_ResetClient()
//  - CAN_MsgFilter(FromID, ToID: LongWord; _Type: Integer)
//  - CAN_ResetFilter()
//	- SetUSBDeviceNr(DevNum: Integer)
//	- GetUSBDeviceNr(var DevNum: Integer)
//  - CAN_SetRcvFunc(hEvent: THandle)
//
//  ------------------------------------------------------------------
//  Author : Hoppe, Wilhelm
//  Modified By: Wagner (29.09.2009)
//
//  Language: PASCAL OO
//  ------------------------------------------------------------------
//
//  Copyright (C) 1999-2009  PEAK-System Technik GmbH, Darmstadt
//
unit pcan_usb;

interface 

const
    // Constants definitions - Frame Type
    //
    CAN_INIT_TYPE_EX = $01;	//Extended Frame
    CAN_INIT_TYPE_ST = $00;	//Standart Frame

    // Constants definitions - ID
    //
    CAN_MAX_STANDARD_ID = $7ff;
    CAN_MAX_EXTENDED_ID = $1fffffff;

    // Constants definitions  - CAN message types
    //
    MSGTYPE_STANDARD = $00;  // Standard Data frame (11-bit ID)
    MSGTYPE_RTR      = $01;  // 1, if Remote Request frame
    MSGTYPE_EXTENDED = $02;  // 1, if Extended Data frame (CAN 2.0B, 29-bit ID)
    MSGTYPE_ERROR    = $80;  // 1, if Status information

    // Baud rate codes = BTR0/BTR1 register values for the CAN controller.
    // You can define your own Baudrate with the BTROBTR1 register !!
    // take a look at www.peak-system.com for our software BAUDTOOL to
    // calculate the BTROBTR1 register for every baudrate and sample point.
    //
    CAN_BAUD_1M   =  $0014;       //   1 MBit/s
    CAN_BAUD_500K =  $001C;       // 500 kBit/s
    CAN_BAUD_250K =  $011C;       // 250 kBit/s
    CAN_BAUD_125K =  $031C;       // 125 kBit/s
    CAN_BAUD_100K =  $432F;       // 100 kBit/s
    CAN_BAUD_50K  =  $472F;       //  50 kBit/s
    CAN_BAUD_20K  =  $532F;       //  20 kBit/s
    CAN_BAUD_10K  =  $672F;       //  10 kBit/s
    CAN_BAUD_5K   =  $7F7F;       //   5 kBit/s

    // Error codes (bit code)
    //
    CAN_ERR_OK         = $0000;	// No error
    CAN_ERR_XMTFULL    = $0001;   // Transmit buffer in CAN controller is full
    CAN_ERR_OVERRUN    = $0002;   // CAN controller was read too late
    CAN_ERR_BUSLIGHT   = $0004;   // Bus error: an error counter reached the 'light' limit
    CAN_ERR_BUSHEAVY   = $0008;   // Bus error: an error counter reached the 'heavy' limit  
    CAN_ERR_BUSOFF     = $0010;   // Bus error: the CAN controller is in bus-off state
    CAN_ERR_QRCVEMPTY  = $0020;   // Receive queue is empty
    CAN_ERR_QOVERRUN   = $0040;   // Receive queue was read too late
    CAN_ERR_QXMTFULL   = $0080;   // Transmit queue ist full
    CAN_ERR_REGTEST    = $0100;   // Test of the CAN controller hardware registers failed (no hardware found)
    CAN_ERR_NOVXD      = $0200;   // Driver not loaded
    CAN_ERR_NODRIVER   = $0200;   // Driver not loaded
    CAN_ERRMASK_ILLHANDLE=$1C00;  // Mask for all handle errors
    CAN_ERR_HWINUSE    = $0400;   // Hardware already in use by a Net
    CAN_ERR_NETINUSE   = $0800;   // a Client is already connected to the Net
    CAN_ERR_ILLHW      = $1400;   // Hardware handle is invalid
    CAN_ERR_ILLNET     = $1800;   // Net handle is invalid
    CAN_ERR_ILLCLIENT  = $1C00;   // Client handle is invalid
    CAN_ERR_RESOURCE   = $2000;   // Resource (FIFO, Client, timeout) cannot be created
    CAN_ERR_ILLPARAMTYPE = $4000; // Invalid parameter
    CAN_ERR_ILLPARAMVAL  = $8000; // Invalid parameter value
    CAN_ERR_UNKNOWN    = $10000;  // Unknown error
    CAN_ERR_ANYBUSERR = (CAN_ERR_BUSLIGHT or CAN_ERR_BUSHEAVY or CAN_ERR_BUSOFF);


type
    // CAN Message
    //
    TPCANMsg = record
        ID: LongWord;               // 11/29 bit identifier
        MSGTYPE: Byte;              // Bits from MSGTYPE_*
        LEN: Byte;                  // Data Length Code of the Msg (0..8)
        DATA: array[0..7] of Byte;  // Data 0 .. 7
    end;

    // Timestamp of a receive/transmit event
    // Total microseconds = micros + 1000 * millis + 0xFFFFFFFF * 1000 * millis_overflow
    //    
    TPCANTimestamp = record
        millis: LongWord;           // Base-value: milliseconds: 0.. 2^32-1
        millis_overflow: Word;      // Roll-arounds of millis
        micros: Word;               // Microseconds: 0..999
    end;  

///////////////////////////////////////////////////////////////////////////////
//  CAN_Init()
//  This function make the following:
//		- Activate a Hardware
//		- Make a Register Test of 82C200/SJA1000
//		- Allocate a Send buffer and a Hardware handle
//		- Programs the configuration of the transmit/receive driver
//		- Set the Baudrate register
//		- Set the Controller in RESET condition	
//		
//  If CANMsgType=0  ---> ID 11Bit
//  If CANMsgType=1  ---> ID 11/29Bit 
//
//  Possible Errors: NOVXD ILLHW REGTEST RESOURCE
//
function CAN_Init(wBTR0BTR1: Word;
        CANMsgType: Integer): LongWord; stdcall;

///////////////////////////////////////////////////////////////////////////////
//  CAN_Close()
//  This function terminate and release the configured hardware and all 
//  allocated resources
//
//  Possible Errors: NOVXD
//
function CAN_Close: LongWord; stdcall;

///////////////////////////////////////////////////////////////////////////////
//  CAN_Status()
//  This function request the current status of the hardware (b.e. BUS-OFF)
//
//  Possible Errors: NOVXD BUSOFF BUSHEAVY OVERRUN
//
function CAN_Status: LongWord; stdcall;

///////////////////////////////////////////////////////////////////////////////
//  CAN_Write()
//  This function Place a CAN message into the Transmit Queue of the CAN Hardware
//
//  Possible Errors: NOVXD RESOURCE BUSOFF QXMTFULL
//
function CAN_Write(var MsgBuff: TPCANMsg): LongWord; stdcall;

///////////////////////////////////////////////////////////////////////////////
//  CAN_Read()
//  This function get the next message or the next error from the Receive Queue of 
//  the CAN Hardware.  
//  REMARK:
//		- Check always the type of the received Message (MSGTYPE_STANDARD,MSGTYPE_RTR,
//		  MSGTYPE_EXTENDED,MSGTYPE_STATUS)
//		- The function will return ERR_OK always that you receive a CAN message successfully 
//		  although if the messages is a MSGTYPE_STATUS message.  
//		- When a MSGTYPE_STATUS mesasge is got, the ID and Length information of the message 
//		  will be treated as indefined values. Actually information of the received message
//		  should be interpreted using the first 4 data bytes as follow:
//			*	Data0	Data1	Data2	Data3	Kind of Error
//				0x00	0x00	0x00	0x02	CAN_ERR_OVERRUN		0x0002	CAN Controller was read to late
//				0x00	0x00	0x00	0x04	CAN_ERR_BUSLIGHT	0x0004  Bus Error: An error counter limit reached (96)
//				0x00	0x00	0x00	0x08	CAN_ERR_BUSHEAVY	0x0008	Bus Error: An error counter limit reached (128)
//				0x00	0x00	0x00	0x10	CAN_ERR_BUSOFF		0x0010	Bus Error: Can Controller went "Bus-Off"
//		- If a CAN_ERR_BUSOFF status message is received, the CAN Controller must to be 
//		  initialized again using the Init() function.  Otherwise, will be not possible 
//		  to send/receive more messages. 
//		- The message will be written to 'msgbuff'.
//
//  Possible Errors: NOVXD  QRCVEMPTY
//
function CAN_Read(var MsgBuff: TPCANMsg): LongWord; stdcall;

///////////////////////////////////////////////////////////////////////////////
//  CAN_ReadEx()
//  This function get the next message or the next error from the Receive Queue of 
//  the CAN Hardware and the time when the message arrived.  
//  REMARK:
//		- Check always the type of the received Message (MSGTYPE_STANDARD,MSGTYPE_RTR,
//		  MSGTYPE_EXTENDED,MSGTYPE_STATUS)
//		- The function will return ERR_OK always that you receive a CAN message successfully 
//		  although if the messages is a MSGTYPE_STATUS message.  
//		- When a MSGTYPE_STATUS mesasge is got, the ID and Length information of the message 
//		  will be treated as indefined values. Actually information of the received message
//		  should be interpreted using the first 4 data bytes as follow:
//			*	Data0	Data1	Data2	Data3	Kind of Error
//				0x00	0x00	0x00	0x02	CAN_ERR_OVERRUN		0x0002	CAN Controller was read to late
//				0x00	0x00	0x00	0x04	CAN_ERR_BUSLIGHT	0x0004  Bus Error: An error counter limit reached (96)
//				0x00	0x00	0x00	0x08	CAN_ERR_BUSHEAVY	0x0008	Bus Error: An error counter limit reached (128)
//				0x00	0x00	0x00	0x10	CAN_ERR_BUSOFF		0x0010	Bus Error: Can Controller went "Bus-Off"
//		- If a CAN_ERR_BUSOFF status message is received, the CAN Controller must to be 
//		  initialized again using the Init() function.  Otherwise, will be not possible 
//		  to send/receive more messages. 
//		- The message will be written to 'msgbuff'.
//		Since Version 2.x the Ext. Version is available - new Parameter:
//		-  Receive timestamp
//
//  Possible Errors: NOVXD  QRCVEMPTY
//
function CAN_ReadEx(
        var MsgBuff: TPCANMsg; 
        var RcvTime: TPCANTimestamp
        ): LongWord; stdcall;
        
///////////////////////////////////////////////////////////////////////////////
//  CAN_VersionInfo()
//  This function get the Version and copyright of the hardware as text 
//  (max. 255 characters)
//
//  Possible Errors:  NOVXD
//
function CAN_VersionInfo(
		lpszTextBuff: PChar
		): LongWord; stdcall;

///////////////////////////////////////////////////////////////////////////////
//  CAN_DLLVersionInfo()
//  This function is used to get the Version and copyright of the DLL as 
//  text (max. 255 characters)
//
//  Possible Errors: -1 for NULL-Pointer parameters :-)
//
function CAN_DLLVersionInfo(
        lpszTextBuff: PChar
		): LongWord; stdcall;
        
///////////////////////////////////////////////////////////////////////////////
//  CAN_SpecialFunktion()
//  This function is an special function to be used "ONLY" for distributors
//  Return: 1 - the given parameters and the parameters in the hardware agree 
//		    0 - otherwise
//
//  Possible Errors:  NOVXD
//
function CAN_SpecialFunktion(
        distributorcode: LongWord;
        codenumber: Integer
        ): LongWord; stdcall;

//////////////////////////////////////////////////////////////////////////////
//  CAN_ResetClient()
//  This function delete the both queues (Transmit,Receive) of the CAN Controller 
//  using a RESET
//
//  Possible Errors: ERR_ILLCLIENT ERR_NOVXD
//
function CAN_ResetClient: LongWord; stdcall;

///////////////////////////////////////////////////////////////////////////////
//  CAN_MsgFilter(FromID, ToID, int Type)
//  This function set the receive message filter of the CAN Controller.
//  REMARK:
//		- A quick register of all messages is possible using the parameters FromID and ToID = 0
//		- Every call of this function maybe cause an extention of the receive filter of the 
//		  CAN controller, which one can go briefly to RESET
//		- New in Ver 2.x:
//			* Standard frames will be put it down in the acc_mask/code as Bits 28..13
//			* Hardware driver for 82C200 must to be moved to Bits 10..0 again!
//	WARNING: 
//		It is not guaranteed to receive ONLY the registered messages.
//
//  Possible Errors: NOVXD ILLCLIENT ILLNET REGTEST
//
function CAN_MsgFilter(FromID, ToID: LongWord; _Type: Integer): LongWord; stdcall;

///////////////////////////////////////////////////////////////////////////////
//  CAN_ResetFilter()
//  This function close completely the Message Filter of the Hardware.
//  They will be no more messages received.
//
//  Possible Errors: NOVXD
//
function CAN_ResetFilter: LongWord; stdcall;

//////////////////////////////////////////////////////////////////////////////
//  SetUSBDeviceNr()
//  This function set an identification number to the USB CAN hardware
//
//  Possible Errors: NOVXD ILLHW ILLPARAMTYPE ILLPARAMVAL REGTEST
//
function SetUSBDeviceNr(DevNum: LongWord): LongWord; stdcall;

//////////////////////////////////////////////////////////////////////////////
//  GetUSBDeviceNr()
//  This function read the device number of a USB CAN Hardware
//
//  Possible Errors: NOVXD ILLHW ILLPARAMTYPE
//
function GetUSBDeviceNr(var DevNum: LongWord): LongWord; stdcall;

///////////////////////////////////////////////////////////////////////////////
//  CAN_SetRcvEvent()
//  This function is used to set the Event for the Event Handler
//
//  Possible Errors: ILLCLIENT ILLPARAMTYPE ILLPARAMVAL NOVXD
//
function CAN_SetRcvEvent(hEvent: LongInt): LongWord; stdcall;


implementation

uses SysUtils;

const DLL_Name = 'PCAN_USB.dll';

function CAN_Init(wBTR0BTR1: Word; CANMsgType: Integer): LongWord; stdcall;
external DLL_Name;

function CAN_Close: LongWord; stdcall;
external DLL_Name;

function CAN_Status: LongWord; stdcall;
external DLL_Name;

function CAN_Write(var MsgBuff: TPCANMsg): LongWord; stdcall;
external DLL_Name;

function CAN_Read(var MsgBuff: TPCANMsg): LongWord; stdcall;
external DLL_Name;

function CAN_ReadEx(var MsgBuff: TPCANMsg; var RcvTime: TPCANTimestamp): LongWord; stdcall;
external DLL_NAME;

function CAN_VersionInfo(lpszTextBuff: PChar): LongWord; stdcall;
external DLL_Name;

function CAN_DLLVersionInfo(lpszTextBuff: PChar): LongWord; stdcall;
external DLL_Name;

function CAN_SpecialFunktion(distributorcode: LongWord; codenumber: Integer): LongWord; stdcall;
external DLL_Name;

function CAN_ResetClient: LongWord; stdcall;
external DLL_Name;

function CAN_MsgFilter(FromID, ToID: LongWord; _Type: Integer): LongWord; stdcall;
external DLL_Name;

function CAN_ResetFilter: LongWord; stdcall;
external DLL_Name;

function SetUSBDeviceNr(DevNum: LongWord): LongWord; stdcall;
external DLL_Name;

function GetUSBDeviceNr(var DevNum: LongWord): LongWord; stdcall;
external DLL_Name;

function CAN_SetRcvEvent(hEvent: LongInt):LongWord; stdcall;
external DLL_Name;

end.
