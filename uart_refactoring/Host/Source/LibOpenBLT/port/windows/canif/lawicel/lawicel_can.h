/*
**     __          ___   ____    __    ____  __    ______  _______  __      
**    |  |        /   \  \   \  /  \  /   / |  |  /      ||   ____||  |     
**    |  |       /  ^  \  \   \/    \/   /  |  | |  ,----'|  |__   |  |     
**    |  |      /  /_\  \  \            /   |  | |  |     |   __|  |  |     
**    |  `----./  _____  \  \    /\    /    |  | |  `----.|  |____ |  `----.
**    |_______/__/     \__\  \__/  \__/     |__|  \______||_______||_______|
**
**              Copyright (c) 2005-2012 LAWICEL AB, Sweden
*/

#ifndef __LAWICELCANH__         
#define __LAWICELCANH__

#include <windows.h>

#ifdef __cplusplus
  extern "C" {
#endif

// Types
typedef unsigned char _u8;
typedef unsigned __int16 _u16;
typedef unsigned __int32 _u32;


typedef long CANHANDLE;
typedef unsigned char CANDATA;

// Status bits
#define CANSTATUS_RECEIVE_FIFO_FULL		0x01
#define CANSTATUS_TRANSMIT_FIFO_FULL	0x02
#define CANSTATUS_ERROR_WARNING			0x04
#define CANSTATUS_DATA_OVERRUN			0x08
#define CANSTATUS_ERROR_PASSIVE			0x20
#define CANSTATUS_ARBITRATION_LOST		0x40
#define CANSTATUS_BUS_ERROR				0x80

// Filter mask settings
#define CANUSB_ACCEPTANCE_CODE_ALL		0x00000000
#define CANUSB_ACCEPTANCE_MASK_ALL		0xFFFFFFFF

// Message flags
#define CANMSG_EXTENDED		0x80 // Extended CAN id
#define CANMSG_RTR			0x40 // Remote frame

// Flush flags
#define FLUSH_WAIT			0x00
#define FLUSH_DONTWAIT		0x01
#define FLUSH_EMPTY_INQUEUE	0x02

// CAN Frame
typedef struct {
    _u32 id;        // Message id
    _u32 timestamp; // timestamp in milliseconds	
    _u8  flags;     // [extended_id|1][RTR:1][reserver:6]
    _u8  len;       // Frame size (0.8)
    _u8  data[ 8 ]; // Databytes 0..7
} CANMsg;


// Alternative CAN Frame
typedef struct {
	_u32 id;        // Message id
	_u32 timestamp; // timestamp in milliseconds
	_u8  flags;     // [extended_id|1][RTR:1][reserver:6]
	_u8  len;       // Frame size (0.8)
} CANMsgEx;

// Interface statistics
typedef struct structCANUsbStatistics {
	_u32 cntReceiveFrames;			// # of receive frames
	_u32 cntTransmitFrames;			// # of transmitted frames
	_u32 cntReceiveData;			// # of received data bytes
	_u32 cntTransmitData;			// # of transmitted data bytes	
	_u32 cntOverruns;				// # of overruns
	_u32 cntBusWarnings;			// # of bys warnings
	_u32 cntBusOff;					// # of bus off's
} CANUsbStatistics;


// Error return codes
#define ERROR_CANUSB_OK					1
#define ERROR_CANUSB_GENERAL			-1
#define ERROR_CANUSB_OPEN_SUBSYSTEM		-2
#define ERROR_CANUSB_COMMAND_SUBSYSTEM	-3
#define ERROR_CANUSB_NOT_OPEN			-4
#define ERROR_CANUSB_TX_FIFO_FULL		-5
#define ERROR_CANUSB_INVALID_PARAM		-6
#define ERROR_CANUSB_NO_MESSAGE			-7
#define ERROR_CANUSB_MEMORY_ERROR		-8
#define ERROR_CANUSB_NO_DEVICE			-9
#define ERROR_CANUSB_TIMEOUT			-10
#define ERROR_CANUSB_INVALID_HARDWARE	-11


// Open flags
#define CANUSB_FLAG_TIMESTAMP			  0x0001	// Timestamp messages
#define CANUSB_FLAG_QUEUE_REPLACE		0x0002	// If input queue is full remove
												                    // oldest message and insert new
												                    // message.
#define CANUSB_FLAG_BLOCK				    0x0004	// Block receive/transmit
#define CANUSB_FLAG_SLOW				    0x0008	// Check ACK/NACK's
#define CANUSB_FLAG_NO_LOCAL_SEND		0x0010	// Don't send transmited frames on
												                    // other local channels for the same
												                    // interface



// This is the define for the received callback method
typedef void ( __stdcall * LPFNDLL_RECEIVE_CALLBACK) ( CANMsg *pMsg );
 


// Prototypes

///////////////////////////////////////////////////////////////////////////////
// canusb_Open
//
// Open CAN interface to device
//
// Returs handle to device if open was successfull or zero
// or negative error code on falure.
//
//
// szID
// ====
// Serial number for adapter or NULL to open the first found.
//
//
// szBitrate
// =========
// "10" for 10kbps  
// "20" for 20kbps
// "50" for 50kbps
// "100" for 100kbps
// "250" for 250kbps
// "500" for 500kbps
// "800" for 800kbps
// "1000" for 1Mbps
//
// or 
//
// btr0:btr1 pair  ex. "0x03:0x1c" or 3:28
//
// acceptance_code
// ===============
// Set to CANUSB_ACCEPTANCE_CODE_ALL to  get all messages.
//
// acceptance_mask
// ===============
// Set to CANUSB_ACCEPTANCE_MASk_ALL to  get all messages.
//
// flags
// =====
// CANUSB_FLAG_TIMESTAMP - Timestamp will be set by adapter.
 
CANHANDLE WINAPI canusb_Open( LPCSTR szID, 
								LPCSTR szBitrate, 
								_u32 acceptance_code,
								_u32 acceptance_mask,
								_u32 flags );



///////////////////////////////////////////////////////////////////////////////
// canusb_Close
//
// Close channel with handle h.
//
// Returns <= 0 on failure. >0 on success.

int WINAPI canusb_Close( CANHANDLE h );



///////////////////////////////////////////////////////////////////////////////
// canusb_Read 
//
// Read message from channel with handle h. 
//
// Returns <= 0 on failure. >0 on success.
//

int WINAPI canusb_Read( CANHANDLE h, CANMsg *msg );


///////////////////////////////////////////////////////////////////////////////
// canusb_ReadEx
//
// Read message from channel with handle h.
//
// This is a version without a data-array in the structure to work with LabView
//
// Returns <= 0 on failure. >0 on success.
//

int WINAPI canusb_ReadEx( CANHANDLE h, CANMsgEx *msg, CANDATA *pData );

///////////////////////////////////////////////////////////////////////////////
// canusb_ReadFirst 
//
// Read message from channel with handle h and id "id" which satisfy flags. 
//
// Returns <= 0 on failure. >0 on success.
//

int WINAPI canusb_ReadFirst( CANHANDLE h,
								_u32 id,
								_u32 flags,
								CANMsg *msg );


///////////////////////////////////////////////////////////////////////////////
// canusb_ReadFirstEx
//
// Read message from channel with handle h and id "id" which satisfying flags.
//
// This is a version without a data-array in the structure to work with LabView
//
// Returns <= 0 on failure. >0 on success.
//

int WINAPI canusb_ReadFirstEx( CANHANDLE h, 
									_u32 id, 
									_u32 flags, 
									CANMsgEx *msg, 
									CANDATA *pData );


///////////////////////////////////////////////////////////////////////////////
// canusb_Write
//
// Write message to channel with handle h.
//
// Returns <= 0 on failure. >0 on success.
//

int WINAPI canusb_Write( CANHANDLE h, CANMsg *msg );


///////////////////////////////////////////////////////////////////////////////
// canusb_WriteEx
//
// Write message to channel with handle h. 
//
// This is a version without a data-array in the structure to work with LabView
//
// Returns <= 0 on failure. >0 on success.
//

int WINAPI canusb_WriteEx( CANHANDLE h, CANMsgEx *msg, CANDATA *pData );



///////////////////////////////////////////////////////////////////////////////
// canusb_Status
//
// Get Adaper status for channel with handle h. 

int WINAPI canusb_Status( CANHANDLE h );



///////////////////////////////////////////////////////////////////////////////
// canusb_VersionInfo
//
// Get hardware/fi4rmware and driver version for channel with handle h. 
//
// Returns <= 0 on failure. >0 on success.
//
// 

int WINAPI canusb_VersionInfo( CANHANDLE h, LPSTR verinfo );


///////////////////////////////////////////////////////////////////////////////
// canusb_Flush
//
// Flush output buffer on channel with handle h. 
//
// Returns <= 0 on failure. >0 on success.
//
// If flushflags is set to FLUSH_DONTWAIT the queue is just emptied and 
// there will be no wait for any frames in it to be sent 
//

int WINAPI canusb_Flush( CANHANDLE h, _u8 flushflags );


///////////////////////////////////////////////////////////////////////////////
// canusb_GetStatistics
//
// Get transmission statistics for channel with handle h. 
//
// Returns <= 0 on failure. >0 on success.
//
//

int WINAPI canusb_GetStatistics( CANHANDLE h, CANUsbStatistics *pStatistics );

///////////////////////////////////////////////////////////////////////////////
// canusb_SetTimeouts
//
// Set timeouts used for blocking calls for channel with handle h. 
//
// Returns <= 0 on failure. >0 on success.
//
//

int WINAPI canusb_SetTimeouts( CANHANDLE h, 
								_u32 receiveTimeout, 
								_u32 transmitTimeout );

///////////////////////////////////////////////////////////////////////////////
// canusb_getFirstAdapter
//
// Get the first found adapter that is connected to this machine.
//
// Returns <= 0 on failure. 0 if no adapter found. >0 if one or more adapters
//			is found.
//
//

int WINAPI canusb_getFirstAdapter( char *szAdapter, int size );


///////////////////////////////////////////////////////////////////////////////
// canusb_getNextAdapter
//
// Get the found adapter(s) in turn that is connected to this machine.
//
// Returns <= 0 on failure.  >0 for a valid adapter return.
//
//

int WINAPI canusb_getNextAdapter( char *szAdapter, int size );

///////////////////////////////////////////////////////////////////////////////
// canusb_setReceiveCallBack
//
// Set a receive call back function. Set the callback to NULL to
// reset it.
//
// Returns <= 0 on failure.  >0 for a valid adapter return.
//
//

int WINAPI canusb_setReceiveCallBack( CANHANDLE handle,
										LPFNDLL_RECEIVE_CALLBACK fn );


#ifdef __cplusplus
}
#endif

#endif // __LAWICELCANH__
