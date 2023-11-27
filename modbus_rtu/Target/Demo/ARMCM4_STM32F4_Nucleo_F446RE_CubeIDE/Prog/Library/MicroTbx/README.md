# MicroTBX
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) [![MISRA Check](https://github.com/feaser/microtbx/actions/workflows/misra_check.yml/badge.svg)](https://github.com/feaser/microtbx/actions/workflows/misra_check.yml) [![Unit Tests](https://github.com/feaser/microtbx/actions/workflows/unit_tests.yml/badge.svg)](https://github.com/feaser/microtbx/actions/workflows/unit_tests.yml) [![Publish Docs](https://github.com/feaser/microtbx/actions/workflows/publish_docs.yml/badge.svg)](https://github.com/feaser/microtbx/actions/workflows/publish_docs.yml)

MicroTBX is an open source Microcontroller ToolBoX consisting of software components commonly needed in embedded software applications. Think of it as a Swiss army knife for your firmware. The following software components are included in MicroTBX:

* Assertions - For checking situations that should never happen.
* Critical Sections - For mutual exclusive access to shared resources.
* Heap - For static memory pre-allocation on the heap.
* Memory Pools - For pool based dynamic memory allocation on the heap.
* Linked Lists - For dynamically sized lists of data items.
* Random Numbers - For generating random numbers.
* Checksums - For calculating data checksums.
* Cryptography - For data encryption and decryption.
* Platform - For platform specifics.

MicroTBX is written in the C programming language (C99) with MISRA compliance in mind. It currently supports all microcontrollers based on an ARM Cortex-M core, for example: ST STM32, Infineon XMC, and NXP S32K. There is little effort involved with porting MicroTBX to different microcontroller families.

While developing MicroTBX, special care was given to making the code threadsafe. Access to shared resources are serialized with the help of critical sections. This makes it safe to use MicroTBX in a multithreaded environment, for example when your embedded software program is built upon a pre-emptive RTOS.

If you do not feel like reinventing the wheel, consider using MicroTBX. Ideally, MicroTBX is integrated into your embedded software program at the start of each new project. The following image illustrates where MicroTBX fits into your firmware's architecture:

![alt text](docs/images/software_architecture.png "Software architecture with MicroTBX")

# Getting Started

To get started with MicroTBX, it is recommended to download the latest stable release from the [releases](https://github.com/feaser/microtbx/releases/) page. Next, follow the integration steps outlined on the first page of the [user manual](https://feaser.github.io/microtbx).

Additionally, an assortment of demo programs is available in a separate Git repository: 

* https://github.com/feaser/microtbx-demos

# User manual

The online user manual is located at the following location:

* [https://feaser.github.io/microtbx](https://feaser.github.io/microtbx)

# Development

Development of MicroTBX takes place at GitHub. Feel free to contribute by submitting issues and pull requests.

* [https://github.com/feaser/microtbx/](https://github.com/feaser/microtbx/)

# Contact

Development and maintenance of MicroTBX is sponsored by Feaser. Feaser also offers integration/customization services, and professional technical support on a commercial basis:

* [https://www.feaser.com](https://www.feaser.com)
