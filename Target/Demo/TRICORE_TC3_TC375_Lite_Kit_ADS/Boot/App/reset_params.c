/************************************************************************************//**
* \file         Demo/TRICORE_TC3_TC375_Lite_Kit_Master_ADS/Boot/App/reset_params.c
* \brief        Microcontroller reset parameters source file.
* \details      With the functionality in this module you can set and reset bit-flags in
*               16-bit RSTCON2.USRINFO register.
*
*               The data stored in the RSTCON2.USRINFO register is always cleared on a
*               cold power-on reset, yet remains present during warm resets. This makes
*               it perfect for storing information which must survive warm resest.
*
*               One example scenario is when you would like to pass information from your
*               own firmware to the bootloader, when activating it with a software reset.
* \ingroup      Boot_TRICORE_TC3_TC375_Lite_Kit_Master_ADS
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2024  by Feaser    http://www.feaser.com    All rights reserved
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include "reset_params.h"                              /* Reset parameters header.     */
#include "IfxScuRcu.h"                                 /* SCU basic functionality.     */


/************************************************************************************//**
** \brief     Sets the specified flag in the RSTCON2.USRINFO. A special case is when
**            you specify the ResetParamsNoFlags value. In this case all flags
**            will be reset to 0.
** \param     flagType The flag to set to 1 in the RSTCON2.USRINFO register. Or use
**            ResetParamseNoFlags to reset all flags back to 0.
** \return    none.
**
****************************************************************************************/
void ResetParamsSetFlag(tResetParamsFlagType flagType)
{
  uint16 password;

  /* RSTCON2 is CPU endinit protected. Clear end init protection. */
  password = IfxScuWdt_getCpuWatchdogPasswordInline(&MODULE_SCU.WDTCPU[IfxCpu_getCoreIndex()]);
  IfxScuWdt_clearCpuEndinitInline(&MODULE_SCU.WDTCPU[IfxCpu_getCoreIndex()], password);
  /* Should all flags be cleared? */
  if (flagType == ResetParamsNoFlags)
  {
    MODULE_SCU.RSTCON2.B.USRINFO = 0;
  }
  /* Set the flag. */
  else
  {
    MODULE_SCU.RSTCON2.B.USRINFO |= flagType;
  }
  /* Re-enable CPU endInit protection. */
  IfxScuWdt_setCpuEndinitInline(&MODULE_SCU.WDTCPU[IfxCpu_getCoreIndex()], password);
} /*** end of ResetParamsSetFlag ***/


/************************************************************************************//**
** \brief     Determines if the specified flag is currently set to 1 in the
**            RSTCON2.USRINFO register.
** \param     flagType The flag to check in the RSTCON2.USRINFO register.
** \return    The same flagType value if the flag is currently set. If the flag is not
**            set, the value will be ResetParamsNoFlags.
**
****************************************************************************************/
tResetParamsFlagType ResetParamsGetFlag(tResetParamsFlagType flagType)
{
  tResetParamsFlagType result = ResetParamsNoFlags;
  uint16               password;

  /* Only check the flag if it is valid. */
  if (flagType != ResetParamsNoFlags)
  {
    /* RSTCON2 is CPU endinit protected. Clear end init protection. */
    password = IfxScuWdt_getCpuWatchdogPasswordInline(&MODULE_SCU.WDTCPU[IfxCpu_getCoreIndex()]);
    IfxScuWdt_clearCpuEndinitInline(&MODULE_SCU.WDTCPU[IfxCpu_getCoreIndex()], password);
    /* Is the flag currently set? */
    if ((MODULE_SCU.RSTCON2.B.USRINFO & flagType) != 0)
    {
      /* Update the result. */
      result = flagType;
    }
    /* Re-enable CPU endInit protection. */
    IfxScuWdt_setCpuEndinitInline(&MODULE_SCU.WDTCPU[IfxCpu_getCoreIndex()], password);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of ResetParamsGetFlag ***/


/*********************************** end of reset_params.c *****************************/
