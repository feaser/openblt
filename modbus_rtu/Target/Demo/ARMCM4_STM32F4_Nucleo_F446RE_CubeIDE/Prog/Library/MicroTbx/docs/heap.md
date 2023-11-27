# Heap

In an embedded software program, memory can be dynamically allocated. The heap is
available for this, which is a part of RAM specifically reserved for dynamically
allocated memory.

The C standard library offers the functions `malloc()` and `free()` for allocating
and releasing memory on the heap. Both functions treat the heap as a byte pool, which
means there is always a fragmentation risk present. The fragmentation eventually
leads to a situation where memory can no longer be allocated, resulting in lost
functionality or a crash of the software program in the worst case.

This fragmentation risk is therefore unacceptable and another solution needs to be
found for handling dynamic allocation. The solution is two-fold:

1. Use static memory preallocation. This entails performing the memory
   allocation once during software program initialization and never releasing
   (free-ing) the allocated memory. This means that the lifetime of the allocated data spans the run-time of the entire software program.
2. Use memory pools. With a memory pool, memory is allocated as fixed-size memory
   blocks (partitions). The benefit of dynamic memory allocation using memory
   pools is that the memory can be released again without the before mentioned
   fragmentation risk. It is perfect in situations when temporary data storage is needed, and the lifetime of the data is longer than just one function.

MicroTBX offers software components for both solutions. The heap software
component described in this section covers solution (1) with static memory
preallocation. The [memory pools](mempools.md) software component covers
solution (2).

## Usage

The heap software component offers the function [`TbxHeapAllocate()`](apiref.md#tbxheapallocate) for performing
memory allocation on the heap. Because it is meant for static memory preallocation
only, there is no function for free-ing the allocated memory. The idea is that
function [`TbxHeapAllocate()`](apiref.md#tbxheapallocate) is only called during the initialization of the
software program, so before the infinite program loop is entered. It is basically
its own implementation of `malloc()`, without `free()`.

To find out how many bytes of memory are still available on the heap, function
[`TbxHeapGetFree()`](apiref.md#tbxheapgetfree) can be called.

If a software program has a need to allocate and release memory during the
infinite program loop, the memory allocation should be performed with the
functionality present in the [memory pools](mempools.md) software component.

## Examples

The following example demonstrates how to call the functions of the heap software
component. Memory for a FIFO buffer is preallocated during the software program
initialization.

```c
/* A first-in first-out buffer of 32-bit values. */
uint32_t       * fifoBuffer  = NULL;
uint32_t const   fifoMaxSize = 32;
uint8_t          fifoIdx;

/* Make sure there is enough space on the heap for the FIFO buffer. */
if (TbxHeapGetFree() < (fifoMaxSize * sizeof(uint32_t)))
{
  TBX_ASSERT(TBX_FALSE);
}

/* Perform static preallocation of the FIFO buffer. */
fifoBuffer = TbxHeapAllocate(fifoMaxSize * sizeof(uint32_t));

/* Make sure the allocation was successful. */
TBX_ASSERT(fifoBuffer != NULL);

/* Initialize the FIFO buffer. */
for (fifoIdx = 0; fifoIdx < fifoMaxSize; fifoIdx++)
{
  fifoBuffer[fifoIdx] = 0;
}
```

## Configuration

The maximum size of the heap is configured with macro [`TBX_CONF_HEAP_SIZE`](apiref.md#configuration):

```c
/** \brief Configure the size of the heap in bytes. */
#define TBX_CONF_HEAP_SIZE                       (2048U)
```
