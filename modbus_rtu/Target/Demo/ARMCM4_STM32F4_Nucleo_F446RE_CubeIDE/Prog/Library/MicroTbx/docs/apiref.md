# API reference

This section provides a full reference of all the functions, macros and types that MicroTBX offers.

## Macros

#### Generic

| Macro                             | Description |
| :-------------------------------- | :---------- |
| `TBX_VERSION_MAIN`                | Main version number of MicroTBX. |
| `TBX_VERSION_MINOR`               | Minor version number of MicroTBX. |
| `TBX_VERSION_PATCH`               | Patch number of MicroTBX. |
| `TBX_TRUE`                        | Boolean true value. |
| `TBX_FALSE`                       | Boolean false value. |
| `TBX_OK`                          | Generic okay value. |
| `TBX_ERROR`                       | Generic error value. |
| `TBX_ON` | Generic on value. |
| `TBX_OFF` | Generic off value. |
| `TBX_UNUSED_ARG()`                | Function-like macro to flag a function parameter as unused. |
| `TBX_ASSERT()` | Function-like macro to perform an assertion check. |

#### Configuration

| Macro                        | Description                              |
| ---------------------------- | ---------------------------------------- |
| `TBX_CONF_HEAP_SIZE`         | Configure the size of the heap in bytes. |
| `TBX_CONF_ASSERTIONS_ENABLE` | Enable/disable run-time assertions.      |

## Types

#### tTbxAssertHandler

```c
typedef void (* tTbxAssertHandler)(char     const * const file,
                                   uint32_t               line)
```

Function type for a run-time assertion handler function.

#### tTbxRandomSeedInitHandler

```c
typedef uint32_t (* tTbxRandomSeedInitHandler)(void)
```

Function type for an application specific seed initialization handler.

#### tTbxList

```c
typedef struct tTbxList
```

Layout of a linked list. Its pointer serves as the handle to the linked list which is obtained after creation of the list and which is needed in the other functions of this module. Note that its elements should be considered private and only be accessed internally by the linked list module.

#### tTbxListCompareItems

```c
typedef uint8_t (* tTbxListCompareItems)(void const * item1, 
                                         void const * item2)
```

Callback function to compare items. It is called during list sorting. The return value of the callback function has the following meaning: `TBX_TRUE` if `item1`'s data is greater than `item2`'s data, `TBX_FALSE` otherwise.

## Functions

### Assertions

More information regarding this software component, including code examples, is found [here](assertions.md).

#### TbxAssertSetHandler

```c
void TbxAssertSetHandler(tTbxAssertHandler assertHandler)
```

Sets the application specific assertion handler. Use macro [`TBX_ASSERT()`](##generic) in your code for assertions.

| Param           | Description                                                  |
| --------------- | ------------------------------------------------------------ |
| `assertHandler` | Pointer to the application specific assertion handler to use instead of the default<br>internal handler. The type is [`tTbxAssertHandler`](#ttbxasserthandler). |

### Critical Sections

More information regarding this software component, including code examples, is found [here](critsect.md).

#### TbxCriticalSectionEnter

```c
void TbxCriticalSectionEnter(void)
```

Enter a critical section. Critical sections are needed in an interrupt driven software program to obtain mutual exclusive access shared resources such as global data and certain peripherals. Note that each call to this function should always be followed by a call to [`TbxCriticalSectionExit()`](#tbxcriticalsectionexit). 

#### TbxCriticalSectionExit

```c
void TbxCriticalSectionExit(void)
```

Exit a critical section. Critical sections are needed in an interrupt driven software program to obtain mutual exclusive access shared resources such as global data and certain peripherals. Note that each call to this function should always be preceded by a call to [`TbxCriticalSectionEnter()`](#tbxcriticalsectionenter).

### Heap

More information regarding this software component, including code examples, is found [here](heap.md).

#### TbxHeapAllocate

```c
void * TbxHeapAllocate(size_t size)
```

Allocates the desired number of bytes on the heap. It can be used instead of the compiler specific `malloc()` function. Note that free-ing of allocated memory is not supported to prevent memory fragmentation. If you want to dynamically allocate and release memory, use [memory pools](mempools.md). Note that you configure the overall heap size with macro [`TBX_CONF_HEAP_SIZE`](#configuration) in `tbx_conf.h`.

| Parameter | Description                                  |
| --------- | -------------------------------------------- |
| `size`    | The number of bytes to allocate on the heap. |

| Return value                                                 |
| ------------------------------------------------------------ |
| Pointer to the start of the newly allocated heap memory if successful, `NULL` otherwise. |

#### TbxHeapGetFree

```c
size_t TbxHeapGetFree(void)
```

Obtains the current amount of bytes that are still available on the heap.

| Return value                      |
| --------------------------------- |
| Number of free bytes on the heap. |


### Memory Pools

More information regarding this software component, including code examples, is found [here](mempools.md).

#### TbxMemPoolCreate

```c
uint8_t TbxMemPoolCreate(size_t numBlocks,
                         size_t blockSize)
```

Creates a new memory pool with the specified number of blocks, where each block has the size as specified by the second function parameter. The required memory is statically preallocated on the heap. This function is typically called one or more times during software program initialization, before the infinite program loop is entered. Once one ore more memory pools were created, the functions [`TbxMemPoolAllocate()`](#tbxmempoolallocate) and [`TbxMemPoolRelease()`](#tbxmempoolrelease) can be called to perform dynamic memory allocation.

A well designed memory pool approach makes dynamic memory allocation possible on RAM constrained microcontrollers, without the need to worry about memory fragmentation. Note that deleting a previously created memory pool is not supported on purpose to prevent memory fragmentation. 

When this function is called to create a memory pool with a size that was already created before, the already existing memory pool is extended instead of creating a new memory pool. This is an important feature because it makes it possible to automatically expand the memory pool if more blocks are needed.

| Parameter   | Description                                                  |
| ----------- | ------------------------------------------------------------ |
| `numBlocks` | The number of blocks to statically preallocate on the heap for this memory pool. |
| `blockSize` | The size of each block in bytes.                             |

| Return value                                                 |
| ------------------------------------------------------------ |
| `TBX_OK` if successful, `TBX_ERROR` otherwise for example when there is no more space available on<br>the heap to statically preallocated the blocks. |

#### TbxMemPoolAllocate

```c
void * TbxMemPoolAllocate(size_t size)
```

Attempts to allocate the desired number of bytes in a previously created memory pool. A block will be allocated in a memory pool that best fits the requested byte size. For example, if the following memory pools were created during program initialization:

```c
TbxMemPoolCreate(10, 8);
TbxMemPoolCreate(10, 16);
TbxMemPoolCreate(10, 32);
```

And the following function call is made:

```c
uint8_t * myMem = TbxMemPoolAllocate(9);
```

Then the memory will be allocated from the memory pool with block size 16, so the second memory pool that was created. If there are no more blocks available in that memory pool, then `NULL` is returned. So note that this function does NOT move on to the memory pool with one block size larger. Such a feature would be easy to realize but this design decision was made on purpose.

| Parameter | Description                                       |
| --------- | ------------------------------------------------- |
| `size`    | The number of bytes to allocate in a memory pool. |

| Return value                                                 |
| ------------------------------------------------------------ |
| Pointer to the start of the newly allocated memory if successful, `NULL` otherwise. |

#### TbxMemPoolRelease

```c
void TbxMemPoolRelease(void * memPtr)
```

Releases the previously allocated block of memory. Once the memory is released, it can be allocated again afterwards with function [`TbxMemPoolAllocate()`](#tbxmempoolallocate). Note that this function automatically finds the memory pool that the memory block belongs to.

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `memPtr`  | Pointer to the start of the memory block. Basically, the pointer that was returned by<br>function [`TbxMemPoolAllocate()`](#tbxmempoolallocate), when the memory was initially allocated. |


### Linked Lists

More information regarding this software component, including code examples, is found [here](lists.md).

#### TbxListCreate

```c
tTbxList * TbxListCreate(void)
```

Creates a new and empty linked list and returns its pointer. Make sure to store the pointer because it serves as a handle to the linked list, which is needed when calling the other API functions in this module.

| Return value                                                 |
| ------------------------------------------------------------ |
| The pointer to the created linked list or `NULL` in case or error. The type is [`tTbxList`](#ttbxlist). |

#### TbxListDelete

```c
void TbxListDelete(tTbxList * list)
```

Deletes a previously created linked list. Afterwards, the pointer to the linked list is no longer valid and should not be used anymore.

| Parameter | Description                                                |
| --------- | ---------------------------------------------------------- |
| `list`    | Pointer to a previously created linked list to operate on. |

#### TbxListClear

```c
void TbxListClear(tTbxList * list)
```

Removes all items from the linked list. Keep in mind that it is the caller's responsibility to release the memory of the items that were placed in the list, before calling this function.

| Parameter | Description                                                |
| --------- | ---------------------------------------------------------- |
| `list`    | Pointer to a previously created linked list to operate on. |

#### TbxListGetSize

```c
size_t TbxListGetSize(tTbxList const * list)
```

Obtains the number of items that are currently stored in the list.

| Parameter | Description                                                |
| --------- | ---------------------------------------------------------- |
| `list`    | Pointer to a previously created linked list to operate on. |

| Return values                                       |
| --------------------------------------------------- |
| Total number of items currently stored in the list. |

#### TbxListInsertItemFront

```c
uint8_t TbxListInsertItemFront(tTbxList * list, 
                               void     * item)
```

Inserts an item into the list. The item will be added at the start of the list.

| Parameter | Description                                                |
| --------- | ---------------------------------------------------------- |
| `list`    | Pointer to a previously created linked list to operate on. |
| `item`    | Pointer to the item to insert.                             |

| Return value                                                 |
| ------------------------------------------------------------ |
| `TBX_OK` if the item could be inserted, `TBX_ERROR` otherwise. |

#### TbxListInsertItemBack

```c
uint8_t TbxListInsertItemBack(tTbxList * list,
                              void     * item)
```

Inserts an item into the list. The item will be added at the end of the list.

| Parameter | Description                                                |
| --------- | ---------------------------------------------------------- |
| `list`    | Pointer to a previously created linked list to operate on. |
| `item`    | Pointer to the item to insert.                             |

| Return value                                                 |
| ------------------------------------------------------------ |
| `TBX_OK` if the item could be inserted, `TBX_ERROR` otherwise. |

#### TbxListInsertItemBefore

```c
uint8_t TbxListInsertItemBefore(tTbxList       * list,
                                void           * item,
                                void     const * itemRef)
```

Inserts an item into the list. The item will be added before the reference item.

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `list`    | Pointer to a previously created linked list to operate on.   |
| `item`    | Pointer to the item to insert.                               |
| `itemRef` | Reference item before which the new item should be inserted. |

| Return value                                                 |
| ------------------------------------------------------------ |
| `TBX_OK` if the item could be inserted, `TBX_ERROR` otherwise. |

#### TbxListInsertItemAfter

```c
uint8_t TbxListInsertItemAfter(tTbxList       * list, 
                               void           * item,
                               void     const * itemRef)
```

Inserts an item into the list. The item will be added after the reference item.

| Parameter | Description                                                 |
| --------- | ----------------------------------------------------------- |
| `list`    | Pointer to a previously created linked list to operate on.  |
| `item`    | Pointer to the item to insert.                              |
| `itemRef` | Reference item after which the new item should be inserted. |

| Return value                                                 |
| ------------------------------------------------------------ |
| `TBX_OK` if the item could be inserted, `TBX_ERROR` otherwise. |

#### TbxListRemoveItem

```c
void TbxListRemoveItem(tTbxList       * list,
                       void     const * item)
```

Removes an item from the list, if present. Keep in mind that it is the caller's responsibility to release the memory of the item that is being removed from the list, before calling this function.

| Parameter | Description                                                |
| --------- | ---------------------------------------------------------- |
| `list`    | Pointer to a previously created linked list to operate on. |
| `item`    | Pointer to the item to remove.                             |

#### TbxListGetFirstItem

```c
void * TbxListGetFirstItem(tTbxList const * list)
```

Obtains the item that is located at the start of the list. Note that the item is just read, not removed.

| Parameter | Description                                                |
| --------- | ---------------------------------------------------------- |
| `list`    | Pointer to a previously created linked list to operate on. |

| Return value                                                 |
| ------------------------------------------------------------ |
| The item at the start of the list or `NULL` if the list is empty. |

#### TbxListGetLastItem

```c
void * TbxListGetLastItem(tTbxList const * list)
```

Obtains the item that is located at the end of the list. Note that the item is just read, not removed.

| Parameter | Description                                                |
| --------- | ---------------------------------------------------------- |
| `list`    | Pointer to a previously created linked list to operate on. |

| Return value                                                 |
| ------------------------------------------------------------ |
| The item at the end of the list or NULL if the list is empty. |

#### TbxListGetPreviousItem

```c
void * TbxListGetPreviousItem(tTbxList const * list,
                              void     const * itemRef)
```

Obtains the item that is located one position before in the list, relative to the item given in the parameter. Note that the item is just read, not removed.

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `list`    | Pointer to a previously created linked list to operate on.   |
| `itemRef` | The item that is the next one in the list relative to the one this function should return. |

| Return value                                                 |
| ------------------------------------------------------------ |
| The item one position before in the list or `NULL` if there are no more items in the list before the item<br>given in the parameter. |

#### TbxListGetNextItem

```c
void * TbxListGetNextItem(tTbxList const * list,
                          void     const * itemRef)
```

Obtains the item that is located one position further down in the list, relative to the item given in the parameter. Note that the item is just read, not removed.

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `list`    | Pointer to a previously created linked list to operate on.   |
| `itemRef` | The item that is the previous one in the list relative to the one this function should<br>return. |

| Return value                                                 |
| ------------------------------------------------------------ |
| The item one position further down in the list or NULL if there are no more items in the list after the<br>item given in the parameter. |

#### TbxListSwapItems

```c
void TbxListSwapItems(tTbxList const * list,
                      void           * item1,
                      void           * item2)
```

Swaps the specified list items around.

| Parameter | Description                                                |
| --------- | ---------------------------------------------------------- |
| `list`    | Pointer to a previously created linked list to operate on. |
| `item1`   | The first item for the swap operation.                     |
| `item2`   | The second item for the swap operation.                    |

#### TbxListSortItems

```c
void TbxListSortItems(tTbxList             const * list,
                      tTbxListCompareItems         compareItemsFcn)
```

Sorts the items in the list. While sorting, it calls the specified callback function which should do the actual comparison of the items.

| Parameter         | Description                                                  |
| ----------------- | ------------------------------------------------------------ |
| `list`            | Pointer to a previously created linked list to operate on.   |
| `compareItemsFcn` | Callback function that does the item comparison. It is of type<br>[`tTbxListCompareItems`](#ttbxlistcompareitems). |


### Random Numbers

More information regarding this software component, including code examples, is found [here](random.md).

#### TbxRandomNumberGet

```c
uint32_t TbxRandomNumberGet(void)
```

Obtains a random number.

| Return value                                |
| ------------------------------------------- |
| Value of the newly generated random number. |

#### TbxRandomSetSeedInitHandler

```c
void TbxRandomSetSeedInitHandler(tTbxRandomSeedInitHandler seedInitHandler)
```

Sets the application specific function that should be called when the seed for the random number generation should be initialized. The actual seed value is not really important, as long as it is a value that is different every time the software program runs, so after each reset event.

Here are a few different approaches that could be implemented to realize an seed value that is different each time the software program runs: 

1. Keep one of the analog inputs on your microcontroller unconnected. Such a floating analog input picks up noise from the surrounding environment and readings will therefore always vary slightly. Configure your ADC peripheral to perform an analog to digital conversion of this analog input and use its result value to seed the generator.

2. If your microcontroller has EEPROM or a non-volatile register, you can use it to store a value that you use to seed the generator. Then simply increment its value by one upon software program startup. This way you have a different seed value every time your software program is started. Keep in mind though that these data storage options have a limited amount of write cycles. A better option might be to use external FRAM.

3. If your microcontroller has access to an external file system such as an SD-card, you could store a value, which you use to seed the generator, in a file. Then simply increment its value by one upon software program startup. This way you have a different seed value every time your software program is started.

4. If your microcontroller based system features a battery backed real-time clock, you could use the current date/time value to seed the generator algorithm.

| Parameter         | Description                                                  |
| ----------------- | ------------------------------------------------------------ |
| `seedInitHandler` | Pointer to the application specific seed initialization handler to use instead of the<br>default internal handler. If is of type [`tTbxRandomSeedInitHandler`](#ttbxrandomseedinithandler). |

### Checksums

More information regarding this software component, including code examples, is found [here](checksum.md).

#### TbxChecksumCrc16Calculate

```c
uint16_t TbxChecksumCrc16Calculate(uint8_t const * data,
                                   size_t          len)
```

Calculates a 16-bit CRC value over the specified data.

| Parameter | Description                                                 |
| --------- | ----------------------------------------------------------- |
| `data`    | Array with bytes over which the CRC16 should be calculated. |
| `len`     | Number of bytes in the data array.                          |

| Return value          |
| --------------------- |
| The 16-bit CRC value. |

#### TbxChecksumCrc32Calculate

```c
uint32_t TbxChecksumCrc32Calculate(uint8_t const * data,
                                   size_t          len)
```

Calculates a 32-bit CRC value over the specified data.

| Parameter | Description                                                 |
| --------- | ----------------------------------------------------------- |
| `data`    | Array with bytes over which the CRC32 should be calculated. |
| `len`     | Number of bytes in the data array.                          |

| Return value          |
| --------------------- |
| The 32-bit CRC value. |


### Cryptography

More information regarding this software component is, including code examples, found [here](crypto.md).

#### TbxCryptoAes256Encrypt

```c
void TbxCryptoAes256Encrypt(uint8_t       * data,
                            size_t          len,
                            uint8_t const * key)
```

Encrypts the len-bytes in the specified data-array, using the specified 256-bit (32 bytes) key. The results are written back into the same array.

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `data`    | Pointer to the byte array with data to encrypt. The encrypted bytes are stored in the<br>same array. |
| `len`     | The number of bytes in the data-array to encrypt. It must be a multiple of 16, as this is<br>the AES256 minimal block size. |
| `key`     | The 256-bit encryption key as a array of 32 bytes.           |

#### TbxCryptoAes256Decrypt

```c
void TbxCryptoAes256Decrypt(uint8_t       * data,
                            size_t          len,
                            uint8_t const * key)
```

Decrypts the len-bytes in the specified data-array, using the specified 256-bit (32 bytes) key. The results are written back into the same array.

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `data`    | Pointer to the byte array with data to decrypt. The decrypted bytes are stored in the<br>same array. |
| `len`     | The number of bytes in the data-array to decrypt. It must be a multiple of 16, as this is<br>the AES256 minimal block size. |
| `key`     | The 256-bit decryption key as a array of 32 bytes.           |

### Platform

More information regarding this software component is, including code examples, found [here](platform.md).

#### TbxPlatformLittleEndian

```c
uint8_t TbxPlatformLittleEndian(void)
```

Utility function to determine if the targets memory storage organization is little endian (Intel) or big endian (Motorola).

| Return value                                          |
| ----------------------------------------------------- |
| TBX_TRUE for little endian, TBX_FALSE for big endian. |

