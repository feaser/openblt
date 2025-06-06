# Extra functionality

You can find some optional and extra functionality in the directory `source/extra`. This section provides additional information regarding this functionality.

## MicroTBX for FreeRTOS

The [FreeRTOS](https://www.freertos.org) real-time operating system is widely used in the embedded industry. No wonder, because it's free, open source, high quality, MISRA compliant and maintained by [AWS](https://aws.amazon.com/). 

Whenever you use both FreeRTOS and MicroTBX, a few pieces of functionality are redundant. Both MicroTBX and FreeRTOS offer heap management and assertion functionality. There is nothing wrong with using both. However, if you'd like to reap the benefits of MicroTBX, while also using FreeRTOS, consider using the files in directory:

* `source/extra/freertos/`

### Heap management

FreeRTOS ships with a few different examples for heap management. Some allow just one-time memory allocation, some feature functionality to release allocated memory again, with minimal memory fragmentation. You can find these examples in directory:

* `FreeRTOS/Source/portable/MemMang/`

The file `tbx_freertos.c` offers an alternative heap management implementation, using MicroTBX's memory pools. This allows you to dynamically allocate and release memory on the heap, for your application's FreeRTOS objects.

To use this heap management solution, you just need to remove the `heap_x.c` source file from your project and compile and link `tbx_freertos.c` instead.

### Assertions

In the FreeRTOS configuration header file `FreeRTOSConfig.h`, you can add and configure the `configASSERT` macro to enable assertions in the FreeRTOS code base. MicroTBX includes an assertion module that you can use for this. The easiest way to link the MicroTBX assertion `TBX_ASSERT` macro to the FreeRTOS `configASSERT` macro, is by including the `tbx_freertos.h` header file all the way at the end. Just before the last `#endif`:

```c
#define INCLUDE_vTaskCleanUpResources                 1
#define INCLUDE_vTaskSuspend                          1
#define INCLUDE_vTaskDelayUntil                       1
#define INCLUDE_vTaskDelay                            1
#define INCLUDE_uxTaskGetStackHighWaterMark           1

/* Use MicroTBX assertion in FreeRTOS. */
#include "tbx_freertos.h" /* <---- ADD THIS LINE */

#endif /* FREERTOS_CONFIG_H */
```

Just make sure to add the directory, where the `tbx_freertos.h` resides, to your compiler's search path for included header files.

Alternatively, you can directly add and configure the `configASSERT` macro as follows in `FreeRTOSConfig.h`:

```c
#include "microtbx.h"
#define configASSERT( x ) TBX_ASSERT( x )
```

## C++ new and delete using MicroTBX memory pools

On a microcontroller it is totally fine to dynamically allocate memory on the heap using `new` (or `malloc`). It gets potentially troublesome when you also release it at run-time using `delete` (or `free`). Multiple allocation and release operations can cause memory fragmentation. In a worst case scenario this leads to memory allocation failures, because of running out of heap memory.

A possible solution is by dynamically allocating memory using memory pools. This lowers the risk of memory fragmentation. With a carefully selected memory pool setup, you can even eliminate this risk completely. Allowing you to dynamically allocate and release memory during run-time. MicroTBX includes a ready-made memory pool module that can be used for this purpose.

One way to approach this is by using C++ *placement new* instead of the usual *new*. This allows you the first allocate memory from a memory pool and then place the new object exactly at that memory, instead of having the `new` operator do the allocation. The only issue with this is that there is no *placement delete*. This means that to delete the object, you need to manually call its destructor and then give the allocated memory back to the memory pool. Definitely an option, it just requires a bit more work.

Another approach is to overload the default `new` and `delete` operators to do all the memory allocation and release using memory pools automatically. The following source-file implements these operator overloads for the [GNU ARM Embedded](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain) toolchain:

* `source/extra/cplusplus/tbxcxx.cpp`

By compiling and linking this source file with your project, the global `new` and `delete` operators are overloaded, such that they by default always use the memory pools module of MicroTBX. This also apply to objects created using smart pointers.

