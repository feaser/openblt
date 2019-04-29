/**************************************************************************//**
 * @file
 * @brief LCD Controller header file
 * @author Energy Micro AS
 * @version 1.1.1
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
 *****************************************************************************/

#ifndef _LCDCONTROLLER_H
#define _LCDCONTROLLER_H

#include "efm32.h"

/* Range of symbols available on display */
typedef enum
{
  LCD_SYMBOL_OLIMEX,

  LCD_SYMBOL_ARR_UP,
  LCD_SYMBOL_ARR_DN,
  LCD_SYMBOL_ARR_L,
  LCD_SYMBOL_ARR_R,

  LCD_SYMBOL_MINUS,
  LCD_SYMBOL_PLUS,

  LCD_SYMBOL_u,
  LCD_SYMBOL_m,
  LCD_SYMBOL_H,
  LCD_SYMBOL_F,

  LCD_SYMBOL_10COL,
} lcdSymbol;

/* Regular functions */
void LCD_Init(LCD_TypeDef *lcd);
void LCD_IRQHandler(void);
void LCD_Disable(LCD_TypeDef *lcd);

void LCD_AllOff(LCD_TypeDef *lcd);
void LCD_AllOn(LCD_TypeDef *lcd);

void LCD_ARing(LCD_TypeDef *lcd, int anum, int on);
void LCD_Battery(LCD_TypeDef *lcd, int batteryLevel);
void LCD_EnergyMode(LCD_TypeDef *lcd, int em, int on);
void LCD_Number(LCD_TypeDef *lcd, int value);
void LCD_NumberOff(LCD_TypeDef *lcd);
void LCD_Symbol(LCD_TypeDef *lcd, lcdSymbol s, int on);
void LCD_Write(LCD_TypeDef *lcd, char *string);

#endif
