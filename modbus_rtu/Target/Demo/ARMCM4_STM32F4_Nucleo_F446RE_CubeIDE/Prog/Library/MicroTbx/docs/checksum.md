# Checksums

A checksum is an integer value that can verify the contents of a data block. In
embedded software programs, the common purposes of a checksum are: to validate
the contents of a data bock, and to detect accidental changes in a data block.

A checksum could for example be calculated over the entire program code that is
programmed into flash memory. The checksum could then stored at another
location in flash memory, with the help of the debugger / flash programmer. At program startup, the initialization code could recalculate the checksum and
compare it with the value programmed by the debugger / flash programmer. It the
values do not match, you can conclude that somehow the flash contents got
changed and the software program might not execute properly.

Checksums are also often embedded in communication data streams. The sender
calculates and adds the checksum to the communication data. Upon reception,
the receiver recalculates the checksum and compares the calculated value with
the value that was added by the sender. The communication data is deemed valid
if both values match.

## Usage

MicroTBX includes functionality to calculate both 16-bit and 32-bit checksum
values over a fixed length byte array, using the following cyclic redundancy check (CRC)
algorithms:

* CRC16-CCITT-FALSE
* CRC32-MPEG2

To calculate the 16-bit checksum value over a number of bytes, function [`TbxChecksumCrc16Calculate()`](apiref.md#tbxchecksumcrc16calculate) is available. In case a 32-bit checksum value
is preferred, function [`TbxChecksumCrc32Calculate()`](apiref.md#tbxchecksumcrc32calculate) can be called.

## Examples

The following example declares a data block with communication data, consisting
of 32 bytes (8 * 4). Function `CalculateChecksum()` demonstrates how a 32-bit
checksum over the communication data is calculated. The sender of this data
packet could send the checksum value together with the actual packet data.

The receiver could read out the received packet data and packet checksum value.
Next, it calls function `VerifyChecksum()` to check if the packet data is valid
and no bits got changed during the communication transfer.

```c
uint32_t communicationData[8] =
{
  0x8ef9c15f, 0x4323a3cb, 0xb12ba488, 0xb3f5ec04,
  0xc1c7544f, 0x4140ec9d, 0xc5dd421a, 0x14d57e3d
};

uint32_t CalculateChecksum(void)
{
  uint8_t   result;
  uint8_t * dataPtr;
  size_t    dataLen;

  /* Collect data block settings. */
  dataPtr = (uint8_t *)&communicationData[0];
  dataLen = sizeof(communicationData)/sizeof(uint8_t);
  /* Calculate the checksum. */
  result = TbxChecksumCrc32Calculate(dataPtr, dataLen);
  /* Return the result. */
  return result;
}

uint8_t VerifyChecksum(uint32_t checksum)
{
  uint8_t  result = TBX_FALSE;
  uint32_t calculatedChecksum;

  /* Recalculate the checksum. */
  calculatedChecksum = CalculateChecksum();
  /* Verify that the data contents are still valid. */
  if (calculatedChecksum == checksum)
  {
    result = TBX_TRUE;
  }
  /* Return the result. */
  return result;
}
```

## Configuration

The 16-bit and 32-bit CRC algorithms uses a specific polynomial value and are
seeded with an initial value. The default configuration of these values work
fine and there is no need to change these.

If for some reason your application requires you to set different values for the
initial and polynomial values, you can override the default configuration by
adding the following macros to the `tbx_conf.h` configuration file and assign
the values you prefer:

```c
/** \brief Polynomial of the 16-bit CRC. */
#define TBX_CONF_CHECKSUM_CRC16_POLYNOM          (0x1021U)

/** \brief Initial value of the 6-bit CRC calculation. */
#define TBX_CONF_CHECKSUM_CRC16_INITIAL          (0xFFFFU)

/** \brief Polynomial of the 32-bit CRC. */
#define TBX_CONF_CHECKSUM_CRC32_POLYNOM          (0x04C11DB7UL)

/** \brief Initial value of the 32-bit CRC calculation. */
#define TBX_CONF_CHECKSUM_CRC32_INITIAL          (0xFFFFFFFFUL)
```
