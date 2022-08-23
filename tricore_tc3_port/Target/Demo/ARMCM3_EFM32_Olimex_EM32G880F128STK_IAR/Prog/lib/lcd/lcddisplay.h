/**************************************************************************//**
 * @file
 * @brief LCD Controller font and display layout for EFM32 development MCU
 *        module
 * @author Energy Micro AS
 * @version 1.1.1 (adjusted for the Olimex display)
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2009 Energy Micro AS, http://www.energymicro.com</b>
 ******************************************************************************
 *
 * This source code is the property of Energy Micro AS. The source and compiled
 * code may only be used on Energy Micro "EFM32" microcontrollers.
 *
 * This copyright notice may not be removed from the source code nor changed.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
******************************************************************************/

#ifndef _LCDDISPLAY_H
#define _LCDDISPLAY_H

#include <stdint.h>
/**************************************************************************//**
 * @brief
 * Defines each text symbol's segment in terms of COM and BIT numbers,
 * in a way that we can enumerate each bit for each text segment in the
 * following bit pattern:
 * @verbatim
 *  -------0------
 *
 * |   \7  |8  /9 |
 * |5   \  |  /   |1
 *
 *  --6---  ---10--
 *
 * |    /  |  \11 |
 * |4  /13 |12 \  |2
 *
 *  -------3------  .(14)
 * @endverbatim
 * E.g.: First text character bit pattern #3 (above) is
 *  Segment 1D for Display
 *  Location COM 3, BIT 0
 *****************************************************************************/
typedef struct
{
  uint32_t com[15]; /**< LCD COM line (for multiplexing) */
  uint32_t bit[15]; /**< LCD bit number */
} CHAR_TypeDef;


/**************************************************************************//**
 * @brief Defines segment COM and BIT fields numeric display
 * Segments: A, B, C, D, E, F, G, dp
 *****************************************************************************/
typedef struct
{
  uint32_t com[8];
  uint32_t bit[8];
} NUMBER_TypeDef;

/**************************************************************************//**
 * @brief Defines segment COM and BIT fields for BATTERY symbol on display
 * Segments: BRBL, B0, B1, B2
 *****************************************************************************/
typedef struct
{
  uint32_t com[4]; /**< LCD COM line (for multiplexing) */
  uint32_t bit[4]; /**< LCD bit number */
} BATTERY_TypeDef;

/**************************************************************************//**
 * @brief Defines segment COM and BIT fields for POWER METER
 * Segments: PL, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, PR
 *****************************************************************************/
typedef struct
{
  uint32_t com[12]; /**< LCD COM line (for multiplexing) */
  uint32_t bit[12]; /**< LCD bit number */
} POWERMETER_TypeDef;

/**************************************************************************//**
 * @brief Defines segment COM and BIT fields for ARROWS
 * Segments: AD, AR, AU, AL
 *****************************************************************************/
typedef struct
{
  uint32_t com[4]; /**< LCD COM line (for multiplexing) */
  uint32_t bit[4]; /**< LCD bit number */
} ARROWS_TypeDef;

/**************************************************************************//**
 * @brief Defines prototype for all segments in display
 *****************************************************************************/
typedef struct
{
  CHAR_TypeDef			Text[7];
  NUMBER_TypeDef		Number[4];
  BATTERY_TypeDef		Battery;
  POWERMETER_TypeDef	PowerMeter;
  ARROWS_TypeDef		Arrows;
} MCU_DISPLAY;

/**************************************************************************//**
 * @brief Working instance of LCD display
 *****************************************************************************/
MCU_DISPLAY EFMDisplay = {
  .Text        = {
    { /* 1 */
      .com[0] = 3, .com[1] = 2, .com[2] = 1, .com[3] = 0,
      .bit[0] = 37, .bit[1] = 37, .bit[2] = 37, .bit[3] = 37,

      .com[4] = 1, .com[5] = 2, .com[6] = 2, .com[7] = 3,
      .bit[4] = 39, .bit[5] = 39, .bit[6] = 38, .bit[7] = 38,

      .com[8] = 3, .com[9] = 2, .com[10] = 1, .com[11] = 0,
      .bit[8] = 36, .bit[9] = 36, .bit[10] = 36, .bit[11] = 36,

      .com[12] = 0, .com[13] = 1, .com[14] = 0,
      .bit[12] = 38, .bit[13] = 38, .bit[14] = 35
    },
    { /* 2 */
      .com[0] = 3, .com[1] = 2, .com[2] = 1, .com[3] = 0,
      .bit[0] = 33, .bit[1] = 33, .bit[2] = 33, .bit[3] = 33,

      .com[4] = 1, .com[5] = 2, .com[6] = 2, .com[7] = 3,
      .bit[4] = 35, .bit[5] = 35, .bit[6] = 34, .bit[7] = 34,

      .com[8] = 3, .com[9] = 2, .com[10] = 1, .com[11] = 0,
      .bit[8] = 32, .bit[9] = 32, .bit[10] = 32, .bit[11] = 32,

      .com[12] = 0, .com[13] = 1, .com[14] = 0,
      .bit[12] = 34, .bit[13] = 34, .bit[14] = 31
    },
    { /* 3 */
      .com[0] = 3, .com[1] = 2, .com[2] = 1, .com[3] = 0,
      .bit[0] = 29, .bit[1] = 29, .bit[2] = 29, .bit[3] = 29,

      .com[4] = 1, .com[5] = 2, .com[6] = 2, .com[7] = 3,
      .bit[4] = 31, .bit[5] = 31, .bit[6] = 30, .bit[7] = 30,

      .com[8] = 3, .com[9] = 2, .com[10] = 1, .com[11] = 0,
      .bit[8] = 28, .bit[9] = 28, .bit[10] = 28, .bit[11] = 28,

      .com[12] = 0, .com[13] = 1, .com[14] = 0,
      .bit[12] = 30, .bit[13] = 30, .bit[14] = 27
    },
    { /* 4 */
      .com[0] = 3, .com[1] = 2, .com[2] = 1, .com[3] = 0,
      .bit[0] = 25, .bit[1] = 25, .bit[2] = 25, .bit[3] = 25,

      .com[4] = 1, .com[5] = 2, .com[6] = 2, .com[7] = 3,
      .bit[4] = 27, .bit[5] = 27, .bit[6] = 26, .bit[7] = 26,

      .com[8] = 3, .com[9] = 2, .com[10] = 1, .com[11] = 0,
      .bit[8] = 24, .bit[9] = 24, .bit[10] = 24, .bit[11] = 24,

      .com[12] = 0, .com[13] = 1, .com[14] = 0,
      .bit[12] = 26, .bit[13] = 26, .bit[14] = 23
    },	
    { /* 5 */
      .com[0] = 3, .com[1] = 2, .com[2] = 1, .com[3] = 0,
      .bit[0] = 21, .bit[1] = 21, .bit[2] = 21, .bit[3] = 21,

      .com[4] = 1, .com[5] = 2, .com[6] = 2, .com[7] = 3,
      .bit[4] = 23, .bit[5] = 23, .bit[6] = 22, .bit[7] = 22,

      .com[8] = 3, .com[9] = 2, .com[10] = 1, .com[11] = 0,
      .bit[8] = 20, .bit[9] = 20, .bit[10] = 20, .bit[11] = 20,

      .com[12] = 0, .com[13] = 1, .com[14] = 0,
      .bit[12] = 22, .bit[13] = 22, .bit[14] = 19
    },
    { /* 6 */
      .com[0] = 3, .com[1] = 2, .com[2] = 1, .com[3] = 0,
      .bit[0] = 17, .bit[1] = 17, .bit[2] = 17, .bit[3] = 17,

      .com[4] = 1, .com[5] = 2, .com[6] = 2, .com[7] = 3,
      .bit[4] = 19, .bit[5] = 19, .bit[6] = 18, .bit[7] = 18,

      .com[8] = 3, .com[9] = 2, .com[10] = 1, .com[11] = 0,
      .bit[8] = 16, .bit[9] = 16, .bit[10] = 16, .bit[11] = 16,

      .com[12] = 0, .com[13] = 1, .com[14] = 0,
      .bit[12] = 18, .bit[13] = 18, .bit[14] = 15
    },
    { /* 7 */
      .com[0] = 3, .com[1] = 2, .com[2] = 1, .com[3] = 0,
      .bit[0] = 13, .bit[1] = 13, .bit[2] = 13, .bit[3] = 13,

      .com[4] = 1, .com[5] = 2, .com[6] = 2, .com[7] = 3,
      .bit[4] = 15, .bit[5] = 15, .bit[6] = 14, .bit[7] = 14,

      .com[8] = 3, .com[9] = 2, .com[10] = 1, .com[11] = 0,
      .bit[8] = 12, .bit[9] = 12, .bit[10] = 12, .bit[11] = 12,

      .com[12] = 0, .com[13] = 1, .com[14] = 0,
      .bit[12] = 14, .bit[13] = 14, .bit[14] = 39
    }
  },
  .Number      = {
    { /* 11 */
      .com[0] = 0, .com[1] = 0, .com[2] = 2, .com[3] = 3,
      .bit[0] = 5, .bit[1] = 4, .bit[2] = 4, .bit[3] = 4,

      .com[4] = 2, .com[5] = 1, .com[6] = 1, .com[7] = 3,
      .bit[4] = 5, .bit[5] = 5, .bit[6] = 4, .bit[7] = 7
    },
    { /* 10 */
      .com[0] = 0, .com[1] = 0, .com[2] = 2, .com[3] = 3,
      .bit[0] = 7, .bit[1] = 6, .bit[2] = 6, .bit[3] = 6,

      .com[4] = 2, .com[5] = 1, .com[6] = 1, .com[7] = 3,
      .bit[4] = 7, .bit[5] = 7, .bit[6] = 6, .bit[7] = 5
    },
    { /* 9 */
      .com[0] = 0, .com[1] = 1, .com[2] = 2, .com[3] = 3,
      .bit[0] = 8, .bit[1] = 8, .bit[2] = 8, .bit[3] = 9,

      .com[4] = 2, .com[5] = 0, .com[6] = 1, .com[7] = 3,
      .bit[4] = 9, .bit[5] = 9, .bit[6] = 9, .bit[7] = 8
    },
    { /* 8 */
      .com[0] = 0, .com[1] = 1, .com[2] = 2, .com[3] = 3,
      .bit[0] = 10, .bit[1] = 10, .bit[2] = 10, .bit[3] = 11,

      .com[4] = 2, .com[5] = 0, .com[6] = 1, .com[7] = 3,
      .bit[4] = 11, .bit[5] = 11, .bit[6] = 11, .bit[7] = 10
    }
  },
  .Battery       = {
    .com[0] = 0, .bit[0] = 2,
    .com[1] = 3, .bit[1] = 2,
    .com[2] = 2, .bit[2] = 2,
    .com[3] = 1, .bit[3] = 2,
  },


  .PowerMeter       = {
    .com[0] = 3, .bit[0] = 0,
    .com[1] = 2, .bit[1] = 0,
    .com[2] = 1, .bit[2] = 0,
    .com[3] = 0, .bit[3] = 0,
    .com[4] = 0, .bit[4] = 0,
    .com[5] = 1, .bit[5] = 0,
    .com[6] = 2, .bit[6] = 0,
    .com[7] = 3, .bit[7] = 0,
    .com[8] = 3, .bit[0] = 0,
    .com[9] = 2, .bit[1] = 0,
    .com[10] = 1, .bit[2] = 0,
    .com[11] = 0, .bit[3] = 0
  },
  .Arrows     = {
    .com[0] = 3, .bit[0] = 3,
    .com[1] = 2, .bit[1] = 3,
    .com[2] = 1, .bit[2] = 3,
    .com[3] = 0, .bit[3] = 3
  }
};

/**************************************************************************//**
 * @brief
 * Defines higlighted segments for the alphabet, starting from "blank" (SPACE)
 * Uses bit pattern as defined for text segments above.
 * E.g. a capital O, would have bits 0 1 2 3 4 5 => 0x003f defined
 *****************************************************************************/
uint16_t EM_alphabet[] = {
  0x0000, /* space */
  0x1100, /* ! */
  0x0280, /* " */
  0x0000, /* # */
  0x0000, /* $ */
  0x0000, /* % */
  0x0000, /* & */
  0x0000, /* £ */
  0x0039, /* ( */
  0x000f, /* ) */
  0x0000, /* * */
  0x1540, /* + */
  0x0000, /* , */
  0x0440, /* - */
  0x1000, /* . */
  0x2200, /* / */

  0x003f, /* 0 */
  0x0006, /* 1 */
  0x045b, /* 2 */
  0x044f, /* 3 */
  0x0466, /* 4 */
  0x046d, /* 5 */
  0x047d, /* 6 */
  0x0007, /* 7 */
  0x047f, /* 8 */
  0x046f, /* 9 */

  0x0000, /* : */
  0x0000, /* ; */
  0x0a00, /* < */
  0x0000, /* = */
  0x2080, /* > */
  0x0000, /* ? */
  0xffff, /* @ */

  0x0477, /* A */
  0x0a79, /* B */
  0x0039, /* C */
  0x20b0, /* D */
  0x0079, /* E */
  0x0071, /* F */
  0x047d, /* G */
  0x0476, /* H */
//  0x0006, /* I */
  0x0030, /* I edit */
  0x000e, /* J */
  0x0a70, /* K */
  0x0038, /* L */
  0x02b6, /* M */
  0x08b6, /* N */
  0x003f, /* O */
  0x0473, /* P */
  0x083f, /* Q */
  0x0c73, /* R */
  0x046d, /* S */
  0x1101, /* T */
  0x003e, /* U */
  0x2230, /* V */
  0x2836, /* W */
  0x2a80, /* X */
  0x046e, /* Y */
  0x2209, /* Z */

  0x0039, /* [ */
  0x0880, /* backslash */
  0x000f, /* ] */
  0x0001, /* ^ */
  0x0008, /* _ */
  0x0100, /* ` */

  0x1058, /* a */
  0x047c, /* b */
  0x0058, /* c */
  0x045e, /* d */
  0x2058, /* e */
  0x0471, /* f */
  0x0c0c, /* g */
  0x0474, /* h */
  0x0004, /* i */
  0x000e, /* j */
  0x0c70, /* k */
  0x0038, /* l */
  0x1454, /* m */
  0x0454, /* n */
  0x045c, /* o */
  0x0473, /* p */
  0x0467, /* q */
  0x0450, /* r */
  0x0c08, /* s */
  0x0078, /* t */
  0x001c, /* u */
  0x2010, /* v */
  0x2814, /* w */
  0x2a80, /* x */
  0x080c, /* y */
  0x2048, /* z */

  0x0000,
};

/**************************************************************************//**
 * @brief
 * Defines higlighted segments for the numeric display
 *****************************************************************************/
uint16_t EM_Numbers[] =
{
  0x003f, /* 0 */
  0x0006, /* 1 */
  0x005b, /* 2 */
  0x004f, /* 3 */
  0x0066, /* 4 */
  0x006d, /* 5 */
  0x007d, /* 6 */
  0x0007, /* 7 */
  0x007f, /* 8 */
  0x006f, /* 9 */
  0x0040, /* - */
};

#endif

