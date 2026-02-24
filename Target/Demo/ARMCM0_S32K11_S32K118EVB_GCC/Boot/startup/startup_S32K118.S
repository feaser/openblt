/* ---------------------------------------------------------------------------------------*/
/*  @file:    startup_S32K118.s                                                           */
/*  @purpose: GNU Compiler Collection Startup File                                        */
/*            S32K118                                                                     */
/*  @version: 1.0                                                                         */
/*  @date:    2018-1-22                                                                   */
/*  @build:   b170107                                                                     */
/* ---------------------------------------------------------------------------------------*/
/*                                                                                        */
/*****************************************************************************
 * Copyright 2018-2021, 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/
/*****************************************************************************/
/* Version: GNU Compiler Collection                                          */
/*****************************************************************************/
    .syntax unified
    .arch armv6-m

    .section .isr_vector, "a"
    .align 2
    .globl __isr_vector
__isr_vector:
    .long   __StackTop                            /* Top of Stack */
    .long   Reset_Handler                         /* Reset Handler */
    .long   NMI_Handler                        /* Non Maskable Interrupt */
    .long   HardFault_Handler                  /* Cortex-M0 SV Hard Fault Interrupt */
    .long   0
    .long   0
    .long   0
    .long   0
    .long   0
    .long   0
    .long   0
    .long   SVC_Handler                        /* Cortex-M0 SV Call Interrupt */
    .long   0
    .long   0
    .long   PendSV_Handler                     /* Cortex-M0 Pend SV Interrupt */
    .long   SysTick_Handler                    /* Cortex-M0 System Tick Interrupt */


    .long   DMA0_IRQHandler                       /* DMA channel 0 transfer complete */
    .long   DMA1_IRQHandler                       /* DMA channel 1 transfer complete */
    .long   DMA2_IRQHandler                       /* DMA channel 2 transfer complete */
    .long   DMA3_IRQHandler                       /* DMA channel 3 transfer complete */
    .long   DMA_Error_IRQHandler                  /* DMA error interrupt channels 0-3 */
    .long   ERM_fault_IRQHandler                  /* ERM single and double bit error correction */
    .long   RTC_IRQHandler                        /* RTC alarm interrupt */
    .long   RTC_Seconds_IRQHandler                /* RTC seconds interrupt */
    .long   LPTMR0_IRQHandler                     /* LPTIMER interrupt request */
    .long   PORT_IRQHandler                       /* Port A, B, C, D and E pin detect interrupt */
    .long   CAN0_ORed_Err_Wakeup_IRQHandler       /* OR’ed [Bus Off OR Bus Off Done OR Transmit Warning OR Receive Warning], Interrupt indicating that errors were detected on the CAN bus, Interrupt asserted when Pretended Networking operation is enabled, and a valid message matches the selected filter criteria during Low Power mode */
    .long   CAN0_ORed_0_31_MB_IRQHandler          /* OR’ed Message buffer (0-15, 16-31) */
    .long   FTM0_Ch0_7_IRQHandler                 /* FTM0 Channel 0 to 7 interrupt */
    .long   FTM0_Fault_IRQHandler                 /* FTM0 Fault interrupt */
    .long   FTM0_Ovf_Reload_IRQHandler            /* FTM0 Counter overflow and Reload interrupt */
    .long   FTM1_Ch0_7_IRQHandler                 /* FTM1 Channel 0 to 7 interrupt */
    .long   FTM1_Fault_IRQHandler                 /* FTM1 Fault interrupt */
    .long   FTM1_Ovf_Reload_IRQHandler            /* FTM1 Counter overflow and Reload interrupt */
    .long   FTFC_IRQHandler                       /* FTFC Command complete, Read collision and Double bit fault detect */
    .long   PDB0_IRQHandler                       /* PDB0 interrupt */
    .long   LPIT0_IRQHandler                      /* LPIT interrupt */
    .long   SCG_CMU_LVD_LVWSCG_IRQHandler         /* PMC Low voltage detect interrupt, SCG bus interrupt request and CMU loss of range interrupt */
    .long   WDOG_IRQHandler                       /* WDOG interrupt request out before wdg reset out */
    .long   RCM_IRQHandler                        /* RCM Asynchronous Interrupt */
    .long   LPI2C0_Master_Slave_IRQHandler        /* LPI2C0 Master Interrupt and Slave Interrupt */
    .long   FLEXIO_IRQHandler                     /* FlexIO Interrupt */
    .long   LPSPI0_IRQHandler                     /* LPSPI0 Interrupt */
    .long   LPSPI1_IRQHandler                     /* LPSPI1 Interrupt */
    .long   ADC0_IRQHandler                       /* ADC0 interrupt request. */
    .long   CMP0_IRQHandler                       /* CMP0 interrupt request */
    .long   LPUART1_RxTx_IRQHandler               /* LPUART1 Transmit / Receive  Interrupt */
    .long   LPUART0_RxTx_IRQHandler               /* LPUART0 Transmit / Receive Interrupt */

    .size    __isr_vector, . - __isr_vector

/* Flash Configuration */
    .section .FlashConfig, "a"
    .long 0xFFFFFFFF     /* 8 bytes backdoor comparison key           */
    .long 0xFFFFFFFF     /*                                           */
    .long 0xFFFFFFFF     /* 4 bytes program flash protection bytes    */
    .long 0xFFFF7FFE     /* FDPROT:FEPROT:FOPT:FSEC(0xFE = unsecured) */

    .text
    .thumb

/* Reset Handler */

    .thumb_func
    .align 2
    .globl   Reset_Handler
    .weak    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
    cpsid   i               /* Mask interrupts */

    /* Init the rest of the registers */
    ldr     r1,=0
    ldr     r2,=0
    ldr     r3,=0
    ldr     r4,=0
    ldr     r5,=0
    ldr     r6,=0
    ldr     r7,=0
    mov     r8,r7
    mov     r9,r7
    mov     r10,r7
    mov     r11,r7
    mov     r12,r7

#ifdef START_FROM_FLASH

    /* Init ECC RAM */

    ldr r1, =__RAM_START
    ldr r2, =__RAM_END

    subs    r2, r1
    subs    r2, #1
    ble .LC5

    movs    r0, 0
    movs    r3, #4
.LC4:
    str r0, [r1]
    add	r1, r1, r3
    subs r2, 4
    bge .LC4
.LC5:
#endif

    /* Initialize the stack pointer */
    ldr     r0,=__StackTop
    mov     r13,r0

#ifndef __NO_SYSTEM_INIT
    /* Call the system init routine */
    ldr     r0,=SystemInit
    blx     r0
#endif

    /* Init .data and .bss sections */
    ldr     r0,=init_data_bss
    blx     r0
    cpsie   i               /* Unmask interrupts */

#ifndef __START
#ifdef __EWL__
#define __START  __thumb_startup
#else
#define __START _start
#endif
#endif
	bl	__START
    
JumpToSelf:
    b       JumpToSelf

    .pool
    .size Reset_Handler, . - Reset_Handler

    .align  1
    .thumb_func
    .weak DefaultISR
    .type DefaultISR, %function
DefaultISR:
    b       DefaultISR
    .size DefaultISR, . - DefaultISR

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro def_irq_handler  handler_name
    .weak \handler_name
    .set  \handler_name, DefaultISR
    .endm

/* Exception Handlers */
    def_irq_handler    NMI_Handler
    def_irq_handler    HardFault_Handler
    def_irq_handler    SVC_Handler
    def_irq_handler    PendSV_Handler
    def_irq_handler    SysTick_Handler
    def_irq_handler    DMA0_IRQHandler
    def_irq_handler    DMA1_IRQHandler
    def_irq_handler    DMA2_IRQHandler
    def_irq_handler    DMA3_IRQHandler
    def_irq_handler    DMA_Error_IRQHandler
    def_irq_handler    ERM_fault_IRQHandler
    def_irq_handler    RTC_IRQHandler
    def_irq_handler    RTC_Seconds_IRQHandler
    def_irq_handler    LPTMR0_IRQHandler
    def_irq_handler    PORT_IRQHandler
    def_irq_handler    CAN0_ORed_Err_Wakeup_IRQHandler
    def_irq_handler    CAN0_ORed_0_31_MB_IRQHandler
    def_irq_handler    FTM0_Ch0_7_IRQHandler
    def_irq_handler    FTM0_Fault_IRQHandler
    def_irq_handler    FTM0_Ovf_Reload_IRQHandler
    def_irq_handler    FTM1_Ch0_7_IRQHandler
    def_irq_handler    FTM1_Fault_IRQHandler
    def_irq_handler    FTM1_Ovf_Reload_IRQHandler
    def_irq_handler    FTFC_IRQHandler
    def_irq_handler    PDB0_IRQHandler
    def_irq_handler    LPIT0_IRQHandler
    def_irq_handler    SCG_CMU_LVD_LVWSCG_IRQHandler
    def_irq_handler    WDOG_IRQHandler
    def_irq_handler    RCM_IRQHandler
    def_irq_handler    LPI2C0_Master_Slave_IRQHandler
    def_irq_handler    FLEXIO_IRQHandler
    def_irq_handler    LPSPI0_IRQHandler
    def_irq_handler    LPSPI1_IRQHandler
    def_irq_handler    ADC0_IRQHandler
    def_irq_handler    CMP0_IRQHandler
    def_irq_handler    LPUART1_RxTx_IRQHandler
    def_irq_handler    LPUART0_RxTx_IRQHandler

    .end
