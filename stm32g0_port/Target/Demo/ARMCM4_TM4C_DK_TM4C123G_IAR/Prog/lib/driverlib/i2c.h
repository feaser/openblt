//*****************************************************************************
//
// i2c.h - Prototypes for the I2C Driver.
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
// This is part of revision 1.1 of the Tiva Peripheral Driver Library.
//
//*****************************************************************************

#ifndef __DRIVERLIB_I2C_H__
#define __DRIVERLIB_I2C_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Defines for the API.
//
//*****************************************************************************

//*****************************************************************************
//
// Interrupt defines.
//
//*****************************************************************************
#define I2C_INT_MASTER          0x00000001
#define I2C_INT_SLAVE           0x00000002

//*****************************************************************************
//
// I2C Master commands.
//
//*****************************************************************************
#define I2C_MASTER_CMD_SINGLE_SEND                                            \
                                0x00000007
#define I2C_MASTER_CMD_SINGLE_RECEIVE                                         \
                                0x00000007
#define I2C_MASTER_CMD_BURST_SEND_START                                       \
                                0x00000003
#define I2C_MASTER_CMD_BURST_SEND_CONT                                        \
                                0x00000001
#define I2C_MASTER_CMD_BURST_SEND_FINISH                                      \
                                0x00000005
#define I2C_MASTER_CMD_BURST_SEND_STOP                                        \
                                0x00000004
#define I2C_MASTER_CMD_BURST_SEND_ERROR_STOP                                  \
                                0x00000004
#define I2C_MASTER_CMD_BURST_RECEIVE_START                                    \
                                0x0000000b
#define I2C_MASTER_CMD_BURST_RECEIVE_CONT                                     \
                                0x00000009
#define I2C_MASTER_CMD_BURST_RECEIVE_FINISH                                   \
                                0x00000005
#define I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP                               \
                                0x00000004
#define I2C_MASTER_CMD_QUICK_COMMAND                                          \
                                0x00000027
#define I2C_MASTER_CMD_HS_MASTER_CODE_SEND                                    \
                                0x00000013

//*****************************************************************************
//
// I2C Master error status.
//
//*****************************************************************************
#define I2C_MASTER_ERR_NONE     0
#define I2C_MASTER_ERR_ADDR_ACK 0x00000004
#define I2C_MASTER_ERR_DATA_ACK 0x00000008
#define I2C_MASTER_ERR_ARB_LOST 0x00000010
#define I2C_MASTER_ERR_CLK_TOUT 0x00000080

//*****************************************************************************
//
// I2C Slave action requests
//
//*****************************************************************************
#define I2C_SLAVE_ACT_NONE      0
#define I2C_SLAVE_ACT_RREQ      0x00000001  // Master has sent data
#define I2C_SLAVE_ACT_TREQ      0x00000002  // Master has requested data
#define I2C_SLAVE_ACT_RREQ_FBR  0x00000005  // Master has sent first byte
#define I2C_SLAVE_ACT_OWN2SEL   0x00000008  // Master requested secondary slave
#define I2C_SLAVE_ACT_QCMD      0x00000010  // Master has sent a Quick Command
#define I2C_SLAVE_ACT_QCMD_DATA 0x00000020  // Master Quick Command value

//*****************************************************************************
//
// Miscellaneous I2C driver definitions.
//
//*****************************************************************************
#define I2C_MASTER_MAX_RETRIES  1000        // Number of retries

//*****************************************************************************
//
// I2C Master interrupts.
//
//*****************************************************************************
#define I2C_MASTER_INT_TIMEOUT  0x00000002  // Clock Timeout Interrupt
#define I2C_MASTER_INT_DATA     0x00000001  // Data Interrupt

//*****************************************************************************
//
// I2C Slave interrupts.
//
//*****************************************************************************
#define I2C_SLAVE_INT_STOP      0x00000004  // Stop Condition Interrupt
#define I2C_SLAVE_INT_START     0x00000002  // Start Condition Interrupt
#define I2C_SLAVE_INT_DATA      0x00000001  // Data Interrupt

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern void I2CIntRegister(uint32_t ui32Base, void(pfnHandler)(void));
extern void I2CIntUnregister(uint32_t ui32Base);
extern bool I2CMasterBusBusy(uint32_t ui32Base);
extern bool I2CMasterBusy(uint32_t ui32Base);
extern void I2CMasterControl(uint32_t ui32Base, uint32_t ui32Cmd);
extern uint32_t I2CMasterDataGet(uint32_t ui32Base);
extern void I2CMasterDataPut(uint32_t ui32Base, uint8_t ui8Data);
extern void I2CMasterDisable(uint32_t ui32Base);
extern void I2CMasterEnable(uint32_t ui32Base);
extern uint32_t I2CMasterErr(uint32_t ui32Base);
extern void I2CMasterInitExpClk(uint32_t ui32Base, uint32_t ui32I2CClk,
                                bool bFast);
extern void I2CMasterIntClear(uint32_t ui32Base);
extern void I2CMasterIntDisable(uint32_t ui32Base);
extern void I2CMasterIntEnable(uint32_t ui32Base);
extern bool I2CMasterIntStatus(uint32_t ui32Base, bool bMasked);
extern void I2CMasterIntEnableEx(uint32_t ui32Base,
                                 uint32_t ui32IntFlags);
extern void I2CMasterIntDisableEx(uint32_t ui32Base,
                                  uint32_t ui32IntFlags);
extern uint32_t I2CMasterIntStatusEx(uint32_t ui32Base,
                                       bool bMasked);
extern void I2CMasterIntClearEx(uint32_t ui32Base,
                                uint32_t ui32IntFlags);
extern void I2CMasterTimeoutSet(uint32_t ui32Base, uint32_t ui32Value);
extern void I2CSlaveACKOverride(uint32_t ui32Base, bool bEnable);
extern void I2CSlaveACKValueSet(uint32_t ui32Base, bool bACK);
extern uint32_t I2CMasterLineStateGet(uint32_t ui32Base);
extern void I2CMasterSlaveAddrSet(uint32_t ui32Base,
                                  uint8_t ui8SlaveAddr,
                                  bool bReceive);
extern uint32_t I2CSlaveDataGet(uint32_t ui32Base);
extern void I2CSlaveDataPut(uint32_t ui32Base, uint8_t ui8Data);
extern void I2CSlaveDisable(uint32_t ui32Base);
extern void I2CSlaveEnable(uint32_t ui32Base);
extern void I2CSlaveInit(uint32_t ui32Base, uint8_t ui8SlaveAddr);
extern void I2CSlaveAddressSet(uint32_t ui32Base, uint8_t ui8AddrNum,
                                 uint8_t ui8SlaveAddr);
extern void I2CSlaveIntClear(uint32_t ui32Base);
extern void I2CSlaveIntDisable(uint32_t ui32Base);
extern void I2CSlaveIntEnable(uint32_t ui32Base);
extern void I2CSlaveIntClearEx(uint32_t ui32Base, uint32_t ui32IntFlags);
extern void I2CSlaveIntDisableEx(uint32_t ui32Base,
                                 uint32_t ui32IntFlags);
extern void I2CSlaveIntEnableEx(uint32_t ui32Base, uint32_t ui32IntFlags);
extern bool I2CSlaveIntStatus(uint32_t ui32Base, bool bMasked);
extern uint32_t I2CSlaveIntStatusEx(uint32_t ui32Base,
                                      bool bMasked);
extern uint32_t I2CSlaveStatus(uint32_t ui32Base);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __DRIVERLIB_I2C_H__
