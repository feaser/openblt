/****************************************************************************************
|  Description: bootloader communication interface source file
|    File Name: com.c
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
#if (BOOT_COM_CAN_ENABLE > 0)
  #include "can.h"                                    /* can driver module             */
#endif
#if (BOOT_COM_UART_ENABLE > 0)
  #include "uart.h"                                   /* uart driver module            */
#endif
#if (BOOT_COM_USB_ENABLE > 0)
  #include "usb.h"                                    /* usb driver module             */
#endif


#if (BOOT_COM_ENABLE > 0)
/****************************************************************************************
* Local data declarations
****************************************************************************************/
/* This variable holds state information about the bootloader being started by a reset 
 * event or by an activation from a running user program. In the latter case, the COM 
 * interface should proceed after initialization as if it already received a connection 
 * request, which the running user program did before it activated the bootloader. It's
 * volatile because some targets might call functions that access this variable from
 * assembly, which might cause the C compiler to incorrectly remove this variable during
 * an optimization step.
 */

/* after a normal reset event, variable comEntryStateConnect must be set to BLT_FALSE
 * right before function main() is called. After an entry from the user program, the
 * variable must be set to BLT_TRUE right before function main() is called. There are
 * typically two methods to correctly implement this functionality:
 * 
 * (1) Initialize comEntryStateConnect to BLT_FALSE upon declaration. After a regular
 *     reset, the C-startup will then set it when initializing the .data and .bss 
 *     sections. When the bootloader is re-entered from the user program, function
 *     ComSetConnectEntryState() must be called AFTER the .data and .bss sections
 *     are initialized, but BEFORE function main is called.
 * (2) Don't give comEntryStateConnect an initialize value and add a compiler specific 
 *     keyword that results in this variable being skipped during the .data and .bss
 *     section initialization. In this case function ComSetDisconnectEntryState() must
 *     be called before main() when starting from a regular reset. When re-entering from
 *     the user program, function ComSetConnectEntryState() must be called before main().
 *    
 * The compiler specific keyword for method 2 can be defined through preprocessor macro
 * BOOT_CPU_CONNECT_STATE_PREFIX or BOOT_CPU_CONNECT_STATE_POSTFIX.
 */
#if defined (BOOT_CPU_CONNECT_STATE_PREFIX)
BOOT_CPU_CONNECT_STATE_PREFIX static volatile blt_bool comEntryStateConnect;
#elif defined (BOOT_CPU_CONNECT_STATE_POSTFIX)
static volatile blt_bool comEntryStateConnect BOOT_CPU_CONNECT_STATE_POSTFIX;
#else
static volatile blt_bool comEntryStateConnect = BLT_FALSE;
#endif


/****************************************************************************************
** NAME:           ComInit
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Initializes the communication module including the hardware needed for 
**                 the communication.
**
****************************************************************************************/
void ComInit(void)
{
  blt_int8u xcpCtoConnectCmdPacket[2] = { 0xff, 0x00 };

  /* initialize the XCP communication protocol */
  XcpInit();
#if (BOOT_COM_CAN_ENABLE > 0)
  /* initialize the CAN controller */
  CanInit();
#endif
#if (BOOT_COM_UART_ENABLE > 0)
  /* initialize the UART interface */
  UartInit();
#endif
#if (BOOT_COM_USB_ENABLE > 0)
  /* initialize the USB interface */
  UsbInit();
#endif
  /* simulate the reception of a CONNECT command if requested */
  if (comEntryStateConnect == BLT_TRUE)
  {
    XcpPacketReceived(&xcpCtoConnectCmdPacket[0]);
  }
} /*** end of ComInit ***/


/****************************************************************************************
** NAME:           ComTask
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Updates the communication module by checking if new data was received 
**                 and submitting the request to process newly received data.
**
****************************************************************************************/
void ComTask(void)
{
  /* make xcpCtoReqPacket static for runtime efficiency */
#if (BOOT_COM_CAN_ENABLE > 0)
  static unsigned char xcpCtoReqPacket[BOOT_COM_CAN_RX_MAX_DATA];
#endif
#if (BOOT_COM_UART_ENABLE > 0)
  static unsigned char xcpCtoReqPacket[BOOT_COM_UART_RX_MAX_DATA];
#endif
#if (BOOT_COM_USB_ENABLE > 0)
  static unsigned char xcpCtoReqPacket[BOOT_COM_UART_RX_MAX_DATA];
#endif
 
#if (BOOT_COM_CAN_ENABLE > 0)
  if (CanReceivePacket(&xcpCtoReqPacket[0]) == BLT_TRUE)
  {
    /* process packet */
    XcpPacketReceived(&xcpCtoReqPacket[0]);
  }
#endif
#if (BOOT_COM_UART_ENABLE > 0)
  if (UartReceivePacket(&xcpCtoReqPacket[0]) == BLT_TRUE)
  {
    /* process packet */
    XcpPacketReceived(&xcpCtoReqPacket[0]);
  }
#endif
#if (BOOT_COM_USB_ENABLE > 0)
  if (UsbReceivePacket(&xcpCtoReqPacket[0]) == BLT_TRUE)
  {
    /* process packet */
    XcpPacketReceived(&xcpCtoReqPacket[0]);
  }
#endif
} /*** end of ComTask ***/


/****************************************************************************************
** NAME:           ComFree
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Releases the communication module.
**
****************************************************************************************/
void ComFree(void)
{
#if (BOOT_COM_USB_ENABLE > 0)
  /* disconnect the usb device from the usb host */
  UsbFree();
#endif
} /*** end of ComFree ***/


/****************************************************************************************
** NAME:           ComTransmitPacket
** PARAMETER:      data pointer to the byte buffer with packet data.
**                 len  number of data bytes that need to be transmitted.
** RETURN VALUE:   none
** DESCRIPTION:    Transmits the packet using the xcp transport layer.
**
****************************************************************************************/
void ComTransmitPacket(blt_int8u *data, blt_int16u len)
{
#if (BOOT_COM_CAN_ENABLE > 0)
  /* transmit the packet. note that len can only be 8 on CAN, so cast is okay */
  CanTransmitPacket(data, (blt_int8u)len);
#endif
#if (BOOT_COM_UART_ENABLE > 0)
  /* transmit the packet */
  UartTransmitPacket(data, len);
#endif
#if (BOOT_COM_USB_ENABLE > 0)
  /* transmit the packet */
  UsbTransmitPacket(data, len);
#endif

  /* send signal that the packet was transmitted */
  XcpPacketTransmitted();
} /*** end of ComTransmitPacket ***/


/****************************************************************************************
** NAME:           ComSetConnectEntryState
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    This function should be called by the function that is called to 
**                 enter the bootloader from a running user program after the stack 
**                 pointer, data section and bss section are initialized, but before 
**                 function main is called. It stores state information that indicates 
**                 that the COM module must be initialized in a connected state.
**
****************************************************************************************/
void ComSetConnectEntryState(void)
{
  comEntryStateConnect = BLT_TRUE;
} /*** end of ComSetConnectEntryState ***/


/****************************************************************************************
** NAME:           ComSetDisconnectEntryState
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    This function should be called by the reset handler after the stack 
**                 pointer, data section and bss section are initialized, but before 
**                 function main is called. It stores state information that indicates 
**                 that the COM module must be initialized in a disconnected state.
**
****************************************************************************************/
void ComSetDisconnectEntryState(void)
{
  comEntryStateConnect = BLT_FALSE;
} /*** end of ComSetDisconnectEntryState ***/


/****************************************************************************************
** NAME:           ComIsConnectEntryState
** PARAMETER:      none
** RETURN VALUE:   BLT_TRUE is there is a pending connection request, BLT_FALSE otherwise
** DESCRIPTION:    This function checks if there is a pending request for a connection 
**                 upon COM module initialization.
**
****************************************************************************************/
blt_bool ComIsConnectEntryState(void)
{
  return comEntryStateConnect;
} /*** end of ComIsConnectEntryState ***/


/****************************************************************************************
** NAME:           ComIsConnected
** PARAMETER:      none
** RETURN VALUE:   BLT_TRUE when an XCP connection is established, BLT_FALSE otherwise. 
** DESCRIPTION:    This function obtains the XCP connection state.
**
****************************************************************************************/
blt_bool ComIsConnected(void)
{
  return XcpIsConnected();
} /*** end of ComIsConnected ***/

#else
/****************************************************************************************
** NAME:           ComSetConnectEntryState
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    This function should be called by the function that is called to 
**                 enter the bootloader from a running user program after the stack 
**                 pointer, data section and bss section are initialized, but before 
**                 function main is called. It stores state information that indicates 
**                 that the COM module must be initialized in a connected state.
**
****************************************************************************************/
void ComSetConnectEntryState(void)
{
  /* empty placeholder function because this one is called from the c-startup. on some
   * targets, this one is written in assembly that doesn't support precompiler 
   * statements
   */
} /*** end of ComSetConnectEntryState ***/


/****************************************************************************************
** NAME:           ComSetDisconnectEntryState
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    This function should be called by the reset handler after the stack 
**                 pointer, data section and bss section are initialized, but before 
**                 function main is called. It stores state information that indicates 
**                 that the COM module must be initialized in a disconnected state.
**
****************************************************************************************/
void ComSetDisconnectEntryState(void)
{
  /* empty placeholder function because this one is called from the c-startup. on some
   * targets, this one is written in assembly that doesn't support precompiler 
   * statements
   */
} /*** end of ComSetDisconnectEntryState ***/

#endif /* BOOT_COM_ENABLE > 0 */

/*********************************** end of com.c **************************************/
