# Introduction to MicroTBX-Modbus

MicroTBX-Modbus is a modern Modbus communication stack, targeting microcontroller based embedded systems. Its aim is to be: easy to use, easy to port, high quality, well maintained, stable and flexible. The ideal solution for any embedded software engineer, with an interest in adding Modbus communication to their product.

## Features

* Carefully crafted application programming interface ([API](apiref.md)), focusing on ease of use.
* Written in the C programming language (C99) with high [MISRA compliance](misra.md).
* Supports both Modbus client and Modbus server functionality.
* Supports multi channel for both the Modbus client and server.
* Flexible [dual licensing](licensing.md) model.
* Designed such that it can be used with and without and RTOS.
* Easy to [integrate](integration.md) into existing software projects, especially when using CMake.
* Includes [C++ wrappers](extras.md) for those preferring to develop in an object-oriented manner.
* Quick and simple to [adjust](portation.md) to your microcontroller system.
* No compile-time configuration needed.
* Option to implement additional and custom Modbus function codes.

## Supported function codes

MicroTBX-Modbus currently supports the following Modbus function codes:

| Function Code | Name                                               |
| :-----------: | -------------------------------------------------- |
|       1       | Read Coils                                         |
|       2       | Read Discrete Inputs                               |
|       3       | Read Holding Registers                             |
|       4       | Read Input Registers                               |
|       5       | Write Single Coil                                  |
|       6       | Write Single Register                              |
|       8       | Diagnostics (sub codes: 0, 10, 11, 12, 13, 14, 15) |
|      15       | Write Multiple Coils                               |
|      16       | Write Multiple Registers                           |

Note that MicroTBX-Modbus includes functionality, enabling you to extend it by adding support for additional and custom function codes.

## Why another Modbus stack?

With Modbus being such a convenient and mature communication protocol, several other Modbus software stacks exist; Both closed sourced and open sourced. Why bother with developing and maintaining yet another one? It turns out that most of the existing ones all seem to be limiting on at least one front:

* No multi-channel support.
* Not including both client and server functionality.
* Not being able to add support for additional and custom function codes. 
* Skipping features needed for protocol compliance.
* No open source option.
* Not actively maintained.

MicroTBX-Modbus addresses all these limitations. Thanks to the flexible [dual licensing](licensing.md) model, you can start out right away with the open source GPLv3 version. Perfect for testing, evaluation and prototyping purposes. Once you're satisfied with it and would like to include MicroTBX-Modbus in your proprietary closed sourced product, you can move on to the commercial license.

The only reason not to use MicroTBX-Modbus is that it currently only supports Modbus RTU communication. Note though that support for ASCII and TCP communication is planned for the future.

## System requirements

With MicroTBX-Modbus being a modern communication stack, the focus is more on ease and flexibility of use for the developer, and less on keeping the ROM footprint low. Therefore the recommended system requirements are slightly higher than comparative Modbus software solutions:

It is recommended to use a microcontroller with at least 32 KiB flash and 4 KiB RAM. However, it will run on a basic 8-bit microcontroller with just 10 KiB of flash and 1.5 KiB RAM. Although you then run out of storage space quickly, when adding your own firmware’s functionality.

## Next steps

The [getting started](gettingstarted.md) section of this user manual shows you how to quickly setup a Modbus server and client. Definitely worth a glance if you're new to MicroTBX-Modbus.

For those who want to see MicroTBX-Modbus in action, you can find ready-to-run demo programs located in a separate repository. These demo programs target an ever growing collection of popular microcontroller evaluation boards and serve as a good starting point:

* [https://github.com/feaser/microtbx-demos](https://github.com/feaser/microtbx-demos)

If you're ready to integrate MicroTBX-Modbus into your own embedded software project, head over to the [integration](integration.md) section of this user manual for detailed instructions.

MicroTBX-Modbus itself is hardware independent. To handle the hardware specifics of your microcontroller, you just need to implement a few port functions. A template source-file is provided. You can find detailed instructions in the [portation](portation.md) section of this user manual.

Once you got everything up-and-running and would like to use MicroTBX-Modbus in the closed source proprietary firmware of your product, make sure to upgrade to the [commercial license](licensing.md). The default GPLv3 licensed version is not suitable for that use case.
