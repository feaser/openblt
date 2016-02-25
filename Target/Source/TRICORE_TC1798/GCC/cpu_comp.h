/************************************************************************************//**
* \file         Source\TRICORE_TC1798\GCC\cpu_comp.h
* \brief        Bootloader compiler specific cpu module header file.
* \ingroup      Target_TRICORE_TC1798
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2015  by Feaser    http://www.feaser.com    All rights reserved
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
* You should have received a copy of the GNU General Public License along with OpenBLT.
* If not, see <http://www.gnu.org/licenses/>.
*
* A special exception to the GPL is included to allow you to distribute a combined work 
* that includes OpenBLT without being obliged to provide the source code for any 
* proprietary components. The exception text is included at the bottom of the license
* file <license.html>.
* 
* \endinternal
****************************************************************************************/
#ifndef CPU_COMP_H
#define CPU_COMP_H

/****************************************************************************************
* Include files
****************************************************************************************/
#include  <TC1798.h> 
#include  <machine/intrinsics.h>
#include  <machine/cint.h> 
#include  <sys/types.h> 


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Macro for performing a processor specific desync. This part is located in the
 *         compiler specific part because it uses an inline assembly call.
 */
#define CpuSetDSYNC()   asm("DSYNC") 


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void CpuEnterInitMode(void);
void CpuLeaveInitMode(void);


#endif /* CPU_COMP_H */
/*********************************** end of cpu_comp.h *********************************/
