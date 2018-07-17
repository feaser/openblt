/************************************************************************************//**
* \file         Demo/ARMCM4_XMC4_XMC4700_Relax_Kit_IAR/Prog/timer.h
* \brief        Timer driver header file.
* \ingroup      Prog_ARMCM4_XMC4_XMC4700_Relax_Kit_IAR
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2017  by Feaser    http://www.feaser.com    All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option) any later
* version.
*
* OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with OpenBLT. It 
* should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
*
* \endinternal
****************************************************************************************/
#ifndef TIMER_H
#define TIMER_H

/****************************************************************************************
* Function prototypes
****************************************************************************************/
void          TimerInit(void);
void          TimerDeinit(void);
void          TimerSet(unsigned long timer_value);
unsigned long TimerGet(void);

#endif /* TIMER_H */
/*********************************** end of timer.h ************************************/
