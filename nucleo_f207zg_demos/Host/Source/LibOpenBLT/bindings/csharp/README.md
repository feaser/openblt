## C# wrapper for LibOpenBLT
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

Under Microsoft Windows, the LibOpenBLT shared library (**libopenblt.dll**) is 64-bit ever since OpenBLT version 1.14. For this reason you need to build your own C# program as a 64-bit application as well.  To configure this, go to your project's properties in Microsoft Visual Studio. In the *Build* settings, set the **Platform target** to **x64**.

If you use the LibOpenBLT shared library from before OpenBLT version 1.14 or if you rebuilt it yourself as 32-bit, then you need to build your own C# program as a 32-bit application. To configure this, go to your project's properties in Microsoft Visual Studio. In the *Build* settings, set the **Platform target** to **x86**.

### Example program

The following console program demonstrates how to use the C# wrapper for LibOpenBLT to perform a firmware update via RS232. It can be used as a starting point and reference for developing your own firmware update tool.

```c#
using System;

namespace ConsoleApp
{
    class Program
    {
        static void Main(string[] args)
        {
            // ------------ Display library info ----------------------------------------
            Console.WriteLine("LibOpenBLT version string: {0}", OpenBLT.Lib.Version.GetString());

            // ------------ Initialization ---------------------------------------------- 
            OpenBLT.Lib.Session.SessionSettingsXcpV10 sessionSettings;
            sessionSettings.timeoutT1 = 1000;
            sessionSettings.timeoutT3 = 2000;
            sessionSettings.timeoutT4 = 10000;
            sessionSettings.timeoutT5 = 1000;
            sessionSettings.timeoutT6 = 50;
            sessionSettings.timeoutT7 = 2000;
            sessionSettings.seedKeyFile = "";
            sessionSettings.connectMode = 0;

            OpenBLT.Lib.Session.TransportSettingsXcpV10Rs232 transportSettingsRs232;
            transportSettingsRs232.portName = "COM14";
            transportSettingsRs232.baudrate = 57600;

            OpenBLT.Lib.Session.Init(sessionSettings, transportSettingsRs232);
            OpenBLT.Lib.Firmware.Init(OpenBLT.Lib.Firmware.FIRMWARE_PARSER_SRECORD);

            // ------------ Load firmware ----------------------------------------------- 
            if (OpenBLT.Lib.Firmware.LoadFromFile("demoprog_stm32f091.srec", 0) != OpenBLT.Lib.RESULT_OK)
            {
                Console.WriteLine("Could not load firmware data from the S-record.");
            }

            // ------------ Connect to target ------------------------------------------- 
            if (OpenBLT.Lib.Session.Start() != OpenBLT.Lib.RESULT_OK)
            {
                Console.WriteLine("Could not connect to the target.");
            }

            // ------------ Erase flash ------------------------------------------------- 
            for (UInt32 segmentIdx=0; segmentIdx<OpenBLT.Lib.Firmware.GetSegmentCount(); segmentIdx++)
            {
                var segment = OpenBLT.Lib.Firmware.GetSegment(segmentIdx);
                if (OpenBLT.Lib.Session.ClearMemory(segment.address, segment.data.Length) != OpenBLT.Lib.RESULT_OK)
                {
                    Console.WriteLine("Could not erase memory.");
                }
            }

            // ------------ Program flash -----------------------------------------------
            for (UInt32 segmentIdx = 0; segmentIdx < OpenBLT.Lib.Firmware.GetSegmentCount(); segmentIdx++)
            {
                var segment = OpenBLT.Lib.Firmware.GetSegment(segmentIdx);
                if (OpenBLT.Lib.Session.WriteData(segment.address, segment.data) != OpenBLT.Lib.RESULT_OK)
                {
                    Console.WriteLine("Could not program memory.");
                }
            }

            // ------------ Disconnect from target --------------------------------------
            OpenBLT.Lib.Session.Stop();

            // ------------ Termination -------------------------------------------------
            OpenBLT.Lib.Firmware.Terminate();
            OpenBLT.Lib.Session.Terminate();
        }
    }
}
```

