# Cryptography

The cryptography software component contains functionality to encrypt and
decrypt data blocks. Encryption essentially changes the data contents such
that its contents can no longer be interpreted by unwanted third parties. Only
the party that has access to the so called key, with which the data was
encrypted, can decrypt the data contents back into its original state.

Encryption enables you to securely protect data that you don't want anyone else
to have access to. Possible fields of application in an embedded software
program are: securing communication data, securing parameters or other
proprietary data stored in EEPROM, etc.

The cryptography software component is based on 256-bit AES ECB. AES stands for
Advanced Encryption Standard and ECB stands for Electronic CodeBook. The key
needed to perform the actual encryption and decryption is 256-bit in size. In
C code, this is an array of 32 bytes.

The only restriction is that the data to encrypt or decrypt must always be a
multiple of 16 bytes in size. If this is not the case, the data needs to be
aligned to a multiple of 16 bytes prior to performing the encryption/decryption
operation.

## Usage

The first step is to define your 256-bit cryptography key. This is nothing more
than an array of 32 bytes. You can decide on the contents yourself. The only
important part to remember is that you need the same key for both the
encryption and decryption steps, in order to get the original data back when
decrypting the encrypted data. Here is an example of how to define a
cryptography key in C:

```c
/* The 256-bit key that will be used to encrypt and decrypt data. */
const uint8_t cryptoKey[32] =
{
  0x32, 0x72, 0x35, 0x75, 0x38, 0x78, 0x21, 0x41,
  0x25, 0x44, 0x2A, 0x47, 0x2D, 0x4B, 0x61, 0x50,
  0x64, 0x53, 0x67, 0x56, 0x6B, 0x59, 0x70, 0x33,
  0x73, 0x36, 0x76, 0x39, 0x79, 0x24, 0x42, 0x3F
};
```

Note that you can use an [encryption key generator](https://www.allkeysgenerator.com/Random/Security-Encryption-Key-Generator.aspx) to
assist with getting unique contents for your cryptography key.

To encrypt a 16-byte aligned data block, the function [`TbxCryptoAes256Encrypt()`](apiref.md#tbxcryptoaes256encrypt)
is available. To decrypt the data block back to its original state, the function
[`TbxCryptoAes256Decrypt()`](apiref.md#tbxcryptoaes256decrypt) can be called.

## Examples

The following code example first encrypts the contents of a data buffer.
Afterwards, the encrypted data is decrypted again. Followed by a verification
to test that the decrypted data actually resembles the original data. The
`cryptoKey[]` presented above serves as the 256-bit cryptography key:

```c
/* Original data. */
const uint8_t originalData[64] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
};

/* Temporary buffer to storing the encrypted/decrypted data. */
uint8_t buffer[64];

/* Array indexer for looping through array data. */
size_t idx;

/* Copy the original data to the buffer. */
for (idx = 0U; idx < 64U; idx++)
{
  buffer[idx] = originalData[idx];
}

/* Encrypt the data in the buffer. */
TbxCryptoAes256Encrypt(buffer, 64, cryptoKey);

/* Decrypt the data in the buffer. */
TbxCryptoAes256Decrypt(buffer, 64, cryptoKey);

/* Verify that the decrypted data is the same as the original data. */
for (idx = 0U; idx < 64U; idx++)
{
  if (buffer[idx] != originalData[idx])
  {
    TBX_ASSERT(TBX_ERROR);
  }
}
```
