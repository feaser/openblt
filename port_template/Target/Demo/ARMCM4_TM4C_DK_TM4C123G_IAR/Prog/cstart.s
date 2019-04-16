/****************************************************************************************
|  Description: Demo program C startup source file
|    File Name: cstart.s
|
|----------------------------------------------------------------------------------------
|                          C O P Y R I G H T
|----------------------------------------------------------------------------------------
|   Copyright (c) 2014  by Feaser    http://www.feaser.com    All rights reserved
|
|----------------------------------------------------------------------------------------
|                            L I C E N S E
|----------------------------------------------------------------------------------------
| This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
| modify it under the terms of the GNU General Public License as published by the Free
| Software Foundation, either version 3 of the License, or (at your option) any later
| version.
|
| OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You have received a copy of the GNU General Public License along with OpenBLT. It 
| should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
| 
****************************************************************************************/


        MODULE  ?cstartup

  ; Forward declaration of section.
  SECTION CSTACK:DATA:NOROOT(3)

	PUBLIC	__iar_program_start
	EXTERN	__cmain
	EXTERN  __vector_table
        EXTWEAK __iar_init_core
        EXTWEAK __iar_init_vfp


/****************************************************************************************
** NAME:           __iar_program_start
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Reset interrupt service routine. Configures the stack, initializes RAM
**                 and jumps to function main.
**
****************************************************************************************/
        SECTION .text:CODE:REORDER(2)
        THUMB
__iar_program_start:	
  ; Initialize the stack pointer
  LDR	  R3, =sfe(CSTACK)
	MOV	  SP, R3

  BL	__iar_init_core
	BL	__iar_init_vfp

	BL	__cmain

	REQUIRE __vector_table
	

        END
/*********************************** end of cstart.s ***********************************/
