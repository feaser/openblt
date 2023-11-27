/*  
*   Byte-oriented AES-256 implementation.
*   All lookup tables replaced with 'on the fly' calculations. 
*
*   Copyright (c) 2007-2009 Ilya O. Levin, www.literatecode.com
*   Other contributors: Hal Finney, Frank Voorburg (MISRA compliance)
*
*   Permission to use, copy, modify, and distribute this software for any
*   purpose with or without fee is hereby granted, provided that the above
*   copyright notice and this permission notice appear in all copies.
*
*   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
*   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
*   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
*   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
*   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
*   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
*   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifdef __cplusplus
extern "C" { 
#endif

    typedef struct {
        uint8_t key[32]; 
        uint8_t enckey[32]; 
        uint8_t deckey[32];
    } tbx_aes256_context; 


    void tbx_aes256_init(tbx_aes256_context *ctx, uint8_t const *k);
    void tbx_aes256_done(tbx_aes256_context *ctx);
    void tbx_aes256_encrypt_ecb(tbx_aes256_context *ctx, uint8_t *buf);
    void tbx_aes256_decrypt_ecb(tbx_aes256_context *ctx, uint8_t *buf);

#ifdef __cplusplus
}
#endif
