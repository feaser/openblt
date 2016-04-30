/****************************************************************************************
|  Description: NXP LPC2294 register definitions
|    File Name: lpc2294.h
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
| You have received a copy of the GNU General Public License along with OpenBLT. It 
| should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
| 
****************************************************************************************/
#ifndef LPC2294_H
#define LPC2294_H


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* EXTERNAL MEMORY CONTROLLER (EMC) */
#define BCFG0           (*((volatile unsigned long *) 0xFFE00000))    /* lpc22xx only */
#define BCFG1           (*((volatile unsigned long *) 0xFFE00004))    /* lpc22xx only */
#define BCFG2           (*((volatile unsigned long *) 0xFFE00008))    /* lpc22xx only */
#define BCFG3           (*((volatile unsigned long *) 0xFFE0000C))    /* lpc22xx only */

/* External Interrupts */
#define EXTINT          (*((volatile unsigned char *) 0xE01FC140))
#define EXTWAKE         (*((volatile unsigned char *) 0xE01FC144))
#define EXTMODE         (*((volatile unsigned char *) 0xE01FC148))    /* no in lpc210x*/
#define EXTPOLAR        (*((volatile unsigned char *) 0xE01FC14C))    /* no in lpc210x*/

/* SMemory mapping control. */
#define MEMMAP          (*((volatile unsigned long *) 0xE01FC040))

/* Phase Locked Loop (PLL) */
#define PLLCON          (*((volatile unsigned char *) 0xE01FC080))
#define PLLCFG          (*((volatile unsigned char *) 0xE01FC084))
#define PLLSTAT         (*((volatile unsigned short*) 0xE01FC088))
#define PLLFEED         (*((volatile unsigned char *) 0xE01FC08C))

/* Power Control */
#define PCON            (*((volatile unsigned char *) 0xE01FC0C0))
#define PCONP           (*((volatile unsigned long *) 0xE01FC0C4))

/* VPB Divider */
#define VPBDIV          (*((volatile unsigned char *) 0xE01FC100))

/* Memory Accelerator Module (MAM) */
#define MAMCR           (*((volatile unsigned char *) 0xE01FC000))
#define MAMTIM          (*((volatile unsigned char *) 0xE01FC004))

/* Vectored Interrupt Controller (VIC) */
#define VICIRQStatus    (*((volatile unsigned long *) 0xFFFFF000))
#define VICFIQStatus    (*((volatile unsigned long *) 0xFFFFF004))
#define VICRawIntr      (*((volatile unsigned long *) 0xFFFFF008))
#define VICIntSelect    (*((volatile unsigned long *) 0xFFFFF00C))
#define VICIntEnable    (*((volatile unsigned long *) 0xFFFFF010))
#define VICIntEnClr     (*((volatile unsigned long *) 0xFFFFF014))
#define VICSoftInt      (*((volatile unsigned long *) 0xFFFFF018))
#define VICSoftIntClear (*((volatile unsigned long *) 0xFFFFF01C))
#define VICProtection   (*((volatile unsigned long *) 0xFFFFF020))
#define VICVectAddr     (*((volatile unsigned long *) 0xFFFFF030))
#define VICDefVectAddr  (*((volatile unsigned long *) 0xFFFFF034))
#define VICVectAddr0    (*((volatile unsigned long *) 0xFFFFF100))
#define VICVectAddr1    (*((volatile unsigned long *) 0xFFFFF104))
#define VICVectAddr2    (*((volatile unsigned long *) 0xFFFFF108))
#define VICVectAddr3    (*((volatile unsigned long *) 0xFFFFF10C))
#define VICVectAddr4    (*((volatile unsigned long *) 0xFFFFF110))
#define VICVectAddr5    (*((volatile unsigned long *) 0xFFFFF114))
#define VICVectAddr6    (*((volatile unsigned long *) 0xFFFFF118))
#define VICVectAddr7    (*((volatile unsigned long *) 0xFFFFF11C))
#define VICVectAddr8    (*((volatile unsigned long *) 0xFFFFF120))
#define VICVectAddr9    (*((volatile unsigned long *) 0xFFFFF124))
#define VICVectAddr10   (*((volatile unsigned long *) 0xFFFFF128))
#define VICVectAddr11   (*((volatile unsigned long *) 0xFFFFF12C))
#define VICVectAddr12   (*((volatile unsigned long *) 0xFFFFF130))
#define VICVectAddr13   (*((volatile unsigned long *) 0xFFFFF134))
#define VICVectAddr14   (*((volatile unsigned long *) 0xFFFFF138))
#define VICVectAddr15   (*((volatile unsigned long *) 0xFFFFF13C))
#define VICVectCntl0    (*((volatile unsigned long *) 0xFFFFF200))
#define VICVectCntl1    (*((volatile unsigned long *) 0xFFFFF204))
#define VICVectCntl2    (*((volatile unsigned long *) 0xFFFFF208))
#define VICVectCntl3    (*((volatile unsigned long *) 0xFFFFF20C))
#define VICVectCntl4    (*((volatile unsigned long *) 0xFFFFF210))
#define VICVectCntl5    (*((volatile unsigned long *) 0xFFFFF214))
#define VICVectCntl6    (*((volatile unsigned long *) 0xFFFFF218))
#define VICVectCntl7    (*((volatile unsigned long *) 0xFFFFF21C))
#define VICVectCntl8    (*((volatile unsigned long *) 0xFFFFF220))
#define VICVectCntl9    (*((volatile unsigned long *) 0xFFFFF224))
#define VICVectCntl10   (*((volatile unsigned long *) 0xFFFFF228))
#define VICVectCntl11   (*((volatile unsigned long *) 0xFFFFF22C))
#define VICVectCntl12   (*((volatile unsigned long *) 0xFFFFF230))
#define VICVectCntl13   (*((volatile unsigned long *) 0xFFFFF234))
#define VICVectCntl14   (*((volatile unsigned long *) 0xFFFFF238))
#define VICVectCntl15   (*((volatile unsigned long *) 0xFFFFF23C))

/* Pin Connect Block */
#define PINSEL0         (*((volatile unsigned long *) 0xE002C000))
#define PINSEL1         (*((volatile unsigned long *) 0xE002C004))
#define PINSEL2         (*((volatile unsigned long *) 0xE002C014))      /* no in lpc210x*/

/* General Purpose Input/Output (GPIO) */
#define IOPIN           (*((volatile unsigned long *) 0xE0028000))      /* lpc210x only */
#define IOSET           (*((volatile unsigned long *) 0xE0028004))      /* lpc210x only */
#define IODIR           (*((volatile unsigned long *) 0xE0028008))      /* lpc210x only */
#define IOCLR           (*((volatile unsigned long *) 0xE002800C))      /* lpc210x only */

#define IO0PIN          (*((volatile unsigned long *) 0xE0028000))      /* no in lpc210x*/
#define IO0SET          (*((volatile unsigned long *) 0xE0028004))      /* no in lpc210x*/
#define IO0DIR          (*((volatile unsigned long *) 0xE0028008))      /* no in lpc210x*/
#define IO0CLR          (*((volatile unsigned long *) 0xE002800C))      /* no in lpc210x*/

#define IO1PIN          (*((volatile unsigned long *) 0xE0028010))      /* no in lpc210x*/
#define IO1SET          (*((volatile unsigned long *) 0xE0028014))      /* no in lpc210x*/
#define IO1DIR          (*((volatile unsigned long *) 0xE0028018))      /* no in lpc210x*/
#define IO1CLR          (*((volatile unsigned long *) 0xE002801C))      /* no in lpc210x*/

#define IO2PIN          (*((volatile unsigned long *) 0xE0028020))      /* lpc22xx only */
#define IO2SET          (*((volatile unsigned long *) 0xE0028024))      /* lpc22xx only */
#define IO2DIR          (*((volatile unsigned long *) 0xE0028028))      /* lpc22xx only */
#define IO2CLR          (*((volatile unsigned long *) 0xE002802C))      /* lpc22xx only */

#define IO3PIN          (*((volatile unsigned long *) 0xE0028030))      /* lpc22xx only */
#define IO3SET          (*((volatile unsigned long *) 0xE0028034))      /* lpc22xx only */
#define IO3DIR          (*((volatile unsigned long *) 0xE0028038))      /* lpc22xx only */
#define IO3CLR          (*((volatile unsigned long *) 0xE002803C))      /* lpc22xx only */

/* Universal Asynchronous Receiver Transmitter 0 (UART0) */
#define U0RBR           (*((volatile unsigned char *) 0xE000C000))
#define U0THR           (*((volatile unsigned char *) 0xE000C000))
#define U0IER           (*((volatile unsigned char *) 0xE000C004))
#define U0IIR           (*((volatile unsigned char *) 0xE000C008))
#define U0FCR           (*((volatile unsigned char *) 0xE000C008))
#define U0LCR           (*((volatile unsigned char *) 0xE000C00C))
#define U0LSR           (*((volatile unsigned char *) 0xE000C014))
#define U0SCR           (*((volatile unsigned char *) 0xE000C01C))
#define U0DLL           (*((volatile unsigned char *) 0xE000C000))
#define U0DLM           (*((volatile unsigned char *) 0xE000C004))

/* Universal Asynchronous Receiver Transmitter 1 (UART1) */
#define U1RBR           (*((volatile unsigned char *) 0xE0010000))
#define U1THR           (*((volatile unsigned char *) 0xE0010000))
#define U1IER           (*((volatile unsigned char *) 0xE0010004))
#define U1IIR           (*((volatile unsigned char *) 0xE0010008))
#define U1FCR           (*((volatile unsigned char *) 0xE0010008))
#define U1LCR           (*((volatile unsigned char *) 0xE001000C))
#define U1MCR           (*((volatile unsigned char *) 0xE0010010))
#define U1LSR           (*((volatile unsigned char *) 0xE0010014))
#define U1MSR           (*((volatile unsigned char *) 0xE0010018))
#define U1SCR           (*((volatile unsigned char *) 0xE001001C))
#define U1DLL           (*((volatile unsigned char *) 0xE0010000))
#define U1DLM           (*((volatile unsigned char *) 0xE0010004))

/* I2C (8/16 bit data bus) */
#define I2CONSET        (*((volatile unsigned long *) 0xE001C000))
#define I2STAT          (*((volatile unsigned long *) 0xE001C004))
#define I2DAT           (*((volatile unsigned long *) 0xE001C008))
#define I2ADR           (*((volatile unsigned long *) 0xE001C00C))
#define I2SCLH          (*((volatile unsigned long *) 0xE001C010))
#define I2SCLL          (*((volatile unsigned long *) 0xE001C014))
#define I2CONCLR        (*((volatile unsigned long *) 0xE001C018))

/* SPI (Serial Peripheral Interface) */
        /* only for lpc210x*/
#define SPI_SPCR        (*((volatile unsigned char *) 0xE0020000))
#define SPI_SPSR        (*((volatile unsigned char *) 0xE0020004))
#define SPI_SPDR        (*((volatile unsigned char *) 0xE0020008))
#define SPI_SPCCR       (*((volatile unsigned char *) 0xE002000C))
#define SPI_SPINT       (*((volatile unsigned char *) 0xE002001C))

#define S0PCR           (*((volatile unsigned char *) 0xE0020000))      /* no in lpc210x*/
#define S0PSR           (*((volatile unsigned char *) 0xE0020004))      /* no in lpc210x*/
#define S0PDR           (*((volatile unsigned char *) 0xE0020008))      /* no in lpc210x*/
#define S0PCCR          (*((volatile unsigned char *) 0xE002000C))      /* no in lpc210x*/
#define S0PINT          (*((volatile unsigned char *) 0xE002001C))      /* no in lpc210x*/

#define S1PCR           (*((volatile unsigned char *) 0xE0030000))      /* no in lpc210x*/
#define S1PSR           (*((volatile unsigned char *) 0xE0030004))      /* no in lpc210x*/
#define S1PDR           (*((volatile unsigned char *) 0xE0030008))      /* no in lpc210x*/
#define S1PCCR          (*((volatile unsigned char *) 0xE003000C))      /* no in lpc210x*/
#define S1PINT          (*((volatile unsigned char *) 0xE003001C))      /* no in lpc210x*/

/* CAN CONTROLLERS AND ACCEPTANCE FILTER */
#define CAN1MOD         (*((volatile unsigned long *) 0xE0044000))      /* All CAN Parts */
#define CAN1CMR         (*((volatile unsigned long *) 0xE0044004))      /* All CAN Parts */
#define CAN1GSR         (*((volatile unsigned long *) 0xE0044008))      /* All CAN Parts */
#define CAN1ICR         (*((volatile unsigned long *) 0xE004400C))      /* All CAN Parts */
#define CAN1IER         (*((volatile unsigned long *) 0xE0044010))      /* All CAN Parts */
#define CAN1BTR         (*((volatile unsigned long *) 0xE0044014))      /* All CAN Parts */
#define CAN1EWL         (*((volatile unsigned long *) 0xE0044018))      /* All CAN Parts */
#define CAN1SR          (*((volatile unsigned long *) 0xE004401C))      /* All CAN Parts */
#define CAN1RFS         (*((volatile unsigned long *) 0xE0044020))      /* All CAN Parts */
#define CAN1RID         (*((volatile unsigned long *) 0xE0044024))      /* All CAN Parts */
#define CAN1RDA         (*((volatile unsigned long *) 0xE0044028))      /* All CAN Parts */
#define CAN1RDB         (*((volatile unsigned long *) 0xE004402C))      /* All CAN Parts */
#define CAN1TFI1        (*((volatile unsigned long *) 0xE0044030))      /* All CAN Parts */
#define CAN1TID1        (*((volatile unsigned long *) 0xE0044034))      /* All CAN Parts */
#define CAN1TDA1        (*((volatile unsigned long *) 0xE0044038))      /* All CAN Parts */
#define CAN1TDB1        (*((volatile unsigned long *) 0xE004403C))      /* All CAN Parts */
#define CAN1TFI2        (*((volatile unsigned long *) 0xE0044040))      /* All CAN Parts */
#define CAN1TID2        (*((volatile unsigned long *) 0xE0044044))      /* All CAN Parts */
#define CAN1TDA2        (*((volatile unsigned long *) 0xE0044048))      /* All CAN Parts */
#define CAN1TDB2        (*((volatile unsigned long *) 0xE004404C))      /* All CAN Parts */
#define CAN1TFI3        (*((volatile unsigned long *) 0xE0044050))      /* All CAN Parts */
#define CAN1TID3        (*((volatile unsigned long *) 0xE0044054))      /* All CAN Parts */
#define CAN1TDA3        (*((volatile unsigned long *) 0xE0044058))      /* All CAN Parts */
#define CAN1TDB3        (*((volatile unsigned long *) 0xE004405C))      /* All CAN Parts */

#define CAN2MOD         (*((volatile unsigned long *) 0xE0048000))      /* All CAN Parts */
#define CAN2CMR         (*((volatile unsigned long *) 0xE0048004))      /* All CAN Parts */
#define CAN2GSR         (*((volatile unsigned long *) 0xE0048008))      /* All CAN Parts */
#define CAN2ICR         (*((volatile unsigned long *) 0xE004800C))      /* All CAN Parts */
#define CAN2IER         (*((volatile unsigned long *) 0xE0048010))      /* All CAN Parts */
#define CAN2BTR         (*((volatile unsigned long *) 0xE0048014))      /* All CAN Parts */
#define CAN2EWL         (*((volatile unsigned long *) 0xE0048018))      /* All CAN Parts */
#define CAN2SR          (*((volatile unsigned long *) 0xE004801C))      /* All CAN Parts */
#define CAN2RFS         (*((volatile unsigned long *) 0xE0048020))      /* All CAN Parts */
#define CAN2RID         (*((volatile unsigned long *) 0xE0048024))      /* All CAN Parts */
#define CAN2RDA         (*((volatile unsigned long *) 0xE0048028))      /* All CAN Parts */
#define CAN2RDB         (*((volatile unsigned long *) 0xE004802C))      /* All CAN Parts */
#define CAN2TFI1        (*((volatile unsigned long *) 0xE0048030))      /* All CAN Parts */
#define CAN2TID1        (*((volatile unsigned long *) 0xE0048034))      /* All CAN Parts */
#define CAN2TDA1        (*((volatile unsigned long *) 0xE0048038))      /* All CAN Parts */
#define CAN2TDB1        (*((volatile unsigned long *) 0xE004803C))      /* All CAN Parts */
#define CAN2TFI2        (*((volatile unsigned long *) 0xE0048040))      /* All CAN Parts */
#define CAN2TID2        (*((volatile unsigned long *) 0xE0048044))      /* All CAN Parts */
#define CAN2TDA2        (*((volatile unsigned long *) 0xE0048048))      /* All CAN Parts */
#define CAN2TDB2        (*((volatile unsigned long *) 0xE004804C))      /* All CAN Parts */
#define CAN2TFI3        (*((volatile unsigned long *) 0xE0048050))      /* All CAN Parts */
#define CAN2TID3        (*((volatile unsigned long *) 0xE0048054))      /* All CAN Parts */
#define CAN2TDA3        (*((volatile unsigned long *) 0xE0048058))      /* All CAN Parts */
#define CAN2TDB3        (*((volatile unsigned long *) 0xE004805C))      /* All CAN Parts */

#define CAN3MOD         (*((volatile unsigned long *) 0xE004C000))      /* lpc2194\lpc2294 only */
#define CAN3CMR         (*((volatile unsigned long *) 0xE004C004))      /* lpc2194\lpc2294 only */
#define CAN3GSR         (*((volatile unsigned long *) 0xE004C008))      /* lpc2194\lpc2294 only */
#define CAN3ICR         (*((volatile unsigned long *) 0xE004C00C))      /* lpc2194\lpc2294 only */
#define CAN3IER         (*((volatile unsigned long *) 0xE004C010))      /* lpc2194\lpc2294 only */
#define CAN3BTR         (*((volatile unsigned long *) 0xE004C014))      /* lpc2194\lpc2294 only */
#define CAN3EWL         (*((volatile unsigned long *) 0xE004C018))      /* lpc2194\lpc2294 only */
#define CAN3SR          (*((volatile unsigned long *) 0xE004C01C))      /* lpc2194\lpc2294 only */
#define CAN3RFS         (*((volatile unsigned long *) 0xE004C020))      /* lpc2194\lpc2294 only */
#define CAN3RID         (*((volatile unsigned long *) 0xE004C024))      /* lpc2194\lpc2294 only */
#define CAN3RDA         (*((volatile unsigned long *) 0xE004C028))      /* lpc2194\lpc2294 only */
#define CAN3RDB         (*((volatile unsigned long *) 0xE004C02C))      /* lpc2194\lpc2294 only */
#define CAN3TFI1        (*((volatile unsigned long *) 0xE004C030))      /* lpc2194\lpc2294 only */
#define CAN3TID1        (*((volatile unsigned long *) 0xE004C034))      /* lpc2194\lpc2294 only */
#define CAN3TDA1        (*((volatile unsigned long *) 0xE004C038))      /* lpc2194\lpc2294 only */
#define CAN3TDB1        (*((volatile unsigned long *) 0xE004C03C))      /* lpc2194\lpc2294 only */
#define CAN3TFI2        (*((volatile unsigned long *) 0xE004C040))      /* lpc2194\lpc2294 only */
#define CAN3TID2        (*((volatile unsigned long *) 0xE004C044))      /* lpc2194\lpc2294 only */
#define CAN3TDA2        (*((volatile unsigned long *) 0xE004C048))      /* lpc2194\lpc2294 only */
#define CAN3TDB2        (*((volatile unsigned long *) 0xE004C04C))      /* lpc2194\lpc2294 only */
#define CAN3TFI3        (*((volatile unsigned long *) 0xE004C050))      /* lpc2194\lpc2294 only */
#define CAN3TID3        (*((volatile unsigned long *) 0xE004C054))      /* lpc2194\lpc2294 only */
#define CAN3TDA3        (*((volatile unsigned long *) 0xE004C058))      /* lpc2194\lpc2294 only */
#define CAN3TDB3        (*((volatile unsigned long *) 0xE004C05C))      /* lpc2194\lpc2294 only */

#define CAN4MOD         (*((volatile unsigned long *) 0xE0050000))      /* lpc2194\lpc2294 only */
#define CAN4CMR         (*((volatile unsigned long *) 0xE0050004))      /* lpc2194\lpc2294 only */
#define CAN4GSR         (*((volatile unsigned long *) 0xE0050008))      /* lpc2194\lpc2294 only */
#define CAN4ICR         (*((volatile unsigned long *) 0xE005000C))      /* lpc2194\lpc2294 only */
#define CAN4IER         (*((volatile unsigned long *) 0xE0050010))      /* lpc2194\lpc2294 only */
#define CAN4BTR         (*((volatile unsigned long *) 0xE0050014))      /* lpc2194\lpc2294 only */
#define CAN4EWL         (*((volatile unsigned long *) 0xE0050018))      /* lpc2194\lpc2294 only */
#define CAN4SR          (*((volatile unsigned long *) 0xE005001C))      /* lpc2194\lpc2294 only */
#define CAN4RFS         (*((volatile unsigned long *) 0xE0050020))      /* lpc2194\lpc2294 only */
#define CAN4RID         (*((volatile unsigned long *) 0xE0050024))      /* lpc2194\lpc2294 only */
#define CAN4RDA         (*((volatile unsigned long *) 0xE0050028))      /* lpc2194\lpc2294 only */
#define CAN4RDB         (*((volatile unsigned long *) 0xE005002C))      /* lpc2194\lpc2294 only */
#define CAN4TFI1        (*((volatile unsigned long *) 0xE0050030))      /* lpc2194\lpc2294 only */
#define CAN4TID1        (*((volatile unsigned long *) 0xE0050034))      /* lpc2194\lpc2294 only */
#define CAN4TDA1        (*((volatile unsigned long *) 0xE0050038))      /* lpc2194\lpc2294 only */
#define CAN4TDB1        (*((volatile unsigned long *) 0xE005003C))      /* lpc2194\lpc2294 only */
#define CAN4TFI2        (*((volatile unsigned long *) 0xE0050040))      /* lpc2194\lpc2294 only */
#define CAN4TID2        (*((volatile unsigned long *) 0xE0050044))      /* lpc2194\lpc2294 only */
#define CAN4TDA2        (*((volatile unsigned long *) 0xE0050048))      /* lpc2194\lpc2294 only */
#define CAN4TDB2        (*((volatile unsigned long *) 0xE005004C))      /* lpc2194\lpc2294 only */
#define CAN4TFI3        (*((volatile unsigned long *) 0xE0050050))      /* lpc2194\lpc2294 only */
#define CAN4TID3        (*((volatile unsigned long *) 0xE0050054))      /* lpc2194\lpc2294 only */
#define CAN4TDA3        (*((volatile unsigned long *) 0xE0050058))      /* lpc2194\lpc2294 only */
#define CAN4TDB3        (*((volatile unsigned long *) 0xE005005C))      /* lpc2194\lpc2294 only */


#define CANTxSR         (*((volatile unsigned long *) 0xE0040000))      /* ALL CAN Parts */
#define CANRxSR         (*((volatile unsigned long *) 0xE0040004))      /* ALL CAN Parts */
#define CANMSR          (*((volatile unsigned long *) 0xE0040008))      /* ALL CAN Parts */

#define CANAFMR         (*((volatile unsigned char *) 0xE003C000))      /* ALL CAN Parts */
#define CANSFF_sa       (*((volatile unsigned short*) 0xE003C004))      /* ALL CAN Parts */
#define CANSFF_GRP_sa   (*((volatile unsigned short*) 0xE003C008))      /* ALL CAN Parts */
#define CANEFF_sa       (*((volatile unsigned short*) 0xE003C00C))      /* ALL CAN Parts */
#define CANEFF_GRP_sa   (*((volatile unsigned short*) 0xE003C010))      /* ALL CAN Parts */
#define CANENDofTable   (*((volatile unsigned short*) 0xE003C014))      /* ALL CAN Parts */
#define CANLUTerrAd     (*((volatile unsigned short*) 0xE003C018))      /* ALL CAN Parts */
#define CANLUTerr       (*((volatile unsigned char *) 0xE003C01C))      /* ALL CAN Parts */


/* Timer 0 */
#define T0IR            (*((volatile unsigned long *) 0xE0004000))
#define T0TCR           (*((volatile unsigned long *) 0xE0004004))
#define T0TC            (*((volatile unsigned long *) 0xE0004008))
#define T0PR            (*((volatile unsigned long *) 0xE000400C))
#define T0PC            (*((volatile unsigned long *) 0xE0004010))
#define T0MCR           (*((volatile unsigned long *) 0xE0004014))
#define T0MR0           (*((volatile unsigned long *) 0xE0004018))
#define T0MR1           (*((volatile unsigned long *) 0xE000401C))
#define T0MR2           (*((volatile unsigned long *) 0xE0004020))
#define T0MR3           (*((volatile unsigned long *) 0xE0004024))
#define T0CCR           (*((volatile unsigned long *) 0xE0004028))
#define T0CR0           (*((volatile unsigned long *) 0xE000402C))
#define T0CR1           (*((volatile unsigned long *) 0xE0004030))
#define T0CR2           (*((volatile unsigned long *) 0xE0004034))
#define T0CR3           (*((volatile unsigned long *) 0xE0004038))
#define T0EMR           (*((volatile unsigned long *) 0xE000403C))

/* Timer 1 */
#define T1IR            (*((volatile unsigned long *) 0xE0008000))
#define T1TCR           (*((volatile unsigned long *) 0xE0008004))
#define T1TC            (*((volatile unsigned long *) 0xE0008008))
#define T1PR            (*((volatile unsigned long *) 0xE000800C))
#define T1PC            (*((volatile unsigned long *) 0xE0008010))
#define T1MCR           (*((volatile unsigned long *) 0xE0008014))
#define T1MR0           (*((volatile unsigned long *) 0xE0008018))
#define T1MR1           (*((volatile unsigned long *) 0xE000801C))
#define T1MR2           (*((volatile unsigned long *) 0xE0008020))
#define T1MR3           (*((volatile unsigned long *) 0xE0008024))
#define T1CCR           (*((volatile unsigned long *) 0xE0008028))
#define T1CR0           (*((volatile unsigned long *) 0xE000802C))
#define T1CR1           (*((volatile unsigned long *) 0xE0008030))
#define T1CR2           (*((volatile unsigned long *) 0xE0008034))
#define T1CR3           (*((volatile unsigned long *) 0xE0008038))
#define T1EMR           (*((volatile unsigned long *) 0xE000803C))

/* Pulse Width Modulator (PWM) */
#define PWMIR           (*((volatile unsigned long *) 0xE0014000))
#define PWMTCR          (*((volatile unsigned long *) 0xE0014004))
#define PWMTC           (*((volatile unsigned long *) 0xE0014008))
#define PWMPR           (*((volatile unsigned long *) 0xE001400C))
#define PWMPC           (*((volatile unsigned long *) 0xE0014010))
#define PWMMCR          (*((volatile unsigned long *) 0xE0014014))
#define PWMMR0          (*((volatile unsigned long *) 0xE0014018))
#define PWMMR1          (*((volatile unsigned long *) 0xE001401C))
#define PWMMR2          (*((volatile unsigned long *) 0xE0014020))
#define PWMMR3          (*((volatile unsigned long *) 0xE0014024))
#define PWMMR4          (*((volatile unsigned long *) 0xE0014040))
#define PWMMR5          (*((volatile unsigned long *) 0xE0014044))
#define PWMMR6          (*((volatile unsigned long *) 0xE0014048))
#define PWMPCR          (*((volatile unsigned long *) 0xE001404C))
#define PWMLER          (*((volatile unsigned long *) 0xE0014050))

/* A/D CONVERTER */
#define ADCR            (*((volatile unsigned long *) 0xE0034000))      /* no in lpc210x*/
#define ADDR            (*((volatile unsigned long *) 0xE0034004))      /* no in lpc210x*/

/* Real Time Clock */
#define ILR             (*((volatile unsigned char *) 0xE0024000))
#define CTC             (*((volatile unsigned short*) 0xE0024004))
#define CCR             (*((volatile unsigned char *) 0xE0024008))
#define CIIR            (*((volatile unsigned char *) 0xE002400C))
#define AMR             (*((volatile unsigned char *) 0xE0024010))
#define CTIME0          (*((volatile unsigned long *) 0xE0024014))
#define CTIME1          (*((volatile unsigned long *) 0xE0024018))
#define CTIME2          (*((volatile unsigned long *) 0xE002401C))
#define SEC             (*((volatile unsigned char *) 0xE0024020))
#define MIN             (*((volatile unsigned char *) 0xE0024024))
#define HOUR            (*((volatile unsigned char *) 0xE0024028))
#define DOM             (*((volatile unsigned char *) 0xE002402C))
#define DOW             (*((volatile unsigned char *) 0xE0024030))
#define DOY             (*((volatile unsigned short*) 0xE0024034))
#define MONTH           (*((volatile unsigned char *) 0xE0024038))
#define YEAR            (*((volatile unsigned short*) 0xE002403C))
#define ALSEC           (*((volatile unsigned char *) 0xE0024060))
#define ALMIN           (*((volatile unsigned char *) 0xE0024064))
#define ALHOUR          (*((volatile unsigned char *) 0xE0024068))
#define ALDOM           (*((volatile unsigned char *) 0xE002406C))
#define ALDOW           (*((volatile unsigned char *) 0xE0024070))
#define ALDOY           (*((volatile unsigned short*) 0xE0024074))
#define ALMON           (*((volatile unsigned char *) 0xE0024078))
#define ALYEAR          (*((volatile unsigned short*) 0xE002407C))
#define PREINT          (*((volatile unsigned short*) 0xE0024080))
#define PREFRAC         (*((volatile unsigned short*) 0xE0024084))

/* Watchdog */
#define WDMOD           (*((volatile unsigned char *) 0xE0000000))
#define WDTC            (*((volatile unsigned long *) 0xE0000004))
#define WDFEED          (*((volatile unsigned char *) 0xE0000008))
#define WDTV            (*((volatile unsigned long *) 0xE000000C))

#endif /* LPC2294_H */
/*********************************** end of lpc2294.h **********************************/
