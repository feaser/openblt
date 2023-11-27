# Random numbers

The random number generator software component offers functionality for
generating 32-bit random numbers. The generator algorithm is based on the linear
feedback shift register approach ([LFSR](https://en.wikipedia.org/wiki/Linear-feedback_shift_register)), specifically the one presented in [application note 4400](https://www.maximintegrated.com/en/app-notes/index.mvp/id/4400) from Maxim Integrated.

## Usage

Whenever a random number is to be obtained, call function [`TbxRandomNumberGet()`](apiref.md#tbxrandomnumberget).
The internal generator algorithm needs to be properly seeded. By default, the
same constant value is assigned to the seed. This means that each time function
[`TbxRandomNumberGet()`](apiref.md#tbxrandomnumberget) is called, you get a different number, but the consecutive
numbers will always be the same each time your program is restarted:

| Reset #1     | Reset #2     | Reset #3     |
| ------------:| ------------:| ------------:|
| 3196210335   | 3196210335   | 3196210335   |
| 2375684060   | 2375684060   | 2375684060   |
| 3691277220   | 3691277220   | 3691277220   |
| 3596136368   | 3596136368   | 3596136368   |

For some applications, this behavior is acceptable. However, most applications
require different random numbers to be generated each time the software program
is started. This is achieved by calling [`TbxRandomSetSeedInitHandler()`](apiref.md#tbxrandomsetseedinithandler) to
register your own seed initialization function.

The ideal implementation of your seed initialization function is such that it
returns a different seed value, each time the software program is newly started.
Here are some possible approaches to realize such a changing seed value:

1. Keep one of the analog inputs on your microcontroller unconnected. Such a
   floating analog input picks up noise from the surrounding environment and
   readings will therefore always vary slightly. Configure your ADC peripheral
   to perform an analog to digital conversion of this analog input and use its
   result value to seed the generator.
2. If your microcontroller has EEPROM or a non-volatile register, you can use it
   to store a value that you use to seed the generator. Then simply increment
   its value by one upon software program startup. This way you have a different
   seed value every time your software program is started.
   * Keep in mind though that these data storage options have a limited amount
     of write cycles. A better option might be to use external FRAM.
3. If your microcontroller has access to an external file system such as an
   SD-card, you could store a value, which you use to seed the generator, in a
   file. Then simply increment its value by one upon software program startup.
   This way you have a different seed value every time your software program is
   started.
4. If your microcontroller based system features a battery backed real-time
   clock, you could use the current date/time value to seed the generator
   algorithm.

Here are the results of a demo program that implements method (1) with the
floating analog input to seed the generator algorithm:  

| Reset #1     | Reset #2     | Reset #3     |
| ------------:| ------------:| ------------:|
| 1193105914   |   99986390   | 2131584566   |
| 3607630837   | 1266123007   |    2093232   |
| 2184628642   | 3493368727   | 1164981103   |
|  793839531   |  296055939   | 3318979929   |


## Examples

To generate a new random number, call the function [`TbxRandomNumberGet()`](apiref.md#tbxrandomnumberget) and it
will return a new 32-bit random number. Example:

```c
uint32_t number;

/* Generate a new random number. */
number = TbxRandomNumberGet();
```

The following function is an example implementation of an application specific
seed initialization. It is based on the above described method (1) where a
floating analog input provides randomness upon each read of the analog pin:

```c
uint32_t CustomSeedInitHandler(void)
{
  /* Create a 32-bit seed value by combining two reads of the floating analog
   * pin.
   */
  return (AnalogFloatGet() << 16u) | AnalogFloatGet();
}
```

The above example assumes that the application provides the function
`AnalogFloatGet()` for performing an analog to digital conversion of the analog
input pin that is left floating.

To register the function `CustomSeedInitHandler()` with the random number
generator software component, make the following function call during the
initialization of your software program:

```c
/* Register application specific seed initialization function. */
TbxRandomSetSeedInitHandler(CustomSeedInitHandler);
```
