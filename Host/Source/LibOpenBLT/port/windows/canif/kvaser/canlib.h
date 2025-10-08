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

/**
 * \file canlib.h
 * \brief Definitions for the CANlib API.
 * \details
 * \defgroup grp_canlib              CANlib
 * \brief Library for interacting with the CAN bus
 * \defgroup can_status_codes        Status Codes
 * \brief  Return codes from the CANlib functions
 * \details
 * Generally, a return code greater than or equal to zero means success.\n
 * A value less than zero means failure.
 *
 * You should avoid testing for a specific error code unless it is explicitly\n
 * stated in the documentation for the API in question that this specific error\n
 * code is returned. Instead, check if the error code is less than zero.\n
 * Use \ref canGetErrorText() to obtain an error message suitable for a message to the\n
 * user, or for a line in an error log file.
 * \ingroup grp_canlib
 * \defgroup can_general             General
 * \brief Initialization, reading versions and other information
 * \ingroup grp_canlib
 * \defgroup CAN                     CAN
 * \brief CAN related functions
 * \ingroup grp_canlib
 * \defgroup ObjectBuffers           Object buffers
 * \brief Used for auto transmit and auto response messages
 * \ingroup grp_canlib
 * \defgroup TimeDomainHandling      Time Domain Handling
 * \brief Creating and handling Time Domains
 * \ingroup grp_canlib
 * \defgroup NamedParameterSettings  Named Parameter Settings
 * \brief Handling of Bus Parameters and more
 * \ingroup grp_canlib
 * \defgroup tScript                 t-script
 * \ingroup grp_canlib
 * \brief Starting, stopping scripts, moving files to/from device
 * \defgroup grp_kvfile              File Handling
 * \brief Handling files and the files system on devices.
 * \ingroup grp_canlib
 * \defgroup kv_io                  I/O Pin Handling
 * \brief Functions related to handling I/O pins
 * \details The kvIoXxx functions are used for configuring and controlling I/O pins on I/O modules connected to a device.
 *
 * I/O functions use a pin number to access each pin on connected I/O modules. Pin enumeration is product dependent.
 *
 * It is required to verify the configuration before it is possible to use any kvIoPinSetXxx()/kvIoPinGetXxx() function.
 * The module configuration can be examined with kvIoGetNumberOfPins() and kvIoPinGetInfo(). If the configuration is as expected, a call to kvIoConfirmConfig() will confirm the configuration. Removing or adding I/O modules will change the configuration and the pins are re-enumerated. It is always required to confirm the new configuration after a configuration change.
 *
 * \ingroup grp_canlib
 *
 * \defgroup kvdiag_diagnostics    CANtegrity API
 * \brief CANtegrity API
 * \details The kvDiagXxx functions specify the CANtegrity API, which is based on high frequency sampling of the CAN bus.
 * To check if a device has CANtegrity support, use \ref canGetChannelData() with the item \ref canCHANNELDATA_CHANNEL_CAP and the flag \ref canCHANNEL_CAP_CANTEGRITY.
 * \ingroup grp_canlib
 *
 */

/**
 *  \page page_canlib CAN bus API (CANlib)
 *
 * The CAN bus API (CANlib) is used to interact with Kvaser CAN devices
 * connected to your computer and the CAN bus. At its core you have functions
 * to set bus parameters (e.g. bit rate), go bus on/off and read/write CAN
 * messages. You can also use CANlib to download and start t programs on
 * supported devices. If you can see your device listed in the Kvaser Device
 * Guide tool, it is connected and you can communicate with it through CANlib.
 *
 * Contents:
 *
 *  - \subpage page_user_guide_intro
 *  - \subpage page_user_guide_init
 *  - \subpage page_user_guide_device_and_channel
 *  - \subpage page_user_guide_chips_channels
 *  - \subpage page_user_guide_can_frame_types_types
 *  - \subpage page_user_guide_send_recv
 *  - \subpage page_user_guide_bus_errors
 *  - \subpage page_user_guide_time
 *  - \subpage page_user_guide_version
 *  - \subpage page_user_guide_threads
 *  - \subpage page_user_guide_send_recv_asynch_not
 *  - \subpage page_user_guide_kvscript
 *  - \subpage page_user_guide_kvfile
 *  - \subpage page_user_guide_kviopin
 *  - \subpage page_user_guide_kvdiag
 *  - \subpage page_user_guide_send_recv_mailboxes
 *  - \subpage page_user_guide_userdata
 *  - \subpage page_user_guide_install
 *  - \subpage page_canlib_api_calls_grouped_by_function
 *  - \subpage page_user_guide_canlib_samples
**/

#ifndef _CANLIB_H_
#define _CANLIB_H_

#include <stdlib.h>

#include <windows.h>
#include "predef.h"
#include "canevt.h"

#include "canstat.h"

/** Handle to an opened circuit, created with \ref canOpenChannel(). */
typedef int canHandle;

#define canINVALID_HANDLE (-1) ///< Indicates an invalid \ref canHandle

/** Handle to an opened circuit, created with \ref canOpenChannel(). */
#define CanHandle int

/** Notify message sent to the application window */
#define WM__CANLIB              (WM_USER+16354)

/**
 * \name canOPEN_xxx
 * \anchor canOPEN_xxx
 *
 * These defines are used in \ref canOpenChannel().
 *
 * @{
 */


/**
 * Don't allow sharing of this CANlib channel.
 *
 * Two or more threads or applications can share the same CAN channel by
 * opening multiple handles to the same CANlib channel. If this is not desired
 * you can open a single exclusive handle to a channel which is done by passing
 * the \ref canOPEN_EXCLUSIVE flag in the flags argument to \ref
 * canOpenChannel().  If a handle to the CANlib channel is already open, the
 * call to \ref canOpenChannel() will fail.
 *
 * This define is used in \ref canOpenChannel()
 */
#define canOPEN_EXCLUSIVE               0x0008

/**
 * This flag causes two things to happen:
 *
 * \li The call will fail if the specified circuit doesn't allow extended CAN
 *     (CAN 2.0B).
 *
 * \li If no frame-type flag is specified in a call to \ref canWrite, it is assumed
 *     that extended CAN should be used.
 *
 * This define is used in \ref canOpenChannel().
 */
#define canOPEN_REQUIRE_EXTENDED        0x0010

/**
 * Allow opening of virtual channels as well as physical channels.
 *
 * This define is used in \ref canOpenChannel().
 *
 * \sa \ref section_user_guide_virtual
 */
#define canOPEN_ACCEPT_VIRTUAL         0x0020

/**
 * Open the channel even if it is opened for exclusive
 * access already.
 *
 * \warning Use this flag with caution.
 *
 * This define is used in \ref canOpenChannel().
 */
#define canOPEN_OVERRIDE_EXCLUSIVE     0x0040

/**
 * Fail the call if the channel cannot be opened with init access.
 *
 * Init access means that the CAN handle can set bit rate and CAN driver
 * mode. At most one CAN handle may have init access to any given channel. If
 * you try to set the bit rate or CAN driver mode for a handle to which you
 * don't have init access, the call will silently fail (i.e. \ref canOK is
 * returned although the call had no effect), unless you enable "access error
 * reporting" by calling \ref canIoCtl() using \ref
 * canIOCTL_SET_REPORT_ACCESS_ERRORS. Access error reporting is by default
 * off. Init access is the default.
 *
 * This define is used in \ref canOpenChannel().
 *
 * \sa \ref canOPEN_NO_INIT_ACCESS, \ref canIOCTL_SET_REPORT_ACCESS_ERRORS
 */
#define canOPEN_REQUIRE_INIT_ACCESS    0x0080

/**
 * Don't open the handle with init access.
 *
 * This define is used in \ref canOpenChannel().
 *
 * \note A handle opened without init access will still set default bitrate
 * when going on bus, if no other handle has opened the channel with init
 * access at the time of the buson.
 *
 * \sa \ref canOPEN_REQUIRE_INIT_ACCESS
 */
#define canOPEN_NO_INIT_ACCESS         0x0100

/**
 * The channel will accept messages with DLC (Data Length Code) greater than
 * 8. If this flag is not used, a message with DLC > 8 will always be
 * reported or transmitted as a message with DLC = 8. If the
 * \ref canOPEN_ACCEPT_LARGE_DLC flag is used, the message will be sent and/or
 * received with the true DLC, which can be at most 15.
 *
 * \note The length of the message is always at most 8.
 *
 * This define is used in \ref canOpenChannel().
 */
#define canOPEN_ACCEPT_LARGE_DLC       0x0200 // DLC can be greater than 8

/**
 * The channel will use the CAN FD protocol, ISO compliant. This also means that messages with
 * \ref canFDMSG_xxx flags can now be used.
 *
 * This define is used in \ref canOpenChannel().
 */
#define canOPEN_CAN_FD                 0x0400

/**
 * The channel will use the CAN FD NON-ISO protocol.
 * Use this if you want to configure the can controller to be able to communicate with
 * a can controller designed prior to the release of the CAN FD ISO specification.
 *
 * Non ISO mode implies:
 * \li The stuff bit counter will not be included in the frame format.
 * \li Initial value for CRC17 and CRC21 will be zero.
 *
 * This also means that messages with \ref canFDMSG_xxx flags can now be used.
 *
 * This define is used in \ref canOpenChannel().
 */
#define canOPEN_CAN_FD_NONISO          0x0800


/**
 * This is for internal use ONLY.
 */
#define canOPEN_INTERNAL_L                 0x1000

/** @} */

/**
 * \ingroup CAN
 * \name canFILTER_xxx
 * \anchor canFILTER_xxx
 *
 * Flags for \ref canAccept().
 *
 * @{
 */
#define canFILTER_ACCEPT        1
#define canFILTER_REJECT        2
/** Sets the code for standard (11-bit) identifiers. */
#define canFILTER_SET_CODE_STD  3
/** Sets the mask for standard (11-bit) identifiers. */
#define canFILTER_SET_MASK_STD  4
/** Sets the code for extended (29-bit) identifiers. */
#define canFILTER_SET_CODE_EXT  5
/** Sets the mask for extended (29-bit) identifiers. */
#define canFILTER_SET_MASK_EXT  6

#define canFILTER_NULL_MASK     0L
/** @} */


/**
 * \ingroup CAN
 * \name canDRIVER_xxx
 * \anchor canDRIVER_xxx
 *
 * CAN driver types - not all are supported on all cards.
 *
 * @{
 */

/**
 * The "normal" driver type (push-pull). This is the default.
 */
#define canDRIVER_NORMAL           4

/**
 * Sets the CAN controller in Silent Mode; that is, it doesn't send anything,
 * not even ACK bits, on the bus.  Reception works as usual.
 *
 * \note The values 2,3,5,6,7 are reserved values for compatibility reasons.
 */
#define canDRIVER_SILENT           1

/**
 * Self-reception. Not implemented.
 */
#define canDRIVER_SELFRECEPTION    8

/**
 * The driver is turned off. Not implemented in all types of hardware.
 */
#define canDRIVER_OFF              0

/** @} */


/**
 * \ingroup CAN
 * \anchor BAUD_xxx
 * \anchor canBITRATE_xxx
 * \anchor canFD_BITRATE_xxx
 * \name canBITRATE_xxx
 *
 * Common bus speeds. Used in \ref canSetBusParams() and \ref canSetBusParamsC200().
 * The values are translated in canlib, \ref canTranslateBaud().
 *
 * \note The \ref BAUD_xxx names are only retained for compatibility.
 *
 * \sa \ref section_user_guide_init_bit_rate_can, \ref section_user_guide_init_bit_rate_canfd
 *
 * @{
 */


/** Used in \ref canSetBusParams() and \ref canSetBusParamsC200(). Indicate a bitrate of 1 Mbit/s. */
#define canBITRATE_1M        (-1)
/** Used in \ref canSetBusParams() and \ref canSetBusParamsC200(). Indicate a bitrate of 500 kbit/s. */
#define canBITRATE_500K      (-2)
/** Used in \ref canSetBusParams() and \ref canSetBusParamsC200(). Indicate a bitrate of 250 kbit/s. */
#define canBITRATE_250K      (-3)
/** Used in \ref canSetBusParams() and \ref canSetBusParamsC200(). Indicate a bitrate of 125 kbit/s. */
#define canBITRATE_125K      (-4)
/** Used in \ref canSetBusParams() and \ref canSetBusParamsC200(). Indicate a bitrate of 100 kbit/s. */
#define canBITRATE_100K      (-5)
/** Used in \ref canSetBusParams() and \ref canSetBusParamsC200(). Indicate a bitrate of 62 kbit/s. */
#define canBITRATE_62K       (-6)
/** Used in \ref canSetBusParams() and \ref canSetBusParamsC200(). Indicate a bitrate of 50 kbit/s. */
#define canBITRATE_50K       (-7)
/** Used in \ref canSetBusParams() and \ref canSetBusParamsC200(). Indicate a bitrate of 83 kbit/s. */
#define canBITRATE_83K       (-8)
/** Used in \ref canSetBusParams() and \ref canSetBusParamsC200(). Indicate a bitrate of 10 kbit/s. */
#define canBITRATE_10K       (-9)

// CAN FD Bit Rates
/** Used in \ref canSetBusParams() and \ref canSetBusParamsFd() when using the
    CAN FD protocol. Indicates a bitrate of 0.5 Mbit/s and sampling point at
    80%. */
#define canFD_BITRATE_500K_80P     (-1000)
/** Used in \ref canSetBusParams() and \ref canSetBusParamsFd() when using the
    CAN FD protocol. Indicates a bitrate of 1.0 Mbit/s and sampling point at
    80%. */
#define canFD_BITRATE_1M_80P       (-1001)
/** Used in \ref canSetBusParams() and \ref canSetBusParamsFd() when using the
    CAN FD protocol. Indicates a bitrate of 2.0 Mbit/s and sampling point at
    80%. */
#define canFD_BITRATE_2M_80P       (-1002)
/** Used in \ref canSetBusParams() and \ref canSetBusParamsFd() when using the
    CAN FD protocol. Indicates a bitrate of 2.0 Mbit/s and sampling point at
    60%. */
#define canFD_BITRATE_2M_60P       (-1007)
/** Used in \ref canSetBusParams() and \ref canSetBusParamsFd() when using the
    CAN FD protocol. Indicates a bitrate of 4.0 Mbit/s and sampling point at
    80%. */
#define canFD_BITRATE_4M_80P       (-1003)
/** Used in \ref canSetBusParams() and \ref canSetBusParamsFd() when using the
    CAN FD protocol. Indicates a bitrate of 8.0 Mbit/s and sampling point at
    60%. */
#define canFD_BITRATE_8M_60P       (-1004)
/** Used in \ref canSetBusParams() and \ref canSetBusParamsFd() when using the
    CAN FD protocol. Indicates a bitrate of 8.0 Mbit/s and sampling point at
    80%. */
#define canFD_BITRATE_8M_80P       (-1005)
/** Used in \ref canSetBusParams() and \ref canSetBusParamsFd() when using the
    CAN FD protocol. Indicates a bitrate of 8.0 Mbit/s and sampling point at
    70%. */
#define canFD_BITRATE_8M_70P       (-1006)

/** The \ref BAUD_xxx names are deprecated, use \ref canBITRATE_1M instead. */
#define BAUD_1M              (-1)
/** The \ref BAUD_xxx names are deprecated, use \ref canBITRATE_500K instead. */
#define BAUD_500K            (-2)
/** The \ref BAUD_xxx names are deprecated, use \ref canBITRATE_250K instead. */
#define BAUD_250K            (-3)
/** The \ref BAUD_xxx names are deprecated, use \ref canBITRATE_125K instead. */
#define BAUD_125K            (-4)
/** The \ref BAUD_xxx names are deprecated, use \ref canBITRATE_100K instead. */
#define BAUD_100K            (-5)
/** The \ref BAUD_xxx names are deprecated, use \ref canBITRATE_62K instead. */
#define BAUD_62K             (-6)
/** The \ref BAUD_xxx names are deprecated, use \ref canBITRATE_50K instead. */
#define BAUD_50K             (-7)
/** The \ref BAUD_xxx names are deprecated, use \ref canBITRATE_83K instead. */
#define BAUD_83K             (-8)
/** @} */


//
// Define CANLIBAPI unless it's done already.
// (canlib.c provides its own definitions of CANLIBAPI, DLLIMPORT
// and DLLEXPORT before including this file.)
//
#ifndef CANLIBAPI
#   define CANLIBAPI __stdcall
#   define DLLIMPORT __declspec(dllimport)
#   define DLLEXPORT __declspec(dllexport)
#endif /* CANLIBAPI */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static void canInitializeLibrary();</b>
 *
 * \source_delphi   <b>procedure canInitializeLibrary;    </b>
 * \source_end
 * This function must be called before any other functions is used.  It will
 * initialize the driver.
 *
 * You may call \ref canInitializeLibrary() more than once. The actual
 * initialization will take place only once.
 *
 * Any errors encountered during library initialization will be "silent" and an
 * appropriate \ref canERR_xxx error code will be returned later on when
 * \ref canOpenChannel() (or any other API call that requires initialization) is
 * called.
 *
 * \sa \ref page_user_guide_init
 *
 */
void CANLIBAPI canInitializeLibrary(void);

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.canStatus canEnumHardwareEx(out Int32 channelCount);</b>
 *
 * \source_delphi   <b>function canEnumHardwareEx(var channelCount: Integer): canStatus;</b>
 * \source_end
 *
 * \param[out]  channelCount Number of channels present.
 *
 * This function will re-enumerate all currently available CAN channels while
 * not affecting already opened channel handles.
 *
 * \note When using this function, make sure your program does not keep any
 * references to CANlib channel numbers since these numbers may change.<br>
 * On Linux, no re-enumeration is needed since enumeration takes place
 * when a device is plugged in or unplugged.
 *
 * \sa \ref section_user_guide_enumerate_hw
 *
 */
canStatus CANLIBAPI canEnumHardwareEx(int *channelCount);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canClose(CanHandle handle);</b>
 *
 * \source_delphi   <b>function canClose(handle: canHandle): canStatus;</b>
 * \source_end
 *
 * Closes the channel associated with the handle. If no other threads
 * are using the CAN circuit, it is taken off bus. The handle can not be
 * used for further references to the channel, so any variable containing
 * it should be zeroed.
 *
 * \ref canClose() will almost always return \ref canOK; the specified handle is closed
 * on an best-effort basis.
 *
 * \param[in]  hnd  An open handle to a CAN channel.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_init_sel_channel_close
 * \sa \ref canOpenChannel(), \ref canBusOn(), \ref canBusOff()
 */
canStatus CANLIBAPI canClose(const CanHandle hnd);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canBusOn(CanHandle handle);</b>
 *
 * \source_delphi   <b>function canBusOn(handle: canHandle): canStatus;    </b>
 * \source_end
 *
 * Takes the specified channel on-bus.
 *
 * If you are using multiple handles to the same physical channel, for example
 * if you are writing a threaded application, you must call \ref canBusOn() once for
 * each handle. The same applies to \ref canBusOff() - the physical channel will not
 * go off bus until the last handle to the channel goes off bus.
 *
 * \param[in]  hnd  An open handle to a CAN channel.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_bus_on_off
 * \sa \ref canBusOff(), \ref canResetBus()
 *
 */
canStatus CANLIBAPI canBusOn(const CanHandle hnd);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canBusOff(CanHandle handle);</b>
 *
 * \source_delphi   <b>function canBusOff(handle: canHandle): canStatus; </b>
 * \source_end
 *
 * Takes the specified handle off-bus. If no other handle is active on the same
 * channel, the channel will also be taken off-bus
 *
 * \param[in]  hnd  An open handle to a CAN channel.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_bus_on_off
 * \sa \ref canBusOn(), \ref canResetBus()
 *
 */
canStatus CANLIBAPI canBusOff(const CanHandle hnd);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canSetBusParams(CanHandle handle, Int32 freq, Int32 tseg1, Int32 tseg2, Int32 sjw, Int32 noSamp); </b>
 *
 * \source_delphi   <b>function canSetBusParams(handle: canHandle; freq: Longint; tseg1, tseg2, sjw, noSamp, syncmode: Cardinal): canStatus;     </b>
 * \source_end
 *
 * This function sets the nominal bus timing parameters for the specified CAN
 * controller.
 *
 * The library provides default values for \a tseg1, \a tseg2, \a sjw and \a
 * noSamp when \a freq is specified to one of the pre-defined constants,
 * \ref canBITRATE_xxx for classic CAN and \ref canFD_BITRATE_xxx for CAN FD.
 *
 * If \a freq is any other value, no default values are supplied by the
 * library.
 *
 * If you are using multiple handles to the same physical channel, for example
 * if you are writing a threaded application, you must call \ref canBusOff() once
 * for each handle. The same applies to \ref canBusOn() - the physical channel will
 * not go off bus until the last handle to the channel goes off bus.
 *
 * \note The value of sjw should normally be less than tseg1 and tseg2.
 *
 * Use \ref canSetBusParamsC200() to set the bus timing parameters in the
 * ubiquitous 82c200 bit-timing register format.
 *
 * \param[in]  hnd       An open handle to a CAN controller.
 * \param[in]  freq      Bit rate (measured in bits per second); or one of the
 *                       predefined constants (\ref canBITRATE_xxx for classic
 *                       CAN and \ref canFD_BITRATE_xxx for CAN FD).
 * \param[in]  tseg1     Time segment 1, that is, the number of quanta from (but not
 *                       including) the Sync Segment to the sampling point.
 * \param[in]  tseg2     Time segment 2, that is, the number of quanta from the sampling
 *                       point to the end of the bit.
 * \param[in]  sjw       The Synchronization Jump Width.
 * \param[in]  noSamp    The number of sampling points; can be 1 or 3.
 * \param[in]  syncmode  Unsupported and ignored.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_init_bit_rate_can,
 * \ref section_user_guide_init_bit_rate_canfd,
 * \sa \ref canSetBusParamsFd(), \ref canSetBusParamsC200(), \ref canGetBusParams()
 * \sa \ref canSetBusParamsTq(), \ref canSetBusParamsFdTq(), \ref canGetBusParamsTq()
 *
 */
canStatus CANLIBAPI canSetBusParams(const CanHandle hnd,
                                    long freq,
                                    unsigned int tseg1,
                                    unsigned int tseg2,
                                    unsigned int sjw,
                                    unsigned int noSamp,
                                    unsigned int syncmode);

/**
 * \ingroup CAN
 *\b Constraints
 \verbatim
 Constraints that must be fulfilled when opening channel in classic CAN Mode:

   tq         = 1 + prop + phase1 + phase2
   tq        >= 3
   sjw       <= min(phase1, phase2)
   prescaler >= 1
 \endverbatim
*
 \verbatim
   Constraints that must be fulfilled when opening channel in CAN FD Mode:

   arbitration.tq         = 1 + arbitration.prop + arbitration.phase1 + arbitration.phase2
   arbitration.tq        >= 3
   arbitration.sjw       <= min(arbitration.phase1, arbitration.phase2)
   arbitration.prescaler >= 1  (<=2 will enable Transmitter Delay Compensation Mechanism)

   data.tq         = 1 + data.phase1 + data.phase2
   data.tq        >= 3
   data.sjw       <= min(data.phase1, data.phase2)
   data.prop       = 0
   data.prescaler  = arbitration.prescaler
 \endverbatim
*
* Used in \ref canSetBusParamsTq, \ref canSetBusParamsFdTq, \ref canGetBusParamsTq and \ref canGetBusParamsTq
*/
typedef struct kvBusParamsTq {
    int tq;        /**< Total bit time, in number of time quanta. */
    int phase1;    /**< Phase segment 1, in number of time quanta */
    int phase2;    /**< Phase segment 2, in number of time quanta */
    int sjw;       /**< Sync jump width, in number of time quanta */
    int prop;      /**< Propagation segment, in number of time quanta */
    int prescaler; /**< Prescaler */
} kvBusParamsTq;

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canSetBusParamsTq(CanHandle handle, kvBusParamsTq nominal);</b>
 *
 * \source_delphi   <b>function canSetBusParamsTq(hnd: canHandle; nominal: kvBusParamsTq): canStatus;</b>
 * \source_end
 *
 * Set bus parameters for the specified CAN controller (classic CAN).
 *
 * If the channel is opened (see \ref canOpenChannel()) with flags \ref canOPEN_CAN_FD or \ref canOPEN_CAN_FD_NONISO,
 * use \ref canSetBusParamsFdTq() instead.
 *
 * To get device specific limits of bus parameters, see \ref canCHANNELDATA_BUS_PARAM_LIMITS.
 *
 * The <a href="https://www.kvaser.com/support/calculators/can-fd-bit-timing-calculator/"> Kvaser Bit Timing calculator</a>,
 * available on the Kvaser website, can be used to calculate specific bit rates.
 *
 * \param[in] hnd      A handle to an open CAN circuit.
 * \param[in] nominal  See \ref kvBusParamsTq
 *
 *
 * return  canOK (zero)  if success
 * return  canERR_xxx    (negative) if failure
 */
 canStatus CANLIBAPI canSetBusParamsTq(const CanHandle hnd,
                                       const kvBusParamsTq nominal);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canSetBusParamsFd(CanHandle handle, Int32 freq_brs, Int32 tseg1_brs, Int32 tseg2_brs, Int32 sjw_brs);</b>
 *
 * \source_delphi   <b>function canSetBusParamsFd(hnd: canHandle; freq_brs: Longint; tseg1_brs, tseg2_brs, sjw_brs: Cardinal): canStatus;</b>
 * \source_end
 *
 * This function sets the data phase bus timing parameters for the specified
 * CAN controller.
 *
 * The library provides default values for \a tseg1_brs, \a tseg2_brs and
 * \a sjw_brs when \a freq_brs is specified to one of the pre-defined
 * constants, \ref canFD_BITRATE_xxx.
 *
 * If \a freq_brs is any other value, no default values are supplied
 * by the library.
 *
 * \param[in]  hnd        An open handle to a CAN controller.
 * \param[in]  freq_brs   CAN FD data bit rate (measured in bits per second); or
 *                        one of the predefined constants \ref
 *                        canFD_BITRATE_xxx.
 * \param[in]  tseg1_brs  Time segment 1, that is, the number of quanta from (but not
 *                        including) the Sync Segment to the sampling point.
 * \param[in]  tseg2_brs  Time segment 2, that is, the number of quanta from the sampling
 *                        point to the end of the bit.
 * \param[in]  sjw_brs    The Synchronization Jump Width.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canSetBusParamsFdTq(), \ref canGetBusParamsFdTq()
 */
canStatus CANLIBAPI canSetBusParamsFd(const CanHandle hnd,
                                      long freq_brs,
                                      unsigned int tseg1_brs,
                                      unsigned int tseg2_brs,
                                      unsigned int sjw_brs);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canSetBusParamsFdTq(CanHandle handle, kvBusParamsTq arbitration, kvBusParamsTq data);</b>
 *
 * \source_delphi   <b>function canSetBusParamsFdTq(hnd: canHandle; arbitration: kvBusParamsTq; data: kvBusParamsTq): canStatus;</b>
 * \source_end
 *
 * Set bus parameters for the specified CAN controller.
 *
 * The channel \b MUST be opened with flags \ref canOPEN_CAN_FD or \ref canOPEN_CAN_FD_NONISO, see \ref canOpenChannel().
 * In the case of classic can, use \ref canSetBusParamsTq() instead.
 *
 * To get device specific limits of bus parameters, see \ref canCHANNELDATA_BUS_PARAM_LIMITS.
 *
 * The <a href="https://www.kvaser.com/support/calculators/can-fd-bit-timing-calculator/"> Kvaser Bit Timing calculator</a>,
 * available on the Kvaser website, can be used to calculate specific bit rates.
 *
 * \param[in] hnd      A handle to an open CAN circuit.
 * \param[in] arbitration  See \ref kvBusParamsTq
 * \param[in] data     See \ref kvBusParamsTq
 *
 *
 * return  canOK (zero)  if success
 * return  canERR_xxx    (negative) if failure
 */
canStatus CANLIBAPI canSetBusParamsFdTq(const CanHandle     hnd,
                                        const kvBusParamsTq arbitration,
                                        const kvBusParamsTq data);


/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canGetBusParams(CanHandle handle, out Int64 freq, out Int32 tseg1, out Int32 tseg2, out Int32 sjw, out Int32 noSamp);</b>
 *
 * \source_delphi   <b>function canGetBusParams(handle: canHandle; var freq: Longint; var tseg1, tseg2, sjw, noSamp, syncmode: Cardinal): canStatus;     </b>
 * \source_end
 *
 * This function retrieves the current nominal bus parameters for the specified
 * channel.
 *
 * The anatomy of a CAN bit is discussed in detail at Kvaser's
 * web site at <a href="http://www.kvaser.com">www.kvaser.com</a>.
 *
 * \param[in]  hnd       An open handle to a CAN controller.
 * \param[out] freq      Bit rate (bits per second).
 * \param[out] tseg1     Time segment 1, that is, the number of quanta from (but not
 *                       including) the Sync Segment to the sampling point.
 * \param[out] tseg2     Time segment 2, that is, the number of quanta from the sampling
 *                       point to the end of the bit.
 * \param[out] sjw       The Synchronization Jump Width.
 * \param[out] noSamp    The number of sampling points; can be 1 or 3.
 * \param[out] syncmode  Unsupported, always read as one.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_init_bit_rate_can, \ref section_user_guide_init_bit_rate_can
 * \sa \ref canSetBusParams(), \ref canSetBusParamsC200()
 *
 */
canStatus CANLIBAPI canGetBusParams(const CanHandle hnd,
                                    long  *freq,
                                    unsigned int *tseg1,
                                    unsigned int *tseg2,
                                    unsigned int *sjw,
                                    unsigned int *noSamp,
                                    unsigned int *syncmode);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canGetBusParamsTq(CanHandle handle, out kvBusParamsTq nominal);</b>
 *
 * \source_delphi   <b>function canGetBusParamsTq(handle: canHandle; var nominal: kvBusParamsTq): canStatus;     </b>
 * \source_end
 *
 * Get bus parameters for the specified CAN controller.
 *
 * If the channel is opened (see \ref canOpenChannel()) with flags \ref canOPEN_CAN_FD or \ref canOPEN_CAN_FD_NONISO,
 * use \ref canGetBusParamsFdTq() instead.
 *
 * \param[in]  hnd      A handle to an open CAN circuit.
 * \param[out] nominal  See \ref kvBusParamsTq.
 *
 * return  canOK (zero)  if success
 * return  canERR_xxx    (negative) if failure
 */
canStatus CANLIBAPI canGetBusParamsTq(const CanHandle hnd,
                                      kvBusParamsTq *nominal);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canGetBusParamsFd(CanHandle handle, out Int64 freq_brs, out Int32 tseg1_brs, out Int32 tseg2_brs, out Int32 sjw_brs);</b>
 *
 * \source_delphi   <b>function canGetBusParamsFd(hnd: canHandle; var freq_brs: Longint; var tseg1_brs, tseg2_brs, sjw_brs: Cardinal): canStatus;</b>
 * \source_end
 *
 * This function retrieves the current data bus parameters for the specified
 * CAN FD channel.
 *
 * \param[in]  hnd         An open handle to a CAN FD controller.
 * \param[out] freq_brs    Bit rate (bits per second).
 * \param[out] tseg1_brs   Time segment 1, that is, the number of quanta from (but not
 *                         including) the Sync Segment to the sampling point.
 * \param[out] tseg2_brs   Time segment 2, that is, the number of quanta from the sampling
 *                         point to the end of the bit.
 * \param[out] sjw_brs     The Synchronization Jump Width.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 */
canStatus CANLIBAPI canGetBusParamsFd(const CanHandle hnd,
                                      long  *freq_brs,
                                      unsigned int *tseg1_brs,
                                      unsigned int *tseg2_brs,
                                      unsigned int *sjw_brs);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canGetBusParamsFdTq(CanHandle handle, out kvBusParamsTq nominal, out kvBusParamsTq data);</b>
 *
 * \source_delphi   <b>function canGetBusParamsFdTq(handle: canHandle; var nominal: kvBusParamsTq; var data: kvBusParamsTq): canStatus;     </b>
 * \source_end
 *
 * Get bus parameters for the specified CAN controller.
 *
 * If the channel is \b NOT opened (see \ref canOpenChannel()) with flags \ref canOPEN_CAN_FD or \ref canOPEN_CAN_FD_NONISO,
 * use \ref canGetBusParamsTq() instead.
 *
 * \param[in]  hnd      A handle to an open CAN circuit.
 * \param[out] nominal  See \ref kvBusParamsTq.
 * \param[out] data     See \ref kvBusParamsTq.
 *
 * return  canOK (zero)  if success
 * return  canERR_xxx    (negative) if failure
 */
canStatus CANLIBAPI canGetBusParamsFdTq(const CanHandle hnd,
                                        kvBusParamsTq *nominal,
                                        kvBusParamsTq *data);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canSetBusOutputControl(CanHandle handle, Int32 drivertype);</b>
 *
 * \source_delphi   <b>function canSetBusOutputControl(handle: canHandle; drivertype: Cardinal): canStatus;     </b>
 * \source_end
 *
 * This function sets the driver type for a CAN controller. This corresponds
 * loosely to the bus output control register in the CAN controller, hence the
 * name of this function. CANlib does not allow for direct manipulation of the
 * bus output control register; instead, symbolic constants are used to select
 * the desired driver type.
 *
 * \note Not all CAN driver types are supported on all cards.
 *
 * \param[in]  hnd         A handle to an open circuit.
 * \param[in]  drivertype  Can driver type, \ref canDRIVER_xxx)
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canGetBusOutputControl()
 */
canStatus CANLIBAPI canSetBusOutputControl(const CanHandle hnd,
                                           const unsigned int drivertype);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canGetBusOutputControl(CanHandle handle, out Int32 drivertype);</b>
 *
 * \source_delphi   <b>function canGetBusOutputControl(handle: canHandle; var drivertype: Cardinal): canStatus;  </b>
 * \source_end
 *
 * This function retrieves the current CAN controller driver type.
 * This corresponds loosely to the bus output control register in the
 * CAN controller, hence the name of this function. CANlib does not
 * allow for direct manipulation of the bus output control register;
 * instead, symbolic constants are used to select the desired driver
 * type.
 *
 * \note Don't confuse the CAN controller driver type with the bus driver
 *       type. The CAN controller is not connected directly to the CAN bus;
 *       instead, it is connected to a bus transceiver circuit which interfaces
 *       directly to the bus. The "CAN controller driver type" we are talking
 *       about here refers to the mode which the CAN controller uses to drive
 *       the bus transceiver circuit.
 *
 * \note Silent Mode is not supported by all CAN controllers.
 *
 * \param[in]  hnd         An open handle to a CAN circuit.
 * \param[out] drivertype  A pointer to an unsigned int which receives the
 *                         current driver type. The driver type can be either
 *                         \ref canDRIVER_NORMAL or \ref canDRIVER_SILENT.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canSetBusOutputControl()
 */
canStatus CANLIBAPI canGetBusOutputControl(const CanHandle hnd,
                                           unsigned int *drivertype);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canAccept(CanHandle handle, Int32 envelope, Int32 flag);</b>
 *
 * \source_delphi   <b>function canAccept(handle: canHandle; envelope: Longint; flag: Cardinal): canStatus;     </b>
 * \source_end
 *
 * This routine sets the message acceptance filters on a CAN channel.
 *
 * On some boards the acceptance filtering is done by the CAN hardware; on
 * other boards (typically those with an embedded CPU,) the acceptance
 * filtering is done by software. \ref canAccept() behaves in the same way for all
 * boards, however.
 *
 * \ref canSetAcceptanceFilter() and \ref canAccept() both serve the same purpose but the
 * former can set the code and mask in just one call.
 *
 * If you want to remove a filter, call \ref canAccept() with the mask set to 0.
 *
 * \note You can set the extended code and mask only on CAN boards that support
 *       extended identifiers.
 *
 * \note Acceptance filters for 11-bit and 29-bit (ext) are independent, meaning 11-bit filters
 *  will not affect 29-bit can frames and vice versa.
 *
 * \note Not all CAN boards support different masks for standard and extended
 *       CAN identifiers.
 *
 * \param[in]  hnd       An open handle to a CAN circuit.
 * \param[in]  envelope  The mask or code to set.
 * \param[in]  flag      Any of \ref canFILTER_SET_CODE_STD,
 *                       \ref canFILTER_SET_MASK_STD,
 *                       \ref canFILTER_SET_CODE_EXT or
 *                       \ref canFILTER_SET_MASK_EXT
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_filters
 * \sa \ref canSetAcceptanceFilter()
 */
canStatus CANLIBAPI canAccept(const CanHandle hnd,
                              const long envelope,
                              const unsigned int flag);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canReadStatus(CanHandle handle, out Int64 flags);</b>
 *
 * \source_delphi   <b>function canReadStatus(handle: canHandle; var flags: Cardinal): canStatus;     </b>
 * \source_end
 *
 * Returns the status for the specified circuit. flags points to a longword
 * which receives a combination of the \ref canSTAT_xxx flags.
 *
 * \note \ref canReadStatus() returns the latest known status of the specified
 *       circuit. If a status change happens precisely when \ref canReadStatus() is
 *       called, it may not be reflected in the returned result.
 *
 * \param[in]  hnd    A handle to an open circuit.
 * \param[out] flags  Pointer to a \c DWORD which receives the status flags;
 *                    this is a combination of any of the \ref canSTAT_xxx.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 */
canStatus CANLIBAPI canReadStatus(const CanHandle hnd,
                                  unsigned long *const flags);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canReadErrorCounters(CanHandle handle, out Int32 txErr, out Int32 rxErr, out Int32 ovErr);</b>
 *
 * \source_delphi   <b>function canReadErrorCounters(handle: canHandle; var txErr, rxErr, ovErr: Cardinal): canStatus;     </b>
 * \source_end
 *
 * Reads the error counters of the CAN controller.
 *
 * \ref canReadErrorCounters() returns the latest known values of the error counters
 * in the specified circuit. If the error counters change values precisely when
 * \ref canReadErrorCounters() is called, it may not be reflected in the returned
 * result.
 *
 * It is allowed to pass \c NULL as the value of the \a txErr, \a rxErr, and \a
 * ovErr parameters.
 *
 * Use \ref canIoCtl() to clear the counters.
 *
 * \note Not all CAN controllers provide access to the error counters;
 *       in this case, an educated guess is returned.
 *
 * \param[in]  hnd    A handle to an open circuit.
 * \param[out] txErr  A pointer to a \c DWORD which receives the transmit error
 *                    counter.
 * \param[out] rxErr  A pointer to a \c DWORD which receives the receive error
 *                    counter.
 * \param[out] ovErr  A pointer to a \c DWORD which receives the number of
 *                    overrun errors.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canIoCtl()
 */
canStatus CANLIBAPI canReadErrorCounters(const CanHandle hnd,
                                         unsigned int *txErr,
                                         unsigned int *rxErr,
                                         unsigned int *ovErr);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canWrite(CanHandle handle, Int32 id, Byte[] msg, Int32 dlc, Int32 flag);</b>
 *
 * \source_delphi   <b>function canWrite(handle: canHandle; id: Longint; msg: Pointer; dlc: Cardinal; flag: Cardinal): canStatus;     </b>
 * \source_end
 *
 * This function sends a CAN message. The call returns immediately after queuing
 * the message to the driver so the message has not necessarily been transmitted.
 *
 * If you are using the same channel via multiple handles, note that the
 * default behaviour is that the different handles will "hear" each other just as
 * if each handle referred to a channel of its own. If you open, say, channel 0
 * from thread A and thread B and then send a message from thread A, it will be
 * "received" by thread B.
 * This behaviour can be changed using \ref canIOCTL_SET_LOCAL_TXECHO.
 *
 * \note The message has been queued for transmission when this calls return.
 *       It has not necessarily been sent.
 *
 * \param[in]  hnd       A handle to an open CAN circuit.
 * \param[in]  id        The identifier of the CAN message to send.
 * \param[in]  msg       A pointer to the message data, or \c NULL.
 * \param[in]  dlc       The length of the message in bytes.<br>
                         For Classic CAN dlc can be at most 8, unless \ref canOPEN_ACCEPT_LARGE_DLC is used.<br>
                         For CAN FD dlc can be one of the following 0-8, 12, 16, 20, 24, 32, 48, 64.
 * \param[in]  flag      A combination of message flags, \ref canMSG_xxx
 *                       (including \ref canFDMSG_xxx if the CAN FD protocol is
 *                       enabled).
 *                       Use this parameter to send extended (29-bit) frames
 *                       and/or remote frames. Use \ref canMSG_EXT and/or
 *                       \ref canMSG_RTR for this purpose.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_sending
 * \sa \ref canWriteSync(), \ref canWriteWait()
 *
 */
canStatus CANLIBAPI canWrite(const CanHandle hnd,
                             long id,
                             void *msg,
                             unsigned int dlc,
                             unsigned int flag);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canWriteSync(CanHandle handle, Int64 timeout);</b>
 *
 * \source_delphi   <b>function canWriteSync(handle: canHandle; timeout: Cardinal): canStatus;     </b>
 * \source_end
 *
 * Waits until all CAN messages for the specified handle are sent, or the
 * timeout period expires.
 *
 * \param[in]  hnd       A handle to an open CAN circuit.
 * \param[in]  timeout   The timeout in milliseconds. 0xFFFFFFFF gives an
 *                       infinite timeout.
 *
 * \return \ref canOK (zero) if the queue emptied before the timeout period came to
 *         its end.
 * \return \ref canERR_TIMEOUT (negative) not all messages were transmitted when
 *         the timeout occurred.
 * \return \ref canERR_PARAM (negative) This could be caused by an erroneous
 *         parameter, or if you have turned TXACKs off (by using \ref canIoCtl())
 *         because if you do you can't use this call. The driver simply doesn't
 *         know when all the messages are sent!
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canWrite(), \ref canWriteWait()
 */
canStatus CANLIBAPI canWriteSync(const CanHandle hnd, unsigned long timeout);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canRead(CanHandle handle, out Int32 id, Byte[] msg, out Int32 dlc, out Int32 flag, out Int64 time);</b>
 *
 * \source_delphi   <b>function canRead(handle: canHandle; var id: Longint; msg: Pointer; var dlc: Cardinal; var flag: Cardinal; var time: Cardinal): canStatus;     </b>
 * \source_end
 *
 * Reads a message from the receive buffer. If no message is available, the
 * function returns immediately with return code \ref canERR_NOMSG.
 *
 * If you are using the same channel via multiple handles, note that the
 * default behaviour is that the different handles will "hear" each other just as
 * if each handle referred to a channel of its own. If you open, say, channel 0
 * from thread A and thread B and then send a message from thread A, it will be
 * "received" by thread B.
 * This behaviour can be changed using \ref canIOCTL_SET_LOCAL_TXECHO.
 *
 * It is allowed to pass \c NULL as the value of \a id, \a msg, \a dlc, \a
 * flag, and \a time.
 *
 * \param[in]  hnd   A handle to an open circuit.
 * \param[out] id    Pointer to a buffer which receives the CAN identifier.
 *                   This buffer will only get the identifier. To determine
 *                   whether this identifier was standard (11-bit) or extended
 *                   (29-bit), and/or whether it was remote or not, or if it
 *                   was an error frame, examine the contents of the flag
 *                   argument.
 * \param[out] msg   Pointer to the buffer which receives the message data. This
 *                   buffer must be large enough (i.e. 8 bytes for classic CAN
 *                   and up to 64 bytes for CAN FD).
 * \param[out] dlc   Pointer to a buffer which receives the message length.
 * \param[out] flag  Pointer to a buffer which receives the message flags,
 *                   which is a combination of the \ref canMSG_xxx (including
 *                   \ref canFDMSG_xxx if the CAN FD protocol is enabled) and
 *                   \ref canMSGERR_xxx values.
 * \param[out] time  Pointer to a buffer which receives the message time stamp.
 *                   The unit of the time stamp is configurable using
 *                   \ref canIOCTL_SET_TIMER_SCALE, default is 1 ms.
 *
 * \return \ref canOK (zero) if a message was read.
 * \return \ref canERR_NOMSG (negative) if there was no message available.
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_reading, \ref
 * page_user_guide_send_recv_mailboxes, \ref
 * section_user_guide_send_recv_reading
 * page_user_guide_time Time Measurement
 *
 * \sa \ref canReadSpecific(), \ref canReadSpecificSkip(), \ref canReadSync(),
 *     \ref canReadSyncSpecific(), \ref canReadWait(), \ref canIoCtl()
 *
 */
canStatus CANLIBAPI canRead(const CanHandle hnd,
                            long *id,
                            void *msg,
                            unsigned int *dlc,
                            unsigned int *flag,
                            unsigned long *time);
/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canReadWait(CanHandle handle, out Int32 id, Byte[] msg, out Int32 dlc, out Int32 flag, out Int64 time, Int64 timeout);</b>
 *
 * \source_delphi   <b>function canReadWait(handle: canHandle; var id: Longint; msg: Pointer; var dlc: Cardinal; var flag: Cardinal; var time: Cardinal; timeout: Cardinal): canStatus;     </b>
 * \source_end
 *
 * Reads a message from the receive buffer. If no message is available, the
 * function waits until a message arrives or a timeout occurs.
 *
 * If you are using the same channel via multiple handles, note that the
 * default behaviour is that the different handles will "hear" each other just as
 * if each handle referred to a channel of its own. If you open, say, channel 0
 * from thread A and thread B and then send a message from thread A, it will be
 * "received" by thread B.
 * This behaviour can be changed using \ref canIOCTL_SET_LOCAL_TXECHO.
 *
 * It is allowed to pass \c NULL as the value of \a id, \a msg, \a dlc, \a
 * flag, and \a time.
 *
 * \param[in]   hnd    A handle to an open circuit.
 * \param[out]  id     Pointer to a buffer which receives the CAN identifier.
 *                     This buffer will only get the identifier. To determine
 *                     whether this identifier was standard (11-bit) or extended
 *                     (29-bit), and/or whether it was remote or not, or if it
 *                     was an error frame, examine the contents of the flag
 *                     argument.
 * \param[out]  msg    Pointer to the buffer which receives the message data.
 *                     This buffer must be large enough (i.e. 8 bytes for
 *                     classic CAN and up to 64 bytes for CAN FD).
 * \param[out]  dlc    Pointer to a buffer which receives the message length.
 * \param[out]  flag   Pointer to a buffer which receives the message flags,
 *                     which is a combination of the \ref canMSG_xxx (including
 *                     \ref canFDMSG_xxx if the CAN FD protocol is enabled) and
 *                     \ref canMSGERR_xxx values.
 * \param[out] time    Pointer to a buffer which receives the message time stamp.
 *                     The unit of the time stamp is configurable using
 *                     \ref canIOCTL_SET_TIMER_SCALE, default is 1 ms.
 * \param[in]  timeout If no message is immediately available, this parameter
 *                     gives the number of milliseconds to wait for a message
 *                     before returning. 0xFFFFFFFF gives an infinite timeout.
 *
 * \return \ref canOK (zero) if a message was read.
 * \return \ref canERR_NOMSG (negative) if there was no message available.
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canRead(), \ref canReadSpecific(), \ref canReadSpecificSkip(),
 *  \ref canReadSyncSpecific(), \ref canReadSync(), \ref canIoCtl()
 *
 * \sa \ref page_user_guide_time Time Measurement
 */
canStatus CANLIBAPI canReadWait(const CanHandle hnd,
                                long *id,
                                void *msg,
                                unsigned int  *dlc,
                                unsigned int  *flag,
                                unsigned long *time,
                                unsigned long timeout);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canReadSpecific(CanHandle handle, Int32 id, Byte[] msg, out Int32 dlc, out Int32 flag, out Int64 time);</b>
 *
 * \source_delphi   <b>function canReadSpecific(handle: canHandle; id: Longint; msg: Pointer; var dlc: Cardinal; var flag: Cardinal; var time: Cardinal): canStatus;     </b>
 * \source_end
 *
 * Reads a message with a specified identifier from the receive buffer. Any
 * preceding message not matching the specified identifier will be kept
 * in the receive buffer. If no message with the specified identifier is
 * available, the function returns immediately with an error code.
 *
 * If you are using the same channel via multiple handles, note that the
 * default behaviour is that the different handles will "hear" each other just as
 * if each handle referred to a channel of its own. If you open, say, channel 0
 * from thread A and thread B and then send a message from thread A, it will be
 * "received" by thread B.
 * This behaviour can be changed using \ref canIOCTL_SET_LOCAL_TXECHO.
 *
 * It is allowed to pass \c NULL as the value of \a msg, \a dlc, \a
 * flag, and \a time.
 *
 * \note Use \ref canReadSpecific only if you for some reason must keep the unread
 *       messages in the queue. If this is not the case, consider using
 *       \ref canReadSpecificSkip() or \ref canRead() for better performance.
 *
 * \param[in]   hnd    A handle to an open circuit.
 * \param[in]   id     The desired CAN identifier.
 * \param[out]  msg    Pointer to the buffer which receives the message data.
 *                     This buffer must be large enough (i.e. 8 bytes for
 *                     classic CAN and up to 64 bytes for CAN FD).
 * \param[out]  dlc    Pointer to a buffer which receives the message length.
 * \param[out]  flag   Pointer to a buffer which receives the message flags,
 *                     which is a combination of the \ref canMSG_xxx (including
 *                     \ref canFDMSG_xxx if the CAN FD protocol is enabled) and
 *                     \ref canMSGERR_xxx values.
 * \param[out] time    Pointer to a buffer which receives the message time stamp.
 *
 * \return \ref canOK (zero) if a message was read.
 * \return \ref canERR_NOMSG (negative) if a matching message was not found.
 *         There might be other messages in the queue, though.
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref page_user_guide_send_recv_mailboxes, \ref
 * section_user_guide_send_recv_reading, \ref
 * page_user_guide_time Time Measurement
 * \sa \ref canRead(), \ref canReadSpecificSkip(), \ref canReadSync(), \ref canReadSyncSpecific(),
 * \ref canReadWait()
 *
 */
canStatus CANLIBAPI canReadSpecific(const CanHandle hnd, long id, void *msg,
                                    unsigned int *dlc, unsigned int *flag,
                                    unsigned long *time);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canReadSync(CanHandle handle, Int64 timeout);</b>
 *
 * \source_delphi   <b>function canReadSync(handle: canHandle; timeout: Cardinal): canStatus;     </b>
 * \source_end
 *
 * Waits until the receive buffer contains at least one message or a timeout
 * occurs.
 *
 * If you are using the same channel via multiple handles, note that the
 * default behaviour is that the different handles will "hear" each other just as
 * if each handle referred to a channel of its own. If you open, say, channel 0
 * from thread A and thread B and then send a message from thread A, it will be
 * "received" by thread B.
 * This behaviour can be changed using \ref canIOCTL_SET_LOCAL_TXECHO.
 *
 * \param[in]  hnd      A handle to an open circuit.
 * \param[in]  timeout  The timeout in milliseconds. 0xFFFFFFFF gives an
 *                      infinite timeout.
 *
 * \return \ref canOK (zero) if the queue contains the desired message.
 * \return \ref canERR_TIMEOUT (negative) if a timeout occurs before a message
 *         arrived.
 * \return \ref canERR_xxx (negative) if the call fails.
 *
 * \sa \ref canRead(), \ref canReadSpecific(), \ref canReadSpecificSkip(),
 * \ref canReadSyncSpecific(), \ref canReadWait()
 */
canStatus CANLIBAPI canReadSync(const CanHandle hnd, unsigned long timeout);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canReadSyncSpecific(CanHandle handle, Int32 id, Int64 timeout);</b>
 *
 * \source_delphi   <b>function canReadSyncSpecific(handle: canHandle; id: Longint; timeout: Cardinal): canStatus;  </b>
 * \source_end
 *
 * Waits until the receive queue contains a message with the specified id, or a
 * timeout occurs..
 *
 * If you are using the same channel via multiple handles, note that the
 * default behaviour is that the different handles will "hear" each other just as
 * if each handle referred to a channel of its own. If you open, say, channel 0
 * from thread A and thread B and then send a message from thread A, it will be
 * "received" by thread B.
 * This behaviour can be changed using \ref canIOCTL_SET_LOCAL_TXECHO.
 *
 * \param[in]  hnd      A handle to an open circuit.
 * \param[in]  id       The desired message identifier.
 * \param[in]  timeout  The timeout in milliseconds. 0xFFFFFFFF gives an
 *                      infinite timeout.
 *
 * \return \ref canOK (zero) if the queue contains the desired message.
 * \return \ref canERR_TIMEOUT (negative) if a timeout occurs before the specified
 *         message arrived.
 * \return \ref canERR_xxx (negative) if the call fails.
 *
 * \sa \ref canRead(), \ref canReadSpecific(), \ref canReadSpecificSkip(),
 * \ref canReadSync(), \ref canReadWait()
 */
canStatus CANLIBAPI canReadSyncSpecific(const CanHandle hnd,
                                        long id,
                                        unsigned long timeout);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canReadSpecificSkip(CanHandle hnd, Int32 id, Byte[] msg, out Int32 dlc, out Int32 flag, out Int64 time);</b>
 *
 * \source_delphi   <b>function canReadSpecificSkip(handle: canHandle; id: Longint; msg: Pointer; var dlc: Cardinal; var flag: Cardinal; var time: Cardinal): canStatus;     </b>
 * \source_end
 *
 * Reads a message with a specified identifier from the receive buffer. Any
 * preceding message not matching the specified identifier will be removed
 * in the receive buffer. If no message with the specified identifier is
 * available, the function returns immediately with an error code.
 *
 * If you are using the same channel via multiple handles, note that the
 * default behaviour is that the different handles will "hear" each other just as
 * if each handle referred to a channel of its own. If you open, say, channel 0
 * from thread A and thread B and then send a message from thread A, it will be
 * "received" by thread B.
 * This behaviour can be changed using \ref canIOCTL_SET_LOCAL_TXECHO.
 *
 * It is allowed to pass \c NULL as the value of \a msg, \a dlc, \a
 * flag, and \a time.
 *
 * \param[in]   hnd    A handle to an open circuit.
 * \param[in]   id     The desired CAN identifier.
 * \param[out]  msg    Pointer to the buffer which receives the message data.
 *                     This buffer must be large enough (i.e. 8 bytes for
 *                     classic CAN and up to 64 bytes for CAN FD).
 * \param[out]  dlc    Pointer to a buffer which receives the message length.
 * \param[out]  flag   Pointer to a buffer which receives the message flags,
 *                     which is a combination of the \ref canMSG_xxx (including
 *                     \ref canFDMSG_xxx if the CAN FD protocol is enabled) and
 *                     \ref canMSGERR_xxx values.
 * \param[out] time    Pointer to a buffer which receives the message time stamp.
 *
 * \return \ref canOK (zero) if a matching message was found.
 * \return \ref canERR_NOMSG if there was no matching message available. All other
           messages (if any!) were discarded.
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_reading, \ref
 * page_user_guide_time Time Measurement
 * \sa \ref canRead(), \ref canReadSpecific(), \ref canReadSync(),
 * \ref canReadSyncSpecific(), \ref canReadWait()
 */
canStatus CANLIBAPI canReadSpecificSkip(const CanHandle hnd,
                                        long id,
                                        void *msg,
                                        unsigned int *dlc,
                                        unsigned int *flag,
                                        unsigned long *time);


/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canSetNotify(CanHandle handle, IntPtr win_handle, Int32 aNotifyFlags);</b>
 *
 * \source_delphi   <b>function canSetNotify(handle: canHandle; aHWnd: HWND; aNotifyFlags: Cardinal): canStatus;     </b>
 * \source_end
 *
 * This function associates a window handle with the CAN circuit. A
 * \c WM__CANLIB message is sent to that window when certain events
 * (specified by the \ref canNOTIFY_xxx flags) occur.
 *
 * When an event take place, a \c WM__CANLIB message will be sent to the window
 * whose handle is aHWnd. This \c WM__CANLIB message will have:
 *
 * \li \c WPARAM handle to the circuit where the event occurred
 * \li \c HIWORD(LPARAM) 0
 * \li \c LOWORD(LPARAM) \ref canEVENT_xxx
 *
 * In the routine that handles \c WM__CANLIB, you can call the CANlib API
 * functions (for example, \ref canRead()) using the handle found in \c wParam.
 *
 * \param[in]  hnd          A handle to an open CAN circuit.
 * \param[in] aHWnd         Handle of the window which will receive the
 *                          notification messages.
 * \param[in] aNotifyFlags  The events specified with \ref canNOTIFY_xxx, for
 *                          which callback should be called.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \n In the routine that handles \c WM__CANLIB, you must call \ref canRead() repeatedly
 *    until it returns \ref canERR_NOMSG, regardless of the \c LPARAM value. This will
 *    flush the driver's internal event queues. If you fail to do this, no more
 *    events will be reported.
 *
 * \sa \ref section_user_guide_send_recv_asynch
 */
canStatus CANLIBAPI canSetNotify(const CanHandle hnd,
                                 HWND aHWnd,
                                 unsigned int aNotifyFlags);


/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canTranslateBaud(int bitrate, out Int32 freq, out Int32 tseg1, out Int32 tseg2, out Int32 sjw, out Int32 nosamp);</b>
 *
 * \source_delphi   <b>function canTranslateBaud(var freq: Longint; var tseg1, tseg2, sjw, noSamp, syncMode: Cardinal): canStatus;     </b>
 * \source_end
 *
 * This function translates the \ref canBITRATE_xxx constants to their corresponding
 * bus parameter values. At return, this \a freq contains the actual bit rate
 * (in bits per second). \a TSeg1 is the number of quanta (less one) in a bit
 * before the sampling point. \a TSeg2 is the number of quanta after the
 * sampling point.
 *
 * \param[in,out] freq      A pointer to a \c DWORD which contains the \ref canBITRATE_xxx
 *                          constant to translate. The bitrate constant value is
 *                          overwritten with the frequency value.
 * \param[out]    tseg1     A pointer to a buffer which receives the Time segment 1,
 *                          that is, the number of quanta from (but not including)
 *                          the Sync Segment to the sampling point.
 * \param[out]    tseg2     A pointer to a buffer which receives the Time segment 2,
 *                          that is, the number of quanta from the sampling point to
 *                          the end of the bit.
 * \param[out]    sjw       A pointer to a buffer which receives the Synchronization
 *                          Jump Width.
 * \param[out]    nosamp    A pointer to a buffer which receives the number of
 *                          sampling points.
 * \param[out]    syncMode  Unsupported, always read as zero. May be set to NULL.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canSetBusParams()
 */
canStatus CANLIBAPI canTranslateBaud(long *const freq,
                                     unsigned int *const tseg1,
                                     unsigned int *const tseg2,
                                     unsigned int *const sjw,
                                     unsigned int *const nosamp,
                                     unsigned int *const syncMode);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus Canlib.kvBitrateToBusParamsTq(CanHandle hnd, Int32 freqA, out kvBusParamsTq nominal);</b>
 *
 * \source_delphi   <b>function kvBitrateToBusparamsTq(handle: canHandle; const freq: Integer; var nominal: kvBusParamsTq): canStatus;</b>
 * \source_end
 *
 * This function translates the \ref canBITRATE_xxx constants to corresponding
 * bus parameter values. At return \a nominal contains parameter values for the
 * specified \a freq. If the channel is opened in CAN FD mode, see \ref kvBitrateToBusParamsFdTq().
 *
 * \param[in]     hnd       A handle to an open circuit.
 * \param[in]     freq      Bitrate constant, \ref canBITRATE_xxx.
 * \param[in,out] nominal   A pointer to a \ref kvBusParamsTq, upon return receives
 *                          the bus busparameters specified by \a freq.
 *
 * \return \ref canOK (zero) is success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canSetBusParamsTq(), \ref kvBitrateToBusParamsFdTq()
 */
canStatus CANLIBAPI kvBitrateToBusParamsTq(const canHandle hnd,
                                           int freq,
                                           kvBusParamsTq *nominal);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus Canlib.kvBitrateToBusParamsFdTq(CanHandle hnd, Int32 freqA, Int32 freqD, out kvBusParamsTq arbitration, out kvBusParamsTq data);</b>
 *
 * \source_delphi   <b>function kvBitrateToBusParamsFdTq (handle: canHandle; const freqA, freqD: Integer; var arbitration, data: kvBusParamsTq): canStatus;</b>
 *
 * This function translates the \ref canFD_BITRATE_xxx constants to corresponding
 * bus parameter values. At return \a arbitration contains parameter values for the
 * specified \a freqA, and \a data contains parameter values for the specified
 * \a freqD. If the channel is opened in classic CAN mode see \ref kvBitrateToBusParamsTq().
 *
 * \param[in]     hnd         A handle to an open circuit.
 * \param[in]     freqA       Bitrate constant, \ref canFD_BITRATE_xxx.
 * \param[in]     freqD       Bitrate constant, \ref canFD_BITRATE_xxx.
 * \param[in,out] arbitration A pointer to a \ref kvBusParamsTq, upon return receives
 *                            the bus busparameters specified by \a freqA.
 * \param[in,out] data        A pointer to a \ref kvBusParamsTq, upon return receives
 *                            the bus busparameters specified by \a freqD.
 *
 * \return \ref canOK (zero) is success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canSetBusParamsFdTq(), \ref kvBitrateToBusParamsTq()
 */
canStatus CANLIBAPI kvBitrateToBusParamsFdTq(const canHandle hnd,
                                             int freqA,
                                             int freqD,
                                             kvBusParamsTq *arbitration,
                                             kvBusParamsTq *data);

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.canStatus canGetErrorText(Canlib.canStatus err, out string buf_str);</b>
 *
 * \source_delphi   <b>function canGetErrorText(err: canStatus; buf: PAnsiChar; bufsiz: Cardinal): canStatus;     </b>
 * \source_end
 *
 * This function translates an error code (\ref canERR_xxx)
 * to a human-readable, English text.
 *
 * \param[in]     err     The error code.
 * \param[out]    buf     The buffer which is to receive the text, which is a
 *                        zero-terminated string (provided the buffer is large enough.)
 * \param[in]     bufsiz  The length of the input buffer.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_canstatus
 *
 */
canStatus CANLIBAPI canGetErrorText(canStatus err, char *buf, unsigned int bufsiz);

/**
 * \ingroup  can_general
 *
 * \source_cs       <b>static Int16 canGetVersion();</b>
 *
 * \source_delphi   <b>function canGetVersion: Word;     </b>
 * \source_end
 *

 * This API call returns the version of the CANlib API DLL (canlib32.dll).  The
 * most significant byte is the major version number and the least significant
 * byte is the minor version number.
 *
 * The actual version of the different driver files can be obtained by studying
 * the version resources in each of the files.
 *
 * \note The version number of the canlib32.dll file is not related to the
 *       product version of CANlib you are using. CANlib consists of several
 *       driver and DLL files. To obtain the product version, use
 *       \ref canGetVersionEx().
 *
 * \return version number of canlib32.dll
 *
 * \note Linux returns version number from libcanlib.so
 *
 * \sa \ref page_user_guide_version
 * \sa \ref canGetVersionEx()
 *     \ref canProbeVersion()
 *
 */
unsigned short CANLIBAPI canGetVersion(void);

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.canStatus canIoCtl(CanHandle handle, Int32 func, out Int32 val);<br>
                       static Canlib.canStatus canIoCtl(CanHandle handle, Int32 func, out String str_buf);<br>
                       static Canlib.canStatus canIoCtl(CanHandle handle, Int32 func, ref object obj_buf);</b>
 *
 * \source_delphi   <b>function canIoCtl(handle: canHandle; func: Cardinal; buf: Pointer; buflen: Cardinal): canStatus;     </b>
 * \source_end
 *
 * This API call performs several different functions (\ref canIOCTL_xxx). The
 * functions are handle-specific unless otherwise noted; this means that they
 * affect only the handle you pass to \ref canIoCtl(), whereas other open
 * handles will remain unaffected.  The contents of \a buf after the call is
 * dependent on the function code you specified.
 *
 * \param[in]     hnd     A handle to an open circuit.
 * \param[in]     func    A \ref canIOCTL_xxx function code
 * \param[in,out] buf     Pointer to a buffer containing function-dependent data;
                          or a \c NULL pointer for certain function codes. The
                          buffer can be used for both input and output
                          depending on the function code. See \ref canIOCTL_xxx.
 * \param[in]     buflen  The length of the buffer.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 */
canStatus CANLIBAPI canIoCtl(const CanHandle hnd,
                             unsigned int func,
                             void *buf,
                             unsigned int buflen);


/**
 * \ingroup Obsolete
 *
 * \source_delphi   <b>function canReadTimer(handle: canHandle): Longint;     </b>
 * \source_end
 *
 * \warning Obsolete! \ref kvReadTimer() should be used instead.
 */
unsigned long CANLIBAPI canReadTimer(const CanHandle hnd);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static CanHandle canOpenChannel(Int32 channel, Int32 flags);</b>
 *
 * \source_delphi   <b>function canOpenChannel(channel: Integer; flags: Integer): canHandle;     </b>
 * \source_end
 *
 * Opens a CAN channel (circuit) and returns a handle which is used
 * in subsequent calls to CANlib.
 *
 *
 * Channel numbering is dependent on the installed hardware. The first channel
 * always has number 0.
 *
 * For example,
 *
 * \li If you have a single LAPcan, the channels are numbered 0 and 1.
 *
 * \li If you have a USBcan Professional, the channels are numbered 0-1
 *     according to the labels on the cables.
 *
 * \li The virtual channels come after all physical channels.
 *
 * If you are using multiple threads, note that the returned handle is usable
 * only in the context of the thread that created it. That is, you must call
 * \ref canOpenChannel() in each of the threads in your application that uses the
 * CAN bus. You can open the same channel from multiple threads, but you must
 * call \ref canOpenChannel() once per thread.
 *
 * If you are using the same channel via multiple handles, note that the
 * default behaviour is that the different handles will "hear" each other just as
 * if each handle referred to a channel of its own. If you open, say, channel 0
 * from thread A and thread B and then send a message from thread A, it will be
 * "received" by thread B.
 * This behaviour can be changed using \ref canIOCTL_SET_LOCAL_TXECHO.
 *
 * \note The handle returned may be zero which is perfectly valid.
 *
 * \param[in]  channel  The number of the channel. Channel numbering is hardware
 *                      dependent.
 * \param[in]  flags    A combination of \ref canOPEN_xxx flags
 *
 * \return Returns a handle to the opened circuit, or \ref canERR_xxx
 *         (negative) if the call failed.
 *
 * \sa \ref page_user_guide_chips_channels,  \ref section_user_guide_virtual
 * \sa \ref canClose(), \ref canGetNumberOfChannels(), \ref canGetChannelData(), \ref canIoCtl()
 *
 */
CanHandle CANLIBAPI canOpenChannel(int channel, int flags);

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.canStatus canGetNumberOfChannels(out Int32 channelCount);</b>
 *
 * \source_delphi   <b>function canGetNumberOfChannels(var channelCount: Integer): canStatus;     </b>
 * \source_end
 *
 * This function returns the number of available CAN channels in the
 * computer. The virtual channels are included in this number.
 *
 * \param[out] channelCount  A pointer to a \c DWORD which will receive the current
 *                           number of channels.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_unique_device, \ref section_user_guide_virtual
 * \sa \ref canGetChannelData()
 */
canStatus CANLIBAPI canGetNumberOfChannels(int *channelCount);


/**
 * \name kvREMOTE_TYPExxx
 * \anchor kvREMOTE_TYPExxx
 *
 * Remote type, returned when using \ref canCHANNELDATA_REMOTE_TYPE
 * @{
 */
#define kvREMOTE_TYPE_NOT_REMOTE  0 ///<
#define kvREMOTE_TYPE_WLAN        1 ///<
#define kvREMOTE_TYPE_LAN         2 ///<
/** @} */

/**
 * \name kvLOGGER_TYPE_xxx
 * \anchor kvLOGGER_TYPE_xxx
 *
 * Logger type, returned when using \ref canCHANNELDATA_LOGGER_TYPE
 * @{
 */
#define kvLOGGER_TYPE_NOT_A_LOGGER  0 ///<
#define kvLOGGER_TYPE_V1            1 ///<
#define kvLOGGER_TYPE_V2            2 ///<
/** @} */


/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.canStatus canGetChannelData(Int32 channel, Int32 item, out object buffer);</b>
 *
 * \source_delphi   <b>function canGetChannelData(channel, item: Integer; var buffer; bufsize: size_t): canStatus;     </b>
 * \source_end
 *
 * This function can be used to retrieve certain pieces of information about a channel.
 *
 * \note You must pass a channel number and not a channel handle.
 *
 * \param[in]  channel  The number of the channel you are interested in. Channel
 *                        numbers are integers in the interval beginning at 0
 *                        (zero) and ending at the value returned by
 *                        \ref canGetNumberOfChannels() minus 1.
 * \param[in]  item  This parameter specifies what data to obtain for the
 *                        specified channel. The value is one of the constants
 *                        \ref canCHANNELDATA_xxx.
 * \param[out] buffer     The address of a buffer which is to receive the data.
 * \param[in]  bufsize    The size of the buffer to which the buffer parameter
 *                        points.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_unique_device
 * \sa \ref canGetNumberOfChannels()
 * \sa \ref canGetHandleData()
 */
canStatus CANLIBAPI canGetChannelData(int channel,
                                      int item,
                                      void *buffer,
                                      size_t bufsize);

/**
 * \ingroup can_general
 * \anchor canCHANNELDATA_xxx
 * \name canCHANNELDATA_xxx
 *
 * These defines are used in \ref canGetChannelData() and \ref canGetHandleData().
 *
 *  @{
 */

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer that receives the
   * capabilities of the CAN controller; this is a combination of the \ref
   * canCHANNEL_CAP_xxx flags.
   */
#define canCHANNELDATA_CHANNEL_CAP                1

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer that receives the
   * capabilities of the CAN transceiver; this is a combination of the
   * \ref canDRIVER_CAP_xxx flags.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_TRANS_CAP                  2

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer that receives
   * a combination of \ref canCHANNEL_IS_xxx flags.
   *
   */
#define canCHANNELDATA_CHANNEL_FLAGS              3   // available, etc

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer that receives the hardware
   * type of the card. This value is any one of the \ref canHWTYPE_xxx
   * constants.
   */
#define canCHANNELDATA_CARD_TYPE                  4

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer that receives the card's
   * number in the computer. Each card type is numbered separately. For
   * example, the first LAPcan card in a machine will have number 0, the second
   * LAPcan number 1, etc.
   */
#define canCHANNELDATA_CARD_NUMBER                5

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer which receives the channel
   * number on the card.
   */
#define canCHANNELDATA_CHAN_NO_ON_CARD            6

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 64-bit (8 bytes) area which receives the serial
   * number of the card. If the card doesn't have a serial number, 0 is
   * returned. The serial number is an 8-byte unsigned integer. Currently, no
   * products are using all 8 bytes; at most 4 bytes are used.
   */
#define canCHANNELDATA_CARD_SERIAL_NO             7

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 64-bit (8 bytes) area which receives the serial
   * number of the transceiver. The serial number is an 8-byte unsigned
   * integer. If the transceiver doesn't have a serial number, 0 is returned.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_TRANS_SERIAL_NO            8

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 64-bit (8 bytes) area which receives the firmware
   * revision number on the card. This number consists of four 16-bit words:
   * the major revision, the minor revision, the release number and the build
   * number, listed in order from the most significant to the least
   * significant.
   */
#define canCHANNELDATA_CARD_FIRMWARE_REV          9

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 64-bit (8 bytes) area which receives the hardware
   * revision number on the card. This number consists of four 16-bit words;
   * the two most significant are always 0, and the two least significant are
   * the major revision and the minor revision, listed in order from the most
   * significant to the least significant.
   */
#define canCHANNELDATA_CARD_HARDWARE_REV          10

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 8-byte area which receives the UPC (EAN) number for
   * the card. If there is no UPC number, the buffer is filled with zeros. The
   * UPC (EAN) number is coded as a BCD string with the LSB first, so
   * e.g. 73-30130-00122-0 is coded as 0x30001220 0x00073301.
   */
#define canCHANNELDATA_CARD_UPC_NO                11

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 8-byte area which receives the UPC (EAN) number for
   * the transceiver. If there is no UPC number, the buffer is filled with
   * zeros. The UPC (EAN) number is coded as a BCD string with the LSB first,
   * so e.g. 73-30130-00122-0 is coded as 0x30001220 0x00073301.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_TRANS_UPC_NO               12

  /**
   * \deprecated Use \ref canCHANNELDATA_DEVDESCR_ASCII instead.
   *
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to an area which receives a zero-terminated string with a
   * clear-text name of the channel.
   *
   * \note Use of this item code is no longer recommended. The returned
   * channel name doesn't contain the exact hardware type (it just contains
   * the device family) and uses zero-based channel numbering, which is not
   * user friendly.  Instead, use e.g. \ref canCHANNELDATA_DEVDESCR_ASCII and
   * \ref canCHANNELDATA_CHAN_NO_ON_CARD to build your own channel name.
   *
   * \sa \ref canCHANNELDATA_DEVNAME_ASCII
   */
#define canCHANNELDATA_CHANNEL_NAME               13

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to an array of 4 16-bit unsigned integers which receives
   * the file version number of the second-level DLL driver file, i.e. the DLL
   * that interfaces between canlib32.dll and the driver proper.
   *
   * Contents depening on index:
   *
   * \li 0: 0
   * \li 1: The build number
   * \li 2: The minor revision number
   * \li 3: The major revision number
   */
#define canCHANNELDATA_DLL_FILE_VERSION          14

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to an array of 4 16-bit unsigned integers which receives
   * the product version number of the second-level DLL driver file, i.e. the
   * DLL that interfaces between canlib32.dll and the driver proper.
   *
   * Contents depending on index:
   *
   * \li 0: 0
   * \li 1: 1
   * \li 2: The minor revision number
   * \li 3: The major revision number
   */
#define canCHANNELDATA_DLL_PRODUCT_VERSION       15

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer which receives a number that
   * identifies the second-level DLL driver file, i.e. the DLL that interfaces
   * between canlib32.dll and the driver proper.
   *
   * Values:
   *
   * \li 1: kvalapw.dll - used with CANlib up to 2.29.
   *
   * \li 2: kvalapw2.dll - used with CANlib from 3.0 and on.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_DLL_FILETYPE              16

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer which receives the CAN
   * transceiver type of the specified channel.  This value is one of the
   * \ref canTRANSCEIVER_TYPE_xxx
   */
#define canCHANNELDATA_TRANS_TYPE                17

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer which receives an address
   * indicating where the device is located on its underlying bus. The
   * interpretation of this number is bus-specific. If the address is unknown
   * or the bus driver does not support an address, the bus driver leaves this
   * member at its default value of 0xFFFFFFFF.
   *
   * The following list describes the information certain bus drivers store in
   * the Address field for their child devices:
   *
   * \li ISA: Does not supply an address. Defaults to 0xFFFFFFFF.
   *
   * \li PC Card (PCMCIA): The socket number (typically 0x00 or 0x40)
   *
   * \li PCI: The device number in the high word and the function number in the
   *          low word.
   *
   * \li USB: The port number.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_DEVICE_PHYSICAL_POSITION  18

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer which receives a number
   * associated with the device that can be displayed in the user
   * interface. This number is typically a user-perceived slot number, such as
   * a number printed next to the slot on the board, or some other number that
   * makes locating the physical device easier for the user. For buses with no
   * such convention, or when the UI number is unknown, 0xFFFFFFFF is returned.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_UI_NUMBER                 19

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer which is set to 0, if the
   * legacy time synchronization is not currently enabled for the specified
   * channel, and 1, if the legacy time synchronization is currently enabled
   * for the specified channel.
   *
   * Legacy time synchronization is a mechanism that will keep the PC and CAN
   * channel clocks in sync. The synchronization is done in the driver, which
   * periodically calculates the difference between the PC clock and the CAN
   * device clock and compensates for the clock drift by recalculating the CAN
   * message time stamps. You need to enable clock synchronization in the
   * Control Panel using the Kvaser Hardware applet.
   *
   * \note Legacy time synchronization is implemented only on LAPcan and LAPcan
   * II. It is not related to Kvaser MagiSync&tm; which is implemented in the
   * high-end members of the Kvaser Leaf family. Kvaser MagiSync&tm; is always
   * enabled and allows for much more accurate time synchronization.
   * \note Not implemented in linux.
   *
   */
#define canCHANNELDATA_TIMESYNC_ENABLED          20

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to an array of four 16-bit unsigned integers which
   * receives the file version number of the kernel-mode driver.
   *
   * Contents depending on index:
   *
   * \li 0: The build number
   * \li 1: 0
   * \li 2: The minor revision number
   * \li 3: The major revision number
   */
#define canCHANNELDATA_DRIVER_FILE_VERSION       21

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   *  \a buffer points to an array of four 16-bit unsigned integers which
   *  receives the product version number of the kernel-mode driver.
   *
   * Contents depending on index:
   *
   * \li 0: 0
   * \li 1: 0
   * \li 2: The minor revision number
   * \li 3: The major revision number
   */
#define canCHANNELDATA_DRIVER_PRODUCT_VERSION    22

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a buffer which receives the device manufacturer's name
   * as a zero-terminated Unicode string.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_MFGNAME_UNICODE           23

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a buffer which receives the device manufacturer's name
   * as a zero-terminated ASCII string.
   */
#define canCHANNELDATA_MFGNAME_ASCII             24

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a buffer which receives the product name of the device
   * as a zero-terminated Unicode string.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_DEVDESCR_UNICODE          25

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a buffer which receives the product name of the device
   * as a zero-terminated ASCII string.
   */
#define canCHANNELDATA_DEVDESCR_ASCII            26

 /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a buffer which receives the name of the device
   * driver (e.g. "kcanl") as a zero-terminated ASCII string.
   *
   * \note The device driver names have no special meanings and may change
   * from a release to another.
   */
#define canCHANNELDATA_DRIVER_NAME               27

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer that receives the quality of
   * the channel, where the quality is measured in percent of optimal quality.
   *
   * For WLAN, -90 dBm and -35 dBm are considered 0% and 100%, respectively.
   *
   * The quality is 100% for any directly connected channel (USB, PCI etc.).
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_CHANNEL_QUALITY           28

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer point to a 32-bit unsigned integer that receives the roundtrip
   * time which is measured in milliseconds.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_ROUNDTRIP_TIME            29

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer that receives the
   * \ref kvBUSTYPE_GROUP_xxx bus type.
   * \note Not implemented in Linux.
   */
#define canCHANNELDATA_BUS_TYPE                  30

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a CHAR array of at least 32 characters which receives
   * the current device name as a \c NULL terminated ASCII string. The user
   * settable device name is currently only implemented in remote devices.
   *
   * If device name is not set or the device does not support this
   * functionality, an error will be returned.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_DEVNAME_ASCII             31

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer that contains the time in
   * milliseconds since the last communication occurred.
   *
   * For WLAN devices, this is the time since the last keep-alive message.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_TIME_SINCE_LAST_SEEN      32

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer that receives the
   * current WLAN operational mode of the remote capable device;
   * \ref canCHANNEL_OPMODE_xxx.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_REMOTE_OPERATIONAL_MODE   33

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a buffer which receives the remote profile name
   * of the device as a zero-terminated ASCII string.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_REMOTE_PROFILE_NAME   34

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a buffer which receives the remote host name
   * of the device as a zero-terminated ASCII string.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_REMOTE_HOST_NAME   35

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a buffer which receives the mac address
   * of the device as a zero-terminated ASCII string.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_REMOTE_MAC   36

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer which receives
   * maximum bitrate of the device. Zero value means no limit on
   * bitrate.
   */
#define canCHANNELDATA_MAX_BITRATE   37

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer that receives the
   * capabilities mask of the CAN channel. This mask specifies
   * which capabilities the corresponding device is guaranteed
   * to support/not support at the moment, see \ref canCHANNEL_CAP_xxx
   * for info about flags.
   */
#define canCHANNELDATA_CHANNEL_CAP_MASK  38

/**
 * This define is used in \ref canGetChannelData(), \a buffer
 * mentioned below refers to this functions argument.
 *
 * \note If no channel name is set, \ref canERR_NOT_IMPLEMENTED will be
 * returned, regardless of channel name is supported in the device or not.
 *
 * \a buffer is a user supplied byte array of length 'bufsize' (at
 * least one byte long) to which the null terminated UTF-8 coded
 * channel name will be placed.
 */
#define canCHANNELDATA_CUST_CHANNEL_NAME  39

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer that is 1 if
   * the channel(device) is currently connected as a remote device. 0 if it is not
   * currently a remote device.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_IS_REMOTE  40

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer that returns the type of remote connection.
   * See \ref kvREMOTE_TYPExxx for returned values.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_REMOTE_TYPE  41

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to a 32-bit unsigned integer that returns the logger type of the device.
   * See \ref kvLOGGER_TYPE_xxx for returned values.
   * \note Not implemented in linux.
   */
#define canCHANNELDATA_LOGGER_TYPE  42

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to an array of six 32-bit unsigned integers that receives
   * hardware status codes for the device.
   *
   * \note This is only intended for internal use.
   */
#define canCHANNELDATA_HW_STATUS  43

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer points to an array of two 32-bit unsigned integers that returns
   * the feature eanLo and feature eanHi of the device.
   *
   * \note This is only intended for internal use.
   */
#define canCHANNELDATA_FEATURE_EAN  44

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer is \ref kvBusParamLimits.
   */
#define canCHANNELDATA_BUS_PARAM_LIMITS  45

  /**
   * This define is used in \ref canGetChannelData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \a buffer is \ref kvClockInfo.
   */
#define canCHANNELDATA_CLOCK_INFO  46

/**
 * This define is used in \ref canGetChannelData(), \a buffer
 * mentioned below refers to this functions argument.
 *
 * \a buffer points to an array of two 64-bit unsigned integers that
 * receives the capabilities value and capabilities mask of the CAN channel.
 *
 * The value specifies the capabilities of the CAN controller;
 * this is a combination of the \ref canCHANNEL_CAP_EX_xxx flags.
 * A one in a bit means that the capability is supported.
 *
 * The mask specifies which capabilities the corresponding device is guaranteed
 * to support/not support at the moment. See \ref canCHANNEL_CAP_EX_xxx
 * for info about flags.
 *
 * Contents depending on index:
 *
 * \li 0: value
 * \li 1: mask
 */
#define canCHANNELDATA_CHANNEL_CAP_EX                47

 /** @} */

/**
 * Returned when using \ref canCHANNELDATA_CLOCK_INFO
 *
 * Returns clock characteristics for device.
 *
 * The device clock frequency can then be calculated as:
 *
 * double frequency = numerator/denominator * 10 ** power_of_ten;
*/

typedef struct kvClockInfo {
    int version;      /**< The version of this struct, currently 1 */
    int numerator;    /**< The numerator part of the device clock frequency. */
    int denominator;  /**< The denominator part of the device clock frequency. */
    int power_of_ten; /**< The power_of_ten part of the device clock frequency. */
    int accuracy_ppm; /**< The accuracy (in ppm) of the device clock. */
}kvClockInfo;

/**
 * Returned when using \ref canCHANNELDATA_BUS_PARAM_LIMITS
 *
 * This struct shows the low level limits of the parameters.
 *
 * The tq field is always zero for both min/max arbitration/data, and is reserved
 * for possible other uses in future releases.
 *
 * If prop is zero for both min and max values, that means that the unit
 * does not distinguish between phase segment one and the propagation
 * segment, i.e. the limit applies to (phase1 + prop).
 *
 * \note  This is only intended for internal use.
*/
typedef struct kvBusParamLimits {
    int version;   /**< The version of this struct, currently 2 */
    kvBusParamsTq arbitration_min;
    kvBusParamsTq arbitration_max;
    kvBusParamsTq data_min;
    kvBusParamsTq data_max;
}kvBusParamLimits;

/**
 * \name canCHANNEL_IS_xxx
 * \anchor canCHANNEL_IS_xxx
 *
 * These channelFlags are used in \ref canGetChannelData() and in conjunction with \ref
 * canCHANNELDATA_CHANNEL_FLAGS.
 *  @{
 */
/** Used with \ref canCHANNELDATA_CHANNEL_FLAGS, indicates that the channel is
    opened exclusively. */
#define canCHANNEL_IS_EXCLUSIVE         0x0001
/** Used with \ref canCHANNELDATA_CHANNEL_FLAGS, indicates that the channel is
    active (either opened in LIN mode or on-bus in CAN mode. */
#define canCHANNEL_IS_OPEN              0x0002

/** Used with \ref canCHANNELDATA_CHANNEL_FLAGS, indicates that the channel has
 *  been opened as CAN FD.  */
#define canCHANNEL_IS_CANFD             0x0004

//#define canCHANNEL_IS_CANFD_NON_ISO          0x0008 Reserved for when needed

/** Used with \ref canCHANNELDATA_CHANNEL_FLAGS, indicates that the channel has
    been opened as LIN.  */
#define canCHANNEL_IS_LIN               0x0010

/** Used with \ref canCHANNELDATA_CHANNEL_FLAGS, indicates that the channel has
    been opened as a LIN master.  */
#define canCHANNEL_IS_LIN_MASTER        0x0020

/** Used with \ref canCHANNELDATA_CHANNEL_FLAGS, indicates that the channel has
    been opened as a LIN slave.  */
#define canCHANNEL_IS_LIN_SLAVE         0x0040


 /** @} */


/**
 * \name canHWTYPE_xxx
 * \anchor canHWTYPE_xxx
 *
 * The following constants can be returned from \ref canGetChannelData(), using the
 * \ref canCHANNELDATA_CARD_TYPE item code. They identify the hardware type for
 * the channel specified in the call to \ref canGetChannelData().
 *
 * \note They indicate a hardware type, but not necessarily a specific
 * product. For example, \ref canHWTYPE_LAPCAN is returned both for LAPcan and
 * LAPcan II. (You can use \ref canGetChannelData() to obtain the UPC/EAN code for
 * the device. This number uniquely identifies the product.)
 *
 *  @{
 */
#define canHWTYPE_NONE                0 ///< Unknown or undefined
#define canHWTYPE_VIRTUAL             1 ///< The virtual CAN bus
#define canHWTYPE_LAPCAN              2 ///< LAPcan Family
#define canHWTYPE_CANPARI             3 ///< CANpari (obsolete).
#define canHWTYPE_PCCAN               8 ///< PCcan Family
#define canHWTYPE_PCICAN              9 ///< PCIcan Family
#define canHWTYPE_USBCAN             11 ///< USBcan (obsolete).
#define canHWTYPE_PCICAN_II          40 ///< PCIcan II family
#define canHWTYPE_USBCAN_II          42 ///< USBcan II, USBcan Rugged, Kvaser Memorator
#define canHWTYPE_SIMULATED          44 ///< Simulated CAN bus for Kvaser Creator (obsolete).
#define canHWTYPE_ACQUISITOR         46 ///< Kvaser Acquisitor (obsolete).
#define canHWTYPE_LEAF               48 ///< Kvaser Leaf Family
#define canHWTYPE_PC104_PLUS         50 ///< Kvaser PC104+
#define canHWTYPE_PCICANX_II         52 ///< Kvaser PCIcanx II
#define canHWTYPE_MEMORATOR_II       54 ///< Kvaser Memorator Professional family
#define canHWTYPE_MEMORATOR_PRO      54 ///< Kvaser Memorator Professional family
#define canHWTYPE_USBCAN_PRO         56 ///< Kvaser USBcan Professional
#define canHWTYPE_IRIS               58 ///< Obsolete name, use canHWTYPE_BLACKBIRD instead
#define canHWTYPE_BLACKBIRD          58 ///< Kvaser BlackBird
#define canHWTYPE_MEMORATOR_LIGHT    60 ///< Kvaser Memorator Light
#define canHWTYPE_MINIHYDRA          62 ///< Obsolete name, use canHWTYPE_EAGLE instead
#define canHWTYPE_EAGLE              62 ///< Kvaser Eagle family
#define canHWTYPE_BAGEL              64 ///< Obsolete name, use canHWTYPE_BLACKBIRD_V2 instead
#define canHWTYPE_BLACKBIRD_V2       64 ///< Kvaser BlackBird v2
#define canHWTYPE_MINIPCIE           66 ///< Kvaser Mini PCI Express
#define canHWTYPE_USBCAN_KLINE       68 ///< USBcan Pro HS/K-Line
#define canHWTYPE_ETHERCAN           70 ///< Kvaser Ethercan
#define canHWTYPE_USBCAN_LIGHT       72 ///< Kvaser USBcan Light
#define canHWTYPE_USBCAN_PRO2        74 ///< Kvaser USBcan Pro 5xHS and variants
#define canHWTYPE_PCIE_V2            76 ///< Kvaser PCIEcan 4xHS and variants
#define canHWTYPE_MEMORATOR_PRO2     78 ///< Kvaser Memorator Pro 5xHS and variants
#define canHWTYPE_LEAF2              80 ///< Kvaser Leaf Pro HS v2 and variants
#define canHWTYPE_MEMORATOR_V2       82 ///< Kvaser Memorator (2nd generation)
#define canHWTYPE_CANLINHYBRID       84 ///< Kvaser Hybrid CAN/LIN
#define canHWTYPE_DINRAIL            86 ///< Kvaser DIN Rail SE400S and variants
#define canHWTYPE_U100               88 ///< Kvaser U100 and variants
#define canHWTYPE_LEAF3              90 ///< Kvaser Kvaser Leaf v3 and variants

/** @} */

/**
 * \name canCHANNEL_CAP_xxx
 * \anchor canCHANNEL_CAP_xxx
 *
 * Channel capabilities.
 */
#define canCHANNEL_CAP_EXTENDED_CAN      0x00000001L ///< Used in \ref canGetChannelData(). Can use extended identifiers
#define canCHANNEL_CAP_BUS_STATISTICS    0x00000002L ///< Used in \ref canGetChannelData(). Can report busload etc
#define canCHANNEL_CAP_ERROR_COUNTERS    0x00000004L ///< Used in \ref canGetChannelData(). Can return error counters
#define canCHANNEL_CAP_RESERVED_2        0x00000008L ///< Obsolete, only used by LAPcan driver
#define canCHANNEL_CAP_GENERATE_ERROR    0x00000010L ///< Used in \ref canGetChannelData(). Can send error frames
#define canCHANNEL_CAP_GENERATE_OVERLOAD 0x00000020L ///< Used in \ref canGetChannelData(). Can send CAN overload frame
#define canCHANNEL_CAP_TXREQUEST         0x00000040L ///< Used in \ref canGetChannelData(). Can report when a CAN message transmission is initiated
#define canCHANNEL_CAP_TXACKNOWLEDGE     0x00000080L ///< Used in \ref canGetChannelData(). Can report when a CAN messages has been transmitted
#define canCHANNEL_CAP_VIRTUAL           0x00010000L ///< Used in \ref canGetChannelData(). Virtual CAN channel
#define canCHANNEL_CAP_SIMULATED         0x00020000L ///< Used in \ref canGetChannelData(). Simulated CAN channel
#define canCHANNEL_CAP_RESERVED_1        0x00040000L ///< Obsolete, use canCHANNEL_CAP_REMOTE_ACCESS or \ref canGetChannelData() instead.
#define canCHANNEL_CAP_CAN_FD            0x00080000L ///< Used in \ref canGetChannelData(). CAN-FD ISO compliant channel
#define canCHANNEL_CAP_CAN_FD_NONISO     0x00100000L ///< Used in \ref canGetChannelData(). CAN-FD NON-ISO compliant channel
#define canCHANNEL_CAP_SILENT_MODE       0x00200000L ///< Used in \ref canGetChannelData(). Channel supports Silent mode
#define canCHANNEL_CAP_SINGLE_SHOT       0x00400000L ///< Used in \ref canGetChannelData(). Channel supports Single Shot messages
#define canCHANNEL_CAP_LOGGER            0x00800000L ///< Used in \ref canGetChannelData(). Channel has logger capabilities.
#define canCHANNEL_CAP_REMOTE_ACCESS     0x01000000L ///< Used in \ref canGetChannelData(). Channel has remote capabilities
#define canCHANNEL_CAP_SCRIPT            0x02000000L ///< Used in \ref canGetChannelData(). Channel has script capabilities.
#define canCHANNEL_CAP_LIN_HYBRID        0x04000000L ///< Used in \ref canGetChannelData(). Channel has LIN capabilities.
#define canCHANNEL_CAP_IO_API            0x08000000L ///< Used in \ref canGetChannelData(). Channel has IO API capabilities.
#define canCHANNEL_CAP_CANTEGRITY        0x10000000L ///< Used in \ref canGetChannelData(). Channel has CANtegrity capabilities.

/**
 * \name canCHANNEL_CAP_EX_xxx
 * \anchor canCHANNEL_CAP_EX_xxx
 *
 * Channel extended capabilities.
 */
#define canCHANNEL_CAP_EX_BUSPARAMS_TQ      0x0000000000000001L ///< Used in \ref canGetChannelData() with \ref canCHANNELDATA_CHANNEL_CAP_EX as the item argument. Channel has BusParams TQ API

/**
 * \name canCHANNEL_OPMODE_xxx
 * \anchor canCHANNEL_OPMODE_xxx
 *
 * Current WLAN operational mode.
 * \note Not implemented in linux.
 *
 *  @{
 */
/**
 * Not applicable, or unknown
 */
#define canCHANNEL_OPMODE_NONE                 1

/**
 * Infrastructure mode
 */
#define canCHANNEL_OPMODE_INFRASTRUCTURE       2

/**
 * Reserved value, do not use
 */
#define canCHANNEL_OPMODE_RESERVED             3

/**
 * Adhoc mode
 */
#define canCHANNEL_OPMODE_ADHOC                4
/** @} */

/**
 *  \name canDRIVER_CAP_xxx
 *  \anchor canDRIVER_CAP_xxx
 *
 *  Driver (transceiver) capabilities.
 *  @{
 */
/** Used with \ref canCHANNELDATA_TRANS_CAP */
#define canDRIVER_CAP_HIGHSPEED             0x00000001L
/** @} */

/**
 * \ingroup can_general
 * \name canIOCTL_xxx
 * \anchor canIOCTL_xxx
 *
 * These defines are used in \ref canIoCtl().
 *
 * @{
 */


  /**
   * This define is used in \ref canIoCtl(), \a buf and \a buflen refers to this
   * functions arguments.
   *
   * Tells CANlib to "prefer" extended identifiers; that is, if you send a
   * message with \ref canWrite() and don't specify \ref canMSG_EXT nor \ref
   * canMSG_STD, \ref canMSG_EXT will be assumed and the most significant bits
   * of the identifier will be cut off. The contents of \a buf and \a buflen
   * is ignored. \ref canRead() et al will set \ref canMSG_EXT and/or \ref
   * canMSG_STD as usual and are not affected by this call.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_PREFER_EXT             1

  /**
   * This define is used in \ref canIoCtl(), \a buf and \a buflen refers to this
   * functions arguments.
   *
   * Tells CANlib to "prefer" standard identifiers; that is, if you send a
   * message with \ref canWrite() and don't specify \ref canMSG_EXT nor \ref canMSG_STD,
   * \ref canMSG_STD will be assumed. The contents of \a buf and \a buflen are
   * ignored. \ref canRead() et al will set \ref canMSG_EXT and/or \ref canMSG_STD as usual
   * and are not affected by this call.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_PREFER_STD             2

  /**
   *
   * Note that CAN error counters are never updated on device and will be briefly
   * changed back to their original values after this call.
   *
   * This define is used in \ref canIoCtl(), \a buf and \a buflen refers to this
   * functions arguments.
   *
   * Tells CANlib to clear the CAN error counters. The contents of \a buf and \a
   * buflen is ignored. CAN error counters on device side are NOT updated.
   *
   * \note It is recommended to use \ref canIOCTL_RESET_OVERRUN_COUNT to
   * reset overrun status.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_CLEAR_ERROR_COUNTERS   5

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *

   * The timer scale determines how precisely the channel's timestamps will be
   * displayed without changing the accuracy of the clock. \a buf points to a
   * DWORD which contains the desired time-stamp clock resolution in
   * microseconds. If you want to change the resolution to 10 microseconds, set
   * the value to 10. The default value is 1000, i.e. one millisecond.
   *
   * \note The accuracy of the clock isn't affected.
   */
#define canIOCTL_SET_TIMER_SCALE        6

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * Enabling transmit Acknowledges on a channel results in that channel receiving a
   * message with the TXACK flag enabled every time a message is successfully
   * transmitted on the bus.
   *
   * \a buf points to a DWORD which contains
   *
   * \li 0: to turn Transmit Acknowledges off.
   * \li 1: to turn Transmit Acknowledges on.
   * \deprecated \li 2: to turn Transmit Acknowledges off, even for the driver's internal
   * usage. This might enhance performance but will cause some other APIs to
   * stop working (for example, the current size of the transmit queue can not
   * be read when this mode is active.)
   *
   * The default value is 0, Transmit Acknowledge is off.
   */
#define canIOCTL_SET_TXACK              7

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \a buf points at a \c DWORD which receives the current RX queue level. The
   * returned value is approximative, this is because not all hardware supports
   * retrieving the queue levels. In that case a best-effort guess is
   * returned. Also note that a device with embedded CPU will report its queue
   * levels to the host computer after a short delay that depends on the bus
   * traffic intensity, and consequently the value returned by the call to
   * \ref canIoCtl() might be a few milliseconds old.
   */
#define canIOCTL_GET_RX_BUFFER_LEVEL              8

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \a buf points at a \c DWORD which receives the current TX queue level. The
   * returned value is approximative, this is because not all hardware supports
   * retrieving the queue levels. In that case a best-effort guess is
   * returned. Also note that a device with embedded CPU will report its queue
   * levels to the host computer after a short delay that depends on the bus
   * traffic intensity, and consequently the value returned by the call to
   * \ref canIoCtl() might be a few milliseconds old.
   */
#define canIOCTL_GET_TX_BUFFER_LEVEL              9

  /**
   * This define is used in \ref canIoCtl(), \a buf and \a buflen refers to this
   * functions arguments.
   *
   * Discard the current contents of the RX queue. The values of \a buf and \a
   * buflen is ignored.
   *
   * \note This is the same thing as calling \ref canFlushReceiveQueue()
   */
#define canIOCTL_FLUSH_RX_BUFFER                  10

  /**
   * This define is used in \ref canIoCtl(), \a buf and \a buflen refers to this
   * functions arguments.
   *
   * Discard the current contents of the TX queue. The values of \a buf and \a
   * buflen is ignored.
   *
   * \note This is the same thing as calling \ref canFlushTransmitQueue().
   */
#define canIOCTL_FLUSH_TX_BUFFER                  11

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \a buf points to a \c DWORD which contains the desired time-stamp clock
   * resolution in microseconds. Note that the accuracy of the clock isn't
   * affected. The default value is 1000 microseconds, i.e. one millisecond.
   */
#define canIOCTL_GET_TIMER_SCALE                  12

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * Turns transmit requests on or off. If transmit requests are enabled on a
   * channel, the channel will receive a message any time it writes a message
   * to the channel.
   *
   * \a buf points to a \c DWORD which contains
   *
   * \li \c 0 to turn Transmit Requests off.
   * \li \c 1 to turn Transmit Requests on.
   *
   * Default value is \c 0, Transmit Requests off.
   */
#define canIOCTL_SET_TXRQ                         13

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \a buf points at a \c HANDLE which receives a Windows Event handle which can
   * be passed to the Win32 API \c WaitForSingleObject. The event is signaled
   * when "something" (typically that a CAN message has been received or
   * transmitted) happens in the driver.
   *
   * \note There is no more information available as to what happened when this
   * call returns. The call may return on an "internal" event in CANlib and your
   * application must be prepared to handle this (i.e. go to sleep again.)
   *
   * \note If \ref canWaitForEvent() returns with success status (\ref canOK), you must call
   * \ref canRead() repeatedly until it returns \ref canERR_NOMSG, before calling
   * \ref canWaitForEvent() again. This will flush the driver's internal event queues.
   * Failure to call \ref canRead() can cause \ref canWaitForEvent() to get stuck in a state
   * where it always sleeps for the specified timeout and then returns with
   * \ref canERR_TIMEOUT.
   *
   * \sa \ref canWaitForEvent()
   *
   * \note You must not set, reset, nor close this handle.  Waiting on it is
   *       the only supported operation.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_GET_EVENTHANDLE                  14

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \note Not yet implemented.
   */
#define canIOCTL_SET_BYPASS_MODE                  15

  /**
   * This define is used in \ref canIoCtl().
   *
   * \note This is only intended for internal use.
   */
#define canIOCTL_SET_WAKEUP                       16

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \a buf points to a HANDLE which receives the Windows handle related to the
   * CANlib handle.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_GET_DRIVERHANDLE                17

  /**
   * This define is used in \ref canIoCtl().
   *
   * \note This is only intended for internal use.
   */
#define canIOCTL_MAP_RXQUEUE                     18

  /**
   * This define is used in \ref canIoCtl().
   *
   *  \note This is only intended for internal use.
   */
#define canIOCTL_GET_WAKEUP                      19

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \a buf points to a unsigned char which contains
   *
   * \li \c 0 to turn access error reporting off, and
   * \li \c 1 to turn access error reporting on.
   *
   * Default value is \c 0, access error reporting off.
   */
#define canIOCTL_SET_REPORT_ACCESS_ERRORS        20

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \a buf points to a unsigned char which receives the current setting of the access
   * error reporting (0 or 1.)
   */
#define canIOCTL_GET_REPORT_ACCESS_ERRORS        21

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * Connects the handle to a virtual bus.
   * \a buf points to a unsigned int containing the virtual bus number (0..31).
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_CONNECT_TO_VIRTUAL_BUS          22

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * Disconnects the handle from a virtual bus.
   * \a buf points to a unsigned int containing the virtual bus number (0..31).
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_DISCONNECT_FROM_VIRTUAL_BUS     23

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \a buf points to a \ref canUserIoPortData struct that contains a port number
   * and a port value to set. This is used by special hardware only.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_SET_USER_IOPORT                 24

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \a buf points to a \ref canUserIoPortData struct that contains a port
   *  number. After the call, the struct will contain the current value of the
   *  I/O port. This is used by special hardware only.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_GET_USER_IOPORT                 25

  /**
   * This define is used in \ref canIoCtl().
   *
   * \note This is only intended for internal use.
   */
#define canIOCTL_SET_BUFFER_WRAPAROUND_MODE      26

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * Use this function code to set the size of the receive buffer for a
   * specific handle. \a buf points to an unsigned integer which contains the
   * new size (number of messages) of the receive buffer.
   *
   * The receive buffer consumes system non-paged pool memory, which is a
   * limited resource. Do not increase the receive buffer size unless you have
   * good reasons to do so.
   *
   * \note You can't use this function code when the channel is on bus.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_SET_RX_QUEUE_SIZE               27

  /**
   * This define is used in \ref canIoCtl().
   *
   * \note This is only intended for internal use.
   */
#define canIOCTL_SET_USB_THROTTLE                28

  /**
   * This define is used in \ref canIoCtl().
   *
   * \note This is only intended for internal use.
   */
#define canIOCTL_GET_USB_THROTTLE                29

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to
   * this functions argument.
   *
   * This function enables or disables automatic time reset on bus on. By
   * default, this is enabled, so the timer will automatically reset when a
   * handle goes on bus.
   *
   * \a buf points to a DWORD. If the value is zero, the CAN clock will not be
   * reset at buson for the handle. Otherwise, the CAN clock will be reset at
   * buson.
   *
   * Default value is \c 1, the CAN clock will be reset at buson.
   */
#define canIOCTL_SET_BUSON_TIME_AUTO_RESET       30

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * Returns the state of the Transmit Acknowledge as a DWORD in \a buf:
   *
   * \li 0: Transmit Acknowledges is turned off.
   * \li 1: Transmit Acknowledges is turned on.
   * \li 2: Transmit Acknowledges is turned off, even for the driver's internal
   *        usage.
   */
#define canIOCTL_GET_TXACK                       31

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \a buf points to an unsigned byte. If the value is zero, the local transmit
   * echo is turned off for the handle. Otherwise, local transmit echo is turned
   * on.
   *
   * Local transmit echo is turned on by default on all handles.  This means
   * that if two handles are open on the same channel, and a message is
   * transmitted on the first handle, it will be received as a normal message
   * on the second handle. Use the \ref canIOCTL_SET_LOCAL_TXECHO function code to
   * turn this function off, if it is not desired on a certain handle.
   */
#define canIOCTL_SET_LOCAL_TXECHO                32

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * This function turns error frame reporting on or off. If it is off, the
   * channel handle will ignore any error frames it receives.
   *
   * \a buf points to an unsigned byte. If the value is zero, the reporting of
   * error frames is turned off for the handle. Otherwise, error frame reporting
   * is turned on.
   *
   * Default value is \c 1, error frame reporting is turned on.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_SET_ERROR_FRAMES_REPORTING      33

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \a buf points to a 32-bit unsigned integer that receives the quality of
   * the channel, where the quality is measured in percent of optimal quality.
   *
   * For a WLAN, -90 dBm and -35 dBm are considered 0% and 100%, respectively.
   *
   * The quality is 100% for any directly connected channel (USB, PCI etc.).
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_GET_CHANNEL_QUALITY             34

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * Returns the round trip time to a device.
   *
   * \a buf points to a \c DWORD that contains the roundtrip time measured in
   * milliseconds.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_GET_ROUNDTRIP_TIME              35

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \a buf points to a \c DWORD that contains the \ref kvBUSTYPE_GROUP_xxx bus type.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_GET_BUS_TYPE                    36

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \a buf points to a CHAR array of at least 32 characters which receives the
   * current device name as a \c NULL terminated ASCII string.
   *
   * If device name is not set or the device does not support this
   * functionality, an error will be returned.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_GET_DEVNAME_ASCII               37

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \a buf points to a \c DWORD that contains the time in milliseconds since the last
   * communication occurred.
   *
   * For WLAN devices, this is the time since the last keep-alive message.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_GET_TIME_SINCE_LAST_SEEN        38

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * Obtain the time reference list for MagiSync devices.
   *
   * \a buf points to an array of pairs of 64-bit ints, one of which
   * will contain the reference number and the other one the timestamp
   * in nanoseconds.
   *
   * \note This function is subject to change in future releases and is
   * not supported by Kvaser.
   */
#define canIOCTL_GET_TREF_LIST                   39

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * \a buf points to a \c DWORD that contains the number of microseconds
   * the minimum CAN message transmit interval should be set to, or 0xffffffff
   * to fetch the current setting.
   * The minimum interval can not be set to more than one second.
   *
   * When a CAN channel goes bus on, the minimum interval is set to zero.
   * I.e. CAN transmissions happen at the maximum speed the device is capable of.
   *
   * If the device does not support this functionality, or if an invalid
   * minimum interval is requested, an error will be returned.
   *
   * \note The minimum CAN messages transmit interval applies to the physical CAN
   * channel. It will thus affect all messages transmitted, even those sent
   * using other CANlib handles to the same physical channel. The interval
   * is defined as the time from the successful completion of one transmit
   * to the beginning of the next one.
   */
#define canIOCTL_TX_INTERVAL                     40

   /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * Some hardware have a bitrate limit, which must be met when using any of \a canSetBusParams(),
   * \a canSetBusParamsC200() and \a canSetBusParamsFd() functions.
   * The bitrate limit can be overridden with this IOCTL.
   * \a buf points to a \c unsigned long value that contains a user defined bitrate.
   * A value of 0 means that the device should use its own default bitrate limit.<br>
   * To find out which devices that have a bitrate limit, see \ref canCHANNELDATA_MAX_BITRATE.
   */
#define canIOCTL_SET_BRLIMIT                            43

  /**
   * \deprecated Use \ref canIOCTL_SET_THROTTLE_SCALED instead.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_SET_USB_THROTTLE_SCALED                41

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * This ioctl can be used to set the responsitivity of some devices.
   * \a buf points to a \c DWORD that should contain a value between 0 and 100.
   * A value of 0 means that the device should be very responsive and a value
   * of 100 means that the device generates less CPU load or requires more bandwidth.
   * Note that not all
   * devices support this. Some hardware will accept this command but neglect it.
   * This can be found out by reading the scaled throttle.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_SET_THROTTLE_SCALED                    41

  /**
   * \deprecated Use \ref canIOCTL_GET_THROTTLE_SCALED instead
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_GET_USB_THROTTLE_SCALED                42

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * This ioctl can be used to set the responsitivity of some devices.
   * \a buf points to a \c DWORD that should contain a value between 0 and 100.
   * A value of 0 means that the device should be very responsive and a value
   * of 100 means that the device generates less CPU load or requires more bandwidth.
   * Note that not all
   * devices support this. Some hardware will accept this command but neglect it.
   * This can be found out by reading the scaled throttle.
   *
   * \note Not implemented in linux.
   */
#define canIOCTL_GET_THROTTLE_SCALED                    42

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to this
   * functions argument.
   *
   * This ioctl resets overrun count and flags, \sa \ref canReadStatus \sa \ref canGetBusStatistics.
   * The contents of \a buf and \a buflen is ignored.
   */
#define canIOCTL_RESET_OVERRUN_COUNT                          44

  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to an
   * argument of that function.
   *
   * Set the current LIN access mode returned in \ref canCHANNELDATA_CHANNEL_FLAGS.
   *
   * \note This is only intended for internal use.
   */
#define canIOCTL_LIN_MODE                               45


  /**
   * This define is used in \ref canIoCtl(), \a buf mentioned below refers to an
   * argument of that function.
   *
   * Enable reception of canMSG_LOCAL_TXACK
   * When Local transmit acknowledge on a handle is enabled, the handle will receive
   * messages with the canMSG_LOCAL_TXACK flag set every time a message is successfully
   * transmitted on the bus by another handle using the same channel. It will not be received
   * if it is sent on the same handle.
   *
   * \a buf points to an unsigned int which contains
   *
   * \li 0: to turn Local Transmit Acknowledges off.
   * \li 1: to turn Local Transmit Acknowledges on.
   *
   * The default value is 0, Local Transmit Acknowledge is off.
   */
#define canIOCTL_SET_LOCAL_TXACK                     46
 /** @} */

/** Used in \ref canIOCTL_SET_USER_IOPORT and \ref canIOCTL_GET_USER_IOPORT. */
typedef struct {
    unsigned int portNo;     ///< Port number used in e.g. \ref canIOCTL_SET_USER_IOPORT
    unsigned int portValue;  ///< Port value used in e.g. \ref canIOCTL_SET_USER_IOPORT
} canUserIoPortData;

/**
 * \ingroup Notification and Waiting
 *
 * \source_cs       <b>static Canlib.canStatus canWaitForEvent(CanHandle hnd, Int64 timeout);</b>
 *
 * \source_delphi   <b>function canWaitForEvent(hnd: canHandle; timeout: Cardinal): canStatus;     </b>
 * \source_end
 *
 * Waits for an event (of any kind) to happen at the specified CAN circuit, or
 * a timeout to occur. An event in this context means for example the arrival
 * of a CAN message or a CAN bus status change, but it can also be an event
 * internal to the driver.
 *
 * \param[in] hnd      A handle to an open CAN circuit.
 * \param[in] timeout  The number of milliseconds to wait before the call
 *                     returns, if no event occurs. 0xFFFFFFFF gives an
 *                     infinite timeout.
 *
 * \return \ref canOK (zero) if an event happened during the specified time
 *         period.
 * \return \ref canERR_TIMEOUT (negative) if nothing happened during the specified
 *         time period.
 * \return \ref canERR_xxx (negative) if failure.
 *
 * \sa \ref section_user_guide_send_recv_asynch_event
 * \sa \ref canRead()
 *
 * \note Not implemented in linux.
 */
canStatus CANLIBAPI canWaitForEvent(const CanHandle hnd, DWORD timeout);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canSetBusParamsC200(CanHandle hnd, byte btr0, byte btr1);</b>
 *
 * \source_delphi   <b>function canSetBusParamsC200(hnd: canHandle; btr0, btr1: byte): canStatus;     </b>
 * \source_end
 *
 * This function sets the bus timing parameters using the same
 * convention as the 82c200 CAN controller (which is the same as many
 * other CAN controllers, for example, the 82527.)
 *
 * To calculate the bit timing parameters, you can use the bit timing
 * calculator that is included with CANlib SDK. Look in the BIN directory.
 *
 * 82c200 Bit Timing
 *
 * \li \a btr0 [b7..b6]: SJW - 1
 * \li \a btr0 [b5..b0]: Prescaler -1
 * \li \a btr1 [b7]: \c 1: 3 samples, \c 0: 1 samples
 * \li \a btr1 [b6..b4]: tseg2 - 1
 * \li \a btr1 [b3..b0]: tseg1 - 2
 *
 * \note CANlib will always behave as if the clock frequency is 16 MHz. It does
 * not matter if the device has a different physical clock, since this will be
 * compensated for by the driver.
 *
 * \param[in] hnd   A handle to an open CAN circuit.
 * \param[in] btr0  The desired bit timing, formatted as the contents of the
 *                  BTR0 register in the 82c200.
 * \param[in] btr1  The desired bit timing, formatted as the contents of the
 *                  BTR1 register in the 82c200.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 *
 * \sa \ref section_user_guide_init_bit_rate_can, \ref canSetBusParams()
 * \sa \ref canSetBusParamsTq()
 */
canStatus CANLIBAPI canSetBusParamsC200(const CanHandle hnd, unsigned char btr0, unsigned char btr1);



/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canSetDriverMode(CanHandle hnd, Int32 lineMode, Int32 resNet);</b>
 *
 * \source_delphi   <b>function canSetDriverMode(hnd: canHandle; lineMode, resNet: Integer): canStatus;     </b>
 * \source_end
 *
 * This function sets the current CAN bus driver mode. This is
 * typically a mode like sleep, wakeup, standby, fast mode, etc. The
 * different modes are almost always hardware dependent and requires
 * special DRVcan cables. As an example, the DRVcan S implements J2411
 * compliant single-wire CAN and supports four line modes, namely
 * Normal, Sleep, Fast and Wakeup.
 *
 * Standard ISO 11898 CAN do not support any of these bus driver modes.
 *
 * \note The bus driver mode is typically used to control things like one- or
 * two-wire mode, sleep mode, and so on. It requires special support in the CAN
 * driver circuit.
 *
 * \param[in] hnd       An open handle to a CAN circuit.
 * \param[in] lineMode  An int which defines the line mode,
 *                      \ref canTRANSCEIVER_LINEMODE_xxx.
 * \param[in] resNet    An int which defines the resnet mode. Set this parameter to
 *                      \ref canTRANSCEIVER_RESNET_NA unless you have good reasons to set it
 *                      to something else.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canGetDriverMode()
 *
 * \note Not implemented in linux.
 */
canStatus CANLIBAPI canSetDriverMode(const CanHandle hnd, int lineMode, int resNet);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canGetDriverMode(CanHandle hnd, out Int32 lineMode, out Int32 resNet);</b>
 *
 * \source_delphi   <b>function canGetDriverMode(hnd: canHandle; var lineMode: Integer; var resNet: Integer): canStatus;     </b>
 * \source_end
 *
 * This function retrieves the current CAN bus driver mode. This is typically a
 * mode like sleep, wakeup, standby, fast mode, etc. The different modes are
 * almost always hardware dependent and requires special DRVcan cables. As an
 * example, the DRVcan S implements J2411 compliant single-wire CAN and
 * supports four line modes, namely Normal, Sleep, Fast and Wakeup.
 *
 * Standard ISO 11898 CAN do not support any of these bus driver modes.
 *
 * \note The bus driver mode is typically used to control things like one- or
 * two-wire mode, sleep mode, and so on. It requires special support in the CAN
 * driver circuit.
 *
 * \param[in]  hnd       An open handle to a CAN circuit.
 * \param[out] lineMode  A pointer to an int which receives the current line
 *                       mode (\ref canTRANSCEIVER_LINEMODE_xxx).
 * \param[out] resNet    A pointer to an int which receives the current resnet
 *                       mode.  This value is usually
 *                       \ref canTRANSCEIVER_RESNET_NA except for special DRVcan
 *                       cables.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canSetDriverMode()
 *
 * \note Not implemented in linux.
 *
 */
canStatus CANLIBAPI canGetDriverMode(const CanHandle hnd, int *lineMode, int *resNet);

/**
 * \name canVERSION_CANLIB32_xxx
 * \anchor canVERSION_CANLIB32_xxx
 * @{
 * Item codes for canGetVersionEx().
 */

  /**
   * This define is used in \ref canGetVersionEx() and controls which version number
   * that returned.
   *
   * Version number of the canlib32.dll file coded as an unsigned 16-bit word
   * with the major version number in the upper byte and the minor version
   * number in the lower byte. This version number is not related to the
   * product version number of the whole CANlib.  For example, canlib32.dll
   * belonging to CANlib 2.27 would return 0x305.
   *
   * \note Linux returns version of libcanlib.so.
   */
#define canVERSION_CANLIB32_VERSION     0

  /**
   * This define is used in \ref canGetVersionEx() and controls what type of version
   * number that is returned.
   *
   * Product version number of CANlib coded as an unsigned 16-bit word
   * with the major version number in the upper byte and the minor version
   * number in the lower byte. The product version number corresponds to the
   * version number of the whole CANlib. For example, CANlib 2.27 would return
   * 0x21B.
   */
#define canVERSION_CANLIB32_PRODVER     1

  /**
   * This define is used in \ref canGetVersionEx() and controls what type of version
   * number that is returned.
   *
   * Product version number of canlib32.dll coded as an unsigned 32-bit word
   * where the bytes contain (in order from the most significant to the least
   * significant byte) 0, major version number, minor version number, and the
   * minor version letter. (The minor version letter is the ASCII code for the
   * letter, or 0 (zero) if no letter). For example, CANlib 3.8 would return
   * 0x00030800 and CANlib 3.8a would return 0x00030861.
   */
#define canVERSION_CANLIB32_PRODVER32   2

  /**
   * This define is used in \ref canGetVersionEx() and controls what type of version
   * number that is returned.
   *
   * Returns 1 if the present version is a beta (preview) release, or 0 if it
   * is an official release.
   *
   * \note In Linux, this depends on the define CANLIB_BETA
   *
   */
#define canVERSION_CANLIB32_BETA        3


/** @} */


/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Int32 canGetVersionEx(Int32 itemCode);</b>
 *
 * \source_delphi   <b>function canGetVersionEx(itemCode: Cardinal): Cardinal;     </b>
 * \source_end
 *
 * This function returns various version numbers from the driver routines.
 *
 * \param[in] itemCode  Specifies which version number to retrieve. See
 *                      \ref canVERSION_CANLIB32_xxx
 *
 * \return The return value is desired version number.
 *
 * \sa \ref page_user_guide_version
 */
unsigned int CANLIBAPI canGetVersionEx(unsigned int itemCode);


/**
 * \ingroup NamedParameterSettings
 *
 * \source_cs       <b>static Canlib.canStatus canParamGetCount();</b>
 *
 * \source_delphi   <b>function canParamGetCount(): canStatus;     </b>
 * \source_end
 *
 * This function returns the number of entries in the table of named
 * channels.
 *
 * \return The number of channels (zero or positive)
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_misc_named_parameters
 *
 */
canStatus CANLIBAPI canParamGetCount(void);

/**
 * \ingroup NamedParameterSettings
 *
 * \source_cs       <b>static Canlib.canStatus canParamCommitChanges();</b>
 *
 * \source_delphi   <b>function canParamCommitChanges(): canStatus;   </b>
 * \source_end
 *
 * This function writes the current set of named parameters to the
 * Registry. Previous entries are erased.
 *
 * The named parameters are stored in the
 * \c HKEY_LOCAL_MACHINE\\SOFTWARE\\KVASER \c AB\\CANLIB32\\PredefinedBitrates
 * key in the Registry.
 *
 * \note You must have Administrator's rights to write to the Registry.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_misc_named_parameters
 *
 */
canStatus CANLIBAPI canParamCommitChanges(void);

/**
 * \ingroup NamedParameterSettings
 *
 * \source_cs       <b>static Canlib.canStatus canParamDeleteEntry(Int32 index);</b>
 *
 * \source_delphi   <b>function canParamDeleteEntry(index: Integer): canStatus;      </b>
 * \source_end
 *
 * This function deletes the entry in the table of named parameter settings
 * with the given index. The entries below (i.e. with higher indices) the
 * deleted entry are moved up one step in the table.
 *
 * The named parameters are stored in the
 * \c HKEY_LOCAL_MACHINE\\SOFTWARE\\KVASER \c AB\\CANLIB32\\PredefinedBitrates
 * key in the Registry.
 *
 * \param[in]  index  The index of the entry to delete.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_misc_named_parameters
 * \sa \ref canParamCreateNewEntry(), \ref canParamCommitChanges()
 */
canStatus CANLIBAPI canParamDeleteEntry(int index);

/**
 * \ingroup NamedParameterSettings
 *
 * \source_cs       <b>static Int32 canParamCreateNewEntry();</b>
 *
 * \source_delphi   <b>function canParamCreateNewEntry(): canStatus;     </b>
 * \source_end
 *
 * This function creates a new entry in the table of named parameter
 * settings.
 *
 * The named parameters are stored in the
 * \c HKEY_LOCAL_MACHINE\\SOFTWARE\\KVASER \c AB\\CANLIB32\\PredefinedBitrates
 * key in the Registry.
 *
 * \return The index of the created entry (zero or positive) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_misc_named_parameters
 * \sa \ref canParamCommitChanges(), \ref canParamDeleteEntry()
 *
 */
canStatus CANLIBAPI canParamCreateNewEntry(void);

/**
 * \ingroup NamedParameterSettings
 *
 * \source_cs       <b>static Canlib.canStatus canParamSwapEntries(Int32 index1, Int32 index2);</b>
 *
 * \source_delphi   <b>function canParamSwapEntries(index1, index2: Integer): canStatus;      </b>
 * \source_end
 *
 * This function swaps two entries in the list of named
 * parameters.
 *
 * The named parameters are stored in the
 * \c HKEY_LOCAL_MACHINE\\SOFTWARE\\KVASER \c AB\\CANLIB32\\PredefinedBitrates
 * key in the Registry.
 *
 * \param[in]  index1  The first of the two entries that are to be swapped in the
 *                     named parameters list.
 * \param[in]  index2  The second of the two entries that are to be swapped in the
 *                     named parameters list.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_misc_named_parameters
 * \sa \ref canParamCommitChanges()
 *
 */
canStatus CANLIBAPI canParamSwapEntries(int index1, int index2);

/**
 * \ingroup NamedParameterSettings
 *
 * \source_cs       <b>static Canlib.canStatus canParamGetName(Int32 index, ref String str_buf);</b>
 *
 * \source_delphi   <b>function canParamGetName(index: Integer; buffer: PAnsiChar; maxlen: Integer): canStatus;      </b>
 * \source_end
 *
 * This function returns the name of a given entry in the list of
 * named parameters.
 *
 * The named parameters are stored in the
 * \c HKEY_LOCAL_MACHINE\\SOFTWARE\\KVASER \c AB\\CANLIB32\\PredefinedBitrates
 * key in the Registry.
 *
 * \param[in]  index   The index of the entry in the named parameters list, whose
 *                     name is to be returned.
 * \param[out] buffer  A pointer to a buffer that is to receive a
 *                     \c NULL terminated string which contains the name. The
 *                     buffer is allocated and deallocated by the user.
 * \param[in]  maxlen  The length of the buffer.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_misc_named_parameters
 * \sa \ref canParamSetName(), \ref canParamCommitChanges()
 */
canStatus CANLIBAPI canParamGetName(int index, char *buffer, int maxlen);

/**
 * \ingroup NamedParameterSettings
 *
 * \source_cs       <b>static Int32 canParamGetChannelNumber(Int32 index);</b>
 *
 * \source_delphi   <b>function canParamGetChannelNumber(index: Integer): canStatus;     </b>
 * \source_end
 *
 * This function returns the channel number of the entry with the
 * given index in the table of named parameter settings.
 *
 * \param[in] index The index of the entry in the table of named parameter
 *                  settings.
 *
 * \return The channel number of the entry in question (zero or positive)
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_misc_named_parameters
 * \sa \ref canParamCommitChanges(), \ref canParamGetChannelNumber()
 */
canStatus CANLIBAPI canParamGetChannelNumber(int index);

/**
 * \ingroup NamedParameterSettings
 *
 * \source_cs       <b>static Canlib.canStatus canParamGetBusParams(Int32 index, out Int32 bitrate, out Int32 tseg1, out Int32 tseg2, out Int32 sjw, out Int32 noSamp);</b>
 *
 * \source_delphi   <b>function canParamGetBusParams(index: Integer; var bitrate: Longint; var tseg1: Cardinal; var tseg2: Cardinal; var sjw: Cardinal; var nosamp: Cardinal): canStatus;    </b>
 * \source_end
 *
 * This function retrieves the bus parameters associated with the
 * entry with the given index in the table of named parameter
 *   settings.
 *
 * \param[in] index     The index of the entry in the table of named parameter
 *                      settings.
 * \param[out] bitrate  Bit rate (bits per second).
 * \param[out] tseg1    Time segment 1, that is, the number of quanta from (but
 *                      not including) the Sync Segment to the sampling point.
 * \param[out] tseg2    Time segment 2, that is, the number of quanta from the
 *                      sampling point to the end of the bit.
 * \param[out] sjw      The Synchronization Jump Width.
 * \param[out] noSamp   The number of sampling points; can be 1 or 3.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_misc_named_parameters
 * \sa \ref canParamCommitChanges(), \ref canParamSetBusParams()
 */
canStatus CANLIBAPI canParamGetBusParams(int index,
                                         long *bitrate,
                                         unsigned int *tseg1,
                                         unsigned int *tseg2,
                                         unsigned int *sjw,
                                         unsigned int *noSamp);

/**
 * \ingroup NamedParameterSettings
 *
 * \source_cs       <b>static Canlib.canStatus canParamSetName(Int32 index, String str_buf);</b>
 *
 * \source_delphi   <b>function canParamSetName(index: Integer; buffer: PAnsiChar): canStatus;    </b>
 * \source_end
 *
 * This function sets or changes the name of a named parameter.
 *
 * \param[in]  index  The index of the named parameter whose name is to be
 *                    changed or set.
 * \param[out] buffer A pointer to a \c NULL terminated string that contains the
 *                    new name. If the string is longer than the maximum
 *                    allowed name length, it is truncated.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_misc_named_parameters
 * \sa \ref canParamCommitChanges(), \ref canParamGetName()
 */
canStatus CANLIBAPI canParamSetName(int index, const char *buffer);

/**
 * \ingroup NamedParameterSettings
 *
 * \source_cs       <b>static Canlib.canStatus canParamSetChannelNumber(Int32 index, Int32 channel);</b>
 *
 * \source_delphi   <b>function canParamSetChannelNumber(index, channel: Integer): canStatus;     </b>
 * \source_end
 *
 * This function sets the channel number for a specified entry in the list of
 * named parameters. Channels are numbered from 0 and up.
 *
 * \param[in] index    The index of the entry in the named parameter list whose
 *                     channel number is to be set.
 * \param[in] channel  The channel number.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_misc_named_parameters
 * \sa \ref canParamCommitChanges(), \ref canParamGetChannelNumber()
 */
canStatus CANLIBAPI canParamSetChannelNumber(int index, int channel);

/**
 * \ingroup NamedParameterSettings
 *
 * \source_cs       <b>static Canlib.canStatus canParamSetBusParams(Int32 index, Int32 bitrate, Int32 tseg1, Int32 tseg2, Int32 sjw, Int32 noSamp);</b>
 *
 * \source_delphi   <b>function canParamSetBusParams(index: Integer; bitrate: Longint; tseq1, tseq2, sjw, noSamp: Cardinal): canStatus;     </b>
 * \source_end
 *
 * This function sets or changes the bus parameters for a given entry in the
 * list of named parameters.
 *
 * \note The bus parameters are not checked for validity.
 *
 * \param[in]  index   The index of the entry in the named parameter list whose
 *                     parameters are to be set or changed.
 * \param[in] bitrate  Bit rate (measured in bits per second); or one of the
 *                     predefined constants \ref canBITRATE_xxx.

 * \param[in] tseg1    Time segment 1, that is, the number of quanta from (but not
 *                     including) the Sync Segment to the sampling point.
 * \param[in] tseg2    Time segment 2, that is, the number of quanta from the
 *                     sampling point to the end of the bit.

 * \param[in]  sjw     The Synchronization Jump Width.
 * \param[in]  noSamp  The number of sampling points; can be 1 or 3.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_misc_named_parameters
 * \sa \ref canParamCommitChanges(), \ref canParamGetBusParams()
 */
canStatus CANLIBAPI canParamSetBusParams(int index,
                                         long bitrate,
                                         unsigned int tseg1,
                                         unsigned int tseg2,
                                         unsigned int sjw,
                                         unsigned int noSamp);

/**
 * \ingroup NamedParameterSettings
 *
 * \source_cs       <b>static Canlib.canStatus canParamFindByName(String str_name);</b>
 *
 * \source_delphi   <b>function canParamFindByName(const Name: PAnsiChar): canStatus;     </b>
 * \source_end
 *
 * This function returns the index of the parameter setting with the
 * given name.
 *
 * \param[in] name A pointer to a string containing the name of the setting.
 *
 * \return The index of the setting (zero or positive) if success.
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_misc_named_parameters
 * \sa \ref canParamCommitChanges()
 */
canStatus CANLIBAPI canParamFindByName(const char *name);
/**
 * \ingroup ObjectBuffers
 *
 * \source_cs       <b>static Canlib.canStatus canObjBufFreeAll(CanHandle hnd);</b>
 *
 * \source_delphi   <b>function canObjBufFreeAll(handle: canHandle): canStatus;     </b>
 * \source_end
 *
 * Deallocates all object buffers on the specified handle. The
 * buffers cannot be referenced after this operation.
 *
 * \param[in]  hnd  An open handle to a CAN circuit.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_obj_buf
 * \sa \ref canObjBufFree(), \ref canObjBufAllocate()
 */
canStatus CANLIBAPI canObjBufFreeAll(const CanHandle hnd);

/**
 * \ingroup ObjectBuffers
 *
 * \source_cs       <b>static Canlib.canStatus canObjBufAllocate(CanHandle hnd, Int32 type);</b>
 *
 * \source_delphi   <b>function canObjBufAllocate(handle: canHandle; tp: Integer): canStatus;     </b>
 * \source_end
 *
 * Allocates an object buffer associated with a handle to a CAN
 * circuit.
 *
 * \param[in] hnd   An open handle to a CAN circuit.
 * \param[in] type  The type of the buffer. Must be one of \ref canOBJBUF_TYPE_xxx
 *
 * \return A buffer index (zero or positive) if success.
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_obj_buf
 * \sa \ref canObjBufFree(), \ref canObjBufFreeAll()
 */
canStatus CANLIBAPI canObjBufAllocate(const CanHandle hnd, int type);

 /**
  * \name canOBJBUF_TYPE_xxx
  * \anchor canOBJBUF_TYPE_xxx
  *
  * Used in \ref canObjBufAllocate().
  *
  * @{
  */
#define canOBJBUF_TYPE_AUTO_RESPONSE            0x01 ///< The buffer is an auto-response buffer.
#define canOBJBUF_TYPE_PERIODIC_TX              0x02 ///< The buffer is an auto-transmit buffer.
 /** @} */

/**
 * \ingroup ObjectBuffers
 *
 * \source_cs       <b>static Canlib.canStatus canObjBufFree(CanHandle hnd, Int32 idx);</b>
 *
 * \source_delphi   <b>function canObjBufFree(handle: canHandle; idx: Integer): canStatus;      </b>
 * \source_end
 *
 * Deallocates the object buffer with the specified index. The buffer
 * can not be referenced after this operation.
 *
 * \param[in] hnd  An open handle to a CAN circuit.
 * \param[in] idx  The object buffer to deallocate.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_obj_buf
 * \sa \ref canObjBufFreeAll(), \ref canObjBufAllocate(),
 */
canStatus CANLIBAPI canObjBufFree(const CanHandle hnd, int idx);

// Writes CAN data to the object buffer with the specified index.

/**
 * \ingroup ObjectBuffers
 *
 * \source_cs       <b>static Canlib.canStatus canObjBufWrite(CanHandle hnd, Int32 idx, Int32 id, Byte[] msg, Int32 dlc, Int32 flags);</b>
 *
 * \source_delphi   <b>function canObjBufWrite(handle: canHandle; idx, id: Integer; var msg; dlc, flags: cardinal): canStatus;     </b>
 * \source_end
 *
 * Defines the contents of a specific object buffer.
 *
 * \param[in] hnd   An open handle to a CAN circuit.
 * \param[in] idx   The index of the object buffer whose contents is to be
 *                  defined.
 * \param[in] id    The CAN identifier of the message.
 * \param[in] msg   Points to the contents of the message.
 * \param[in] dlc   The length of the message in bytes.<br>
                    For Classic CAN dlc can be at most 8, unless \ref canOPEN_ACCEPT_LARGE_DLC is used.<br>
                    For CAN FD dlc can be one of the following 0-8, 12, 16, 20, 24, 32, 48, 64.
 * \param[in] flags Message flags; a combination of the \ref canMSG_xxx flags.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_obj_buf
 */
canStatus CANLIBAPI canObjBufWrite(const CanHandle hnd,
                                   int idx,
                                   int id,
                                   void *msg,
                                   unsigned int dlc,
                                   unsigned int flags);

/**
 * \ingroup ObjectBuffers
 *
 * \source_cs       <b>static Canlib.canStatus canObjBufSetFilter(CanHandle hnd, Int32 idx, Int32 code, Int32 mask);</b>
 *
 * \source_delphi   <b>function canObjBufSetFilter(handle: canHandle; idx: Integer; code, mask: Cardinal): canStatus;      </b>
 * \source_end
 *
 * Defines a message reception filter on the specified object buffer.
 * Messages not matching the filter are discarded.
 *
 * \note For an auto response buffer, set the code and mask that together define
 * the identifier(s) that trigger(s) the automatic response.
 *
 * \param[in] hnd   An open handle to a CAN circuit.
 * \param[in] idx   The index of the object buffer on which the filter is to be
 *                  set.
 * \param[in] code  The acceptance code in the filter.
 * \param[in] mask  The acceptance mask in the filter.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_obj_buf
 */
canStatus CANLIBAPI canObjBufSetFilter(const CanHandle hnd,
                                       int idx,
                                       unsigned int code,
                                       unsigned int mask);

/**
 * \ingroup ObjectBuffers
 *
 * \source_cs       <b>static Canlib.canStatus canObjBufSetFlags(CanHandle hnd, Int32 idx, Int32 flags);</b>
 *
 * \source_delphi   <b>function canObjBufSetFlags(handle: canHandle; idx: Integer; flags: Cardinal): canStatus;     </b>
 * \source_end
 *
 * Sets object buffer flags on a specified object buffer.
 *
 * \param[in] hnd    An open handle to a CAN circuit.
 * \param[in] idx    The buffer on which the flags are to be set.
 * \param[in] flags  Specifies a combination of zero or more of the
 *                   \ref canOBJBUF_AUTO_RESPONSE_xxx flag values
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_obj_buf
 */
canStatus CANLIBAPI canObjBufSetFlags(const CanHandle hnd,
                                      int idx,
                                      unsigned int flags);

/**
 * \name canOBJBUF_AUTO_RESPONSE_xxx
 * \anchor canOBJBUF_AUTO_RESPONSE_xxx
 *
 * These defines are used in \ref canObjBufSetFlags().
 *
 * @{
 */
 /**
  * This define is used in \ref canObjBufSetFlags().
  *
  * For auto-response buffers only. When this flag is in effect, the buffer
  * will auto-respond to remote requests only.  If this flag is not in effect,
  * the buffer will auto-respond to both remote requests and ordinary data
  * frames.
  *
  */
#define canOBJBUF_AUTO_RESPONSE_RTR_ONLY        0x01
 /** @} */

/**
 * \ingroup ObjectBuffers
 *
 * \source_cs       <b>static Canlib.canStatus canObjBufSetPeriod(CanHandle hnd, Int32 idx, Int32 period);</b>
 *
 * \source_delphi   <b>function canObjBufSetPeriod(handle: canHandle; idx: Integer; period: Cardinal): canStatus;     </b>
 * \source_end
 *
 * The \ref canObjBufSetPeriod function sets the transmission period for an auto
 * transmission object buffer.
 *
 * \param[in] hnd     An open handle to a CAN channel.
 * \param[in] idx     The index of a CAN object buffer.
 * \param[in] period  The transmission interval, in microseconds.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_obj_buf
 */
canStatus CANLIBAPI canObjBufSetPeriod(const CanHandle hnd,
                                       int idx,
                                       unsigned int period);

/**
 * \ingroup ObjectBuffers
 *
 * \source_cs       <b>static Canlib.canStatus canObjBufSetMsgCount(CanHandle hnd, Int32 idx, Int32 count);</b>
 *
 * \source_delphi   <b>function canObjBufSetMsgCount(handle: canHandle; idx: Integer; count: Cardinal): canStatus;     </b>
 * \source_end
 *
 * The \ref canObjBufSetMsgCount function sets the message count for an auto
 * transmit object buffer.
 *
 * \param[in] hnd    An open handle to a CAN channel.
 * \param[in] idx    The index of a CAN object buffer.
 * \param[in] count  The message count.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_obj_buf
 */
canStatus CANLIBAPI canObjBufSetMsgCount(const CanHandle hnd,
                                         int idx,
                                         unsigned int count);

/**
 * \ingroup ObjectBuffers
 *
 * \source_cs       <b>static Canlib.canStatus canObjBufEnable(CanHandle hnd, Int32 idx);</b>
 *
 * \source_delphi   <b>function canObjBufEnable(handle: canHandle; idx: Integer): canStatus;     </b>
 * \source_end
 *
 * Enables the object buffer with the specified index.
 *
 * \param[in] hnd  An open handle to a CAN circuit.
 * \param[in] idx  The index of the object buffer to enable.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_obj_buf
 * \sa \ref canObjBufDisable()
 */
canStatus CANLIBAPI canObjBufEnable(const CanHandle hnd, int idx);

/**
 * \ingroup ObjectBuffers
 *
 * \source_cs       <b>static Canlib.canStatus canObjBufDisable(CanHandle hnd, Int32 idx);</b>
 *
 * \source_delphi   <b>function canObjBufDisable(handle: canHandle; idx: Integer): canStatus;     </b>
 * \source_end
 *
 * Disables the object buffer with the specified index.
 *
 * \param[in] hnd  An open handle to a CAN circuit.
 * \param[in] idx  The index of the buffer.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_obj_buf
 * \sa \ref canObjBufEnable()
 */
canStatus CANLIBAPI canObjBufDisable(const CanHandle hnd, int idx);

/**
 * \ingroup ObjectBuffers
 *
 * \source_cs       <b>static Canlib.canStatus canObjBufSendBurst(CanHandle hnd, Int32 idx, Int32 burstlen);</b>
 *
 * \source_delphi   <b>function canObjBufSendBurst(handle: canHandle; idx: Integer; burstLen: Cardinal): canStatus;      </b>
 * \source_end
 *
 * The canObjBufSendBurst function sends a burst of CAN messages. You have to
 * set up an object buffer first with the message to send. The messages will be
 * sent as fast as possible from the hardware.
 *
 * This function is intended for certain diagnostic applications.
 *
 * \param[in] hnd       An open handle to a CAN channel.
 * \param[in] idx       The index of a CAN object buffer.
 * \param[in] burstlen  The number of messages to send.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_obj_buf
 */
canStatus CANLIBAPI canObjBufSendBurst(const CanHandle hnd,
                                       int idx,
                                       unsigned int burstlen);

/**
 * \name canVERSION_xxx
 * \anchor canVERSION_xxx
 *
 * These defines are used in \ref canProbeVersion().
 *
 * @{
 */
  /**
   * This define is used in \ref canProbeVersion(), \a major and \a minor refer to
   * this functions arguments.
   *
   * Require that exactly the version specified by \a major and \a minor be
   * present. Earlier and later versions are not accepted. This flag does not
   * affect the acceptance of beta versions.
   */
#define canVERSION_DONT_ACCEPT_LATER      0x01
  /**
   * This define is used in \ref canProbeVersion(), \a major and \a minor refer to
   * this functions arguments.
   *
   * Return FALSE if a beta version (preview version) of CANlib is installed,
   * regardless of its version number.
   */
#define canVERSION_DONT_ACCEPT_BETAS      0x02
 /** @} */

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static bool canProbeVersion(CanHandle hnd, Int32 major, Int32 minor, Int32 oem_id, Int32 flags);</b>
 *
 * \source_delphi   <b>function canProbeVersion(handle: canHandle; major, minor, oem_id: Integer; flags: Cardinal): Boolean;     </b>
 * \source_end
 *
 * This function checks whether a specific version of CANlib is installed on
 * the system.
 *
 * The default behaviour of \ref canProbeVersion is to accept
 *
 * \li the version specified by \a major and \a minor, and
 * \li any later version, and
 * \li all beta versions.
 *
 * You get the default behaviour by setting \a flags to 0. Use any
 * combination of the \ref canVERSION_xxx flags to modify the behaviour.
 *
 * \note Different handles might have different driver versions installed. This
 * should not normally be the case but it might happen anyway. You should check
 * the version for each handle you open, e.g. directly after calling
 * \ref canOpenChannel().
 *
 * \param[in] hnd     A handle to an open circuit.
 * \param[in] major   The major version number of the version to test for.
 * \param[in] minor   The minor version number of the version to test for.
 * \param[in] oem_id  Reserved, must be zero.
 * \param[in] flags   Any combination of the \ref canVERSION_xxx flags, or 0.
 *
 * \return TRUE if the specified version of CANlib is installed on the system.
 *
 * \sa \ref page_user_guide_version
 * \sa \ref canGetVersion(), \ref canGetVersionEx(), \ref canGetChannelData()
 */
BOOL CANLIBAPI canProbeVersion (const CanHandle hnd,
                                int major,
                                int minor,
                                int oem_id,
                                unsigned int flags);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canResetBus(CanHandle handle);</b>
 *
 * \source_delphi   <b>function canResetBus(handle: canHandle): canStatus;     </b>
 * \source_end
 *
 * This function tries to reset a CAN bus controller by taking the channel off
 * bus and then on bus again (if it was on bus before the call to \ref canResetBus().)
 *
 * This function will affect the hardware (and cause a real reset of the CAN
 * chip) only if \a hnd is the only handle open on the channel. If there
 * are other open handles, this operation will not affect the hardware.
 *
 * \param[in] hnd  A handle to an open circuit.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canBusOn(), \ref canBusOff()
 */
canStatus CANLIBAPI canResetBus(const CanHandle hnd);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canWriteWait(CanHandle handle, Int32 id, Byte[] msg, Int32 dlc, Int32 flag, Int64 timeout);</b>
 *
 * \source_delphi   <b>function canWriteWait(handle: canHandle; id: Longint; var msg; dlc, flag, timeout: Cardinal): canStatus;     </b>
 * \source_end
 *
 * This function sends a CAN message and returns when the message has been
 * successfully transmitted, or the timeout expires.
 *
 * This is a convenience function that combines \ref canWrite() and \ref canWriteSync().
 *
 * If you are using the same channel via multiple handles, note that the
 * default behaviour is that the different handles will "hear" each other just as
 * if each handle referred to a channel of its own. If you open, say, channel 0
 * from thread A and thread B and then send a message from thread A, it will be
 * "received" by thread B.
 * This behaviour can be changed using \ref canIOCTL_SET_LOCAL_TXECHO.
 *
 * \param[in]  hnd       A handle to an open CAN circuit.
 * \param[in]  id        The identifier of the CAN message to send.
 * \param[in]  msg       A pointer to the message data, or \c NULL.
 * \param[in]  dlc       The length of the message in bytes.<br>
                         For Classic CAN dlc can be at most 8, unless \ref
                         canOPEN_ACCEPT_LARGE_DLC is used.<br> For CAN FD dlc
                         can be one of the following 0-8, 12, 16, 20, 24, 32,
                         48, 64.
 * \param[in]  flag      A combination of message flags, \ref canMSG_xxx.
 *                       Use this parameter to send extended (29-bit) frames
 *                       and/or remote frames. Use \ref canMSG_EXT and/or
 *                       \ref canMSG_RTR for this purpose.
 * \param[in] timeout    The timeout, in milliseconds. 0xFFFFFFFF gives an
 *                       infinite timeout.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canIOCTL_SET_TXACK
 */
canStatus CANLIBAPI canWriteWait(const CanHandle hnd,
                                 long id,
                                 void *msg,
                                 unsigned int dlc,
                                 unsigned int flag,
                                 unsigned long timeout);


/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.canStatus canUnloadLibrary();</b>
 *
 * \source_delphi   <b>function canUnloadLibrary: Integer;     </b>
 * \source_end
 *
 * Use this function if you are loading canlib32.dll dynamically (that is,
 * using the Win32 API \c LoadLibrary) and need to unload it using the Win32
 * API \c FreeLibrary. \ref canUnloadLibrary() will free allocated memory, unload
 * the DLLs canlib32.dll has loaded and de-initialize data structures. You must
 * call \ref canInitializeLibrary() again to use the API functions in canlib32.dll.
 *
 * Calling \ref canUnloadLibrary() makes any open \ref CanHandle invalid. The proper
 * procedure for using \ref canUnloadLibrary() is:
 *
 *   -# Close all active handles using \ref canBusOff() and \ref canClose().
 *   -# At this point all handles earlier returned by \ref canOpenChannel() should be freed.
 *   -# Call \ref canUnloadLibrary().
 *   -# Call \ref canInitializeLibrary().
 *   -# You can now setup your handles to Kvaser devices from scratch
 *      (ie. \ref canOpenChannel(), \ref canSetBusParams(), \ref canBusOn(), etc).
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canInitializeLibrary()
 */
canStatus CANLIBAPI canUnloadLibrary(void);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canSetAcceptanceFilter(CanHandle hnd, Int32 code, Int32 mask, Int32 is_extended);</b>
 *
 * \source_delphi   <b>function canSetAcceptanceFilter(handle: canHandle; code, mask: Cardinal; is_extended: Integer): canStatus;     </b>
 * \source_end
 *
 * This routine sets the message acceptance filters on a CAN channel.
 *
 * Format of \a code and \a mask:
 *
 * \li A binary 1 in a mask means "the corresponding bit in the code is
 *     relevant"
 * \li A binary 0 in a mask means "the corresponding bit in the code is not
 *     relevant"
 * \li A relevant binary 1 in a code means "the corresponding bit in the
 *     identifier must be 1"
 * \li A relevant binary 0 in a code means "the corresponding bit in the
 *     identifier must be 0"
 *
 * In other words, the message is accepted if ((code XOR id) AND mask) == 0.
 *
 * \a extended should be set to:
 *
 * \li \c 0 (FALSE): if the code and mask shall apply to 11-bit CAN identifiers.
 * \li \c 1 (TRUE): if the code and mask shall apply to 29-bit CAN identifiers.
 *
 * If you want to remove a filter, call \ref canSetAcceptanceFilter() with the mask
 * set to \c 0.
 *
 * On some boards the acceptance filtering is done by the CAN hardware; on
 * other boards (typically those with an embedded CPU,) the acceptance
 * filtering is done by software.  \ref canSetAcceptanceFilter() behaves in the same
 * way for all boards, however.
 *
 * \ref canSetAcceptanceFilter() and \ref canAccept() both serve the same purpose but the
 * former can set the code and mask in just one call.
 *
 * \note You can set the extended code and mask only on CAN boards that support
 *       extended identifiers.
 *
 * \note Acceptance filters for 11-bit and 29-bit (ext) are independent, meaning 11-bit filters
 *  will not affect 29-bit can frames and vice versa.
 *
 * \note Not all CAN boards support different masks for standard and
 *       extended CAN identifiers.
 *
 * \note Not implemented in linux.
 *
 * \param[in] hnd          An open handle to a CAN circuit.
 * \param[in] code         The acceptance code to set.
 * \param[in] mask         The acceptance mask to set.
 * \param[in] is_extended  Select 29-bit CAN identifiers.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_send_recv_filters
 * \sa  \ref canAccept()
 */
canStatus CANLIBAPI canSetAcceptanceFilter(const CanHandle hnd,
                                           unsigned int code,
                                           unsigned int mask,
                                           int is_extended);
/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canFlushReceiveQueue(CanHandle hnd);</b>
 *
 * \source_delphi   <b>function canFlushReceiveQueue(handle: canHandle): canStatus;     </b>
 * \source_end
 *
 * This function removes all received messages from the handle's receive queue.
 * Other handles open to the same channel are not affected by this
 * operation. That is, only the messages belonging to the handle you are
 * passing to \ref canFlushReceiveQueue are discarded.
 *
 * \note This call has the same effect as calling \ref canIoCtl() with a function
 * code of \ref canIOCTL_FLUSH_RX_BUFFER.
 *
 * \param[in] hnd  A handle to an open circuit.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canFlushTransmitQueue()
 */
canStatus CANLIBAPI canFlushReceiveQueue(const CanHandle hnd);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canFlushTransmitQueue(CanHandle hnd);</b>
 *
 * \source_delphi   <b>function canFlushTransmitQueue(handle: canHandle): canStatus;     </b>
 * \source_end
 *
 * This function removes all messages pending transmission from the
 * transmit queue of the circuit.
 *
 * \note If there are other handles open to the same circuit, they are also
 * flushed.
 *
 * \note This call has the same effect as calling \ref canIoCtl() with a function
 * code of \ref canIOCTL_FLUSH_TX_BUFFER.
 *
 * \param[in] hnd  A handle to an open circuit.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canFlushReceiveQueue()
 */
canStatus CANLIBAPI canFlushTransmitQueue(const CanHandle hnd);

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.kvStatus kvGetApplicationMapping(Int32 busType, String appName, Int32 appChannel, out Int32 resultingChannel);</b>
 *
 * \source_delphi   <b>function kvGetApplicationMapping(busType: Integer; appName: PAnsiChar; appChannel: Integer; var resultingChannel: Integer): canStatus;     </b>
 * \source_end
 *
 * \note The \ref kvGetApplicationMapping function is presently not implemented.
 *
 * \param busType
 * \param appName
 * \param appChannel
 * \param resultingChannel
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 */
canStatus CANLIBAPI kvGetApplicationMapping(int busType,
                                            char *appName,
                                            int appChannel,
                                            int *resultingChannel);

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.kvStatus kvBeep(CanHandle hnd, Int32 freq, Int32 duration);</b>
 *
 * \source_delphi   <b>function kvBeep(handle: canHandle; freq: Integer; duration: Cardinal): canStatus;     </b>
 * \source_end
 *
 * The \ref kvBeep function emits a sound of a specific frequency and duration from
 * the loudspeaker on the device.
 *
 * \note This function requires that a loudspeaker be present on the hardware.
 *
 * \param[in] hnd       An open handle to a CAN channel.
 * \param[in] freq      The frequency (in Hertz) of the sound.
 * \param[in] duration  The duration of the sound, in milliseconds.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 */
canStatus CANLIBAPI kvBeep(const CanHandle hnd,
                           int freq,
                           unsigned int duration);

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.kvStatus kvSelfTest(CanHandle hnd, out Int32 presults);</b>
 *
 * \source_delphi   <b>function kvSelfTest(handle: canHandle; var presults: Cardinal): canStatus;     </b>
 * \source_end
 *
 * The \ref kvSelfTest function runs a built-in self test in the device. Note that
 * not all devices supports built-in self tests.
 *
 * \param[in]  hnd       An open hnd to a CAN channel.
 * \param[out] presults  A pointer to a 32-bit unsigned integer where the
 *                       results of the self test will be placed.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 */
canStatus CANLIBAPI kvSelfTest(const CanHandle hnd, unsigned long *presults);

  /**
   * \anchor kvLED_ACTION_xxx
   * \name kvLED_ACTION_xxx
   *
   * The following constants can be used together with the \ref kvFlashLeds() function.
   *
   * @{
   */
#define kvLED_ACTION_ALL_LEDS_ON   0 ///< Turn all LEDs on.
#define kvLED_ACTION_ALL_LEDS_OFF  1 ///< Turn all LEDs off.
#define kvLED_ACTION_LED_0_ON      2 ///< Turn LED 0 on.
#define kvLED_ACTION_LED_0_OFF     3 ///< Turn LED 0 off.
#define kvLED_ACTION_LED_1_ON      4 ///< Turn LED 1 on.
#define kvLED_ACTION_LED_1_OFF     5 ///< Turn LED 1 off.
#define kvLED_ACTION_LED_2_ON      6 ///< Turn LED 2 on.
#define kvLED_ACTION_LED_2_OFF     7 ///< Turn LED 2 off.
#define kvLED_ACTION_LED_3_ON      8 ///< Turn LED 3 on.
#define kvLED_ACTION_LED_3_OFF     9 ///< Turn LED 3 off.
#define kvLED_ACTION_LED_4_ON     10 ///< Turn LED 4 on.
#define kvLED_ACTION_LED_4_OFF    11 ///< Turn LED 4 off.
#define kvLED_ACTION_LED_5_ON     12 ///< Turn LED 5 on.
#define kvLED_ACTION_LED_5_OFF    13 ///< Turn LED 5 off.
#define kvLED_ACTION_LED_6_ON     14 ///< Turn LED 6 on.
#define kvLED_ACTION_LED_6_OFF    15 ///< Turn LED 6 off.
#define kvLED_ACTION_LED_7_ON     16 ///< Turn LED 7 on.
#define kvLED_ACTION_LED_7_OFF    17 ///< Turn LED 7 off.
#define kvLED_ACTION_LED_8_ON     18 ///< Turn LED 8 on.
#define kvLED_ACTION_LED_8_OFF    19 ///< Turn LED 8 off.
#define kvLED_ACTION_LED_9_ON     20 ///< Turn LED 9 on.
#define kvLED_ACTION_LED_9_OFF    21 ///< Turn LED 9 off.
#define kvLED_ACTION_LED_10_ON    22 ///< Turn LED 10 on.
#define kvLED_ACTION_LED_10_OFF   23 ///< Turn LED 10 off.
#define kvLED_ACTION_LED_11_ON    24 ///< Turn LED 11 on.
#define kvLED_ACTION_LED_11_OFF   25 ///< Turn LED 11 off.

  /** @} */

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.kvStatus kvFlashLeds(CanHandle hnd, Int32 action, Int32 timeout);</b>
 *
 * \source_delphi   <b>function kvFlashLeds(handle: canHandle; action: Integer; timeout: Integer): canStatus;     </b>
 * \source_end
 *
 * The \ref kvFlashLeds function will turn the LEDs on the device on or off.
 *
 * \param[in] hnd
 * \param[in] action   One of the \ref kvLED_ACTION_xxx constants, defining
 *                     which LED to turn on or off.
 * \param[in] timeout  Specifies the time, in milliseconds, during which the
 *                     action is to be carried out. When the timeout expires,
 *                     the LED(s) will return to its ordinary function.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 */
canStatus CANLIBAPI kvFlashLeds (const CanHandle hnd, int action, int timeout);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canRequestChipStatus(CanHandle hnd);</b>
 *
 * \source_delphi   <b>function canRequestChipStatus(handle: canHandle): canStatus;     </b>
 * \source_end
 *
 * The canRequestChipStatus function requests that the hardware report the chip
 * status (bus on/error passive status etc.) to the driver. The chip status can
 * later be retrieved using the \ref canReadStatus() function.
 *
 * \note The \ref canRequestChipStatus() function is asynchronous, that is, it
 * completes before the answer is returned from the hardware.  The time between
 * a call to \ref canRequestChipStatus() and the point in time where the chip status
 * is actually available via a call to \ref canReadStatus() is not
 * defined. \ref canReadStatus() always returns the latest data reported by the
 * hardware.
 *
 * \param[in] hnd An open handle to a CAN channel.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canReadStatus()
 */
canStatus CANLIBAPI canRequestChipStatus(const CanHandle hnd);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canRequestBusStatistics(CanHandle hnd);</b>
 *
 * \source_delphi   <b>function canRequestBusStatistics(handle: canHandle): canStatus;     </b>
 * \source_end
 *
 * The \ref canRequestBusStatistics function requests bus statistics from the
 * hardware. The bus statistics figures can be retrieved later by a call to the
 * \ref canGetBusStatistics function.
 *
 * \note The time between a call to \ref canRequestBusStatistics() and the point in
 * time where the bus statistics is actually available via a call to
 * \ref canGetBusStatistics() is not defined. Typically, you would call
 * \ref canRequestBusStatistics() from your application periodically (for example,
 * once per second) to request the data from the driver and then call
 * \ref canGetBusStatistics() with the same rate to obtain the latest reported data.
 *
 * \param[in] hnd   An open handle to a CAN channel.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canGetBusStatistics()
 */
canStatus CANLIBAPI canRequestBusStatistics (const CanHandle hnd);

  /**
   * This struct is returned by \ref canGetBusStatistics()
   *
   * The values are cleared when the corresponding channel goes on bus.
   */
typedef struct canBusStatistics_s {
    unsigned long stdData;   ///< Number of received standard (11-bit identifiers) data frames.
    unsigned long stdRemote; ///< Number of received standard (11-bit identifiers) remote frames.
    unsigned long extData;   ///< Number of received extended (29-bit identifiers) data frames.
    unsigned long extRemote; ///< Number of received extended (29-bit identifiers) remote frames.
    unsigned long errFrame;  ///< Number of error frames

    /**
     * The bus load, expressed as an integer in the interval 0 - 10000
     * representing 0.00% - 100.00% bus load.
     */
    unsigned long busLoad;
    unsigned long overruns;  ///< The number of overruns detected by the hardware, firmware or driver.
} canBusStatistics;

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canGetBusStatistics(CanHandle hnd, out Canlib.canBusStatistics stat);</b>
 *
 * \source_delphi   <b>function canGetBusStatistics(handle: canHandle; var stat: canBusStatistics; bufsiz: size_t): canStatus;     </b>
 * \source_end
 *
 * The \ref canGetBusStatistics() function retrieves the latest bus statistics
 * figures reported by the driver.  You request the bus statistics from the
 * driver by calling the \ref canRequestBusStatistics() function.
 *
 * \param[in]  hnd     An open handle to a CAN channel.
 * \param[out] stat    A pointer to a \ref canBusStatistics struct that will receive
 *                     the bus statistics figures.
 * \param[in]  bufsiz  The size, in bytes, of the stat buffer.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canRequestBusStatistics()
 */
canStatus CANLIBAPI canGetBusStatistics(const CanHandle hnd,
                                        canBusStatistics *stat,
                                        size_t bufsiz);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canSetBitrate(CanHandle hnd, Int32 bitrate);</b>
 *
 * \source_delphi   <b>function canSetBitrate(handle: canHandle; bitrate: Integer): canStatus;     </b>
 * \source_end
 *
 * The \ref canSetBitrate() function sets the nominal bit rate of the specified
 * CAN channel. The sampling point is recalculated and kept as close as
 * possible to the value before the call.
 *
 * \param[in] hnd      An open handle to a CAN channel.
 * \param[in] bitrate  The new bit rate, in bits/second.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canSetBusParamsC200(), \ref canSetBusParams(), \ref canGetBusParams()
 */
canStatus CANLIBAPI canSetBitrate(const CanHandle hnd, int bitrate);

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.kvStatus kvAnnounceIdentity(CanHandle hnd, Object buffer);</b>
 *
 * \source_delphi   <b>function kvAnnounceIdentity(handle: canHandle; var buf; bufsiz: size_t): canStatus;     </b>
 * \source_end
 *
 * The \ref kvAnnounceIdentity function is used by certain OEM applications.
 *
 * \param[in]  hnd     An open handle to a CAN channel.
 * \param[out] buf     A pointer to the data to be sent to the driver.
 * \param[in]  bufsiz  The size, in bytes, of the buffer that buf points to.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 */
canStatus CANLIBAPI kvAnnounceIdentity(const CanHandle hnd,
                                       void *buf,
                                       size_t bufsiz);

 /**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.kvStatus kvAnnounceIdentityEx(CanHandle hnd, Int32 type, Byte[] data);</b>
 *
 * \source_delphi   <b>function kvAnnounceIdentityEx(handle: canHandle; tp: Integer; var buf; bufsiz: size_t): canStatus;     </b>
 *
 * \source_end
 * The \ref kvAnnounceIdentityEx function is used by certain OEM applications.
 *
 * \note Not implemented in linux.
 *
 * \param[in]  hnd     An open handle to a CAN channel.
 * \param[in]  type    Type of announcement.
 * \param[out] buf     A pointer to the data to be sent to the driver.
 * \param[in]  bufsiz  The size, in bytes, of the buffer that buf points to.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 */
canStatus CANLIBAPI kvAnnounceIdentityEx(const CanHandle hnd,
                                         int type,
                                         void *buf,
                                         size_t bufsiz);

/**
 * \ingroup CAN
 *
 * \source_cs       <b>static Canlib.canStatus canGetHandleData(CanHandle hnd, Int32 item, out object buffer);</b>
 *
 * \source_delphi   <b>function canGetHandleData(handle: canHandle; item: Integer; var Buffer; bufsize: size_t): canStatus;     </b>
 * \source_end
 *
 * \note This function can be used to retrieve certain pieces of information about an open handle to a CANlib channel.
 *
 * \param[in]  hnd      An open handle to a CAN channel.
 * \param[in]  item     This parameter specifies what data to obtain for the specified handle. The value is one of the constants \ref canCHANNELDATA_xxx
 * \param[out] buffer   The address of a buffer which is to receive the data.
 * \param[in]  bufsize  The size of the buffer to which the buffer parameter points.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_unique_device
 * \sa \ref canGetChannelData()
 */
canStatus CANLIBAPI canGetHandleData(const CanHandle hnd,
                                     int item,
                                     void *buffer,
                                     size_t bufsize);

/** Used for time domain handling. */
typedef void *kvTimeDomain;

/** Contains status codes according to \ref canSTAT_xxx. */
typedef canStatus kvStatus;

/**
 *
 * Used for time domain handling.
 */
typedef struct kvTimeDomainData_s {
    int nMagiSyncGroups;       ///< number of MagiSync&tm; groups
    int nMagiSyncedMembers;    ///< number of MagiSync&tm; members
    int nNonMagiSyncCards;     ///< number of non MagiSync&tm; interfaces
    int nNonMagiSyncedMembers; ///< number of non MagiSync&tm; members
} kvTimeDomainData;

/**
 * \ingroup TimeDomainHandling
 *
 * \source_cs       <b>static Canlib.kvStatus kvTimeDomainCreate(out object domain);</b>
 *
 * \source_delphi   <b>function kvTimeDomainCreate(var domain: kvTimeDomain): kvStatus;     </b>
 * \source_end
 *
 * This routine creates an empty time domain.
 *
 * The variable is set by this function and then used in later calls to
 * other functions using a \ref kvTimeDomain.
 *
 * Time domains created by \ref kvTimeDomainCreate() can be destroyed with a
 * call to \ref kvTimeDomainDelete().
 *
 * \note A time domain is a set of channels with a common time base.
 *
 * \param[out] domain  A pointer to a caller allocated, opaque variable of type
 *                     \ref kvTimeDomain that holds data to identify a particular
 *                     time domain.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_time_domain
 * \sa \ref kvTimeDomainDelete()
 */
kvStatus CANLIBAPI kvTimeDomainCreate(kvTimeDomain *domain);

/**
 * \ingroup TimeDomainHandling
 *
 * \source_cs       <b>static Canlib.kvStatus kvTimeDomainDelete(object domain);</b>
 *
 * \source_delphi   <b>function kvTimeDomainDelete(domain: kvTimeDomain): kvStatus;     </b>
 * \source_end
 *
 * This is a cleanup routine that deletes all members of a domain and then
 * deletes the domain itself.
 *
 * \note A time domain is a set of channels with a common time base.
 *
 * \param[in] domain  An opaque variable set by \ref kvTimeDomainCreate() that
 *                    identifies the domain to be deleted.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_time_domain
 * \sa \ref kvTimeDomainCreate()
 */
kvStatus CANLIBAPI kvTimeDomainDelete(kvTimeDomain domain);

/**
 * \ingroup TimeDomainHandling
 *
 * \source_cs       <b>static Canlib.kvStatus kvTimeDomainResetTime(object domain);</b>
 *
 * \source_delphi   <b>function kvTimeDomainResetTime(domain: kvTimeDomain): kvStatus;     </b>
 * \source_end
 *
 * This routine resets the time on all members of a time domain.
 *
 * After a call to this routine timestamps from all channels with MagiSync&tm;
 * running have no offset at all any longer. The same applies for channels that
 * reside on the same physical interface.
 *
 * \note A time domain is a set of channels with a common time base.
 *
 * \param[in] domain  An opaque variable set by \ref kvTimeDomainCreate() that
 *                    identifies the domain to reset the time on.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_time_domain
 * \sa \ref kvTimeDomainCreate()
 */
kvStatus CANLIBAPI kvTimeDomainResetTime(kvTimeDomain domain);

/**
 * \ingroup TimeDomainHandling
 *
 * \source_cs       <b>static Canlib.kvStatus kvTimeDomainGetData(object domain, Canlib.kvTimeDomainData data);</b>
 *
 * \source_delphi   <b>function kvTimeDomainGetData(domain: kvTimeDomain; var data: kvTimeDomainData; bufsiz: size_t): kvStatus;     </b>
 * \source_end
 *
 * This routine collects some data on a time domain.
 *
 * \note A time domain is a set of channels with a common time base.
 *
 * \param[in]  domain  An opaque variable set by \ref kvTimeDomainCreate() that
 *                     identifies the domain to add a handle to.
 * \param[out] data    A pointer to a \ref kvTimeDomainData that is to be filled by
 *                     the function.
 * \param[in]  bufsiz  The size in bytes of the \ref kvTimeDomainData struct.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_time_domain
 * \sa \ref kvTimeDomainCreate()
 */
kvStatus CANLIBAPI kvTimeDomainGetData(kvTimeDomain domain,
                                       kvTimeDomainData *data,
                                       size_t bufsiz);

/**
 * \ingroup TimeDomainHandling
 *
 * \source_cs       <b>static Canlib.kvStatus kvTimeDomainAddHandle(object domain, CanHandle hnd);</b>
 *
 * \source_delphi   <b>function kvTimeDomainAddHandle(domain: kvTimeDomain; handle: canHandle): kvStatus;     </b>
 * \source_end
 *
 * This routine adds an open channel handle to a domain.
 *
 * \note A time domain is a set of channels with a common time base.
 *
 * \param[in] domain  An opaque variable set by \ref kvTimeDomainCreate()
 *                    that identifies the domain to add a handle to.
 * \param[in] hnd     A handle to an open channel.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_time_domain
 * \sa \ref kvTimeDomainCreate(), \ref kvTimeDomainRemoveHandle()
 */
kvStatus CANLIBAPI kvTimeDomainAddHandle(kvTimeDomain domain,
                                         const CanHandle hnd);

/**
 * \ingroup TimeDomainHandling
 *
 * \source_cs       <b>static Canlib.kvStatus kvTimeDomainRemoveHandle(object domain, CanHandle hnd);</b>
 *
 * \source_delphi   <b>function kvTimeDomainRemoveHandle(domain: kvTimeDomain; handle: canHandle): kvStatus;     </b>
 * \source_end
 *
 * This routine removes an open channel handle from a domain.
 *
 * \note A time domain is a set of channels with a common time base.
 *
 * \param[in] domain  An opaque variable set by \ref kvTimeDomainCreate()
 *                    that identifies the domain to remove a handle from.
 * \param[in] hnd     A handle to an open channel.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref kvTimeDomainCreate(), \ref kvTimeDomainAddHandle()
 */
kvStatus CANLIBAPI kvTimeDomainRemoveHandle(kvTimeDomain domain,
                                            const CanHandle hnd);

/**
 * \ref kvCallback_t is used by the function \ref kvSetNotifyCallback()
 *
 * The callback function, on Windows required to be a \c __stdcall function, should be called with the following arguments:
 *
 * \li hnd - the handle of the CAN channel where the event happened.
 * \li context - the context pointer you passed to \ref kvSetNotifyCallback().
 * \li notifyEvent - one of the \ref canNOTIFY_xxx notification codes.
 *
 * \note It is really the \ref canNOTIFY_xxx codes, and not the
 *  \ref canEVENT_xxx codes that the \ref canSetNotify() API is using.
 *
 * \note The default calling convention for C/C++ programs on Windows is \c __cdecl.
 * <br>On Linux, the default calling convention is used.
 *
 * \sa \ref section_user_guide_send_recv_asynch_callback
 */
typedef void(__stdcall *kvCallback_t)(CanHandle hnd, void *context, unsigned int notifyEvent);
/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.kvStatus kvSetNotifyCallback(CanHandle hnd, Canlib.kvCallbackDelegate callback, IntPtr context, Int32 notifyFlags);</b>
 *
 * \source_delphi   <b>function kvSetNotifyCallback(handle: canHandle; callback: kvCallback_t; context: Pointer; notifyFlags: Cardinal): canStatus;     </b>
 * \source_end
 *
 * The \ref kvSetNotifyCallback() function registers a callback function which is
 * called when certain events occur.
 *
 * You can register at most one callback function per handle at any time.
 *
 * To remove the callback, call \ref kvSetNotifyCallback() with a \c NULL pointer in
 * the callback argument.
 *
 * \note The callback function is called in the context of a high-priority
 * thread created by CANlib. You should take precaution not to do any time
 * consuming tasks in the callback.  You must also arrange the synchronization
 * between the callback and your other threads yourself.
 *
 * \param[in] hnd          An open handle to a CAN channel.
 * \param[in] callback     A pointer to a callback function of type
 *                         \ref kvCallback_t
 * \param[in] context      A pointer to arbitrary user-defined context data which
 *                         is passed to the callback function.
 * \param[in] notifyFlags  One or more of the \ref canNOTIFY_xxx flags.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canSetNotify()
 */
kvStatus CANLIBAPI kvSetNotifyCallback(const CanHandle hnd,
                                       kvCallback_t callback,
                                       void *context,
                                       unsigned int notifyFlags);

/**
 * \name kvBUSTYPE_xxx
 * \anchor kvBUSTYPE_xxx
 *
 *  Bus types, returned by \ref kvGetSupportedInterfaceInfo().
 * \note Not inplemented in linux.
 * @{
 */

/**
* Unknown bus type.
* \note Not inplemented in linux.
*/
#define kvBUSTYPE_NONE          0

/**
* Bus of type PCI.
* \note Not inplemented in linux.
*/
#define kvBUSTYPE_PCI           1

/**
* Bus of type PCMCIA
* \note Not inplemented in linux.
*/
#define kvBUSTYPE_PCMCIA        2

/**
* Bus of type USB
* \note Not inplemented in linux.
*/
#define kvBUSTYPE_USB           3

/**
* Bus of type WLAN
* \note Not inplemented in linux.
*/
#define kvBUSTYPE_WLAN          4

/**
* Bus of type PCI Express
* \note Not inplemented in linux.
*/
#define kvBUSTYPE_PCI_EXPRESS   5

/**
* Bus of type ISA
* \note Not inplemented in linux.
*/
#define kvBUSTYPE_ISA           6

/**
* Bus of type virtual
* \note Not inplemented in linux.
*/
#define kvBUSTYPE_VIRTUAL       7

/**
* Bus of type PC104+
* \note Not inplemented in linux.
*/
#define kvBUSTYPE_PC104_PLUS    8

/**
* Bus of type LAN
* \note Not inplemented in linux.
*/
#define kvBUSTYPE_LAN           9
/** @} */

/**
 * \name kvBUSTYPE_GROUP_xxx
 * \anchor kvBUSTYPE_GROUP_xxx
 *
 * Bus type group, returned when using \ref canCHANNELDATA_BUS_TYPE
 * This is a grouping of the individual \ref kvBUSTYPE_xxx.
 * \note Not implemented in linux.
 * @{
 */

/**
* \ref kvBUSTYPE_VIRTUAL
* \note Not implemented in linux.
*/
#define kvBUSTYPE_GROUP_VIRTUAL  1

/**
* \ref kvBUSTYPE_USB
* \note Not implemented in linux.
*/
#define kvBUSTYPE_GROUP_LOCAL    2

/**
* \ref kvBUSTYPE_WLAN, \ref kvBUSTYPE_LAN
* \note Not implemented in linux.
*/
#define kvBUSTYPE_GROUP_REMOTE   3
/**
* \ref kvBUSTYPE_PCI, \ref kvBUSTYPE_PCMCIA, ...
* \note Not implemented in linux.
*/
#define kvBUSTYPE_GROUP_INTERNAL 4
/** @} */

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.kvStatus kvGetSupportedInterfaceInfo(Int32 index, out String hwName, out Int32 hwType, out Int32 hwBusType);</b>
 *
 * \source_delphi   <b>function kvGetSupportedInterfaceInfo(index: Integer; hwName: PAnsiChar; nameLen: size_t; var hwType: Integer; var hwBusType: Integer): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvGetSupportedInterfaceInfo function returns information about the
 * different supported hardware types in the installed version of CANlib.
 *
 * This function is used to enumerate all the supported hardware types in the
 * installed version of CANlib. It does not return a complete list of all
 * supported devices, nor does it return a list of the presently installed
 * hardware. The returned data indicates which device families are supported.
 *
 * For example, a returned set of data might be:
 * \li \a hwType = \ref canHWTYPE_MEMORATOR_PRO
 * \li \a hwBusType = \ref kvBUSTYPE_USB
 * \li \a hwName = \c "Kvaser Memorator Professional"
 *
 * This means that
 * \li the presently installed version of CANlib supports members in the Kvaser
 *     Memorator Pro family (e.g. the HS/HS and the HS/LS),
 * \li the members of the Kvaser Memorator Pro family are USB devices,
 * \li the members of the Kvaser Memorator Pro family use the
 *     \ref canHWTYPE_MEMORATOR_PRO hardware type.
 *
 * The \ref kvGetSupportedInterfaceInfo() function is intended to help
 * application designers build a bus-oriented display of the different
 * installed and/or supported Kvaser devices in the computer.
 *
 * \note Not inplemented in linux.
 *
 * \param[in] index       Use this parameter to enumerate the different supported
 *                        hardware types. Start with index = 0, and then call
 *                        \ref kvGetSupportedInterfaceInfo again() with index =
 *                        1,2,3,... until the function returns an error code.
 * \param[out] hwName     A pointer to a buffer that will receive the name of the
 *                        hardware family, as a zero-terminated ASCII string.
 * \param[in]  nameLen    The length of the hwName buffer.
 * \param[out] hwType     Pointer to a 32-bit integer that will receive the
 *                        hardware type (one of the \ref canHWTYPE_xxx
 *                        constants.)
 * \param[out] hwBusType  Pointer to a 32-bit integer that will receive the bus
 *                        type (one of the \ref kvBUSTYPE_xxx constants.)
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canBusOn(), \ref canResetBus()
 */
kvStatus CANLIBAPI kvGetSupportedInterfaceInfo(int index,
                                               char *hwName,
                                               size_t nameLen,
                                               int *hwType,
                                               int *hwBusType);

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.kvStatus kvReadDeviceCustomerData(CanHandle hnd, Int32 userNumber, Int32 itemNumber, Byte[] data, Int32 bufsize);</b>
 *
 * \source_delphi   <b>function kvReadDeviceCustomerData(hnd: canHandle; userNumber, itemNumber: Integer; var data; bufsize: size_t): kvStatus;     </b>
 * \source_end
 *
 * Reading customer data works with Kvaser Leaf (of all
 * types), Kvaser USBcan Professional, Kvaser Memorator Professional, Kvaser Eagle and
 * Kvaser Memorator Light. To write customer data use external tools.
 *
 * \note Not implemented in linux.
 *
 * \param[in]  hnd        An open handle to a CAN channel.
 * \param[in]  userNumber Assigned by Kvaser.
 * \param[in]  itemNumber Must be zero (reserved)
 * \param[out] data       A pointer to a buffer of up to 8 bytes where
 *                        the result will be placed.
 * \param[in]  bufsiz     The size of the buffer that data points at.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 */
kvStatus CANLIBAPI kvReadDeviceCustomerData(const CanHandle hnd,
                                            int userNumber,
                                            int itemNumber,
                                            void *data,
                                            size_t bufsiz);

/**
 * \ingroup tScript
 * \name kvENVVAR_TYPE_xxx
 * \anchor kvENVVAR_TYPE_xxx
 *
 * These defines are used in \ref kvScriptEnvvarOpen().
 *  @{
 */
  /**
   * This define is used in \ref kvScriptEnvvarOpen().
   *
   * It defines the type of the envvar as \c int.
   */
#define kvENVVAR_TYPE_INT       1

  /**
   * This define is used in \ref kvScriptEnvvarOpen().
   *
   * It defines the type of the envvar as \c float.
   */
#define kvENVVAR_TYPE_FLOAT     2

  /**
   * This define is used in \ref kvScriptEnvvarOpen().
   *
   * It defines the type of the envvar as \c string.
   */
#define kvENVVAR_TYPE_STRING    3
 /** @} */

/**
 * \ingroup tScript
 * \name kvEVENT_xxx
 * \anchor kvEVENT_xxx
 *
 * These defines are used in \ref kvScriptSendEvent().
 *  @{
 */

  /**
   * This define is used in  \ref kvScriptSendEvent().
   *
   * It defines an event of type "key pressed".
   */
#define kvEVENT_TYPE_KEY        1
 /** @} */

  /**
   *
   * A handle to a t-script envvar.
   * Returned by the function \ref kvScriptEnvvarOpen().
   */

typedef __int64 kvEnvHandle;

/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptStart(CanHandle hnd, Int32 slotNo);</b>
 *
 * \source_delphi   <b>function kvScriptStart(const hnd: canHandle; slotNo: integer): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvScriptStart() function starts a loaded script.
 *
 * \param[in] hnd     An open handle to a CAN channel.
 * \param[in] slotNo  The slot with the loaded script we want to start.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_kvscript_start_stop
 * \sa \ref kvScriptLoadFile(), \ref kvScriptStop()
 */
kvStatus CANLIBAPI kvScriptStart(const CanHandle hnd, int slotNo);

/**
 * Script stop modes. Used by \ref kvScriptStop().
 * \anchor kvSCRIPT_STOP_xxx
 * \name kvSCRIPT_STOP_xxx
 * @{
 */
#define kvSCRIPT_STOP_NORMAL         0 /**< Stop a running script */
#define kvSCRIPT_STOP_FORCED        -9 /**< Request termination of a hanged script */
/** @} */


/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptStop(CanHandle hnd, Int32 slotNo, Int32 mode);</b>
 *
 * \source_delphi   <b>function kvScriptStop(const hnd: canHandle; slotNo: integer; mode: integer): kvStatus;     </b>
 * \source_end
 *
 *  The \ref kvScriptStop() function stops a started script.
 *
 * \param[in] hnd     An open handle to a CAN channel.
 * \param[in] slotNo  The slot with the loaded and running script we want to stop.
 * \param[in] mode    Stop mode of type \ref kvSCRIPT_STOP_xxx
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_kvscript_start_stop
 * \sa \ref kvScriptLoadFile(), \ref kvScriptStart()
 */
kvStatus CANLIBAPI kvScriptStop(const CanHandle hnd, int slotNo, int mode);

/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptUnload(CanHandle hnd, Int32 slotNo);</b>
 *
 * \source_delphi   <b>function kvScriptUnload(const hnd: canHandle; slotNo: integer): kvStatus;     </b>
 * \source_end
 *
 *  The \ref kvScriptUnload() function unloads a stopped script.
 *
 * \param[in] hnd     An open handle to a CAN channel.
 * \param[in] slotNo  The slot with the loaded and stopped script we want to unload.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_kvscript_loading
 * \sa \ref kvScriptLoadFile(), \ref kvScriptStop()
 */
kvStatus CANLIBAPI kvScriptUnload(const CanHandle hnd, int slotNo);

/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptSendEvent(CanHandle hnd, Int32 slotNo, Int32 eventType, Int32 eventNo, Int32 data);</b>
 *
 * \source_delphi   <b>function kvScriptSendEvent(const hnd: canHandle; slotNo: integer; eventType: integer; eventNo: integer; data: Cardinal): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvScriptSendEvent() function sends an event of a type, and an event
 * number and associated data to a script running in a specific slot.
 *
 * \note Not implemented in linux.
 *
 * \param[in] hnd        An open handle to a CAN channel.
 * \param[in] slotNo     The slot where the script was loaded and is running.
 * \param[in] eventType  The event to send, of type \ref kvEVENT_xxx
 * \param[in] eventNo    The event's number.
 * \param[in] data       The event's data.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_kvscript_send_event
 */
kvStatus CANLIBAPI kvScriptSendEvent(const CanHandle hnd,
                                     int slotNo,
                                     int eventType,
                                     int eventNo,
                                     unsigned int data);

/**
 * \ingroup tScript
 *
 * \source_cs       <b>static kvEnvHandle kvScriptEnvvarOpen(CanHandle hnd, String envvarName, out Int32 envvarType, out Int32 envvarSize);</b>
 *
 * \source_delphi   <b>function kvScriptEnvvarOpen(const hnd: canHandle; envvarName: PAnsiChar; var envvarType: Integer; var envvarSize: Integer): kvEnvHandle;     </b>
 * \source_end
 *
 * The \ref kvScriptEnvvarOpen() opens an existing envvar and returns a handle to it.
 *
 * \param[in]  hnd         An open handle to a CAN channel.
 * \param[in]  envvarName  The envvar's name; a pointer to a \c NULL terminated
 *                         array of chars.
 * \param[out] envvarType  A pointer to a 32-bit integer that will receive the
 *                         \ref kvENVVAR_TYPE_xxx type.
 * \param[out] envvarSize  A pointer to a 32-bit integer that will receive the
 *                         size of the envvar in bytes.
 *
 * \return A \ref kvEnvHandle handle (positive) to an envvar if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_kvscript_envvar
 * \sa \ref kvScriptEnvvarClose()
 */
kvEnvHandle CANLIBAPI kvScriptEnvvarOpen(const CanHandle hnd,
                                         const char *envvarName,
                                         int *envvarType,
                                         int *envvarSize); // returns scriptHandle

/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptEnvvarClose(kvEnvHandle eHnd);</b>
 *
 * \source_delphi   <b>function kvScriptEnvvarClose(const eHnd: kvEnvHandle): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvScriptEnvvarClose() function closes an open envvar.
 *
 * \param[in] eHnd  An open handle to an envvar.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref kvScriptEnvvarOpen()
 */
kvStatus CANLIBAPI kvScriptEnvvarClose(kvEnvHandle eHnd);

/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptEnvvarSetInt(kvEnvHandle eHnd, Int32 val);</b>
 *
 * \source_delphi   <b>function kvScriptEnvvarSetInt(const eHnd: kvEnvHandle; val: Integer): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvScriptEnvvarSetInt() sets the value of an \c int envvar.
 *
 * \param[in] eHnd  An open handle to an envvar.
 * \param[in] val   The new value.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_kvscript_envvar
 * \sa \ref kvScriptEnvvarOpen(), \ref kvScriptEnvvarGetInt(), \ref kvScriptEnvvarSetFloat(),
 * \ref kvScriptEnvvarSetData()
 */
kvStatus CANLIBAPI kvScriptEnvvarSetInt(kvEnvHandle eHnd, int val);

/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptEnvvarGetInt(kvEnvHandle eHnd, out Int32 val);</b>
 *
 * \source_delphi   <b>function kvScriptEnvvarGetInt(const eHnd: kvEnvHandle; var val: Integer): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvScriptEnvvarGetInt() function retrieves the value of an \c int envvar.
 *
 * \param[in]  eHnd An open handle to an envvar.
 * \param[out] val  The current value.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_kvscript_envvar
 * \sa \ref kvScriptEnvvarOpen(), \ref kvScriptEnvvarSetInt(), \ref kvScriptEnvvarGetFloat(),
 * \ref kvScriptEnvvarGetData()
 *
 */
kvStatus CANLIBAPI kvScriptEnvvarGetInt(kvEnvHandle eHnd, int *val);

/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptEnvvarSetFloat(kvEnvHandle eHnd, Single val);</b>
 *
 * \source_delphi   <b>function kvScriptEnvvarSetFloat(const eHnd: kvEnvHandle; val: Single): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvScriptEnvvarSetFloat() sets the value of a \c float envvar.
 *
 * \note Not implemented in linux.
 *
 * \param[in] eHnd  An open handle to an envvar.
 * \param[in] val   The new value.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_kvscript_envvar
 * \sa \ref kvScriptEnvvarOpen(), \ref kvScriptEnvvarGetFloat(), \ref kvScriptEnvvarSetInt(),
 * \ref kvScriptEnvvarSetData()
 */
kvStatus CANLIBAPI kvScriptEnvvarSetFloat(kvEnvHandle eHnd, float val);

/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptEnvvarGetFloat(kvEnvHandle eHnd, out Single val);</b>
 *
 * \source_delphi   <b>function kvScriptEnvvarGetFloat(const eHnd: kvEnvHandle; var val: Single): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvScriptEnvvarGetFloat() function retrieves the value of a \c float envvar.
 *
 * \note Not implemented in linux.
 *
 * \param[in]  eHnd  An open handle to an envvar.
 * \param[out] val   A pointer to a \c float where the retrieved result should be
 *                   stored.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_kvscript_envvar
 * \sa \ref kvScriptEnvvarOpen(), \ref kvScriptEnvvarSetFloat(), \ref kvScriptEnvvarGetInt(),
 * \ref kvScriptEnvvarGetData()
 */
kvStatus CANLIBAPI kvScriptEnvvarGetFloat(kvEnvHandle eHnd, float *val);

/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptEnvvarSetData(kvEnvHandle eHnd, byte[] buf, Int32 start_index, Int32 data_len);</b>
 *
 * \source_delphi   <b>function kvScriptEnvvarSetData(const eHnd: kvEnvHandle; var buf; start_index: Integer; data_len: Integer): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvScriptEnvvarSetData() function sets a range of data bytes in an envvar.
 *
 * \note Not implemented in linux.
 *
 * \param[in] eHnd         An open handle to an envvar.
 * \param[in] buf          A pointer to a data area with the new values.
 * \param[in] start_index  The start index of the envvar's data range that we
 *                         want to update.
 * \param[in] data_len     The length in bytes of the envvar's data range that
 *                         we want to update.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_kvscript_envvar
 * \sa \ref kvScriptEnvvarOpen(), \ref kvScriptEnvvarGetData(), \ref kvScriptEnvvarSetInt(),
 * \ref kvScriptEnvvarSetFloat()
 */
kvStatus CANLIBAPI kvScriptEnvvarSetData(kvEnvHandle eHnd,
                                         const void *buf,
                                         int start_index,
                                         int data_len);

/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptEnvvarGetData(kvEnvHandle eHnd, out byte[] buf, Int32 start_index, Int32 data_len);</b>
 *
 * \source_delphi   <b>function kvScriptEnvvarGetData(const eHnd: kvEnvHandle; var buf; start_index: Integer; data_len: Integer): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvScriptEnvvarGetData() function retrieves a range of data bytes from an envvar.
 *
 * \note Not implemented in linux.
 *
 * \param[in]  eHnd         An open handle to an envvar.
 * \param[out] buf          A pointer to a data area where the retrieved data
 *                          range should be stored.
 * \param[in]  start_index  The start index of the data range.
 * \param[in]  data_len     The length in bytes of the data range.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_kvscript_envvar
 * \sa \ref kvScriptEnvvarOpen(), \ref kvScriptEnvvarSetData(), \ref kvScriptEnvvarGetInt(),
 * \ref kvScriptEnvvarGetFloat()
 */
kvStatus CANLIBAPI kvScriptEnvvarGetData(kvEnvHandle eHnd,
                                         void *buf,
                                         int start_index,
                                         int data_len);

/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptLoadFileOnDevice(CanHandle hnd, Int32 slotNo, String localFile);</b>
 *
 * \source_delphi   <b>function kvScriptLoadFileOnDevice(hnd: canHandle; slotNo: Integer; localFile: PAnsiChar): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvScriptLoadFileOnDevice() function loads a compiled script file (.txe)
 * stored on the device (SD card) into a script slot on the device.
 *
 * \param[in] hnd        An open handle to a CAN channel.
 * \param[in] slotNo     The slot where to load the script.
 * \param[in] localFile  The script file name; a pointer to a \c NULL terminated
 *                       array of chars.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_kvscript_loading
 * \sa \ref kvScriptLoadFile(), \ref kvFileCopyToDevice(), \ref kvScriptStart(),
 * \ref kvScriptStop()
 */
kvStatus CANLIBAPI kvScriptLoadFileOnDevice(const CanHandle hnd,
                                            int slotNo,
                                            char *localFile);

/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptLoadFile(CanHandle hnd, Int32 slotNo, String filePathOnPC);</b>
 *
 * \source_delphi   <b>function kvScriptLoadFile(hnd: canHandle; slotNo: Integer; filePathOnPC: PAnsiChar): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvScriptLoadFile() function loads a compiled script file (.txe) stored
 * on the host (PC) into a script slot on the device.
 *
 * \note The canHandle is used to determine what channel is set as the default
 * channel for the loaded script. If your canHandle was opened via a device's
 * second channel, the default channel number will be set to 1 (the numbering
 * of channel on the card starts from 0).
 *
 * \param[in] hnd           An open handle to a CAN channel.
 * \param[in] slotNo        The slot where to load the script.
 * \param[in] filePathOnPC  The script file name; a pointer to a \c NULL
 *                          terminated array of chars.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref section_user_guide_kvscript_loading
 * \sa \ref kvScriptLoadFileOnDevice(), \ref kvFileCopyToDevice(), \ref kvScriptStart(),
 * \ref kvScriptStop()
 */
kvStatus CANLIBAPI kvScriptLoadFile(const CanHandle hnd,
                                    int slotNo,
                                    char *filePathOnPC);


/**
 * \ingroup can_general
 * \name kvSCRIPT_REQUEST_TEXT_xxx
 * \anchor kvSCRIPT_REQUEST_TEXT_xxx
 *
 * These defines are used in \ref kvScriptRequestText() for printf message subscribe/unsubscribe.
 *
 * @{
 */

/**
 *  Cancel subscription of printf messages from script slots.
 */
#define kvSCRIPT_REQUEST_TEXT_UNSUBSCRIBE  1

/**
 *  Subscribe to printf messages from script slots.
 */
#define kvSCRIPT_REQUEST_TEXT_SUBSCRIBE    2

/**
 *  Select all script slots.
 */
#define kvSCRIPT_REQUEST_TEXT_ALL_SLOTS    255

/** @} */


/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptRequestText(CanHandle hnd, Int32 slot, Int32 request);</b>
 *
 * \source_delphi   <b>function kvScriptRequestText(hnd: canHandle; slotNo: cardinal; request: cardinal): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvScriptRequestText() Sets up a printf subscription to a
 * selected script slot.
 * Read the printf messages with \ref kvScriptGetText().
 *
 * \param[in] hnd          An open handle to a CAN channel.
 * \param[in] slot         The slot to subscribe to.
 * \param[in] request      Subscription request i.e. \ref kvSCRIPT_REQUEST_TEXT_xxx.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 */
kvStatus CANLIBAPI kvScriptRequestText(const CanHandle hnd,
                                       unsigned int slot,
                                       unsigned int request);



/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptGetText(CanHandle hnd, out Int32 slot, out Int64 time, out Int32 flags, out String buf);</b>
 *
 * \source_delphi   <b>function kvScriptGetText(hnd: canHandle; var slot: integer; var time: Cardinal; var flags: Cardinal; buf: PAnsiChar; bufsize: size_t): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvScriptGetText() Reads a printf from a subscribed script slot.
 * Set up a subscription with \ref kvScriptRequestText().
 *
 * \param[in] hnd           An open handle to a CAN channel.
 * \param[out] slot         The slot where the printf originated.
 * \param[out] time         The printf timestamp.
 * \param[out] flags        Printf flags. A combination of \ref canSTAT_xxx flags.
 * \param[out] buf          Buffer to hold the printf string.
 * \param[in] bufsize       Size of the buffer.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 */
kvStatus CANLIBAPI kvScriptGetText(const CanHandle hnd,
                                        int  *slot,
                                        unsigned long *time,
                                        unsigned int  *flags,
                                        char *buf,
                                        size_t bufsize);

/**
 * Script status flag bits. Used by \ref kvScriptStatus().
 * \anchor kvSCRIPT_STATUS_xxx
 * \name kvSCRIPT_STATUS_xxx
 * @{
 */
#define kvSCRIPT_STATUS_LOADED      1 /**< The slot is loaded with a script */
#define kvSCRIPT_STATUS_RUNNING     2 /**< The slot is running a script. */
/** @} */


/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptStatus(CanHandle hnd, Int32 slotNo, out Int32 status);</b>
 *
 * \source_delphi   <b>function kvScriptStatus(hnd: canHandle; slot: integer; var status: Cardinal): kvStatus;</b>
 * \source_end
 *
 * The \ref kvScriptStatus() function reads the current status of a script slot.
 *
 * \param[in] hnd           An open handle to a CAN channel.
 * \param[in] slot          The slot which status we want.
 * \param[out] status       The script status, as \ref kvSCRIPT_STATUS_xxx flag bits
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 */
kvStatus CANLIBAPI kvScriptStatus(const CanHandle hnd,
                                  int  slot,
                                  unsigned int *status);


/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptGetMaxEnvvarSize(CanHandle hnd, out Int32 size);</b>
 *
 * \source_delphi   <b>function kvScriptGetMaxEnvvarSize(hnd: canHandle; var envvarSize: Integer): kvStatus;</b>
 * \source_end
 *
 * The \ref kvScriptGetMaxEnvvarSize() function returns the maximum size of an envvar.
 *
 * \note Not implemented in linux.
 *
 * \param[in] hnd           An open handle to a CAN channel.
 * \param[out] envvarSize   The maximum size of an envvar.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 */
kvStatus CANLIBAPI kvScriptGetMaxEnvvarSize(int hnd, int *envvarSize);

/**
 * \ingroup tScript
 *
 * \source_cs       <b>static Canlib.kvStatus kvScriptTxeGetData(String filePathOnPC, Int32 item, out object buffer);</b>
 *
 * \source_delphi   <b>function kvScriptTxeGetData(filePathOnPC: PAnsiChar; item: Integer; var buffer; var bufsize: Cardinal): canStatus;</b>
 * \source_end
 *
 * This function can be used to retrieve information from a compiled script file (.txe).
 *
 * \param[in] filePathOnPC  The compiled script file name; a pointer to a \c NULL
 *                          terminated array of chars.
 *
 * \param[in] item          This parameter specifies what data to obtain. Valid values are one of the constants
 *                          \ref canTXEDATA_xxx.
 * \param[out] buffer       The address of a buffer which is to receive the data.
 * \param[in,out] bufsize   The size of the buffer to which the buffer parameter
 *                          points. When the function returns, bufsize contains the number of bytes copied into the buffer.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * If the buffer specified by \a buffer and \a bufsize is not large enough to hold the data requested,
 * the function returns \ref canERR_BUFFER_TOO_SMALL and stores the required buffer size in the integer pointed to by \a bufsize.
 * The content of buffer is undefined.
 *
 * If parameter \a buffer is NULL, and bufsize is non-NULL, the function returns \ref canOK and stores the required
 * buffer size in the integer pointed to by \a bufsize.
 *
 */
kvStatus CANLIBAPI kvScriptTxeGetData(const char *filePathOnPC,
                                      int item,
                                      void *buffer,
                                      unsigned int *bufsize);

/**
 * \ingroup tScript
 * \anchor canTXEDATA_xxx
 * \name canTXEDATA_xxx
 *
 * These defines are used in \ref kvScriptTxeGetData().
 *
 *  @{
 */

  /**
   * This define is used in \ref kvScriptTxeGetData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \source_cs <b>buffer contains a uint[3] array.</b>
   *
   * \a buffer points to an array of 3 32-bit unsigned integers which receives
   * the three part version number of the compiled script file (.txe) file format.
   *
   * Contents
   *
   * \li 0: major
   * \li 1: minor
   * \li 2: build
   */
#define canTXEDATA_FILE_VERSION               1

  /**
   * This define is used in \ref kvScriptTxeGetData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \source_cs <b>buffer contains a uint[3] array.</b>
   *
   * \a buffer points to an array of 3 32-bit unsigned integers which receives
   * the three part version number of the compiler used to create the compiled script file (.txe).
   *
   * Contents
   *
   * \li 0: major
   * \li 1: minor
   * \li 2: build
   */
#define canTXEDATA_COMPILER_VERSION      2

  /**
   * This define is used in \ref kvScriptTxeGetData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \source_cs <b>buffer contains a DateTime object.</b>
   *
   * \a buffer points to an array of 6 32-bit unsigned integers which receives
   * the compilation date in Coordinated Universal Time (UTC) of the compiled script file (.txe).
   *
   * Contents
   *
   * \li 0: year
   * \li 1: mon
   * \li 2: day
   * \li 3: hour
   * \li 4: min
   * \li 5: sec
   */
#define canTXEDATA_DATE                  3

  /**
   * This define is used in \ref kvScriptTxeGetData(), \a buffer
   * mentioned below refers to this functions argument.
   *
   * \source_cs <b>buffer contains a string.</b>
   *
   * \a buffer points to an area which receives a zero-terminated string with a
   * description of the compiled script file (.txe).
   * If no description is available then bufsize is set to zero and no data is written into buffer.
   */
#define canTXEDATA_DESCRIPTION           4

/**
 * This define is used in \ref kvScriptTxeGetData(), \a buffer
 * mentioned below refers to this functions argument.
 *
 * \source_cs <b>buffer contains a string.</b>
 *
 * \a buffer points to an area which receives a list containing the names and content of all source files which were
 * used to generate the compiled script file (.txe).
 *
 * The name followed by the content of each source file is written into \a buffer as consecutive zero-terminated strings.
 *
 * If no source code is available or the container is encrypted, then bufsize is set to zero and no data is written into buffer.
 */
#define canTXEDATA_SOURCE               5

/**
 * This define is used in \ref kvScriptTxeGetData(), \a buffer
 * mentioned below refers to this functions argument.
 *
 * \source_cs <b>buffer contains a uint.</b>
 *
 * \a buffer points to a single unsigned integer which receives the size of the compiled code of the
 * compiled script file (.txe).
 */
#define canTXEDATA_SIZE_OF_CODE          6

/**
 * This define is used in \ref kvScriptTxeGetData(), \a buffer
 * mentioned below refers to this functions argument.
 *
 * \source_cs <b>buffer contains a bool.</b>
 *
 * \a buffer points to a single unsigned integer which will receive a non-zero value if the compiled script file (.txe) contents
 * is encrypted.
 */
#define canTXEDATA_IS_ENCRYPTED           7


/** @} */


/**
 * \ingroup grp_kvfile
 *
 * \source_cs       <b>static Canlib.kvStatus kvFileCopyToDevice(CanHandle hnd, String hostFileName, String deviceFileName);</b>
 *
 * \source_delphi   <b>function kvFileCopyToDevice(hnd: canHandle; hostFileName: PAnsiChar; deviceFileName: PAnsiChar): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvFileCopyToDevice() function copies an arbitrary file from the host to
 * the device.
 *
 * \param[in] hnd             An open handle to a CAN channel.
 * \param[in] hostFileName    The host file name; a pointer to a \c NULL terminated
 *                            array of chars.
 * \param[in] deviceFileName  The target device file name; a pointer to a \c NULL
 *                            terminated array of chars.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref kvFileCopyFromDevice(), \ref kvFileDelete()
 * \sa \ref section_user_guide_kvfile_copying_files_to_device
 */
kvStatus CANLIBAPI kvFileCopyToDevice(const CanHandle hnd,
                                      char *hostFileName,
                                      char *deviceFileName);

/**
 * \ingroup grp_kvfile
 *
 * \source_cs       <b>static Canlib.kvStatus kvFileCopyFromDevice(CanHandle hnd, String deviceFileName, String hostFileName);</b>
 *
 * \source_delphi   <b>function kvFileCopyFromDevice(hnd: canHandle; deviceFileName: PAnsiChar; hostFileName: PAnsiChar): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvFileCopyFromDevice() function copies an arbitrary file from the device
 * to the host.
 *
 * \param[in] hnd             An open handle to a CAN channel.
 * \param[in] deviceFileName  The device file name; a pointer to a \c NULL
 *                            terminated array of chars.
 * \param[in] hostFileName    The target host file name; a pointer to a \c NULL terminated
 *                            array of chars.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref kvFileCopyToDevice()
 */
kvStatus CANLIBAPI kvFileCopyFromDevice(const CanHandle hnd,
                                        char *deviceFileName,
                                        char *hostFileName);

/**
 * \ingroup grp_kvfile
 *
 * \source_cs       <b>static Canlib.kvStatus kvFileDelete(CanHandle hnd, String deviceFileName);</b>
 *
 * \source_delphi   <b>function kvFileDelete(hnd: canHandle; deviceFileName: PAnsiChar): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvFileDelete() function deletes a file on the device.
 *
 * \note Deleting system files is not recommended.
 *
 * \param[in] hnd             An open handle to a CAN channel.
 * \param[in] deviceFileName  The file on the device to delete; a pointer
 *                            to a \c NULL terminated array of chars.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref kvFileCopyToDevice()
 */
kvStatus CANLIBAPI kvFileDelete(const CanHandle hnd, char *deviceFileName);

/**
 * \ingroup grp_kvfile
 *
 * \source_cs       <b>static Canlib.kvStatus kvFileGetName(CanHandle hnd, Int32 fileNo, out String name);</b>
 *
 * \source_delphi   <b>function kvFileGetName(hnd: canHandle; fileNo: Integer; name: PAnsiChar; namelen: Integer): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvFileGetName() function returns the name of the file with
 *   number \a fileNo.
 *
 * \param[in]  hnd      An open handle to a CAN channel.
 * \param[in]  fileNo   The number of the file.
 * \param[out] name     A buffer that will contain the name of the
 *                      file. The name is a zero-terminated ASCII string.
 * \param[in]  namelen  The length, in bytes, of the \a name buffer.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref kvFileGetCount(), \ref kvFileGetSystemData()
 */
kvStatus CANLIBAPI kvFileGetName(const CanHandle hnd,
                                 int fileNo,
                                 char *name,
                                 int namelen);

/**
 * \ingroup grp_kvfile
 *
 * \source_cs       <b>static Canlib.kvStatus kvFileGetCount(CanHandle hnd, out Int32 count);</b>
 *
 * \source_delphi   <b>function kvFileGetCount(hnd: canHandle; var count: Integer): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvFileGetCount() function returns the number of files.
 *
 * \param[in]  hnd    An open handle to a CAN channel.
 * \param[out] count  A pointer to a 32-bit integer that will receive
 *                    the file count.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref kvFileGetName(), \ref kvFileGetSystemData()
 */
kvStatus CANLIBAPI kvFileGetCount(const CanHandle hnd, int *count);

/**
 * \ingroup grp_kvfile
 *
 * \source_cs       <b>static Canlib.kvStatus kvFileGetSystemData(CanHandle hnd, Int32 itemCode, out Int32 result);</b>
 *
 * \source_delphi   <b>function kvFileGetSystemData(hnd: canHandle; itemCode: Integer; var result: Integer): kvStatus; </b>
 * \source_end
 *
 * The \ref kvFileGetSystemData() function is used for reading disk parameters,
 * e.g. size, max number of (user) files, etc.
 *
 * \note Not yet implemented
 *
 * \param[in]  hnd       An open handle to a CAN channel.
 * \param[in]  itemCode  The item we want information on.
 * \param[out] result    A pointer to a 32-bit integer that will
 *                       receive the result.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 */
kvStatus CANLIBAPI kvFileGetSystemData(const CanHandle hnd,
                                       int itemCode,
                                       int *result);

/**
 * \ingroup grp_kvfile
 *
 * \source_cs       <b>static Canlib.canStatus  kvFileDiskFormat(CanHandle hnd);</b>
 *
 * \source_delphi   <b>function kvFileDiskFormat(hnd: canHandle): kvStatus; </b>
 * \source_end
 *
 * The \ref kvFileDiskFormat() function is used for formating the disk,
 * back to FAT32.
 *
 *
 * \param[in] hnd       An open handle to a CAN channel.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 */
kvStatus CANLIBAPI kvFileDiskFormat(const CanHandle hnd);

/**
 * \ingroup can_general
 * \anchor kvDEVICE_MODE_xxx
 * \name kvDEVICE_MODE_xxx
 *
 * These defines are used in \ref kvDeviceSetMode() and \ref kvDeviceGetMode().
 *
 * \note The mode is device specific, which means that not all modes are
 * implemented in all products.
 *
 *  @{
 */
   /**
   * This define is used in \ref kvDeviceSetMode() and \ref kvDeviceGetMode().
   *
   * Device is running or should be running in interface mode.
   */
#define kvDEVICE_MODE_INTERFACE      0x00
  /**
   * This define is used in \ref kvDeviceSetMode() and \ref kvDeviceGetMode().
   *
   * Device is running or should be running in logger mode.
   */
#define kvDEVICE_MODE_LOGGER      0x01
 /** @} */


/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.kvStatus kvDeviceSetMode(CanHandle hnd, Int32 mode);</b>
 *
 * \source_delphi   <b>function kvDeviceSetMode(hnd: canHandle; mode: Integer): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvDeviceSetMode() sets the mode.
 *
 * \note The mode is device specific, which means that not all modes are
 * implemented in all products.
 *
 * \param[in] hnd   An open handle to a CAN channel.
 * \param[in] mode  One of the \ref kvDEVICE_MODE_xxx constants,
 *                  defining which mode to use.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref kvDeviceGetMode()
 */
kvStatus CANLIBAPI kvDeviceSetMode(const CanHandle hnd, int mode);

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.kvStatus kvDeviceGetMode(CanHandle hnd, out Int32 result);</b>
 *
 * \source_delphi   <b>function kvDeviceGetMode(hnd: canHandle; var mode: Integer): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvDeviceGetMode() reads the current device's specific mode.
 *
 * \note The mode is device specific, which means that not all modes are
 * implemented in all products.
 *
 * \param[in]  hnd     An open handle to a CAN channel.
 * \param[out] result  A pointer to a 32-bit integer that will receive the
 *                     \ref kvDEVICE_MODE_xxx value.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref kvDeviceSetMode()
 */
kvStatus CANLIBAPI kvDeviceGetMode(const CanHandle hnd, int *result);


/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.kvStatus kvPingRequest(CanHandle hnd, out Int32 requestTime);</b>
 *
 * \source_delphi   <b>function kvPingRequest(hnd: canHandle; var requestTime: Cardinal): kvStatus;     </b>
 * \source_end
 *
 * This function sends an active ping to a device. The ping time can later be
 * retrieved using \ref kvPingGetLatest().
 *
 * \param[in]  hnd          A handle to an open circuit.
 * \param[out] requestTime  Time of request in microseconds. Used for matching answer to request.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 */
kvStatus CANLIBAPI kvPingRequest(const CanHandle hnd,
                                 unsigned int *requestTime);

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.kvStatus kvPingGetLatest(CanHandle hnd, out Int32 requestTime, out Int32 pingTime);</b>
 *
 * \source_delphi   <b>function kvPingGetLatest(hnd: canHandle; var requestTime: Cardinal; var pingTime: Cardinal): kvStatus;     </b>
 * \source_end
 *
 * This retrieves the latest ping time issued by an earlier call to \ref kvPingRequest()
 *
 * \param[in]  hnd         A handle to an open circuit.
 * \param[out] requestTime Time of request in microseconds. Used for matching answer to request.
 * \param[out] pingTime    Latest value of ping time in milliseconds.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 */
kvStatus CANLIBAPI kvPingGetLatest(const CanHandle hnd,
                                   unsigned int *requestTime,
                                   unsigned int *pingTime);

/// @cond UNIMPLEMENTED
/**
 * \ingroup Internal
 * \anchor Internal
 * \name Unimplemented internal
 *
 * The following are not yet implemented. Do not use it.
 *
 *  @{
 */
//
//
//
#if defined(_CANEVT_H_)
canStatus CANLIBAPI canReadEvent(const CanHandle hnd, CanEvent *event);
#endif /* _CANEVT_H_ */

/** This function is not implemented, do not use it. */
void CANLIBAPI canSetDebug(int d);
/** This function is not implemented, do not use it. */
canStatus CANLIBAPI canSetNotifyEx(const CanHandle hnd,
                                   HANDLE event,
                                   unsigned int flags);
/** This function is not implemented, do not use it. */
canStatus CANLIBAPI canSetTimer(const CanHandle hnd,
                                DWORD interval,
                                DWORD flags);
/** This is reserved for \ref canSetTimer() but not implemented, do not use it. */
#define canTIMER_CYCLIC             0x01
/** This is reserved for \ref canSetTimer() but not implemented, do not use it. */
#define canTIMER_EXPENSIVE          0x02
/** This function is not implemented, do not use it. */
int CANLIBAPI canSplitHandle(CanHandle hnd, int channel);
/** This function is not implemented, do not use it. */
int CANLIBAPI canOpenMultiple(DWORD bitmask, int flags);
/** @} */
/// @endcond

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.kvStatus kvReadTimer(CanHandle hnd, out Int32 time);</b>
 *
 * \source_delphi   <b>function kvReadTimer(handle: canHandle; var time: Cardinal): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvReadTimer reads the hardware clock on the specified device and returns
 * the value.
 *
 * When the call to \ref kvReadTimer() returns, the time value is already
 * obsolete. The time required for the device firmware, any intermediary buses
 * (like USB,) and the operating system to return the time value is not
 * defined.
 *
 * This call should be used instead of \ref canReadTimer() because it can return an
 * error code if it fails.
 *
 * \param[in]  hnd   An open handle to a CAN channel.
 * \param[out] time  A pointer to a 32-bit unsigned integer that will receive
 *                   the time value.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref canReadTimer(), \ref kvReadTimer64()
 */
kvStatus CANLIBAPI kvReadTimer(const CanHandle hnd, unsigned int *time);

/**
 * \ingroup can_general
 *
 * \source_cs       <b>static Canlib.kvStatus kvReadTimer64(CanHandle hnd, out Int64 time);</b>
 *
 * \source_delphi   <b>function kvReadTimer64(handle: canHandle; var time: Int64): kvStatus;     </b>
 * \source_end
 *
 * The \ref kvReadTimer64 reads the hardware clock on the specified device and
 * returns the value.
 *
 * When the call to \ref kvReadTimer64() returns, the time value is already
 * obsolete. The time required for the device firmware, any intermediary buses
 * (like USB,) and the operating system to return the time value is not
 * defined.
 *
 * This call should be used instead of \ref canReadTimer() because it can return an
 * error code if it fails.
 *
 * \param[in]  hnd   An open handle to a CAN channel.
 * \param[out] time  A pointer to a 64-bit signed integer that will receive the
 *                   time value.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref kvReadTimer(), \ref canReadTimer()
 */
kvStatus CANLIBAPI kvReadTimer64(const CanHandle hnd, KVINT64 *time);

/**
 * \ingroup kv_io
 * \anchor kvIO_INFO_GET_xxx
 * \name kvIO_INFO_GET_xxx
 *
 * These defines are used in \ref kvIoPinGetInfo().
 * The value range for each property is specified in the manufacturer's user manual.
 *
 *  @{
 */
  /**
   * This define is used in \ref kvIoPinGetInfo().
   *
   * An unsigned 32-bit integer, see \ref kvIO_MODULE_TYPE_xxx.
   * Read-only.
   */
#define kvIO_INFO_GET_MODULE_TYPE                1
  /**
   * This define is used in \ref kvIoPinGetInfo().
   *
   * An unsigned 32-bit integer, see \ref kvIO_PIN_DIRECTION_xxx.
   * Read-only.
   */
#define kvIO_INFO_GET_DIRECTION                     2
  /**
   * This define is used in \ref kvIoPinGetInfo().
   *
   * An unsigned 32-bit integer, see \ref kvIO_PIN_TYPE_xxx.
   * Read-only.
   */
#define kvIO_INFO_GET_PIN_TYPE                      4
  /**
   * This define is used in \ref kvIoPinGetInfo().
   *
   * An unsigned 32-bit integer that contains the resolution
   * in number of bits. Read-only.
   */
#define kvIO_INFO_GET_NUMBER_OF_BITS                5
  /**
   * This define is used in \ref kvIoPinGetInfo().
   *
   * A float that contains the lower range limit
   * in volts. Read-only.
   *
   * \note This is not applicable for relay pins.
   */
#define kvIO_INFO_GET_RANGE_MIN                     6
  /**
   * This define is used in \ref kvIoPinGetInfo().
   *
   * A float that contains the upper range limit
   * in volts. Read-only.
   *
   * \note This is not applicable for relay pins.
   */
#define kvIO_INFO_GET_RANGE_MAX                     7
  /**
   * This define is used in \ref kvIoPinGetInfo().
   *
   * An unsigned 32-bit integer that contains the filter time in micro
   * seconds when a digital input pin goes from LOW to HIGH.
   *
   * \note This is only used for digital input pins.
   */
#define kvIO_INFO_GET_DI_LOW_HIGH_FILTER            8
  /**
   * This define is used in \ref kvIoPinGetInfo().
   *
   * An unsigned 32-bit integer that contains the filter time in micro
   * seconds when a digital input pin goes from HIGH to LOW.
   *
   * \note This is only used for digital input pins.
   */
#define kvIO_INFO_GET_DI_HIGH_LOW_FILTER            9
  /**
   * This define is used in \ref kvIoPinGetInfo().
   *
   * An unsigned 32-bit integer that contains the low-pass filter
   * order for an analog input pin.
   *
   * \note This is only used for analog input pins.
   */
#define kvIO_INFO_GET_AI_LP_FILTER_ORDER           10
  /**
   * This define is used in \ref kvIoPinGetInfo().
   *
   * A float that contains the hysteresis in volt for an
   * analog input pin, i.e. the amount the input have to change
   * before the sampled value is updated.
   *
   * \note This is only used for analog input pins.
   */
#define kvIO_INFO_GET_AI_HYSTERESIS                11
  /**
   * This define is used in \ref kvIoPinGetInfo() to get the module number.
   *
   * An unsigned 32-bit integer that contains the module number
   * the pin belongs to. The number starts from 0. Read-only.
   */
#define kvIO_INFO_GET_MODULE_NUMBER                14
  /**
   * This define is used in \ref kvIoPinGetInfo() to get the serial number.
   *
   * An unsigned 32-bit integer that contains the serial number
   * of the submodule the pin belongs to. Read-only.
   */
#define kvIO_INFO_GET_SERIAL_NUMBER                15
  /**
   * This define is used in \ref kvIoPinGetInfo() to get the FW version number.
   *
   * An unsigned 32-bit integer that contains the software version number
   * of the submodule the pin belongs to. This number consists of two 16-bit words,
   * where the most significant word represents the major and the least significant
   * the minor software version number. Read-only.
   */
#define kvIO_INFO_GET_FW_VERSION                   16
 /** @} */


/**
 * \ingroup kv_io
 * \anchor kvIO_INFO_SET_xxx
 * \name kvIO_INFO_SET_xxx
 *
 * These defines are used in \ref kvIoPinSetInfo().
 * The value range for each property is specified in the manufacturer's user manual.
 *
 *  @{
 */
  /**
   * This define is used in \ref kvIoPinSetInfo().
   *
   * An unsigned 32-bit integer that contains the filter time in micro
   * seconds when a digital input pin goes from LOW to HIGH.
   *
   * \note This is only used for digital input pins.
   */
#define kvIO_INFO_SET_DI_LOW_HIGH_FILTER            8
  /**
   * This define is used in \ref kvIoPinSetInfo().
   *
   * An unsigned 32-bit integer that contains the filter time in micro
   * seconds when a digital input pin goes from HIGH to LOW.
   *
   * \note This is only used for digital input pins.
   */
#define kvIO_INFO_SET_DI_HIGH_LOW_FILTER            9
  /**
   * This define is used in \ref kvIoPinSetInfo().
   *
   * An unsigned 32-bit integer that contains the low-pass filter
   * order for an analog input pin.
   *
   * \note This is only used for analog input pins.
   */
#define kvIO_INFO_SET_AI_LP_FILTER_ORDER           10
  /**
   * This define is used in \ref kvIoPinSetInfo().
   *
   * A float that contains the hysteresis in volt for an
   * analog input pin, i.e. the amount the input have to change
   * before the sampled value is updated.
   *
   * \note This is only used for analog input pins.
   */
#define kvIO_INFO_SET_AI_HYSTERESIS                11
 /** @} */




/**
 * \ingroup kv_io
 * \anchor kvIO_MODULE_TYPE_xxx
 * \name kvIO_MODULE_TYPE_xxx
 *
 * These defines are used in \ref kvIoPinGetInfo().
 *
 *  @{
 */
   /**
   * Kvaser Add-on module with digital inputs and digital outputs
   */
#define kvIO_MODULE_TYPE_DIGITAL        1
  /**
   * Kvaser Add-on module with analog inputs and analog outputs
   *
   */
#define kvIO_MODULE_TYPE_ANALOG         2
  /**
   * Kvaser Add-on module with relays and digital inputs
   *
   */
#define kvIO_MODULE_TYPE_RELAY          3
  /**
   * Kvaser built-in module with digital inputs and digital outputs
   *
   */
#define kvIO_MODULE_TYPE_INTERNAL       4
 /** @} */


/**
 * \ingroup kv_io
 * \anchor kvIO_PIN_TYPE_xxx
 * \name kvIO_PIN_TYPE_xxx
 *
 * These defines are used in \ref kvIoPinGetInfo().
 *
 *  @{
 */
   /**
   * Digital
   */
#define kvIO_PIN_TYPE_DIGITAL           1
  /**
   * Analog
   *
   */
#define kvIO_PIN_TYPE_ANALOG            2
  /**
   * Relay
   *
   */
#define kvIO_PIN_TYPE_RELAY             3
 /** @} */


/**
 * \ingroup kv_io
 * \anchor kvIO_PIN_DIRECTION_xxx
 * \name kvIO_PIN_DIRECTION_xxx
 *
 * These defines are used in \ref kvIoPinGetInfo().
 *
 *  @{
 */
  /**
   * Input
   */
#define kvIO_PIN_DIRECTION_IN           4
  /**
   * Output
   *
   */
#define kvIO_PIN_DIRECTION_OUT          8
 /** @} */

/**
 * \ingroup kv_io
 *
 * \source_cs       <b>static Canlib.kvStatus kvIoGetNumberOfPins(CanHandle handle, out Int32 pinCount);</b>
 *
 * \source_delphi   <b>function kvIoGetNumberOfPins(hnd: canHandle; var pinCount: cardinal): canStatus;</b>
 * \source_end
 *
 * Get the number of I/O pins available from a device.
 *
 * \param[in]  hnd   An open handle to a CAN channel.
 * \param[out] pinCount  A pointer to an int which receives the number of pins.
 *
 * \return \ref canOK (zero) if success
 * \return \ref canERR_xxx (negative) if failure
 *
 * \sa \ref kvIoPinGetInfo(), \ref kv_io
 * \note Preliminary API that may change.
 * \note Not implemented in Linux.
 */
canStatus CANLIBAPI kvIoGetNumberOfPins(const CanHandle hnd, unsigned int *pinCount);

/**
 * \ingroup kv_io
 *
 * \source_cs       <b>static Canlib.kvStatus kvIoConfirmConfig(CanHandle handle);</b>
 *
 * \source_delphi   <b>function kvIoConfirmConfig(hnd: canHandle): canStatus;</b>
 * \source_end
 *
 * This function is used to confirm configuration. It is required to call this function, before it is possible to use any kvIoPinSetXxx()/kvIoPinGetXxx() function. After a configuration change, module removal or insertion, it is required to confirm the new configuration.
 *
 * \param[in]  hnd   An open handle to a CAN channel.
 *
 * \sa \ref kv_io
 * \note Preliminary API that may change.
 * \note Not implemented in Linux.
 */
canStatus CANLIBAPI kvIoConfirmConfig(const CanHandle hnd);

/**
 * \ingroup kv_io
 *
 * \source_cs       <b>static Canlib.kvStatus kvIoPinGetInfo(CanHandle handle, Int32 pin, kvIOGetInfo item, ref Object data);</b>
 *
 * \source_delphi   <b>function kvIoPinGetInfo(hnd: canHandle; pin: cardinal; item: Integer; var buffer; bufsize: cardinal): canStatus;</b>
 * \source_end
 * This function is used to retrieve I/O pin properties.
 *
 * \param[in]  hnd        An open handle to a CAN channel.
 * \param[in]  pin        The pin number, see \ref kvIoGetNumberOfPins.
 * \param[in]  item       Type of item to retrieve \ref kvIO_INFO_GET_xxx.
 * \param[out] buffer     The address of a buffer which is to receive the data.
 * \param[in]  bufsize    The size of the buffer, matching the size of selected \ref kvIO_INFO_GET_xxx item.
 *
 * \sa \ref kvIoPinSetInfo(), \ref kv_io
 * \note Preliminary API that may change.
 * \note Not implemented in Linux.
 */
canStatus CANLIBAPI kvIoPinGetInfo(const CanHandle hnd, unsigned int pin, int item, void *buffer, const unsigned int bufsize);

/**
 * \ingroup kv_io
 *
 * \source_cs       <b>static Canlib.kvStatus kvIoPinSetInfo(CanHandle handle, Int32 pin, kvIOSetInfo item, Object data);</b>
 *
 * \source_delphi   <b>function kvIoPinSetInfo(hnd: canHandle; pin: cardinal; item: Integer; var buffer; bufsize: cardinal): canStatus;</b>
 * \source_end
 * This function is used to set I/O pin properties, for items that can be changed.
 *
 * \param[in]  hnd        An open handle to a CAN channel.
 * \param[in]  pin        The pin number, see \ref kvIoGetNumberOfPins.
 * \param[in]  item       Type of item to set, see \ref kvIO_INFO_SET_xxx.
 * \param[in]  buffer     The address of a buffer contains the data to set.
 * \param[in]  bufsize    The size of the buffer, matching the size of selected \ref kvIO_INFO_SET_xxx item.
 *
 * \sa \ref kvIoPinGetInfo()
 * \note Preliminary API that may change.
 * \note Not implemented in Linux.
 */
canStatus CANLIBAPI kvIoPinSetInfo(const CanHandle hnd, unsigned int pin, int item, const void *buffer, const unsigned int bufsize);

/**
 * \ingroup kv_io
 *
 * \source_cs       <b>static Canlib.kvStatus kvIoPinSetDigital(CanHandle handle, Int32 pin, Int32 value);</b>
 *
 * \source_delphi   <b>function kvIoPinSetDigital(hnd: canHandle; pin: cardinal; value: cardinal): canStatus;</b>
 * \source_end
 * This function is used to set a digital output I/O pin. If \a value is zero,
 * the pin is set LOW. For any non-zero \a value, the pin is set HIGH.
 *
 * \param[in]  hnd   An open handle to a CAN channel.
 * \param[in]  pin   The pin number, see \ref kvIoGetNumberOfPins.
 * \param[in]  value An unsigned int which sets a value of the pin.
 *
 * \note Preliminary API that may change.
 * \note Not implemented in Linux.
 */
canStatus CANLIBAPI kvIoPinSetDigital(const CanHandle hnd, unsigned int pin, unsigned int value);

/**
 * \ingroup kv_io
 *
 * \source_cs       <b>static Canlib.kvStatus kvIoPinGetDigital(CanHandle handle, Int32 pin, out Int32 value);</b>
 *
 * \source_delphi   <b>function kvIoPinGetDigital(hnd: canHandle; pin: cardinal; var value: cardinal): canStatus;</b>
 * \source_end
 * This function is used to retrieve the value of the specified digital input I/O pin.
 * If the pin is LOW, the integer pointed to by \a value is assigned zero.
 * If the pin is HIGH, the integer pointed to by \a value is assigned a '1'.
 *
 * \param[in]  hnd   An open handle to a CAN channel.
 * \param[in]  pin   The pin number, see \ref kvIoGetNumberOfPins.
 * \param[out] value A pointer to an unsigned int which receives the value of the pin.
 *
 * \sa \ref kvIoPinSetDigital()
 * \note Preliminary API that may change.
 * \note Not implemented in Linux.
 */
canStatus CANLIBAPI kvIoPinGetDigital(const CanHandle hnd, unsigned int pin, unsigned int *value);

/**
 * \ingroup kv_io
 *
 * \source_cs       <b>static Canlib.kvStatus kvIoPinGetOutputDigital(CanHandle handle, Int32 pin, out Int32 value);</b>
 *
 * \source_delphi   <b>function kvIoPinGetOutputDigital(hnd: canHandle; pin: cardinal; var value: cardinal): canStatus;</b>
 * \source_end
 * This function is used to get the latest set value of a digital output I/O pin.
 * If the latest value written to the pin is LOW, the integer pointed to by \a value is assigned zero.
 * If it is HIGH, the integer pointed to by \a value is assigned a '1'.
 * This function only returns values as they are presented in memory and the actual value on the output pin may therefore differ.
 *
 * \param[in]  hnd   An open handle to a CAN channel.
 * \param[in]  pin   The pin number, see \ref kvIoGetNumberOfPins.
 * \param[out] value A pointer to an unsigned int which receives the latest set value of the pin.
 *
 * \sa \ref kvIoPinSetDigital()
 * \note The actual value on the output pin may differ.
 * \note Preliminary API that may change.
 * \note Not implemented in Linux.
 */
canStatus CANLIBAPI kvIoPinGetOutputDigital(const CanHandle hnd, unsigned int pin, unsigned int *value);

/**
 * \ingroup kv_io
 *
 * \source_cs       <b>static Canlib.kvStatus kvIoPinSetRelay(CanHandle handle, Int32 pin, Int32 value);</b>
 *
 * \source_delphi   <b>function kvIoPinSetRelay(hnd: canHandle; pin: cardinal; value: cardinal): canStatus;</b>
 * \source_end
 * This function is used to control a relay of the specified I/O pin.
 * If \a value is zero, the relay is set to OFF. For any non-zero \a value,
 * the relay is set to ON.
 *
 * \param[in]  hnd   An open handle to a CAN channel.
 * \param[in]  pin   The pin number, see \ref kvIoGetNumberOfPins.
 * \param[in]  value An unsigned int which sets a value of the pin.
 * \note Preliminary API that may change.
 * \note Not implemented in Linux.
 */
canStatus CANLIBAPI kvIoPinSetRelay(const CanHandle hnd, unsigned int pin, unsigned int value);

/**
 * \ingroup kv_io
 *
 * \source_cs       <b>static Canlib.kvStatus kvIoPinGetOutputRelay(CanHandle handle, Int32 pin, out Int32 value);</b>
 *
 * \source_delphi   <b>function kvIoPinGetOutputRelay(hnd: canHandle; pin: cardinal; var value: cardinal): canStatus;</b>
 * \source_end
 * This function is used to get the latest set value of a relay I/O pin.
 * If \a value is zero, the relay has been set to OFF. For any non-zero \a value,
 * the relay has been set to ON.
 * This function returns values as they are presented in memory and the actual state on the relay pin may differ.
 *
 * \param[in]  hnd   An open handle to a CAN channel.
 * \param[in]  pin   The pin number, see \ref kvIoGetNumberOfPins.
 * \param[in]  value A pointer to an unsigned int which receives the latest set value of the pin.
 *
 * \note Preliminary API that may change.
 * \note Not implemented in Linux.
 */
canStatus CANLIBAPI kvIoPinGetOutputRelay(const CanHandle hnd, unsigned int pin, unsigned int *value);

/**
 * \ingroup kv_io
 *
 * \source_cs       <b>static Canlib.kvStatus kvIoPinSetAnalog(CanHandle handle, Int32 pin, float value);</b>
 *
 * \source_delphi   <b>function kvIoPinSetAnalog(hnd: canHandle; pin: cardinal; value: single): canStatus;</b>
 * \source_end
 * This function is used to set the voltage level of the specified analog I/O pin.
 *
 * \param[in]  hnd   An open handle to a CAN channel.
 * \param[in]  pin   The pin number, see \ref kvIoGetNumberOfPins.
 * \param[in]  value A float which sets a voltage of the pin.
 *
 * \sa \ref kvIoPinGetAnalog()
 * \note Preliminary API that may change.
 * \note Not implemented in Linux.
 */
canStatus CANLIBAPI kvIoPinSetAnalog(const CanHandle hnd, unsigned int pin, float value);

/**
 * \ingroup kv_io
 *
 * \source_cs       <b>static Canlib.kvStatus kvIoPinGetAnalog(CanHandle handle, Int32 pin, out float value);</b>
 *
 * \source_delphi   <b>function kvIoPinGetAnalog(hnd: canHandle; pin: cardinal; var value: single): canStatus;</b>
 * \source_end
 * This function is used to retrieve the voltage level of the specified analog I/O pin.
 *
 * \param[in]  hnd   An open handle to a CAN channel.
 * \param[in]  pin   The pin number, see \ref kvIoGetNumberOfPins.
 * \param[out] value A pointer to a float which receives the voltage of the pin.
 *
 * \sa \ref kvIoPinSetAnalog()
 * \note Preliminary API that may change.
 * \note Not implemented in Linux.
 */
canStatus CANLIBAPI kvIoPinGetAnalog(const CanHandle hnd, unsigned int pin, float *value);

/**
 * \ingroup kv_io
 *
 * \source_cs       <b>static Canlib.kvStatus kvIoPinGetOutputAnalog(CanHandle handle, Int32 pin, out float value);</b>
 *
 * \source_delphi   <b>function kvIoPinGetOutputAnalog(hnd: canHandle; pin: cardinal; var value: single): canStatus;</b>
 * \source_end
 * This function is used to get the latest set voltage level of an analog I/O pin.
 * This function only returns values as they are presented in memory and the actual value on the output pin may therefore differ.
 *
 * \param[in]  hnd   An open handle to a CAN channel.
 * \param[in]  pin   The pin number, see \ref kvIoGetNumberOfPins.
 * \param[out] value A pointer to a float which receives the latest set voltage level of the pin.
 *
 * \sa \ref kvIoPinSetAnalog()
 * \note The actual voltage level on the output pin may differ.
 * \note Preliminary API that may change.
 * \note Not implemented in Linux.
 */
canStatus CANLIBAPI kvIoPinGetOutputAnalog(const CanHandle hnd, unsigned int pin, float *value);

/**
 * This define is used in \ref kvIoGetModulePins() and \ref kvIoSetModulePins.
 *
 * This struct represents a digital add-on module.
 */
typedef struct {
    int type; /**< The type of the module. See: \ref kvIO_MODULE_TYPE_xxx. */
    unsigned char DO1 : 1; /**< Digital Out */
    unsigned char DO2 : 1; /**< Digital Out */
    unsigned char DO3 : 1; /**< Digital Out */
    unsigned char DO4 : 1; /**< Digital Out */
    unsigned char DO5 : 1; /**< Digital Out */
    unsigned char DO6 : 1; /**< Digital Out */
    unsigned char DO7 : 1; /**< Digital Out */
    unsigned char DO8 : 1; /**< Digital Out */
    unsigned char DO9 : 1; /**< Digital Out */
    unsigned char DO10: 1; /**< Digital Out */
    unsigned char DO11: 1; /**< Digital Out */
    unsigned char DO12: 1; /**< Digital Out */
    unsigned char DO13: 1; /**< Digital Out */
    unsigned char DO14: 1; /**< Digital Out */
    unsigned char DO15: 1; /**< Digital Out */
    unsigned char DO16: 1; /**< Digital Out */
    unsigned char DI1 : 1; /**< Digital In */
    unsigned char DI2 : 1; /**< Digital In */
    unsigned char DI3 : 1; /**< Digital In */
    unsigned char DI4 : 1; /**< Digital In */
    unsigned char DI5 : 1; /**< Digital In */
    unsigned char DI6 : 1; /**< Digital In */
    unsigned char DI7 : 1; /**< Digital In */
    unsigned char DI8 : 1; /**< Digital In */
    unsigned char DI9 : 1; /**< Digital In */
    unsigned char DI10: 1; /**< Digital In */
    unsigned char DI11: 1; /**< Digital In */
    unsigned char DI12: 1; /**< Digital In */
    unsigned char DI13: 1; /**< Digital In */
    unsigned char DI14: 1; /**< Digital In */
    unsigned char DI15: 1; /**< Digital In */
    unsigned char DI16: 1; /**< Digital In */
} kvIoModuleDigital;


/**
* /struct
* This define is used in \ref kvIoGetModulePins() and \ref kvIoSetModulePins().
* This struct represents a digital internal module
*/
typedef struct {
    int type; /**< The type of the module. See: \ref kvIO_MODULE_TYPE_xxx.. */
    unsigned char DO: 1; /**< Digital Out */
    unsigned char DI: 1; /**< Digital In */
} kvIoModuleInternal;


/**
 * /struct
 * This define is used in \ref kvIoGetModulePins() and \ref kvIoSetModulePins().
 * This struct represents a relay add-on module.
 */

typedef struct {
    int type; /**< The type of the module. See: \ref kvIO_MODULE_TYPE_xxx.. */
    unsigned char RO1 : 1; /**< Relay Out */
    unsigned char RO2 : 1; /**< Relay Out */
    unsigned char RO3 : 1; /**< Relay Out */
    unsigned char RO4 : 1; /**< Relay Out */
    unsigned char RO5 : 1; /**< Relay Out */
    unsigned char RO6 : 1; /**< Relay Out */
    unsigned char RO7 : 1; /**< Relay Out */
    unsigned char RO8 : 1; /**< Relay Out */
    unsigned char DI1 : 1; /**< Digital In */
    unsigned char DI2 : 1; /**< Digital In */
    unsigned char DI3 : 1; /**< Digital In */
    unsigned char DI4 : 1; /**< Digital In */
    unsigned char DI5 : 1; /**< Digital In */
    unsigned char DI6 : 1; /**< Digital In */
    unsigned char DI7 : 1; /**< Digital In */
    unsigned char DI8 : 1; /**< Digital In */
} kvIoModuleRelay;

/**
 *
 * This define is used in \ref kvIoGetModulePins() and \ref kvIoSetModulePins().
 * This struct represents an analog add-on module.
 */

typedef struct {
    int type; /**< The type of the module. See: \ref kvIO_MODULE_TYPE_xxx.. */
    float AO1; /**< Analog Out */
    float AO2; /**< Analog Out */
    float AO3; /**< Analog Out */
    float AO4; /**< Analog Out */
    float AI1; /**< Analog In */
    float AI2; /**< Analog In */
    float AI3; /**< Analog In */
    float AI4; /**< Analog In */
} kvIoModuleAnalog;


/**
 * \ingroup kv_io
 *
 * \source_cs       <b>static Canlib.canStatus kvIoGetModulePins(int hnd, int module, object buffer);</b>
 *
 * \source_delphi   <b>function kvIoGetModulePins(hnd: canHandle; module: cardinal; var buffer; bufsize: cardinal): canStatus;</b>
 * \source_end
 *
 * This function is used to read all the pins on one module in a single call.
 *
 * \param[in]  hnd      An open handle to a CAN channel.
 * \param[in]  module   The module number, see \ref kvIO_INFO_GET_MODULE_NUMBER.
 * \param[out] buffer   A pointer to a struct that receives the pin values of the module. The struct can be any one of the following depending on the module. The returned type is described in the first byte of the returned struct. See:
 * - \ref kvIoModuleDigital
 * - \ref kvIoModuleAnalog
 * - \ref kvIoModuleRelay
 * \param[in]  bufsize  The size of the struct pointed to by buffer
 *
 * \sa \ref kvIoSetModulePins()
 * \note Preliminary API that may change.
 * \note Not implemented in Linux.
 */
canStatus CANLIBAPI kvIoGetModulePins(const CanHandle hnd, unsigned int module, void *buffer, const unsigned int bufsize);


/**
 * \ingroup kv_io
 *
 * \source_cs       <b>static Canlib.canStatus kvIoSetModulePins(int hnd, int module, object buffer);</b>
 *
 * \source_delphi   <b>function kvIoSetModulePins(hnd: canHandle; module: cardinal; var buffer; bufsize: cardinal): canStatus;</b>
 * \source_end
 *
 * This function is used to set all the pins on one single module in a single call.
 *
 * \param[in]  hnd      An open handle to a CAN channel.
 * \param[in]  module   The module number, see \ref kvIO_INFO_GET_MODULE_NUMBER.
 * \param[out] buffer   A pointer to a struct that contains the module type and pin values to set. The struct can be any one of the following depending on the module.
 * - \ref kvIoModuleDigital
 * - \ref kvIoModuleAnalog
 * - \ref kvIoModuleRelay
 * \param[in]  bufsize  The size of the struct pointed to by buffer
 *
 * \sa \ref kvIoGetModulePins()
 * \note Note The input Pins are ignored.
 * \note Preliminary API that may change.
 * \note Not implemented in Linux.
 */
canStatus CANLIBAPI kvIoSetModulePins(const CanHandle hnd, unsigned int module, const void *buffer, const unsigned int bufsize);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "obsolete.h"

#endif /*  _CANLIB_H_ */
