/* ---------------------------------------------------------------------------------------*/
/*  @file:    startup_S32K144.s                                                           */
/*  @purpose: GNU Compiler Collection Startup File                                        */
/*            S32K144                                                                     */
/*  @version: 2.0                                                                         */
/*  @date:    2017-1-10                                                                   */
/*  @build:   b170107                                                                     */
/* ---------------------------------------------------------------------------------------*/
/*                                                                                        */
/* Copyright (c) 1997 - 2016 , Freescale Semiconductor, Inc.                              */
/* Copyright 2016-2017 NXP                                                                */
/* All rights reserved.                                                                   */
/*                                                                                        */
/* THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR                          */
/* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES              */
/* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.                */
/* IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,                    */
/* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES                     */
/* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR                     */
/* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)                     */
/* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,                    */
/* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING                  */
/* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF                         */
/* THE POSSIBILITY OF SUCH DAMAGE.                                                        */
/*****************************************************************************/
/* Version: GNU Compiler Collection                                          */
/*****************************************************************************/
    .syntax unified
    .arch armv7-m

    .section .isr_vector, "a"
    .align 2
    .globl __isr_vector
__isr_vector:
    .long   __StackTop                                      /* Top of Stack */
    .long   Reset_Handler                                   /* Reset Handler */
    .long   NMI_Handler                                     /* NMI Handler*/
    .long   HardFault_Handler                               /* Hard Fault Handler*/
    .long   MemManage_Handler                               /* MPU Fault Handler*/
    .long   BusFault_Handler                                /* Bus Fault Handler*/
    .long   UsageFault_Handler                              /* Usage Fault Handler*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   SVC_Handler                                     /* SVCall Handler*/
    .long   DebugMon_Handler                                /* Debug Monitor Handler*/
    .long   0                                               /* Reserved*/
    .long   PendSV_Handler                                  /* PendSV Handler*/
    .long   SysTick_Handler                                 /* SysTick Handler*/

                                                            /* External Interrupts*/
    .long   DMA0_IRQHandler                                 /* DMA channel 0 transfer complete*/
    .long   DMA1_IRQHandler                                 /* DMA channel 1 transfer complete*/
    .long   DMA2_IRQHandler                                 /* DMA channel 2 transfer complete*/
    .long   DMA3_IRQHandler                                 /* DMA channel 3 transfer complete*/
    .long   DMA4_IRQHandler                                 /* DMA channel 4 transfer complete*/
    .long   DMA5_IRQHandler                                 /* DMA channel 5 transfer complete*/
    .long   DMA6_IRQHandler                                 /* DMA channel 6 transfer complete*/
    .long   DMA7_IRQHandler                                 /* DMA channel 7 transfer complete*/
    .long   DMA8_IRQHandler                                 /* DMA channel 8 transfer complete*/
    .long   DMA9_IRQHandler                                 /* DMA channel 9 transfer complete*/
    .long   DMA10_IRQHandler                                /* DMA channel 10 transfer complete*/
    .long   DMA11_IRQHandler                                /* DMA channel 11 transfer complete*/
    .long   DMA12_IRQHandler                                /* DMA channel 12 transfer complete*/
    .long   DMA13_IRQHandler                                /* DMA channel 13 transfer complete*/
    .long   DMA14_IRQHandler                                /* DMA channel 14 transfer complete*/
    .long   DMA15_IRQHandler                                /* DMA channel 15 transfer complete*/
    .long   DMA_Error_IRQHandler                            /* DMA error interrupt channels 0-15*/
    .long   MCM_IRQHandler                                  /* FPU sources*/
    .long   FTFC_IRQHandler                                 /* FTFC Command complete*/
    .long   Read_Collision_IRQHandler                       /* FTFC Read collision*/
    .long   LVD_LVW_IRQHandler                              /* PMC Low voltage detect interrupt*/
    .long   FTFC_Fault_IRQHandler                           /* FTFC Double bit fault detect*/
    .long   WDOG_EWM_IRQHandler                             /* Single interrupt vector for WDOG and EWM*/
    .long   RCM_IRQHandler                                  /* RCM Asynchronous Interrupt*/
    .long   LPI2C0_Master_IRQHandler                        /* LPI2C0 Master Interrupt*/
    .long   LPI2C0_Slave_IRQHandler                         /* LPI2C0 Slave Interrupt*/
    .long   LPSPI0_IRQHandler                               /* LPSPI0 Interrupt*/
    .long   LPSPI1_IRQHandler                               /* LPSPI1 Interrupt*/
    .long   LPSPI2_IRQHandler                               /* LPSPI2 Interrupt*/
    .long   Reserved45_IRQHandler                           /* Reserved Interrupt 45*/
    .long   Reserved46_IRQHandler                           /* Reserved Interrupt 46*/
    .long   LPUART0_RxTx_IRQHandler                         /* LPUART0 Transmit / Receive Interrupt*/
    .long   Reserved48_IRQHandler                           /* Reserved Interrupt 48*/
    .long   LPUART1_RxTx_IRQHandler                         /* LPUART1 Transmit / Receive  Interrupt*/
    .long   Reserved50_IRQHandler                           /* Reserved Interrupt 50*/
    .long   LPUART2_RxTx_IRQHandler                         /* LPUART2 Transmit / Receive  Interrupt*/
    .long   Reserved52_IRQHandler                           /* Reserved Interrupt 52*/
    .long   Reserved53_IRQHandler                           /* Reserved Interrupt 53*/
    .long   Reserved54_IRQHandler                           /* Reserved Interrupt 54*/
    .long   ADC0_IRQHandler                                 /* ADC0 interrupt request.*/
    .long   ADC1_IRQHandler                                 /* ADC1 interrupt request.*/
    .long   CMP0_IRQHandler                                 /* CMP0 interrupt request*/
    .long   Reserved58_IRQHandler                           /* Reserved Interrupt 58*/
    .long   Reserved59_IRQHandler                           /* Reserved Interrupt 59*/
    .long   ERM_single_fault_IRQHandler                     /* ERM single bit error correction*/
    .long   ERM_double_fault_IRQHandler                     /* ERM double bit error non-correctable*/
    .long   RTC_IRQHandler                                  /* RTC alarm interrupt*/
    .long   RTC_Seconds_IRQHandler                          /* RTC seconds interrupt*/
    .long   LPIT0_Ch0_IRQHandler                            /* LPIT0 channel 0 overflow interrupt*/
    .long   LPIT0_Ch1_IRQHandler                            /* LPIT0 channel 1 overflow interrupt*/
    .long   LPIT0_Ch2_IRQHandler                            /* LPIT0 channel 2 overflow interrupt*/
    .long   LPIT0_Ch3_IRQHandler                            /* LPIT0 channel 3 overflow interrupt*/
    .long   PDB0_IRQHandler                                 /* PDB0 interrupt*/
    .long   Reserved69_IRQHandler                           /* Reserved Interrupt 69*/
    .long   Reserved70_IRQHandler                           /* Reserved Interrupt 70*/
    .long   Reserved71_IRQHandler                           /* Reserved Interrupt 71*/
    .long   Reserved72_IRQHandler                           /* Reserved Interrupt 72*/
    .long   SCG_IRQHandler                                  /* SCG bus interrupt request*/
    .long   LPTMR0_IRQHandler                               /* LPTIMER interrupt request*/
    .long   PORTA_IRQHandler                                /* Port A pin detect interrupt*/
    .long   PORTB_IRQHandler                                /* Port B pin detect interrupt*/
    .long   PORTC_IRQHandler                                /* Port C pin detect interrupt*/
    .long   PORTD_IRQHandler                                /* Port D pin detect interrupt*/
    .long   PORTE_IRQHandler                                /* Port E pin detect interrupt*/
    .long   SWI_IRQHandler                                  /* Software interrupt*/
    .long   Reserved81_IRQHandler                           /* Reserved Interrupt 81*/
    .long   Reserved82_IRQHandler                           /* Reserved Interrupt 82*/
    .long   Reserved83_IRQHandler                           /* Reserved Interrupt 83*/
    .long   PDB1_IRQHandler                                 /* PDB1 interrupt*/
    .long   FLEXIO_IRQHandler                               /* FlexIO Interrupt*/
    .long   Reserved86_IRQHandler                           /* Reserved Interrupt 86*/
    .long   Reserved87_IRQHandler                           /* Reserved Interrupt 87*/
    .long   Reserved88_IRQHandler                           /* Reserved Interrupt 88*/
    .long   Reserved89_IRQHandler                           /* Reserved Interrupt 89*/
    .long   Reserved90_IRQHandler                           /* Reserved Interrupt 90*/
    .long   Reserved91_IRQHandler                           /* Reserved Interrupt 91*/
    .long   Reserved92_IRQHandler                           /* Reserved Interrupt 92*/
    .long   Reserved93_IRQHandler                           /* Reserved Interrupt 93*/
    .long   CAN0_ORed_IRQHandler                            /* CAN0 OR'ed [Bus Off OR Transmit Warning OR Receive Warning]*/
    .long   CAN0_Error_IRQHandler                           /* CAN0 Interrupt indicating that errors were detected on the CAN bus*/
    .long   CAN0_Wake_Up_IRQHandler                         /* CAN0 Interrupt asserted when Pretended Networking operation is enabled, and a valid message matches the selected filter criteria during Low Power mode*/
    .long   CAN0_ORed_0_15_MB_IRQHandler                    /* CAN0 OR'ed Message buffer (0-15)*/
    .long   CAN0_ORed_16_31_MB_IRQHandler                   /* CAN0 OR'ed Message buffer (16-31)*/
    .long   Reserved99_IRQHandler                           /* Reserved Interrupt 99*/
    .long   Reserved100_IRQHandler                          /* Reserved Interrupt 100*/
    .long   CAN1_ORed_IRQHandler                            /* CAN1 OR'ed [Bus Off OR Transmit Warning OR Receive Warning]*/
    .long   CAN1_Error_IRQHandler                           /* CAN1 Interrupt indicating that errors were detected on the CAN bus*/
    .long   Reserved103_IRQHandler                          /* Reserved Interrupt 103*/
    .long   CAN1_ORed_0_15_MB_IRQHandler                    /* CAN1 OR'ed Interrupt for Message buffer (0-15)*/
    .long   Reserved105_IRQHandler                          /* Reserved Interrupt 105*/
    .long   Reserved106_IRQHandler                          /* Reserved Interrupt 106*/
    .long   Reserved107_IRQHandler                          /* Reserved Interrupt 107*/
    .long   CAN2_ORed_IRQHandler                            /* CAN2 OR'ed [Bus Off OR Transmit Warning OR Receive Warning]*/
    .long   CAN2_Error_IRQHandler                           /* CAN2 Interrupt indicating that errors were detected on the CAN bus*/
    .long   Reserved110_IRQHandler                          /* Reserved Interrupt 110*/
    .long   CAN2_ORed_0_15_MB_IRQHandler                    /* CAN2 OR'ed Message buffer (0-15)*/
    .long   Reserved112_IRQHandler                          /* Reserved Interrupt 112*/
    .long   Reserved113_IRQHandler                          /* Reserved Interrupt 113*/
    .long   Reserved114_IRQHandler                          /* Reserved Interrupt 114*/
    .long   FTM0_Ch0_Ch1_IRQHandler                         /* FTM0 Channel 0 and 1 interrupt*/
    .long   FTM0_Ch2_Ch3_IRQHandler                         /* FTM0 Channel 2 and 3 interrupt*/
    .long   FTM0_Ch4_Ch5_IRQHandler                         /* FTM0 Channel 4 and 5 interrupt*/
    .long   FTM0_Ch6_Ch7_IRQHandler                         /* FTM0 Channel 6 and 7 interrupt*/
    .long   FTM0_Fault_IRQHandler                           /* FTM0 Fault interrupt*/
    .long   FTM0_Ovf_Reload_IRQHandler                      /* FTM0 Counter overflow and Reload interrupt*/
    .long   FTM1_Ch0_Ch1_IRQHandler                         /* FTM1 Channel 0 and 1 interrupt*/
    .long   FTM1_Ch2_Ch3_IRQHandler                         /* FTM1 Channel 2 and 3 interrupt*/
    .long   FTM1_Ch4_Ch5_IRQHandler                         /* FTM1 Channel 4 and 5 interrupt*/
    .long   FTM1_Ch6_Ch7_IRQHandler                         /* FTM1 Channel 6 and 7 interrupt*/
    .long   FTM1_Fault_IRQHandler                           /* FTM1 Fault interrupt*/
    .long   FTM1_Ovf_Reload_IRQHandler                      /* FTM1 Counter overflow and Reload interrupt*/
    .long   FTM2_Ch0_Ch1_IRQHandler                         /* FTM2 Channel 0 and 1 interrupt*/
    .long   FTM2_Ch2_Ch3_IRQHandler                         /* FTM2 Channel 2 and 3 interrupt*/
    .long   FTM2_Ch4_Ch5_IRQHandler                         /* FTM2 Channel 4 and 5 interrupt*/
    .long   FTM2_Ch6_Ch7_IRQHandler                         /* FTM2 Channel 6 and 7 interrupt*/
    .long   FTM2_Fault_IRQHandler                           /* FTM2 Fault interrupt*/
    .long   FTM2_Ovf_Reload_IRQHandler                      /* FTM2 Counter overflow and Reload interrupt*/
    .long   FTM3_Ch0_Ch1_IRQHandler                         /* FTM3 Channel 0 and 1 interrupt*/
    .long   FTM3_Ch2_Ch3_IRQHandler                         /* FTM3 Channel 2 and 3 interrupt*/
    .long   FTM3_Ch4_Ch5_IRQHandler                         /* FTM3 Channel 4 and 5 interrupt*/
    .long   FTM3_Ch6_Ch7_IRQHandler                         /* FTM3 Channel 6 and 7 interrupt*/
    .long   FTM3_Fault_IRQHandler                           /* FTM3 Fault interrupt*/
    .long   FTM3_Ovf_Reload_IRQHandler                      /* FTM3 Counter overflow and Reload interrupt*/
    .long   DefaultISR                                      /* 139*/
    .long   DefaultISR                                      /* 140*/
    .long   DefaultISR                                      /* 141*/
    .long   DefaultISR                                      /* 142*/
    .long   DefaultISR                                      /* 143*/
    .long   DefaultISR                                      /* 144*/
    .long   DefaultISR                                      /* 145*/
    .long   DefaultISR                                      /* 146*/
    .long   DefaultISR                                      /* 147*/
    .long   DefaultISR                                      /* 148*/
    .long   DefaultISR                                      /* 149*/
    .long   DefaultISR                                      /* 150*/
    .long   DefaultISR                                      /* 151*/
    .long   DefaultISR                                      /* 152*/
    .long   DefaultISR                                      /* 153*/
    .long   DefaultISR                                      /* 154*/
    .long   DefaultISR                                      /* 155*/
    .long   DefaultISR                                      /* 156*/
    .long   DefaultISR                                      /* 157*/
    .long   DefaultISR                                      /* 158*/
    .long   DefaultISR                                      /* 159*/
    .long   DefaultISR                                      /* 160*/
    .long   DefaultISR                                      /* 161*/
    .long   DefaultISR                                      /* 162*/
    .long   DefaultISR                                      /* 163*/
    .long   DefaultISR                                      /* 164*/
    .long   DefaultISR                                      /* 165*/
    .long   DefaultISR                                      /* 166*/
    .long   DefaultISR                                      /* 167*/
    .long   DefaultISR                                      /* 168*/
    .long   DefaultISR                                      /* 169*/
    .long   DefaultISR                                      /* 170*/
    .long   DefaultISR                                      /* 171*/
    .long   DefaultISR                                      /* 172*/
    .long   DefaultISR                                      /* 173*/
    .long   DefaultISR                                      /* 174*/
    .long   DefaultISR                                      /* 175*/
    .long   DefaultISR                                      /* 176*/
    .long   DefaultISR                                      /* 177*/
    .long   DefaultISR                                      /* 178*/
    .long   DefaultISR                                      /* 179*/
    .long   DefaultISR                                      /* 180*/
    .long   DefaultISR                                      /* 181*/
    .long   DefaultISR                                      /* 182*/
    .long   DefaultISR                                      /* 183*/
    .long   DefaultISR                                      /* 184*/
    .long   DefaultISR                                      /* 185*/
    .long   DefaultISR                                      /* 186*/
    .long   DefaultISR                                      /* 187*/
    .long   DefaultISR                                      /* 188*/
    .long   DefaultISR                                      /* 189*/
    .long   DefaultISR                                      /* 190*/
    .long   DefaultISR                                      /* 191*/
    .long   DefaultISR                                      /* 192*/
    .long   DefaultISR                                      /* 193*/
    .long   DefaultISR                                      /* 194*/
    .long   DefaultISR                                      /* 195*/
    .long   DefaultISR                                      /* 196*/
    .long   DefaultISR                                      /* 197*/
    .long   DefaultISR                                      /* 198*/
    .long   DefaultISR                                      /* 199*/
    .long   DefaultISR                                      /* 200*/
    .long   DefaultISR                                      /* 201*/
    .long   DefaultISR                                      /* 202*/
    .long   DefaultISR                                      /* 203*/
    .long   DefaultISR                                      /* 204*/
    .long   DefaultISR                                      /* 205*/
    .long   DefaultISR                                      /* 206*/
    .long   DefaultISR                                      /* 207*/
    .long   DefaultISR                                      /* 208*/
    .long   DefaultISR                                      /* 209*/
    .long   DefaultISR                                      /* 210*/
    .long   DefaultISR                                      /* 211*/
    .long   DefaultISR                                      /* 212*/
    .long   DefaultISR                                      /* 213*/
    .long   DefaultISR                                      /* 214*/
    .long   DefaultISR                                      /* 215*/
    .long   DefaultISR                                      /* 216*/
    .long   DefaultISR                                      /* 217*/
    .long   DefaultISR                                      /* 218*/
    .long   DefaultISR                                      /* 219*/
    .long   DefaultISR                                      /* 220*/
    .long   DefaultISR                                      /* 221*/
    .long   DefaultISR                                      /* 222*/
    .long   DefaultISR                                      /* 223*/
    .long   DefaultISR                                      /* 224*/
    .long   DefaultISR                                      /* 225*/
    .long   DefaultISR                                      /* 226*/
    .long   DefaultISR                                      /* 227*/
    .long   DefaultISR                                      /* 228*/
    .long   DefaultISR                                      /* 229*/
    .long   DefaultISR                                      /* 230*/
    .long   DefaultISR                                      /* 231*/
    .long   DefaultISR                                      /* 232*/
    .long   DefaultISR                                      /* 233*/
    .long   DefaultISR                                      /* 234*/
    .long   DefaultISR                                      /* 235*/
    .long   DefaultISR                                      /* 236*/
    .long   DefaultISR                                      /* 237*/
    .long   DefaultISR                                      /* 238*/
    .long   DefaultISR                                      /* 239*/
    .long   DefaultISR                                      /* 240*/
    .long   DefaultISR                                      /* 241*/
    .long   DefaultISR                                      /* 242*/
    .long   DefaultISR                                      /* 243*/
    .long   DefaultISR                                      /* 244*/
    .long   DefaultISR                                      /* 245*/
    .long   DefaultISR                                      /* 246*/
    .long   DefaultISR                                      /* 247*/
    .long   DefaultISR                                      /* 248*/
    .long   DefaultISR                                      /* 249*/
    .long   DefaultISR                                      /* 250*/
    .long   DefaultISR                                      /* 251*/
    .long   DefaultISR                                      /* 252*/
    .long   DefaultISR                                      /* 253*/
    .long   DefaultISR                                      /* 254*/
    .long   0xFFFFFFFF                                      /*  Reserved for user TRIM value*/

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
    .macro def_irq_handler	handler_name
    .weak \handler_name
    .set  \handler_name, DefaultISR
    .endm

/* Exception Handlers */
    def_irq_handler    NMI_Handler
    def_irq_handler    HardFault_Handler
    def_irq_handler    MemManage_Handler
    def_irq_handler    BusFault_Handler
    def_irq_handler    UsageFault_Handler
    def_irq_handler    SVC_Handler
    def_irq_handler    DebugMon_Handler
    def_irq_handler    PendSV_Handler
    def_irq_handler    SysTick_Handler
    def_irq_handler    DMA0_IRQHandler
    def_irq_handler    DMA1_IRQHandler
    def_irq_handler    DMA2_IRQHandler
    def_irq_handler    DMA3_IRQHandler
    def_irq_handler    DMA4_IRQHandler
    def_irq_handler    DMA5_IRQHandler
    def_irq_handler    DMA6_IRQHandler
    def_irq_handler    DMA7_IRQHandler
    def_irq_handler    DMA8_IRQHandler
    def_irq_handler    DMA9_IRQHandler
    def_irq_handler    DMA10_IRQHandler
    def_irq_handler    DMA11_IRQHandler
    def_irq_handler    DMA12_IRQHandler
    def_irq_handler    DMA13_IRQHandler
    def_irq_handler    DMA14_IRQHandler
    def_irq_handler    DMA15_IRQHandler
    def_irq_handler    DMA_Error_IRQHandler
    def_irq_handler    MCM_IRQHandler
    def_irq_handler    FTFC_IRQHandler
    def_irq_handler    Read_Collision_IRQHandler
    def_irq_handler    LVD_LVW_IRQHandler
    def_irq_handler    FTFC_Fault_IRQHandler
    def_irq_handler    WDOG_EWM_IRQHandler
    def_irq_handler    RCM_IRQHandler
    def_irq_handler    LPI2C0_Master_IRQHandler
    def_irq_handler    LPI2C0_Slave_IRQHandler
    def_irq_handler    LPSPI0_IRQHandler
    def_irq_handler    LPSPI1_IRQHandler
    def_irq_handler    LPSPI2_IRQHandler
    def_irq_handler    Reserved45_IRQHandler
    def_irq_handler    Reserved46_IRQHandler
    def_irq_handler    LPUART0_RxTx_IRQHandler
    def_irq_handler    Reserved48_IRQHandler
    def_irq_handler    LPUART1_RxTx_IRQHandler
    def_irq_handler    Reserved50_IRQHandler
    def_irq_handler    LPUART2_RxTx_IRQHandler
    def_irq_handler    Reserved52_IRQHandler
    def_irq_handler    Reserved53_IRQHandler
    def_irq_handler    Reserved54_IRQHandler
    def_irq_handler    ADC0_IRQHandler
    def_irq_handler    ADC1_IRQHandler
    def_irq_handler    CMP0_IRQHandler
    def_irq_handler    Reserved58_IRQHandler
    def_irq_handler    Reserved59_IRQHandler
    def_irq_handler    ERM_single_fault_IRQHandler
    def_irq_handler    ERM_double_fault_IRQHandler
    def_irq_handler    RTC_IRQHandler
    def_irq_handler    RTC_Seconds_IRQHandler
    def_irq_handler    LPIT0_Ch0_IRQHandler
    def_irq_handler    LPIT0_Ch1_IRQHandler
    def_irq_handler    LPIT0_Ch2_IRQHandler
    def_irq_handler    LPIT0_Ch3_IRQHandler
    def_irq_handler    PDB0_IRQHandler
    def_irq_handler    Reserved69_IRQHandler
    def_irq_handler    Reserved70_IRQHandler
    def_irq_handler    Reserved71_IRQHandler
    def_irq_handler    Reserved72_IRQHandler
    def_irq_handler    SCG_IRQHandler
    def_irq_handler    LPTMR0_IRQHandler
    def_irq_handler    PORTA_IRQHandler
    def_irq_handler    PORTB_IRQHandler
    def_irq_handler    PORTC_IRQHandler
    def_irq_handler    PORTD_IRQHandler
    def_irq_handler    PORTE_IRQHandler
    def_irq_handler    SWI_IRQHandler
    def_irq_handler    Reserved81_IRQHandler
    def_irq_handler    Reserved82_IRQHandler
    def_irq_handler    Reserved83_IRQHandler
    def_irq_handler    PDB1_IRQHandler
    def_irq_handler    FLEXIO_IRQHandler
    def_irq_handler    Reserved86_IRQHandler
    def_irq_handler    Reserved87_IRQHandler
    def_irq_handler    Reserved88_IRQHandler
    def_irq_handler    Reserved89_IRQHandler
    def_irq_handler    Reserved90_IRQHandler
    def_irq_handler    Reserved91_IRQHandler
    def_irq_handler    Reserved92_IRQHandler
    def_irq_handler    Reserved93_IRQHandler
    def_irq_handler    CAN0_ORed_IRQHandler
    def_irq_handler    CAN0_Error_IRQHandler
    def_irq_handler    CAN0_Wake_Up_IRQHandler
    def_irq_handler    CAN0_ORed_0_15_MB_IRQHandler
    def_irq_handler    CAN0_ORed_16_31_MB_IRQHandler
    def_irq_handler    Reserved99_IRQHandler
    def_irq_handler    Reserved100_IRQHandler
    def_irq_handler    CAN1_ORed_IRQHandler
    def_irq_handler    CAN1_Error_IRQHandler
    def_irq_handler    Reserved103_IRQHandler
    def_irq_handler    CAN1_ORed_0_15_MB_IRQHandler
    def_irq_handler    Reserved105_IRQHandler
    def_irq_handler    Reserved106_IRQHandler
    def_irq_handler    Reserved107_IRQHandler
    def_irq_handler    CAN2_ORed_IRQHandler
    def_irq_handler    CAN2_Error_IRQHandler
    def_irq_handler    Reserved110_IRQHandler
    def_irq_handler    CAN2_ORed_0_15_MB_IRQHandler
    def_irq_handler    Reserved112_IRQHandler
    def_irq_handler    Reserved113_IRQHandler
    def_irq_handler    Reserved114_IRQHandler
    def_irq_handler    FTM0_Ch0_Ch1_IRQHandler
    def_irq_handler    FTM0_Ch2_Ch3_IRQHandler
    def_irq_handler    FTM0_Ch4_Ch5_IRQHandler
    def_irq_handler    FTM0_Ch6_Ch7_IRQHandler
    def_irq_handler    FTM0_Fault_IRQHandler
    def_irq_handler    FTM0_Ovf_Reload_IRQHandler
    def_irq_handler    FTM1_Ch0_Ch1_IRQHandler
    def_irq_handler    FTM1_Ch2_Ch3_IRQHandler
    def_irq_handler    FTM1_Ch4_Ch5_IRQHandler
    def_irq_handler    FTM1_Ch6_Ch7_IRQHandler
    def_irq_handler    FTM1_Fault_IRQHandler
    def_irq_handler    FTM1_Ovf_Reload_IRQHandler
    def_irq_handler    FTM2_Ch0_Ch1_IRQHandler
    def_irq_handler    FTM2_Ch2_Ch3_IRQHandler
    def_irq_handler    FTM2_Ch4_Ch5_IRQHandler
    def_irq_handler    FTM2_Ch6_Ch7_IRQHandler
    def_irq_handler    FTM2_Fault_IRQHandler
    def_irq_handler    FTM2_Ovf_Reload_IRQHandler
    def_irq_handler    FTM3_Ch0_Ch1_IRQHandler
    def_irq_handler    FTM3_Ch2_Ch3_IRQHandler
    def_irq_handler    FTM3_Ch4_Ch5_IRQHandler
    def_irq_handler    FTM3_Ch6_Ch7_IRQHandler
    def_irq_handler    FTM3_Fault_IRQHandler
    def_irq_handler    FTM3_Ovf_Reload_IRQHandler

    .end
