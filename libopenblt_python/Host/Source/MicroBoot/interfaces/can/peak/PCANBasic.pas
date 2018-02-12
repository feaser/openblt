//  PCANBasic.pas
//
//  ~~~~~~~~~~~~
//
//  PCAN-Basic API
//
//  ~~~~~~~~~~~~
//
//  ------------------------------------------------------------------
//  Author : Keneth Wagner
//	Last change: 18.05.2016 Wagner
//
//  Language: Pascal
//  ------------------------------------------------------------------
//
//  Copyright (C) 1999-2016  PEAK-System Technik GmbH, Darmstadt
//  more Info at http://www.peak-system.com
//
unit PCANBasic;

interface

const
    ////////////////////////////////////////////////////////////
    // Value definitions
    ////////////////////////////////////////////////////////////

    // Currently defined and supported PCAN channels
    //
    PCAN_NONEBUS              = $00;  // Undefined/default value for a PCAN bus

    PCAN_ISABUS1              = $21;  // PCAN-ISA interface, channel 1
    PCAN_ISABUS2              = $22;  // PCAN-ISA interface, channel 2
    PCAN_ISABUS3              = $23;  // PCAN-ISA interface, channel 3
    PCAN_ISABUS4              = $24;  // PCAN-ISA interface, channel 4
    PCAN_ISABUS5              = $25;  // PCAN-ISA interface, channel 5
    PCAN_ISABUS6              = $26;  // PCAN-ISA interface, channel 6
    PCAN_ISABUS7              = $27;  // PCAN-ISA interface, channel 7
    PCAN_ISABUS8              = $28;  // PCAN-ISA interface, channel 8

    PCAN_DNGBUS1              = $31;  // PPCAN-Dongle/LPT interface, channel 1

    PCAN_PCIBUS1              = $41;  // PCAN-PCI interface, channel 1
    PCAN_PCIBUS2              = $42;  // PCAN-PCI interface, channel 2
    PCAN_PCIBUS3              = $43;  // PCAN-PCI interface, channel 3
    PCAN_PCIBUS4              = $44;  // PCAN-PCI interface, channel 4
    PCAN_PCIBUS5              = $45;  // PCAN-PCI interface, channel 5
    PCAN_PCIBUS6              = $46;  // PCAN-PCI interface, channel 6
    PCAN_PCIBUS7              = $47;  // PCAN-PCI interface, channel 7
    PCAN_PCIBUS8              = $48;  // PCAN-PCI interface, channel 8
    PCAN_PCIBUS9              = $409;  // PCAN-PCI interface, channel 9
    PCAN_PCIBUS10             = $40A;  // PCAN-PCI interface, channel 10
    PCAN_PCIBUS11             = $40B;  // PCAN-PCI interface, channel 11
    PCAN_PCIBUS12             = $40C;  // PCAN-PCI interface, channel 12
    PCAN_PCIBUS13             = $40D;  // PCAN-PCI interface, channel 13
    PCAN_PCIBUS14             = $40E;  // PCAN-PCI interface, channel 14
    PCAN_PCIBUS15             = $40F;  // PCAN-PCI interface, channel 15
    PCAN_PCIBUS16             = $410;  // PCAN-PCI interface, channel 16

    PCAN_USBBUS1              = $51;  // PCAN-USB interface, channel 1
    PCAN_USBBUS2              = $52;  // PCAN-USB interface, channel 2
    PCAN_USBBUS3              = $53;  // PCAN-USB interface, channel 3
    PCAN_USBBUS4              = $54;  // PCAN-USB interface, channel 4
    PCAN_USBBUS5              = $55;  // PCAN-USB interface, channel 5
    PCAN_USBBUS6              = $56;  // PCAN-USB interface, channel 6
    PCAN_USBBUS7              = $57;  // PCAN-USB interface, channel 7
    PCAN_USBBUS8              = $58;  // PCAN-USB interface, channel 8
    PCAN_USBBUS9              = $509;  // PCAN-USB interface, channel 9
    PCAN_USBBUS10             = $50A;  // PCAN-USB interface, channel 10
    PCAN_USBBUS11             = $50B;  // PCAN-USB interface, channel 11
    PCAN_USBBUS12             = $50C;  // PCAN-USB interface, channel 12
    PCAN_USBBUS13             = $50D;  // PCAN-USB interface, channel 13
    PCAN_USBBUS14             = $50E;  // PCAN-USB interface, channel 14
    PCAN_USBBUS15             = $50F;  // PCAN-USB interface, channel 15
    PCAN_USBBUS16             = $510;  // PCAN-USB interface, channel 16

    PCAN_PCCBUS1              = $61;  // PCAN-PC Card interface, channel 1
    PCAN_PCCBUS2              = $62;  // PCAN-PC Card interface, channel 2

    PCAN_LANBUS1              = $801;  // PCAN-LAN interface, channel 1
    PCAN_LANBUS2              = $802;  // PCAN-LAN interface, channel 2
    PCAN_LANBUS3              = $803;  // PCAN-LAN interface, channel 3
    PCAN_LANBUS4              = $804;  // PCAN-LAN interface, channel 4
    PCAN_LANBUS5              = $805;  // PCAN-LAN interface, channel 5
    PCAN_LANBUS6              = $806;  // PCAN-LAN interface, channel 6
    PCAN_LANBUS7              = $807;  // PCAN-LAN interface, channel 7
    PCAN_LANBUS8              = $808;  // PCAN-LAN interface, channel 8
    PCAN_LANBUS9              = $809;  // PCAN-LAN interface, channel 9
    PCAN_LANBUS10             = $80A;  // PCAN-LAN interface, channel 10
    PCAN_LANBUS11             = $80B;  // PCAN-LAN interface, channel 11
    PCAN_LANBUS12             = $80C;  // PCAN-LAN interface, channel 12
    PCAN_LANBUS13             = $80D;  // PCAN-LAN interface, channel 13
    PCAN_LANBUS14             = $80E;  // PCAN-LAN interface, channel 14
    PCAN_LANBUS15             = $80F;  // PCAN-LAN interface, channel 15
    PCAN_LANBUS16             = $810;  // PCAN-LAN interface, channel 16

    // Represent the PCAN error and status codes
    //
    PCAN_ERROR_OK             = $00000;  // No error
    PCAN_ERROR_XMTFULL        = $00001;  // Transmit buffer in CAN controller is full
    PCAN_ERROR_OVERRUN        = $00002;  // CAN controller was read too late
    PCAN_ERROR_BUSLIGHT       = $00004;  // Bus error: an error counter reached the 'light' limit [Not used with the *FD functions]
    PCAN_ERROR_BUSHEAVY       = $00008;  // Bus error: an error counter reached the 'heavy' limit
    PCAN_ERROR_BUSWARNING     = PCAN_ERROR_BUSHEAVY;  // An error counter reached the 'warning' limit [ONLY used with the *FD functions]
    PCAN_ERROR_BUSPASSIVE     = $40000;  // Bus error: the CAN controller is in bus-off state
    PCAN_ERROR_BUSOFF         = $00010;  // Bus error: the CAN controller is in bus-off state
    PCAN_ERROR_ANYBUSERR      = PCAN_ERROR_BUSWARNING Or PCAN_ERROR_BUSLIGHT Or PCAN_ERROR_BUSHEAVY Or PCAN_ERROR_BUSOFF Or PCAN_ERROR_BUSPASSIVE; // Mask for all bus errors
    PCAN_ERROR_QRCVEMPTY      = $00020;  // Receive queue is empty
    PCAN_ERROR_QOVERRUN       = $00040;  // Receive queue was read too late
    PCAN_ERROR_QXMTFULL       = $00080;  // Transmit queue is full
    PCAN_ERROR_REGTEST        = $00100;  // Test of the CAN controller hardware registers failed (no hardware found)
    PCAN_ERROR_NODRIVER       = $00200;  // Driver not loaded
    PCAN_ERROR_HWINUSE        = $00400;  // Hardware already in use by a Net
    PCAN_ERROR_NETINUSE       = $00800;  // A Client is already connected to the Net
    PCAN_ERROR_ILLHW          = $01400;  // Hardware handle is invalid
    PCAN_ERROR_ILLNET         = $01800;  // Net handle is invalid
    PCAN_ERROR_ILLCLIENT      = $01C00;  // Client handle is invalid
    PCAN_ERROR_ILLHANDLE      = PCAN_ERROR_ILLHW Or PCAN_ERROR_ILLNET Or PCAN_ERROR_ILLCLIENT; // Mask for all handle errors
    PCAN_ERROR_RESOURCE       = $02000;  // Resource (FIFO, Client, timeout) cannot be created
    PCAN_ERROR_ILLPARAMTYPE   = $04000;  // Invalid parameter
    PCAN_ERROR_ILLPARAMVAL    = $08000;  // Invalid parameter value
    PCAN_ERROR_UNKNOWN        = $10000;  // Unknown error
    PCAN_ERROR_ILLDATA        = $20000;  // Invalid data, function, or action
    PCAN_ERROR_CAUTION        = $2000000;  // An operation was successfully carried out, however, irregularities were registered
    PCAN_ERROR_INITIALIZE     = $4000000;  // Channel is not initialized [Value was changed from 0x40000 to 0x4000000]
    PCAN_ERROR_ILLOPERATION   = $8000000;  // Invalid operation [Value was changed from 0x80000 to 0x8000000]

    // PCAN devices
    //
    PCAN_NONE                 = $00;  // Undefined, unknown or not selected PCAN device value
    PCAN_PEAKCAN              = $01;  // PCAN Non-Plug&Play devices. NOT USED WITHIN PCAN-Basic API
    PCAN_ISA                  = $02;  // PCAN-ISA, PCAN-PC/104, and PCAN-PC/104-Plus
    PCAN_DNG                  = $03;  // PCAN-Dongle
    PCAN_PCI                  = $04;  // PCAN-PCI, PCAN-cPCI, PCAN-miniPCI, and PCAN-PCI Express
    PCAN_USB                  = $05;  // PCAN-USB and PCAN-USB Pro
    PCAN_PCC                  = $06;  // PCAN-PC Card
    PCAN_VIRTUAL              = $07;  // PCAN Virtual hardware. NOT USED WITHIN PCAN-Basic API
    PCAN_LAN                  = $08;  // PCAN Gateway devices

    // PCAN parameters
    //
    PCAN_DEVICE_NUMBER        = $01;  // PCAN-USB device number parameter
    PCAN_5VOLTS_POWER         = $02;  // PCAN-PC Card 5-Volt power parameter
    PCAN_RECEIVE_EVENT        = $03;  // PCAN receive event handler parameter
    PCAN_MESSAGE_FILTER       = $04;  // PCAN message filter parameter
    PCAN_API_VERSION          = $05;  // PCAN-Basic API version parameter
    PCAN_CHANNEL_VERSION      = $06;  // PCAN device channel version parameter
    PCAN_BUSOFF_AUTORESET     = $07;  // PCAN Reset-On-Busoff parameter
    PCAN_LISTEN_ONLY          = $08;  // PCAN Listen-Only parameter
    PCAN_LOG_LOCATION         = $09;  // Directory path for log files
    PCAN_LOG_STATUS           = $0A;  // Debug-Log activation status
    PCAN_LOG_CONFIGURE        = $0B;  // Configuration of the debugged information (LOG_FUNCTION_***)
    PCAN_LOG_TEXT             = $0C;  // Custom insertion of text into the log file
    PCAN_CHANNEL_CONDITION    = $0D;  // Availability status of a PCAN-Channel
    PCAN_HARDWARE_NAME        = $0E;  // PCAN hardware name parameter
    PCAN_RECEIVE_STATUS       = $0F;  // Message reception status of a PCAN-Channel
    PCAN_CONTROLLER_NUMBER    = $10;  // CAN-Controller number of a PCAN-Channel
    PCAN_TRACE_LOCATION       = $11;  // Directory path for PCAN trace files
    PCAN_TRACE_STATUS         = $12;  // CAN tracing activation status
    PCAN_TRACE_SIZE           = $13;  // Configuration of the maximum file size of a CAN trace
    PCAN_TRACE_CONFIGURE      = $14;  // Configuration of the trace file storing mode (TRACE_FILE_***)
    PCAN_CHANNEL_IDENTIFYING  = $15;  // Physical identification of a USB based PCAN-Channel by blinking its associated LED
    PCAN_CHANNEL_FEATURES     = $16;  // Capabilities of a PCAN device (FEATURE_***)
    PCAN_BITRATE_ADAPTING     = $17;  // Using of an existing bit rate (PCAN-View connected to a channel)
    PCAN_BITRATE_INFO         = $18;  // Configured bit rate as Btr0Btr1 value
    PCAN_BITRATE_INFO_FD      = $19;  // Configured bit rate as TPCANBitrateFD string
    PCAN_BUSSPEED_NOMINAL     = $1A;  // Configured nominal CAN Bus speed as Bits per seconds
    PCAN_BUSSPEED_DATA        = $1B;  // Configured CAN data speed as Bits per seconds
    PCAN_IP_ADDRESS           = $1C;  // Remote address of a LAN channel as string in IPv4 format
    PCAN_LAN_SERVICE_STATUS   = $1D;  // Status of the Virtual PCAN-Gateway Service 

    // PCAN parameter values
    //
    PCAN_PARAMETER_OFF        = $00;  // The PCAN parameter is not set (inactive)
    PCAN_PARAMETER_ON         = $01;  // The PCAN parameter is set (active)
    PCAN_FILTER_CLOSE         = $00;  // The PCAN filter is closed. No messages will be received
    PCAN_FILTER_OPEN          = $01;  // The PCAN filter is fully opened. All messages will be received
    PCAN_FILTER_CUSTOM        = $02;  // The PCAN filter is custom configured. Only registered
    PCAN_CHANNEL_UNAVAILABLE  = $00;  // The PCAN-Channel handle is illegal, or its associated hardware is not available
    PCAN_CHANNEL_AVAILABLE    = $01;  // The PCAN-Channel handle is available to be connected (Plug&Play Hardware: it means furthermore that the hardware is plugged-in)
    PCAN_CHANNEL_OCCUPIED     = $02;  // The PCAN-Channel handle is valid, and is already being used
    PCAN_CHANNEL_PCANVIEW     = PCAN_CHANNEL_AVAILABLE Or PCAN_CHANNEL_OCCUPIED;  // The PCAN-Channel handle is already being used by a PCAN-View application, but is available to connect

    LOG_FUNCTION_DEFAULT      = $00;  // Logs system exceptions / errors
    LOG_FUNCTION_ENTRY        = $01;  // Logs the entries to the PCAN-Basic API functions
    LOG_FUNCTION_PARAMETERS   = $02;  // Logs the parameters passed to the PCAN-Basic API functions
    LOG_FUNCTION_LEAVE        = $04;  // Logs the exits from the PCAN-Basic API functions
    LOG_FUNCTION_WRITE        = $08;  // Logs the CAN messages passed to the CAN_Write function
    LOG_FUNCTION_READ         = $10;  // Logs the CAN messages received within the CAN_Read function
    LOG_FUNCTION_ALL          = $FFFF;// Logs all possible information within the PCAN-Basic API functions

    TRACE_FILE_SINGLE         = $00;  // A single file is written until it size reaches PAN_TRACE_SIZE
    TRACE_FILE_SEGMENTED      = $01;  // Traced data is distributed in several files with size PAN_TRACE_SIZE
    TRACE_FILE_DATE           = $02;  // Includes the date into the name of the trace file
    TRACE_FILE_TIME           = $04;  // Includes the start time into the name of the trace file
    TRACE_FILE_OVERWRITE      = $80;  // Causes the overwriting of available traces (same name)

   	FEATURE_FD_CAPABLE        = $01;  // Device supports flexible data-rate (CAN-FD)	

    SERVICE_STATUS_STOPPED    = $01;  // The service is not running
  	SERVICE_STATUS_RUNNING    = $04;  // The service is running

    // PCAN message types
    //
    PCAN_MESSAGE_STANDARD     = $00;  // The PCAN message is a CAN Standard Frame (11-bit identifier)
    PCAN_MESSAGE_RTR          = $01;  // The PCAN message is a CAN Remote-Transfer-Request Frame
    PCAN_MESSAGE_EXTENDED     = $02;  // The PCAN message is a CAN Extended Frame (29-bit identifier)
    PCAN_MESSAGE_FD           = $04;  // The PCAN message represents a FD frame in terms of CiA Specs
    PCAN_MESSAGE_BRS          = $08;  // The PCAN message represents a FD bit rate switch (CAN data at a higher bit rate)
    PCAN_MESSAGE_ESI          = $10;  // The PCAN message represents a FD error state indicator(CAN FD transmitter was error active)
    PCAN_MESSAGE_STATUS       = $80;  // The PCAN message represents a PCAN status message

    // Frame Type / Initialization Mode
    //
    PCAN_MODE_STANDARD        = PCAN_MESSAGE_STANDARD;  // Mode is Standard (11-bit identifier)
    PCAN_MODE_EXTENDED        = PCAN_MESSAGE_EXTENDED;  // Mode is Extended (29-bit identifier)


    // Baud rate codes = BTR0/BTR1 register values for the CAN controller.
    // You can define your own Baud rate with the BTROBTR1 register.
    // Take a look at www.peak-system.com for our free software "BAUDTOOL"
    // to calculate the BTROBTR1 register for every bit rate and sample point.
    //
    PCAN_BAUD_1M              = $0014;  // 1 MBit/s
    PCAN_BAUD_800K            = $0016;  // 800 kBit/s
    PCAN_BAUD_500K            = $001C;  // 500 kBit/s
    PCAN_BAUD_250K            = $011C;  // 250 kBit/s
    PCAN_BAUD_125K            = $031C;  // 125 kBit/s
    PCAN_BAUD_100K            = $432F;  // 100 kBit/s
    PCAN_BAUD_95K             = $C34E;  // 95,238 kBit/s
    PCAN_BAUD_83K             = $852B;  // 83,333 kBit/s
    PCAN_BAUD_50K             = $472F;  // 50 kBit/s
    PCAN_BAUD_47K             = $1414;  // 47,619 kBit/s
    PCAN_BAUD_33K             = $8B2F;  // 33,333 kBit/s
    PCAN_BAUD_20K             = $532F;  // 20 kBit/s
    PCAN_BAUD_10K             = $672F;  // 10 kBit/s
    PCAN_BAUD_5K              = $7F7F;  // 5 kBit/s

    // Represents the configuration for a CAN bit rate
    // Note:
    //    * Each parameter and its value must be separated with a '='.
    //    * Each pair of parameter/value must be separated using ','.
    //
    // Example:
    //    f_clock=80000000,nom_brp=0,nom_tseg1=13,nom_tseg2=0,nom_sjw=0,data_brp=0,data_tseg1=13,data_tseg2=0,data_sjw=0
    //
    PCAN_BR_CLOCK            = 'f_clock';
    PCAN_BR_CLOCK_MHZ        = 'f_clock_mhz';
    PCAN_BR_NOM_BRP          = 'nom_brp';
    PCAN_BR_NOM_TSEG1        = 'nom_tseg1';
    PCAN_BR_NOM_TSEG2        = 'nom_tseg2';
    PCAN_BR_NOM_SJW          = 'nom_sjw';
    PCAN_BR_NOM_SAMPLE       = 'nom_sam';
    PCAN_BR_DATA_BRP         = 'data_brp';
    PCAN_BR_DATA_TSEG1       = 'data_tseg1';
    PCAN_BR_DATA_TSEG2       = 'data_tseg2';
    PCAN_BR_DATA_SJW         = 'data_sjw';
    PCAN_BR_DATA_SAMPLE      = 'data_ssp_offset';

    // Type of PCAN (non plug&play) hardware
    //
    PCAN_TYPE_ISA             = $01;  // PCAN-ISA 82C200
    PCAN_TYPE_ISA_SJA         = $09;  // PCAN-ISA SJA1000
    PCAN_TYPE_ISA_PHYTEC      = $04;  // PHYTEC ISA
    PCAN_TYPE_DNG             = $02;  // PCAN-Dongle 82C200
    PCAN_TYPE_DNG_EPP         = $03;  // PCAN-Dongle EPP 82C200
    PCAN_TYPE_DNG_SJA         = $05;  // PCAN-Dongle SJA1000
    PCAN_TYPE_DNG_SJA_EPP     = $06;  // PCAN-Dongle EPP SJA1000

type
    ////////////////////////////////////////////////////////////
    // Type definitions
    ////////////////////////////////////////////////////////////

    TPCANHandle = Word;        // Represents a PCAN hardware channel handle
    TPCANStatus = Longword;    // Represents a PCAN status/error code
    TPCANParameter = Byte;     // Represents a PCAN parameter to be read or set
    TPCANDevice = Byte;        // Represents a PCAN device
    TPCANMessageType = Byte;   // Represents the type of a PCAN message
    TPCANType = Byte;          // Represents the type of PCAN hardware to be initialized
    TPCANMode = Byte;          // Represents a PCAN filter mode
    TPCANBaudrate = Word;      // Represents a PCAN Baud rate register value
   	TPCANBitrateFD = PAnsiChar;// Represents a PCAN-FD bit rate string
   	TPCANTimestampFD = UInt64; // Represents a timestamp of a received PCAN FD message

    ////////////////////////////////////////////////////////////
    // Structure definitions
    ////////////////////////////////////////////////////////////

    // Represents a PCAN message
    //
    TPCANMsg = record
        ID: Longword;              // 11/29-bit message identifier
        MSGTYPE: TPCANMessageType; // Type of the message
        LEN: Byte;                 // Data Length Code of the message (0..8)
        DATA: array[0..7] of Byte; // Data of the message (DATA[0]..DATA[7])
    end;

    // Represents a timestamp of a received PCAN message.
    // Total Microseconds = micros + 1000 * millis + 0x100000000 * 1000 * millis_overflow
    //
    TPCANTimestamp = record
        millis: Longword;          // Base-value: milliseconds: 0.. 2^32-1
        millis_overflow: Word;     // Roll-arounds of millis
        micros: Word;              // Microseconds: 0..999
    end;
    PTPCANTimestamp = ^TPCANTimestamp;

    // Represents a PCAN message from a FD capable hardware
    //
    TPCANMsgFD = record
        ID: Longword;              // 11/29-bit message identifier
        MSGTYPE: TPCANMessageType; // Type of the message
        DLC: Byte;                 // Data Length Code of the message (0..15)
        DATA: array[0..63] of Byte; // Data of the message (DATA[0]..DATA[63])
    end;
	  PTPCANTimestampFD = ^TPCANTimestampFD;

////////////////////////////////////////////////////////////
// PCAN-Basic API function declarations
////////////////////////////////////////////////////////////

/// <summary>
/// Initializes a PCAN Channel
/// </summary>
/// <param name="Channel">The handle of a PCAN Channel</param>
/// <param name="Btr0Btr1">The speed for the communication (BTR0BTR1 code)</param>
/// <param name="HwType">NON PLUG&PLAY: The type of hardware and operation mode</param>
/// <param name="IOPort">NON PLUG&PLAY: The I/O address for the parallel port</param>
/// <param name="Interrupt">NON PLUG&PLAY: Interrupt number of the parallel port</param>
/// <returns>A TPCANStatus error code</returns>
function CAN_Initialize(
    Channel: TPCANHandle;
    Btr0Btr1: TPCANBaudrate;
    HwType: TPCANType;
    IOPort: LongWord;
    Interrupt: Word
    ): TPCANStatus; stdcall;

/// <summary>
/// Initializes a FD capable PCAN Channel  
/// </summary>
/// <param name="Channel">"The handle of a FD capable PCAN Channel"</param>
/// <param name="BitrateFD">"The speed for the communication (FD bit rate string)"</param>
/// <remarks>See PCAN_BR_* values
/// * parameter and values ust be separated by '='
/// * Couples of Parameter/value must be separated by ','
/// * Following Parameter must be filled out: f_clock, data_brp, data_sjw, data_tseg1, data_tseg2,
///   nom_brp, nom_sjw, nom_tseg1, nom_tseg2.
/// * Following Parameters are optional (not used yet): data_ssp_offset, nom_samp
///</remarks>
/// <example>f_clock_mhz=80,nom_brp=0,nom_tseg1=13,nom_tseg2=0,nom_sjw=0,data_brp=0,
/// data_tseg1=13,data_tseg2=0,data_sjw=0</example>
/// <returns>"A TPCANStatus error code"</returns>
function CAN_InitializeFD(
    Channel: TPCANHandle;
  	BitrateFD: TPCANBitrateFD
	  ): TPCANStatus; stdcall;

/// <summary>
/// Uninitializes one or all PCAN Channels initialized by CAN_Initialize
/// </summary>
/// <remarks>Giving the TPCANHandle value "PCAN_NONEBUS",
/// uninitialize all initialized channels</remarks>
/// <param name="Channel">The handle of a PCAN Channel</param>
/// <returns>A TPCANStatus error code</returns>
function CAN_Uninitialize(
    Channel: TPCANHandle
    ): TPCANStatus; stdcall;

/// <summary>
/// Resets the receive and transmit queues of the PCAN Channel
/// </summary>
/// <remarks>A reset of the CAN controller is not performed</remarks>
/// <param name="Channel">The handle of a PCAN Channel</param>
/// <returns>A TPCANStatus error code</returns>
function CAN_Reset(
    Channel: TPCANHandle
    ): TPCANStatus; stdcall;

/// <summary>
/// Gets the current status of a PCAN Channel
/// </summary>
/// <param name="Channel">The handle of a PCAN Channel</param>
/// <returns>A TPCANStatus error code</returns>
function CAN_GetStatus(
    Channel: TPCANHandle
    ): TPCANStatus; stdcall;

/// <summary>
/// Reads a CAN message from the receive queue of a PCAN Channel
/// </summary>
/// <param name="Channel">The handle of a PCAN Channel</param>
/// <param name="MessageBuffer">A TPCANMsg structure buffer to store the CAN message</param>
/// <param name="TimestampBuffer">A TPCANTimestamp structure buffer to get
/// the reception time of the message</param>
/// <returns>A TPCANStatus error code</returns>
function CAN_Read(
    Channel: TPCANHandle;
    var MessageBuffer: TPCANMsg;
    TimestampBuffer: PTPCANTimestamp
    ):TPCANStatus; stdcall;

/// <summary>
/// Reads a CAN message from the receive queue of a FD capable PCAN Channel 
/// </summary>
/// <param name="Channel">"The handle of a FD capable PCAN Channel"</param>
/// <param name="MessageBuffer">"A TPCANMsgFD structure buffer to store the CAN message"</param>
/// <param name="TimestampBuffer">"A TPCANTimestampFD buffer to get 
/// the reception time of the message. If this value is not desired, this parameter
/// should be passed as NULL"</param>
/// <returns>"A TPCANStatus error code"</returns>
function CAN_ReadFD(
    Channel: TPCANHandle;
	var MessageBuffer: TPCANMsgFD; 
	TimestampBuffer: PTPCANTimestampFD
	): TPCANStatus; stdcall;

/// <summary>
///  Transmits a CAN message
/// </summary>
/// <param name="Channel">The handle of a PCAN Channel</param>
/// <param name="MessageBuffer">A TPCANMsg buffer with the message to be sent</param>
/// <returns>A TPCANStatus error code</returns>
function CAN_Write(
    Channel: TPCANHandle;
    var MessageBuffer: TPCANMsg
    ): TPCANStatus; stdcall;

/// <summary>
/// Transmits a CAN message over a FD capable PCAN Channel
/// </summary>
/// <param name="Channel">"The handle of a FD capable PCAN Channel"</param>
/// <param name="MessageBuffer">"A TPCANMsgFD buffer with the message to be sent"</param>
/// <returns>"A TPCANStatus error code"</returns>
function CAN_WriteFD(
    Channel: TPCANHandle;
	var MessageBuffer: TPCANMsgFD
	): TPCANStatus; stdcall;

/// <summary>
/// Configures the reception filter
/// </summary>
/// <remarks>The message filter will be expanded with every call to
/// this function. If it is desired to reset the filter, please use
/// the 'SetValue' function</remarks>
/// <param name="Channel">The handle of a PCAN Channel</param>
/// <param name="FromID">The lowest CAN ID to be received</param>
/// <param name="ToID">The highest CAN ID to be received</param>
/// <param name="Mode">Message type, Standard (11-bit identifier) or
/// Extended (29-bit identifier)</param>
/// <returns>A TPCANStatus error code</returns>
function CAN_FilterMessages(
    Channel: TPCANHandle;
    FromID: LongWord;
    ToID: LongWord;
    Mode: TPCANMode
    ): TPCANStatus; stdcall;

/// <summary>
/// Retrieves a PCAN Channel value
/// </summary>
/// <remarks>Parameters can be present or not according with the kind
/// of Hardware (PCAN Channel) being used. If a parameter is not available,
/// a PCAN_ERROR_ILLPARAMTYPE error will be returned</remarks>
/// <param name="Channel">The handle of a PCAN Channel</param>
/// <param name="Parameter">The TPCANParameter parameter to get</param>
/// <param name="Buffer">Buffer for the parameter value</param>
/// <param name="BufferLength">Size in bytes of the buffer</param>
/// <returns>A TPCANStatus error code</returns>
function CAN_GetValue(
    Channel: TPCANHandle;
    Parameter: TPCANParameter;
    Buffer: Pointer;
    BufferLength: LongWord
    ): TPCANStatus; stdcall;

/// <summary>
/// Configures or sets a PCAN Channel value
/// </summary>
/// <remarks>Parameters can be present or not according with the kind
/// of Hardware (PCAN Channel) being used. If a parameter is not available,
/// a PCAN_ERROR_ILLPARAMTYPE error will be returned</remarks>
/// <param name="Channel">The handle of a PCAN Channel</param>
/// <param name="Parameter">The TPCANParameter parameter to set</param>
/// <param name="Buffer">Buffer with the value to be set</param>
/// <param name="BufferLength">Size in bytes of the buffer</param>
/// <returns>A TPCANStatus error code</returns>
function CAN_SetValue(
    Channel: TPCANHandle;
    Parameter: TPCANParameter;
    Buffer: Pointer;
    BufferLength: LongWord
    ): TPCANStatus; stdcall;

/// <summary>
/// Returns a descriptive text of a given TPCANStatus error
/// code, in any desired language
/// </summary>
/// <remarks>The current languages available for translation are:
/// Neutral (0x00), German (0x07), English (0x09), Spanish (0x0A),
/// Italian (0x10) and French (0x0C)</remarks>
/// <param name="Error">A TPCANStatus error code</param>
/// <param name="Language">Indicates a 'Primary language ID'</param>
/// <param name="StringBuffer">Buffer for the text (must be at least 256 in length)</param>
/// <returns>A TPCANStatus error code</returns>
function CAN_GetErrorText(
    Error: TPCANStatus;
    Language: Word;
    StringBuffer: PAnsiChar
    ): TPCANStatus; stdcall;

implementation
uses SysUtils;

const DLL_Name = 'PCANBASIC.DLL';

function CAN_Initialize(Channel: TPCANHandle; Btr0Btr1: TPCANBaudrate; HwType: TPCANType; IOPort: LongWord; Interrupt: Word): TPCANStatus; stdcall;
external DLL_Name;

function CAN_InitializeFD(Channel: TPCANHandle; BitrateFD: TPCANBitrateFD): TPCANStatus; stdcall;
external DLL_Name;

function CAN_Uninitialize(Channel: TPCANHandle): TPCANStatus; stdcall;
external DLL_Name;

function CAN_Reset(Channel: TPCANHandle): TPCANStatus; stdcall;
external DLL_Name;

function CAN_GetStatus(Channel: TPCANHandle): TPCANStatus; stdcall;
external DLL_Name;

function CAN_Read(Channel: TPCANHandle; var MessageBuffer: TPCANMsg; TimestampBuffer: PTPCANTimestamp):TPCANStatus; stdcall;
external DLL_Name;

function CAN_ReadFD(Channel: TPCANHandle; var MessageBuffer: TPCANMsgFD; TimestampBuffer: PTPCANTimestampFD):TPCANStatus; stdcall;
external DLL_Name;

function CAN_Write(Channel: TPCANHandle; var MessageBuffer: TPCANMsg): TPCANStatus; stdcall;
external DLL_Name;

function CAN_WriteFD(Channel: TPCANHandle; var MessageBuffer: TPCANMsgFD): TPCANStatus; stdcall;
external DLL_Name;

function CAN_FilterMessages(Channel: TPCANHandle; FromID: LongWord; ToID: LongWord; Mode: TPCANMode): TPCANStatus; stdcall;
external DLL_Name;

function CAN_GetValue(Channel: TPCANHandle; Parameter: TPCANParameter; Buffer: Pointer; BufferLength: LongWord): TPCANStatus; stdcall;
external DLL_Name;

function CAN_SetValue(Channel: TPCANHandle; Parameter: TPCANParameter; Buffer: Pointer; BufferLength: LongWord): TPCANStatus; stdcall;
external DLL_Name;

function CAN_GetErrorText(Error: TPCANStatus; Language: Word; StringBuffer: PAnsiChar): TPCANStatus; stdcall;
external DLL_Name;

end.