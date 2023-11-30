# Assertions

Assertions are statements used for checking situations that should never happen. It is a good coding practice to add such statements to your software program. For example to validate the values of function
parameters, to check pointers for not being NULL, to check that an array index is not out of range, etc.

## Usage

In MicroTBX, assertions are implemented with the [`TBX_ASSERT(argument)`](apiref.md#generic) macro. The argument must evaluate
to boolean `true` for the program to continue its execution as expected. If the argument evaluates to
boolean `false`, an error is triggered.

In this error case, MicroTBX jumps to an internal function with an infinite loop to hang the program. This
function is called with two important parameters for debugging purposes: the filename of the source file
that caused the assertion to fail and the line number inside this file where the assertion occurred.

Practically, this means that when debugging and testing your software program, you simply set a breakpoint
inside the internal function `TbxAssertTrigger()`. If an assertion is triggered, the debugger stops at the
breakpoint. At this point you can inspect the function parameters `file` and `line` to determine which
assertion statement caused the problem. Once you know the root cause of the assertion error, you can start
working on a solution to fix the problem from happening in the future.

In case you do not like the default assertion error handling, MicroTBX also makes it possible for you to configure your own assertion handler with function [`TbxAssertSetHandler()`](apiref.md#tbxassertsethandler). For example to execute a
`printf` statement, to turn on an LED, or to write assertion error info to a file an on SD-card to indicate that an assertion error was detected.

## Examples

The following example demonstrates how assertions can be used to test that a function parameter has a
valid value range:

```c
uint8_t myArray[4] = { 1, 2, 3, 4 };

void IncrementArrayElement(int8_t index)
{
  /* Make sure the array index is positive. */
  TBX_ASSERT(index >= 0);
  /* Make sure the array index is not out of range. */
  TBX_ASSERT(index < (sizeof(myArray)/sizeof(myArray[0])));

  /* Increment the array element at the requested index. */
  myArray[index]++;
}
```

The next example shows how you can register your own assertion error handler, which overrides the
internal error handling:

```c
#include <stdio.h>

void MyAssertionHandler(const char * const file, uint32_t line)
{
  /* Display the assertion error info. */
  printf("[ERROR] Assertion in %s at line %lu", file, line);
}

int main(void)
{
  /* Register the application specific assertion handler. */
  TbxAssertSetHandler(MyAssertionHandler);

  /* TODO Implement your program here. */

  return 0;
}
```

## Configuration

The enable the assertion error handling, set the macro [`TBX_CONF_ASSERTIONS_ENABLE`](apiref.md#configuration) to a value of `1`:

```c
/** \brief Enable/disable run-time assertions. */
#define TBX_CONF_ASSERTIONS_ENABLE              (1U)
```

To disable the assertion error handling, set this macro to a value of `0`. Note that there is not need
to remove the [`TBX_ASSERT()`](apiref.md#generic) macros from your code. MicroTBX automatically makes the [`TBX_ASSERT()`](apiref.md#generic) macro and empty macro without side effect, when assertion error handling was disabled.

Assertions are typically enabled in a debug version of the software program and disabled in a
release version of the software program.

## Extra tips

Make sure to write your assertion statements such that you do not accidentally remove functionality
from your software program, after disabling the assertion error handling. So instead of the following:

```c
FATFS fs;

void MountDrive(void)
{
  /* Mount the drive and make sure the drive mounting succeeded. */
  TBX_ASSERT(f_mount(&fs, "0:", 0) == FR_OK);
}
```

Do the following:

```c
FATFS fs;

void MountDrive(void)
{
  FRESULT fresult;

  /* Mount the drive. */
  fresult = f_mount(&fs, "0:", 0);

  /* Make sure the drive mounting succeeded. */
  TBX_ASSERT(fresult == FR_OK);
}
```

To be sure that the before mentioned problem does not occur, one could make the argument that assertion error handling should simply never be disabled.
