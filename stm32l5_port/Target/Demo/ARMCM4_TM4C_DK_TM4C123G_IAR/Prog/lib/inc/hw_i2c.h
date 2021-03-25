//*****************************************************************************
//
// hw_i2c.h - Macros used when accessing the I2C master and slave hardware.
//
// Copyright (c) 2005-2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
// 
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the  
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// This is part of revision 1.1 of the Tiva Firmware Development Package.
//
//*****************************************************************************

#ifndef __HW_I2C_H__
#define __HW_I2C_H__

//*****************************************************************************
//
// The following are defines for the I2C register offsets.
//
//*****************************************************************************
#define I2C_O_MSA               0x00000000  // I2C Master Slave Address
#define I2C_O_MCS               0x00000004  // I2C Master Control/Status
#define I2C_O_MDR               0x00000008  // I2C Master Data
#define I2C_O_MTPR              0x0000000C  // I2C Master Timer Period
#define I2C_O_MIMR              0x00000010  // I2C Master Interrupt Mask
#define I2C_O_MRIS              0x00000014  // I2C Master Raw Interrupt Status
#define I2C_O_MMIS              0x00000018  // I2C Master Masked Interrupt
                                            // Status
#define I2C_O_MICR              0x0000001C  // I2C Master Interrupt Clear
#define I2C_O_MCR               0x00000020  // I2C Master Configuration
#define I2C_O_MCLKOCNT          0x00000024  // I2C Master Clock Low Timeout
                                            // Count
#define I2C_O_MBMON             0x0000002C  // I2C Master Bus Monitor
#define I2C_O_MCR2              0x00000038  // I2C Master Configuration 2
#define I2C_O_SOAR              0x00000800  // I2C Slave Own Address
#define I2C_O_SCSR              0x00000804  // I2C Slave Control/Status
#define I2C_O_SDR               0x00000808  // I2C Slave Data
#define I2C_O_SIMR              0x0000080C  // I2C Slave Interrupt Mask
#define I2C_O_SRIS              0x00000810  // I2C Slave Raw Interrupt Status
#define I2C_O_SMIS              0x00000814  // I2C Slave Masked Interrupt
                                            // Status
#define I2C_O_SICR              0x00000818  // I2C Slave Interrupt Clear
#define I2C_O_SOAR2             0x0000081C  // I2C Slave Own Address 2
#define I2C_O_SACKCTL           0x00000820  // I2C Slave ACK Control
#define I2C_O_PP                0x00000FC0  // I2C Peripheral Properties
#define I2C_O_PC                0x00000FC4  // I2C Peripheral Configuration

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MSA register.
//
//*****************************************************************************
#define I2C_MSA_SA_M            0x000000FE  // I2C Slave Address
#define I2C_MSA_RS              0x00000001  // Receive not send
#define I2C_MSA_SA_S            1

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MCS register.
//
//*****************************************************************************
#define I2C_MCS_CLKTO           0x00000080  // Clock Timeout Error
#define I2C_MCS_BUSBSY          0x00000040  // Bus Busy
#define I2C_MCS_IDLE            0x00000020  // I2C Idle
#define I2C_MCS_ARBLST          0x00000010  // Arbitration Lost
#define I2C_MCS_HS              0x00000010  // High-Speed Enable
#define I2C_MCS_ACK             0x00000008  // Data Acknowledge Enable
#define I2C_MCS_DATACK          0x00000008  // Acknowledge Data
#define I2C_MCS_ADRACK          0x00000004  // Acknowledge Address
#define I2C_MCS_STOP            0x00000004  // Generate STOP
#define I2C_MCS_ERROR           0x00000002  // Error
#define I2C_MCS_START           0x00000002  // Generate START
#define I2C_MCS_RUN             0x00000001  // I2C Master Enable
#define I2C_MCS_BUSY            0x00000001  // I2C Busy

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MDR register.
//
//*****************************************************************************
#define I2C_MDR_DATA_M          0x000000FF  // Data Transferred
#define I2C_MDR_DATA_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MTPR register.
//
//*****************************************************************************
#define I2C_MTPR_HS             0x00000080  // High-Speed Enable
#define I2C_MTPR_TPR_M          0x0000007F  // Timer Period
#define I2C_MTPR_TPR_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MIMR register.
//
//*****************************************************************************
#define I2C_MIMR_CLKIM          0x00000002  // Clock Timeout Interrupt Mask
#define I2C_MIMR_IM             0x00000001  // Master Interrupt Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MRIS register.
//
//*****************************************************************************
#define I2C_MRIS_CLKRIS         0x00000002  // Clock Timeout Raw Interrupt
                                            // Status
#define I2C_MRIS_RIS            0x00000001  // Master Raw Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MMIS register.
//
//*****************************************************************************
#define I2C_MMIS_CLKMIS         0x00000002  // Clock Timeout Masked Interrupt
                                            // Status
#define I2C_MMIS_MIS            0x00000001  // Masked Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MICR register.
//
//*****************************************************************************
#define I2C_MICR_CLKIC          0x00000002  // Clock Timeout Interrupt Clear
#define I2C_MICR_IC             0x00000001  // Master Interrupt Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MCR register.
//
//*****************************************************************************
#define I2C_MCR_GFE             0x00000040  // I2C Glitch Filter Enable
#define I2C_MCR_SFE             0x00000020  // I2C Slave Function Enable
#define I2C_MCR_MFE             0x00000010  // I2C Master Function Enable
#define I2C_MCR_LPBK            0x00000001  // I2C Loopback

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MCLKOCNT register.
//
//*****************************************************************************
#define I2C_MCLKOCNT_CNTL_M     0x000000FF  // I2C Master Count
#define I2C_MCLKOCNT_CNTL_S     0

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MBMON register.
//
//*****************************************************************************
#define I2C_MBMON_SDA           0x00000002  // I2C SDA Status
#define I2C_MBMON_SCL           0x00000001  // I2C SCL Status

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_MCR2 register.
//
//*****************************************************************************
#define I2C_MCR2_GFPW_M         0x00000070  // I2C Glitch Filter Pulse Width
#define I2C_MCR2_GFPW_BYPASS    0x00000000  // Bypass
#define I2C_MCR2_GFPW_1         0x00000010  // 1 clock
#define I2C_MCR2_GFPW_2         0x00000020  // 2 clocks
#define I2C_MCR2_GFPW_3         0x00000030  // 3 clocks
#define I2C_MCR2_GFPW_4         0x00000040  // 4 clocks
#define I2C_MCR2_GFPW_8         0x00000050  // 8 clocks
#define I2C_MCR2_GFPW_16        0x00000060  // 16 clocks
#define I2C_MCR2_GFPW_32        0x00000070  // 32 clocks

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SOAR register.
//
//*****************************************************************************
#define I2C_SOAR_OAR_M          0x0000007F  // I2C Slave Own Address
#define I2C_SOAR_OAR_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SCSR register.
//
//*****************************************************************************
#define I2C_SCSR_OAR2SEL        0x00000008  // OAR2 Address Matched
#define I2C_SCSR_FBR            0x00000004  // First Byte Received
#define I2C_SCSR_TREQ           0x00000002  // Transmit Request
#define I2C_SCSR_DA             0x00000001  // Device Active
#define I2C_SCSR_RREQ           0x00000001  // Receive Request

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SDR register.
//
//*****************************************************************************
#define I2C_SDR_DATA_M          0x000000FF  // Data for Transfer
#define I2C_SDR_DATA_S          0

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SIMR register.
//
//*****************************************************************************
#define I2C_SIMR_STOPIM         0x00000004  // Stop Condition Interrupt Mask
#define I2C_SIMR_STARTIM        0x00000002  // Start Condition Interrupt Mask
#define I2C_SIMR_DATAIM         0x00000001  // Data Interrupt Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SRIS register.
//
//*****************************************************************************
#define I2C_SRIS_STOPRIS        0x00000004  // Stop Condition Raw Interrupt
                                            // Status
#define I2C_SRIS_STARTRIS       0x00000002  // Start Condition Raw Interrupt
                                            // Status
#define I2C_SRIS_DATARIS        0x00000001  // Data Raw Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SMIS register.
//
//*****************************************************************************
#define I2C_SMIS_STOPMIS        0x00000004  // Stop Condition Masked Interrupt
                                            // Status
#define I2C_SMIS_STARTMIS       0x00000002  // Start Condition Masked Interrupt
                                            // Status
#define I2C_SMIS_DATAMIS        0x00000001  // Data Masked Interrupt Status

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SICR register.
//
//*****************************************************************************
#define I2C_SICR_STOPIC         0x00000004  // Stop Condition Interrupt Clear
#define I2C_SICR_STARTIC        0x00000002  // Start Condition Interrupt Clear
#define I2C_SICR_DATAIC         0x00000001  // Data Interrupt Clear

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SOAR2 register.
//
//*****************************************************************************
#define I2C_SOAR2_OAR2EN        0x00000080  // I2C Slave Own Address 2 Enable
#define I2C_SOAR2_OAR2_M        0x0000007F  // I2C Slave Own Address 2
#define I2C_SOAR2_OAR2_S        0

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_SACKCTL register.
//
//*****************************************************************************
#define I2C_SACKCTL_ACKOVAL     0x00000002  // I2C Slave ACK Override Value
#define I2C_SACKCTL_ACKOEN      0x00000001  // I2C Slave ACK Override Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_PP register.
//
//*****************************************************************************
#define I2C_PP_HS               0x00000001  // High-Speed Capable

//*****************************************************************************
//
// The following are defines for the bit fields in the I2C_O_PC register.
//
//*****************************************************************************
#define I2C_PC_HS               0x00000001  // High-Speed Capable

#endif // __HW_I2C_H__
