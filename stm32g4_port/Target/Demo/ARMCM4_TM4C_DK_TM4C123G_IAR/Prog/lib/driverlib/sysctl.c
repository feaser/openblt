//*****************************************************************************
//
// sysctl.c - Driver for the system controller.
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

//*****************************************************************************
//
//! \addtogroup sysctl_api
//! @{
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_flash.h"
#include "driverlib/cpu.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"

//*****************************************************************************
//
// An array that maps the crystal number in RCC to a frequency.
//
//*****************************************************************************
static const uint32_t g_pui32Xtals[] =
{
    1000000,
    1843200,
    2000000,
    2457600,
    3579545,
    3686400,
    4000000,
    4096000,
    4915200,
    5000000,
    5120000,
    6000000,
    6144000,
    7372800,
    8000000,
    8192000,
    10000000,
    12000000,
    12288000,
    13560000,
    14318180,
    16000000,
    16384000,
    18000000,
    20000000,
    24000000,
    25000000
};
//*****************************************************************************
//
// The base addresses of the various peripheral control registers.
//
//*****************************************************************************
#define SYSCTL_PPBASE           0x400fe300
#define SYSCTL_SRBASE           0x400fe500
#define SYSCTL_RCGCBASE         0x400fe600
#define SYSCTL_SCGCBASE         0x400fe700
#define SYSCTL_DCGCBASE         0x400fe800
#define SYSCTL_PCBASE           0x400fe900
#define SYSCTL_PRBASE           0x400fea00

//*****************************************************************************
//
//! \internal
//! Checks a peripheral identifier.
//!
//! \param ui32Peripheral is the peripheral identifier.
//!
//! This function determines if a peripheral identifier is valid.
//!
//! \return Returns \b true if the peripheral identifier is valid and \b false
//! otherwise.
//
//*****************************************************************************
#ifdef DEBUG
static bool
_SysCtlPeripheralValid(uint32_t ui32Peripheral)
{
    return((ui32Peripheral == SYSCTL_PERIPH_ADC0) ||
           (ui32Peripheral == SYSCTL_PERIPH_ADC1) ||
           (ui32Peripheral == SYSCTL_PERIPH_CAN0) ||
           (ui32Peripheral == SYSCTL_PERIPH_CAN1) ||
           (ui32Peripheral == SYSCTL_PERIPH_CAN2) ||
           (ui32Peripheral == SYSCTL_PERIPH_COMP0) ||
           (ui32Peripheral == SYSCTL_PERIPH_COMP1) ||
           (ui32Peripheral == SYSCTL_PERIPH_COMP2) ||
           (ui32Peripheral == SYSCTL_PERIPH_EEPROM0) ||
           (ui32Peripheral == SYSCTL_PERIPH_FAN0) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOA) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOB) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOC) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOD) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOE) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOF) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOG) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOH) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOJ) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOK) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOL) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOM) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPION) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOP) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOQ) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOR) ||
           (ui32Peripheral == SYSCTL_PERIPH_GPIOS) ||
           (ui32Peripheral == SYSCTL_PERIPH_HIBERNATE) ||
           (ui32Peripheral == SYSCTL_PERIPH_I2C0) ||
           (ui32Peripheral == SYSCTL_PERIPH_I2C1) ||
           (ui32Peripheral == SYSCTL_PERIPH_I2C2) ||
           (ui32Peripheral == SYSCTL_PERIPH_I2C3) ||
           (ui32Peripheral == SYSCTL_PERIPH_I2C4) ||
           (ui32Peripheral == SYSCTL_PERIPH_I2C5) ||
           (ui32Peripheral == SYSCTL_PERIPH_LPC0) ||
           (ui32Peripheral == SYSCTL_PERIPH_PECI0) ||
           (ui32Peripheral == SYSCTL_PERIPH_PWM0) ||
           (ui32Peripheral == SYSCTL_PERIPH_PWM1) ||
           (ui32Peripheral == SYSCTL_PERIPH_QEI0) ||
           (ui32Peripheral == SYSCTL_PERIPH_QEI1) ||
           (ui32Peripheral == SYSCTL_PERIPH_SSI0) ||
           (ui32Peripheral == SYSCTL_PERIPH_SSI1) ||
           (ui32Peripheral == SYSCTL_PERIPH_SSI2) ||
           (ui32Peripheral == SYSCTL_PERIPH_SSI3) ||
           (ui32Peripheral == SYSCTL_PERIPH_TIMER0) ||
           (ui32Peripheral == SYSCTL_PERIPH_TIMER1) ||
           (ui32Peripheral == SYSCTL_PERIPH_TIMER2) ||
           (ui32Peripheral == SYSCTL_PERIPH_TIMER3) ||
           (ui32Peripheral == SYSCTL_PERIPH_TIMER4) ||
           (ui32Peripheral == SYSCTL_PERIPH_TIMER5) ||
           (ui32Peripheral == SYSCTL_PERIPH_UART0) ||
           (ui32Peripheral == SYSCTL_PERIPH_UART1) ||
           (ui32Peripheral == SYSCTL_PERIPH_UART2) ||
           (ui32Peripheral == SYSCTL_PERIPH_UART3) ||
           (ui32Peripheral == SYSCTL_PERIPH_UART4) ||
           (ui32Peripheral == SYSCTL_PERIPH_UART5) ||
           (ui32Peripheral == SYSCTL_PERIPH_UART6) ||
           (ui32Peripheral == SYSCTL_PERIPH_UART7) ||
           (ui32Peripheral == SYSCTL_PERIPH_UDMA) ||
           (ui32Peripheral == SYSCTL_PERIPH_USB0) ||
           (ui32Peripheral == SYSCTL_PERIPH_WDOG0) ||
           (ui32Peripheral == SYSCTL_PERIPH_WDOG1) ||
           (ui32Peripheral == SYSCTL_PERIPH_WTIMER0) ||
           (ui32Peripheral == SYSCTL_PERIPH_WTIMER1) ||
           (ui32Peripheral == SYSCTL_PERIPH_WTIMER2) ||
           (ui32Peripheral == SYSCTL_PERIPH_WTIMER3) ||
           (ui32Peripheral == SYSCTL_PERIPH_WTIMER4) ||
           (ui32Peripheral == SYSCTL_PERIPH_WTIMER5));
}
#endif

//*****************************************************************************
//
//! Gets the size of the SRAM.
//!
//! This function determines the size of the SRAM on the Tiva device.
//!
//! \return The total number of bytes of SRAM.
//
//*****************************************************************************
uint32_t
SysCtlSRAMSizeGet(void)
{
    return((HWREG(FLASH_SSIZE) + 1) * 256);
}

//*****************************************************************************
//
//! Gets the size of the flash.
//!
//! This function determines the size of the flash on the Tiva device.
//!
//! \return The total number of bytes of flash.
//
//*****************************************************************************
uint32_t
SysCtlFlashSizeGet(void)
{
    //
    // Compute the size of the flash.
    //
    return(((HWREG(SYSCTL_DC0) & SYSCTL_DC0_FLASHSZ_M) << 11) + 0x800);
}

//*****************************************************************************
//
//! Determines if a peripheral is present.
//!
//! \param ui32Peripheral is the peripheral in question.
//!
//! This function determines if a particular peripheral is present in the
//! device.  Each member of the Tiva family has a different peripheral
//! set; this function determines which peripherals are present on this device.
//!
//! The \e ui32Peripheral parameter must be only one of the following values:
//! \b SYSCTL_PERIPH_ADC0, \b SYSCTL_PERIPH_ADC1, \b SYSCTL_PERIPH_CAN0,
//! \b SYSCTL_PERIPH_CAN1, \b SYSCTL_PERIPH_CAN2, \b SYSCTL_PERIPH_COMP0,
//! \b SYSCTL_PERIPH_COMP1, \b SYSCTL_PERIPH_COMP2,
//! \b SYSCTL_PERIPH_FAN0, \b SYSCTL_PERIPH_GPIOA,
//! \b SYSCTL_PERIPH_GPIOB, \b SYSCTL_PERIPH_GPIOC, \b SYSCTL_PERIPH_GPIOD,
//! \b SYSCTL_PERIPH_GPIOE, \b SYSCTL_PERIPH_GPIOF, \b SYSCTL_PERIPH_GPIOG,
//! \b SYSCTL_PERIPH_GPIOH, \b SYSCTL_PERIPH_GPIOJ, \b SYSCTL_PERIPH_GPIOK,
//! \b SYSCTL_PERIPH_GPIOL, \b SYSCTL_PERIPH_GPIOM, \b SYSCTL_PERIPH_GPION,
//! \b SYSCTL_PERIPH_GPIOP, \b SYSCTL_PERIPH_GPIOQ, \b SYSCTL_PERIPH_HIBERNATE,
//! \b SYSCTL_PERIPH_I2C0, \b SYSCTL_PERIPH_I2C1, \b SYSCTL_PERIPH_I2C2,
//! \b SYSCTL_PERIPH_I2C3, \b SYSCTL_PERIPH_I2C4, \b SYSCTL_PERIPH_I2C5,
//! \b SYSCTL_PERIPH_LPC0,
//! \b SYSCTL_PERIPH_PECI0, \b SYSCTL_PERIPH_PWM0, \b SYSCTL_PERIPH_PWM1,
//! \b SYSCTL_PERIPH_QEI0, \b SYSCTL_PERIPH_QEI1, \b SYSCTL_PERIPH_SSI0,
//! \b SYSCTL_PERIPH_SSI1, \b SYSCTL_PERIPH_SSI2, \b SYSCTL_PERIPH_SSI3,
//! \b SYSCTL_PERIPH_TIMER0, \b SYSCTL_PERIPH_TIMER1, \b SYSCTL_PERIPH_TIMER2,
//! \b SYSCTL_PERIPH_TIMER3, \b SYSCTL_PERIPH_TIMER4, \b SYSCTL_PERIPH_TIMER5,
//! \b SYSCTL_PERIPH_UART0, \b SYSCTL_PERIPH_UART1, \b SYSCTL_PERIPH_UART2,
//! \b SYSCTL_PERIPH_UART3, \b SYSCTL_PERIPH_UART4, \b SYSCTL_PERIPH_UART5,
//! \b SYSCTL_PERIPH_UART6, \b SYSCTL_PERIPH_UART7, \b SYSCTL_PERIPH_UDMA,
//! \b SYSCTL_PERIPH_USB0, \b SYSCTL_PERIPH_WDOG0, \b SYSCTL_PERIPH_WDOG1,
//! \b SYSCTL_PERIPH_WTIMER0, \b SYSCTL_PERIPH_WTIMER1,
//! \b SYSCTL_PERIPH_WTIMER2, \b SYSCTL_PERIPH_WTIMER3,
//! \b SYSCTL_PERIPH_WTIMER4, or \b SYSCTL_PERIPH_WTIMER5,
//!
//! \return Returns \b true if the specified peripheral is present and \b false
//! if it is not.
//
//*****************************************************************************
bool
SysCtlPeripheralPresent(uint32_t ui32Peripheral)
{
    //
    // Check the arguments.
    //
    ASSERT(_SysCtlPeripheralValid(ui32Peripheral));

    //
    // See if this peripheral is present.
    //
    return(HWREGBITW(SYSCTL_PPBASE + ((ui32Peripheral & 0xff00) >> 8),
                     ui32Peripheral & 0xff));
}

//*****************************************************************************
//
//! Determines if a peripheral is ready.
//!
//! \param ui32Peripheral is the peripheral in question.
//!
//! This function determines if a particular peripheral is ready to be
//! accessed.  The peripheral may be in a non-ready state if it is not enabled,
//! is being held in reset, or is in the process of becoming ready after being
//! enabled or taken out of reset.
//!
//! The \e ui32Peripheral parameter must be only one of the following values:
//! \b SYSCTL_PERIPH_ADC0, \b SYSCTL_PERIPH_ADC1, \b SYSCTL_PERIPH_CAN0,
//! \b SYSCTL_PERIPH_CAN1, \b SYSCTL_PERIPH_CAN2, \b SYSCTL_PERIPH_COMP0,
//! \b SYSCTL_PERIPH_COMP1, \b SYSCTL_PERIPH_COMP2, \b SYSCTL_PERIPH_EEPROM0,
//! \b SYSCTL_PERIPH_FAN0,
//! \b SYSCTL_PERIPH_GPIOA, \b SYSCTL_PERIPH_GPIOB, \b SYSCTL_PERIPH_GPIOC,
//! \b SYSCTL_PERIPH_GPIOD, \b SYSCTL_PERIPH_GPIOE, \b SYSCTL_PERIPH_GPIOF,
//! \b SYSCTL_PERIPH_GPIOG, \b SYSCTL_PERIPH_GPIOH, \b SYSCTL_PERIPH_GPIOJ,
//! \b SYSCTL_PERIPH_GPIOK, \b SYSCTL_PERIPH_GPIOL, \b SYSCTL_PERIPH_GPIOM,
//! \b SYSCTL_PERIPH_GPION, \b SYSCTL_PERIPH_GPIOP, \b SYSCTL_PERIPH_GPIOQ,
//! \b SYSCTL_PERIPH_HIBERNATE, \b SYSCTL_PERIPH_I2C0, \b SYSCTL_PERIPH_I2C1,
//! \b SYSCTL_PERIPH_I2C2, \b SYSCTL_PERIPH_I2C3, \b SYSCTL_PERIPH_I2C4,
//! \b SYSCTL_PERIPH_I2C5, \b SYSCTL_PERIPH_LPC0,
//! \b SYSCTL_PERIPH_PECI0, \b SYSCTL_PERIPH_PWM0, \b SYSCTL_PERIPH_PWM1,
//! \b SYSCTL_PERIPH_QEI0, \b SYSCTL_PERIPH_QEI1, \b SYSCTL_PERIPH_SSI0,
//! \b SYSCTL_PERIPH_SSI1, \b SYSCTL_PERIPH_SSI2, \b SYSCTL_PERIPH_SSI3,
//! \b SYSCTL_PERIPH_TIMER0, \b SYSCTL_PERIPH_TIMER1, \b SYSCTL_PERIPH_TIMER2,
//! \b SYSCTL_PERIPH_TIMER3, \b SYSCTL_PERIPH_TIMER4, \b SYSCTL_PERIPH_TIMER5,
//! \b SYSCTL_PERIPH_UART0, \b SYSCTL_PERIPH_UART1, \b SYSCTL_PERIPH_UART2,
//! \b SYSCTL_PERIPH_UART3, \b SYSCTL_PERIPH_UART4, \b SYSCTL_PERIPH_UART5,
//! \b SYSCTL_PERIPH_UART6, \b SYSCTL_PERIPH_UART7, \b SYSCTL_PERIPH_UDMA,
//! \b SYSCTL_PERIPH_USB0, \b SYSCTL_PERIPH_WDOG0, \b SYSCTL_PERIPH_WDOG1,
//! \b SYSCTL_PERIPH_WTIMER0, \b SYSCTL_PERIPH_WTIMER1,
//! \b SYSCTL_PERIPH_WTIMER2, \b SYSCTL_PERIPH_WTIMER3,
//! \b SYSCTL_PERIPH_WTIMER4, or \b SYSCTL_PERIPH_WTIMER5.
//!
//! \note The ability to check for a peripheral being ready varies based on the
//! Tiva part in use.  Please consult the data sheet for the part you are
//! using to determine if this feature is available.
//!
//! \return Returns \b true if the specified peripheral is ready and \b false
//! if it is not.
//
//*****************************************************************************
bool
SysCtlPeripheralReady(uint32_t ui32Peripheral)
{
    //
    // Check the arguments.
    //
    ASSERT(_SysCtlPeripheralValid(ui32Peripheral));

    //
    // See if this peripheral is ready.
    //
    return(HWREGBITW(SYSCTL_PRBASE + ((ui32Peripheral & 0xff00) >> 8),
                     ui32Peripheral & 0xff));
}

//*****************************************************************************
//
//! Powers on a peripheral.
//!
//! \param ui32Peripheral is the peripheral to be powered on.
//!
//! This function turns on the power to a peripheral.  The peripheral continues
//! to receive power even when its clock is not enabled.
//!
//! The \e ui32Peripheral parameter must be only one of the following values:
//! \b SYSCTL_PERIPH_ADC0, \b SYSCTL_PERIPH_ADC1, \b SYSCTL_PERIPH_CAN0,
//! \b SYSCTL_PERIPH_CAN1, \b SYSCTL_PERIPH_CAN2, \b SYSCTL_PERIPH_COMP0,
//! \b SYSCTL_PERIPH_COMP1, \b SYSCTL_PERIPH_COMP2, \b SYSCTL_PERIPH_EEPROM0,
//! \b SYSCTL_PERIPH_FAN0,
//! \b SYSCTL_PERIPH_GPIOA, \b SYSCTL_PERIPH_GPIOB, \b SYSCTL_PERIPH_GPIOC,
//! \b SYSCTL_PERIPH_GPIOD, \b SYSCTL_PERIPH_GPIOE, \b SYSCTL_PERIPH_GPIOF,
//! \b SYSCTL_PERIPH_GPIOG, \b SYSCTL_PERIPH_GPIOH, \b SYSCTL_PERIPH_GPIOJ,
//! \b SYSCTL_PERIPH_GPIOK, \b SYSCTL_PERIPH_GPIOL, \b SYSCTL_PERIPH_GPIOM,
//! \b SYSCTL_PERIPH_GPION, \b SYSCTL_PERIPH_GPIOP, \b SYSCTL_PERIPH_GPIOQ,
//! \b SYSCTL_PERIPH_HIBERNATE, \b SYSCTL_PERIPH_I2C0, \b SYSCTL_PERIPH_I2C1,
//! \b SYSCTL_PERIPH_I2C2, \b SYSCTL_PERIPH_I2C3, \b SYSCTL_PERIPH_I2C4,
//! \b SYSCTL_PERIPH_I2C5, \b SYSCTL_PERIPH_LPC0,
//! \b SYSCTL_PERIPH_PECI0, \b SYSCTL_PERIPH_PWM0, \b SYSCTL_PERIPH_PWM1,
//! \b SYSCTL_PERIPH_QEI0, \b SYSCTL_PERIPH_QEI1, \b SYSCTL_PERIPH_SSI0,
//! \b SYSCTL_PERIPH_SSI1, \b SYSCTL_PERIPH_SSI2, \b SYSCTL_PERIPH_SSI3,
//! \b SYSCTL_PERIPH_TIMER0, \b SYSCTL_PERIPH_TIMER1, \b SYSCTL_PERIPH_TIMER2,
//! \b SYSCTL_PERIPH_TIMER3, \b SYSCTL_PERIPH_TIMER4, \b SYSCTL_PERIPH_TIMER5,
//! \b SYSCTL_PERIPH_UART0, \b SYSCTL_PERIPH_UART1, \b SYSCTL_PERIPH_UART2,
//! \b SYSCTL_PERIPH_UART3, \b SYSCTL_PERIPH_UART4, \b SYSCTL_PERIPH_UART5,
//! \b SYSCTL_PERIPH_UART6, \b SYSCTL_PERIPH_UART7, \b SYSCTL_PERIPH_UDMA,
//! \b SYSCTL_PERIPH_USB0, \b SYSCTL_PERIPH_WDOG0, \b SYSCTL_PERIPH_WDOG1,
//! \b SYSCTL_PERIPH_WTIMER0, \b SYSCTL_PERIPH_WTIMER1,
//! \b SYSCTL_PERIPH_WTIMER2, \b SYSCTL_PERIPH_WTIMER3,
//! \b SYSCTL_PERIPH_WTIMER4, or \b SYSCTL_PERIPH_WTIMER5.
//!
//! \note The ability to power off a peripheral varies based on the Tiva
//! part in use.  Please consult the data sheet for the part you are using to
//! determine if this feature is available.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlPeripheralPowerOn(uint32_t ui32Peripheral)
{
    //
    // Check the arguments.
    //
    ASSERT(_SysCtlPeripheralValid(ui32Peripheral));

    //
    // Power on this peripheral.
    //
    HWREGBITW(SYSCTL_PCBASE + ((ui32Peripheral & 0xff00) >> 8),
              ui32Peripheral & 0xff) = 1;
}

//*****************************************************************************
//
//! Powers off a peripheral.
//!
//! \param ui32Peripheral is the peripheral to be powered off.
//!
//! This function allows the power to a peripheral to be turned off.  The
//! peripheral continues to receive power when its clock is enabled, but
//! the power is removed when its clock is disabled.
//!
//! The \e ui32Peripheral parameter must be only one of the following values:
//! \b SYSCTL_PERIPH_ADC0, \b SYSCTL_PERIPH_ADC1, \b SYSCTL_PERIPH_CAN0,
//! \b SYSCTL_PERIPH_CAN1, \b SYSCTL_PERIPH_CAN2, \b SYSCTL_PERIPH_COMP0,
//! \b SYSCTL_PERIPH_COMP1, \b SYSCTL_PERIPH_COMP2, \b SYSCTL_PERIPH_EEPROM0,
//! \b SYSCTL_PERIPH_FAN0,
//! \b SYSCTL_PERIPH_GPIOA, \b SYSCTL_PERIPH_GPIOB, \b SYSCTL_PERIPH_GPIOC,
//! \b SYSCTL_PERIPH_GPIOD, \b SYSCTL_PERIPH_GPIOE, \b SYSCTL_PERIPH_GPIOF,
//! \b SYSCTL_PERIPH_GPIOG, \b SYSCTL_PERIPH_GPIOH, \b SYSCTL_PERIPH_GPIOJ,
//! \b SYSCTL_PERIPH_GPIOK, \b SYSCTL_PERIPH_GPIOL, \b SYSCTL_PERIPH_GPIOM,
//! \b SYSCTL_PERIPH_GPION, \b SYSCTL_PERIPH_GPIOP, \b SYSCTL_PERIPH_GPIOQ,
//! \b SYSCTL_PERIPH_HIBERNATE, \b SYSCTL_PERIPH_I2C0, \b SYSCTL_PERIPH_I2C1,
//! \b SYSCTL_PERIPH_I2C2, \b SYSCTL_PERIPH_I2C3, \b SYSCTL_PERIPH_I2C4,
//! \b SYSCTL_PERIPH_I2C5, \b SYSCTL_PERIPH_LPC0,
//! \b SYSCTL_PERIPH_PECI0, \b SYSCTL_PERIPH_PWM0, \b SYSCTL_PERIPH_PWM1,
//! \b SYSCTL_PERIPH_QEI0, \b SYSCTL_PERIPH_QEI1, \b SYSCTL_PERIPH_SSI0,
//! \b SYSCTL_PERIPH_SSI1, \b SYSCTL_PERIPH_SSI2, \b SYSCTL_PERIPH_SSI3,
//! \b SYSCTL_PERIPH_TIMER0, \b SYSCTL_PERIPH_TIMER1, \b SYSCTL_PERIPH_TIMER2,
//! \b SYSCTL_PERIPH_TIMER3, \b SYSCTL_PERIPH_TIMER4, \b SYSCTL_PERIPH_TIMER5,
//! \b SYSCTL_PERIPH_UART0, \b SYSCTL_PERIPH_UART1, \b SYSCTL_PERIPH_UART2,
//! \b SYSCTL_PERIPH_UART3, \b SYSCTL_PERIPH_UART4, \b SYSCTL_PERIPH_UART5,
//! \b SYSCTL_PERIPH_UART6, \b SYSCTL_PERIPH_UART7, \b SYSCTL_PERIPH_UDMA,
//! \b SYSCTL_PERIPH_USB0, \b SYSCTL_PERIPH_WDOG0, \b SYSCTL_PERIPH_WDOG1,
//! \b SYSCTL_PERIPH_WTIMER0, \b SYSCTL_PERIPH_WTIMER1,
//! \b SYSCTL_PERIPH_WTIMER2, \b SYSCTL_PERIPH_WTIMER3,
//! \b SYSCTL_PERIPH_WTIMER4, or \b SYSCTL_PERIPH_WTIMER5.
//!
//! \note The ability to power off a peripheral varies based on the Tiva
//! part in use.  Please consult the data sheet for the part you are using to
//! determine if this feature is available.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlPeripheralPowerOff(uint32_t ui32Peripheral)
{
    //
    // Check the arguments.
    //
    ASSERT(_SysCtlPeripheralValid(ui32Peripheral));

    //
    // Power off this peripheral.
    //
    HWREGBITW(SYSCTL_PCBASE + ((ui32Peripheral & 0xff00) >> 8),
              ui32Peripheral & 0xff) = 0;
}

//*****************************************************************************
//
//! Performs a software reset of a peripheral.
//!
//! \param ui32Peripheral is the peripheral to reset.
//!
//! This function performs a software reset of the specified peripheral.  An
//! individual peripheral reset signal is asserted for a brief period and then
//! de-asserted, returning the internal state of the peripheral to its reset
//! condition.
//!
//! The \e ui32Peripheral parameter must be only one of the following values:
//! \b SYSCTL_PERIPH_ADC0, \b SYSCTL_PERIPH_ADC1, \b SYSCTL_PERIPH_CAN0,
//! \b SYSCTL_PERIPH_CAN1, \b SYSCTL_PERIPH_CAN2, \b SYSCTL_PERIPH_COMP0,
//! \b SYSCTL_PERIPH_COMP1, \b SYSCTL_PERIPH_COMP2, \b SYSCTL_PERIPH_EEPROM0,
//! \b SYSCTL_PERIPH_FAN0,
//! \b SYSCTL_PERIPH_GPIOA, \b SYSCTL_PERIPH_GPIOB, \b SYSCTL_PERIPH_GPIOC,
//! \b SYSCTL_PERIPH_GPIOD, \b SYSCTL_PERIPH_GPIOE, \b SYSCTL_PERIPH_GPIOF,
//! \b SYSCTL_PERIPH_GPIOG, \b SYSCTL_PERIPH_GPIOH, \b SYSCTL_PERIPH_GPIOJ,
//! \b SYSCTL_PERIPH_GPIOK, \b SYSCTL_PERIPH_GPIOL, \b SYSCTL_PERIPH_GPIOM,
//! \b SYSCTL_PERIPH_GPION, \b SYSCTL_PERIPH_GPIOP, \b SYSCTL_PERIPH_GPIOQ,
//! \b SYSCTL_PERIPH_HIBERNATE, \b SYSCTL_PERIPH_I2C0, \b SYSCTL_PERIPH_I2C1,
//! \b SYSCTL_PERIPH_I2C2, \b SYSCTL_PERIPH_I2C3, \b SYSCTL_PERIPH_I2C4,
//! \b SYSCTL_PERIPH_I2C5, \b SYSCTL_PERIPH_LPC0,
//! \b SYSCTL_PERIPH_PECI0, \b SYSCTL_PERIPH_PWM0, \b SYSCTL_PERIPH_PWM1,
//! \b SYSCTL_PERIPH_QEI0, \b SYSCTL_PERIPH_QEI1, \b SYSCTL_PERIPH_SSI0,
//! \b SYSCTL_PERIPH_SSI1, \b SYSCTL_PERIPH_SSI2, \b SYSCTL_PERIPH_SSI3,
//! \b SYSCTL_PERIPH_TIMER0, \b SYSCTL_PERIPH_TIMER1, \b SYSCTL_PERIPH_TIMER2,
//! \b SYSCTL_PERIPH_TIMER3, \b SYSCTL_PERIPH_TIMER4, \b SYSCTL_PERIPH_TIMER5,
//! \b SYSCTL_PERIPH_UART0, \b SYSCTL_PERIPH_UART1, \b SYSCTL_PERIPH_UART2,
//! \b SYSCTL_PERIPH_UART3, \b SYSCTL_PERIPH_UART4, \b SYSCTL_PERIPH_UART5,
//! \b SYSCTL_PERIPH_UART6, \b SYSCTL_PERIPH_UART7, \b SYSCTL_PERIPH_UDMA,
//! \b SYSCTL_PERIPH_USB0, \b SYSCTL_PERIPH_WDOG0, \b SYSCTL_PERIPH_WDOG1,
//! \b SYSCTL_PERIPH_WTIMER0, \b SYSCTL_PERIPH_WTIMER1,
//! \b SYSCTL_PERIPH_WTIMER2, \b SYSCTL_PERIPH_WTIMER3,
//! \b SYSCTL_PERIPH_WTIMER4, or \b SYSCTL_PERIPH_WTIMER5.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlPeripheralReset(uint32_t ui32Peripheral)
{
    volatile uint_fast8_t ui8Delay;

    //
    // Check the arguments.
    //
    ASSERT(_SysCtlPeripheralValid(ui32Peripheral));

    //
    // Put the peripheral into the reset state.
    //
    HWREGBITW(SYSCTL_SRBASE + ((ui32Peripheral & 0xff00) >> 8),
              ui32Peripheral & 0xff) = 1;

    //
    // Delay for a little bit.
    //
    for(ui8Delay = 0; ui8Delay < 16; ui8Delay++)
    {
    }

    //
    // Take the peripheral out of the reset state.
    //
    HWREGBITW(SYSCTL_SRBASE + ((ui32Peripheral & 0xff00) >> 8),
              ui32Peripheral & 0xff) = 0;
}

//*****************************************************************************
//
//! Enables a peripheral.
//!
//! \param ui32Peripheral is the peripheral to enable.
//!
//! This function enables a peripheral.  At power-up, all peripherals are
//! disabled; they must be enabled in order to operate or respond to register
//! reads/writes.
//!
//! The \e ui32Peripheral parameter must be only one of the following values:
//! \b SYSCTL_PERIPH_ADC0, \b SYSCTL_PERIPH_ADC1, \b SYSCTL_PERIPH_CAN0,
//! \b SYSCTL_PERIPH_CAN1, \b SYSCTL_PERIPH_CAN2, \b SYSCTL_PERIPH_COMP0,
//! \b SYSCTL_PERIPH_COMP1, \b SYSCTL_PERIPH_COMP2, \b SYSCTL_PERIPH_EEPROM0,
//! \b SYSCTL_PERIPH_FAN0,
//! \b SYSCTL_PERIPH_GPIOA, \b SYSCTL_PERIPH_GPIOB, \b SYSCTL_PERIPH_GPIOC,
//! \b SYSCTL_PERIPH_GPIOD, \b SYSCTL_PERIPH_GPIOE, \b SYSCTL_PERIPH_GPIOF,
//! \b SYSCTL_PERIPH_GPIOG, \b SYSCTL_PERIPH_GPIOH, \b SYSCTL_PERIPH_GPIOJ,
//! \b SYSCTL_PERIPH_GPIOK, \b SYSCTL_PERIPH_GPIOL, \b SYSCTL_PERIPH_GPIOM,
//! \b SYSCTL_PERIPH_GPION, \b SYSCTL_PERIPH_GPIOP, \b SYSCTL_PERIPH_GPIOQ,
//! \b SYSCTL_PERIPH_HIBERNATE, \b SYSCTL_PERIPH_I2C0, \b SYSCTL_PERIPH_I2C1,
//! \b SYSCTL_PERIPH_I2C2, \b SYSCTL_PERIPH_I2C3, \b SYSCTL_PERIPH_I2C4,
//! \b SYSCTL_PERIPH_I2C5, \b SYSCTL_PERIPH_LPC0,
//! \b SYSCTL_PERIPH_PECI0, \b SYSCTL_PERIPH_PWM0, \b SYSCTL_PERIPH_PWM1,
//! \b SYSCTL_PERIPH_QEI0, \b SYSCTL_PERIPH_QEI1, \b SYSCTL_PERIPH_SSI0,
//! \b SYSCTL_PERIPH_SSI1, \b SYSCTL_PERIPH_SSI2, \b SYSCTL_PERIPH_SSI3,
//! \b SYSCTL_PERIPH_TIMER0, \b SYSCTL_PERIPH_TIMER1, \b SYSCTL_PERIPH_TIMER2,
//! \b SYSCTL_PERIPH_TIMER3, \b SYSCTL_PERIPH_TIMER4, \b SYSCTL_PERIPH_TIMER5,
//! \b SYSCTL_PERIPH_UART0, \b SYSCTL_PERIPH_UART1, \b SYSCTL_PERIPH_UART2,
//! \b SYSCTL_PERIPH_UART3, \b SYSCTL_PERIPH_UART4, \b SYSCTL_PERIPH_UART5,
//! \b SYSCTL_PERIPH_UART6, \b SYSCTL_PERIPH_UART7, \b SYSCTL_PERIPH_UDMA,
//! \b SYSCTL_PERIPH_USB0, \b SYSCTL_PERIPH_WDOG0, \b SYSCTL_PERIPH_WDOG1,
//! \b SYSCTL_PERIPH_WTIMER0, \b SYSCTL_PERIPH_WTIMER1,
//! \b SYSCTL_PERIPH_WTIMER2, \b SYSCTL_PERIPH_WTIMER3,
//! \b SYSCTL_PERIPH_WTIMER4, or \b SYSCTL_PERIPH_WTIMER5.
//!
//! \note It takes five clock cycles after the write to enable a peripheral
//! before the the peripheral is actually enabled.  During this time, attempts
//! to access the peripheral result in a bus fault.  Care should be taken
//! to ensure that the peripheral is not accessed during this brief time
//! period.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlPeripheralEnable(uint32_t ui32Peripheral)
{
    //
    // Check the arguments.
    //
    ASSERT(_SysCtlPeripheralValid(ui32Peripheral));

    //
    // Enable this peripheral.
    //
    HWREGBITW(SYSCTL_RCGCBASE + ((ui32Peripheral & 0xff00) >> 8),
              ui32Peripheral & 0xff) = 1;
}

//*****************************************************************************
//
//! Disables a peripheral.
//!
//! \param ui32Peripheral is the peripheral to disable.
//!
//! This function disables a peripheral.  Once disabled, they do not operate or
//! respond to register reads/writes.
//!
//! The \e ui32Peripheral parameter must be only one of the following values:
//! \b SYSCTL_PERIPH_ADC0, \b SYSCTL_PERIPH_ADC1, \b SYSCTL_PERIPH_CAN0,
//! \b SYSCTL_PERIPH_CAN1, \b SYSCTL_PERIPH_CAN2, \b SYSCTL_PERIPH_COMP0,
//! \b SYSCTL_PERIPH_COMP1, \b SYSCTL_PERIPH_COMP2, \b SYSCTL_PERIPH_EEPROM0,
//! \b SYSCTL_PERIPH_FAN0,
//! \b SYSCTL_PERIPH_GPIOA, \b SYSCTL_PERIPH_GPIOB, \b SYSCTL_PERIPH_GPIOC,
//! \b SYSCTL_PERIPH_GPIOD, \b SYSCTL_PERIPH_GPIOE, \b SYSCTL_PERIPH_GPIOF,
//! \b SYSCTL_PERIPH_GPIOG, \b SYSCTL_PERIPH_GPIOH, \b SYSCTL_PERIPH_GPIOJ,
//! \b SYSCTL_PERIPH_GPIOK, \b SYSCTL_PERIPH_GPIOL, \b SYSCTL_PERIPH_GPIOM,
//! \b SYSCTL_PERIPH_GPION, \b SYSCTL_PERIPH_GPIOP, \b SYSCTL_PERIPH_GPIOQ,
//! \b SYSCTL_PERIPH_HIBERNATE, \b SYSCTL_PERIPH_I2C0, \b SYSCTL_PERIPH_I2C1,
//! \b SYSCTL_PERIPH_I2C2, \b SYSCTL_PERIPH_I2C3, \b SYSCTL_PERIPH_I2C4,
//! \b SYSCTL_PERIPH_I2C5, \b SYSCTL_PERIPH_LPC0,
//! \b SYSCTL_PERIPH_PECI0, \b SYSCTL_PERIPH_PWM0, \b SYSCTL_PERIPH_PWM1,
//! \b SYSCTL_PERIPH_QEI0, \b SYSCTL_PERIPH_QEI1, \b SYSCTL_PERIPH_SSI0,
//! \b SYSCTL_PERIPH_SSI1, \b SYSCTL_PERIPH_SSI2, \b SYSCTL_PERIPH_SSI3,
//! \b SYSCTL_PERIPH_TIMER0, \b SYSCTL_PERIPH_TIMER1, \b SYSCTL_PERIPH_TIMER2,
//! \b SYSCTL_PERIPH_TIMER3, \b SYSCTL_PERIPH_TIMER4, \b SYSCTL_PERIPH_TIMER5,
//! \b SYSCTL_PERIPH_UART0, \b SYSCTL_PERIPH_UART1, \b SYSCTL_PERIPH_UART2,
//! \b SYSCTL_PERIPH_UART3, \b SYSCTL_PERIPH_UART4, \b SYSCTL_PERIPH_UART5,
//! \b SYSCTL_PERIPH_UART6, \b SYSCTL_PERIPH_UART7, \b SYSCTL_PERIPH_UDMA,
//! \b SYSCTL_PERIPH_USB0, \b SYSCTL_PERIPH_WDOG0, \b SYSCTL_PERIPH_WDOG1,
//! \b SYSCTL_PERIPH_WTIMER0, \b SYSCTL_PERIPH_WTIMER1,
//! \b SYSCTL_PERIPH_WTIMER2, \b SYSCTL_PERIPH_WTIMER3,
//! \b SYSCTL_PERIPH_WTIMER4, or \b SYSCTL_PERIPH_WTIMER5.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlPeripheralDisable(uint32_t ui32Peripheral)
{
    //
    // Check the arguments.
    //
    ASSERT(_SysCtlPeripheralValid(ui32Peripheral));

    //
    // Disable this peripheral.
    //
    HWREGBITW(SYSCTL_RCGCBASE + ((ui32Peripheral & 0xff00) >> 8),
              ui32Peripheral & 0xff) = 0;
}

//*****************************************************************************
//
//! Enables a peripheral in sleep mode.
//!
//! \param ui32Peripheral is the peripheral to enable in sleep mode.
//!
//! This function allows a peripheral to continue operating when the processor
//! goes into sleep mode.  Because the clocking configuration of the device
//! does not change, any peripheral can safely continue operating while the
//! processor is in sleep mode and can therefore wake the processor from sleep
//! mode.
//!
//! Sleep mode clocking of peripherals must be enabled via
//! SysCtlPeripheralClockGating(); if disabled, the peripheral sleep mode
//! configuration is maintained but has no effect when sleep mode is entered.
//!
//! The \e ui32Peripheral parameter must be only one of the following values:
//! \b SYSCTL_PERIPH_ADC0, \b SYSCTL_PERIPH_ADC1, \b SYSCTL_PERIPH_CAN0,
//! \b SYSCTL_PERIPH_CAN1, \b SYSCTL_PERIPH_CAN2, \b SYSCTL_PERIPH_COMP0,
//! \b SYSCTL_PERIPH_COMP1, \b SYSCTL_PERIPH_COMP2, \b SYSCTL_PERIPH_EEPROM0,
//! \b SYSCTL_PERIPH_FAN0,
//! \b SYSCTL_PERIPH_GPIOA, \b SYSCTL_PERIPH_GPIOB, \b SYSCTL_PERIPH_GPIOC,
//! \b SYSCTL_PERIPH_GPIOD, \b SYSCTL_PERIPH_GPIOE, \b SYSCTL_PERIPH_GPIOF,
//! \b SYSCTL_PERIPH_GPIOG, \b SYSCTL_PERIPH_GPIOH, \b SYSCTL_PERIPH_GPIOJ,
//! \b SYSCTL_PERIPH_GPIOK, \b SYSCTL_PERIPH_GPIOL, \b SYSCTL_PERIPH_GPIOM,
//! \b SYSCTL_PERIPH_GPION, \b SYSCTL_PERIPH_GPIOP, \b SYSCTL_PERIPH_GPIOQ,
//! \b SYSCTL_PERIPH_HIBERNATE, \b SYSCTL_PERIPH_I2C0, \b SYSCTL_PERIPH_I2C1,
//! \b SYSCTL_PERIPH_I2C2, \b SYSCTL_PERIPH_I2C3, \b SYSCTL_PERIPH_I2C4,
//! \b SYSCTL_PERIPH_I2C5, \b SYSCTL_PERIPH_LPC0,
//! \b SYSCTL_PERIPH_PECI0, \b SYSCTL_PERIPH_PWM0, \b SYSCTL_PERIPH_PWM1,
//! \b SYSCTL_PERIPH_QEI0, \b SYSCTL_PERIPH_QEI1, \b SYSCTL_PERIPH_SSI0,
//! \b SYSCTL_PERIPH_SSI1, \b SYSCTL_PERIPH_SSI2, \b SYSCTL_PERIPH_SSI3,
//! \b SYSCTL_PERIPH_TIMER0, \b SYSCTL_PERIPH_TIMER1, \b SYSCTL_PERIPH_TIMER2,
//! \b SYSCTL_PERIPH_TIMER3, \b SYSCTL_PERIPH_TIMER4, \b SYSCTL_PERIPH_TIMER5,
//! \b SYSCTL_PERIPH_UART0, \b SYSCTL_PERIPH_UART1, \b SYSCTL_PERIPH_UART2,
//! \b SYSCTL_PERIPH_UART3, \b SYSCTL_PERIPH_UART4, \b SYSCTL_PERIPH_UART5,
//! \b SYSCTL_PERIPH_UART6, \b SYSCTL_PERIPH_UART7, \b SYSCTL_PERIPH_UDMA,
//! \b SYSCTL_PERIPH_USB0, \b SYSCTL_PERIPH_WDOG0, \b SYSCTL_PERIPH_WDOG1,
//! \b SYSCTL_PERIPH_WTIMER0, \b SYSCTL_PERIPH_WTIMER1,
//! \b SYSCTL_PERIPH_WTIMER2, \b SYSCTL_PERIPH_WTIMER3,
//! \b SYSCTL_PERIPH_WTIMER4, or \b SYSCTL_PERIPH_WTIMER5.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlPeripheralSleepEnable(uint32_t ui32Peripheral)
{
    //
    // Check the arguments.
    //
    ASSERT(_SysCtlPeripheralValid(ui32Peripheral));

    //
    // Enable this peripheral in sleep mode.
    //
    HWREGBITW(SYSCTL_SCGCBASE + ((ui32Peripheral & 0xff00) >> 8),
              ui32Peripheral & 0xff) = 1;
}

//*****************************************************************************
//
//! Disables a peripheral in sleep mode.
//!
//! \param ui32Peripheral is the peripheral to disable in sleep mode.
//!
//! This function causes a peripheral to stop operating when the processor goes
//! into sleep mode.  Disabling peripherals while in sleep mode helps to lower
//! the current draw of the device.  If enabled (via SysCtlPeripheralEnable()),
//! the peripheral automatically resumes operation when the processor
//! leaves sleep mode, maintaining its entire state from before sleep mode was
//! entered.
//!
//! Sleep mode clocking of peripherals must be enabled via
//! SysCtlPeripheralClockGating(); if disabled, the peripheral sleep mode
//! configuration is maintained but has no effect when sleep mode is entered.
//!
//! The \e ui32Peripheral parameter must be only one of the following values:
//! \b SYSCTL_PERIPH_ADC0, \b SYSCTL_PERIPH_ADC1, \b SYSCTL_PERIPH_CAN0,
//! \b SYSCTL_PERIPH_CAN1, \b SYSCTL_PERIPH_CAN2, \b SYSCTL_PERIPH_COMP0,
//! \b SYSCTL_PERIPH_COMP1, \b SYSCTL_PERIPH_COMP2, \b SYSCTL_PERIPH_EEPROM0,
//! \b SYSCTL_PERIPH_FAN0,
//! \b SYSCTL_PERIPH_GPIOA, \b SYSCTL_PERIPH_GPIOB, \b SYSCTL_PERIPH_GPIOC,
//! \b SYSCTL_PERIPH_GPIOD, \b SYSCTL_PERIPH_GPIOE, \b SYSCTL_PERIPH_GPIOF,
//! \b SYSCTL_PERIPH_GPIOG, \b SYSCTL_PERIPH_GPIOH, \b SYSCTL_PERIPH_GPIOJ,
//! \b SYSCTL_PERIPH_GPIOK, \b SYSCTL_PERIPH_GPIOL, \b SYSCTL_PERIPH_GPIOM,
//! \b SYSCTL_PERIPH_GPION, \b SYSCTL_PERIPH_GPIOP, \b SYSCTL_PERIPH_GPIOQ,
//! \b SYSCTL_PERIPH_HIBERNATE, \b SYSCTL_PERIPH_I2C0, \b SYSCTL_PERIPH_I2C1,
//! \b SYSCTL_PERIPH_I2C2, \b SYSCTL_PERIPH_I2C3, \b SYSCTL_PERIPH_I2C4,
//! \b SYSCTL_PERIPH_I2C5, \b SYSCTL_PERIPH_LPC0,
//! \b SYSCTL_PERIPH_PECI0, \b SYSCTL_PERIPH_PWM0, \b SYSCTL_PERIPH_PWM1,
//! \b SYSCTL_PERIPH_QEI0, \b SYSCTL_PERIPH_QEI1, \b SYSCTL_PERIPH_SSI0,
//! \b SYSCTL_PERIPH_SSI1, \b SYSCTL_PERIPH_SSI2, \b SYSCTL_PERIPH_SSI3,
//! \b SYSCTL_PERIPH_TIMER0, \b SYSCTL_PERIPH_TIMER1, \b SYSCTL_PERIPH_TIMER2,
//! \b SYSCTL_PERIPH_TIMER3, \b SYSCTL_PERIPH_TIMER4, \b SYSCTL_PERIPH_TIMER5,
//! \b SYSCTL_PERIPH_UART0, \b SYSCTL_PERIPH_UART1, \b SYSCTL_PERIPH_UART2,
//! \b SYSCTL_PERIPH_UART3, \b SYSCTL_PERIPH_UART4, \b SYSCTL_PERIPH_UART5,
//! \b SYSCTL_PERIPH_UART6, \b SYSCTL_PERIPH_UART7, \b SYSCTL_PERIPH_UDMA,
//! \b SYSCTL_PERIPH_USB0, \b SYSCTL_PERIPH_WDOG0, \b SYSCTL_PERIPH_WDOG1,
//! \b SYSCTL_PERIPH_WTIMER0, \b SYSCTL_PERIPH_WTIMER1,
//! \b SYSCTL_PERIPH_WTIMER2, \b SYSCTL_PERIPH_WTIMER3,
//! \b SYSCTL_PERIPH_WTIMER4, or \b SYSCTL_PERIPH_WTIMER5.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlPeripheralSleepDisable(uint32_t ui32Peripheral)
{
    //
    // Check the arguments.
    //
    ASSERT(_SysCtlPeripheralValid(ui32Peripheral));

    //
    // Disable this peripheral in sleep mode.
    //
    HWREGBITW(SYSCTL_SCGCBASE + ((ui32Peripheral & 0xff00) >> 8),
              ui32Peripheral & 0xff) = 0;
}

//*****************************************************************************
//
//! Enables a peripheral in deep-sleep mode.
//!
//! \param ui32Peripheral is the peripheral to enable in deep-sleep mode.
//!
//! This function allows a peripheral to continue operating when the processor
//! goes into deep-sleep mode.  Because the clocking configuration of the
//! device may change, not all peripherals can safely continue operating while
//! the processor is in deep-sleep mode.  Those that must run at a particular
//! frequency (such as a UART) do not work as expected if the clock changes.
//! It is the responsibility of the caller to make sensible choices.
//!
//! Deep-sleep mode clocking of peripherals must be enabled via
//! SysCtlPeripheralClockGating(); if disabled, the peripheral deep-sleep mode
//! configuration is maintained but has no effect when deep-sleep mode is
//! entered.
//!
//! The \e ui32Peripheral parameter must be only one of the following values:
//! \b SYSCTL_PERIPH_ADC0, \b SYSCTL_PERIPH_ADC1, \b SYSCTL_PERIPH_CAN0,
//! \b SYSCTL_PERIPH_CAN1, \b SYSCTL_PERIPH_CAN2, \b SYSCTL_PERIPH_COMP0,
//! \b SYSCTL_PERIPH_COMP1, \b SYSCTL_PERIPH_COMP2, \b SYSCTL_PERIPH_EEPROM0,
//! \b SYSCTL_PERIPH_FAN0,
//! \b SYSCTL_PERIPH_GPIOA, \b SYSCTL_PERIPH_GPIOB, \b SYSCTL_PERIPH_GPIOC,
//! \b SYSCTL_PERIPH_GPIOD, \b SYSCTL_PERIPH_GPIOE, \b SYSCTL_PERIPH_GPIOF,
//! \b SYSCTL_PERIPH_GPIOG, \b SYSCTL_PERIPH_GPIOH, \b SYSCTL_PERIPH_GPIOJ,
//! \b SYSCTL_PERIPH_GPIOK, \b SYSCTL_PERIPH_GPIOL, \b SYSCTL_PERIPH_GPIOM,
//! \b SYSCTL_PERIPH_GPION, \b SYSCTL_PERIPH_GPIOP, \b SYSCTL_PERIPH_GPIOQ,
//! \b SYSCTL_PERIPH_HIBERNATE, \b SYSCTL_PERIPH_I2C0, \b SYSCTL_PERIPH_I2C1,
//! \b SYSCTL_PERIPH_I2C2, \b SYSCTL_PERIPH_I2C3, \b SYSCTL_PERIPH_I2C4,
//! \b SYSCTL_PERIPH_I2C5, \b SYSCTL_PERIPH_LPC0,
//! \b SYSCTL_PERIPH_PECI0, \b SYSCTL_PERIPH_PWM0, \b SYSCTL_PERIPH_PWM1,
//! \b SYSCTL_PERIPH_QEI0, \b SYSCTL_PERIPH_QEI1, \b SYSCTL_PERIPH_SSI0,
//! \b SYSCTL_PERIPH_SSI1, \b SYSCTL_PERIPH_SSI2, \b SYSCTL_PERIPH_SSI3,
//! \b SYSCTL_PERIPH_TIMER0, \b SYSCTL_PERIPH_TIMER1, \b SYSCTL_PERIPH_TIMER2,
//! \b SYSCTL_PERIPH_TIMER3, \b SYSCTL_PERIPH_TIMER4, \b SYSCTL_PERIPH_TIMER5,
//! \b SYSCTL_PERIPH_UART0, \b SYSCTL_PERIPH_UART1, \b SYSCTL_PERIPH_UART2,
//! \b SYSCTL_PERIPH_UART3, \b SYSCTL_PERIPH_UART4, \b SYSCTL_PERIPH_UART5,
//! \b SYSCTL_PERIPH_UART6, \b SYSCTL_PERIPH_UART7, \b SYSCTL_PERIPH_UDMA,
//! \b SYSCTL_PERIPH_USB0, \b SYSCTL_PERIPH_WDOG0, \b SYSCTL_PERIPH_WDOG1,
//! \b SYSCTL_PERIPH_WTIMER0, \b SYSCTL_PERIPH_WTIMER1,
//! \b SYSCTL_PERIPH_WTIMER2, \b SYSCTL_PERIPH_WTIMER3,
//! \b SYSCTL_PERIPH_WTIMER4, or \b SYSCTL_PERIPH_WTIMER5.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlPeripheralDeepSleepEnable(uint32_t ui32Peripheral)
{
    //
    // Check the arguments.
    //
    ASSERT(_SysCtlPeripheralValid(ui32Peripheral));

    //
    // Enable this peripheral in deep-sleep mode.
    //
    HWREGBITW(SYSCTL_DCGCBASE + ((ui32Peripheral & 0xff00) >> 8),
              ui32Peripheral & 0xff) = 1;
}

//*****************************************************************************
//
//! Disables a peripheral in deep-sleep mode.
//!
//! \param ui32Peripheral is the peripheral to disable in deep-sleep mode.
//!
//! This function causes a peripheral to stop operating when the processor goes
//! into deep-sleep mode.  Disabling peripherals while in deep-sleep mode helps
//! to lower the current draw of the device, and can keep peripherals that
//! require a particular clock frequency from operating when the clock changes
//! as a result of entering deep-sleep mode.  If enabled (via
//! SysCtlPeripheralEnable()), the peripheral automatically resumes
//! operation when the processor leaves deep-sleep mode, maintaining its entire
//! state from before deep-sleep mode was entered.
//!
//! Deep-sleep mode clocking of peripherals must be enabled via
//! SysCtlPeripheralClockGating(); if disabled, the peripheral deep-sleep mode
//! configuration is maintained but has no effect when deep-sleep mode is
//! entered.
//!
//! The \e ui32Peripheral parameter must be only one of the following values:
//! \b SYSCTL_PERIPH_ADC0, \b SYSCTL_PERIPH_ADC1, \b SYSCTL_PERIPH_CAN0,
//! \b SYSCTL_PERIPH_CAN1, \b SYSCTL_PERIPH_CAN2, \b SYSCTL_PERIPH_COMP0,
//! \b SYSCTL_PERIPH_COMP1, \b SYSCTL_PERIPH_COMP2, \b SYSCTL_PERIPH_EEPROM0,
//! \b SYSCTL_PERIPH_FAN0,
//! \b SYSCTL_PERIPH_GPIOA, \b SYSCTL_PERIPH_GPIOB, \b SYSCTL_PERIPH_GPIOC,
//! \b SYSCTL_PERIPH_GPIOD, \b SYSCTL_PERIPH_GPIOE, \b SYSCTL_PERIPH_GPIOF,
//! \b SYSCTL_PERIPH_GPIOG, \b SYSCTL_PERIPH_GPIOH, \b SYSCTL_PERIPH_GPIOJ,
//! \b SYSCTL_PERIPH_GPIOK, \b SYSCTL_PERIPH_GPIOL, \b SYSCTL_PERIPH_GPIOM,
//! \b SYSCTL_PERIPH_GPION, \b SYSCTL_PERIPH_GPIOP, \b SYSCTL_PERIPH_GPIOQ,
//! \b SYSCTL_PERIPH_HIBERNATE, \b SYSCTL_PERIPH_I2C0, \b SYSCTL_PERIPH_I2C1,
//! \b SYSCTL_PERIPH_I2C2, \b SYSCTL_PERIPH_I2C3, \b SYSCTL_PERIPH_I2C4,
//! \b SYSCTL_PERIPH_I2C5, \b SYSCTL_PERIPH_LPC0,
//! \b SYSCTL_PERIPH_PECI0, \b SYSCTL_PERIPH_PWM0, \b SYSCTL_PERIPH_PWM1,
//! \b SYSCTL_PERIPH_QEI0, \b SYSCTL_PERIPH_QEI1, \b SYSCTL_PERIPH_SSI0,
//! \b SYSCTL_PERIPH_SSI1, \b SYSCTL_PERIPH_SSI2, \b SYSCTL_PERIPH_SSI3,
//! \b SYSCTL_PERIPH_TIMER0, \b SYSCTL_PERIPH_TIMER1, \b SYSCTL_PERIPH_TIMER2,
//! \b SYSCTL_PERIPH_TIMER3, \b SYSCTL_PERIPH_TIMER4, \b SYSCTL_PERIPH_TIMER5,
//! \b SYSCTL_PERIPH_UART0, \b SYSCTL_PERIPH_UART1, \b SYSCTL_PERIPH_UART2,
//! \b SYSCTL_PERIPH_UART3, \b SYSCTL_PERIPH_UART4, \b SYSCTL_PERIPH_UART5,
//! \b SYSCTL_PERIPH_UART6, \b SYSCTL_PERIPH_UART7, \b SYSCTL_PERIPH_UDMA,
//! \b SYSCTL_PERIPH_USB0, \b SYSCTL_PERIPH_WDOG0, \b SYSCTL_PERIPH_WDOG1,
//! \b SYSCTL_PERIPH_WTIMER0, \b SYSCTL_PERIPH_WTIMER1,
//! \b SYSCTL_PERIPH_WTIMER2, \b SYSCTL_PERIPH_WTIMER3,
//! \b SYSCTL_PERIPH_WTIMER4, or \b SYSCTL_PERIPH_WTIMER5.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlPeripheralDeepSleepDisable(uint32_t ui32Peripheral)
{
    //
    // Check the arguments.
    //
    ASSERT(_SysCtlPeripheralValid(ui32Peripheral));

    //
    // Disable this peripheral in deep-sleep mode.
    //
    HWREGBITW(SYSCTL_DCGCBASE + ((ui32Peripheral & 0xff00) >> 8),
              ui32Peripheral & 0xff) = 0;
}

//*****************************************************************************
//
//! Controls peripheral clock gating in sleep and deep-sleep mode.
//!
//! \param bEnable is a boolean that is \b true if the sleep and deep-sleep
//! peripheral configuration should be used and \b false if not.
//!
//! This function controls how peripherals are clocked when the processor goes
//! into sleep or deep-sleep mode.  By default, the peripherals are clocked the
//! same as in run mode; if peripheral clock gating is enabled, they are
//! clocked according to the configuration set by
//! SysCtlPeripheralSleepEnable(), SysCtlPeripheralSleepDisable(),
//! SysCtlPeripheralDeepSleepEnable(), and SysCtlPeripheralDeepSleepDisable().
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlPeripheralClockGating(bool bEnable)
{
    //
    // Enable peripheral clock gating as requested.
    //
    if(bEnable)
    {
        HWREG(SYSCTL_RCC) |= SYSCTL_RCC_ACG;
    }
    else
    {
        HWREG(SYSCTL_RCC) &= ~(SYSCTL_RCC_ACG);
    }
}

//*****************************************************************************
//
//! Registers an interrupt handler for the system control interrupt.
//!
//! \param pfnHandler is a pointer to the function to be called when the system
//! control interrupt occurs.
//!
//! This function registers the handler to be called when a system control
//! interrupt occurs.  This function enables the global interrupt in the
//! interrupt controller; specific system control interrupts must be enabled
//! via SysCtlIntEnable().  It is the interrupt handler's responsibility to
//! clear the interrupt source via SysCtlIntClear().
//!
//! System control can generate interrupts when the PLL achieves lock, if the
//! internal LDO current limit is exceeded, if the internal oscillator fails,
//! if the main oscillator fails, if the internal LDO output voltage droops too
//! much, if the external voltage droops too much, or if the PLL fails.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \note The events that cause system control interrupts vary based on the
//! Tiva part in use.  Please consult the data sheet for the part you are
//! using to determine which interrupt sources are available.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlIntRegister(void (*pfnHandler)(void))
{
    //
    // Register the interrupt handler, returning an error if an error occurs.
    //
    IntRegister(INT_SYSCTL_BLIZZARD, pfnHandler);

    //
    // Enable the system control interrupt.
    //
    IntEnable(INT_SYSCTL_BLIZZARD);
}

//*****************************************************************************
//
//! Unregisters the interrupt handler for the system control interrupt.
//!
//! This function unregisters the handler to be called when a system control
//! interrupt occurs.  This function also masks off the interrupt in the
//! interrupt controller so that the interrupt handler no longer is called.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlIntUnregister(void)
{
    //
    // Disable the interrupt.
    //
    IntDisable(INT_SYSCTL_BLIZZARD);

    //
    // Unregister the interrupt handler.
    //
    IntUnregister(INT_SYSCTL_BLIZZARD);
}

//*****************************************************************************
//
//! Enables individual system control interrupt sources.
//!
//! \param ui32Ints is a bit mask of the interrupt sources to be enabled.  Must
//! be a logical OR of \b SYSCTL_INT_PLL_LOCK, \b SYSCTL_INT_CUR_LIMIT,
//! \b SYSCTL_INT_IOSC_FAIL, \b SYSCTL_INT_MOSC_FAIL, \b SYSCTL_INT_POR,
//! \b SYSCTL_INT_BOR, and/or \b SYSCTL_INT_PLL_FAIL.
//!
//! This function enables the indicated system control interrupt sources.  Only
//! the sources that are enabled can be reflected to the processor interrupt;
//! disabled sources have no effect on the processor.
//!
//! \note The interrupt sources vary based on the Tiva part in use.
//! Please consult the data sheet for the part you are using to determine
//! which interrupt sources are available.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlIntEnable(uint32_t ui32Ints)
{
    //
    // Enable the specified interrupts.
    //
    HWREG(SYSCTL_IMC) |= ui32Ints;
}

//*****************************************************************************
//
//! Disables individual system control interrupt sources.
//!
//! \param ui32Ints is a bit mask of the interrupt sources to be disabled.
//! Must be a logical OR of \b SYSCTL_INT_PLL_LOCK, \b SYSCTL_INT_CUR_LIMIT,
//! \b SYSCTL_INT_IOSC_FAIL, \b SYSCTL_INT_MOSC_FAIL, \b SYSCTL_INT_POR,
//! \b SYSCTL_INT_BOR, and/or \b SYSCTL_INT_PLL_FAIL.
//!
//! This function disables the indicated system control interrupt sources.
//! Only the sources that are enabled can be reflected to the processor
//! interrupt; disabled sources have no effect on the processor.
//!
//! \note The interrupt sources vary based on the Tiva part in use.
//! Please consult the data sheet for the part you are using to determine
//! which interrupt sources are available.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlIntDisable(uint32_t ui32Ints)
{
    //
    // Disable the specified interrupts.
    //
    HWREG(SYSCTL_IMC) &= ~(ui32Ints);
}

//*****************************************************************************
//
//! Clears system control interrupt sources.
//!
//! \param ui32Ints is a bit mask of the interrupt sources to be cleared.  Must
//! be a logical OR of \b SYSCTL_INT_PLL_LOCK, \b SYSCTL_INT_CUR_LIMIT,
//! \b SYSCTL_INT_IOSC_FAIL, \b SYSCTL_INT_MOSC_FAIL, \b SYSCTL_INT_POR,
//! \b SYSCTL_INT_BOR, and/or \b SYSCTL_INT_PLL_FAIL.
//!
//! The specified system control interrupt sources are cleared, so that they no
//! longer assert.  This function must be called in the interrupt handler to
//! keep it from being called again immediately upon exit.
//!
//! \note Because there is a write buffer in the Cortex-M processor, it may
//! take several clock cycles before the interrupt source is actually cleared.
//! Therefore, it is recommended that the interrupt source be cleared early in
//! the interrupt handler (as opposed to the very last action) to avoid
//! returning from the interrupt handler before the interrupt source is
//! actually cleared.  Failure to do so may result in the interrupt handler
//! being immediately reentered (because the interrupt controller still sees
//! the interrupt source asserted).
//!
//! \note The interrupt sources vary based on the Tiva part in use.
//! Please consult the data sheet for the part you are using to determine
//! which interrupt sources are available.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlIntClear(uint32_t ui32Ints)
{
    //
    // Clear the requested interrupt sources.
    //
    HWREG(SYSCTL_MISC) = ui32Ints;
}

//*****************************************************************************
//
//! Gets the current interrupt status.
//!
//! \param bMasked is false if the raw interrupt status is required and true if
//! the masked interrupt status is required.
//!
//! This function returns the interrupt status for the system controller.
//! Either the raw interrupt status or the status of interrupts that are
//! allowed to reflect to the processor can be returned.
//!
//! \note The interrupt sources vary based on the Tiva part in use.
//! Please consult the data sheet for the part you are using to determine
//! which interrupt sources are available.
//!
//! \return The current interrupt status, enumerated as a bit field of
//! \b SYSCTL_INT_PLL_LOCK, \b SYSCTL_INT_CUR_LIMIT, \b SYSCTL_INT_IOSC_FAIL,
//! \b SYSCTL_INT_MOSC_FAIL, \b SYSCTL_INT_POR, \b SYSCTL_INT_BOR, and
//! \b SYSCTL_INT_PLL_FAIL.
//
//*****************************************************************************
uint32_t
SysCtlIntStatus(bool bMasked)
{
    //
    // Return either the interrupt status or the raw interrupt status as
    // requested.
    //
    if(bMasked)
    {
        return(HWREG(SYSCTL_MISC));
    }
    else
    {
        return(HWREG(SYSCTL_RIS));
    }
}

//*****************************************************************************
//
//! Resets the device.
//!
//! This function performs a software reset of the entire device.  The
//! processor and all peripherals are reset and all device registers are
//! returned to their default values (with the exception of the reset cause
//! register, which maintains its current value but has the software reset
//! bit set as well).
//!
//! \return This function does not return.
//
//*****************************************************************************
void
SysCtlReset(void)
{
    //
    // Perform a software reset request.  This request causes the device to
    // reset, no further code is executed.
    //
    HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;

    //
    // The device should have reset, so this should never be reached.  Just in
    // case, loop forever.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
//! Puts the processor into sleep mode.
//!
//! This function places the processor into sleep mode; it does not return
//! until the processor returns to run mode.  The peripherals that are enabled
//! via SysCtlPeripheralSleepEnable() continue to operate and can wake up the
//! processor (if automatic clock gating is enabled with
//! SysCtlPeripheralClockGating(), otherwise all peripherals continue to
//! operate).
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlSleep(void)
{
    //
    // Wait for an interrupt.
    //
    CPUwfi();
}

//*****************************************************************************
//
//! Puts the processor into deep-sleep mode.
//!
//! This function places the processor into deep-sleep mode; it does not return
//! until the processor returns to run mode.  The peripherals that are enabled
//! via SysCtlPeripheralDeepSleepEnable() continue to operate and can wake up
//! the processor (if automatic clock gating is enabled with
//! SysCtlPeripheralClockGating(), otherwise all peripherals continue to
//! operate).
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlDeepSleep(void)
{
    //
    // Enable deep-sleep.
    //
    HWREG(NVIC_SYS_CTRL) |= NVIC_SYS_CTRL_SLEEPDEEP;

    //
    // Wait for an interrupt.
    //
    CPUwfi();

    //
    // Disable deep-sleep so that a future sleep works correctly.
    //
    HWREG(NVIC_SYS_CTRL) &= ~(NVIC_SYS_CTRL_SLEEPDEEP);
}

//*****************************************************************************
//
//! Gets the reason for a reset.
//!
//! This function returns the reason(s) for a reset.  Because the reset
//! reasons are sticky until either cleared by software or a power-on reset,
//! multiple reset reasons may be returned if multiple resets have occurred.
//! The reset reason is a logical OR of \b SYSCTL_CAUSE_LDO,
//! \b SYSCTL_CAUSE_SW, \b SYSCTL_CAUSE_WDOG, \b SYSCTL_CAUSE_BOR,
//! \b SYSCTL_CAUSE_POR, and/or \b SYSCTL_CAUSE_EXT.
//!
//! \return Returns the reason(s) for a reset.
//
//*****************************************************************************
uint32_t
SysCtlResetCauseGet(void)
{
    //
    // Return the reset reasons.
    //
    return(HWREG(SYSCTL_RESC));
}

//*****************************************************************************
//
//! Clears reset reasons.
//!
//! \param ui32Causes are the reset causes to be cleared; must be a logical OR
//! of \b SYSCTL_CAUSE_LDO, \b SYSCTL_CAUSE_SW, \b SYSCTL_CAUSE_WDOG,
//! \b SYSCTL_CAUSE_BOR, \b SYSCTL_CAUSE_POR, and/or \b SYSCTL_CAUSE_EXT.
//!
//! This function clears the specified sticky reset reasons.  Once cleared,
//! another reset for the same reason can be detected, and a reset for a
//! different reason can be distinguished (instead of having two reset causes
//! set).  If the reset reason is used by an application, all reset causes
//! should be cleared after they are retrieved with SysCtlResetCauseGet().
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlResetCauseClear(uint32_t ui32Causes)
{
    //
    // Clear the given reset reasons.
    //
    HWREG(SYSCTL_RESC) &= ~(ui32Causes);
}

//*****************************************************************************
//
//! Provides a small delay.
//!
//! \param ui32Count is the number of delay loop iterations to perform.
//!
//! This function provides a means of generating a constant length delay.  It
//! is written in assembly to keep the delay consistent across tool chains,
//! avoiding the need to tune the delay based on the tool chain in use.
//!
//! The loop takes 3 cycles/loop.
//!
//! \return None.
//
//*****************************************************************************
#if defined(ewarm) || defined(DOXYGEN)
void
SysCtlDelay(uint32_t ui32Count)
{
    __asm("    subs    r0, #1\n"
          "    bne.n   SysCtlDelay\n"
          "    bx      lr");
}
#endif
#if defined(codered) || defined(gcc) || defined(sourcerygxx)
void __attribute__((naked))
SysCtlDelay(uint32_t ui32Count)
{
    __asm("    subs    r0, #1\n"
          "    bne     SysCtlDelay\n"
          "    bx      lr");
}
#endif
#if defined(rvmdk) || defined(__ARMCC_VERSION)
__asm void
SysCtlDelay(uint32_t ui32Count)
{
    subs    r0, #1;
    bne     SysCtlDelay;
    bx      lr;
}
#endif
//
// For CCS implement this function in pure assembly.  This prevents the TI
// compiler from doing funny things with the optimizer.
//
#if defined(ccs)
__asm("    .sect \".text:SysCtlDelay\"\n"
      "    .clink\n"
      "    .thumbfunc SysCtlDelay\n"
      "    .thumb\n"
      "    .global SysCtlDelay\n"
      "SysCtlDelay:\n"
      "    subs r0, #1\n"
      "    bne.n SysCtlDelay\n"
      "    bx lr\n");
#endif

//*****************************************************************************
//
//! Sets the configuration of the main oscillator (MOSC) control.
//!
//! \param ui32Config is the required configuration of the MOSC control.
//!
//! This function configures the control of the main oscillator.  The
//! \e ui32Config is specified as the logical OR of the following values:
//!
//! - \b SYSCTL_MOSC_VALIDATE enables the MOSC verification circuit that
//!   detects a failure of the main oscillator (such as a loss of the clock).
//! - \b SYSCTL_MOSC_INTERRUPT indicates that a MOSC failure should generate an
//!   interrupt instead of resetting the processor.
//! - \b SYSCTL_MOSC_NO_XTAL indicates that there is no crystal connected to
//!   the OSC0/OSC1 pins, allowing power consumption to be reduced.
//!
//! \note The availability of MOSC control varies based on the Tiva part
//! in use.  Please consult the data sheet for the part you are using to
//! determine whether this support is available.  In addition, the capability
//! of MOSC control varies based on the Tiva part in use.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlMOSCConfigSet(uint32_t ui32Config)
{
    //
    // Configure the MOSC control.
    //
    HWREG(SYSCTL_MOSCCTL) = ui32Config;
}

//*****************************************************************************
//
//! Calibrates the precision internal oscillator.
//!
//! \param ui32Type is the type of calibration to perform.
//!
//! This function performs a calibration of the PIOSC.  There are three types
//! of calibration available; the desired calibration type as specified in
//! \e ui32Type is one of:
//!
//! - \b SYSCTL_PIOSC_CAL_AUTO to perform automatic calibration using the
//!   32-kHz clock from the hibernate module as a reference.  This type is
//!   only possible on parts that have a hibernate module, and then only if
//!   it is enabled and the hibernate module's RTC is also enabled.
//!
//! - \b SYSCTL_PIOSC_CAL_FACT to reset the PIOSC calibration to the factory
//!   provided calibration.
//!
//! - \b SYSCTL_PIOSC_CAL_USER to set the PIOSC calibration to a user-supplied
//!   value.  The value to be used is ORed into the lower 7-bits of this value,
//!   with 0x40 being the ``nominal'' value (in other words, if everything were
//!   perfect, 0x40 provides exactly 16 MHz).  Values larger than 0x40
//!   slow down PIOSC, and values smaller than 0x40 speed up PIOSC.
//!
//! \return Returns 1 if the calibration was successful and 0 if it failed.
//
//*****************************************************************************
uint32_t
SysCtlPIOSCCalibrate(uint32_t ui32Type)
{
    //
    // Perform the requested calibration.  If performing user calibration, the
    // UTEN bit must be set with one write, then the UT field in a second
    // write, and the UPDATE bit in a final write.  For other calibration
    // types, a single write to set UPDATE or CAL is all that is required.
    //
    if(ui32Type & (SYSCTL_PIOSCCAL_UTEN | SYSCTL_PIOSCCAL_UPDATE))
    {
        HWREG(SYSCTL_PIOSCCAL) = ui32Type & SYSCTL_PIOSCCAL_UTEN;
        HWREG(SYSCTL_PIOSCCAL) =
            ui32Type & (SYSCTL_PIOSCCAL_UTEN | SYSCTL_PIOSCCAL_UT_M);
    }
    HWREG(SYSCTL_PIOSCCAL) = ui32Type;

    //
    // See if an automatic calibration was requested.
    //
    if(ui32Type & SYSCTL_PIOSCCAL_CAL)
    {
        //
        // Wait for the automatic calibration to complete.
        //
        while((HWREG(SYSCTL_PIOSCSTAT) & SYSCTL_PIOSCSTAT_CR_M) == 0)
        {
        }

        //
        // If the automatic calibration failed, return an error.
        //
        if((HWREG(SYSCTL_PIOSCSTAT) & SYSCTL_PIOSCSTAT_CR_M) !=
           SYSCTL_PIOSCSTAT_CRPASS)
        {
            return(0);
        }
    }

    //
    // The calibration was successful.
    //
    return(1);
}
    uint32_t ui32SysDiv, ui32Osc, ui32OscSelect, ui32RSClkConfig;
    bool bNewPLL;


//*****************************************************************************
//
//! Sets the clocking of the device.
//!
//! \param ui32Config is the required configuration of the device clocking.
//!
//! This function configures the clocking of the device.  The input crystal
//! frequency, oscillator to be used, use of the PLL, and the system clock
//! divider are all configured with this function.
//!
//! The \e ui32Config parameter is the logical OR of several different values,
//! many of which are grouped into sets where only one can be chosen.
//!
//! The system clock divider is chosen with one of the following values:
//! \b SYSCTL_SYSDIV_1, \b SYSCTL_SYSDIV_2, \b SYSCTL_SYSDIV_3, ...
//! \b SYSCTL_SYSDIV_64.
//!
//! The use of the PLL is chosen with either \b SYSCTL_USE_PLL or
//! \b SYSCTL_USE_OSC.
//!
//! The external crystal frequency is chosen with one of the following values:
//! \b SYSCTL_XTAL_4MHZ, \b SYSCTL_XTAL_4_09MHZ, \b SYSCTL_XTAL_4_91MHZ,
//! \b SYSCTL_XTAL_5MHZ, \b SYSCTL_XTAL_5_12MHZ, \b SYSCTL_XTAL_6MHZ,
//! \b SYSCTL_XTAL_6_14MHZ, \b SYSCTL_XTAL_7_37MHZ, \b SYSCTL_XTAL_8MHZ,
//! \b SYSCTL_XTAL_8_19MHZ, \b SYSCTL_XTAL_10MHZ, \b SYSCTL_XTAL_12MHZ,
//! \b SYSCTL_XTAL_12_2MHZ, \b SYSCTL_XTAL_13_5MHZ, \b SYSCTL_XTAL_14_3MHZ,
//! \b SYSCTL_XTAL_16MHZ, \b SYSCTL_XTAL_16_3MHZ, \b SYSCTL_XTAL_18MHZ,
//! \b SYSCTL_XTAL_20MHZ, \b SYSCTL_XTAL_24MHZ, or \b SYSCTL_XTAL_25MHz.
//! Values below \b SYSCTL_XTAL_5MHZ are not valid when the PLL is in
//! operation.
//!
//! The oscillator source is chosen with one of the following values:
//! \b SYSCTL_OSC_MAIN, \b SYSCTL_OSC_INT, \b SYSCTL_OSC_INT4,
//! \b SYSCTL_OSC_INT30, or \b SYSCTL_OSC_EXT32.  \b SYSCTL_OSC_EXT32 is only
//! available on devices with the hibernate module, and then only when the
//! hibernate module has been enabled.
//!
//! The internal and main oscillators are disabled with the
//! \b SYSCTL_INT_OSC_DIS and \b SYSCTL_MAIN_OSC_DIS flags, respectively.
//! The external oscillator must be enabled in order to use an external clock
//! source.  Note that attempts to disable the oscillator used to clock the
//! device is prevented by the hardware.
//!
//! To clock the system from an external source (such as an external crystal
//! oscillator), use \b SYSCTL_USE_OSC \b | \b SYSCTL_OSC_MAIN.  To clock the
//! system from the main oscillator, use \b SYSCTL_USE_OSC \b |
//! \b SYSCTL_OSC_MAIN.  To clock the system from the PLL, use
//! \b SYSCTL_USE_PLL \b | \b SYSCTL_OSC_MAIN, and select the appropriate
//! crystal with one of the \b SYSCTL_XTAL_xxx values.
//!
//! \note If selecting the PLL as the system clock source (that is, via
//! \b SYSCTL_USE_PLL), this function polls the PLL lock interrupt to
//! determine when the PLL has locked.  If an interrupt handler for the
//! system control interrupt is in place, and it responds to and clears the
//! PLL lock interrupt, this function delays until its timeout has occurred
//! instead of completing as soon as PLL lock is achieved.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlClockSet(uint32_t ui32Config)
{
    uint32_t ui32Delay, ui32RCC, ui32RCC2;

    //
    // Get the current value of the RCC and RCC2 registers.
    //
    ui32RCC = HWREG(SYSCTL_RCC);
    ui32RCC2 = HWREG(SYSCTL_RCC2);

    //
    // Bypass the PLL and system clock dividers for now.
    //
    ui32RCC |= SYSCTL_RCC_BYPASS;
    ui32RCC &= ~(SYSCTL_RCC_USESYSDIV);
    ui32RCC2 |= SYSCTL_RCC2_BYPASS2;

    //
    // Write the new RCC value.
    //
    HWREG(SYSCTL_RCC) = ui32RCC;
    HWREG(SYSCTL_RCC2) = ui32RCC2;

    //
    // See if the oscillator needs to be enabled.
    //
    if((ui32RCC & SYSCTL_RCC_MOSCDIS) && !(ui32Config & SYSCTL_RCC_MOSCDIS))
    {
        //
        // Make sure that the required oscillators are enabled.  For now, the
        // previously enabled oscillators must be enabled along with the newly
        // requested oscillators.
        //
        ui32RCC &= (~SYSCTL_RCC_MOSCDIS | (ui32Config & SYSCTL_RCC_MOSCDIS));

        //
        // Write the new RCC value.
        //
        HWREG(SYSCTL_RCC) = ui32RCC;

        //
        // Wait for a bit, giving the oscillator time to stabilize.  The number
        // of iterations is adjusted based on the current clock source; a
        // smaller number of iterations is required for slower clock rates.
        //
        if(((ui32RCC2 & SYSCTL_RCC2_USERCC2) &&
            (((ui32RCC2 & SYSCTL_RCC2_OSCSRC2_M) == SYSCTL_RCC2_OSCSRC2_30) ||
             ((ui32RCC2 & SYSCTL_RCC2_OSCSRC2_M) ==
              SYSCTL_RCC2_OSCSRC2_32))) ||
           (!(ui32RCC2 & SYSCTL_RCC2_USERCC2) &&
            ((ui32RCC & SYSCTL_RCC_OSCSRC_M) == SYSCTL_RCC_OSCSRC_30)))
        {
            //
            // Delay for 4096 iterations.
            //
            SysCtlDelay(4096);
        }
        else
        {
            //
            // Delay for 524,288 iterations.
            //
            SysCtlDelay(524288);
        }
    }

    //
    // Set the new crystal value and oscillator source.  Because the OSCSRC2
    // field in RCC2 overlaps the XTAL field in RCC, the OSCSRC field has a
    // special encoding within ui32Config to avoid the overlap.
    //
    ui32RCC &= ~(SYSCTL_RCC_XTAL_M | SYSCTL_RCC_OSCSRC_M);
    ui32RCC |= ui32Config & (SYSCTL_RCC_XTAL_M | SYSCTL_RCC_OSCSRC_M);
    ui32RCC2 &= ~(SYSCTL_RCC2_USERCC2 | SYSCTL_RCC2_OSCSRC2_M);
    ui32RCC2 |= ui32Config & (SYSCTL_RCC2_USERCC2 | SYSCTL_RCC_OSCSRC_M);
    ui32RCC2 |= (ui32Config & 0x00000008) << 3;

    //
    // Write the new RCC value.
    //
    HWREG(SYSCTL_RCC) = ui32RCC;
    HWREG(SYSCTL_RCC2) = ui32RCC2;

    //
    // Wait for a bit so that new crystal value and oscillator source can take
    // effect.
    //
    SysCtlDelay(16);

    //
    // Set the PLL configuration.
    //
    ui32RCC &= ~SYSCTL_RCC_PWRDN;
    ui32RCC |= ui32Config & SYSCTL_RCC_PWRDN;
    ui32RCC2 &= ~SYSCTL_RCC2_PWRDN2;
    ui32RCC2 |= ui32Config & SYSCTL_RCC2_PWRDN2;

    //
    // Clear the PLL lock interrupt.
    //
    HWREG(SYSCTL_MISC) = SYSCTL_INT_PLL_LOCK;

    //
    // Write the new RCC value.
    //
    if(ui32RCC2 & SYSCTL_RCC2_USERCC2)
    {
        HWREG(SYSCTL_RCC2) = ui32RCC2;
        HWREG(SYSCTL_RCC) = ui32RCC;
    }
    else
    {
        HWREG(SYSCTL_RCC) = ui32RCC;
        HWREG(SYSCTL_RCC2) = ui32RCC2;
    }

    //
    // Set the requested system divider and disable the appropriate
    // oscillators.  This value is not written immediately.
    //
    ui32RCC &= ~(SYSCTL_RCC_SYSDIV_M | SYSCTL_RCC_USESYSDIV |
                 SYSCTL_RCC_MOSCDIS);
    ui32RCC |= ui32Config & (SYSCTL_RCC_SYSDIV_M | SYSCTL_RCC_USESYSDIV |
                             SYSCTL_RCC_MOSCDIS);
    ui32RCC2 &= ~(SYSCTL_RCC2_SYSDIV2_M);
    ui32RCC2 |= ui32Config & SYSCTL_RCC2_SYSDIV2_M;
    if(ui32Config & SYSCTL_RCC2_DIV400)
    {
        ui32RCC |= SYSCTL_RCC_USESYSDIV;
        ui32RCC2 &= ~(SYSCTL_RCC_USESYSDIV);
        ui32RCC2 |= ui32Config & (SYSCTL_RCC2_DIV400 | SYSCTL_RCC2_SYSDIV2LSB);
    }
    else
    {
        ui32RCC2 &= ~(SYSCTL_RCC2_DIV400);
    }

    //
    // See if the PLL output is being used to clock the system.
    //
    if(!(ui32Config & SYSCTL_RCC_BYPASS))
    {
        //
        // Wait until the PLL has locked.
        //
        for(ui32Delay = 32768; ui32Delay > 0; ui32Delay--)
        {
            if(HWREG(SYSCTL_RIS) & SYSCTL_INT_PLL_LOCK)
            {
                break;
            }
        }

        //
        // Enable use of the PLL.
        //
        ui32RCC &= ~(SYSCTL_RCC_BYPASS);
        ui32RCC2 &= ~(SYSCTL_RCC2_BYPASS2);
    }

    //
    // Write the final RCC value.
    //
    HWREG(SYSCTL_RCC) = ui32RCC;
    HWREG(SYSCTL_RCC2) = ui32RCC2;

    //
    // Delay for a little bit so that the system divider takes effect.
    //
    SysCtlDelay(16);
}

//*****************************************************************************
//
//! Gets the processor clock rate.
//!
//! This function determines the clock rate of the processor clock, which is
//! also the clock rate of the peripheral modules (with the exception of
//! PWM, which has its own clock divider; other peripherals may have different
//! clocking, see the device data sheet for details).
//!
//! \note This cannot return accurate results if SysCtlClockSet() has not
//! been called to configure the clocking of the device, or if the device is
//! directly clocked from a crystal (or a clock source) that is not one of the
//! supported crystal frequencies.  In the latter case, this function should be
//! modified to directly return the correct system clock rate.
//!
//! \return The processor clock rate.
//
//*****************************************************************************
uint32_t
SysCtlClockGet(void)
{
    uint32_t ui32RCC, ui32RCC2, ui32PLL, ui32Clk;
    uint32_t ui32PLL1;

    //
    // This function is only valid on Blizzard-class devices.
    //
    ASSERT(CLASS_IS_BLIZZARD);

    //
    // Read RCC and RCC2.
    //
    ui32RCC = HWREG(SYSCTL_RCC);
    ui32RCC2 = HWREG(SYSCTL_RCC2);

    //
    // Get the base clock rate.
    //
    switch((ui32RCC2 & SYSCTL_RCC2_USERCC2) ?
           (ui32RCC2 & SYSCTL_RCC2_OSCSRC2_M) :
           (ui32RCC & SYSCTL_RCC_OSCSRC_M))
    {
        //
        // The main oscillator is the clock source.  Determine its rate from
        // the crystal setting field.
        //
        case SYSCTL_RCC_OSCSRC_MAIN:
        {
            ui32Clk = g_pui32Xtals[(ui32RCC & SYSCTL_RCC_XTAL_M) >>
                                   SYSCTL_RCC_XTAL_S];
            break;
        }

        //
        // The internal oscillator is the source clock.
        //
        case SYSCTL_RCC_OSCSRC_INT:
        {
            //
            // The internal oscillator on all devices is 16 MHz.
            //
            ui32Clk = 16000000;
            break;
        }

        //
        // The internal oscillator divided by four is the source clock.
        //
        case SYSCTL_RCC_OSCSRC_INT4:
        {
            //
            // The internal oscillator on all devices is 16 MHz.
            //
            ui32Clk = 16000000 / 4;
            break;
        }

        //
        // The internal 30-KHz oscillator is the source clock.
        //
        case SYSCTL_RCC_OSCSRC_30:
        {
            //
            // The internal 30-KHz oscillator has an accuracy of +/- 30%.
            //
            ui32Clk = 30000;
            break;
        }

        //
        // The 32.768-KHz clock from the hibernate module is the source clock.
        //
        case SYSCTL_RCC2_OSCSRC2_32:
        {
            ui32Clk = 32768;
            break;
        }

        //
        // An unknown setting, so return a zero clock (that is, an unknown
        // clock rate).
        //
        default:
        {
            return(0);
        }
    }

    //
    // See if the PLL is being used.
    //
    if(((ui32RCC2 & SYSCTL_RCC2_USERCC2) &&
        !(ui32RCC2 & SYSCTL_RCC2_BYPASS2)) ||
       (!(ui32RCC2 & SYSCTL_RCC2_USERCC2) && !(ui32RCC & SYSCTL_RCC_BYPASS)))
    {
        //
        // Read the two PLL frequency registers.  The formula for a
        // Blizzard-class device is "(xtal * m) / ((q + 1) * (n + 1))".
        //
        ui32PLL = HWREG(SYSCTL_PLLFREQ0);
        ui32PLL1 = HWREG(SYSCTL_PLLFREQ1);

        //
        // Divide the input clock by the dividers.
        //
        ui32Clk /= ((((ui32PLL1 & SYSCTL_PLLFREQ1_Q_M) >>
                      SYSCTL_PLLFREQ1_Q_S) + 1) *
                    (((ui32PLL1 & SYSCTL_PLLFREQ1_N_M) >>
                      SYSCTL_PLLFREQ1_N_S) + 1) * 2);

        //
        // Multiply the clock by the multiplier, which is split into an
        // integer part and a fractional part.
        //
        ui32Clk = ((ui32Clk * ((ui32PLL & SYSCTL_PLLFREQ0_MINT_M) >>
                               SYSCTL_PLLFREQ0_MINT_S)) +
                   ((ui32Clk * ((ui32PLL & SYSCTL_PLLFREQ0_MFRAC_M) >>
                                SYSCTL_PLLFREQ0_MFRAC_S)) >> 10));

        //
        // Force the system divider to be enabled.  It is always used when
        // using the PLL, but in some cases it does not read as being enabled.
        //
        ui32RCC |= SYSCTL_RCC_USESYSDIV;
    }

    //
    // See if the system divider is being used.
    //
    if(ui32RCC & SYSCTL_RCC_USESYSDIV)
    {
        //
        // Adjust the clock rate by the system clock divider.
        //
        if(ui32RCC2 & SYSCTL_RCC2_USERCC2)
        {
            if((ui32RCC2 & SYSCTL_RCC2_DIV400) &&
               (((ui32RCC2 & SYSCTL_RCC2_USERCC2) &&
                 !(ui32RCC2 & SYSCTL_RCC2_BYPASS2)) ||
                (!(ui32RCC2 & SYSCTL_RCC2_USERCC2) &&
                 !(ui32RCC & SYSCTL_RCC_BYPASS))))

            {
                ui32Clk = ((ui32Clk * 2) / (((ui32RCC2 &
                                              (SYSCTL_RCC2_SYSDIV2_M |
                                               SYSCTL_RCC2_SYSDIV2LSB)) >>
                                             (SYSCTL_RCC2_SYSDIV2_S - 1)) +
                                            1));
            }
            else
            {
                ui32Clk /= (((ui32RCC2 & SYSCTL_RCC2_SYSDIV2_M) >>
                             SYSCTL_RCC2_SYSDIV2_S) + 1);
            }
        }
        else
        {
            ui32Clk /= (((ui32RCC & SYSCTL_RCC_SYSDIV_M) >>
                         SYSCTL_RCC_SYSDIV_S) + 1);
        }
    }

    //
    // Return the computed clock rate.
    //
    return(ui32Clk);
}

//*****************************************************************************
//
//! Sets the clocking of the device while in deep-sleep mode.
//!
//! \param ui32Config is the required configuration of the device clocking
//! while in deep-sleep mode.
//!
//! This function configures the clocking of the device while in deep-sleep
//! mode.  The oscillator to be used and the system clock divider are
//! configured with this function.
//!
//! The \e ui32Config parameter is the logical OR of the following values:
//!
//! The system clock divider is chosen from one of the following values:
//! \b SYSCTL_DSLP_DIV_1, \b SYSCTL_DSLP_DIV_2, \b SYSCTL_DSLP_DIV_3, ...
//! \b SYSCTL_DSLP_DIV_64.
//!
//! The oscillator source is chosen from one of the following values:
//! \b SYSCTL_DSLP_OSC_MAIN, \b SYSCTL_DSLP_OSC_INT, \b SYSCTL_DSLP_OSC_INT30,
//! or \b SYSCTL_DSLP_OSC_EXT32.  \b SYSCTL_OSC_EXT32 is only available on
//! devices with the hibernation module, and then only when the hibernation
//! module has been enabled.
//!
//! The precision internal oscillator can be powered down in deep-sleep mode by
//! specifying \b SYSCTL_DSLP_PIOSC_PD.  The precision internal oscillator is
//! not powered down if it is required for operation while in deep-sleep
//! (based on other configuration settings.)
//! \note The availability of deep-sleep clocking configuration varies with the
//! Tiva part in use.  Please consult the data sheet for the part you are
//! using to determine whether this support is available.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlDeepSleepClockSet(uint32_t ui32Config)
{
    //
    // Set the deep-sleep clock configuration.
    //
    HWREG(SYSCTL_DSLPCLKCFG) = ui32Config;
}

//*****************************************************************************
//
//! Sets the PWM clock configuration.
//!
//! \param ui32Config is the configuration for the PWM clock; it must be one of
//! \b SYSCTL_PWMDIV_1, \b SYSCTL_PWMDIV_2, \b SYSCTL_PWMDIV_4,
//! \b SYSCTL_PWMDIV_8, \b SYSCTL_PWMDIV_16, \b SYSCTL_PWMDIV_32, or
//! \b SYSCTL_PWMDIV_64.
//!
//! This function configures the rate of the clock provided to the PWM module
//! as a ratio of the processor clock.  This clock is used by the PWM module to
//! generate PWM signals; its rate forms the basis for all PWM signals.
//!
//! \note The clocking of the PWM is dependent upon the system clock rate as
//! configured by SysCtlClockSet().
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlPWMClockSet(uint32_t ui32Config)
{
    //
    // Check the arguments.
    //
    ASSERT((ui32Config == SYSCTL_PWMDIV_1) ||
           (ui32Config == SYSCTL_PWMDIV_2) ||
           (ui32Config == SYSCTL_PWMDIV_4) ||
           (ui32Config == SYSCTL_PWMDIV_8) ||
           (ui32Config == SYSCTL_PWMDIV_16) ||
           (ui32Config == SYSCTL_PWMDIV_32) ||
           (ui32Config == SYSCTL_PWMDIV_64));

    //
    // Check that there is a PWM block on this part.
    //
    ASSERT(HWREG(SYSCTL_DC1) & (SYSCTL_DC1_PWM0 | SYSCTL_DC1_PWM1));

    //
    // Set the PWM clock configuration into the run-mode clock configuration
    // register.
    //
    HWREG(SYSCTL_RCC) = ((HWREG(SYSCTL_RCC) &
                          ~(SYSCTL_RCC_USEPWMDIV | SYSCTL_RCC_PWMDIV_M)) |
                         ui32Config);
}

//*****************************************************************************
//
//! Gets the current PWM clock configuration.
//!
//! This function returns the current PWM clock configuration.
//!
//! \return Returns the current PWM clock configuration; is one of
//! \b SYSCTL_PWMDIV_1, \b SYSCTL_PWMDIV_2, \b SYSCTL_PWMDIV_4,
//! \b SYSCTL_PWMDIV_8, \b SYSCTL_PWMDIV_16, \b SYSCTL_PWMDIV_32, or
//! \b SYSCTL_PWMDIV_64.
//
//*****************************************************************************
uint32_t
SysCtlPWMClockGet(void)
{
    //
    // Check that there is a PWM block on this part.
    //
    ASSERT(HWREG(SYSCTL_DC1) & (SYSCTL_DC1_PWM0 | SYSCTL_DC1_PWM1));

    //
    // Return the current PWM clock configuration.  Make sure that
    // SYSCTL_PWMDIV_1 is returned in all cases where the divider is disabled.
    //
    if(!(HWREG(SYSCTL_RCC) & SYSCTL_RCC_USEPWMDIV))
    {
        //
        // The divider is not active so reflect this in the value we return.
        //
        return(SYSCTL_PWMDIV_1);
    }
    else
    {
        //
        // The divider is active so directly return the masked register value.
        //
        return(HWREG(SYSCTL_RCC) &
               (SYSCTL_RCC_USEPWMDIV | SYSCTL_RCC_PWMDIV_M));
    }
}

//*****************************************************************************
//
//! Sets the sample rate of the ADC.
//!
//! \param ui32Speed is the desired sample rate of the ADC; must be one of
//! \b SYSCTL_ADCSPEED_1MSPS, \b SYSCTL_ADCSPEED_500KSPS,
//! \b SYSCTL_ADCSPEED_250KSPS, or \b SYSCTL_ADCSPEED_125KSPS.
//!
//! This function configures the rate at which the ADC samples are captured by
//! the ADC block.  The sampling speed may be limited by the hardware, so the
//! sample rate may end up being slower than requested.  SysCtlADCSpeedGet()
//! returns the actual speed in use.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlADCSpeedSet(uint32_t ui32Speed)
{
    //
    // Check the arguments.
    //
    ASSERT((ui32Speed == SYSCTL_ADCSPEED_1MSPS) ||
           (ui32Speed == SYSCTL_ADCSPEED_500KSPS) ||
           (ui32Speed == SYSCTL_ADCSPEED_250KSPS) ||
           (ui32Speed == SYSCTL_ADCSPEED_125KSPS));

    //
    // Set the ADC speed in run and sleep mode.
    //
    HWREG(SYSCTL_RCGC0) = ((HWREG(SYSCTL_RCGC0) & ~(SYSCTL_RCGC0_ADC1SPD_M |
                                                    SYSCTL_RCGC0_ADC0SPD_M)) |
                           ui32Speed);
    HWREG(SYSCTL_SCGC0) = ((HWREG(SYSCTL_SCGC0) & ~(SYSCTL_SCGC0_ADCSPD_M)) |
                           ui32Speed);
}

//*****************************************************************************
//
//! Gets the sample rate of the ADC.
//!
//! This function gets the current sample rate of the ADC.
//!
//! \return Returns the current ADC sample rate; is one of
//! \b SYSCTL_ADCSPEED_1MSPS, \b SYSCTL_ADCSPEED_500KSPS,
//! \b SYSCTL_ADCSPEED_250KSPS, or \b SYSCTL_ADCSPEED_125KSPS.
//
//*****************************************************************************
uint32_t
SysCtlADCSpeedGet(void)
{
    //
    // Return the current ADC speed.
    //
    return(HWREG(SYSCTL_RCGC0) & (SYSCTL_RCGC0_ADC1SPD_M |
                                  SYSCTL_RCGC0_ADC0SPD_M));
}

//*****************************************************************************
//
//! Enables access to a GPIO peripheral via the AHB.
//!
//! \param ui32GPIOPeripheral is the GPIO peripheral to enable.
//!
//! This function is used to enable the specified GPIO peripheral to be
//! accessed from the Advanced Host Bus (AHB) instead of the legacy Advanced
//! Peripheral Bus (APB).  When a GPIO peripheral is enabled for AHB access,
//! the \b _AHB_BASE form of the base address should be used for GPIO
//! functions.  For example, instead of using \b GPIO_PORTA_BASE as the base
//! address for GPIO functions, use \b GPIO_PORTA_AHB_BASE instead.
//!
//! The \e ui32GPIOPeripheral argument must be only one of the following
//! values:
//! \b SYSCTL_PERIPH_GPIOA, \b SYSCTL_PERIPH_GPIOB, \b SYSCTL_PERIPH_GPIOC,
//! \b SYSCTL_PERIPH_GPIOD, \b SYSCTL_PERIPH_GPIOE, \b SYSCTL_PERIPH_GPIOF,
//! \b SYSCTL_PERIPH_GPIOG, \b SYSCTL_PERIPH_GPIOH, or \b SYSCTL_PERIPH_GPIOJ.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlGPIOAHBEnable(uint32_t ui32GPIOPeripheral)
{
    //
    // Check the arguments.
    //
    ASSERT((ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOA) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOB) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOC) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOD) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOE) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOF) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOG) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOH) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOJ));

    //
    // Enable this GPIO for AHB access.
    //
    HWREG(SYSCTL_GPIOHBCTL) |= (1 << (ui32GPIOPeripheral & 0xF));
}

//*****************************************************************************
//
//! Disables access to a GPIO peripheral via the AHB.
//!
//! \param ui32GPIOPeripheral is the GPIO peripheral to disable.
//!
//! This function disables the specified GPIO peripheral for access from the
//! Advanced Host Bus (AHB).  Once disabled, the GPIO peripheral is accessed
//! from the legacy Advanced Peripheral Bus (APB).
//!
//! The \b ui32GPIOPeripheral argument must be only one of the following
//! values:
//! \b SYSCTL_PERIPH_GPIOA, \b SYSCTL_PERIPH_GPIOB, \b SYSCTL_PERIPH_GPIOC,
//! \b SYSCTL_PERIPH_GPIOD, \b SYSCTL_PERIPH_GPIOE, \b SYSCTL_PERIPH_GPIOF,
//! \b SYSCTL_PERIPH_GPIOG, \b SYSCTL_PERIPH_GPIOH, or \b SYSCTL_PERIPH_GPIOJ.
//!
//! \note Some devices allow disabling AHB access to GPIO ports that are only
//! present on the AHB.  Disabling AHB access to these ports will disable
//! access to these GPIO ports.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlGPIOAHBDisable(uint32_t ui32GPIOPeripheral)
{
    //
    // Check the arguments.
    //
    ASSERT((ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOA) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOB) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOC) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOD) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOE) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOF) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOG) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOH) ||
           (ui32GPIOPeripheral == SYSCTL_PERIPH_GPIOJ));

    //
    // Disable this GPIO for AHB access.
    //
    HWREG(SYSCTL_GPIOHBCTL) &= ~(1 << (ui32GPIOPeripheral & 0xF));
}

//*****************************************************************************
//
//! Powers up the USB PLL.
//!
//! This function enables the USB controller's PLL, which is used by its
//! physical layer.  This call is necessary before connecting to any external
//! devices.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlUSBPLLEnable(void)
{
    //
    // Turn on the USB PLL.
    //
    HWREG(SYSCTL_RCC2) &= ~SYSCTL_RCC2_USBPWRDN;
}

//*****************************************************************************
//
//! Powers down the USB PLL.
//!
//! This function disables the USB controller's PLL, which is used by its
//! physical layer.  The USB registers are still accessible, but the physical
//! layer no longer functions.
//!
//! \return None.
//
//*****************************************************************************
void
SysCtlUSBPLLDisable(void)
{
    //
    // Turn off the USB PLL.
    //
    HWREG(SYSCTL_RCC2) |= SYSCTL_RCC2_USBPWRDN;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
