/************************************************************************************//**
* \file         util.c
* \brief        Utility module source file.
* \ingroup      Utility
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include <assert.h>                         /* for assertions                          */
#include <stdint.h>                         /* for standard integer types              */
#include <stddef.h>                         /* for NULL declaration                    */
#include <stdbool.h>                        /* for boolean type                        */
#include <string.h>                         /* for string library                      */
#include "util.h"                           /* Utility module                          */
#include "aes256.h"                         /* for AES256 cryptography.                */


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief Lookup table for calculating a 16-bit CRC checksum. It was generated using an
 *         initial value of 0 and a polynomial of 0x8005.
 */
static const uint16_t utilChecksumCrc16Table[256] =
{
  0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
  0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
  0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
  0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
  0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
  0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
  0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
  0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
  0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
  0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
  0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
  0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
  0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
  0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
  0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
  0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
  0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
  0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
  0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
  0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
  0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
  0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
  0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
  0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
  0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
  0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
  0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
  0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
  0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
  0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
  0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
  0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
};

/** \brief Lookup table for calculating a 32-bit CRC checksum. It was generated using an
 *         initial value of 0 and a polynomial of 0x04C11DB7
 */
static const uint32_t utilChecksumCrc32Table[256] = 
{
  0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2, 
  0x1E475005, 0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 
  0x3C8EA00A, 0x384FBDBD, 0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC, 
  0x5BD4B01B, 0x569796C2, 0x52568B75, 0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011, 
  0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD, 0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 
  0x95609039, 0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5, 0xBE2B5B58, 0xBAEA46EF, 
  0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D, 0xD4326D90, 
  0xD0F37027, 0xDDB056FE, 0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
  0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 
  0xEC7DD02D, 0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 
  0x2E003DC5, 0x2AC12072, 0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 0x018AEB13, 
  0x054BF6A4, 0x0808D07D, 0x0CC9CDCA, 0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE, 
  0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02, 0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 
  0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA, 0xACA5C697, 0xA864DB20, 
  0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692, 0x8AAD2B2F, 
  0x8E6C3698, 0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A, 
  0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E, 0xF3B06B3B, 0xF771768C, 0xFA325055, 
  0xFEF34DE2, 0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34, 
  0xDC3ABDED, 0xD8FBA05A, 0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637, 0x7A089632, 
  0x7EC98B85, 0x738AAD5C, 0x774BB0EB, 0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 
  0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53, 0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 
  0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B, 0x0315D626, 0x07D4CB91, 
  0x0A97ED48, 0x0E56F0FF, 0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623, 0xF12F560E, 
  0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B, 
  0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 
  0xC960EBB3, 0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2, 0xAAFBE615, 
  0xA7B8C0CC, 0xA379DD7B, 0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F, 0x8832161A, 
  0x8CF30BAD, 0x81B02D74, 0x857130C3, 0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 
  0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C, 0x7B827D21, 0x7F436096, 0x7200464F, 
  0x76C15BF8, 0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24, 0x119B4BE9, 0x155A565E, 
  0x18197087, 0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC, 0x3793A651, 
  0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654, 
  0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 
  0xDBEE767C, 0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18, 0xF0A5BD1D, 0xF464A0AA, 
  0xF9278673, 0xFDE69BC4, 0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 
  0x9E7D9662, 0x933EB0BB, 0x97FFAD0C, 0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668, 
  0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4
};


/************************************************************************************//**
** \brief     Calculates a 16-bit CRC value over the specified data using byte wise
**            computation with a table.
** \param     data Array with bytes over which the CRC16 should be calculated.
** \param     len Number of bytes in the data array.
** \return    The 16-bit CRC value.
**
****************************************************************************************/
uint16_t UtilChecksumCrc16Calculate(uint8_t const * data, uint32_t len)
{
  uint16_t result = 0;
  uint32_t byteIdx;
  
  /* Verify parameters. */
  assert(data != NULL);
  assert(len > 0);
  
  /* Only continue if parameters are valid. */
  if ( (data != NULL) && (len > 0) ) /*lint !e774 */
  {
    /* Loop through all bytes. */
    for (byteIdx = 0; byteIdx < len; byteIdx++)
    {
      result = ((uint16_t)(result << 8)) ^ 
               utilChecksumCrc16Table[(uint8_t)(data[byteIdx] ^ (result >> 8))];
    }
  }
  
  return result;
} /*** end of UtilChecksumCrc16Calculate ***/


/************************************************************************************//**
** \brief     Calculates a 32-bit CRC value over the specified data using byte wise
**            computation with a table.
** \param     data Array with bytes over which the CRC32 should be calculated.
** \param     len Number of bytes in the data array.
** \return    The 32-bit CRC value.
**
****************************************************************************************/
uint32_t UtilChecksumCrc32Calculate(uint8_t const * data, uint32_t len)
{
  uint32_t result = 0;
  uint32_t byteIdx;
  
  /* Verify parameters. */
  assert(data != NULL);
  assert(len > 0);
  
  /* Only continue if parameters are valid. */
  if ( (data != NULL) && (len > 0) ) /*lint !e774 */
  {
    /* Loop through all bytes. */
    for (byteIdx = 0; byteIdx < len; byteIdx++)
    {
      result = ((uint32_t)(result << 8)) ^ 
               utilChecksumCrc32Table[(uint8_t)(data[byteIdx] ^ (result >> 24))];
    }
  }
  
  return result;
} /*** end of UtilChecksumCrc32Calculate ***/


/************************************************************************************//**
** \brief     Extracts the filename including extention from the specified full filename,
**            which could possible include a path. The function can handle both the 
**            backslash and forward slash path delimiter, to make it crossplatform.
** \param     fullFilename The filename with path possible included.
** \param     filenameBuffer Pointer to the character array where the resulting filename
**            should be stored. 
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool UtilFileExtractFilename(char const * fullFilename, char * filenameBuffer)
{
  bool result = false;
  char const * filenamePtr;

  
  /* Verify parameters. */
  assert(fullFilename != NULL);
  assert(filenameBuffer != NULL);
  
  /* Only continue with valid parameters. */
  if ( (fullFilename != NULL) && (filenameBuffer != NULL) ) /*lint !e774 */
  {
    /* Extract just the filename, so without its path from the firmware file. First
     * assume a unix type path with forward slashes.
     */
    filenamePtr = strrchr(fullFilename, '/');
    /* Check if a forward slash was detected. */
    if (filenamePtr != NULL)
    {
      /* Update the pointer so that it point to the start of the filename. */
      filenamePtr++;
    }
    else
    {
      /* No unix type forward slash found. This can be because the path uses windows
       * type backslashes or because there is no path present and it is already just
       * the filename. Now check for a windows type path with backslashes.
       */
      filenamePtr = strrchr(fullFilename, '\\');
      /* Check if a forward slash was detected. */
      if (filenamePtr != NULL)
      {
        /* Update the pointer so that it point to the start of the filename. */
        filenamePtr++;
      }
      /* In case no slashes were found in the firmware file, then it is already just the 
       * filename.
       */
      else
      {
        filenamePtr = fullFilename;
      }
    }
    /* Copy the resulting filename to the buffer. */
    strcpy(filenameBuffer, filenamePtr);
    /* Success. */
    result = true;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of UtilFileExtractFilename ***/


/************************************************************************************//**
** \brief     Encrypts the len-bytes in the specified data-array, using the specified
**            256-bit (32 bytes) key. The results are written back into the same array.
** \param     data Pointer to the byte array with data to encrypt. The encrypted bytes
**            are stored in the same array.
** \param     len The number of bytes in the data-array to encrypt. It must be a multiple
**            of 16, as this is the AES256 minimal block size.
** \param     key The 256-bit encryption key as a array of 32 bytes.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool UtilCryptoAes256Encrypt(uint8_t * data, uint32_t len, uint8_t const * key)
{
  bool result = false;
  aes256_context ctx;

  /* Check parameters */
  assert(data != NULL);
  assert(key != NULL);

  /* Only continue with valid parameters. Also add a block size check for 'len'. */
  if ( (data != NULL) && (key != NULL) && ((len % 16u) == 0) ) /*lint !e774 */
  {
    /* Init context */
    aes256_init(&ctx, key);
    /* Encrypt in blocks of 16 bytes. */
    for (uint32_t i = 0; i < (len / 16u); i++)
    {
      aes256_encrypt_ecb(&ctx, &data[i * 16u]);
    }
    /* Cleanup */
    aes256_done(&ctx);
    /* Set positive result. */
    result = true;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of UtilCryptoAes256Encrypt ***/


/************************************************************************************//**
** \brief     Decrypts the len-bytes in the specified data-array, using the specified 256-
**            bit (32 bytes) key. The results are written back into the same array.
** \param     data Pointer to the byte array with data to decrypt. The decrypted bytes
**            are stored in the same array.
** \param     len The number of bytes in the data-array to decrypt. It must be a multiple
**            of 16, as this is the AES256 minimal block size.
** \param     key The 256-bit decryption key as a array of 32 bytes.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool UtilCryptoAes256Decrypt(uint8_t * data, uint32_t len, uint8_t const * key)
{
  bool result = false;
  aes256_context ctx;

  /* Check parameters */
  assert(data != NULL);
  assert(key != NULL);

  /* Only continue with valid parameters. Also add a block size check for 'len'. */
  if ( (data != NULL) && (key != NULL) && ((len % 16u) == 0) ) /*lint !e774 */
  {
    /* Init context */
    aes256_init(&ctx, key);
    /* Decrypt in blocks of 16 bytes. */
    for (uint32_t i = 0; i < (len / 16u); i++)
    {
      aes256_decrypt_ecb(&ctx, &data[i * 16u]);
    }
    /* Cleanup */
    aes256_done(&ctx);
    /* Set positive result. */
    result = true;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of UtilCryptoAes256Decrypt ***/


/*********************************** end of util.c *************************************/

 
 
