; ---------------------------------------------------------------------------------------
;  @file:    startup_S32K118.s
;  @purpose: IAR Startup File
;            S32K118
;  @version: 1.0
;  @date:    2018-1-22
;  @build:   b170107
; ---------------------------------------------------------------------------------------
;
; Copyright 2018 NXP
; All rights reserved.
;
; THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
; IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
; OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
; IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
; INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
; SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
; HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
; STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
; IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
; THE POSSIBILITY OF SUCH DAMAGE.
;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:ROOT(2)

        EXTERN  main
        EXTERN  SystemInit
        EXTERN  init_data_bss
        PUBLIC  __vector_table
        PUBLIC  __vector_table_0x1c
        PUBLIC  __Vectors
        PUBLIC  __Vectors_End
        PUBLIC  __Vectors_Size

        DATA

__vector_table
        DCD   sfe(CSTACK)
        DCD   Reset_Handler                         ; Reset Handler
        DCD   NMI_Handler                        ; Non Maskable Interrupt
        DCD   HardFault_Handler                  ; Cortex-M0 SV Hard Fault Interrupt
        DCD   0
        DCD   0
        DCD   0
__vector_table_0x1c
        DCD   0
        DCD   0
        DCD   0
        DCD   0
        DCD   SVC_Handler                        ; Cortex-M0 SV Call Interrupt
        DCD   0
        DCD   0
        DCD   PendSV_Handler                     ; Cortex-M0 Pend SV Interrupt
        DCD   SysTick_Handler                    ; Cortex-M0 System Tick Interrupt


        DCD   DMA0_IRQHandler                       ; DMA channel 0 transfer complete
        DCD   DMA1_IRQHandler                       ; DMA channel 1 transfer complete
        DCD   DMA2_IRQHandler                       ; DMA channel 2 transfer complete
        DCD   DMA3_IRQHandler                       ; DMA channel 3 transfer complete
        DCD   DMA_Error_IRQHandler                  ; DMA error interrupt channels 0-3
        DCD   ERM_fault_IRQHandler                  ; ERM single and double bit error correction
        DCD   RTC_IRQHandler                        ; RTC alarm interrupt
        DCD   RTC_Seconds_IRQHandler                ; RTC seconds interrupt
        DCD   LPTMR0_IRQHandler                     ; LPTIMER interrupt request
        DCD   PORT_IRQHandler                       ; Port A, B, C, D and E pin detect interrupt
        DCD   CAN0_ORed_Err_Wakeup_IRQHandler       ; OR’ed [Bus Off OR Bus Off Done OR Transmit Warning OR Receive Warning], Interrupt indicating that errors were detected on the CAN bus, Interrupt asserted when Pretended Networking operation is enabled, and a valid message matches the selected filter criteria during Low Power mode
        DCD   CAN0_ORed_0_31_MB_IRQHandler          ; OR’ed Message buffer (0-15, 16-31)
        DCD   FTM0_Ch0_7_IRQHandler                 ; FTM0 Channel 0 to 7 interrupt
        DCD   FTM0_Fault_IRQHandler                 ; FTM0 Fault interrupt
        DCD   FTM0_Ovf_Reload_IRQHandler            ; FTM0 Counter overflow and Reload interrupt
        DCD   FTM1_Ch0_7_IRQHandler                 ; FTM1 Channel 0 to 7 interrupt
        DCD   FTM1_Fault_IRQHandler                 ; FTM1 Fault interrupt
        DCD   FTM1_Ovf_Reload_IRQHandler            ; FTM1 Counter overflow and Reload interrupt
        DCD   FTFC_IRQHandler                       ; FTFC Command complete, Read collision and Double bit fault detect
        DCD   PDB0_IRQHandler                       ; PDB0 interrupt
        DCD   LPIT0_IRQHandler                      ; LPIT interrupt
        DCD   SCG_CMU_LVD_LVWSCG_IRQHandler         ; PMC Low voltage detect interrupt, SCG bus interrupt request and CMU loss of range interrupt
        DCD   WDOG_IRQHandler                       ; WDOG interrupt request out before wdg reset out
        DCD   RCM_IRQHandler                        ; RCM Asynchronous Interrupt
        DCD   LPI2C0_Master_Slave_IRQHandler        ; LPI2C0 Master Interrupt and Slave Interrupt
        DCD   FLEXIO_IRQHandler                     ; FlexIO Interrupt
        DCD   LPSPI0_IRQHandler                     ; LPSPI0 Interrupt
        DCD   LPSPI1_IRQHandler                     ; LPSPI1 Interrupt
        DCD   ADC0_IRQHandler                       ; ADC0 interrupt request.
        DCD   CMP0_IRQHandler                       ; CMP0 interrupt request
        DCD   LPUART1_RxTx_IRQHandler               ; LPUART1 Transmit / Receive  Interrupt
        DCD   LPUART0_RxTx_IRQHandler               ; LPUART0 Transmit / Receive Interrupt
__Vectors_End

        SECTION FlashConfig:CODE
__FlashConfig
        DCD 0xFFFFFFFF    ; 8 bytes backdoor comparison key
        DCD 0xFFFFFFFF    ;
        DCD 0xFFFFFFFF    ; 4 bytes program flash protection bytes
        DCD 0xFFFF7FFE    ; FDPROT:FEPROT:FOPT:FSEC(0xFE = unsecured)
__FlashConfig_End

__Vectors       EQU   __vector_table
__Vectors_Size  EQU   __Vectors_End - __Vectors


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
        CPSID   I               ; Mask interrupts
        ;; Init the rest of the registers
        LDR     R1,=0
        LDR     R2,=0
        LDR     R3,=0
        LDR     R4,=0
        LDR     R5,=0
        LDR     R6,=0
        LDR     R7,=0
        MOV     R8,R7
        MOV     R9,R7
        MOV     R10,R7
        MOV     R11,R7
        MOV     R12,R7

#ifdef START_FROM_FLASH
        IMPORT __RAM_START, __RAM_END
        ;; INIT ECC RAM

        LDR R1, =__RAM_START
        LDR R2, =__RAM_END

        SUBS    R2, R2, R1
        SUBS    R2, #1
        BLE .LC5

        MOVS    R0, #0
        MOVS    R3, #4
    .LC4:
        STR R0, [R1]
        ADD	R1, R1, R3
        SUBS R2, #4
        BGE .LC4
    .LC5:
#endif
        ;; Initialize the stack pointer
        LDR     R0, =sfe(CSTACK)
        MOV     R13,R0

#ifndef __NO_SYSTEM_INIT
        ;; Call the CMSIS system init routine
        LDR     R0, =SystemInit
        BLX     R0
#endif

        ;; Init .data and .bss sections
        LDR     R0, =init_data_bss
        BLX     R0
        CPSIE   I               ; Unmask interrupts

        BL      main
JumpToSelf
        B   JumpToSelf

        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B .

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B .


        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B .

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B .

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B .

        PUBWEAK    DMA0_IRQHandler
        PUBWEAK    DMA1_IRQHandler
        PUBWEAK    DMA2_IRQHandler
        PUBWEAK    DMA3_IRQHandler
        PUBWEAK    DMA_Error_IRQHandler
        PUBWEAK    ERM_fault_IRQHandler
        PUBWEAK    RTC_IRQHandler
        PUBWEAK    RTC_Seconds_IRQHandler
        PUBWEAK    LPTMR0_IRQHandler
        PUBWEAK    PORT_IRQHandler
        PUBWEAK    CAN0_ORed_Err_Wakeup_IRQHandler
        PUBWEAK    CAN0_ORed_0_31_MB_IRQHandler
        PUBWEAK    FTM0_Ch0_7_IRQHandler
        PUBWEAK    FTM0_Fault_IRQHandler
        PUBWEAK    FTM0_Ovf_Reload_IRQHandler
        PUBWEAK    FTM1_Ch0_7_IRQHandler
        PUBWEAK    FTM1_Fault_IRQHandler
        PUBWEAK    FTM1_Ovf_Reload_IRQHandler
        PUBWEAK    FTFC_IRQHandler
        PUBWEAK    PDB0_IRQHandler
        PUBWEAK    LPIT0_IRQHandler
        PUBWEAK    SCG_CMU_LVD_LVWSCG_IRQHandler
        PUBWEAK    WDOG_IRQHandler
        PUBWEAK    RCM_IRQHandler
        PUBWEAK    LPI2C0_Master_Slave_IRQHandler
        PUBWEAK    FLEXIO_IRQHandler
        PUBWEAK    LPSPI0_IRQHandler
        PUBWEAK    LPSPI1_IRQHandler
        PUBWEAK    ADC0_IRQHandler
        PUBWEAK    CMP0_IRQHandler
        PUBWEAK    LPUART1_RxTx_IRQHandler
        PUBWEAK    LPUART0_RxTx_IRQHandler
        PUBWEAK DefaultISR
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
HardFault_Handler
SVC_Handler
PendSV_Handler
SysTick_Handler
DMA0_IRQHandler
DMA1_IRQHandler
DMA2_IRQHandler
DMA3_IRQHandler
DMA_Error_IRQHandler
ERM_fault_IRQHandler
RTC_IRQHandler
RTC_Seconds_IRQHandler
LPTMR0_IRQHandler
PORT_IRQHandler
CAN0_ORed_Err_Wakeup_IRQHandler
CAN0_ORed_0_31_MB_IRQHandler
FTM0_Ch0_7_IRQHandler
FTM0_Fault_IRQHandler
FTM0_Ovf_Reload_IRQHandler
FTM1_Ch0_7_IRQHandler
FTM1_Fault_IRQHandler
FTM1_Ovf_Reload_IRQHandler
FTFC_IRQHandler
PDB0_IRQHandler
LPIT0_IRQHandler
SCG_CMU_LVD_LVWSCG_IRQHandler
WDOG_IRQHandler
RCM_IRQHandler
LPI2C0_Master_Slave_IRQHandler
FLEXIO_IRQHandler
LPSPI0_IRQHandler
LPSPI1_IRQHandler
ADC0_IRQHandler
CMP0_IRQHandler
LPUART1_RxTx_IRQHandler
LPUART0_RxTx_IRQHandler
DefaultISR
        B DefaultISR

        END
