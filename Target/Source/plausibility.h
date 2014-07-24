/************************************************************************************//**
* \file         Source\plausibility.h
* \brief        Bootloader plausibility check header file, for checking the configuration
*               at compile time.
* \ingroup      Core
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option) any later
* version.
*
* OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with OpenBLT.
* If not, see <http://www.gnu.org/licenses/>.
*
* A special exception to the GPL is included to allow you to distribute a combined work 
* that includes OpenBLT without being obliged to provide the source code for any 
* proprietary components. The exception text is included at the bottom of the license
* file <license.html>.
* 
* \endinternal
****************************************************************************************/
#ifndef PLAUSIBILITY_H
#define PLAUSIBILITY_H

/****************************************************************************************
*   C P U   D R I V E R   C O N F I G U R A T I O N   C H E C K
****************************************************************************************/
#ifndef BOOT_CPU_XTAL_SPEED_KHZ
#error "BOOT_CPU_XTAL_SPEED_KHZ is missing in blt_conf.h"
#endif

#if (BOOT_CPU_XTAL_SPEED_KHZ <= 0)
#error "BOOT_CPU_XTAL_SPEED_KHZ must be > 0"
#endif

#ifndef BOOT_CPU_SYSTEM_SPEED_KHZ
#error "BOOT_CPU_SYSTEM_SPEED_KHZ is missing in blt_conf.h"
#endif

#if (BOOT_CPU_SYSTEM_SPEED_KHZ <= 0)
#error "BOOT_CPU_SYSTEM_SPEED_KHZ must be > 0"
#endif

#ifndef BOOT_CPU_BYTE_ORDER_MOTOROLA
#error "BOOT_CPU_BYTE_ORDER_MOTOROLA is missing in blt_conf.h"
#endif

#if (BOOT_CPU_BYTE_ORDER_MOTOROLA < 0) || (BOOT_CPU_BYTE_ORDER_MOTOROLA > 1)
#error "BOOT_CPU_BYTE_ORDER_MOTOROLA must be 0 or 1"
#endif

#ifndef BOOT_CPU_USER_PROGRAM_START_HOOK
#define BOOT_CPU_USER_PROGRAM_START_HOOK   (0)
#endif

#if (BOOT_CPU_USER_PROGRAM_START_HOOK < 0) || (BOOT_CPU_USER_PROGRAM_START_HOOK > 1)
#error "BOOT_CPU_USER_PROGRAM_START_HOOK must be 0 or 1"
#endif


/****************************************************************************************
*   C O M M U N I C A T I O N   I N T E R F A C E   C O N F I G U R A T I O N   C H E C K
****************************************************************************************/
#ifndef BOOT_COM_CAN_TX_MAX_DATA
#define BOOT_COM_CAN_TX_MAX_DATA       (0)
#endif

#ifndef BOOT_COM_CAN_RX_MAX_DATA
#define BOOT_COM_CAN_RX_MAX_DATA       (0)
#endif

#ifndef BOOT_COM_UART_TX_MAX_DATA
#define BOOT_COM_UART_TX_MAX_DATA      (0)
#endif

#ifndef BOOT_COM_UART_RX_MAX_DATA
#define BOOT_COM_UART_RX_MAX_DATA      (0)
#endif

#ifndef BOOT_COM_USB_TX_MAX_DATA
#define BOOT_COM_USB_TX_MAX_DATA       (0)
#endif

#ifndef BOOT_COM_USB_RX_MAX_DATA
#define BOOT_COM_USB_RX_MAX_DATA       (0)
#endif

#ifndef BOOT_COM_NET_TX_MAX_DATA
#define BOOT_COM_NET_TX_MAX_DATA       (0)
#endif

#ifndef BOOT_COM_NET_RX_MAX_DATA
#define BOOT_COM_NET_RX_MAX_DATA       (0)
#endif


#ifndef BOOT_COM_CAN_ENABLE
#define BOOT_COM_CAN_ENABLE             (0)
#endif

#if (BOOT_COM_CAN_ENABLE > 0)
  #ifndef BOOT_COM_CAN_BAUDRATE
  #error "BOOT_COM_CAN_BAUDRATE is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_CAN_BAUDRATE <= 0)
  #error "BOOT_COM_CAN_BAUDRATE must be > 0"
  #endif

  #ifndef BOOT_COM_CAN_TX_MSG_ID
  #error "BOOT_COM_CAN_TX_MSG_ID is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_CAN_TX_MSG_ID <= 0)
  #error "BOOT_COM_CAN_TX_MSG_ID must be > 0"
  #endif

  #ifndef BOOT_COM_CAN_TX_MAX_DATA
  #error "BOOT_COM_CAN_TX_MAX_DATA is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_CAN_TX_MAX_DATA <= 0)
  #error "BOOT_COM_CAN_TX_MAX_DATA must be > 0"
  #endif
  
  #if (BOOT_COM_CAN_TX_MAX_DATA > 8)
  #error "BOOT_COM_CAN_TX_MAX_DATA must be <= 8"
  #endif
  
  #ifndef BOOT_COM_CAN_RX_MSG_ID
  #error "BOOT_COM_CAN_RX_MSG_ID is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_CAN_RX_MSG_ID <= 0)
  #error "BOOT_COM_CAN_RX_MSG_ID must be > 0"
  #endif

  #ifndef BOOT_COM_CAN_RX_MAX_DATA
  #error "BOOT_COM_CAN_RX_MAX_DATA is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_CAN_RX_MAX_DATA <= 0)
  #error "BOOT_COM_CAN_RX_MAX_DATA must be > 0"
  #endif

  #if (BOOT_COM_CAN_RX_MAX_DATA > 8)
  #error "BOOT_COM_CAN_RX_MAX_DATA must be <= 8"
  #endif

  #ifndef BOOT_COM_CAN_CHANNEL_INDEX
  #error "BOOT_COM_CAN_CHANNEL_INDEX is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_CAN_CHANNEL_INDEX < 0)
  #error "BOOT_COM_CAN_CHANNEL_INDEX must be >= 0"
  #endif
#endif /* BOOT_COM_CAN_ENABLE > 0 */

#ifndef BOOT_COM_UART_ENABLE
#define BOOT_COM_UART_ENABLE            (0)
#endif

#if (BOOT_COM_UART_ENABLE > 0)
  #ifndef BOOT_COM_UART_BAUDRATE
  #error "BOOT_COM_UART_BAUDRATE is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_UART_BAUDRATE <= 0)
  #error "BOOT_COM_UART_BAUDRATE must be > 0"
  #endif

  #ifndef BOOT_COM_UART_TX_MAX_DATA
  #error "BOOT_COM_UART_TX_MAX_DATA is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_UART_TX_MAX_DATA <= 0)
  #error "BOOT_COM_UART_TX_MAX_DATA must be > 0"
  #endif
  
  #if (BOOT_COM_UART_TX_MAX_DATA > 255)
  #error "BOOT_COM_UART_TX_MAX_DATA must be <= 255"
  #endif
  
  #ifndef BOOT_COM_UART_RX_MAX_DATA
  #error "BOOT_COM_UART_RX_MAX_DATA is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_UART_RX_MAX_DATA <= 0)
  #error "BOOT_COM_UART_RX_MAX_DATA must be > 0"
  #endif

  #if (BOOT_COM_UART_RX_MAX_DATA > 255)
  #error "BOOT_COM_UART_RX_MAX_DATA must be <= 255"
  #endif

  #ifndef BOOT_COM_UART_CHANNEL_INDEX
  #error "BOOT_COM_UART_CHANNEL_INDEX is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_UART_CHANNEL_INDEX < 0)
  #error "BOOT_COM_UART_CHANNEL_INDEX must be >= 0"
  #endif
#endif /* BOOT_COM_UART_ENABLE > 0 */

#ifndef BOOT_COM_USB_ENABLE
#define BOOT_COM_USB_ENABLE             (0)
#endif

#if (BOOT_COM_USB_ENABLE > 0)
  #ifndef BOOT_COM_USB_TX_MAX_DATA
  #error "BOOT_COM_USB_TX_MAX_DATA is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_USB_TX_MAX_DATA <= 0)
  #error "BOOT_COM_USB_TX_MAX_DATA must be > 0"
  #endif
  
  #ifndef BOOT_COM_USB_RX_MAX_DATA
  #error "BOOT_COM_USB_RX_MAX_DATA is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_USB_RX_MAX_DATA <= 0)
  #error "BOOT_COM_USB_RX_MAX_DATA must be > 0"
  #endif
#endif /* BOOT_COM_USB_ENABLE > 0 */



#ifndef BOOT_COM_NET_ENABLE
#define BOOT_COM_NET_ENABLE             (0)
#endif

#if (BOOT_COM_NET_ENABLE > 0)
  #ifndef BOOT_COM_NET_TX_MAX_DATA
  #error "BOOT_COM_NET_TX_MAX_DATA is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_NET_TX_MAX_DATA <= 0)
  #error "BOOT_COM_NET_TX_MAX_DATA must be > 0"
  #endif
  
  #ifndef BOOT_COM_NET_RX_MAX_DATA
  #error "BOOT_COM_NET_RX_MAX_DATA is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_NET_RX_MAX_DATA <= 0)
  #error "BOOT_COM_NET_RX_MAX_DATA must be > 0"
  #endif

  #ifndef BOOT_COM_NET_IPADDR0
  #error "BOOT_COM_NET_IPADDR0 is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_NET_IPADDR0 < 0)
  #error "BOOT_COM_NET_IPADDR0 must be >= 0"
  #endif

  #ifndef BOOT_COM_NET_IPADDR1
  #error "BOOT_COM_NET_IPADDR1 is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_NET_IPADDR1 < 0)
  #error "BOOT_COM_NET_IPADDR1 must be >= 0"
  #endif
  
  #ifndef BOOT_COM_NET_IPADDR2
  #error "BOOT_COM_NET_IPADDR2 is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_NET_IPADDR2 < 0)
  #error "BOOT_COM_NET_IPADDR2 must be >= 0"
  #endif
  
  #ifndef BOOT_COM_NET_IPADDR3
  #error "BOOT_COM_NET_IPADDR3 is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_NET_IPADDR3 < 0)
  #error "BOOT_COM_NET_IPADDR3 must be >= 0"
  #endif
  
  #ifndef BOOT_COM_NET_NETMASK0
  #error "BOOT_COM_NET_NETMASK0 is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_NET_NETMASK0 < 0)
  #error "BOOT_COM_NET_NETMASK0 must be >= 0"
  #endif
  
  #ifndef BOOT_COM_NET_NETMASK1
  #error "BOOT_COM_NET_NETMASK1 is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_NET_NETMASK1 < 0)
  #error "BOOT_COM_NET_NETMASK1 must be >= 0"
  #endif

  #ifndef BOOT_COM_NET_NETMASK2
  #error "BOOT_COM_NET_NETMASK2 is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_NET_NETMASK2 < 0)
  #error "BOOT_COM_NET_NETMASK2 must be >= 0"
  #endif
  
  #ifndef BOOT_COM_NET_NETMASK3
  #error "BOOT_COM_NET_NETMASK3 is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_NET_NETMASK3 < 0)
  #error "BOOT_COM_NET_NETMASK3 must be >= 0"
  #endif

  #ifndef BOOT_COM_NET_GATEWAY0
  #error "BOOT_COM_NET_GATEWAY0 is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_NET_GATEWAY0 < 0)
  #error "BOOT_COM_NET_GATEWAY0 must be >= 0"
  #endif
  
  #ifndef BOOT_COM_NET_GATEWAY1
  #error "BOOT_COM_NET_GATEWAY1 is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_NET_GATEWAY1 < 0)
  #error "BOOT_COM_NET_GATEWAY1 must be >= 0"
  #endif

  #ifndef BOOT_COM_NET_GATEWAY2
  #error "BOOT_COM_NET_GATEWAY2 is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_NET_GATEWAY2 < 0)
  #error "BOOT_COM_NET_GATEWAY2 must be >= 0"
  #endif
  
  #ifndef BOOT_COM_NET_GATEWAY3
  #error "BOOT_COM_NET_GATEWAY3 is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_NET_GATEWAY3 < 0)
  #error "BOOT_COM_NET_GATEWAY3 must be >= 0"
  #endif

  #ifndef BOOT_COM_NET_PORT
  #error "BOOT_COM_NET_PORT is missing in blt_conf.h"
  #endif

  #if (BOOT_COM_NET_PORT < 0)
  #error "BOOT_COM_NET_PORT must be >= 0"
  #endif

  #ifndef BOOT_COM_NET_IPADDR_HOOK_ENABLE
  #define BOOT_COM_NET_IPADDR_HOOK_ENABLE  (0)
  #endif

  #ifndef BOOT_COM_NET_NETMASK_HOOK_ENABLE
  #define BOOT_COM_NET_NETMASK_HOOK_ENABLE (0)
  #endif

  #ifndef BOOT_COM_NET_GATEWAY_HOOK_ENABLE
  #define BOOT_COM_NET_GATEWAY_HOOK_ENABLE (0)
  #endif

#endif /* BOOT_COM_USB_ENABLE > 0 */

#ifndef BOOT_FILE_SYS_ENABLE
#define BOOT_FILE_SYS_ENABLE      (0)
#endif

#if (BOOT_FILE_SYS_ENABLE < 0) || (BOOT_FILE_SYS_ENABLE > 1)
#error "BOOT_FILE_SYS_ENABLE must be 0 or 1"
#endif

#if (BOOT_FILE_SYS_ENABLE > 0)
  #ifndef BOOT_FILE_LOGGING_ENABLE
  #define BOOT_FILE_LOGGING_ENABLE         (0)
  #endif

  #if (BOOT_FILE_LOGGING_ENABLE < 0) || (BOOT_FILE_LOGGING_ENABLE > 1)
  #error "BOOT_FILE_LOGGING_ENABLE must be 0 or 1"
  #endif

  #ifndef BOOT_FILE_ERROR_HOOK_ENABLE
  #define BOOT_FILE_ERROR_HOOK_ENABLE      (0)
  #endif

  #if (BOOT_FILE_ERROR_HOOK_ENABLE < 0) || (BOOT_FILE_ERROR_HOOK_ENABLE > 1)
  #error "BOOT_FILE_ERROR_HOOK_ENABLE must be 0 or 1"
  #endif

  #ifndef BOOT_FILE_STARTED_HOOK_ENABLE
  #define BOOT_FILE_STARTED_HOOK_ENABLE    (0)
  #endif

  #if (BOOT_FILE_STARTED_HOOK_ENABLE < 0) || (BOOT_FILE_STARTED_HOOK_ENABLE > 1)
  #error "BOOT_FILE_STARTED_HOOK_ENABLE must be 0 or 1"
  #endif

  #ifndef BOOT_FILE_COMPLETED_HOOK_ENABLE
  #define BOOT_FILE_COMPLETED_HOOK_ENABLE  (0)
  #endif

  #if (BOOT_FILE_COMPLETED_HOOK_ENABLE < 0) || (BOOT_FILE_COMPLETED_HOOK_ENABLE > 1)
  #error "BOOT_FILE_COMPLETED_HOOK_ENABLE must be 0 or 1"
  #endif
#endif /* BOOT_FILE_SYS_ENABLE > 0 */

#if (BOOT_COM_CAN_ENABLE == 1) || (BOOT_COM_UART_ENABLE == 1) || (BOOT_COM_NET_ENABLE == 1) || (BOOT_COM_USB_ENABLE == 1)
#define BOOT_COM_ENABLE   (1)
#else
#define BOOT_COM_ENABLE   (0)
#endif        
        

/****************************************************************************************
*   B A C K D O O R   E N T R Y   C H E C K
****************************************************************************************/
#ifndef BOOT_BACKDOOR_HOOKS_ENABLE
#define BOOT_BACKDOOR_HOOKS_ENABLE      (0)
#endif

#if (BOOT_BACKDOOR_HOOKS_ENABLE < 0) || (BOOT_BACKDOOR_HOOKS_ENABLE > 1)
#error "BOOT_BACKDOOR_HOOKS_ENABLE must be 0 or 1"
#endif


/****************************************************************************************
*   N V M   D R I V E R   C O N F I G U R A T I O N   C H E C K
****************************************************************************************/
#ifndef BOOT_NVM_HOOKS_ENABLE
#define BOOT_NVM_HOOKS_ENABLE           (0)
#endif

#if (BOOT_NVM_HOOKS_ENABLE < 0) || (BOOT_NVM_HOOKS_ENABLE > 1)
#error "BOOT_NVM_HOOKS_ENABLE must be 0 or 1"
#endif

#ifndef BOOT_NVM_SIZE_KB
#error "BOOT_NVM_SIZE_KB is missing in blt_conf.h"
#endif

#if (BOOT_NVM_SIZE_KB <= 0)
#error "BOOT_NVM_SIZE_KB must be > 0"
#endif

#ifndef BOOT_NVM_CHECKSUM_HOOKS_ENABLE
#define BOOT_NVM_CHECKSUM_HOOKS_ENABLE  (0)
#endif

#if (BOOT_NVM_CHECKSUM_HOOKS_ENABLE < 0) || (BOOT_NVM_CHECKSUM_HOOKS_ENABLE > 1)
#error "BOOT_NVM_CHECKSUM_HOOKS_ENABLE must be 0 or 1"
#endif


/****************************************************************************************
*   W A T C H D O G   D R I V E R   C O N F I G U R A T I O N   C H E C K
****************************************************************************************/
#ifndef BOOT_COP_HOOKS_ENABLE
#define BOOT_COP_HOOKS_ENABLE           (0)
#endif

#if (BOOT_COP_HOOKS_ENABLE < 0) || (BOOT_COP_HOOKS_ENABLE > 1)
#error "BOOT_COP_HOOKS_ENABLE must be 0 or 1"
#endif


/****************************************************************************************
*   S E E D / K E Y   S E C U R I T Y   C O N F I G U R A T I O N
****************************************************************************************/
#ifndef BOOT_XCP_SEED_KEY_ENABLE
#define BOOT_XCP_SEED_KEY_ENABLE        (0)
#endif

#if (BOOT_XCP_SEED_KEY_ENABLE < 0) || (BOOT_XCP_SEED_KEY_ENABLE > 1)
#error "BOOT_XCP_SEED_KEY_ENABLE must be 0 or 1"
#endif


#endif /* PLAUSIBILITY_H */
/*********************************** end of plausibility.h *****************************/
