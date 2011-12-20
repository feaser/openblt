/****************************************************************************************
|  Description: XCP 1.0 protocol core header file
|    File Name: xcp.h
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
#ifndef XCP_H
#define XCP_H

/****************************************************************************************
* Configuration
****************************************************************************************/
/* Maximum length of the transport layer's command transmit object packet.
 */
#define XCP_CTO_PACKET_LEN             (BOOT_COM_RX_MAX_DATA)

/* Maximum length of the transport layer's data transmit object packet.
 */
#define XCP_DTO_PACKET_LEN             (BOOT_COM_TX_MAX_DATA)

/* Name in string format that is used to identify the ECU to the XCP master
 * using the GET_ID command.
 */
#define XCP_STATION_ID_STRING          "OpenBLT"

/* XCP byte ordering 
 */
#if (BOOT_CPU_BYTE_ORDER_MOTOROLA > 0)
  #define XCP_MOTOROLA_FORMAT          (0x01)
#else
  #define XCP_MOTOROLA_FORMAT          (0x00)
#endif
  
/* Enable (=1) or disable (=0) support for the calibration resource. This is
 * required when data is written to RAM during the XCP session.
 */
#define XCP_RES_CALIBRATION_EN         (0)

/* Enable (=1) or disable (=0) support for the paging resource. This is
 * required when switching between application specific calibration pages
 * should be supported. In this case the application specific external
 * functions AppCalSetPage and AppCalGetPage must be provided.
 */
#define XCP_RES_PAGING_EN              (0)

/* Enable (=1) or disable (=0) support for the programming resource. This is
 * required when non-volatile memory will be erased or programmed during an
 * XCP session.  In this case the following external hardware specific
 * functions must be provided: HwNvmWriteMemory, HwNvmEraseMemory and
 * HwCpuReset.
 */
#define XCP_RES_PROGRAMMING_EN         (1)

/* Enable (=1) or disable (=0) support for the data acquisition resource. This
 * note that this feature is currently not supported by the XCP driver.
 */
#define XCP_RES_DATA_ACQUISITION_EN    (0)

/* Enable (=1) or disable (=0) support for the data stimulation resource. This
 * note that this feature is currently not supported by the XCP driver.
 */
#define XCP_RES_DATA_STIMULATION_EN    (0)

/* Enable (=1) or disable (=0) support for the seed/key protection feature.
 * If enabled, the XCP master has to perform a GET_SEED/UNLOCK sequence to
 * obtain access to a resource. The protection algorithm is implemented in
 * XcpGetSeed and XcpVerifyKey.
 */
#define XCP_SEED_KEY_PROTECTION_EN     (0)


/****************************************************************************************
* Defines
****************************************************************************************/
/* xcp supported resources */
#define XCP_RES_PGM            (0x10)                 /* ProGraMming                   */
#define XCP_RES_STIM           (0x08)                 /* data STIMulation              */
#define XCP_RES_DAQ            (0x04)                 /* Data AcQuisition              */
#define XCP_RES_CALPAG         (0x01)                 /* CALibration and PAGing        */

/* xcp checksum types */
#define XCP_CS_ADD11           (0x01)                 /* add byte into byte checksum   */
#define XCP_CS_ADD12           (0x02)                 /* add byte into word checksum   */
#define XCP_CS_ADD14           (0x03)                 /* add byte into dword checksum  */
#define XCP_CS_ADD22           (0x04)                 /* add word into word checksum   */
#define XCP_CS_ADD24           (0x05)                 /* add word into dword checksum  */
#define XCP_CS_ADD44           (0x06)                 /* add dword into dword checksum */
#define XCP_CS_CRC16           (0x07)                 /* use 16-bit CRC algorithm      */
#define XCP_CS_CRC16CITT       (0x08)                 /* use 16-bit CRC CITT algorithm */
#define XCP_CS_CRC32           (0x09)                 /* use 32-bit CRC algorithm      */
#define XCP_CS_USER            (0xff)                 /* use user defined algorithm    */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void     XcpInit(void);
blt_bool XcpIsConnected(void);
void     XcpPacketTransmitted(void);
void     XcpPacketReceived(blt_int8u *data);


/****************************************************************************************
* Configuration check
****************************************************************************************/
#if (XCP_RES_DATA_ACQUISITION_EN == 1)
#error "XCP.H, Current XCP driver does not support Data AcQuisition resource."
#endif


#if (XCP_RES_DATA_STIMULATION_EN == 1)
#error "XCP.H, Current XCP driver does not support Data StIMulation resource."
#endif


#ifndef XCP_CTO_PACKET_LEN
#error  "XCP.H, Configuration macro XCP_CTO_PACKET_LEN is missing."
#endif

#if (XCP_CTO_PACKET_LEN < 1)
#error  "XCP.H, XCP_CTO_PACKET_LEN must be at least 1."
#endif

#if (XCP_CTO_PACKET_LEN > 256)
#error  "XCP.H, XCP_CTO_PACKET_LEN cannot be larger than 256."
#endif


#ifndef XCP_DTO_PACKET_LEN
#error  "XCP.H, Configuration macro XCP_DTO_PACKET_LEN is missing."
#endif

#if (XCP_DTO_PACKET_LEN < 1)
#error  "XCP.H, XCP_DTO_PACKET_LEN must be at least 1."
#endif

#if (XCP_DTO_PACKET_LEN > 65536)
#error  "XCP.H, XCP_DTO_PACKET_LEN cannot be larger than 65536."
#endif


#ifndef XCP_STATION_ID_STRING
#error  "XCP.H, Configuration macro XCP_STATION_ID_STRING is missing."
#endif


#ifndef XCP_RES_CALIBRATION_EN
#error  "XCP.H, Configuration macro XCP_RES_CALIBRATION_EN is missing."
#endif

#if     (XCP_RES_CALIBRATION_EN < 0) || (XCP_RES_CALIBRATION_EN > 1)
#error  "XCP.H, XCP_RES_CALIBRATION_EN must be 0 or 1."
#endif


#ifndef XCP_RES_PAGING_EN
#error  "XCP.H, Configuration macro XCP_RES_PAGING_EN is missing."
#endif

#if     (XCP_RES_PAGING_EN < 0) || (XCP_RES_PAGING_EN > 1)
#error  "XCP.H, XCP_RES_PAGING_EN must be 0 or 1."
#endif


#ifndef XCP_RES_PROGRAMMING_EN
#error  "XCP.H, Configuration macro XCP_RES_PAGING_EN is missing."
#endif

#if     (XCP_RES_PROGRAMMING_EN < 0) || (XCP_RES_PROGRAMMING_EN > 1)
#error  "XCP.H, XCP_RES_PROGRAMMING_EN must be 0 or 1."
#endif


#ifndef XCP_RES_DATA_ACQUISITION_EN
#error  "XCP.H, Configuration macro XCP_RES_DATA_ACQUISITION_EN is missing."
#endif

#if     (XCP_RES_DATA_ACQUISITION_EN < 0) || (XCP_RES_DATA_ACQUISITION_EN > 1)
#error  "XCP.H, XCP_RES_DATA_ACQUISITION_EN must be 0 or 1."
#endif


#ifndef XCP_RES_DATA_STIMULATION_EN
#error  "XCP.H, Configuration macro XCP_RES_DATA_STIMULATION_EN is missing."
#endif

#if     (XCP_RES_DATA_STIMULATION_EN < 0) || (XCP_RES_DATA_STIMULATION_EN > 1)
#error  "XCP.H, XCP_RES_DATA_STIMULATION_EN must be 0 or 1."
#endif


#ifndef XCP_SEED_KEY_PROTECTION_EN
#error  "XCP.H, Configuration macro XCP_SEED_KEY_PROTECTION_EN is missing."
#endif

#if     (XCP_SEED_KEY_PROTECTION_EN < 0) || (XCP_SEED_KEY_PROTECTION_EN > 1)
#error  "XCP.H, XCP_SEED_KEY_PROTECTION_EN must be 0 or 1."
#endif


#endif /* XCP_H */
/******************************** end of xcp.h *~~~~~***********************************/
