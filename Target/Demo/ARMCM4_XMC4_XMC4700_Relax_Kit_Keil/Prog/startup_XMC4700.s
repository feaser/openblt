;*******************************************************************************
;* @file     startup_XMC4700.s
;* @brief    CMSIS Core Device Startup File for
;*           Infineon XMC4700 Device Series
;* @version  V1.1
;* @date     Jnauary 2021
;*
;* @cond
;*********************************************************************************************************************
;* Copyright (c) 2015-2020, Infineon Technologies AG
;* All rights reserved.                        
;*                                             
;* Boost Software License - Version 1.0 - August 17th, 2003
;* 
;* Permission is hereby granted, free of charge, to any person or organization
;* obtaining a copy of the software and accompanying documentation covered by
;* this license (the "Software") to use, reproduce, display, distribute,
;* execute, and transmit the Software, and to prepare derivative works of the
;* Software, and to permit third-parties to whom the Software is furnished to
;* do so, all subject to the following:
;* 
;* The copyright notices in the Software and this entire statement, including
;* the above license grant, this restriction and the following disclaimer,
;* must be included in all copies of the Software, in whole or in part, and
;* all derivative works of the Software, unless such copies or derivative
;* works are solely in the form of machine-executable object code generated by
;* a source language processor.
;* 
;* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;* FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
;* SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
;* FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
;* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
;* DEALINGS IN THE SOFTWARE.
;*                                                                              
;* To improve the quality of the software, users are encouraged to share 
;* modifications, enhancements or bug fixes with Infineon Technologies AG 
;* at XMCSupport@infineon.com.
;*********************************************************************************************************************
;*
;************************** Version History ************************************
; V1.0, 03 Sep   2015, First version
; V1.1, January  2021, Stack configured in scatter file
;*******************************************************************************
;* @endcond

				IMPORT |Image$$ARM_LIB_STACK$$ZI$$Limit|

                PRESERVE8
                THUMB

                MACRO
                    Entry $Handler
                    DCD   $Handler
                MEND

; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
                
__Vectors       DCD     |Image$$ARM_LIB_STACK$$ZI$$Limit|              ; 0 Top of Stack
                DCD     Reset_Handler             ; 1 Reset Handler
                Entry   NMI_Handler               ; 2 NMI Handler
                Entry   HardFault_Handler         ; 3 Hard Fault Handler
                Entry   MemManage_Handler         ; 4 MPU Fault Handler
                Entry   BusFault_Handler          ; 5 Bus Fault Handler
                Entry   UsageFault_Handler        ; 6 Usage Fault Handler
                DCD     0                         ; 7 Reserved
                DCD     0                         ; 8 Reserved
                DCD     0                         ; 9 Reserved
                DCD     0                         ; 10 Reserved
                Entry   SVC_Handler               ; 11 SVCall Handler
                Entry   DebugMon_Handler          ; 12 Debug Monitor Handler
                DCD     0                         ; 13 Reserved
                Entry   PendSV_Handler            ; 14 PendSV Handler
                Entry   SysTick_Handler           ; 15 SysTick Handler

; Interrupt Handlers for Service Requests (SR) from XMC4700 Peripherals */
                Entry   SCU_0_IRQHandler            ; Handler name for SR SCU_0     
                Entry   ERU0_0_IRQHandler           ; Handler name for SR ERU0_0    
                Entry   ERU0_1_IRQHandler           ; Handler name for SR ERU0_1    
                Entry   ERU0_2_IRQHandler           ; Handler name for SR ERU0_2    
                Entry   ERU0_3_IRQHandler           ; Handler name for SR ERU0_3     
                Entry   ERU1_0_IRQHandler           ; Handler name for SR ERU1_0    
                Entry   ERU1_1_IRQHandler           ; Handler name for SR ERU1_1    
                Entry   ERU1_2_IRQHandler           ; Handler name for SR ERU1_2    
                Entry   ERU1_3_IRQHandler           ; Handler name for SR ERU1_3    
                DCD     0                           ; Not Available                 
                DCD     0                           ; Not Available                 
                DCD     0                           ; Not Available                 
                Entry   PMU0_0_IRQHandler           ; Handler name for SR PMU0_0    
                DCD     0                           ; Not Available                 
                Entry   VADC0_C0_0_IRQHandler       ; Handler name for SR VADC0_C0_0  
                Entry   VADC0_C0_1_IRQHandler       ; Handler name for SR VADC0_C0_1  
                Entry   VADC0_C0_2_IRQHandler       ; Handler name for SR VADC0_C0_1  
                Entry   VADC0_C0_3_IRQHandler       ; Handler name for SR VADC0_C0_3  
                Entry   VADC0_G0_0_IRQHandler       ; Handler name for SR VADC0_G0_0  
                Entry   VADC0_G0_1_IRQHandler       ; Handler name for SR VADC0_G0_1  
                Entry   VADC0_G0_2_IRQHandler       ; Handler name for SR VADC0_G0_2  
                Entry   VADC0_G0_3_IRQHandler       ; Handler name for SR VADC0_G0_3  
                Entry   VADC0_G1_0_IRQHandler       ; Handler name for SR VADC0_G1_0  
                Entry   VADC0_G1_1_IRQHandler       ; Handler name for SR VADC0_G1_1  
                Entry   VADC0_G1_2_IRQHandler       ; Handler name for SR VADC0_G1_2  
                Entry   VADC0_G1_3_IRQHandler       ; Handler name for SR VADC0_G1_3  
                Entry   VADC0_G2_0_IRQHandler       ; Handler name for SR VADC0_G2_0  
                Entry   VADC0_G2_1_IRQHandler       ; Handler name for SR VADC0_G2_1  
                Entry   VADC0_G2_2_IRQHandler       ; Handler name for SR VADC0_G2_2  
                Entry   VADC0_G2_3_IRQHandler       ; Handler name for SR VADC0_G2_3  
                Entry   VADC0_G3_0_IRQHandler       ; Handler name for SR VADC0_G3_0  
                Entry   VADC0_G3_1_IRQHandler       ; Handler name for SR VADC0_G3_1  
                Entry   VADC0_G3_2_IRQHandler       ; Handler name for SR VADC0_G3_2  
                Entry   VADC0_G3_3_IRQHandler       ; Handler name for SR VADC0_G3_3  
                Entry   DSD0_0_IRQHandler           ; Handler name for SR DSD_SRM_0 
                Entry   DSD0_1_IRQHandler           ; Handler name for SR DSD_SRM_1 
                Entry   DSD0_2_IRQHandler           ; Handler name for SR DSD_SRM_2 
                Entry   DSD0_3_IRQHandler           ; Handler name for SR DSD_SRM_3 
                Entry   DSD0_4_IRQHandler           ; Handler name for SR DSD_SRA_0 
                Entry   DSD0_5_IRQHandler           ; Handler name for SR DSD_SRA_1 
                Entry   DSD0_6_IRQHandler           ; Handler name for SR DSD_SRA_2 
                Entry   DSD0_7_IRQHandler           ; Handler name for SR DSD_SRA_3 
                Entry   DAC0_0_IRQHandler           ; Handler name for SR DAC0_0    
                Entry   DAC0_1_IRQHandler           ; Handler name for SR DAC0_1    
                Entry   CCU40_0_IRQHandler          ; Handler name for SR CCU40_0   
                Entry   CCU40_1_IRQHandler          ; Handler name for SR CCU40_1   
                Entry   CCU40_2_IRQHandler          ; Handler name for SR CCU40_2   
                Entry   CCU40_3_IRQHandler          ; Handler name for SR CCU40_3   
                Entry   CCU41_0_IRQHandler          ; Handler name for SR CCU41_0   
                Entry   CCU41_1_IRQHandler          ; Handler name for SR CCU41_1   
                Entry   CCU41_2_IRQHandler          ; Handler name for SR CCU41_2   
                Entry   CCU41_3_IRQHandler          ; Handler name for SR CCU41_3   
                Entry   CCU42_0_IRQHandler          ; Handler name for SR CCU42_0   
                Entry   CCU42_1_IRQHandler          ; Handler name for SR CCU42_1   
                Entry   CCU42_2_IRQHandler          ; Handler name for SR CCU42_2   
                Entry   CCU42_3_IRQHandler          ; Handler name for SR CCU42_3   
                Entry   CCU43_0_IRQHandler          ; Handler name for SR CCU43_0   
                Entry   CCU43_1_IRQHandler          ; Handler name for SR CCU43_1   
                Entry   CCU43_2_IRQHandler          ; Handler name for SR CCU43_2   
                Entry   CCU43_3_IRQHandler          ; Handler name for SR CCU43_3   
                Entry   CCU80_0_IRQHandler          ; Handler name for SR CCU80_0   
                Entry   CCU80_1_IRQHandler          ; Handler name for SR CCU80_1   
                Entry   CCU80_2_IRQHandler          ; Handler name for SR CCU80_2   
                Entry   CCU80_3_IRQHandler          ; Handler name for SR CCU80_3   
                Entry   CCU81_0_IRQHandler          ; Handler name for SR CCU81_0   
                Entry   CCU81_1_IRQHandler          ; Handler name for SR CCU81_1   
                Entry   CCU81_2_IRQHandler          ; Handler name for SR CCU81_2   
                Entry   CCU81_3_IRQHandler          ; Handler name for SR CCU81_3   
                Entry   POSIF0_0_IRQHandler         ; Handler name for SR POSIF0_0  
                Entry   POSIF0_1_IRQHandler         ; Handler name for SR POSIF0_1  
                Entry   POSIF1_0_IRQHandler         ; Handler name for SR POSIF1_0  
                Entry   POSIF1_1_IRQHandler         ; Handler name for SR POSIF1_1  
                DCD     0                           ; Not Available                 
                DCD     0                           ; Not Available                 
                DCD     0                           ; Not Available                 
                DCD     0                           ; Not Available                 
                Entry   CAN0_0_IRQHandler           ; Handler name for SR CAN0_0    
                Entry   CAN0_1_IRQHandler           ; Handler name for SR CAN0_1    
                Entry   CAN0_2_IRQHandler           ; Handler name for SR CAN0_2    
                Entry   CAN0_3_IRQHandler           ; Handler name for SR CAN0_3    
                Entry   CAN0_4_IRQHandler           ; Handler name for SR CAN0_4    
                Entry   CAN0_5_IRQHandler           ; Handler name for SR CAN0_5    
                Entry   CAN0_6_IRQHandler           ; Handler name for SR CAN0_6    
                Entry   CAN0_7_IRQHandler           ; Handler name for SR CAN0_7    
                Entry   USIC0_0_IRQHandler          ; Handler name for SR USIC0_0   
                Entry   USIC0_1_IRQHandler          ; Handler name for SR USIC0_1   
                Entry   USIC0_2_IRQHandler          ; Handler name for SR USIC0_2   
                Entry   USIC0_3_IRQHandler          ; Handler name for SR USIC0_3   
                Entry   USIC0_4_IRQHandler          ; Handler name for SR USIC0_4   
                Entry   USIC0_5_IRQHandler          ; Handler name for SR USIC0_5   
                Entry   USIC1_0_IRQHandler          ; Handler name for SR USIC1_0   
                Entry   USIC1_1_IRQHandler          ; Handler name for SR USIC1_1   
                Entry   USIC1_2_IRQHandler          ; Handler name for SR USIC1_2   
                Entry   USIC1_3_IRQHandler          ; Handler name for SR USIC1_3   
                Entry   USIC1_4_IRQHandler          ; Handler name for SR USIC1_4   
                Entry   USIC1_5_IRQHandler          ; Handler name for SR USIC1_5   
                Entry   USIC2_0_IRQHandler          ; Handler name for SR USIC2_0
                Entry   USIC2_1_IRQHandler          ; Handler name for SR USIC2_1                 
                Entry   USIC2_2_IRQHandler          ; Handler name for SR USIC2_2
                Entry   USIC2_3_IRQHandler          ; Handler name for SR USIC2_3
                Entry   USIC2_4_IRQHandler          ; Handler name for SR USIC2_4
                Entry   USIC2_5_IRQHandler          ; Handler name for SR USIC2_5
                Entry   LEDTS0_0_IRQHandler         ; Handler name for SR LEDTS0_0  
                DCD     0                           ; Not Available                 
                Entry   FCE0_0_IRQHandler           ; Handler name for SR FCE0_0    
                Entry   GPDMA0_0_IRQHandler         ; Handler name for SR GPDMA0_0  
                Entry   SDMMC0_0_IRQHandler         ; Handler name for SR SDMMC0_0                 
                Entry   USB0_0_IRQHandler           ; Handler name for SR USB0_0    
                Entry   ETH0_0_IRQHandler           ; Handler name for SR ETH0_0    
                DCD     0                           ; Not Available                 
                Entry   GPDMA1_0_IRQHandler         ; Handler name for SR GPDMA0_1
                DCD     0                           ; Not Available                 
                DCD     0x55AA11EE                  ; Reserved for OpenBLT checksum
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler           [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     SP, =|Image$$ARM_LIB_STACK$$ZI$$Limit|
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)
             
Default_Handler PROC
                EXPORT  NMI_Handler             [WEAK]
                EXPORT  HardFault_Handler       [WEAK]
                EXPORT  MemManage_Handler       [WEAK]
                EXPORT  BusFault_Handler        [WEAK]
                EXPORT  UsageFault_Handler      [WEAK]
                EXPORT  SVC_Handler             [WEAK]
                EXPORT  DebugMon_Handler        [WEAK]
                EXPORT  PendSV_Handler          [WEAK]
                EXPORT  SysTick_Handler         [WEAK]
                
                EXPORT  SCU_0_IRQHandler        [WEAK]
                EXPORT  ERU0_0_IRQHandler       [WEAK]
                EXPORT  ERU0_1_IRQHandler       [WEAK]
                EXPORT  ERU0_2_IRQHandler       [WEAK]
                EXPORT  ERU0_3_IRQHandler       [WEAK]
                EXPORT  ERU1_0_IRQHandler       [WEAK]
                EXPORT  ERU1_1_IRQHandler       [WEAK]
                EXPORT  ERU1_2_IRQHandler       [WEAK]
                EXPORT  ERU1_3_IRQHandler       [WEAK]
                EXPORT  PMU0_0_IRQHandler       [WEAK]
                EXPORT  VADC0_C0_0_IRQHandler   [WEAK]
                EXPORT  VADC0_C0_1_IRQHandler   [WEAK]
                EXPORT  VADC0_C0_2_IRQHandler   [WEAK]
                EXPORT  VADC0_C0_3_IRQHandler   [WEAK]
                EXPORT  VADC0_G0_0_IRQHandler   [WEAK]
                EXPORT  VADC0_G0_1_IRQHandler   [WEAK]
                EXPORT  VADC0_G0_2_IRQHandler   [WEAK]
                EXPORT  VADC0_G0_3_IRQHandler   [WEAK]
                EXPORT  VADC0_G1_0_IRQHandler   [WEAK]
                EXPORT  VADC0_G1_1_IRQHandler   [WEAK]
                EXPORT  VADC0_G1_2_IRQHandler   [WEAK]
                EXPORT  VADC0_G1_3_IRQHandler   [WEAK]
                EXPORT  VADC0_G2_0_IRQHandler   [WEAK]
                EXPORT  VADC0_G2_1_IRQHandler   [WEAK]
                EXPORT  VADC0_G2_2_IRQHandler   [WEAK]
                EXPORT  VADC0_G2_3_IRQHandler   [WEAK]
                EXPORT  VADC0_G3_0_IRQHandler   [WEAK]
                EXPORT  VADC0_G3_1_IRQHandler   [WEAK]
                EXPORT  VADC0_G3_2_IRQHandler   [WEAK]
                EXPORT  VADC0_G3_3_IRQHandler   [WEAK]
                EXPORT  DSD0_0_IRQHandler       [WEAK]
                EXPORT  DSD0_1_IRQHandler       [WEAK]
                EXPORT  DSD0_2_IRQHandler       [WEAK]
                EXPORT  DSD0_3_IRQHandler       [WEAK]
                EXPORT  DSD0_4_IRQHandler       [WEAK]
                EXPORT  DSD0_5_IRQHandler       [WEAK]
                EXPORT  DSD0_6_IRQHandler       [WEAK]
                EXPORT  DSD0_7_IRQHandler       [WEAK]
                EXPORT  DAC0_0_IRQHandler       [WEAK]
                EXPORT  DAC0_1_IRQHandler       [WEAK]
                EXPORT  CCU40_0_IRQHandler      [WEAK]
                EXPORT  CCU40_1_IRQHandler      [WEAK]
                EXPORT  CCU40_2_IRQHandler      [WEAK]
                EXPORT  CCU40_3_IRQHandler      [WEAK]
                EXPORT  CCU41_0_IRQHandler      [WEAK]
                EXPORT  CCU41_1_IRQHandler      [WEAK]
                EXPORT  CCU41_2_IRQHandler      [WEAK]
                EXPORT  CCU41_3_IRQHandler      [WEAK]
                EXPORT  CCU42_0_IRQHandler      [WEAK]
                EXPORT  CCU42_1_IRQHandler      [WEAK]
                EXPORT  CCU42_2_IRQHandler      [WEAK]
                EXPORT  CCU42_3_IRQHandler      [WEAK]
                EXPORT  CCU43_0_IRQHandler      [WEAK]
                EXPORT  CCU43_1_IRQHandler      [WEAK]
                EXPORT  CCU43_2_IRQHandler      [WEAK]
                EXPORT  CCU43_3_IRQHandler      [WEAK]
                EXPORT  CCU80_0_IRQHandler      [WEAK]
                EXPORT  CCU80_1_IRQHandler      [WEAK]
                EXPORT  CCU80_2_IRQHandler      [WEAK]
                EXPORT  CCU80_3_IRQHandler      [WEAK]
                EXPORT  CCU81_0_IRQHandler      [WEAK]
                EXPORT  CCU81_1_IRQHandler      [WEAK]
                EXPORT  CCU81_2_IRQHandler      [WEAK]
                EXPORT  CCU81_3_IRQHandler      [WEAK]
                EXPORT  POSIF0_0_IRQHandler     [WEAK]
                EXPORT  POSIF0_1_IRQHandler     [WEAK]
                EXPORT  POSIF1_0_IRQHandler     [WEAK]
                EXPORT  POSIF1_1_IRQHandler     [WEAK]
                EXPORT  CAN0_0_IRQHandler       [WEAK]
                EXPORT  CAN0_1_IRQHandler       [WEAK]
                EXPORT  CAN0_2_IRQHandler       [WEAK]
                EXPORT  CAN0_3_IRQHandler       [WEAK]
                EXPORT  CAN0_4_IRQHandler       [WEAK]
                EXPORT  CAN0_5_IRQHandler       [WEAK]
                EXPORT  CAN0_6_IRQHandler       [WEAK]
                EXPORT  CAN0_7_IRQHandler       [WEAK]
                EXPORT  USIC0_0_IRQHandler      [WEAK]
                EXPORT  USIC0_1_IRQHandler      [WEAK]
                EXPORT  USIC0_2_IRQHandler      [WEAK]
                EXPORT  USIC0_3_IRQHandler      [WEAK]
                EXPORT  USIC0_4_IRQHandler      [WEAK]
                EXPORT  USIC0_5_IRQHandler      [WEAK]
                EXPORT  USIC1_0_IRQHandler      [WEAK]
                EXPORT  USIC1_1_IRQHandler      [WEAK]
                EXPORT  USIC1_2_IRQHandler      [WEAK]
                EXPORT  USIC1_3_IRQHandler      [WEAK]
                EXPORT  USIC1_4_IRQHandler      [WEAK]
                EXPORT  USIC1_5_IRQHandler      [WEAK]
                EXPORT  USIC2_0_IRQHandler      [WEAK]
                EXPORT  USIC2_1_IRQHandler      [WEAK]
                EXPORT  USIC2_2_IRQHandler      [WEAK]
                EXPORT  USIC2_3_IRQHandler      [WEAK]
                EXPORT  USIC2_4_IRQHandler      [WEAK]
                EXPORT  USIC2_5_IRQHandler      [WEAK]
                EXPORT  LEDTS0_0_IRQHandler     [WEAK]
                EXPORT  FCE0_0_IRQHandler       [WEAK]
                EXPORT  GPDMA0_0_IRQHandler     [WEAK]
                EXPORT  SDMMC0_0_IRQHandler     [WEAK]
                EXPORT  USB0_0_IRQHandler       [WEAK]
                EXPORT  ETH0_0_IRQHandler       [WEAK]
                EXPORT  GPDMA1_0_IRQHandler     [WEAK]

NMI_Handler
HardFault_Handler
MemManage_Handler
BusFault_Handler
UsageFault_Handler
SVC_Handler
DebugMon_Handler
PendSV_Handler
SysTick_Handler                
SCU_0_IRQHandler
ERU0_0_IRQHandler 
ERU0_1_IRQHandler 
ERU0_2_IRQHandler 
ERU0_3_IRQHandler 
ERU1_0_IRQHandler 
ERU1_1_IRQHandler 
ERU1_2_IRQHandler 
ERU1_3_IRQHandler 
PMU0_0_IRQHandler 
VADC0_C0_0_IRQHandler 
VADC0_C0_1_IRQHandler 
VADC0_C0_2_IRQHandler 
VADC0_C0_3_IRQHandler 
VADC0_G0_0_IRQHandler 
VADC0_G0_1_IRQHandler 
VADC0_G0_2_IRQHandler 
VADC0_G0_3_IRQHandler 
VADC0_G1_0_IRQHandler 
VADC0_G1_1_IRQHandler 
VADC0_G1_2_IRQHandler 
VADC0_G1_3_IRQHandler 
VADC0_G2_0_IRQHandler 
VADC0_G2_1_IRQHandler 
VADC0_G2_2_IRQHandler 
VADC0_G2_3_IRQHandler 
VADC0_G3_0_IRQHandler 
VADC0_G3_1_IRQHandler 
VADC0_G3_2_IRQHandler 
VADC0_G3_3_IRQHandler 
DSD0_0_IRQHandler 
DSD0_1_IRQHandler 
DSD0_2_IRQHandler 
DSD0_3_IRQHandler 
DSD0_4_IRQHandler 
DSD0_5_IRQHandler 
DSD0_6_IRQHandler 
DSD0_7_IRQHandler 
DAC0_0_IRQHandler 
DAC0_1_IRQHandler 
CCU40_0_IRQHandler
CCU40_1_IRQHandler
CCU40_2_IRQHandler
CCU40_3_IRQHandler
CCU41_0_IRQHandler
CCU41_1_IRQHandler
CCU41_2_IRQHandler
CCU41_3_IRQHandler
CCU42_0_IRQHandler
CCU42_1_IRQHandler
CCU42_2_IRQHandler
CCU42_3_IRQHandler
CCU43_0_IRQHandler
CCU43_1_IRQHandler
CCU43_2_IRQHandler
CCU43_3_IRQHandler
CCU80_0_IRQHandler
CCU80_1_IRQHandler
CCU80_2_IRQHandler
CCU80_3_IRQHandler
CCU81_0_IRQHandler
CCU81_1_IRQHandler
CCU81_2_IRQHandler
CCU81_3_IRQHandler
POSIF0_0_IRQHandler 
POSIF0_1_IRQHandler 
POSIF1_0_IRQHandler 
POSIF1_1_IRQHandler 
CAN0_0_IRQHandler 
CAN0_1_IRQHandler 
CAN0_2_IRQHandler 
CAN0_3_IRQHandler 
CAN0_4_IRQHandler 
CAN0_5_IRQHandler 
CAN0_6_IRQHandler 
CAN0_7_IRQHandler 
USIC0_0_IRQHandler
USIC0_1_IRQHandler
USIC0_2_IRQHandler
USIC0_3_IRQHandler
USIC0_4_IRQHandler
USIC0_5_IRQHandler
USIC1_0_IRQHandler
USIC1_1_IRQHandler
USIC1_2_IRQHandler
USIC1_3_IRQHandler
USIC1_4_IRQHandler
USIC1_5_IRQHandler
USIC2_0_IRQHandler
USIC2_1_IRQHandler
USIC2_2_IRQHandler
USIC2_3_IRQHandler
USIC2_4_IRQHandler
USIC2_5_IRQHandler
LEDTS0_0_IRQHandler 
FCE0_0_IRQHandler 
GPDMA0_0_IRQHandler
SDMMC0_0_IRQHandler
USB0_0_IRQHandler 
ETH0_0_IRQHandler 
GPDMA1_0_IRQHandler 

                B       .

                ENDP
                
                ALIGN

                END
