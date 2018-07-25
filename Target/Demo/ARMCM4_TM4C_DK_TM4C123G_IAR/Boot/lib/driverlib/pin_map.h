//*****************************************************************************
//
// pin_map.h - Mapping of peripherals to pins for all parts.
//
// Copyright (c) 2007-2013 Texas Instruments Incorporated.  All rights reserved.
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

#ifndef __DRIVERLIB_PIN_MAP_H__
#define __DRIVERLIB_PIN_MAP_H__

//*****************************************************************************
//
// TM4C1230C3PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1230C3PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#endif // PART_TM4C1230C3PM

//*****************************************************************************
//
// TM4C1230D5PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1230D5PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#endif // PART_TM4C1230D5PM

//*****************************************************************************
//
// TM4C1230E6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1230E6PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#endif // PART_TM4C1230E6PM

//*****************************************************************************
//
// TM4C1230H6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1230H6PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#endif // PART_TM4C1230H6PM

//*****************************************************************************
//
// TM4C1231C3PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1231C3PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#endif // PART_TM4C1231C3PM

//*****************************************************************************
//
// TM4C1231D5PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1231D5PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#endif // PART_TM4C1231D5PM

//*****************************************************************************
//
// TM4C1231D5PZ Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1231D5PZ

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE7_U1RI           0x00041C01

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_T2CCP1         0x00051407

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PK0_SSI3CLK        0x00090002

#define GPIO_PK1_SSI3FSS        0x00090402

#define GPIO_PK2_SSI3RX         0x00090802

#define GPIO_PK3_SSI3TX         0x00090C02

#endif // PART_TM4C1231D5PZ

//*****************************************************************************
//
// TM4C1231E6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1231E6PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#endif // PART_TM4C1231E6PM

//*****************************************************************************
//
// TM4C1231E6PZ Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1231E6PZ

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE7_U1RI           0x00041C01

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_T2CCP1         0x00051407

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PK0_SSI3CLK        0x00090002

#define GPIO_PK1_SSI3FSS        0x00090402

#define GPIO_PK2_SSI3RX         0x00090802

#define GPIO_PK3_SSI3TX         0x00090C02

#endif // PART_TM4C1231E6PZ

//*****************************************************************************
//
// TM4C1231H6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1231H6PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#endif // PART_TM4C1231H6PM

//*****************************************************************************
//
// TM4C1231H6PZ Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1231H6PZ

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE7_U1RI           0x00041C01

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_T2CCP1         0x00051407

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PK0_SSI3CLK        0x00090002

#define GPIO_PK1_SSI3FSS        0x00090402

#define GPIO_PK2_SSI3RX         0x00090802

#define GPIO_PK3_SSI3TX         0x00090C02

#endif // PART_TM4C1231H6PZ

//*****************************************************************************
//
// TM4C1232C3PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1232C3PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#endif // PART_TM4C1232C3PM

//*****************************************************************************
//
// TM4C1232D5PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1232D5PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#endif // PART_TM4C1232D5PM

//*****************************************************************************
//
// TM4C1232E6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1232E6PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#endif // PART_TM4C1232E6PM

//*****************************************************************************
//
// TM4C1232H6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1232H6PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#endif // PART_TM4C1232H6PM

//*****************************************************************************
//
// TM4C1233C3PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1233C3PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#endif // PART_TM4C1233C3PM

//*****************************************************************************
//
// TM4C1233D5PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1233D5PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#endif // PART_TM4C1233D5PM

//*****************************************************************************
//
// TM4C1233D5PZ Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1233D5PZ

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE7_U1RI           0x00041C01

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_T2CCP1         0x00051407

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PK0_SSI3CLK        0x00090002

#define GPIO_PK1_SSI3FSS        0x00090402

#define GPIO_PK2_SSI3RX         0x00090802

#define GPIO_PK3_SSI3TX         0x00090C02

#endif // PART_TM4C1233D5PZ

//*****************************************************************************
//
// TM4C1233E6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1233E6PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#endif // PART_TM4C1233E6PM

//*****************************************************************************
//
// TM4C1233E6PZ Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1233E6PZ

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE7_U1RI           0x00041C01

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_T2CCP1         0x00051407

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PK0_SSI3CLK        0x00090002

#define GPIO_PK1_SSI3FSS        0x00090402

#define GPIO_PK2_SSI3RX         0x00090802

#define GPIO_PK3_SSI3TX         0x00090C02

#endif // PART_TM4C1233E6PZ

//*****************************************************************************
//
// TM4C1233H6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1233H6PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#endif // PART_TM4C1233H6PM

//*****************************************************************************
//
// TM4C1233H6PZ Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1233H6PZ

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE7_U1RI           0x00041C01

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_T2CCP1         0x00051407

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PK0_SSI3CLK        0x00090002

#define GPIO_PK1_SSI3FSS        0x00090402

#define GPIO_PK2_SSI3RX         0x00090802

#define GPIO_PK3_SSI3TX         0x00090C02

#endif // PART_TM4C1233H6PZ

//*****************************************************************************
//
// TM4C1236D5PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1236D5PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007
#define GPIO_PG4_USB0EPEN       0x00061008

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407
#define GPIO_PG5_USB0PFLT       0x00061408

#endif // PART_TM4C1236D5PM

//*****************************************************************************
//
// TM4C1236E6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1236E6PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007
#define GPIO_PG4_USB0EPEN       0x00061008

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407
#define GPIO_PG5_USB0PFLT       0x00061408

#endif // PART_TM4C1236E6PM

//*****************************************************************************
//
// TM4C1236H6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1236H6PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007
#define GPIO_PG4_USB0EPEN       0x00061008

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407
#define GPIO_PG5_USB0PFLT       0x00061408

#endif // PART_TM4C1236H6PM

//*****************************************************************************
//
// TM4C1237D5PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1237D5PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008

#endif // PART_TM4C1237D5PM

//*****************************************************************************
//
// TM4C1237D5PZ Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1237D5PZ

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE7_U1RI           0x00041C01

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_T2CCP1         0x00051407
#define GPIO_PF5_USB0PFLT       0x00051408

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007
#define GPIO_PG4_USB0EPEN       0x00061008

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407
#define GPIO_PG5_USB0PFLT       0x00061408

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PK0_SSI3CLK        0x00090002

#define GPIO_PK1_SSI3FSS        0x00090402

#define GPIO_PK2_SSI3RX         0x00090802

#define GPIO_PK3_SSI3TX         0x00090C02

#endif // PART_TM4C1237D5PZ

//*****************************************************************************
//
// TM4C1237E6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1237E6PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008

#endif // PART_TM4C1237E6PM

//*****************************************************************************
//
// TM4C1237E6PZ Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1237E6PZ

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE7_U1RI           0x00041C01

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_T2CCP1         0x00051407
#define GPIO_PF5_USB0PFLT       0x00051408

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007
#define GPIO_PG4_USB0EPEN       0x00061008

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407
#define GPIO_PG5_USB0PFLT       0x00061408

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PK0_SSI3CLK        0x00090002

#define GPIO_PK1_SSI3FSS        0x00090402

#define GPIO_PK2_SSI3RX         0x00090802

#define GPIO_PK3_SSI3TX         0x00090C02

#endif // PART_TM4C1237E6PZ

//*****************************************************************************
//
// TM4C1237H6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1237H6PM

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008

#endif // PART_TM4C1237H6PM

//*****************************************************************************
//
// TM4C1237H6PZ Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1237H6PZ

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE7_U1RI           0x00041C01

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_T2CCP1         0x00051407
#define GPIO_PF5_USB0PFLT       0x00051408

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007
#define GPIO_PG4_USB0EPEN       0x00061008

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407
#define GPIO_PG5_USB0PFLT       0x00061408

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PK0_SSI3CLK        0x00090002

#define GPIO_PK1_SSI3FSS        0x00090402

#define GPIO_PK2_SSI3RX         0x00090802

#define GPIO_PK3_SSI3TX         0x00090C02

#endif // PART_TM4C1237H6PZ

//*****************************************************************************
//
// TM4C123AE6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123AE6PM

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_M0PWM0         0x00011804
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_M0PWM1         0x00011C04
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_M0FAULT1       0x00031C04
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M0FAULT1       0x00050C04
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_M0FAULT2       0x00051004
#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_M1FAULT1       0x00060005
#define GPIO_PG0_PHA1           0x00060006
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_M1FAULT2       0x00060405
#define GPIO_PG1_PHB1           0x00060406
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_M0FAULT1       0x00060804
#define GPIO_PG2_M1PWM0         0x00060805
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_M0FAULT2       0x00060C04
#define GPIO_PG3_M1PWM1         0x00060C05
#define GPIO_PG3_PHA1           0x00060C06
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_M0PWM4         0x00061004
#define GPIO_PG4_M1PWM2         0x00061005
#define GPIO_PG4_PHB1           0x00061006
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_M0PWM5         0x00061404
#define GPIO_PG5_M1PWM3         0x00061405
#define GPIO_PG5_IDX1           0x00061406
#define GPIO_PG5_WT0CCP1        0x00061407

#endif // PART_TM4C123AE6PM

//*****************************************************************************
//
// TM4C123AH6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123AH6PM

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_M0PWM0         0x00011804
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_M0PWM1         0x00011C04
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_M0FAULT1       0x00031C04
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M0FAULT1       0x00050C04
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_M0FAULT2       0x00051004
#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_M1FAULT1       0x00060005
#define GPIO_PG0_PHA1           0x00060006
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_M1FAULT2       0x00060405
#define GPIO_PG1_PHB1           0x00060406
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_M0FAULT1       0x00060804
#define GPIO_PG2_M1PWM0         0x00060805
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_M0FAULT2       0x00060C04
#define GPIO_PG3_M1PWM1         0x00060C05
#define GPIO_PG3_PHA1           0x00060C06
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_M0PWM4         0x00061004
#define GPIO_PG4_M1PWM2         0x00061005
#define GPIO_PG4_PHB1           0x00061006
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_M0PWM5         0x00061404
#define GPIO_PG5_M1PWM3         0x00061405
#define GPIO_PG5_IDX1           0x00061406
#define GPIO_PG5_WT0CCP1        0x00061407

#endif // PART_TM4C123AH6PM

//*****************************************************************************
//
// TM4C123BE6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123BE6PM

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_M0PWM0         0x00011804
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_M0PWM1         0x00011C04
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007

#endif // PART_TM4C123BE6PM

//*****************************************************************************
//
// TM4C123BE6PZ Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123BE6PZ

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_M0FAULT1       0x00031C04
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE6_CAN1RX         0x00041808

#define GPIO_PE7_U1RI           0x00041C01
#define GPIO_PE7_CAN1TX         0x00041C08

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M0FAULT1       0x00050C04
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_M0FAULT2       0x00051004
#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_M0FAULT3       0x00051404
#define GPIO_PF5_T2CCP1         0x00051407

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_M1FAULT0       0x00051C05
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_M1FAULT1       0x00060005
#define GPIO_PG0_PHA1           0x00060006
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_M1FAULT2       0x00060405
#define GPIO_PG1_PHB1           0x00060406
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_M0FAULT1       0x00060804
#define GPIO_PG2_M1PWM0         0x00060805
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_M0FAULT2       0x00060C04
#define GPIO_PG3_M1PWM1         0x00060C05
#define GPIO_PG3_PHA1           0x00060C06
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_M0PWM4         0x00061004
#define GPIO_PG4_M1PWM2         0x00061005
#define GPIO_PG4_PHB1           0x00061006
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_M0PWM5         0x00061404
#define GPIO_PG5_M1PWM3         0x00061405
#define GPIO_PG5_IDX1           0x00061406
#define GPIO_PG5_WT0CCP1        0x00061407

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_M0PWM6         0x00061804
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_M0PWM7         0x00061C04
#define GPIO_PG7_IDX1           0x00061C05
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_M0PWM0         0x00070004
#define GPIO_PH0_M0FAULT0       0x00070006
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_M0PWM1         0x00070404
#define GPIO_PH1_IDX0           0x00070405
#define GPIO_PH1_M0FAULT1       0x00070406
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_M0PWM2         0x00070804
#define GPIO_PH2_M0FAULT2       0x00070806
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_M0PWM3         0x00070C04
#define GPIO_PH3_M0FAULT3       0x00070C06
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_M0PWM4         0x00071004
#define GPIO_PH4_PHA0           0x00071005
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_M0PWM5         0x00071404
#define GPIO_PH5_PHB0           0x00071405
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_M0PWM6         0x00071804
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_M0PWM7         0x00071C04
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_IDX0           0x00080805
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PK0_SSI3CLK        0x00090002
#define GPIO_PK0_M1FAULT0       0x00090006

#define GPIO_PK1_SSI3FSS        0x00090402
#define GPIO_PK1_M1FAULT1       0x00090406

#define GPIO_PK2_SSI3RX         0x00090802
#define GPIO_PK2_M1FAULT2       0x00090806

#define GPIO_PK3_SSI3TX         0x00090C02
#define GPIO_PK3_M1FAULT3       0x00090C06

#endif // PART_TM4C123BE6PZ

//*****************************************************************************
//
// TM4C123BH6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123BH6PM

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_M0PWM0         0x00011804
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_M0PWM1         0x00011C04
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007

#endif // PART_TM4C123BH6PM

//*****************************************************************************
//
// TM4C123BH6PZ Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123BH6PZ

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_M0FAULT1       0x00031C04
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE6_CAN1RX         0x00041808

#define GPIO_PE7_U1RI           0x00041C01
#define GPIO_PE7_CAN1TX         0x00041C08

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M0FAULT1       0x00050C04
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_M0FAULT2       0x00051004
#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_M0FAULT3       0x00051404
#define GPIO_PF5_T2CCP1         0x00051407

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_M1FAULT0       0x00051C05
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_M1FAULT1       0x00060005
#define GPIO_PG0_PHA1           0x00060006
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_M1FAULT2       0x00060405
#define GPIO_PG1_PHB1           0x00060406
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_M0FAULT1       0x00060804
#define GPIO_PG2_M1PWM0         0x00060805
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_M0FAULT2       0x00060C04
#define GPIO_PG3_M1PWM1         0x00060C05
#define GPIO_PG3_PHA1           0x00060C06
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_M0PWM4         0x00061004
#define GPIO_PG4_M1PWM2         0x00061005
#define GPIO_PG4_PHB1           0x00061006
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_M0PWM5         0x00061404
#define GPIO_PG5_M1PWM3         0x00061405
#define GPIO_PG5_IDX1           0x00061406
#define GPIO_PG5_WT0CCP1        0x00061407

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_M0PWM6         0x00061804
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_M0PWM7         0x00061C04
#define GPIO_PG7_IDX1           0x00061C05
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_M0PWM0         0x00070004
#define GPIO_PH0_M0FAULT0       0x00070006
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_M0PWM1         0x00070404
#define GPIO_PH1_IDX0           0x00070405
#define GPIO_PH1_M0FAULT1       0x00070406
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_M0PWM2         0x00070804
#define GPIO_PH2_M0FAULT2       0x00070806
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_M0PWM3         0x00070C04
#define GPIO_PH3_M0FAULT3       0x00070C06
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_M0PWM4         0x00071004
#define GPIO_PH4_PHA0           0x00071005
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_M0PWM5         0x00071404
#define GPIO_PH5_PHB0           0x00071405
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_M0PWM6         0x00071804
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_M0PWM7         0x00071C04
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_IDX0           0x00080805
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PK0_SSI3CLK        0x00090002
#define GPIO_PK0_M1FAULT0       0x00090006

#define GPIO_PK1_SSI3FSS        0x00090402
#define GPIO_PK1_M1FAULT1       0x00090406

#define GPIO_PK2_SSI3RX         0x00090802
#define GPIO_PK2_M1FAULT2       0x00090806

#define GPIO_PK3_SSI3TX         0x00090C02
#define GPIO_PK3_M1FAULT3       0x00090C06

#endif // PART_TM4C123BH6PZ

//*****************************************************************************
//
// TM4C123FE6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123FE6PM

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_M0PWM0         0x00011804
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_M0PWM1         0x00011C04
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_M0FAULT1       0x00031C04
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M0FAULT1       0x00050C04
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_M0FAULT2       0x00051004
#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_M1FAULT1       0x00060005
#define GPIO_PG0_PHA1           0x00060006
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_M1FAULT2       0x00060405
#define GPIO_PG1_PHB1           0x00060406
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_M0FAULT1       0x00060804
#define GPIO_PG2_M1PWM0         0x00060805
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_M0FAULT2       0x00060C04
#define GPIO_PG3_M1PWM1         0x00060C05
#define GPIO_PG3_PHA1           0x00060C06
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_M0PWM4         0x00061004
#define GPIO_PG4_M1PWM2         0x00061005
#define GPIO_PG4_PHB1           0x00061006
#define GPIO_PG4_WT0CCP0        0x00061007
#define GPIO_PG4_USB0EPEN       0x00061008

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_M0PWM5         0x00061404
#define GPIO_PG5_M1PWM3         0x00061405
#define GPIO_PG5_IDX1           0x00061406
#define GPIO_PG5_WT0CCP1        0x00061407
#define GPIO_PG5_USB0PFLT       0x00061408

#endif // PART_TM4C123FE6PM

//*****************************************************************************
//
// TM4C123FH6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123FH6PM

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_M0PWM0         0x00011804
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_M0PWM1         0x00011C04
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_M0FAULT1       0x00031C04
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M0FAULT1       0x00050C04
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_M0FAULT2       0x00051004
#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_M1FAULT1       0x00060005
#define GPIO_PG0_PHA1           0x00060006
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_M1FAULT2       0x00060405
#define GPIO_PG1_PHB1           0x00060406
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_M0FAULT1       0x00060804
#define GPIO_PG2_M1PWM0         0x00060805
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_M0FAULT2       0x00060C04
#define GPIO_PG3_M1PWM1         0x00060C05
#define GPIO_PG3_PHA1           0x00060C06
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_M0PWM4         0x00061004
#define GPIO_PG4_M1PWM2         0x00061005
#define GPIO_PG4_PHB1           0x00061006
#define GPIO_PG4_WT0CCP0        0x00061007
#define GPIO_PG4_USB0EPEN       0x00061008

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_M0PWM5         0x00061404
#define GPIO_PG5_M1PWM3         0x00061405
#define GPIO_PG5_IDX1           0x00061406
#define GPIO_PG5_WT0CCP1        0x00061407
#define GPIO_PG5_USB0PFLT       0x00061408

#endif // PART_TM4C123FH6PM

//*****************************************************************************
//
// TM4C123GE6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123GE6PM

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_M0PWM0         0x00011804
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_M0PWM1         0x00011C04
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008

#endif // PART_TM4C123GE6PM

//*****************************************************************************
//
// TM4C123GE6PZ Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123GE6PZ

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_M0FAULT1       0x00031C04
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE6_CAN1RX         0x00041808

#define GPIO_PE7_U1RI           0x00041C01
#define GPIO_PE7_CAN1TX         0x00041C08

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M0FAULT1       0x00050C04
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_M0FAULT2       0x00051004
#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_M0FAULT3       0x00051404
#define GPIO_PF5_T2CCP1         0x00051407
#define GPIO_PF5_USB0PFLT       0x00051408

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_M1FAULT0       0x00051C05
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_M1FAULT1       0x00060005
#define GPIO_PG0_PHA1           0x00060006
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_M1FAULT2       0x00060405
#define GPIO_PG1_PHB1           0x00060406
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_M0FAULT1       0x00060804
#define GPIO_PG2_M1PWM0         0x00060805
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_M0FAULT2       0x00060C04
#define GPIO_PG3_M1PWM1         0x00060C05
#define GPIO_PG3_PHA1           0x00060C06
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_M0PWM4         0x00061004
#define GPIO_PG4_M1PWM2         0x00061005
#define GPIO_PG4_PHB1           0x00061006
#define GPIO_PG4_WT0CCP0        0x00061007
#define GPIO_PG4_USB0EPEN       0x00061008

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_M0PWM5         0x00061404
#define GPIO_PG5_M1PWM3         0x00061405
#define GPIO_PG5_IDX1           0x00061406
#define GPIO_PG5_WT0CCP1        0x00061407
#define GPIO_PG5_USB0PFLT       0x00061408

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_M0PWM6         0x00061804
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_M0PWM7         0x00061C04
#define GPIO_PG7_IDX1           0x00061C05
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_M0PWM0         0x00070004
#define GPIO_PH0_M0FAULT0       0x00070006
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_M0PWM1         0x00070404
#define GPIO_PH1_IDX0           0x00070405
#define GPIO_PH1_M0FAULT1       0x00070406
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_M0PWM2         0x00070804
#define GPIO_PH2_M0FAULT2       0x00070806
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_M0PWM3         0x00070C04
#define GPIO_PH3_M0FAULT3       0x00070C06
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_M0PWM4         0x00071004
#define GPIO_PH4_PHA0           0x00071005
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_M0PWM5         0x00071404
#define GPIO_PH5_PHB0           0x00071405
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_M0PWM6         0x00071804
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_M0PWM7         0x00071C04
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_IDX0           0x00080805
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PK0_SSI3CLK        0x00090002
#define GPIO_PK0_M1FAULT0       0x00090006

#define GPIO_PK1_SSI3FSS        0x00090402
#define GPIO_PK1_M1FAULT1       0x00090406

#define GPIO_PK2_SSI3RX         0x00090802
#define GPIO_PK2_M1FAULT2       0x00090806

#define GPIO_PK3_SSI3TX         0x00090C02
#define GPIO_PK3_M1FAULT3       0x00090C06

#endif // PART_TM4C123GE6PZ

//*****************************************************************************
//
// TM4C123GH6PM Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123GH6PM

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_M0PWM0         0x00011804
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_M0PWM1         0x00011C04
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008

#endif // PART_TM4C123GH6PM

//*****************************************************************************
//
// TM4C123GH6PZ Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123GH6PZ

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_M0FAULT1       0x00031C04
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE6_CAN1RX         0x00041808

#define GPIO_PE7_U1RI           0x00041C01
#define GPIO_PE7_CAN1TX         0x00041C08

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M0FAULT1       0x00050C04
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_M0FAULT2       0x00051004
#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_M0FAULT3       0x00051404
#define GPIO_PF5_T2CCP1         0x00051407
#define GPIO_PF5_USB0PFLT       0x00051408

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_M1FAULT0       0x00051C05
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_M1FAULT1       0x00060005
#define GPIO_PG0_PHA1           0x00060006
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_M1FAULT2       0x00060405
#define GPIO_PG1_PHB1           0x00060406
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_M0FAULT1       0x00060804
#define GPIO_PG2_M1PWM0         0x00060805
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_M0FAULT2       0x00060C04
#define GPIO_PG3_M1PWM1         0x00060C05
#define GPIO_PG3_PHA1           0x00060C06
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_M0PWM4         0x00061004
#define GPIO_PG4_M1PWM2         0x00061005
#define GPIO_PG4_PHB1           0x00061006
#define GPIO_PG4_WT0CCP0        0x00061007
#define GPIO_PG4_USB0EPEN       0x00061008

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_M0PWM5         0x00061404
#define GPIO_PG5_M1PWM3         0x00061405
#define GPIO_PG5_IDX1           0x00061406
#define GPIO_PG5_WT0CCP1        0x00061407
#define GPIO_PG5_USB0PFLT       0x00061408

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_M0PWM6         0x00061804
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_M0PWM7         0x00061C04
#define GPIO_PG7_IDX1           0x00061C05
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_M0PWM0         0x00070004
#define GPIO_PH0_M0FAULT0       0x00070006
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_M0PWM1         0x00070404
#define GPIO_PH1_IDX0           0x00070405
#define GPIO_PH1_M0FAULT1       0x00070406
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_M0PWM2         0x00070804
#define GPIO_PH2_M0FAULT2       0x00070806
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_M0PWM3         0x00070C04
#define GPIO_PH3_M0FAULT3       0x00070C06
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_M0PWM4         0x00071004
#define GPIO_PH4_PHA0           0x00071005
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_M0PWM5         0x00071404
#define GPIO_PH5_PHB0           0x00071405
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_M0PWM6         0x00071804
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_M0PWM7         0x00071C04
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_IDX0           0x00080805
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PK0_SSI3CLK        0x00090002
#define GPIO_PK0_M1FAULT0       0x00090006

#define GPIO_PK1_SSI3FSS        0x00090402
#define GPIO_PK1_M1FAULT1       0x00090406

#define GPIO_PK2_SSI3RX         0x00090802
#define GPIO_PK2_M1FAULT2       0x00090806

#define GPIO_PK3_SSI3TX         0x00090C02
#define GPIO_PK3_M1FAULT3       0x00090C06

#endif // PART_TM4C123GH6PZ

//*****************************************************************************
//
// TM4C1231H6PGE Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1231H6PGE

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE7_U1RI           0x00041C01

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_T2CCP1         0x00051407

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PJ3_U5TX           0x00080C01
#define GPIO_PJ3_T2CCP1         0x00080C07

#define GPIO_PJ4_U6RX           0x00081001
#define GPIO_PJ4_T3CCP0         0x00081007

#define GPIO_PJ5_U6TX           0x00081401
#define GPIO_PJ5_T3CCP1         0x00081407

#define GPIO_PK0_SSI3CLK        0x00090002

#define GPIO_PK1_SSI3FSS        0x00090402

#define GPIO_PK2_SSI3RX         0x00090802

#define GPIO_PK3_SSI3TX         0x00090C02

#define GPIO_PK4_U7RX           0x00091001
#define GPIO_PK4_RTCCLK         0x00091007
#define GPIO_PK4_C0O            0x00091008

#define GPIO_PK5_U7TX           0x00091401
#define GPIO_PK5_C1O            0x00091408

#define GPIO_PK6_WT1CCP0        0x00091807
#define GPIO_PK6_C2O            0x00091808

#define GPIO_PK7_WT1CCP1        0x00091C07

#define GPIO_PL0_T0CCP0         0x000A0007
#define GPIO_PL0_WT0CCP0        0x000A0008

#define GPIO_PL1_T0CCP1         0x000A0407
#define GPIO_PL1_WT0CCP1        0x000A0408

#define GPIO_PL2_T1CCP0         0x000A0807
#define GPIO_PL2_WT1CCP0        0x000A0808

#define GPIO_PL3_T1CCP1         0x000A0C07
#define GPIO_PL3_WT1CCP1        0x000A0C08

#define GPIO_PL4_T2CCP0         0x000A1007
#define GPIO_PL4_WT2CCP0        0x000A1008

#define GPIO_PL5_T2CCP1         0x000A1407
#define GPIO_PL5_WT2CCP1        0x000A1408

#define GPIO_PL6_T3CCP0         0x000A1807
#define GPIO_PL6_WT3CCP0        0x000A1808

#define GPIO_PL7_T3CCP1         0x000A1C07
#define GPIO_PL7_WT3CCP1        0x000A1C08

#define GPIO_PM0_T4CCP0         0x000B0007
#define GPIO_PM0_WT4CCP0        0x000B0008

#define GPIO_PM1_T4CCP1         0x000B0407
#define GPIO_PM1_WT4CCP1        0x000B0408

#define GPIO_PM2_T5CCP0         0x000B0807
#define GPIO_PM2_WT5CCP0        0x000B0808

#define GPIO_PM3_T5CCP1         0x000B0C07
#define GPIO_PM3_WT5CCP1        0x000B0C08

#define GPIO_PM6_WT0CCP0        0x000B1807

#define GPIO_PM7_WT0CCP1        0x000B1C07

#define GPIO_PN0_CAN0RX         0x000C0001

#define GPIO_PN1_CAN0TX         0x000C0401

#define GPIO_PN2_WT2CCP0        0x000C0807

#define GPIO_PN3_WT2CCP1        0x000C0C07

#define GPIO_PN4_WT3CCP0        0x000C1007

#define GPIO_PN5_WT3CCP1        0x000C1407

#define GPIO_PN6_WT4CCP0        0x000C1807

#define GPIO_PN7_WT4CCP1        0x000C1C07

#define GPIO_PP0_T4CCP0         0x000D0007

#define GPIO_PP1_T4CCP1         0x000D0407

#define GPIO_PP2_T5CCP0         0x000D0807

#endif // PART_TM4C1231H6PGE

//*****************************************************************************
//
// TM4C1233H6PGE Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1233H6PGE

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE7_U1RI           0x00041C01

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_T2CCP1         0x00051407

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PJ3_U5TX           0x00080C01
#define GPIO_PJ3_T2CCP1         0x00080C07

#define GPIO_PJ4_U6RX           0x00081001
#define GPIO_PJ4_T3CCP0         0x00081007

#define GPIO_PJ5_U6TX           0x00081401
#define GPIO_PJ5_T3CCP1         0x00081407

#define GPIO_PK0_SSI3CLK        0x00090002

#define GPIO_PK1_SSI3FSS        0x00090402

#define GPIO_PK2_SSI3RX         0x00090802

#define GPIO_PK3_SSI3TX         0x00090C02

#define GPIO_PK4_U7RX           0x00091001
#define GPIO_PK4_RTCCLK         0x00091007
#define GPIO_PK4_C0O            0x00091008

#define GPIO_PK5_U7TX           0x00091401
#define GPIO_PK5_C1O            0x00091408

#define GPIO_PK6_WT1CCP0        0x00091807
#define GPIO_PK6_C2O            0x00091808

#define GPIO_PK7_WT1CCP1        0x00091C07

#define GPIO_PL0_T0CCP0         0x000A0007
#define GPIO_PL0_WT0CCP0        0x000A0008

#define GPIO_PL1_T0CCP1         0x000A0407
#define GPIO_PL1_WT0CCP1        0x000A0408

#define GPIO_PL2_T1CCP0         0x000A0807
#define GPIO_PL2_WT1CCP0        0x000A0808

#define GPIO_PL3_T1CCP1         0x000A0C07
#define GPIO_PL3_WT1CCP1        0x000A0C08

#define GPIO_PL4_T2CCP0         0x000A1007
#define GPIO_PL4_WT2CCP0        0x000A1008

#define GPIO_PL5_T2CCP1         0x000A1407
#define GPIO_PL5_WT2CCP1        0x000A1408

#define GPIO_PL6_T3CCP0         0x000A1807
#define GPIO_PL6_WT3CCP0        0x000A1808

#define GPIO_PL7_T3CCP1         0x000A1C07
#define GPIO_PL7_WT3CCP1        0x000A1C08

#define GPIO_PM0_T4CCP0         0x000B0007
#define GPIO_PM0_WT4CCP0        0x000B0008

#define GPIO_PM1_T4CCP1         0x000B0407
#define GPIO_PM1_WT4CCP1        0x000B0408

#define GPIO_PM2_T5CCP0         0x000B0807
#define GPIO_PM2_WT5CCP0        0x000B0808

#define GPIO_PM3_T5CCP1         0x000B0C07
#define GPIO_PM3_WT5CCP1        0x000B0C08

#define GPIO_PM6_WT0CCP0        0x000B1807

#define GPIO_PM7_WT0CCP1        0x000B1C07

#define GPIO_PN0_CAN0RX         0x000C0001

#define GPIO_PN1_CAN0TX         0x000C0401

#define GPIO_PN2_WT2CCP0        0x000C0807

#define GPIO_PN3_WT2CCP1        0x000C0C07

#define GPIO_PN4_WT3CCP0        0x000C1007

#define GPIO_PN5_WT3CCP1        0x000C1407

#define GPIO_PN6_WT4CCP0        0x000C1807

#define GPIO_PN7_WT4CCP1        0x000C1C07

#define GPIO_PP0_T4CCP0         0x000D0007

#define GPIO_PP1_T4CCP1         0x000D0407

#define GPIO_PP2_T5CCP0         0x000D0807

#endif // PART_TM4C1233H6PGE

//*****************************************************************************
//
// TM4C1237H6PGE Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C1237H6PGE

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE7_U1RI           0x00041C01

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_T2CCP1         0x00051407
#define GPIO_PF5_USB0PFLT       0x00051408

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_WT0CCP0        0x00061007
#define GPIO_PG4_USB0EPEN       0x00061008

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_WT0CCP1        0x00061407
#define GPIO_PG5_USB0PFLT       0x00061408

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PJ3_U5TX           0x00080C01
#define GPIO_PJ3_T2CCP1         0x00080C07

#define GPIO_PJ4_U6RX           0x00081001
#define GPIO_PJ4_T3CCP0         0x00081007

#define GPIO_PJ5_U6TX           0x00081401
#define GPIO_PJ5_T3CCP1         0x00081407

#define GPIO_PK0_SSI3CLK        0x00090002

#define GPIO_PK1_SSI3FSS        0x00090402

#define GPIO_PK2_SSI3RX         0x00090802

#define GPIO_PK3_SSI3TX         0x00090C02

#define GPIO_PK4_U7RX           0x00091001
#define GPIO_PK4_RTCCLK         0x00091007
#define GPIO_PK4_C0O            0x00091008

#define GPIO_PK5_U7TX           0x00091401
#define GPIO_PK5_C1O            0x00091408

#define GPIO_PK6_WT1CCP0        0x00091807
#define GPIO_PK6_C2O            0x00091808

#define GPIO_PK7_WT1CCP1        0x00091C07

#define GPIO_PL0_T0CCP0         0x000A0007
#define GPIO_PL0_WT0CCP0        0x000A0008

#define GPIO_PL1_T0CCP1         0x000A0407
#define GPIO_PL1_WT0CCP1        0x000A0408

#define GPIO_PL2_T1CCP0         0x000A0807
#define GPIO_PL2_WT1CCP0        0x000A0808

#define GPIO_PL3_T1CCP1         0x000A0C07
#define GPIO_PL3_WT1CCP1        0x000A0C08

#define GPIO_PL4_T2CCP0         0x000A1007
#define GPIO_PL4_WT2CCP0        0x000A1008

#define GPIO_PL5_T2CCP1         0x000A1407
#define GPIO_PL5_WT2CCP1        0x000A1408

#define GPIO_PL6_T3CCP0         0x000A1807
#define GPIO_PL6_WT3CCP0        0x000A1808

#define GPIO_PL7_T3CCP1         0x000A1C07
#define GPIO_PL7_WT3CCP1        0x000A1C08

#define GPIO_PM0_T4CCP0         0x000B0007
#define GPIO_PM0_WT4CCP0        0x000B0008

#define GPIO_PM1_T4CCP1         0x000B0407
#define GPIO_PM1_WT4CCP1        0x000B0408

#define GPIO_PM2_T5CCP0         0x000B0807
#define GPIO_PM2_WT5CCP0        0x000B0808

#define GPIO_PM3_T5CCP1         0x000B0C07
#define GPIO_PM3_WT5CCP1        0x000B0C08

#define GPIO_PM6_WT0CCP0        0x000B1807

#define GPIO_PM7_WT0CCP1        0x000B1C07

#define GPIO_PN0_CAN0RX         0x000C0001

#define GPIO_PN1_CAN0TX         0x000C0401

#define GPIO_PN2_WT2CCP0        0x000C0807

#define GPIO_PN3_WT2CCP1        0x000C0C07

#define GPIO_PN4_WT3CCP0        0x000C1007

#define GPIO_PN5_WT3CCP1        0x000C1407

#define GPIO_PN6_WT4CCP0        0x000C1807

#define GPIO_PN7_WT4CCP1        0x000C1C07

#define GPIO_PP0_T4CCP0         0x000D0007

#define GPIO_PP1_T4CCP1         0x000D0407

#define GPIO_PP2_T5CCP0         0x000D0807

#endif // PART_TM4C1237H6PGE

//*****************************************************************************
//
// TM4C123BH6PGE Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123BH6PGE

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_M0FAULT1       0x00031C04
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE6_CAN1RX         0x00041808

#define GPIO_PE7_U1RI           0x00041C01
#define GPIO_PE7_CAN1TX         0x00041C08

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M0FAULT1       0x00050C04
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_M0FAULT2       0x00051004
#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_M0FAULT3       0x00051404
#define GPIO_PF5_T2CCP1         0x00051407

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_M1FAULT0       0x00051C05
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_M1FAULT1       0x00060005
#define GPIO_PG0_PHA1           0x00060006
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_M1FAULT2       0x00060405
#define GPIO_PG1_PHB1           0x00060406
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_M0FAULT1       0x00060804
#define GPIO_PG2_M1PWM0         0x00060805
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_M0FAULT2       0x00060C04
#define GPIO_PG3_M1PWM1         0x00060C05
#define GPIO_PG3_PHA1           0x00060C06
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_M0PWM4         0x00061004
#define GPIO_PG4_M1PWM2         0x00061005
#define GPIO_PG4_PHB1           0x00061006
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_M0PWM5         0x00061404
#define GPIO_PG5_M1PWM3         0x00061405
#define GPIO_PG5_IDX1           0x00061406
#define GPIO_PG5_WT0CCP1        0x00061407

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_M0PWM6         0x00061804
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_M0PWM7         0x00061C04
#define GPIO_PG7_IDX1           0x00061C05
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_M0PWM0         0x00070004
#define GPIO_PH0_M0FAULT0       0x00070006
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_M0PWM1         0x00070404
#define GPIO_PH1_IDX0           0x00070405
#define GPIO_PH1_M0FAULT1       0x00070406
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_M0PWM2         0x00070804
#define GPIO_PH2_M0FAULT2       0x00070806
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_M0PWM3         0x00070C04
#define GPIO_PH3_M0FAULT3       0x00070C06
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_M0PWM4         0x00071004
#define GPIO_PH4_PHA0           0x00071005
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_M0PWM5         0x00071404
#define GPIO_PH5_PHB0           0x00071405
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_M0PWM6         0x00071804
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_M0PWM7         0x00071C04
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_IDX0           0x00080805
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PJ3_U5TX           0x00080C01
#define GPIO_PJ3_T2CCP1         0x00080C07

#define GPIO_PJ4_U6RX           0x00081001
#define GPIO_PJ4_T3CCP0         0x00081007

#define GPIO_PJ5_U6TX           0x00081401
#define GPIO_PJ5_T3CCP1         0x00081407

#define GPIO_PK0_SSI3CLK        0x00090002
#define GPIO_PK0_M1FAULT0       0x00090006

#define GPIO_PK1_SSI3FSS        0x00090402
#define GPIO_PK1_M1FAULT1       0x00090406

#define GPIO_PK2_SSI3RX         0x00090802
#define GPIO_PK2_M1FAULT2       0x00090806

#define GPIO_PK3_SSI3TX         0x00090C02
#define GPIO_PK3_M1FAULT3       0x00090C06

#define GPIO_PK4_U7RX           0x00091001
#define GPIO_PK4_M0FAULT0       0x00091006
#define GPIO_PK4_RTCCLK         0x00091007
#define GPIO_PK4_C0O            0x00091008

#define GPIO_PK5_U7TX           0x00091401
#define GPIO_PK5_M0FAULT1       0x00091406
#define GPIO_PK5_C1O            0x00091408

#define GPIO_PK6_M0FAULT2       0x00091806
#define GPIO_PK6_WT1CCP0        0x00091807
#define GPIO_PK6_C2O            0x00091808

#define GPIO_PK7_M0FAULT3       0x00091C06
#define GPIO_PK7_WT1CCP1        0x00091C07

#define GPIO_PL0_T0CCP0         0x000A0007
#define GPIO_PL0_WT0CCP0        0x000A0008

#define GPIO_PL1_T0CCP1         0x000A0407
#define GPIO_PL1_WT0CCP1        0x000A0408

#define GPIO_PL2_T1CCP0         0x000A0807
#define GPIO_PL2_WT1CCP0        0x000A0808

#define GPIO_PL3_T1CCP1         0x000A0C07
#define GPIO_PL3_WT1CCP1        0x000A0C08

#define GPIO_PL4_T2CCP0         0x000A1007
#define GPIO_PL4_WT2CCP0        0x000A1008

#define GPIO_PL5_T2CCP1         0x000A1407
#define GPIO_PL5_WT2CCP1        0x000A1408

#define GPIO_PL6_T3CCP0         0x000A1807
#define GPIO_PL6_WT3CCP0        0x000A1808

#define GPIO_PL7_T3CCP1         0x000A1C07
#define GPIO_PL7_WT3CCP1        0x000A1C08

#define GPIO_PM0_T4CCP0         0x000B0007
#define GPIO_PM0_WT4CCP0        0x000B0008

#define GPIO_PM1_T4CCP1         0x000B0407
#define GPIO_PM1_WT4CCP1        0x000B0408

#define GPIO_PM2_T5CCP0         0x000B0807
#define GPIO_PM2_WT5CCP0        0x000B0808

#define GPIO_PM3_T5CCP1         0x000B0C07
#define GPIO_PM3_WT5CCP1        0x000B0C08

#define GPIO_PM6_M0PWM4         0x000B1802
#define GPIO_PM6_WT0CCP0        0x000B1807

#define GPIO_PM7_M0PWM5         0x000B1C02
#define GPIO_PM7_WT0CCP1        0x000B1C07

#define GPIO_PN0_CAN0RX         0x000C0001

#define GPIO_PN1_CAN0TX         0x000C0401

#define GPIO_PN2_M0PWM6         0x000C0802
#define GPIO_PN2_WT2CCP0        0x000C0807

#define GPIO_PN3_M0PWM7         0x000C0C02
#define GPIO_PN3_WT2CCP1        0x000C0C07

#define GPIO_PN4_M1PWM4         0x000C1002
#define GPIO_PN4_WT3CCP0        0x000C1007

#define GPIO_PN5_M1PWM5         0x000C1402
#define GPIO_PN5_WT3CCP1        0x000C1407

#define GPIO_PN6_M1PWM6         0x000C1802
#define GPIO_PN6_WT4CCP0        0x000C1807

#define GPIO_PN7_M1PWM7         0x000C1C02
#define GPIO_PN7_WT4CCP1        0x000C1C07

#define GPIO_PP0_M0PWM0         0x000D0001
#define GPIO_PP0_T4CCP0         0x000D0007

#define GPIO_PP1_M0PWM1         0x000D0401
#define GPIO_PP1_T4CCP1         0x000D0407

#define GPIO_PP2_M0PWM2         0x000D0801
#define GPIO_PP2_T5CCP0         0x000D0807

#endif // PART_TM4C123BH6PGE

//*****************************************************************************
//
// TM4C123BH6ZRB Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123BH6ZRB

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_M0PWM0         0x00011804
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_M0PWM1         0x00011C04
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_M0FAULT1       0x00031C04
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE6_CAN1RX         0x00041808

#define GPIO_PE7_U1RI           0x00041C01
#define GPIO_PE7_CAN1TX         0x00041C08

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M0FAULT1       0x00050C04
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_M0FAULT2       0x00051004
#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_M0FAULT3       0x00051404
#define GPIO_PF5_T2CCP1         0x00051407

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_M1FAULT0       0x00051C05
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_M1FAULT1       0x00060005
#define GPIO_PG0_PHA1           0x00060006
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_M1FAULT2       0x00060405
#define GPIO_PG1_PHB1           0x00060406
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_M0FAULT1       0x00060804
#define GPIO_PG2_M1PWM0         0x00060805
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_M0FAULT2       0x00060C04
#define GPIO_PG3_M1PWM1         0x00060C05
#define GPIO_PG3_PHA1           0x00060C06
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_M0PWM4         0x00061004
#define GPIO_PG4_M1PWM2         0x00061005
#define GPIO_PG4_PHB1           0x00061006
#define GPIO_PG4_WT0CCP0        0x00061007

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_M0PWM5         0x00061404
#define GPIO_PG5_M1PWM3         0x00061405
#define GPIO_PG5_IDX1           0x00061406
#define GPIO_PG5_WT0CCP1        0x00061407

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_M0PWM6         0x00061804
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_M0PWM7         0x00061C04
#define GPIO_PG7_IDX1           0x00061C05
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_M0PWM0         0x00070004
#define GPIO_PH0_M0FAULT0       0x00070006
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_M0PWM1         0x00070404
#define GPIO_PH1_IDX0           0x00070405
#define GPIO_PH1_M0FAULT1       0x00070406
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_M0PWM2         0x00070804
#define GPIO_PH2_M0FAULT2       0x00070806
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_M0PWM3         0x00070C04
#define GPIO_PH3_M0FAULT3       0x00070C06
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_M0PWM4         0x00071004
#define GPIO_PH4_PHA0           0x00071005
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_M0PWM5         0x00071404
#define GPIO_PH5_PHB0           0x00071405
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_M0PWM6         0x00071804
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_M0PWM7         0x00071C04
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_IDX0           0x00080805
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PJ3_U5TX           0x00080C01
#define GPIO_PJ3_T2CCP1         0x00080C07

#define GPIO_PJ4_U6RX           0x00081001
#define GPIO_PJ4_T3CCP0         0x00081007

#define GPIO_PJ5_U6TX           0x00081401
#define GPIO_PJ5_T3CCP1         0x00081407

#define GPIO_PK0_SSI3CLK        0x00090002
#define GPIO_PK0_M1FAULT0       0x00090006

#define GPIO_PK1_SSI3FSS        0x00090402
#define GPIO_PK1_M1FAULT1       0x00090406

#define GPIO_PK2_SSI3RX         0x00090802
#define GPIO_PK2_M1FAULT2       0x00090806

#define GPIO_PK3_SSI3TX         0x00090C02
#define GPIO_PK3_M1FAULT3       0x00090C06

#define GPIO_PK4_U7RX           0x00091001
#define GPIO_PK4_M0FAULT0       0x00091006
#define GPIO_PK4_RTCCLK         0x00091007
#define GPIO_PK4_C0O            0x00091008

#define GPIO_PK5_U7TX           0x00091401
#define GPIO_PK5_M0FAULT1       0x00091406
#define GPIO_PK5_C1O            0x00091408

#define GPIO_PK6_M0FAULT2       0x00091806
#define GPIO_PK6_WT1CCP0        0x00091807
#define GPIO_PK6_C2O            0x00091808

#define GPIO_PK7_M0FAULT3       0x00091C06
#define GPIO_PK7_WT1CCP1        0x00091C07

#define GPIO_PL0_T0CCP0         0x000A0007
#define GPIO_PL0_WT0CCP0        0x000A0008

#define GPIO_PL1_T0CCP1         0x000A0407
#define GPIO_PL1_WT0CCP1        0x000A0408

#define GPIO_PL2_T1CCP0         0x000A0807
#define GPIO_PL2_WT1CCP0        0x000A0808

#define GPIO_PL3_T1CCP1         0x000A0C07
#define GPIO_PL3_WT1CCP1        0x000A0C08

#define GPIO_PL4_T2CCP0         0x000A1007
#define GPIO_PL4_WT2CCP0        0x000A1008

#define GPIO_PL5_T2CCP1         0x000A1407
#define GPIO_PL5_WT2CCP1        0x000A1408

#define GPIO_PL6_T3CCP0         0x000A1807
#define GPIO_PL6_WT3CCP0        0x000A1808

#define GPIO_PL7_T3CCP1         0x000A1C07
#define GPIO_PL7_WT3CCP1        0x000A1C08

#define GPIO_PM0_T4CCP0         0x000B0007
#define GPIO_PM0_WT4CCP0        0x000B0008

#define GPIO_PM1_T4CCP1         0x000B0407
#define GPIO_PM1_WT4CCP1        0x000B0408

#define GPIO_PM2_T5CCP0         0x000B0807
#define GPIO_PM2_WT5CCP0        0x000B0808

#define GPIO_PM3_T5CCP1         0x000B0C07
#define GPIO_PM3_WT5CCP1        0x000B0C08

#define GPIO_PM6_M0PWM4         0x000B1802
#define GPIO_PM6_WT0CCP0        0x000B1807

#define GPIO_PM7_M0PWM5         0x000B1C02
#define GPIO_PM7_WT0CCP1        0x000B1C07

#define GPIO_PN0_CAN0RX         0x000C0001

#define GPIO_PN1_CAN0TX         0x000C0401

#define GPIO_PN2_M0PWM6         0x000C0802
#define GPIO_PN2_WT2CCP0        0x000C0807

#define GPIO_PN3_M0PWM7         0x000C0C02
#define GPIO_PN3_WT2CCP1        0x000C0C07

#define GPIO_PN4_M1PWM4         0x000C1002
#define GPIO_PN4_WT3CCP0        0x000C1007

#define GPIO_PN5_M1PWM5         0x000C1402
#define GPIO_PN5_WT3CCP1        0x000C1407

#define GPIO_PN6_M1PWM6         0x000C1802
#define GPIO_PN6_WT4CCP0        0x000C1807

#define GPIO_PN7_M1PWM7         0x000C1C02
#define GPIO_PN7_WT4CCP1        0x000C1C07

#define GPIO_PP0_M0PWM0         0x000D0001
#define GPIO_PP0_T4CCP0         0x000D0007

#define GPIO_PP1_M0PWM1         0x000D0401
#define GPIO_PP1_T4CCP1         0x000D0407

#define GPIO_PP2_M0PWM2         0x000D0801
#define GPIO_PP2_T5CCP0         0x000D0807

#define GPIO_PP3_M0PWM3         0x000D0C01
#define GPIO_PP3_T5CCP1         0x000D0C07

#define GPIO_PP4_M0PWM4         0x000D1001
#define GPIO_PP4_WT0CCP0        0x000D1007

#define GPIO_PP5_M0PWM5         0x000D1401
#define GPIO_PP5_WT0CCP1        0x000D1407

#define GPIO_PP6_M0PWM6         0x000D1801
#define GPIO_PP6_WT1CCP0        0x000D1807

#define GPIO_PP7_M0PWM7         0x000D1C01
#define GPIO_PP7_WT1CCP1        0x000D1C07

#define GPIO_PQ0_M1PWM0         0x000E0001
#define GPIO_PQ0_WT2CCP0        0x000E0007

#define GPIO_PQ1_M1PWM1         0x000E0401
#define GPIO_PQ1_WT2CCP1        0x000E0407

#define GPIO_PQ2_M1PWM2         0x000E0801
#define GPIO_PQ2_WT3CCP0        0x000E0807

#define GPIO_PQ3_M1PWM3         0x000E0C01
#define GPIO_PQ3_WT3CCP1        0x000E0C07

#define GPIO_PQ4_M1PWM4         0x000E1001
#define GPIO_PQ4_WT4CCP0        0x000E1007

#define GPIO_PQ5_M1PWM5         0x000E1401
#define GPIO_PQ5_WT4CCP1        0x000E1407

#define GPIO_PQ6_M1PWM6         0x000E1801
#define GPIO_PQ6_WT5CCP0        0x000E1807

#define GPIO_PQ7_M1PWM7         0x000E1C01
#define GPIO_PQ7_WT5CCP1        0x000E1C07

#endif // PART_TM4C123BH6ZRB

//*****************************************************************************
//
// TM4C123GH6PGE Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123GH6PGE

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_M0FAULT1       0x00031C04
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE6_CAN1RX         0x00041808

#define GPIO_PE7_U1RI           0x00041C01
#define GPIO_PE7_CAN1TX         0x00041C08

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M0FAULT1       0x00050C04
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_M0FAULT2       0x00051004
#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_M0FAULT3       0x00051404
#define GPIO_PF5_T2CCP1         0x00051407
#define GPIO_PF5_USB0PFLT       0x00051408

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_M1FAULT0       0x00051C05
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_M1FAULT1       0x00060005
#define GPIO_PG0_PHA1           0x00060006
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_M1FAULT2       0x00060405
#define GPIO_PG1_PHB1           0x00060406
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_M0FAULT1       0x00060804
#define GPIO_PG2_M1PWM0         0x00060805
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_M0FAULT2       0x00060C04
#define GPIO_PG3_M1PWM1         0x00060C05
#define GPIO_PG3_PHA1           0x00060C06
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_M0PWM4         0x00061004
#define GPIO_PG4_M1PWM2         0x00061005
#define GPIO_PG4_PHB1           0x00061006
#define GPIO_PG4_WT0CCP0        0x00061007
#define GPIO_PG4_USB0EPEN       0x00061008

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_M0PWM5         0x00061404
#define GPIO_PG5_M1PWM3         0x00061405
#define GPIO_PG5_IDX1           0x00061406
#define GPIO_PG5_WT0CCP1        0x00061407
#define GPIO_PG5_USB0PFLT       0x00061408

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_M0PWM6         0x00061804
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_M0PWM7         0x00061C04
#define GPIO_PG7_IDX1           0x00061C05
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_M0PWM0         0x00070004
#define GPIO_PH0_M0FAULT0       0x00070006
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_M0PWM1         0x00070404
#define GPIO_PH1_IDX0           0x00070405
#define GPIO_PH1_M0FAULT1       0x00070406
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_M0PWM2         0x00070804
#define GPIO_PH2_M0FAULT2       0x00070806
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_M0PWM3         0x00070C04
#define GPIO_PH3_M0FAULT3       0x00070C06
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_M0PWM4         0x00071004
#define GPIO_PH4_PHA0           0x00071005
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_M0PWM5         0x00071404
#define GPIO_PH5_PHB0           0x00071405
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_M0PWM6         0x00071804
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_M0PWM7         0x00071C04
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_IDX0           0x00080805
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PJ3_U5TX           0x00080C01
#define GPIO_PJ3_T2CCP1         0x00080C07

#define GPIO_PJ4_U6RX           0x00081001
#define GPIO_PJ4_T3CCP0         0x00081007

#define GPIO_PJ5_U6TX           0x00081401
#define GPIO_PJ5_T3CCP1         0x00081407

#define GPIO_PK0_SSI3CLK        0x00090002
#define GPIO_PK0_M1FAULT0       0x00090006

#define GPIO_PK1_SSI3FSS        0x00090402
#define GPIO_PK1_M1FAULT1       0x00090406

#define GPIO_PK2_SSI3RX         0x00090802
#define GPIO_PK2_M1FAULT2       0x00090806

#define GPIO_PK3_SSI3TX         0x00090C02
#define GPIO_PK3_M1FAULT3       0x00090C06

#define GPIO_PK4_U7RX           0x00091001
#define GPIO_PK4_M0FAULT0       0x00091006
#define GPIO_PK4_RTCCLK         0x00091007
#define GPIO_PK4_C0O            0x00091008

#define GPIO_PK5_U7TX           0x00091401
#define GPIO_PK5_M0FAULT1       0x00091406
#define GPIO_PK5_C1O            0x00091408

#define GPIO_PK6_M0FAULT2       0x00091806
#define GPIO_PK6_WT1CCP0        0x00091807
#define GPIO_PK6_C2O            0x00091808

#define GPIO_PK7_M0FAULT3       0x00091C06
#define GPIO_PK7_WT1CCP1        0x00091C07

#define GPIO_PL0_T0CCP0         0x000A0007
#define GPIO_PL0_WT0CCP0        0x000A0008

#define GPIO_PL1_T0CCP1         0x000A0407
#define GPIO_PL1_WT0CCP1        0x000A0408

#define GPIO_PL2_T1CCP0         0x000A0807
#define GPIO_PL2_WT1CCP0        0x000A0808

#define GPIO_PL3_T1CCP1         0x000A0C07
#define GPIO_PL3_WT1CCP1        0x000A0C08

#define GPIO_PL4_T2CCP0         0x000A1007
#define GPIO_PL4_WT2CCP0        0x000A1008

#define GPIO_PL5_T2CCP1         0x000A1407
#define GPIO_PL5_WT2CCP1        0x000A1408

#define GPIO_PL6_T3CCP0         0x000A1807
#define GPIO_PL6_WT3CCP0        0x000A1808

#define GPIO_PL7_T3CCP1         0x000A1C07
#define GPIO_PL7_WT3CCP1        0x000A1C08

#define GPIO_PM0_T4CCP0         0x000B0007
#define GPIO_PM0_WT4CCP0        0x000B0008

#define GPIO_PM1_T4CCP1         0x000B0407
#define GPIO_PM1_WT4CCP1        0x000B0408

#define GPIO_PM2_T5CCP0         0x000B0807
#define GPIO_PM2_WT5CCP0        0x000B0808

#define GPIO_PM3_T5CCP1         0x000B0C07
#define GPIO_PM3_WT5CCP1        0x000B0C08

#define GPIO_PM6_M0PWM4         0x000B1802
#define GPIO_PM6_WT0CCP0        0x000B1807

#define GPIO_PM7_M0PWM5         0x000B1C02
#define GPIO_PM7_WT0CCP1        0x000B1C07

#define GPIO_PN0_CAN0RX         0x000C0001

#define GPIO_PN1_CAN0TX         0x000C0401

#define GPIO_PN2_M0PWM6         0x000C0802
#define GPIO_PN2_WT2CCP0        0x000C0807

#define GPIO_PN3_M0PWM7         0x000C0C02
#define GPIO_PN3_WT2CCP1        0x000C0C07

#define GPIO_PN4_M1PWM4         0x000C1002
#define GPIO_PN4_WT3CCP0        0x000C1007

#define GPIO_PN5_M1PWM5         0x000C1402
#define GPIO_PN5_WT3CCP1        0x000C1407

#define GPIO_PN6_M1PWM6         0x000C1802
#define GPIO_PN6_WT4CCP0        0x000C1807

#define GPIO_PN7_M1PWM7         0x000C1C02
#define GPIO_PN7_WT4CCP1        0x000C1C07

#define GPIO_PP0_M0PWM0         0x000D0001
#define GPIO_PP0_T4CCP0         0x000D0007

#define GPIO_PP1_M0PWM1         0x000D0401
#define GPIO_PP1_T4CCP1         0x000D0407

#define GPIO_PP2_M0PWM2         0x000D0801
#define GPIO_PP2_T5CCP0         0x000D0807

#endif // PART_TM4C123GH6PGE

//*****************************************************************************
//
// TM4C123GH6ZRB Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_TM4C123GH6ZRB

#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA0_CAN1RX         0x00000008

#define GPIO_PA1_U0TX           0x00000401
#define GPIO_PA1_CAN1TX         0x00000408

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA6_M1PWM2         0x00001805

#define GPIO_PA7_I2C1SDA        0x00001C03
#define GPIO_PA7_M1PWM3         0x00001C05

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_M0PWM2         0x00011004
#define GPIO_PB4_T1CCP0         0x00011007
#define GPIO_PB4_CAN0RX         0x00011008

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_M0PWM3         0x00011404
#define GPIO_PB5_T1CCP1         0x00011407
#define GPIO_PB5_CAN0TX         0x00011408

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_I2C5SCL        0x00011803
#define GPIO_PB6_M0PWM0         0x00011804
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_I2C5SDA        0x00011C03
#define GPIO_PB7_M0PWM1         0x00011C04
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_M0PWM6         0x00021004
#define GPIO_PC4_IDX1           0x00021006
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_M0PWM7         0x00021404
#define GPIO_PC5_PHA1           0x00021406
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_PHB1           0x00021806
#define GPIO_PC6_WT1CCP0        0x00021807
#define GPIO_PC6_USB0EPEN       0x00021808

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07
#define GPIO_PC7_USB0PFLT       0x00021C08

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_M0PWM6         0x00030004
#define GPIO_PD0_M1PWM0         0x00030005
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_M0PWM7         0x00030404
#define GPIO_PD1_M1PWM1         0x00030405
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_M0FAULT0       0x00030804
#define GPIO_PD2_WT3CCP0        0x00030807
#define GPIO_PD2_USB0EPEN       0x00030808

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_IDX0           0x00030C06
#define GPIO_PD3_WT3CCP1        0x00030C07
#define GPIO_PD3_USB0PFLT       0x00030C08

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_M0FAULT0       0x00031804
#define GPIO_PD6_PHA0           0x00031806
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_M0FAULT1       0x00031C04
#define GPIO_PD7_PHB0           0x00031C06
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_M0PWM4         0x00041004
#define GPIO_PE4_M1PWM2         0x00041005
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_M0PWM5         0x00041404
#define GPIO_PE5_M1PWM3         0x00041405
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PE6_CAN1RX         0x00041808

#define GPIO_PE7_U1RI           0x00041C01
#define GPIO_PE7_CAN1TX         0x00041C08

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_M1PWM4         0x00050005
#define GPIO_PF0_PHA0           0x00050006
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF1_PHB0           0x00050406
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_U1DCD          0x00050801
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_M0FAULT0       0x00050804
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_C2O            0x00050809
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_U1DSR          0x00050C01
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_M0FAULT1       0x00050C04
#define GPIO_PF3_M1PWM7         0x00050C05
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_U1DTR          0x00051001
#define GPIO_PF4_M0FAULT2       0x00051004
#define GPIO_PF4_M1FAULT0       0x00051005
#define GPIO_PF4_IDX0           0x00051006
#define GPIO_PF4_T2CCP0         0x00051007
#define GPIO_PF4_USB0EPEN       0x00051008
#define GPIO_PF4_TRD3           0x0005100E

#define GPIO_PF5_M0FAULT3       0x00051404
#define GPIO_PF5_T2CCP1         0x00051407
#define GPIO_PF5_USB0PFLT       0x00051408

#define GPIO_PF6_I2C2SCL        0x00051803
#define GPIO_PF6_T3CCP0         0x00051807

#define GPIO_PF7_I2C2SDA        0x00051C03
#define GPIO_PF7_M1FAULT0       0x00051C05
#define GPIO_PF7_T3CCP1         0x00051C07

#define GPIO_PG0_I2C3SCL        0x00060003
#define GPIO_PG0_M1FAULT1       0x00060005
#define GPIO_PG0_PHA1           0x00060006
#define GPIO_PG0_T4CCP0         0x00060007

#define GPIO_PG1_I2C3SDA        0x00060403
#define GPIO_PG1_M1FAULT2       0x00060405
#define GPIO_PG1_PHB1           0x00060406
#define GPIO_PG1_T4CCP1         0x00060407

#define GPIO_PG2_I2C4SCL        0x00060803
#define GPIO_PG2_M0FAULT1       0x00060804
#define GPIO_PG2_M1PWM0         0x00060805
#define GPIO_PG2_T5CCP0         0x00060807

#define GPIO_PG3_I2C4SDA        0x00060C03
#define GPIO_PG3_M0FAULT2       0x00060C04
#define GPIO_PG3_M1PWM1         0x00060C05
#define GPIO_PG3_PHA1           0x00060C06
#define GPIO_PG3_T5CCP1         0x00060C07

#define GPIO_PG4_U2RX           0x00061001
#define GPIO_PG4_I2C1SCL        0x00061003
#define GPIO_PG4_M0PWM4         0x00061004
#define GPIO_PG4_M1PWM2         0x00061005
#define GPIO_PG4_PHB1           0x00061006
#define GPIO_PG4_WT0CCP0        0x00061007
#define GPIO_PG4_USB0EPEN       0x00061008

#define GPIO_PG5_U2TX           0x00061401
#define GPIO_PG5_I2C1SDA        0x00061403
#define GPIO_PG5_M0PWM5         0x00061404
#define GPIO_PG5_M1PWM3         0x00061405
#define GPIO_PG5_IDX1           0x00061406
#define GPIO_PG5_WT0CCP1        0x00061407
#define GPIO_PG5_USB0PFLT       0x00061408

#define GPIO_PG6_I2C5SCL        0x00061803
#define GPIO_PG6_M0PWM6         0x00061804
#define GPIO_PG6_WT1CCP0        0x00061807

#define GPIO_PG7_I2C5SDA        0x00061C03
#define GPIO_PG7_M0PWM7         0x00061C04
#define GPIO_PG7_IDX1           0x00061C05
#define GPIO_PG7_WT1CCP1        0x00061C07

#define GPIO_PH0_SSI3CLK        0x00070002
#define GPIO_PH0_M0PWM0         0x00070004
#define GPIO_PH0_M0FAULT0       0x00070006
#define GPIO_PH0_WT2CCP0        0x00070007

#define GPIO_PH1_SSI3FSS        0x00070402
#define GPIO_PH1_M0PWM1         0x00070404
#define GPIO_PH1_IDX0           0x00070405
#define GPIO_PH1_M0FAULT1       0x00070406
#define GPIO_PH1_WT2CCP1        0x00070407

#define GPIO_PH2_SSI3RX         0x00070802
#define GPIO_PH2_M0PWM2         0x00070804
#define GPIO_PH2_M0FAULT2       0x00070806
#define GPIO_PH2_WT5CCP0        0x00070807

#define GPIO_PH3_SSI3TX         0x00070C02
#define GPIO_PH3_M0PWM3         0x00070C04
#define GPIO_PH3_M0FAULT3       0x00070C06
#define GPIO_PH3_WT5CCP1        0x00070C07

#define GPIO_PH4_SSI2CLK        0x00071002
#define GPIO_PH4_M0PWM4         0x00071004
#define GPIO_PH4_PHA0           0x00071005
#define GPIO_PH4_WT3CCP0        0x00071007

#define GPIO_PH5_SSI2FSS        0x00071402
#define GPIO_PH5_M0PWM5         0x00071404
#define GPIO_PH5_PHB0           0x00071405
#define GPIO_PH5_WT3CCP1        0x00071407

#define GPIO_PH6_SSI2RX         0x00071802
#define GPIO_PH6_M0PWM6         0x00071804
#define GPIO_PH6_WT4CCP0        0x00071807

#define GPIO_PH7_SSI2TX         0x00071C02
#define GPIO_PH7_M0PWM7         0x00071C04
#define GPIO_PH7_WT4CCP1        0x00071C07

#define GPIO_PJ0_U4RX           0x00080001
#define GPIO_PJ0_T1CCP0         0x00080007

#define GPIO_PJ1_U4TX           0x00080401
#define GPIO_PJ1_T1CCP1         0x00080407

#define GPIO_PJ2_U5RX           0x00080801
#define GPIO_PJ2_IDX0           0x00080805
#define GPIO_PJ2_T2CCP0         0x00080807

#define GPIO_PJ3_U5TX           0x00080C01
#define GPIO_PJ3_T2CCP1         0x00080C07

#define GPIO_PJ4_U6RX           0x00081001
#define GPIO_PJ4_T3CCP0         0x00081007

#define GPIO_PJ5_U6TX           0x00081401
#define GPIO_PJ5_T3CCP1         0x00081407

#define GPIO_PK0_SSI3CLK        0x00090002
#define GPIO_PK0_M1FAULT0       0x00090006

#define GPIO_PK1_SSI3FSS        0x00090402
#define GPIO_PK1_M1FAULT1       0x00090406

#define GPIO_PK2_SSI3RX         0x00090802
#define GPIO_PK2_M1FAULT2       0x00090806

#define GPIO_PK3_SSI3TX         0x00090C02
#define GPIO_PK3_M1FAULT3       0x00090C06

#define GPIO_PK4_U7RX           0x00091001
#define GPIO_PK4_M0FAULT0       0x00091006
#define GPIO_PK4_RTCCLK         0x00091007
#define GPIO_PK4_C0O            0x00091008

#define GPIO_PK5_U7TX           0x00091401
#define GPIO_PK5_M0FAULT1       0x00091406
#define GPIO_PK5_C1O            0x00091408

#define GPIO_PK6_M0FAULT2       0x00091806
#define GPIO_PK6_WT1CCP0        0x00091807
#define GPIO_PK6_C2O            0x00091808

#define GPIO_PK7_M0FAULT3       0x00091C06
#define GPIO_PK7_WT1CCP1        0x00091C07

#define GPIO_PL0_T0CCP0         0x000A0007
#define GPIO_PL0_WT0CCP0        0x000A0008

#define GPIO_PL1_T0CCP1         0x000A0407
#define GPIO_PL1_WT0CCP1        0x000A0408

#define GPIO_PL2_T1CCP0         0x000A0807
#define GPIO_PL2_WT1CCP0        0x000A0808

#define GPIO_PL3_T1CCP1         0x000A0C07
#define GPIO_PL3_WT1CCP1        0x000A0C08

#define GPIO_PL4_T2CCP0         0x000A1007
#define GPIO_PL4_WT2CCP0        0x000A1008

#define GPIO_PL5_T2CCP1         0x000A1407
#define GPIO_PL5_WT2CCP1        0x000A1408

#define GPIO_PL6_T3CCP0         0x000A1807
#define GPIO_PL6_WT3CCP0        0x000A1808

#define GPIO_PL7_T3CCP1         0x000A1C07
#define GPIO_PL7_WT3CCP1        0x000A1C08

#define GPIO_PM0_T4CCP0         0x000B0007
#define GPIO_PM0_WT4CCP0        0x000B0008

#define GPIO_PM1_T4CCP1         0x000B0407
#define GPIO_PM1_WT4CCP1        0x000B0408

#define GPIO_PM2_T5CCP0         0x000B0807
#define GPIO_PM2_WT5CCP0        0x000B0808

#define GPIO_PM3_T5CCP1         0x000B0C07
#define GPIO_PM3_WT5CCP1        0x000B0C08

#define GPIO_PM6_M0PWM4         0x000B1802
#define GPIO_PM6_WT0CCP0        0x000B1807

#define GPIO_PM7_M0PWM5         0x000B1C02
#define GPIO_PM7_WT0CCP1        0x000B1C07

#define GPIO_PN0_CAN0RX         0x000C0001

#define GPIO_PN1_CAN0TX         0x000C0401

#define GPIO_PN2_M0PWM6         0x000C0802
#define GPIO_PN2_WT2CCP0        0x000C0807

#define GPIO_PN3_M0PWM7         0x000C0C02
#define GPIO_PN3_WT2CCP1        0x000C0C07

#define GPIO_PN4_M1PWM4         0x000C1002
#define GPIO_PN4_WT3CCP0        0x000C1007

#define GPIO_PN5_M1PWM5         0x000C1402
#define GPIO_PN5_WT3CCP1        0x000C1407

#define GPIO_PN6_M1PWM6         0x000C1802
#define GPIO_PN6_WT4CCP0        0x000C1807

#define GPIO_PN7_M1PWM7         0x000C1C02
#define GPIO_PN7_WT4CCP1        0x000C1C07

#define GPIO_PP0_M0PWM0         0x000D0001
#define GPIO_PP0_T4CCP0         0x000D0007

#define GPIO_PP1_M0PWM1         0x000D0401
#define GPIO_PP1_T4CCP1         0x000D0407

#define GPIO_PP2_M0PWM2         0x000D0801
#define GPIO_PP2_T5CCP0         0x000D0807

#define GPIO_PP3_M0PWM3         0x000D0C01
#define GPIO_PP3_T5CCP1         0x000D0C07

#define GPIO_PP4_M0PWM4         0x000D1001
#define GPIO_PP4_WT0CCP0        0x000D1007

#define GPIO_PP5_M0PWM5         0x000D1401
#define GPIO_PP5_WT0CCP1        0x000D1407

#define GPIO_PP6_M0PWM6         0x000D1801
#define GPIO_PP6_WT1CCP0        0x000D1807

#define GPIO_PP7_M0PWM7         0x000D1C01
#define GPIO_PP7_WT1CCP1        0x000D1C07

#define GPIO_PQ0_M1PWM0         0x000E0001
#define GPIO_PQ0_WT2CCP0        0x000E0007

#define GPIO_PQ1_M1PWM1         0x000E0401
#define GPIO_PQ1_WT2CCP1        0x000E0407

#define GPIO_PQ2_M1PWM2         0x000E0801
#define GPIO_PQ2_WT3CCP0        0x000E0807

#define GPIO_PQ3_M1PWM3         0x000E0C01
#define GPIO_PQ3_WT3CCP1        0x000E0C07

#define GPIO_PQ4_M1PWM4         0x000E1001
#define GPIO_PQ4_WT4CCP0        0x000E1007

#define GPIO_PQ5_M1PWM5         0x000E1401
#define GPIO_PQ5_WT4CCP1        0x000E1407

#define GPIO_PQ6_M1PWM6         0x000E1801
#define GPIO_PQ6_WT5CCP0        0x000E1807

#define GPIO_PQ7_M1PWM7         0x000E1C01
#define GPIO_PQ7_WT5CCP1        0x000E1C07

#endif // PART_TM4C123GH6ZRB

#endif // __DRIVERLIB_PIN_MAP_H__
