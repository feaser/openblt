/************************************************************************************//**
* \file         tbx_random.c
* \brief        Random number generator source file.
* \details      The RNG algorithm is based on a linear feedback shift register (LFSR) as
*               presented in application note 4400 from Maxim Integrated. It can be found
*               at: www.maximintegrated.com/en/app-notes/index.mvp/id/4400.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019 by Feaser     www.feaser.com     All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
*
* SPDX-License-Identifier: MIT
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "microtbx.h"                            /* MicroTBX global header             */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Polynomial mask for the 32-bit LFSR. The polynomial mask is created by taking
 *         the binary representation of the polynomial and truncating the right-most bit.
 */
#define TBX_RANDOM_LFSR32_POLYMASK     (0xB4BCD35CUL)

/** \brief Polynomial mask for the 31-bit LFSR. The polynomial mask is created by taking
 *         the binary representation of the polynomial and truncating the right-most bit.
 */
#define TBX_RANDOM_LFSR31_POLYMASK     (0x7A5BC2E3UL)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void     TbxRandomInitLFSRs     (void);

static uint32_t TbxRandomShiftLFSR     (uint32_t * lfsr,
                                        uint32_t   polymask);

static uint16_t TbxRandomNumber16BitGet(void);


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Pointer to the application provided seed initialization handler function. */
static tTbxRandomSeedInitHandler tbxRandomSeedInitHandler = NULL;

/** \brief Storage for the 32-bit LFSR value. */
static uint32_t                  tbxRandomNumberLFSR32;

/** \brief Storage for the 31-bit LFSR value. */
static uint32_t                  tbxRandomNumberLFSR31;


/************************************************************************************//**
** \brief     Obtains a random number.
** \return    Value of the newly generated random number.
**
****************************************************************************************/
uint32_t TbxRandomNumberGet(void)
{
  uint32_t       result;
  static uint8_t seeded = TBX_FALSE;

  /* Make sure both the 32-bit and 31-bit LFSRs are seeded. */
  if (seeded == TBX_FALSE)
  {
    TbxRandomInitLFSRs();
    seeded = TBX_TRUE;
  }

  /* Construct a 32-bit random number by combining two 16-bit random numbers. */
  result  = ((uint32_t)TbxRandomNumber16BitGet() << 16UL);
  result |= TbxRandomNumber16BitGet();

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxRandomNumberGet ***/


/************************************************************************************//**
** \brief     Sets the application specific function that should be called when the
**            seed for the random number generation should be initialized. The actual
**            value is not really important as long as it is a value that is different
**            every time the software program runs, so after each reset event.
** \details   Here are a few different approaches that could be implemented to realize
**            an seed value that is different each time the software program runs:
**            1. Keep one of the analog inputs on your microcontroller unconnected. Such
**               a floating analog input picks up noise from the surrounding environment
**               and readings will therefore always vary slightly. Configure your ADC
**               peripheral to perform an analog to digital conversion of this analog
**               input and use its result value to seed the generator.
**            2. If your microcontroller has EEPROM or a non-volatile register, you can
**               use it to store a value that you use to seed the generator. Then simply
**               increment its value by one upon software program startup. This way you
**               have a different seed value every time your software program is started.
**               * Keep in mind though that these data storage options have a limited
**                 amount of write cycles. A better option might be to use external FRAM.
**            3. If your microcontroller has access to an external file system such as an
**               SD-card, you could store a value, which you use to seed the generator,
**               in a file. Then simply increment its value by one upon software program
**               startup. This way you have a different seed value every time your
**               software program is started.
**            4. If your microcontroller based system features a battery backed real-time
**               clock, you could use the current date/time value to seed the generator
**               algorithm.
** \param     seedInitHandler Pointer to the application specific seed initialization
**            handler to use instead of the default internal handler.
**
****************************************************************************************/
void TbxRandomSetSeedInitHandler(tTbxRandomSeedInitHandler seedInitHandler)
{
  /* Verify parameter. */
  TBX_ASSERT(seedInitHandler != NULL);

  /* Only continue if the parameters are valid. */
  if (seedInitHandler != NULL)
  {
    /* Store the pointer to the application specific seed initialization handler. */
    tbxRandomSeedInitHandler = seedInitHandler;
  }
} /*** end of TbxRandomSetSeedInitHandler ***/


/************************************************************************************//**
** \brief     Initialize both the 32-bit and 31-bit LFSRs with a non-zero seed value.
** \return    none.
**
****************************************************************************************/
static void TbxRandomInitLFSRs(void)
{
  uint32_t seedLFSR32 = 0xABCDEUL;
  uint32_t seedLFSR31 = 0x23456789UL;

  /* Request the application to fill in the seed values, if it registered a handler for
   * this.
   */
  if (tbxRandomSeedInitHandler != NULL)
  {
    /* Call the application specific seed initialization handler. */
    seedLFSR32 = tbxRandomSeedInitHandler();
    seedLFSR31 = tbxRandomSeedInitHandler();
  }

  /* Make sure the LFSRs seeds are a non-zero value. */
  if (seedLFSR32 == 0U)
  {
    seedLFSR32 = 0xABCDEUL;
  }
  if (seedLFSR31 == 0U)
  {
    seedLFSR31 = 0x23456789UL;
  }

  /* Seed the 32-bit and 31-bit LFSRs. Note that this function is only called once
   * the first time a random number is generated, so there is no need to use a
   * critical section for writing to the LFSR variables.
   */
  tbxRandomNumberLFSR32 = seedLFSR32;
  tbxRandomNumberLFSR31 = seedLFSR31;
} /*** end of TbxRandomInitLFSRs ***/


/************************************************************************************//**
** \brief     Perform the actual linear feedback shift operation.
** \param     lfsr Pointer to the current value of the LFSR. The newly generated value
**            is also written to this pointer location.
** \param     polymask The 32-bit or 31-bit polynomial mask.
** \return    The newly generated LFSR value.
**
****************************************************************************************/
static uint32_t TbxRandomShiftLFSR(uint32_t * lfsr, 
                                   uint32_t   polymask)
{
  uint32_t result = 0;

  /* Verify parameter. */
  TBX_ASSERT(lfsr != NULL);

  /* Only continue if the parameters are valid. */
  if (lfsr != NULL)
  {
    /* Perform the linear feedback shift operation. */
    uint32_t feedback = *lfsr & 1U;
    *lfsr >>=1U;
    if (feedback == 1U)
    {
      *lfsr ^= polymask;
    }
    /* Assign the calculated LFSR to the result. */
    result = *lfsr;
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxRandomShiftLFSR ***/


/************************************************************************************//**
** \brief     Obtains a 16-bit random number.
** \return    Value of the newly generated 16-bit random number.
**
****************************************************************************************/
static uint16_t TbxRandomNumber16BitGet(void)
{
  uint16_t result;
  uint32_t lfsr32_second_shift;
  uint32_t lfsr31_first_shift;

  /* Obtain mutual exclusive access to the 32-bit and 31-bit global LFSRs. */
  TbxCriticalSectionEnter();
  /* Shifting the 32-bit LFSR more than once before getting a random number improves its
   * statistical properties. For this reason the 32-bit LFSR is shifted twice.
   */
  (void)TbxRandomShiftLFSR(&tbxRandomNumberLFSR32, TBX_RANDOM_LFSR32_POLYMASK);
  lfsr32_second_shift = TbxRandomShiftLFSR(&tbxRandomNumberLFSR32,
                                           TBX_RANDOM_LFSR32_POLYMASK);

  /* The relatively short periods of the LFSRs can be solved by XORing the values of two
   * or more different sized LFSRs together. For this reason a 31-bit LFSR was introduced
   * and is shifted here.
   */
  lfsr31_first_shift = TbxRandomShiftLFSR(&tbxRandomNumberLFSR31,
                                          TBX_RANDOM_LFSR31_POLYMASK);
  /* Release mutual exclusive access to the 32-bit and 31-bit global LFSRs. */
  TbxCriticalSectionExit();

  /* Construct the actual 16-bit random value by XORing the twice shifted 32-bit LFSR and
   * the once shifted 31-bit LFSR.
   */
  result = (uint16_t)((lfsr32_second_shift ^ lfsr31_first_shift) & 0xFFFFU);

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxRandomNumber16BitGet ***/


/*********************************** end of tbx_random.c *******************************/
