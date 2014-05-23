/************************************************************************************//**
* \file         srecord.h
* \brief        Motorola S-record library header file.
* \ingroup      SerialBoot
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2014  by Feaser    http://www.feaser.com    All rights reserved
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
#ifndef SRECORD_H
#define SRECORD_H


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Maximum number of characters that can be on a line in the firmware file. */
#define SRECORD_MAX_CHARS_PER_LINE        (512)

/** \brief Maximum number of data bytes that can be on a line in the firmware file
 *         (S-record).
 */
#define SRECORD_MAX_DATA_BYTES_PER_LINE   (SRECORD_MAX_CHARS_PER_LINE/2)


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Structure type for grouping the parsing results of an S-record file. */
typedef struct
{
  sb_uint32 address_low;                          /**< lowest memory address           */
  sb_uint32 address_high;                         /**< lowest memory address           */
  sb_uint32 data_bytes_total;                     /**< total number of data bytes      */
} tSrecordParseResults;

/** \brief Structure type for grouping the parsing results of an S-record line. */
typedef struct
{
  sb_uint8 data[SRECORD_MAX_DATA_BYTES_PER_LINE]; /**< array for S1,S2 or S3 data bytes*/
  sb_uint32 address;                              /**< address on S1,S2 or S3 line     */
  sb_uint16 length;                               /**< number of bytes written to array */
} tSrecordLineParseResults;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
sb_uint8 SrecordIsValid(const sb_char *srecordFile);
sb_file  SrecordOpen(const sb_char *srecordFile);
void     SrecordParse(sb_file srecordHandle, tSrecordParseResults *parseResults);
void     SrecordClose(sb_file srecordHandle);
sb_uint8 SrecordParseNextDataLine(sb_file srecordHandle, tSrecordLineParseResults *parseResults);


#endif /* SRECORD_H */
/*********************************** end of srecord.h **********************************/
