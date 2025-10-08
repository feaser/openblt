/*
 *                   Copyright 1994-2017 by KVASER AB, SWEDEN
 *
 *                        WWW: http://www.kvaser.com
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license.
 *
 * \note MATLAB users on Windows: if you define WIN32_LEAN_AND_MEAN before
 * including this file, you will see a lot less warnings.
 *
 */

#ifndef _CANSTAT_H_
#define _CANSTAT_H_

/**
 * \file canstat.h
 * \brief Definitions for the CANLIB API.
 *
*/

/**
 * \addtogroup can_status_codes Status Codes
 * \anchor canERR_xxx
 *
 * @{
 */

//
// Don't forget to update canGetErrorText in canlib.c if this is changed!
//
typedef enum {
  /**
   * <b>Normal successful completion</b>; The driver is just fine, and really
   * believes it carried out your command to everyone's satisfaction.
   */
    canOK                  = 0,
  /**
   * <b>Error in one or more parameters</b>; a parameter
   * specified in the call was invalid, out of range, or so. This status code
   * will also be returned when the call is not implemented.
   */
    canERR_PARAM           = -1,
  /**
   * <b>There were no messages to read</b>; A function tried to read a message,
   * but there was no message to read.
   */
    canERR_NOMSG           = -2,
  /**
   * <b>Specified device or channel not found.</b> There is no hardware
   * available that matches the given search criteria. For example, you may
   * have specified \ref canOPEN_REQUIRE_EXTENDED but there's no controller
   * capable of extended CAN. You may have specified a channel number that is
   * out of the range for the hardware in question. You may have requested
   * exclusive access to a channel, but the channel is already occupied.
   */
    canERR_NOTFOUND        = -3,
  /**
   * <b>Out of memory</b>; A memory allocation failed.
   */
    canERR_NOMEM           = -4,
  /**
   * <b>No channels available</b>; There is indeed hardware matching the
   * criteria you specified, but there are no channels available, or the
   * channel you specified is already occupied.
   */
    canERR_NOCHANNELS      = -5,

    canERR_INTERRUPTED     = -6, ///< Interrupted by signals
  /**
   * <b>Timeout occurred</b>; A function waited for something to happen (for
   * example, the arrival of a message), but that something didn't happen.
   */
    canERR_TIMEOUT         = -7,
  /**
   * <b>The library is not initialized</b>; The driver is not
   * initialized. \ref canInitializeLibrary() was probably not called?
   */
    canERR_NOTINITIALIZED  = -8,
  /**
   * <b>Out of handles</b>; No handles are available inside canlib32. The
   * application has too many handles open (i.e. has called \ref canOpenChannel() too
   * many times, or there's a memory leak somewhere.)
   *
   * \note We are not talking about Windows handles here, it's CANLIB's own
   * internal handles.
   */
    canERR_NOHANDLES       = -9,
  /**
   * <b>Handle is invalid</b>; The CANLIB handle you specified (if the API call
   * includes a handle) is not valid. Ensure you are passing the handle and
   * not, for example, a channel number.
   */
    canERR_INVHANDLE       = -10,
    canERR_INIFILE         = -11, ///< Error in the ini-file (16-bit only)
  /**
   * <b>Driver type not supported</b>; CAN driver mode is not supported by the
   * present hardware.
   */
    canERR_DRIVER          = -12,
  /**
   * <b>Transmit buffer overflow</b>; The transmit queue was full, so the
   * message was dropped.
   */
    canERR_TXBUFOFL        = -13,
    canERR_RESERVED_1      = -14, ///< Reserved
  /**
   * <b>A hardware error has occurred</b>; Something probably related to the
   * hardware happened. This could mean that the device does not respond (IRQ
   * or address conflict?), or that the response was invalid or unexpected
   * (faulty card?).
   */
    canERR_HARDWARE        = -15,
  /**
   * <b>A driver DLL can't be found or loaded</b>; (One of) the DLL(s)
   * specified in the registry failed to load. This could be a driver
   * installation problem.
   */
    canERR_DYNALOAD        = -16,
  /**
   * <b>A DLL seems to have wrong version</b>; DLL version mismatch. (One of)
   * the DLL(s) specified in the registry is - probably - too old, or - less
   * likely - too new.
   */
    canERR_DYNALIB         = -17,
  /**
   * <b>Error when initializing a DLL</b>; Something failed when a device
   * driver was being initialized. In other words, we can open the driver but
   * it makes a lot of fuss about something we don't understand.
   */
    canERR_DYNAINIT        = -18,
    canERR_NOT_SUPPORTED   = -19, ///< Operation not supported by hardware or firmware
    canERR_RESERVED_5      = -20, ///< Reserved
    canERR_RESERVED_6      = -21, ///< Reserved
    canERR_RESERVED_2      = -22, ///< Reserved
  /**
   * <b>Can't find or load kernel driver</b>; A device driver (kernel mode
   * driver for NT, VxD for W95/98) failed to load; or the DLL could not open
   * the device. Privileges? Driver file missing?
   */
    canERR_DRIVERLOAD      = -23,
  /**
   * <b>DeviceIOControl failed</b>; Use Win32 GetLastError() to learn what
   * really happened.
   */
    canERR_DRIVERFAILED    = -24,
    canERR_NOCONFIGMGR     = -25, ///< Can't find req'd config s/w (e.g. CS/SS)
    canERR_NOCARD          = -26, ///< The card was removed or not inserted
    canERR_RESERVED_7      = -27, ///< Reserved
  /**
   * <b>Error (missing data) in the Registry</b>; A registry key is missing,
   * invalid, malformed, has gone for lunch or what not. can_verify.exe might
   * provide some insight.
   */
    canERR_REGISTRY        = -28,
    canERR_LICENSE         = -29, ///< The license is not valid.
  /**
   * <b>Internal error in the driver</b>; Indicates an error condition in the
   * driver or DLL, which couldn't be properly handled. Please contact the
   * friendly support at support@kvaser.com.
   */
    canERR_INTERNAL        = -30,
  /**
   * <b>Access denied</b>; This means that you tried to set the bit rate on a
   * handle to which you haven't got init access or you tried to open a channel
   * that already is open with init access. See \ref canOpenChannel() for more
   * information about init access.
   */
    canERR_NO_ACCESS       = -31,
  /**
   * <b>Not implemented</b>; The requested feature or function is not
   * implemented in the device you are trying to use it on.
   */
    canERR_NOT_IMPLEMENTED = -32,
  /**
   * <b>Device File error</b>; An error has occured when trying to access a
   * file on the device.
   */
    canERR_DEVICE_FILE     = -33,
  /**
   * <b>Host File error</b>; An error has occured when trying to access a file on the host.
   *
   */
    canERR_HOST_FILE       = -34,
  /**
   * <b>Disk error</b>; A disk error has occurred. Verify that the disk is
   * initialized.
   */
    canERR_DISK            = -35,
  /**
   * <b>CRC error</b>; The CRC calculation did not match the expected result.
   */
    canERR_CRC             = -36,
  /**
   * <b>Configuration Error</b>; The configuration is corrupt.
   */
    canERR_CONFIG          = -37,
  /**
   * <b>Memo Error</b>; Other configuration error.
   */
    canERR_MEMO_FAIL       = -38,
  /**
   * <b>Script Fail</b>; A script has failed.
   *
   * \note This code represents several different failures, for example:
   * - Trying to load a corrupt file or not a .txe file
   * - Trying to start a t script that has not been loaded
   * - Trying to load a t script compiled with the wrong version of the t compiler
   * - Trying to unload a t script that has not been stopped
   * - Trying to use an envvar that does not exist
   */
    canERR_SCRIPT_FAIL     = -39,

  /**
   * <b>The t script version dosen't match the version(s) that the device firmware supports.</b>;
   */
    canERR_SCRIPT_WRONG_VERSION = -40,

  /**
   * <b>The compiled t script container file format is of a version which is not supported by this version of canlib.</b>;
   */
    canERR_SCRIPT_TXE_CONTAINER_VERSION = -41,

  /**
   * <b>An error occured while trying to parse the compiled t script file.</b>;
   */
    canERR_SCRIPT_TXE_CONTAINER_FORMAT = -42,

  /**
   * <b>The buffer provided was not large enough to contain the requested data.</b>;
   */
    canERR_BUFFER_TOO_SMALL = -43,

  /**
   * <b>The I/O pin doesn't exist or the I/O pin type doesn't match the called
   function, e.g. trying to use input pins as outputs or use digital pins as analog pins. </b>;
   */
    canERR_IO_WRONG_PIN_TYPE = -44,

  /**
   * <b>The I/O pin configuration is not confirmed. Use kvIoConfirmConfig()
   to confirm the configuration.</b>;
   */
    canERR_IO_NOT_CONFIRMED = -45,

  /**
   * <b>The I/O pin configuration has changed after last call to kvIoConfirmConfig. Use kvIoConfirmConfig()
   to confirm the new configuration. </b>;
   */
    canERR_IO_CONFIG_CHANGED = -46,

  /**
   * <b>The previous I/O pin value has not yet changed the output and is still pending.
   This happens when e.g. kvIoPinSetAnalog() is called twice on the same pin
   within a short time. </b>;
   */
    canERR_IO_PENDING = -47,

    /**
   * <b>There is no valid I/O pin configuration.
   * </b>
   */
    canERR_IO_NO_VALID_CONFIG = -48,

    // The last entry - a dummy so we know where NOT to place a comma.
    canERR__RESERVED       = -49    ///< Reserved
} canStatus;
/** @} */

/**
 * \name Convenience
 * @{
 */
#define CANSTATUS_SUCCESS(X) ((X) == canOK)
#define CANSTATUS_FAILURE(X) ((X) != canOK)
/** @} */

/**
 * \name canEVENT_xxx WM__CANLIB notification codes
 * \anchor canEVENT_xxx
 * Appears in the notification \c WM__CANLIB message.
 * @{
 */
#define canEVENT_RX             32000 ///< when the queue of received CAN messages goes from empty to non-empty
#define canEVENT_TX             32001 ///< when a CAN message has been transmitted
#define canEVENT_ERROR          32002 ///< when a CAN bus error is reported by the CAN controller
#define canEVENT_STATUS         32003 ///< when the CAN controller changes state
/**
 * An envvar in a t script changed.
 * \note Ignored by channels that does not have script capabilities,
 * \note Not implemented in Linux.
 * \sa \ref canCHANNEL_CAP_SCRIPT
 */
#define canEVENT_ENVVAR         32004
#define canEVENT_BUSONOFF       32005 ///< Bus on/off status changed
#define canEVENT_REMOVED        32006 ///< Device removed
/** @} */

/**
 * \name canNOTIFY_xxx
 * \anchor canNOTIFY_xxx
 *
 * These are notification codes used in calls to \ref canSetNotify() and \ref kvSetNotifyCallback().
 *
 * This can be \ref canNOTIFY_NONE (zero), meaning that no event notification is to
 * occur, or a combination of any of the following constants
 *
 * @{
 */
#define canNOTIFY_NONE          0      ///< Turn notifications off.
#define canNOTIFY_RX            0x0001 ///< CAN message reception notification
#define canNOTIFY_TX            0x0002 ///< CAN message transmission notification
#define canNOTIFY_ERROR         0x0004 ///< CAN bus error notification
#define canNOTIFY_STATUS        0x0008 ///< CAN chip status change
#define canNOTIFY_ENVVAR        0x0010 ///< An environment variable was changed by a script. Note that you will not be notified when an environment variable is updated from the Canlib API.

#define canNOTIFY_BUSONOFF      0x0020 ///< Notify on bus on/off status changed
#define canNOTIFY_REMOVED       0x0040 ///< Notify on device removed
/** @} */


/**
 * \ingroup InformationServices
 * \name canSTAT_xxx
 * \anchor canSTAT_xxx
 *
 * The following circuit status flags are returned by \ref canReadStatus(). Note
 * that more than one flag might be set at any one time.
 *
 * \note Usually both \ref canSTAT_HW_OVERRUN and \ref canSTAT_SW_OVERRUN are set
 * when overrun has occurred. This is because the kernel driver can't see the
 * difference between a software overrun and a hardware overrun. So the code
 * should always test for both types of overrun using the \ref canSTAT_OVERRUN
 * "flag".
 *
 * @{
 */
#define canSTAT_ERROR_PASSIVE   0x00000001 ///< The circuit is error passive
#define canSTAT_BUS_OFF         0x00000002 ///< The circuit is Off Bus
#define canSTAT_ERROR_WARNING   0x00000004 ///< At least one error counter > 96
#define canSTAT_ERROR_ACTIVE    0x00000008 ///< The circuit is error active.
#define canSTAT_TX_PENDING      0x00000010 ///< There are messages pending transmission
#define canSTAT_RX_PENDING      0x00000020 ///< There are messages in the receive buffer
#define canSTAT_RESERVED_1      0x00000040
#define canSTAT_TXERR           0x00000080 ///< There has been at least one TX error
#define canSTAT_RXERR           0x00000100 ///< There has been at least one RX error of some sort
#define canSTAT_HW_OVERRUN      0x00000200 ///< There has been at least one HW buffer overflow
#define canSTAT_SW_OVERRUN      0x00000400 ///< There has been at least one SW buffer overflow
#define canSTAT_OVERRUN         (canSTAT_HW_OVERRUN | canSTAT_SW_OVERRUN) ///< For convenience.
/** @} */

/**
 * \name Message information flags, CAN (canMSG_xxx)
 * \anchor canMSG_xxx
 *
 * The following flags can be returned from \ref canRead() et al, or passed to
 * \ref canWrite().
 *
 * All flags and/or combinations of them are meaningful for received messages.
 *
 * \ref canMSG_RTR, \ref canMSG_STD, \ref canMSG_EXT, \ref canMSG_WAKEUP and
 * \ref canMSG_ERROR_FRAME are meaningful also for transmitted messages.
 * \ref canMSG_SINGLE_SHOT is only for transmission.
 *
 * Note that \ref canMSG_RTR cannot be set for CAN FD messages.
 *
 * @{
 */
#define canMSG_MASK             0x00ff ///< Used to mask the non-info bits
#define canMSG_RTR              0x0001 ///< Message is a remote request

/**
 * Message has a standard ID. If a message has an extended identifier but no
 * \ref canMSG_EXT flag, the most significant bits of the identifier will be
 * cut off.
 */
#define canMSG_STD      0x0002 ///< Message has a standard (11-bit) identifier
#define canMSG_EXT      0x0004 ///< Message has an extended (29-bit) identifier
#define canMSG_WAKEUP   0x0008 ///< Message is a WAKEUP message (SWC hardware.)

/**
 * NERR was active during the message
 *
 * The NERR flag is set by the CAN transceiver when certain CAN bus wire faults
 * occur, namely:
 * \li CANH wire interrupted
 * \li CANL wire interrupted
 * \li CANH short-circuited to battery
 * \li CANH short-circuited to VCC
 * \li CANL short-circuited to ground
 * \li CANH short-circuited to ground
 * \li CANL short-circuited to battery
 * \li CANL short-circuited to VCC
 * \li CANL and CANH mutually short-circuited
 *
 * See the TJA1054 data sheet (available from Philips) for more detailed
 * information.
 */
#define canMSG_NERR             0x0010 ///< NERR was active during the message (TJA1054 hardware)
#define canMSG_ERROR_FRAME      0x0020 ///< Message represents an error frame.
#define canMSG_TXACK            0x0040 ///< Message is a TX ACK (msg has really been sent)
#define canMSG_TXRQ             0x0080 ///< Message is a TX REQUEST (msg was transfered to the chip)
#define canMSG_DELAY_MSG        0x0100 ///< Message is NOT sent on the bus. The transmission of messages are delayed. The dlc specifies the delay in milliseconds (1..1000).

#define canMSG_LOCAL_TXACK  0x10000000 ///< Message was sent from another handle on the same can channel.

// single shot flags:
#define canMSG_SINGLE_SHOT      0x1000000 ///< Message is Single Shot, try to send once, no retransmission. This flag can only be used with transmitted messages.
#define canMSG_TXNACK           0x2000000 ///< Message is a failed Single Shot, message was not sent. This flag is only used with received messages.
#define canMSG_ABL              0x4000000 ///< Only together with canMSG_TXNACK, Single shot message was not sent because arbitration was lost. This flag is only used with received messages.


/**
 * \name Message information flags, CAN FD (canFDMSG_xxx)
 * \anchor canFDMSG_xxx
 *
 * Flags used in the CAN FD protocol. Set \ref canOPEN_CAN_FD in \ref
 * canOpenChannel() to enable the CAN FD protocol.
 *
 * @{
 */
#define canFDMSG_MASK            0xff0000
#define canFDMSG_EDL             0x010000 ///< Obsolete, use \ref canFDMSG_FDF instead
#define canFDMSG_FDF             0x010000 ///< Message is an FD message (CAN FD)
#define canFDMSG_BRS             0x020000 ///< Message is sent/received with bit rate switch (CAN FD)
#define canFDMSG_ESI             0x040000 ///< Sender of the message is in error passive mode (CAN FD)
/** @} */

/** @} */

/**
 * \name Message error flags (canMsgERR_xxx)
 * \anchor canMSGERR_xxx
 *
 * \note Not all hardware platforms can detect the difference between hardware
 * overruns and software overruns, so your application should test for both
 * conditions. You can use the symbol \ref canMSGERR_OVERRUN for this purpose.
 *
 * @{
 */
#define canMSGERR_MASK          0xff00 ///< Used to mask the non-error bits
// 0x0100 reserved
#define canMSGERR_HW_OVERRUN    0x0200 ///< HW buffer overrun. \sa \ref section_user_guide_send_recv_overruns
#define canMSGERR_SW_OVERRUN    0x0400 ///< SW buffer overrun. \sa \ref section_user_guide_send_recv_overruns
#define canMSGERR_STUFF         0x0800 ///< Stuff error
#define canMSGERR_FORM          0x1000 ///< Form error
#define canMSGERR_CRC           0x2000 ///< CRC error
#define canMSGERR_BIT0          0x4000 ///< Sent dom, read rec
#define canMSGERR_BIT1          0x8000 ///< Sent rec, read dom

// Convenience values for the message error flags.
#define canMSGERR_OVERRUN       0x0600 ///< Any overrun condition. \sa \ref section_user_guide_send_recv_overruns
#define canMSGERR_BIT           0xC000 ///< Any bit error.
#define canMSGERR_BUSERR        0xF800 ///< Any RX error

/** @} */

#define canMSG_RESERVED_FOR_EXTENDING  0x80000000 ///< Reserved future use, tell user that there are more flags

/**
 * \name canTRANSCEIVER_LINEMODE_xxx
 * \anchor canTRANSCEIVER_LINEMODE_xxx
 *
 * The following values can be used with \ref canSetDriverMode() to set different
 * driver (i.e. transceiver) modes. Use \ref canGetDriverMode() to obtain the
 * current driver mode for a particular CAN channel.
 *
 * \note Not implemented in Linux.
 * @{
 */
#define canTRANSCEIVER_LINEMODE_NA         0 ///< Not Affected/Not available.
#define canTRANSCEIVER_LINEMODE_SWC_SLEEP  4 ///< SWC Sleep Mode.
#define canTRANSCEIVER_LINEMODE_SWC_NORMAL 5 ///< SWC Normal Mode.
#define canTRANSCEIVER_LINEMODE_SWC_FAST   6 ///< SWC High-Speed Mode.
#define canTRANSCEIVER_LINEMODE_SWC_WAKEUP 7 ///< SWC Wakeup Mode.
#define canTRANSCEIVER_LINEMODE_SLEEP      8 ///< Sleep mode for those supporting it.
#define canTRANSCEIVER_LINEMODE_NORMAL     9 ///< Normal mode (the inverse of sleep mode) for those supporting it.
#define canTRANSCEIVER_LINEMODE_STDBY      10 ///< Standby for those who support it
#define canTRANSCEIVER_LINEMODE_TT_CAN_H   11 ///< Truck & Trailer: operating mode single wire using CAN high
#define canTRANSCEIVER_LINEMODE_TT_CAN_L   12 ///< Truck & Trailer: operating mode single wire using CAN low
#define canTRANSCEIVER_LINEMODE_OEM1       13 ///< Reserved for OEM apps
#define canTRANSCEIVER_LINEMODE_OEM2       14 ///< Reserved for OEM apps
#define canTRANSCEIVER_LINEMODE_OEM3       15 ///< Reserved for OEM apps
#define canTRANSCEIVER_LINEMODE_OEM4       16 ///< Reserved for OEM apps

/** @} */

/**
 * \anchor canTRANSCEIVER_RESNET_xxx
 *
 */
#define canTRANSCEIVER_RESNET_NA          0
#define canTRANSCEIVER_RESNET_MASTER      1
#define canTRANSCEIVER_RESNET_MASTER_STBY 2
#define canTRANSCEIVER_RESNET_SLAVE       3

/**
 * \name Transceiver (logical) types
 * \anchor canTRANSCEIVER_TYPE_xxx
 *
 * The following constants can be returned from \ref canGetChannelData(), using the
 * \ref canCHANNELDATA_TRANS_TYPE item code. They identify the bus transceiver
 * type for the channel specified in the call to \ref canGetChannelData.
 *
 * \note They indicate a hardware type, but not necessarily a specific circuit
 * or product.
 *
 * @{
 */
// Also see src\include\hwnames.h and registered document 048.
#define canTRANSCEIVER_TYPE_UNKNOWN    0 ///< Unknown or undefined
#define canTRANSCEIVER_TYPE_251        1 ///< 82c251
#define canTRANSCEIVER_TYPE_252        2 ///< 82c252, TJA1053, TJA1054
#define canTRANSCEIVER_TYPE_DNOPTO     3 ///< Optoisolated 82C251
#define canTRANSCEIVER_TYPE_W210       4 ///<
#define canTRANSCEIVER_TYPE_SWC_PROTO  5 ///< AU5790 prototype
#define canTRANSCEIVER_TYPE_SWC        6 ///< AU5790
#define canTRANSCEIVER_TYPE_EVA        7 ///<
#define canTRANSCEIVER_TYPE_FIBER      8 ///< 82c251 with fibre extension
#define canTRANSCEIVER_TYPE_K251       9 ///< K-line + 82c251
#define canTRANSCEIVER_TYPE_K          10 ///< K-line, without CAN
#define canTRANSCEIVER_TYPE_1054_OPTO  11 ///< TJA1054 with optical isolation
#define canTRANSCEIVER_TYPE_SWC_OPTO   12 ///< AU5790 with optical isolation
#define canTRANSCEIVER_TYPE_TT         13 ///< B10011S Truck-And-Trailer
#define canTRANSCEIVER_TYPE_1050       14 ///< TJA1050
#define canTRANSCEIVER_TYPE_1050_OPTO  15 ///< TJA1050 with optical isolation
#define canTRANSCEIVER_TYPE_1041       16 ///< TJA1041
#define canTRANSCEIVER_TYPE_1041_OPTO  17 ///< TJA1041 with optical isolation
#define canTRANSCEIVER_TYPE_RS485      18 ///< RS485 (i.e. J1708)
#define canTRANSCEIVER_TYPE_LIN        19 ///< LIN
#define canTRANSCEIVER_TYPE_KONE       20 ///< KONE
#define canTRANSCEIVER_TYPE_CANFD      22 ///< CAN-FD
#define canTRANSCEIVER_TYPE_CANFD_LIN  24 ///< HYBRID CAN-FD/LIN
#define canTRANSCEIVER_TYPE_CANFD_SIC  26 ///< CAN-FD SIC transceiver
#define canTRANSCEIVER_TYPE_LINX_LIN   64
#define canTRANSCEIVER_TYPE_LINX_J1708 66
#define canTRANSCEIVER_TYPE_LINX_K     68
#define canTRANSCEIVER_TYPE_LINX_SWC   70
#define canTRANSCEIVER_TYPE_LINX_LS    72
/** @} */

#endif
