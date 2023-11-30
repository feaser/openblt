# Platform

The platform software component contains functionality to determine platform and architecture specifics at run-time. This functionality eases cross-platform embedded software development.

## Usage

The platform component currently contains a function to determine if the microcontroller's memory storage organization is little endian (Intel) or big endian (Motorola). To make this determination at run-time, call function `TbxPlatformLittleEndian()`. If it returns `TBX_TRUE` the code runs on a platform with little endian memory storage organization. If it returns `TBX_FALSE`, it is big endian.

## Examples

Especially communication protocols typically require you to either report the endianness of the target or store data larger than one byte in a specific way. One example is on TCP/IP networks, which requires multi-byte numbers to be communicated in the big endian format. Most TCP/IP stacks offer a function named `htonl()` for this; An abbreviation for *host-to-network-long*. With the help of `TbxPlatformLittleEndian()`, you can implement a similar function:

```c
uint32_t StoreU32BigEndian(uint32_t value)
{
  uint32_t result = value;

  /* Little endian (Intel)? */
  if (TbxPlatformLittleEndian() == TBX_TRUE)
  {
    /* Get byte pointer to value and result variables. */
    uint8_t * leValuePtr =  (uint8_t *)&value;
    uint8_t * beResultPtr = (uint8_t *)&result;
    /* Reverse byte ordering to store value in big endian (Motorola) format. */
    beResultPtr[0] = leValuePtr[3];
    beResultPtr[1] = leValuePtr[2];
    beResultPtr[2] = leValuePtr[1];
    beResultPtr[3] = leValuePtr[0];
  }
  /* Give the result back to the caller. */
  return result;
}
```
