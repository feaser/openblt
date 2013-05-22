/****************************************************************************************
|  Description: bootloader plausibility check header file
|    File Name: plausibility.h
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
#ifndef PLAUSIBILITY_H
#define PLAUSIBILITY_H

/****************************************************************************************
*   C P U   D R I V E R   C O N F I G U R A T I O N   C H E C K
****************************************************************************************/
#ifndef BOOT_CPU_XTAL_SPEED_KHZ
#error "BOOT_CPU_XTAL_SPEED_KHZ is missing in config.h"
#endif

#if (BOOT_CPU_XTAL_SPEED_KHZ <= 0)
#error "BOOT_CPU_XTAL_SPEED_KHZ must be > 0"
#endif

#ifndef BOOT_CPU_SYSTEM_SPEED_KHZ
#error "BOOT_CPU_SYSTEM_SPEED_KHZ is missing in config.h"
#endif

#if (BOOT_CPU_SYSTEM_SPEED_KHZ <= 0)
#error "BOOT_CPU_SYSTEM_SPEED_KHZ must be > 0"
#endif

#ifndef BOOT_CPU_BYTE_ORDER_MOTOROLA
#error "BOOT_CPU_BYTE_ORDER_MOTOROLA is missing in config.h"
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
#ifndef BOOT_COM_CAN_ENABLE
#define BOOT_COM_CAN_ENABLE             (0)
#endif

#if (BOOT_COM_CAN_ENABLE > 0)
  #ifndef BOOT_COM_CAN_BAUDRATE
  #error "BOOT_COM_CAN_BAUDRATE is missing in config.h"
  #endif

  #if (BOOT_COM_CAN_BAUDRATE <= 0)
  #error "BOOT_COM_CAN_BAUDRATE must be > 0"
  #endif

  #ifndef BOOT_COM_CAN_TX_MSG_ID
  #error "BOOT_COM_CAN_TX_MSG_ID is missing in config.h"
  #endif

  #if (BOOT_COM_CAN_TX_MSG_ID <= 0)
  #error "BOOT_COM_CAN_TX_MSG_ID must be > 0"
  #endif

  #ifndef BOOT_COM_CAN_TX_MAX_DATA
  #error "BOOT_COM_CAN_TX_MAX_DATA is missing in config.h"
  #endif

  #if (BOOT_COM_CAN_TX_MAX_DATA <= 0)
  #error "BOOT_COM_CAN_TX_MAX_DATA must be > 0"
  #endif
  
  #ifndef BOOT_COM_CAN_RX_MSG_ID
  #error "BOOT_COM_CAN_RX_MSG_ID is missing in config.h"
  #endif

  #if (BOOT_COM_CAN_RX_MSG_ID <= 0)
  #error "BOOT_COM_CAN_RX_MSG_ID must be > 0"
  #endif

  #ifndef BOOT_COM_CAN_RX_MAX_DATA
  #error "BOOT_COM_CAN_RX_MAX_DATA is missing in config.h"
  #endif

  #if (BOOT_COM_CAN_RX_MAX_DATA <= 0)
  #error "BOOT_COM_CAN_RX_MAX_DATA must be > 0"
  #endif

  #ifndef BOOT_COM_CAN_CHANNEL_INDEX
  #error "BOOT_COM_CAN_CHANNEL_INDEX is missing in config.h"
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
  #error "BOOT_COM_UART_BAUDRATE is missing in config.h"
  #endif

  #if (BOOT_COM_UART_BAUDRATE <= 0)
  #error "BOOT_COM_UART_BAUDRATE must be > 0"
  #endif

  #ifndef BOOT_COM_UART_TX_MAX_DATA
  #error "BOOT_COM_UART_TX_MAX_DATA is missing in config.h"
  #endif

  #if (BOOT_COM_UART_TX_MAX_DATA <= 0)
  #error "BOOT_COM_UART_TX_MAX_DATA must be > 0"
  #endif
  
  #ifndef BOOT_COM_UART_RX_MAX_DATA
  #error "BOOT_COM_UART_RX_MAX_DATA is missing in config.h"
  #endif

  #if (BOOT_COM_UART_RX_MAX_DATA <= 0)
  #error "BOOT_COM_UART_RX_MAX_DATA must be > 0"
  #endif

  #ifndef BOOT_COM_UART_CHANNEL_INDEX
  #error "BOOT_COM_UART_CHANNEL_INDEX is missing in config.h"
  #endif

  #if (BOOT_COM_UART_CHANNEL_INDEX < 0)
  #error "BOOT_COM_UART_CHANNEL_INDEX must be >= 0"
  #endif
#endif /* BOOT_COM_UART_ENABLE > 0 */

#if (BOOT_COM_USB_ENABLE > 0)
  #ifndef BOOT_COM_USB_TX_MAX_DATA
  #error "BOOT_COM_USB_TX_MAX_DATA is missing in config.h"
  #endif

  #if (BOOT_COM_USB_TX_MAX_DATA <= 0)
  #error "BOOT_COM_USB_TX_MAX_DATA must be > 0"
  #endif
  
  #ifndef BOOT_COM_USB_RX_MAX_DATA
  #error "BOOT_COM_USB_RX_MAX_DATA is missing in config.h"
  #endif

  #if (BOOT_COM_USB_RX_MAX_DATA <= 0)
  #error "BOOT_COM_USB_RX_MAX_DATA must be > 0"
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

      
#if (BOOT_FILE_SYS_ENABLE == 0)
  #if (BOOT_COM_CAN_ENABLE == 0)  && \
      (BOOT_COM_UART_ENABLE == 0) && \
      (BOOT_COM_USB_ENABLE == 0)
  #error "If not booting from file system (BOOT_FILE_SYS_ENABLE) then a communication interface must be enabled (BOOT_COM_XXX_ENABLE) in config.h"
  #endif
#endif /* BOOT_FILE_SYS_ENABLE == 0 */
            
#if ((BOOT_COM_CAN_ENABLE + BOOT_COM_UART_ENABLE + BOOT_COM_USB_ENABLE) > 1)
#error "Too many communication interfaces enabled (BOOT_COM_XXX_ENABLE) in config.h"
#endif
        
#if (BOOT_COM_CAN_ENABLE == 1) || (BOOT_COM_UART_ENABLE == 1) || (BOOT_COM_USB_ENABLE == 1)
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
#error "BOOT_NVM_SIZE_KB is missing in config.h"
#endif

#if (BOOT_NVM_SIZE_KB <= 0)
#error "BOOT_NVM_SIZE_KB must be > 0"
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


#endif /* PLAUSIBILITY_H */
/*********************************** end of plausibility.h *****************************/
