/*****************************************************************************
 HMS Technology Center Ravensburg GmbH
******************************************************************************

 File    : VCITYPE.H
 Summary : VCI specific constants, data types and macros.

 Date    : 2003-09-04
 Author  : Hartmut Heim

 Compiler: MSVC

******************************************************************************
 all rights reserved
*****************************************************************************/

#ifndef _VCITYPE_H_
#define _VCITYPE_H_

#include <stdtype.h>
#include <pshpack1.h>

/*****************************************************************************
 * unique VCI object identifier
 ****************************************************************************/

typedef union _VCIID
{
  LUID  AsLuid;
  INT64 AsInt64;
} VCIID, *PVCIID;

#if defined(__cplusplus)
typedef const VCIID& REFVCIID;
#else
typedef const VCIID* const REFVCIID;
#endif

/*****************************************************************************
 * VCI version information
 ****************************************************************************/

typedef struct _VCIVERSIONINFO
{
  UINT32 VciMajorVersion;    // major VCI version number
  UINT32 VciMinorVersion;    // minor VCI version number
  UINT32 VciRevNumber;       // VCI revision number
  UINT32 VciBuildNumber;     // VCI build number
  UINT32 OsMajorVersion;     // major OS version number
  UINT32 OsMinorVersion;     // minor OS version number
  UINT32 OsBuildNumber;      // OS build number
  UINT32 OsPlatformId;       // OS platform id
} VCIVERSIONINFO, *PVCIVERSIONINFO;


/*****************************************************************************
 * VCI device license information
 ****************************************************************************/

typedef struct _VCILICINFO
{
  GUID   DeviceClass;        // Class ID of the licensed product
  UINT32 MaxDevices;         // maximum number of allowed devices (0=no limit)
  UINT32 MaxRuntime;         // maximum runtime in seconds (0=no limit)
  UINT32 Restrictions;       // additional restrictions (see VCI_LICX_xxx const)
} VCILICINFO, *PVCILICINFO;

//
// license restriction flags
//

#define VCI_LICX_NORESTRICT  0x00000000 // no additional restrictions
#define VCI_LICX_SINGLEUSE   0x00000001 // single application use only


/*****************************************************************************
 * VCI driver information
 ****************************************************************************/

typedef struct _VCIDRIVERINFO
{
  VCIID  VciObjectId;   // unique VCI object identifier
  GUID   DriverClass;   // driver class identifier
  UINT16 MajorVersion;  // major driver version number
  UINT16 MinorVersion;  // minor driver version number
} VCIDRIVERINFO, *PVCIDRIVERINFO;


/*****************************************************************************
 * VCI device information
 ****************************************************************************/

typedef struct _VCIDEVICEINFO
{
  VCIID  VciObjectId;          // unique VCI object identifier
  GUID   DeviceClass;          // device class identifier

  UINT8  DriverMajorVersion;   // driver major version number
  UINT8  DriverMinorVersion;   // driver minor version number
  UINT16 DriverBuildVersion;   // driver build version number

  UINT8  HardwareBranchVersion;// hardware branch version number
  UINT8  HardwareMajorVersion; // hardware major version number
  UINT8  HardwareMinorVersion; // hardware minor version number
  UINT8  HardwareBuildVersion; // hardware build version number

  union _UniqueHardwareId      // unique hardware identifier
  {
    CHAR AsChar[16];
    GUID AsGuid;
  } UniqueHardwareId;

  CHAR   Description[128];     // device description
  CHAR   Manufacturer[126];    // device manufacturer
  UINT16 DriverReleaseVersion; // driver release version number
} VCIDEVICEINFO, *PVCIDEVICEINFO;


typedef struct _VCIDEVINFOEXA
{
  VCIID  VciObjectId;             // unique VCI object identifier
  GUID   DeviceClass;             // device class identifier

  UINT16 DriverMajorVersion;      // driver major version number
  UINT16 DriverMinorVersion;      // driver minor version number
  UINT16 DriverRevisionNumber;    // driver revision number
  UINT16 DriverBuildNumber;       // driver build number

  UINT16 HardwareBranchNumber;    // hardware branch version number
  UINT16 HardwareMajorVersion;    // hardware major version number
  UINT16 HardwareMinorVersion;    // hardware minor version number
  UINT16 HardwareBuildNumber;     // hardware build number

  UINT16 FpgaMajorVersion;        // FPGA major version number
  UINT16 FpgaMinorVersion;        // FPGA minor version number
  UINT16 FpgaRevisionNumber;      // FPGA revision number
  UINT16 FpgaBuildNumber;         // FPGA build number

  UINT16 BldrMajorVersion;        // boot loader major version number
  UINT16 BldrMinorVersion;        // boot loader minor version number
  UINT16 BldrRevisionNumber;      // boot loader revision number
  UINT16 BldrBuildNumber;         // boot loader build number

  GUID   UniqueHardwareId;        // unique hardware identifier

  CHAR   Description[128];        // device description
  CHAR   Manufacturer[128];       // device manufacturer
  CHAR   AliasName[128];          // device alias name
} VCIDEVINFOEXA, *PVCIDEVINFOEXA;


typedef struct _VCIDEVINFOEXW
{
  VCIID  VciObjectId;             // unique VCI object identifier
  GUID   DeviceClass;             // device class identifier

  UINT16 DriverMajorVersion;      // driver major version number
  UINT16 DriverMinorVersion;      // driver minor version number
  UINT16 DriverRevisionNumber;    // driver revision number
  UINT16 DriverBuildNumber;       // driver build number

  UINT16 HardwareBranchNumber;    // hardware branch version number
  UINT16 HardwareMajorVersion;    // hardware major version number
  UINT16 HardwareMinorVersion;    // hardware minor version number
  UINT16 HardwareBuildNumber;     // hardware build number

  UINT16 FpgaMajorVersion;        // FPGA major version number
  UINT16 FpgaMinorVersion;        // FPGA minor version number
  UINT16 FpgaRevisionNumber;      // FPGA revision number
  UINT16 FpgaBuildNumber;         // FPGA build number

  UINT16 BldrMajorVersion;        // boot loader major version number
  UINT16 BldrMinorVersion;        // boot loader minor version number
  UINT16 BldrRevisionNumber;      // boot loader revision number
  UINT16 BldrBuildNumber;         // boot loader build number

  GUID   UniqueHardwareId;        // unique hardware identifier

  WCHAR  Description[128];        // device description
  WCHAR  Manufacturer[128];       // device manufacturer
  WCHAR  AliasName[128];          // device alias name
} VCIDEVINFOEXW, *PVCIDEVINFOEXW;


/*****************************************************************************
 * VCI bus controller types
 *
 * +-------+--------+
 * | 15..8 | 7 .. 0 |
 * +-------+--------+
 * Bit 15..8 : bus type (0 is reserved)
 * Bit  7..0 : controller type
 ****************************************************************************/

#define VCI_BUS_RES  0 // reserved
#define VCI_BUS_CAN  1 // CAN
#define VCI_BUS_LIN  2 // LIN
#define VCI_BUS_KLN  3 // K-Line
#define VCI_BUS_FXR  4 // FlexRay

#define VCI_BUS_CTRL(Bus,Ctrl) (UINT16) ( ((Bus)<<8) | (Ctrl) )
#define VCI_BUS_TYPE(BusCtrl)  (UINT8)  ( ((BusCtrl) >> 8) & 0x00FF )
#define VCI_CTL_TYPE(BusCtrl)  (UINT8)  ( ((BusCtrl) >> 0) & 0x00FF )

//------------------------------------------------------------------------
// CAN controller types
//------------------------------------------------------------------------

// unknown CAN controller
#define VCI_CAN_UNKNOWN      VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_UNKNOWN)

// Intel 82527
#define VCI_CAN_82527        VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_82527)

// Intel 82C200
#define VCI_CAN_82C200       VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_82C200)

// Intel 81C90
#define VCI_CAN_81C90        VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_81C90)

// Intel 81C92
#define VCI_CAN_81C92        VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_81C92)

// Philips SJA 1000
#define VCI_CAN_SJA1000      VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_SJA1000)

// Infineon 82C900 (TwinCAN)
#define VCI_CAN_82C900       VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_82C900)

// Motorola TOUCAN
#define VCI_CAN_TOUCAN       VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_TOUCAN)

// Freescale Star12 MSCAN
#define VCI_CAN_MSCAN        VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_MSCAN)

// Freescale FlexCAN
#define VCI_CAN_FLEXCAN      VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_FLEXCAN)

// IFI CAN Controller
#define VCI_CAN_IFICAN       VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_IFICAN)

// IFI CAN FD Controller
#define VCI_CAN_IFICFD       VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_IFICFD)

// Bosch C_CAN
#define VCI_CAN_BCCAN        VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_BCCAN)

// ST BX_CAN
#define VCI_CAN_BXCAN        VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_BXCAN)

// Bosch M_CAN version A
#define VCI_CAN_BMCAN        VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_BMCAN)

// IFI CAN FD Controller (new)
#define VCI_CAN_IFICFD2      VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_IFICFD2)

// Bosch M_CAN version B
#define VCI_CAN_BMCAN2       VCI_BUS_CTRL(VCI_BUS_CAN, CAN_CTRL_BMCAN2)


//------------------------------------------------------------------------
// LIN controller types
//------------------------------------------------------------------------

// unknown LIN controller
#define VCI_LIN_UNKNOWN      VCI_BUS_CTRL(VCI_BUS_LIN, LIN_CTRL_UNKNOWN)


//------------------------------------------------------------------------
// K-Line controller types
//------------------------------------------------------------------------

// unknown K-Line controller
#define VCI_KLN_UNKNOWN      VCI_BUS_CTRL(VCI_BUS_KLN, KLN_CTRL_UNKNOWN)

// RS232 to K-Line converter
#define VCI_KLN_RS232        VCI_BUS_CTRL(VCI_BUS_KLN, KLN_CTRL_RS232)


//------------------------------------------------------------------------
// FlexRay controller types
//------------------------------------------------------------------------

// generic FlexRay controller
#define VCI_FXR_GENERIC      VCI_BUS_CTRL(VCI_BUS_FXR, FXR_CTRL_GENERIC)

// Freescale MFR4200, MFR4300, MFR4310
#define VCI_FXR_MFR4200      VCI_BUS_CTRL(VCI_BUS_FXR, FXR_CTRL_MFR4200)
#define VCI_FXR_MFR4300      VCI_BUS_CTRL(VCI_BUS_FXR, FXR_CTRL_MFR4300)
#define VCI_FXR_MFR4310      VCI_BUS_CTRL(VCI_BUS_FXR, FXR_CTRL_MFR4310)


/*****************************************************************************
 * VCI device capabilities
 ****************************************************************************/

typedef struct _VCIDEVICECAPS
{
  UINT16 BusCtrlCount;       // number of supported bus controllers
  UINT16 BusCtrlTypes[32];   // array of supported bus controllers
} VCIDEVICECAPS, *PVCIDEVICECAPS;

#define VCI_MAX_BUSCTRL 32   // maximum number of supported bus controllers


/*****************************************************************************
 * VCI device hardware interface information
 ****************************************************************************/

//
// device interface socket information
//
typedef union _VCIDIFSINFO
{
  struct
  {
    UINT32 Slot;        // slot number
  } isa;

  struct
  {
    UINT32 Slot;        // slot number
  } mca;

  struct
  {
    UINT32 Slot;        // slot number
  } eisa;

  struct
  {
    UINT32 Bus;         // bus number
    UINT32 Slot;        // slot number
    UINT32 Func;        // function number
  } pci;

  struct
  {
    UINT32 Slot;        // slot number
    UINT32 Func;        // function number
  } pcmcia;

  struct
  {
    UINT32 Port;        // serial port number
  } ser;

  struct
  {
    UINT32 Port;        // parallel port number
  } par;

  struct
  {
    UINT32 Port;        // FireWire port number
  } fiwi;

  struct
  {
    UINT16 RootPort;    // USB root hub port number
    UINT16 HubCount;    // Number of external USB hubs. Zero indicates that
                        // the device is connected directly to the root hub.
    UINT16 HubPorts[5]; // Array containing the port number on each external
                        // hub (between the root hub and the device) through
                        // which the USB device is connected.
                        // The first element of the array indicates the port
                        // on the hub that is connected directly to the root
                        // hub. (Only Windows Vista and newer)
  } usb;

  struct
  {
    UINT64 AddrLo;      // Low Part of IPv6 LAN Address
    UINT64 AddrHi;      // High Part of IPv6 LAN Address
    UINT32 Port;        // LAN Port Number
  } lan;

  struct
  {
    UINT64 AddrLo;      // Low Part of IPv6 WLAN Address
    UINT64 AddrHi;      // High Part of IPv6 WLAN Address
    UINT32 Port;        // WLAN Port Number
  } wlan;

  struct
  {
    UINT8 Data[32];   // unspecific socket info
  } unspec;

} VCIDIFSINFO, *PVCIDIFSINFO;

//
// device interface informarion
//
typedef struct _VCIDEVIFINFOA
{
  UINT32      InterfaceType; // interface type (see VCI_DEVIF_)
  VCIDIFSINFO SocketInfo;    // socket information
  CHAR        Location[256]; // additional location information
} VCIDEVIFINFOA, *PVCIDEVIFINFOA;

typedef struct _VCIDEVIFINFOW
{
  UINT32      InterfaceType; // interface type (see VCI_DEVIF_)
  VCIDIFSINFO SocketInfo;    // socket information
  WCHAR       Location[256]; // additional location information
} VCIDEVIFINFOW, *PVCIDEVIFINFOW;


//
// VCI device interface types
//
#define VCI_DEVIF_INVALID    0xFFFFFFFF // invalid interface type

//--- PC specific interfaces ---------------------------------------------

#define VCI_DEVIF_SWI        0x00000000 // software interface
                                        // (e.g.: Mailslot, Pipe, etc.)
#define VCI_DEVIF_INT        0x00000001 // internal bus interface
#define VCI_DEVIF_ISA        0x00000002 // ISA bus interface
#define VCI_DEVIF_MCA        0x00000003 // Micro Channel bus interface
#define VCI_DEVIF_EISA       0x00000004 // extended ISA (EISA) bus interface
#define VCI_DEVIF_PCMCIA     0x00000005 // PCMCI interface
#define VCI_DEVIF_PCI        0x00000006 // PCI interface card
#define VCI_DEVIF_PCIx       0x00000007 // extended PCI interface card
#define VCI_DEVIF_PCIe       0x00000008 // PCI express interface card

//--- embedded interfaces ------------------------------------------------

#define VCI_DEVIF_GPIO       0x00010000 // GPIO interface
#define VCI_DEVIF_SPI        0x00010001 // serial port interface
#define VCI_DEVIF_I2C        0x00010002 // I²C interface
#define VCI_DEVIF_PC104      0x00010003 // PC/104 bus interface
#define VCI_DEVIF_PC104P     0x00010004 // PC/104-plus bus interface
#define VCI_DEVIF_PCI104     0x00010005 // PCI/104 bus interface
#define VCI_DEVIF_PCIe104    0x00010006 // PCIe/104 bus interface

//--- external interfaces ------------------------------------------------

#define VCI_DEVIF_SER        0x00020000 // serial port interface (COM)
#define VCI_DEVIF_PAR        0x00020001 // IEEE-1284 (parallel port) interface
#define VCI_DEVIF_FIWI       0x00020002 // IEEE-1394 (FireWire) interface
#define VCI_DEVIF_USB        0x00020003 // Universal Serial Bus interface
#define VCI_DEVIF_LAN        0x00020004 // local area network interface

//--- wireless interfaces ------------------------------------------------

#define VCI_DEVIF_IRDA       0x00030000 // infrared  interface
#define VCI_DEVIF_WLAN       0x00030001 // wireless local area interface
#define VCI_DEVIF_BTH        0x00030002 // BlueTooth interface


/*****************************************************************************
 * VCI device run-time status information
 ****************************************************************************/

typedef struct _VCIDEVRTINFO
{
  //--- data provided by VCI system service

  UINT32 DevRefCnt;    // current value of the device reference counter
  UINT32 DevReqCnt;    // current value of device request counter
  UINT32 DevResCnt;    // current value of device response counter
  UINT32 DevStatus;    // device status flags (see VCI_DEVRTI_STAT_)
  UINT32 CommLayer;    // currently running communication layer
                       // (see VCI_DEVRTI_COMML_)
  UINT32 CommRefs;     // current value of the communication layer
                       // reference counter

  //--- data provided by hardware device driver

  UINT8  PnpState;     // device PnP status (see VCI_DEVRTI_PNP_)
  UINT8  PwrState;     // current power state (see VCI_DEVRTI_POWER_)
  UINT16 _Rsvd_1;
  UINT32 FwStatus;     // firmware status (see VCI_DEVRTI_FWST_)
  //tbd
} VCIDEVRTINFO, *PVCIDEVRTINFO;


//
// device status flags
//
#define VCI_DEVRTI_STAT_LICEXP    0x00000001 // run-time of license expired
#define VCI_DEVRTI_STAT_DISCON    0x80000000 // device driver disconnected

//
// communication layer IDs
//
#define VCI_DEVRTI_COMML_INV      0x00000000 // invalid
#define VCI_DEVRTI_COMML_DAL      0x004C4144 // 'DAL'
#define VCI_DEVRTI_COMML_FLD      0x00444C46 // 'FLD'
#define VCI_DEVRTI_COMML_CCL      0x004C4343 // 'CCL'
#define VCI_DEVRTI_COMML_BAL      0x004C4142 // 'BAL'


//
// device  Plug & Play status
//
#define VCI_DEVRTI_PNP_INVALID    0 // invalid state
#define VCI_DEVRTI_PNP_NOTINIT    1 // not initialized
#define VCI_DEVRTI_PNP_STARTED    2 // device started
#define VCI_DEVRTI_PNP_STOPPED    3 // device stopped
#define VCI_DEVRTI_PNP_REMOVED    4 // device removed
#define VCI_DEVRTI_PNP_DELETED    5 // device deleted

//
// device power status
//
#define VCI_DEVRTI_POWER_DX       0  // unknown or unspecified
#define VCI_DEVRTI_POWER_D0       1  // device working state
#define VCI_DEVRTI_POWER_D1       2  // device low power state < D0
#define VCI_DEVRTI_POWER_D2       3  // device low power state < D1
#define VCI_DEVRTI_POWER_D3       4  // device sleeping state

//
// firmware status flags
//
#define VCI_DEVRTI_FWST_LOADED    0x00000001 // firmware loaded
#define VCI_DEVRTI_FWST_HALTED    0x00000002 // firmware halted


#include <poppack.h>
#endif //_VCITYPE_H_
