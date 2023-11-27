/************************************************************************************//**
* \file         tbx_crypto.c
* \brief        Cryptography module source file.
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
#include "tbx_aes256.h"                          /* AES256 cryptography                */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Size of an AES block. Decryption is performed in this block size. */
#define TBX_CRYPTO_AES_BLOCK_SIZE                (16U)


/************************************************************************************//**
** \brief     Encrypts the len-bytes in the specified data-array, using the specified
**            256-bit (32 bytes) key. The results are written back into the same array.
** \param     data Pointer to the byte array with data to encrypt. The encrypted bytes
**            are stored in the same array.
** \param     len The number of bytes in the data-array to encrypt. It must be a multiple
**            of 16, as this is the AES256 minimal block size.
** \param     key The 256-bit encryption key as a array of 32 bytes.
**
****************************************************************************************/
void TbxCryptoAes256Encrypt(uint8_t       * data, 
                            size_t          len, 
                            uint8_t const * key)
{
  tbx_aes256_context ctx;

  /* Verify parameters. */
  TBX_ASSERT(data != NULL);
  TBX_ASSERT(len > 0U);
  TBX_ASSERT(key != NULL);
  TBX_ASSERT((len % TBX_CRYPTO_AES_BLOCK_SIZE) == 0U);

  /* Only continue if the parameters are valid. */
  if ( (data != NULL) && (len > 0U) && (key != NULL) && \
       ((len % TBX_CRYPTO_AES_BLOCK_SIZE) == 0U) )
  {
    /* Initialize the context. */
    tbx_aes256_init(&ctx, key);
    /* Encrypt in blocks of 16 bytes. */
    for (size_t idx = 0U; idx < (len / TBX_CRYPTO_AES_BLOCK_SIZE); idx++)
    {
      tbx_aes256_encrypt_ecb(&ctx, &data[idx * TBX_CRYPTO_AES_BLOCK_SIZE]);
    }
    /* Cleanup */
    tbx_aes256_done(&ctx);
  }
} /*** end of TbxCryptoAes256Encrypt ***/


/************************************************************************************//**
** \brief     Decrypts the len-bytes in the specified data-array, using the specified 256-
**            bit (32 bytes) key. The results are written back into the same array.
** \param     data Pointer to the byte array with data to decrypt. The decrypted bytes
**            are stored in the same array.
** \param     len The number of bytes in the data-array to decrypt. It must be a multiple
**            of 16, as this is the AES256 minimal block size.
** \param     key The 256-bit decryption key as a array of 32 bytes.
**
****************************************************************************************/
void TbxCryptoAes256Decrypt(uint8_t       * data, 
                            size_t          len, 
                            uint8_t const * key)
{
  tbx_aes256_context ctx;

  /* Verify parameters. */
  TBX_ASSERT(data != NULL);
  TBX_ASSERT(len > 0U);
  TBX_ASSERT(key != NULL);
  TBX_ASSERT((len % TBX_CRYPTO_AES_BLOCK_SIZE) == 0U);

  /* Only continue if the parameters are valid. */
  if ( (data != NULL) && (len > 0U) && (key != NULL) && \
       ((len % TBX_CRYPTO_AES_BLOCK_SIZE) == 0U) )
  {
    /* Initialize the context. */
    tbx_aes256_init(&ctx, key);
    /* Decrypt in blocks of 16 bytes. */
    for (size_t idx = 0U; idx < (len / TBX_CRYPTO_AES_BLOCK_SIZE); idx++)
    {
      tbx_aes256_decrypt_ecb(&ctx, &data[idx * TBX_CRYPTO_AES_BLOCK_SIZE]);
    }
    /* Cleanup */
    tbx_aes256_done(&ctx);
  }
} /*** end of TbxCryptoAes256Decrypt ***/


/*********************************** end of tbx_crypto.c *******************************/
