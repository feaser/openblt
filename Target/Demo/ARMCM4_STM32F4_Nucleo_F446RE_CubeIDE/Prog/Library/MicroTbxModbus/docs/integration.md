# Integration

To make the MicroTBX-Modbus functionality available to your embedded software project, you need to integrate its source code into your project. This section of the user manual walks you through this process step-by-step. It covers two integration approaches:

* Classical integration, where you add the source files manually to your project and configure your build environment accordingly.

* CMake integration, where you leverage the power of CMake to do the heavy-lifting.

As a reference, you can look at the separate repository with [MicroTBX-Modbus demo programs](https://github.com/feaser/microtbx-demos/tree/main/demos/modbus). It includes demo programs for different microcontroller boards that are preconfigured and already have MicroTBX-Modbus fully integrated.

## Prerequisites

Since MicroTBX-Modbus builds upon the MicroTBX base component, make sure you already integrated the MicroTBX base component into your embedded software project. You can find the MicroTBX integration instructions in the [MicroTBX user manual](https://feaser.github.io/microtbx/#integration).

## Classical integration

Adding MicroTBX-Modbus to your software project is a simple five step process: 

1. Copy all files from the `source` directory to your project.
2. Copy the `source/template/tbxmb_port.c` port template source file to your project.
2. Copy the `source/osal/tbxmb_XXX.c` for your selected operating system to your project.
3. Configure your project such that the added `.c` files are compiled and linked during a build.
4. Add the directories that contain the `.h` files to your compiler's include search path.

## CMake integration

The use of [CMake](https://cmake.org/) to manage the build environment rapidly gains popularity among embedded software developers. It makes adding third-party libraries, such as MicroTBX-Modbus, a breeze:

1. Copy the entire MicroTBX-Modbus directory into your project. Manually or as a [Git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules).
2. Use `add_subdirectory()` to register the MicroTBX-Modbus interface library. 
3. Copy the `source/template/tbxmb_port.c` port template source file to your project and add it as a source file to `add_executable()`. 
4. Add the `microtbx-modbus` interface library to `target_link_libraries()`. 
4. Add the `microtbx-modbus-osal-XXX` interface library for your selected operating system to `target_link_libraries()`. 

Minimal `CMakeLists.txt` example, if you copied MicroTBX-Modbus to directory `third_party/microtbx-modbus`:

```cmake
project(MyProject)

add_subdirectory(third_party/microtbx-modbus)

add_executable(MyProject
	main.c
    tbxmb_port.c
)

target_link_libraries(MyProject
    microtbx-modbus
    microtbx-modbus-osal-superloop
)
```

## Adjust the port

The MicroTBX-Modbus source code itself is fully hardware independent. The `tbxmb_port.c` port source file implements the hardware specifics. This means that you only need to update this source file, to get MicroTBX-Modbus working on your specific microcontroller system. You can find detailed instructions, on how to port MicroTBX-Modbus to your platform, in the [portation](portation.md) section of this user manual.

## Usage

Add the following lines to each source-file, where you intend to make use of MicroTBX-Modbus:

```c
#include <microtbx.h>
#include <microtbxmodbus.h>
```



