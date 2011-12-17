/****************************************************************************************
|  Description: XCP 1.0 protocol core source file
|    File Name: xcp.c
|
|----------------------------------------------------------------------------------------
|                          C O P Y R I G H T
|----------------------------------------------------------------------------------------
|   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
|
|----------------------------------------------------------------------------------------
|                            L I C E N S E
|----------------------------------------------------------------------------------------
| This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
| modify it under the terms of the GNU General Public License as published by the Free
| Software Foundation, either version 3 of the License, or (at your option) any later
| version.
|
| OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with OpenBLT.
| If not, see <http://www.gnu.org/licenses/>.
|
| A special exception to the GPL is included to allow you to distribute a combined work 
| that includes OpenBLT without being obliged to provide the source code for any 
| proprietary components. The exception text is included at the bottom of the license
| file <license.html>.
| 
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */


/****************************************************************************************
* Defines
****************************************************************************************/
/* XCP protocol layer version number (16-bit) */
#define XCP_VERSION_PROTOCOL_LAYER  (0x0100)

/* XCP transport layer version number (16-bit) */
#define XCP_VERSION_TRANSPORT_LAYER (0x0100)

/* XCP packet identifiers */
#define XCP_PID_RES                 (0xff)            /* command response packet       */
#define XCP_PID_ERR                 (0xfe)            /* error packet                  */

/* XCP error codes */
#define XCP_ERR_CMD_SYNCH           (0x00)            /* cmd processor synchronization */
#define XCP_ERR_CMD_BUSY            (0x10)            /* command was not executed      */
#define XCP_ERR_CMD_UNKNOWN         (0x20)            /* unknown or unsupported command*/
#define XCP_ERR_OUT_OF_RANGE        (0x22)            /* parameter out of range        */
#define XCP_ERR_ACCESS_LOCKED       (0x25)            /* protected. seed/key required  */
#define XCP_ERR_PAGE_NOT_VALID      (0x26)            /* cal page not valid            */
#define XCP_ERR_SEQUENCE            (0x29)            /* sequence error                */
#define XCP_ERR_GENERIC             (0x31)            /* generic error                 */

/* XCP command codes */
#define XCP_CMD_CONNECT             (0xff)            /* CONNECT command code          */
#define XCP_CMD_DISCONNECT          (0xfe)            /* DISCONNECT command code       */
#define XCP_CMD_GET_STATUS          (0xfd)            /* GET_STATUS command code       */
#define XCP_CMD_SYNCH               (0xfc)            /* SYNCH command code            */
#define XCP_CMD_GET_ID              (0xfa)            /* GET_ID command code           */
#define XCP_CMD_GET_SEED            (0xf8)            /* GET_SEED command code         */
#define XCP_CMD_UNLOCK              (0xf7)            /* UNLOCK command code           */
#define XCP_CMD_SET_MTA             (0xf6)            /* SET_MTA command code          */
#define XCP_CMD_UPLOAD              (0xf5)            /* UPLOAD command code           */
#define XCP_CMD_SHORT_UPLOAD        (0xf4)            /* SHORT_UPLOAD command code     */
#define XCP_CMD_BUILD_CHECKSUM      (0xf3)            /* BUILD_CHECKSUM command code   */
#define XCP_CMD_DOWNLOAD            (0xf0)            /* DOWNLOAD command code         */
#define XCP_CMD_DOWLOAD_MAX         (0xee)            /* DOWNLOAD_MAX command code     */
#define XCP_CMD_SET_CAL_PAGE        (0xeb)            /* SET_CALPAGE command code      */
#define XCP_CMD_GET_CAL_PAGE        (0xea)            /* GET_CALPAGE command code      */
#define XCP_CMD_PROGRAM_START       (0xd2)            /* PROGRAM_START command code    */
#define XCP_CMD_PROGRAM_CLEAR       (0xd1)            /* PROGRAM_CLEAR command code    */
#define XCP_CMD_PROGRAM             (0xd0)            /* PROGRAM command code          */
#define XCP_CMD_PROGRAM_RESET       (0xcf)            /* PROGRAM_RESET command code    */
#define XCP_CMD_PROGRAM_PREPARE     (0xcc)            /* PROGRAM_PREPARE command code  */
#define XCP_CMD_PROGRAM_MAX         (0xc9)            /* PROGRAM_MAX command code      */


/****************************************************************************************
* Type definitions
****************************************************************************************/
/* XCP internal module information type */
typedef struct
{
  blt_int8u  connected;                               /* connection established        */
  blt_int8u  protection;                              /* protection state              */
  blt_int8u  s_n_k_resource;                          /* for seed/key sequence         */
  blt_int8u  ctoData[XCP_CTO_PACKET_LEN];             /* cto packet data buffer        */
  blt_int8u  ctoPending;                              /* cto transmission pending flag */
  blt_int16s ctoLen;                                  /* cto current packet length     */
  blt_int32u mta;                                     /* memory transfer address       */
} tXcpInfo;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
/* transport layer specific functions */
static void      XcpTransmitPacket(blt_int8u *data, blt_int16s len);

/* application specific functions */
static blt_int8u XcpComputeChecksum(blt_int32u address, blt_int32u length, 
                                    blt_int32u *checksum);

#if (XCP_SEED_KEY_PROTECTION_EN == 1)
static blt_int8u XcpGetSeed(blt_int8u resource, blt_int8u *seed);
static blt_int8u XcpVerifyKey(blt_int8u resource, blt_int8u *key, blt_int8u len);
#endif

/* general utility functions */
static void XcpProtectResources(void);
static void XcpSetCtoError(blt_int8u error);

/* XCP command processors */
static void XcpCmdConnect(blt_int8u *data);
static void XcpCmdDisconnect(blt_int8u *data);
static void XcpCmdGetStatus(blt_int8u *data);
static void XcpCmdSynch(blt_int8u *data);
static void XcpCmdGetId(blt_int8u *data);
static void XcpCmdSetMta(blt_int8u *data);
static void XcpCmdUpload(blt_int8u *data);
static void XcpCmdShortUpload(blt_int8u *data);
static void XcpCmdBuildCheckSum(blt_int8u *data);
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
static void XcpCmdGetSeed(blt_int8u *data);
static void XcpCmdUnlock(blt_int8u *data);
#endif
#if (XCP_RES_CALIBRATION_EN == 1)
static void XcpCmdDownload(blt_int8u *data);
static void XcpCmdDownloadMax(blt_int8u *data);
#endif
#if (XCP_RES_PAGING_EN == 1)
static void XcpCmdSetCalPage(blt_int8u *data);
static void XcpCmdGetCalPage(blt_int8u *data);
#endif
#if (XCP_RES_PROGRAMMING_EN == 1)
static void XcpCmdProgramMax(blt_int8u *data);
static void XcpCmdProgram(blt_int8u *data);
static void XcpCmdProgramStart(blt_int8u *data);
static void XcpCmdProgramClear(blt_int8u *data);
static void XcpCmdProgramReset(blt_int8u *data);
static void XcpCmdProgramPrepare(blt_int8u *data);
#endif


/****************************************************************************************
* Hook functions
****************************************************************************************/
#if F_CAL_RES_PAGING_EN == 1
blt_int8u AppCalSetPage(blt_int8u segment, blt_int8u page);
blt_int8u AppCalGetPage(blt_int8u segment);
#endif


/****************************************************************************************
* Local constants
****************************************************************************************/
/* string buffer with station id */
static const blt_int8s xcpStationId[] = XCP_STATION_ID_STRING;


/****************************************************************************************
* Local data definitions
****************************************************************************************/
static tXcpInfo xcpInfo;                               /* XCP internal module info     */


/****************************************************************************************
** NAME:           XcpInit
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Initializes the XCP driver. Should be called once upon system startup.
**
****************************************************************************************/
void XcpInit(void)
{
  /* reset xcp module info */
  xcpInfo.connected = 0;
  xcpInfo.mta = 0;
  xcpInfo.ctoPending = 0;
  xcpInfo.ctoLen = 0;
  xcpInfo.s_n_k_resource = 0;
  xcpInfo.protection = 0;
} /*** end of XcpInit ***/


/****************************************************************************************
** NAME:           XcpIsConnected
** PARAMETER:      none
** RETURN VALUE:   BLT_TRUE is an XCP connection is established, BLT_FALSE otherwise.
** DESCRIPTION:    Obtains information about the XCP connection state.
**
****************************************************************************************/
blt_bool XcpIsConnected(void)
{
  if (xcpInfo.connected == 0)
  {
    return BLT_FALSE;
  }
  return BLT_TRUE;
} /*** end of XcpIsConnected ***/


/****************************************************************************************
** NAME:           XcpPacketTransmitted
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Informs the core that a pending packet transmission was completed by 
**                 the transport layer.
**
****************************************************************************************/
void XcpPacketTransmitted(void)
{
  /* reset packet transmission pending flag */
  xcpInfo.ctoPending = 0;
} /*** end of XcpPacketTransmitted ***/


/****************************************************************************************
** NAME:           XcpPacketReceived
** PARAMETER:      data pointer to byte buffer with packet data
** RETURN VALUE:   none
** DESCRIPTION:    Informs the core that a new packet was received by the transport 
**                 layer.
**
****************************************************************************************/
void XcpPacketReceived(blt_int8u *data)
{
  /* was this a connect command? */
  if (data[0] == XCP_CMD_CONNECT)
  {
    /* process the connect command */
    XcpCmdConnect(data);
  }
  /* only continue if connected */
  else if (xcpInfo.connected == 1)
  {
    switch (data[0])
    {
      case XCP_CMD_UPLOAD:
        XcpCmdUpload(data);
        break;
      case XCP_CMD_SHORT_UPLOAD:
        XcpCmdShortUpload(data);
        break;
      case XCP_CMD_SET_MTA:
        XcpCmdSetMta(data);
        break;
      case XCP_CMD_BUILD_CHECKSUM:
        XcpCmdBuildCheckSum(data);
        break;
      case XCP_CMD_GET_ID:
        XcpCmdGetId(data);
        break;
      case XCP_CMD_SYNCH:
        XcpCmdSynch(data);
        break;
      case XCP_CMD_GET_STATUS:
        XcpCmdGetStatus(data);
        break;
      case XCP_CMD_DISCONNECT:
        XcpCmdDisconnect(data);
        break;
#if (XCP_RES_CALIBRATION_EN == 1)
      case XCP_CMD_DOWNLOAD:
        XcpCmdDownload(data);
        break;
      case XCP_CMD_DOWLOAD_MAX:
        XcpCmdDownloadMax(data);
        break;
#endif
#if (XCP_RES_PROGRAMMING_EN == 1)
      case XCP_CMD_PROGRAM_MAX:
        XcpCmdProgramMax(data);
        break;
      case XCP_CMD_PROGRAM:
        XcpCmdProgram(data);
        break;
      case XCP_CMD_PROGRAM_START:
        XcpCmdProgramStart(data);
        break;
      case XCP_CMD_PROGRAM_CLEAR:
        XcpCmdProgramClear(data);
        break;
      case XCP_CMD_PROGRAM_RESET:
        XcpCmdProgramReset(data);
        break;
      case XCP_CMD_PROGRAM_PREPARE:
        XcpCmdProgramPrepare(data);
        break;
#endif
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
      case XCP_CMD_GET_SEED:
        XcpCmdGetSeed(data);
        break;
      case XCP_CMD_UNLOCK:
        XcpCmdUnlock(data);
        break;
#endif
#if (XCP_RES_PAGING_EN == 1)
      case XCP_CMD_SET_CAL_PAGE:
        XcpCmdSetCalPage(data);
        break;
      case XCP_CMD_GET_CAL_PAGE:
        XcpCmdGetCalPage(data);
        break;
#endif
      default:
        XcpSetCtoError(XCP_ERR_CMD_UNKNOWN);
        break;
    }
  }
  else
  {
    /* return to make sure response packet is not send because we are not connected */
    return;
  }

  /* make sure the previous command was completed */
  if (xcpInfo.ctoPending == 1)
  {
    /* command overrun occurred */
    XcpSetCtoError(XCP_ERR_CMD_BUSY);
  }

  /* set cto packet transmission pending flag */
  xcpInfo.ctoPending = 1;

  /* transmit the cto response packet */
  XcpTransmitPacket(xcpInfo.ctoData, xcpInfo.ctoLen);
} /*** end of XcpPacketReceived ***/


/****************************************************************************************
** NAME:           XcpTransmitPacket
** PARAMETER:      data pointer to the byte buffer with packet data.
**                 len  number of data bytes that need to be transmitted.
** RETURN VALUE:   none
** DESCRIPTION:    Transmits the packet using the xcp transport layer.
**
****************************************************************************************/
static void XcpTransmitPacket(blt_int8u *data, blt_int16s len)
{
  /* submit packet to the communication interface for transmission */
  ComTransmitPacket(data, len);
} /*** end of XcpTransmitPacket ***/


/****************************************************************************************
** NAME:           XcpComputeChecksum
** PARAMETER:      address   the start address of the memory region.
**                 length    length of the memory region in bytes.
**                 checksum  pointer to where the calculated checksum is to be stored.
** RETURN VALUE:   none
** DESCRIPTION:    Called by the BUILD_CHECKSUM command to perform a checksum calculation
**                 over the specified memory region.
**
****************************************************************************************/
static blt_int8u XcpComputeChecksum(blt_int32u address, blt_int32u length,
                                    blt_int32u *checksum)
{
  blt_int8u cs = 0;

  /* this example computes the checksum using the add byte to byte algorithm */
  while (length-- > 0)
  {
    cs += *((blt_int8u*)(blt_addr)address);
    address++;
  }

  *checksum = cs;

  return XCP_CS_ADD11;
} /*** end of XcpComputeChecksum ***/


#if (XCP_SEED_KEY_PROTECTION_EN == 1)
/****************************************************************************************
** NAME:           XcpGetSeed
** PARAMETER:      resource  resource that the seed if requested for (XCP_RES_XXX).
**                 seed      pointer to byte buffer wher the seed will be stored.
** RETURN VALUE:   none
** DESCRIPTION:    Provides a seed to the XCP master that will be used for the key 
**                 generation when the master attempts to unlock the specified resource. 
**                 Called by the GET_SEED command.
**
****************************************************************************************/
static blt_int8u XcpGetSeed(blt_int8u resource, blt_int8u *seed)
{
  /* request seed for unlocking ProGraMming resource */
  if ((resource & XCP_RES_PGM) != 0)
  {
    seed[0] = 0x55;
  }

  /* request seed for unlocking CALibration and PAGing resource */
  if ((resource & XCP_RES_CALPAG) != 0)
  {
    seed[0] = 0xaa;
  }

  /* return seed length */
  return 1;
} /*** end of XcpGetSeed ***/


/****************************************************************************************
** NAME:           XcpVerifyKey
** PARAMETER:      resource  resource to unlock (XCP_RES_XXX).
**                 key       pointer to the byte buffer holding the key.
**                 len       length of the key in bytes.
** RETURN VALUE:   none
** DESCRIPTION:    Called by the UNLOCK command and checks if the key to unlock the 
**                 specified resource was correct. If so, then the resource protection 
**                 will be removed.
**
****************************************************************************************/
static blt_int8u XcpVerifyKey(blt_int8u resource, blt_int8u *key, blt_int8u len)
{
  /* suppress compiler warning for unused parameter */
  len = len;

  /* the example key algorithm in "FeaserKey.dll" works as follows:
   *  - PGM will be unlocked if key = seed - 1
   *  - CAL_PAG will be unlocked if key = seed + 1
   */

  /* check key for unlocking ProGraMming resource */
  if ((resource == XCP_RES_PGM) && (key[0] == (0x55-1)))
  {
    /* correct key received for unlocking PGM resource */
    return 1;
  }

  /* check key for unlocking CALibration and PAGing resource */
  if ((resource == XCP_RES_CALPAG) && (key[0] == (0xaa+1)))
  {
    /* correct key received for unlocking CAL_PAG resource */
    return 1;
  }

  /* still here so key incorrect */
  return 0;
} /*** end of XcpVerifyKey ***/
#endif /* XCP_SEED_KEY_PROTECTION_EN == 1 */


/****************************************************************************************
** NAME:           XcpProtectResources
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Utility function to protects all the available resources.
**
****************************************************************************************/
static void XcpProtectResources(void)
{
  xcpInfo.protection = 0;

#if (XCP_SEED_KEY_PROTECTION_EN == 1)
  #if (XCP_RES_CALIBRATION_EN == 1)
  xcpInfo.protection |= XCP_RES_CALPAG;
  #endif

  #if (XCP_RES_PAGING_EN == 1)
  xcpInfo.protection |= XCP_RES_CALPAG;
  #endif

  #if (XCP_RES_PROGRAMMING_EN == 1)
  xcpInfo.protection |= XCP_RES_PGM;
  #endif

  #if (XCP_RES_DATA_ACQUISITION_EN == 1)
  xcpInfo.protection |= XCP_RES_DAQ;
  #endif

  #if (XCP_RES_DATA_STIMULATION_EN == 1)
  xcpInfo.protection |= XCP_RES_STIM;
  #endif
#endif /* XCP_SEED_KEY_PROTECTION_EN == 1 */
} /*** end of XcpProtectResources ***/


/****************************************************************************************
** NAME:           XcpSetCtoError
** PARAMETER:      error xcp error code (XCP_ERR_XXX)
** RETURN VALUE:   none
** DESCRIPTION:    Prepares the cto packet data for the specified error.
**
****************************************************************************************/
static void XcpSetCtoError(blt_int8u error)
{
  /* prepare the error packet */
  xcpInfo.ctoData[0] = XCP_PID_ERR;
  xcpInfo.ctoData[1] = error;
  xcpInfo.ctoLen = 2;
} /*** end of XcpSetCtoError ***/


/****************************************************************************************
** NAME:           XcpCmdConnect
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the CONNECT command as
**                 defined by the protocol.
**
****************************************************************************************/
static void XcpCmdConnect(blt_int8u *data)
{
  /* suppress compiler warning for unused parameter */
  data = data;

  /* enable resource protection */
  XcpProtectResources();

  /* indicate that the connection is established */
  xcpInfo.connected = 1;

  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* report available resources */
  xcpInfo.ctoData[1] = 0;
#if (XCP_RES_CALIBRATION_EN == 1)
  xcpInfo.ctoData[1] |= XCP_RES_CALPAG;
#endif

#if (XCP_RES_PAGING_EN == 1)
  xcpInfo.ctoData[1] |= XCP_RES_CALPAG;
#endif

#if (XCP_RES_PROGRAMMING_EN == 1)
  xcpInfo.ctoData[1] |= XCP_RES_PGM;
#endif

#if (XCP_RES_DATA_ACQUISITION_EN == 1)
  xcpInfo.ctoData[1] |= XCP_RES_DAQ;
#endif

#if (XCP_RES_DATA_STIMULATION_EN == 1)
  xcpInfo.ctoData[1] |= XCP_RES_STIM;
#endif

  /* report communication mode info. only byte granularity is supported */
  xcpInfo.ctoData[2] = 0;
  /* configure for motorola or intel byte ordering */
  xcpInfo.ctoData[2] |= XCP_MOTOROLA_FORMAT;

  /* report max cto data length */
  xcpInfo.ctoData[3] = XCP_CTO_PACKET_LEN;

  /* report max dto data length */
#if (XCP_MOTOROLA_FORMAT == 0)
  xcpInfo.ctoData[4] = (blt_int8u)XCP_DTO_PACKET_LEN;
  xcpInfo.ctoData[5] = (blt_int8u)(XCP_DTO_PACKET_LEN >> 8);
#else
  xcpInfo.ctoData[4] = (blt_int8u)(XCP_DTO_PACKET_LEN >> 8);
  xcpInfo.ctoData[5] = (blt_int8u)XCP_DTO_PACKET_LEN;
#endif

  /* report msb of protocol layer version number */
  xcpInfo.ctoData[6] = XCP_VERSION_PROTOCOL_LAYER >> 8;

  /* report msb of transport layer version number */
  xcpInfo.ctoData[7] = XCP_VERSION_TRANSPORT_LAYER >> 8;

  /* set packet length */
  xcpInfo.ctoLen = 8;
} /*** end of XcpCmdConnect ***/


/****************************************************************************************
** NAME:           XcpCmdDisconnect
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the DISCONNECT command as
**                 defined by the protocol.
**
****************************************************************************************/
static void XcpCmdDisconnect(blt_int8u *data)
{
  /* suppress compiler warning for unused parameter */
  data = data;

  /* indicate that the xcp connection is disconnected */
  xcpInfo.connected = 0;

  /* enable resource protection */
  XcpProtectResources();

  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* set packet length */
  xcpInfo.ctoLen = 1;
} /*** end of XcpCmdDisconnect ***/


/****************************************************************************************
** NAME:           XcpCmdGetStatus
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the GET_STATUS command as
**                 defined by the protocol.
**
****************************************************************************************/
static void XcpCmdGetStatus(blt_int8u *data)
{
  /* suppress compiler warning for unused parameter */
  data = data;

  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* report session status */
  xcpInfo.ctoData[1] = 0;

  /* report current resource protection status */
  xcpInfo.ctoData[2] = xcpInfo.protection;

  /* reset reserved and session configuration id values */
  xcpInfo.ctoData[3] = 0;
  xcpInfo.ctoData[4] = 0;
  xcpInfo.ctoData[5] = 0;

  /* set packet length */
  xcpInfo.ctoLen = 6;
} /*** end of XcpCmdGetStatus ***/


/****************************************************************************************
** NAME:           XcpCmdSynch
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the SYNCH command as
**                 defined by the protocol.
**
****************************************************************************************/
static void XcpCmdSynch(blt_int8u *data)
{
  /* suppress compiler warning for unused parameter */
  data = data;

  /* synch requires a negative response */
  XcpSetCtoError(XCP_ERR_CMD_SYNCH);
} /*** end of XcpCmdSynch ***/


/****************************************************************************************
** NAME:           XcpCmdGetId
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the GET_ID command as
**                 defined by the protocol.
**
****************************************************************************************/
static void XcpCmdGetId(blt_int8u *data)
{
  /* suppress compiler warning for unused parameter */
  data = data;

  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* point mta to start of station id string */
  xcpInfo.mta = (blt_int32u)&xcpStationId[0];

  /* set station id mode to 0 */
  xcpInfo.ctoData[1] = 0;

  /* reset reserved values */
  xcpInfo.ctoData[2] = 0;
  xcpInfo.ctoData[3] = 0;

  /* store station id length (excl. null termination) for response packet */
  *(blt_int32u*)&xcpInfo.ctoData[4] = (sizeof(xcpStationId)/sizeof(xcpStationId[0])) - 1;

  /* set packet length */
  xcpInfo.ctoLen = 8;
} /*** end of XcpCmdGetId ***/


/****************************************************************************************
** NAME:           XcpCmdSetMta
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the SET_MTA command as
**                 defined by the protocol.
**
****************************************************************************************/
static void XcpCmdSetMta(blt_int8u *data)
{
  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* update mta. current implementation ignores address extension */
  xcpInfo.mta = *(blt_int32u*)&data[4];

  /* set packet length */
  xcpInfo.ctoLen = 1;
} /*** end of XcpCmdSetMta ***/


/****************************************************************************************
** NAME:           XcpCmdUpload
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the UPLOAD command as
**                 defined by the protocol.
**
****************************************************************************************/
static void XcpCmdUpload(blt_int8u *data)
{
  /* validate length of upload request */
  if (data[1] > (XCP_CTO_PACKET_LEN-1))
  {
    /* requested data length is too long */
    XcpSetCtoError(XCP_ERR_OUT_OF_RANGE);
    return;
  }

  /* copy the data from memory to the data packet */
  CpuMemCopy(((blt_addr)(blt_int32u)&xcpInfo.ctoData[1]),(blt_addr)xcpInfo.mta, data[1]);

  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* post increment the mta */
  xcpInfo.mta += data[1];

  /* set packet length */
  xcpInfo.ctoLen = data[1]+1;
} /*** end of XcpCmdUpload ***/


/****************************************************************************************
** NAME:           XcpCmdShortUpload
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the SHORT_UPLOAD command as
**                 defined by the protocol.
**
****************************************************************************************/
static void XcpCmdShortUpload(blt_int8u *data)
{
  /* validate length of upload request */
  if (data[1] > (XCP_CTO_PACKET_LEN-1))
  {
    /* requested data length is too long */
    XcpSetCtoError(XCP_ERR_OUT_OF_RANGE);
    return;
  }

  /* update mta. current implementation ignores address extension */
  xcpInfo.mta = *(blt_int32u*)&data[4];

  /* copy the data from memory to the data packet */
  CpuMemCopy((blt_addr)((blt_int32u)&xcpInfo.ctoData[1]),(blt_addr)xcpInfo.mta, data[1]);
  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* post increment the mta */
  xcpInfo.mta += data[1];

  /* set packet length */
  xcpInfo.ctoLen = data[1]+1;
} /*** end of XcpCmdShortUpload ***/


#if (XCP_RES_CALIBRATION_EN == 1)
/****************************************************************************************
** NAME:           XcpCmdDownload
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the DOWNLOAD command as
**                 defined by the protocol.
**
****************************************************************************************/
static void XcpCmdDownload(blt_int8u *data)
{
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
  /* check if CAL_PAG resource is unlocked */
  if ((xcpInfo.protection & XCP_RES_CALPAG) != 0)
  {
    /* resource is locked. use seed/key sequence to unlock */
    XcpSetCtoError(XCP_ERR_ACCESS_LOCKED);
    return;
  }
#endif

  /* validate length of download request */
  if (data[1] > (XCP_CTO_PACKET_LEN-2))
  {
    /* requested data length is too long */
    XcpSetCtoError(XCP_ERR_OUT_OF_RANGE);
    return;
  }

  /* copy the data from the data packet to memory */
  CpuMemCopy((blt_addr)xcpInfo.mta, (blt_addr)((blt_int32u)&data[2]), data[1]);
  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* post increment the mta */
  xcpInfo.mta += data[1];

  /* set packet length */
  xcpInfo.ctoLen = 1;
} /*** end of XcpCmdDownload ***/


/****************************************************************************************
** NAME:           XcpCmdDownloadMax
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the DOWNLOAD_MAX command 
**                 as defined by the protocol.
**
****************************************************************************************/
static void XcpCmdDownloadMax(blt_int8u *data)
{
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
  /* check if CAL_PAG resource is unlocked */
  if ((xcpInfo.protection & XCP_RES_CALPAG) != 0)
  {
    /* resource is locked. use seed/key sequence to unlock */
    XcpSetCtoError(XCP_ERR_ACCESS_LOCKED);
    return;
  }
#endif

  /* copy the data from the data packet to memory */
  CpuMemCopy((blt_addr)xcpInfo.mta, (blt_addr)((blt_int32u)&data[1]), \
              XCP_CTO_PACKET_LEN-1);

  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* post increment the mta */
  xcpInfo.mta += XCP_CTO_PACKET_LEN-1;

  /* set packet length */
  xcpInfo.ctoLen = 1;
} /*** end of XcpCmdDownloadMax ***/
#endif /* XCP_RES_CALIBRATION_EN == 1 */


/****************************************************************************************
** NAME:           XcpCmdBuildCheckSum
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the BUILD_CHECKSUM 
**                 command as defined by the protocol.
**
****************************************************************************************/
static void XcpCmdBuildCheckSum(blt_int8u *data)
{
  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* obtain checksum and checksum type */
  xcpInfo.ctoData[1] = XcpComputeChecksum(xcpInfo.mta, *(blt_int32u*)&data[4],
                                          (blt_int32u*)&xcpInfo.ctoData[4]);

  /* initialize reserved parameters */
  xcpInfo.ctoData[2] = 0;
  xcpInfo.ctoData[3] = 0;

  /* set packet length */
  xcpInfo.ctoLen = 8;
} /*** end of XcpCmdBuildCheckSum ***/


#if (XCP_SEED_KEY_PROTECTION_EN == 1)
/****************************************************************************************
** NAME:           XcpCmdGetSeed
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the GET_SEED command as
**                 defined by the protocol.
**
****************************************************************************************/
static void XcpCmdGetSeed(blt_int8u *data)
{
  blt_int8u resourceOK;

  /* init resource check variable as if an illegal resource is requested */
  resourceOK = 0;

  /* check if calibration/paging resource is requested for seed/key and make
   * sure this is the only requested resource
   */
  if (((data[2] & XCP_RES_CALPAG) > 0) && ((data[2] & ~XCP_RES_CALPAG) == 0))
  {
    resourceOK = 1;
  }

  /* check if programming resource is requested for seed/key and make
   * sure this is the only requested resource
   */
  if (((data[2] & XCP_RES_PGM) > 0) && ((data[2] & ~XCP_RES_PGM) == 0))
  {
    resourceOK = 1;
  }

  /* check if data acquisition resource is requested for seed/key and make
   * sure this is the only requested resource
   */
  if (((data[2] & XCP_RES_DAQ) > 0) && ((data[2] & ~XCP_RES_DAQ) == 0))
  {
    resourceOK = 1;
  }

  /* check if data stimulation resource is requested for seed/key and make
   * sure this is the only requested resource
   */
  if (((data[2] & XCP_RES_STIM) > 0) && ((data[2] & ~XCP_RES_STIM) == 0))
  {
    resourceOK = 1;
  }

  /* now process the resource validation */
  if (resourceOK == 0)
  {
    XcpSetCtoError(XCP_ERR_OUT_OF_RANGE);
    return;
  }

  /* store resource for which the seed/key sequence is started */
  xcpInfo.s_n_k_resource = data[2];

  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* request the seed from the application */
  xcpInfo.ctoData[1] = XcpGetSeed(xcpInfo.s_n_k_resource, &xcpInfo.ctoData[2]);

  /* seed cannot be longer than XCP_CTO_PACKET_LEN-2 */
  if (xcpInfo.ctoData[1] > (XCP_CTO_PACKET_LEN-2))
  {
    /* seed length length is too long */
    XcpSetCtoError(XCP_ERR_OUT_OF_RANGE);
    return;
  }

  /* set packet length */
  xcpInfo.ctoLen = xcpInfo.ctoData[1] + 2;
} /*** end of XcpCmdGetSeed ***/


/****************************************************************************************
** NAME:           XcpCmdUnlock
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the UNLOCK command as
**                 defined by the protocol.
**
****************************************************************************************/
static void XcpCmdUnlock(blt_int8u *data)
{
  /* key cannot be longer than XCP_CTO_PACKET_LEN-2 */
  if (data[1] > (XCP_CTO_PACKET_LEN-2))
  {
    /* key is too long incorrect */
    XcpSetCtoError(XCP_ERR_SEQUENCE);
    return;
  }

  /* verify the key */
  if (XcpVerifyKey(xcpInfo.s_n_k_resource, &data[2], data[1]) == 0)
  {
    /* invalid key so inform the master and do a disconnect */
    XcpSetCtoError(XCP_ERR_ACCESS_LOCKED);

    /* indicate that the xcp connection is disconnected */
    xcpInfo.connected = 0;

    /* enable resource protection */
    XcpProtectResources();

    return;
  }

  /* key correct so unlock the resource */
  xcpInfo.protection &= ~xcpInfo.s_n_k_resource;

  /* reset seed/key resource variable for possible next unlock */
  xcpInfo.s_n_k_resource = 0;

  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* report the current resource protection */
  xcpInfo.ctoData[1] = xcpInfo.protection;

  /* set packet length */
  xcpInfo.ctoLen = 2;
} /*** end of XcpCmdUnlock ***/
#endif /* XCP_SEED_KEY_PROTECTION_EN == 1 */


#if (XCP_RES_PAGING_EN == 1)
/****************************************************************************************
** NAME:           XcpCmdSetCalPage
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the SET_CAL_PAGE command
**                 as defined by the protocol.
**
****************************************************************************************/
static void XcpCmdSetCalPage(blt_int8u *data)
{
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
  /* check if CAL_PAG resource is unlocked */
  if ((xcpInfo.protection & XCP_RES_CALPAG) == XCP_RES_CALPAG)
  {
    /* resource is locked. use seed/key sequence to unlock */
    XcpSetCtoError(XCP_ERR_ACCESS_LOCKED);
    return;
  }
#endif

  /* select the page. note that the mode parameter is ignored */
  if (AppCalSetPage(data[2], data[3]) == 0)
  {
    /* calibration page could not be selected */
    XcpSetCtoError(XCP_ERR_PAGE_NOT_VALID);
    return;
  }

  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* set packet length */
  xcpInfo.ctoLen = 1;
} /*** end of XcpCmdSetCalPage ***/


/****************************************************************************************
** NAME:           XcpCmdGetCalPage
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the GET_CAL_PAGE command
**                 as defined by the protocol.
**
****************************************************************************************/
static void XcpCmdGetCalPage(blt_int8u *data)
{
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
  /* check if CAL_PAG resource is unlocked */
  if ((xcpInfo.protection & XCP_RES_CALPAG) == XCP_RES_CALPAG)
  {
    /* resource is locked. use seed/key sequence to unlock */
    XcpSetCtoError(XCP_ERR_ACCESS_LOCKED);
    return;
  }
#endif

  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* initialize reserved parameters */
  xcpInfo.ctoData[1] = 0;
  xcpInfo.ctoData[2] = 0;

  /* store the calibration page */
  xcpInfo.ctoData[3] = AppCalGetPage(data[2]);

  /* set packet length */
  xcpInfo.ctoLen = 4;
} /*** end of XcpCmdGetCalPage ***/
#endif /* XCP_RES_PAGING_EN == 1 */


#if (XCP_RES_PROGRAMMING_EN == 1)
/****************************************************************************************
** NAME:           XcpCmdProgramStart
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the PROGRAM_START command
**                 as defined by the protocol.
**
****************************************************************************************/
static void XcpCmdProgramStart(blt_int8u *data)
{
  /* suppress compiler warning for unused parameter */
  data = data;

#if (XCP_SEED_KEY_PROTECTION_EN == 1)
  /* check if PGM resource is unlocked */
  if ((xcpInfo.protection & XCP_RES_PGM) == XCP_RES_PGM)
  {
    /* resource is locked. use seed/key sequence to unlock */
    XcpSetCtoError(XCP_ERR_ACCESS_LOCKED);
    return;
  }
#endif

  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* initialize reserved parameter */
  xcpInfo.ctoData[1] = 0;

  /* no special communication mode supported during programming */
  xcpInfo.ctoData[2] = 0;

  /* cto packet length stays the same during programming */
  xcpInfo.ctoData[3] = XCP_CTO_PACKET_LEN;

  /* no block size, st-min time, or queue size supported */
  xcpInfo.ctoData[4] = 0;
  xcpInfo.ctoData[5] = 0;
  xcpInfo.ctoData[6] = 0;

  /* set packet length */
  xcpInfo.ctoLen = 7;
} /*** end of XcpCmdProgramStart ***/


/****************************************************************************************
** NAME:           XcpCmdProgramMax
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the PROGRAM_MAX command 
**                 as defined by the protocol.
**
****************************************************************************************/
static void XcpCmdProgramMax(blt_int8u *data)
{
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
  /* check if PGM resource is unlocked */
  if ((xcpInfo.protection & XCP_RES_PGM) == XCP_RES_PGM)
  {
    /* resource is locked. use seed/key sequence to unlock */
    XcpSetCtoError(XCP_ERR_ACCESS_LOCKED);
    return;
  }
#endif

  /* program the data */
  if (NvmWrite((blt_addr)xcpInfo.mta, XCP_CTO_PACKET_LEN-1, &data[1]) == 0)
  {
    /* error occurred during programming */
    XcpSetCtoError(XCP_ERR_GENERIC);
    return;
  }

  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* post increment the mta */
  xcpInfo.mta += XCP_CTO_PACKET_LEN-1;

  /* set packet length */
  xcpInfo.ctoLen = 1;
} /*** end of XcpCmdProgramMax ***/


/****************************************************************************************
** NAME:           XcpCmdProgram
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the PROGRAM command as
**                 defined by the protocol.
**
****************************************************************************************/
static void XcpCmdProgram(blt_int8u *data)
{
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
  /* check if PGM resource is unlocked */
  if ((xcpInfo.protection & XCP_RES_PGM) == XCP_RES_PGM)
  {
    /* resource is locked. use seed/key sequence to unlock */
    XcpSetCtoError(XCP_ERR_ACCESS_LOCKED);
    return;
  }
#endif

  /* validate length of download request */
  if (data[1] > (XCP_CTO_PACKET_LEN-2))
  {
    /* requested data length is too long */
    XcpSetCtoError(XCP_ERR_OUT_OF_RANGE);
    return;
  }

  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* set packet length */
  xcpInfo.ctoLen = 1;

  /* end of programming sequence (datasize is 0)? */
  if (data[1] == 0)
  {
    /* call erase/programming cleanup routine */
    if (NvmDone() == BLT_FALSE)
    {
      /* error occurred while finishing up programming */
      XcpSetCtoError(XCP_ERR_GENERIC);
    }
    return;
  }
  /* program the data */
  if (NvmWrite((blt_addr)xcpInfo.mta, data[1], &data[2]) == 0)
  {
    /* error occurred during programming */
    XcpSetCtoError(XCP_ERR_GENERIC);
    return;
  }

  /* post increment the mta */
  xcpInfo.mta += data[1];
} /*** end of XcpCmdProgram ***/


/****************************************************************************************
** NAME:           XcpCmdProgramClear
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the PROGRAM_CLEAR command 
**                 as defined by the protocol.
**
****************************************************************************************/
static void XcpCmdProgramClear(blt_int8u *data)
{
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
  /* check if PGM resource is unlocked */
  if ((xcpInfo.protection & XCP_RES_PGM) == XCP_RES_PGM)
  {
    /* resource is locked. use seed/key sequence to unlock */
    XcpSetCtoError(XCP_ERR_ACCESS_LOCKED);
    return;
  }
#endif

  /* erase the memory */
  if (NvmErase((blt_addr)xcpInfo.mta, *(blt_int32u*)&data[4]) == 0)
  {
    /* error occurred during erasure */
    XcpSetCtoError(XCP_ERR_GENERIC);
    return;
  }

  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* set packet length */
  xcpInfo.ctoLen = 1;
} /*** end of XcpCmdProgramClear ***/


/****************************************************************************************
** NAME:           XcpCmdProgramReset
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the PROGRAM_RESET command 
**                 as defined by the protocol.
**
****************************************************************************************/
static void XcpCmdProgramReset(blt_int8u *data)
{
  /* suppress compiler warning for unused parameter */
  data = data;

#if (XCP_SEED_KEY_PROTECTION_EN == 1)
  /* check if PGM resource is unlocked */
  if ((xcpInfo.protection & XCP_RES_PGM) == XCP_RES_PGM)
  {
    /* resource is locked. use seed/key sequence to unlock */
    XcpSetCtoError(XCP_ERR_ACCESS_LOCKED);
    return;
  }
#endif

  /* reset the ecu. it is okay if the code does not return here after the reset */
  CpuReset();

  /* set packet id to command response packet */
  xcpInfo.ctoData[0] = XCP_PID_RES;

  /* set packet length */
  xcpInfo.ctoLen = 1;
} /*** end of XcpCmdProgramReset ***/


/****************************************************************************************
** NAME:           XcpCmdProgramPrepare
** PARAMETER:      data pointer to a byte buffer with the packet data.
** RETURN VALUE:   none
** DESCRIPTION:    XCP command processor function which handles the PROGRAM_PREPARE 
**                 command as defined by the protocol.
**
****************************************************************************************/
static void XcpCmdProgramPrepare(blt_int8u *data)
{
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
  /* check if PGM resource is unlocked */
  if ((xcpInfo.protection & XCP_RES_PGM) == XCP_RES_PGM)
  {
    /* resource is locked. use seed/key sequence to unlock */
    XcpSetCtoError(XCP_ERR_ACCESS_LOCKED);
    return;
  }
#endif

  /* programming with kernel currently not needed and therefore not supported */
  XcpSetCtoError(XCP_ERR_GENERIC);
  return;
} /*** end of XcpCmdProgramPrepare ***/
#endif /* XCP_RES_PROGRAMMING_EN == 1 */


/******************************** end of xcp.c *****************************************/
