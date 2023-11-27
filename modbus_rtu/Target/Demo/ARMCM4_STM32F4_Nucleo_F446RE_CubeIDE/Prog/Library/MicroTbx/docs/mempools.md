# Memory pools

Dynamic memory allocation with the help of a memory pool is required for data that has a lifetime longer than just one function, yet shorter than the run-time of the entire software program. This is typically the case in event driven software.

Take for example an embedded software program that communicates via CAN. When a CAN message reception interrupt triggers, the contents of the newly received CAN message are typically stored somewhere for later processing at task level. In this case data storage for the CAN message can be dynamically allocated and freed when it is no longer needed.

The C standard library functions `malloc()` and `free()` can unfortunately not be used for this in a RAM constrained microcontroller based system. Both functions treat the heap as
a byte pool, which means there is always a fragmentation risk present. This risk is unacceptable for an embedded software program.

Note that if dynamic memory allocation is needed, where the lifetime of the allocated data equals the run-time of the entire software program, it is better to use the method of static memory preallocation with the help of the [heap](heap.md) software component. The heap software component is also a part of MicroTBX.

Each memory pool consists of a run-time configurable amount of fixed-sized data blocks. The software program has full control over how many memory pools it creates and how many bytes the data blocks.

## Usage

A prerequisite to performing dynamic memory allocation, is that the software program first
creates one or more memory pools. Function [`TbxMemPoolCreate()`](apiref.md#tbxmempoolcreate) creates a memory pool. Through the function parameters you can specify the number of bytes for each data block inside the pool and how many data blocks it should hold.

It is recommended to create the memory pools during the software program initialization, before the infinite program loop is entered. However, it is safe to create memory pools at any point in the software program. Note that if you attempt to create a memory pool with a data block size that already exists, the memory pools software component is smart enough to
extend the already existing memory pool, instead of creating a new one.


The trick is to tune the memory pools to your specific software program needs. When unsure about the what memory pools to create, it is a good starting point to create memory pools with sizes that are powers of two. For example, 8, 16, 32, 64, 128, etc.

Once the memory pools are created, memory allocation with the memory pool software component is actually quite similar to calling the C standard library functions. To allocate memory, call [`TbxMemPoolAllocate()`](apiref.md#tbxmempoolallocate) instead of `malloc()`. The best fitting memory pool for the data size requested, is automatically selected. Once the allocated data is no longer needed, call [`TbxMemPoolRelease()`](apiref.md#tbxmempoolrelease), instead of `free()`.

## Examples

The following example program demonstrates how memory pools are created and proves that data from the memory pools can be dynamically allocated and released over and over again. It it also an example of how you can expand and
existing memory pool at a later point in time.

```c
void main(void)
{
  size_t    idx;
  uint8_t * dataPtr[16];
  uint8_t   firstLoop = TBX_TRUE;

  /* Create 2 memory pools with block sizes 16 and 32 that have 8
   * data blocks per pool.
   */
  TbxMemPoolCreate(8U, 16U);
  TbxMemPoolCreate(8U, 32U); /* Not actually used in this example. */

  /* Enter the infinite program loop. */
  for (;;)
  {
    /* Allocate all blocks in the memory pool with size 16. A smaller
     * block size (12 in this case) is automatically matched to the
     * memory pool with the next size up (16 in this case).
     */
    for (idx = 0U; idx < 8U; idx++)
    {
      dataPtr[idx] = TbxMemPoolAllocate(12U);
      TBX_ASSERT(dataPtr[idx] != NULL);
    }

    /* Note that when the memory pool with block sizes of 16 bytes is
     * exhausted, the memory pool module does NOT automatically switch
     * to a memory pool with larger sized blocks. This was done on
     * purpose by design.
     *
     * The reason for this is that it is now possible to expand an
     * existing memory pool when it is full. Assume a situation where
     * all blocks in the memory pool are already allocated. The next
     * call to TbxMemPoolAllocate() therefore fails. You could now
     * call TbxMemPoolCreate() again for the same block size and the
     * original memory pool is expanded automatically.
     *
     * This is actually a very powerful feature, as it enables you
     * to dynamically enlarge the memory pool whenever needed,
     * provided the heap is configured to be large enough. A
     * simplified demonstration of this follows here.
     */
    if (firstLoop == TBX_TRUE)
    {
      /* Reset the flag because this part should only execute during
       * the first loop iteration.
       */
      firstLoop = TBX_FALSE;
      /* During the first loop iteration, the next allocation should
       * fail because all 8 blocks in the memory pool with size 16
       * are already allocated.
       */
      TBX_ASSERT(TbxMemPoolAllocate(16U) == NULL);
      /* Now expand the memory pool with block size 16 to add
       * another 8 blocks.
       */
      TbxMemPoolCreate(8U, 16U);
    }

    /* The memory pool with block size of 16 should now have 16
     * blocks. 8 were allocated, so it should be possible to
     * allocate another 8.
     */
    for (idx = 8U; idx < 16U; idx++)
    {
      dataPtr[idx] = TbxMemPoolAllocate(12U);
      TBX_ASSERT(dataPtr[idx] != NULL);
    }

    /* The memory pool with block size of 16 should now be
     * exhausted and it should therefore not be possible
     * to allocate another block it in. Let's test this.
     */
    TBX_ASSERT(TbxMemPoolAllocate(16U) == NULL);

    /* Now release all the allocated data blocks again. */
    for (idx = 0U; idx < 16U; idx++)
    {
      TbxMemPoolRelease(dataPtr[idx]);
    }
  }
}
```

## Configuration

The memory pool software component itself does not have to be configured. However, when
creating memory pools with function [`TbxMemPoolCreate()`](apiref.md#tbxmempoolcreate), the data blocks are statically preallocated on the heap with the help of the [heap](heap.md) module. In case the memory pool creation fails, it is likely that the heap size needs to be increased using the macro [`TBX_CONF_HEAP_SIZE`](apiref.md#configuration):

```c
/** \brief Configure the size of the heap in bytes. */
#define TBX_CONF_HEAP_SIZE                       (2048U)
```
