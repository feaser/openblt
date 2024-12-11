/************************************************************************************//**
* \file         Demo/TRICORE_TC3_TC375_Lite_Kit_Master_ADS/Prog/App/reset_params.h
* \brief        Microcontroller reset parameters header file.
* \ingroup      Prog_TRICORE_TC3_TC375_Lite_Kit_Master_ADS
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
#ifndef RESET_PARAMS_H
#define RESET_PARAMS_H

/****************************************************************************************
* Include files
****************************************************************************************/


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Enumerated type that defines the available parameter bit flags. */
typedef enum
{
  ResetParamsNoFlags    = 0x00,               /**< \brief No flags utility value.      */
  ResetParamsReqTcpInit = 0x01                /**< \brief Request TCP/IP stack init.   */
} tResetParamsFlagType;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void                 ResetParamsSetFlag(tResetParamsFlagType flagType);
tResetParamsFlagType ResetParamsGetFlag(tResetParamsFlagType flagType);


#endif /* RESET_PARAMS_H */
/*********************************** end of reset_params.h *****************************/
