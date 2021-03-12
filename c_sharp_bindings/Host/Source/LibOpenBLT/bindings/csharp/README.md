### C# wrapper for LibOpenBLT
The OpenBLT Host Library ([LibOpenBLT](https://www.feaser.com/openblt/doku.php?id=manual:libopenblt)) contains an application programming interface (API) for communicating with a microcontroller target, running the OpenBLT bootloader, for making firmware updates on the target. The goal of the OpenBLT Host Library is to empower you to quickly and efficiently create your own firmware update tool, in case you don’t prefer the standard MicroBoot and BootCommander tools that are included in the OpenBLT bootloader package.

This directory contains the C# wrapper for LibOpenBLT, which enables you to quickly develop your own firmware update tool in the C# programming language, for example with Visual Studio.

### Using the wrapper
To use the C# wrapper for LibOpenBLT, add the file **openblt.cs** from this directory to your C# project. 

Here is a minimal C# console program that demonstrates how to display the version of LibOpenBLT:

```c#
using System;

namespace ConsoleApp
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("LibOpenBLT version number: {0}", OpenBLT.Lib.Version.GetNumber());
        }
    }
}
```

### Run-time libraries
Copy the LibOpenBLT related run-time libraries into the directory where your program's executable is located. Refer to the following section on the OpenBLT Wiki for a overview of these run-time libraries:

https://www.feaser.com/openblt/doku.php?id=manual:libopenblt#run-time_libraries

These run-time libraries can be found in the ./Host directory of the OpenBLT bootloader package. These run-time libraries should also be included, when distributing your program.

### Configure platform target

Under Microsoft Windows, the LibOpenBLT shared library (**libopenblt.dll**) is 32-bit. For this reason you need to build your own C# program as a 32-bit application as well. 

To configure this, go to your project's properties in Microsoft Visual Studio. In the *Build* settings, set the **Platform target** to **x86**.

