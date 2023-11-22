//***************************************************************************************
//  Description: Class for accessing the OpenBLT shared library in C#.
//    File Name: openblt.cs
//
//---------------------------------------------------------------------------------------
//                          C O P Y R I G H T
//---------------------------------------------------------------------------------------
//   Copyright (c) 2021 by Feaser    http://www.feaser.com    All rights reserved
//
//   This software has been carefully tested, but is not guaranteed for any particular
// purpose. The author does not offer any warranties and does not guarantee the accuracy,
//   adequacy, or completeness of the software and is not responsible for any errors or
//              omissions or the results obtained from use of the software.
//
//---------------------------------------------------------------------------------------
//                            L I C E N S E
//---------------------------------------------------------------------------------------
// This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. See the GNU General Public License for more details.
//
// You have received a copy of the GNU General Public License along with OpenBLT. It
// should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
//
//***************************************************************************************
using System;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace OpenBLT
{
    /// <summary>
    /// C# wrapper class for the OpenBLT host library called LibOpenBLT. For more
    /// information about LibOpenBLT, refer to:
    /// https://www.feaser.com/openblt/doku.php?id=manual:libopenblt
    /// </summary>
    /// <remarks>
    /// Note that under Microsoft Windows, the LibOpenBLT shared library (libopenblt.dll)
    /// is 64-bit, ever since OpenBLT version 1.14 . For this reason, whatever project
    /// uses this wrapper class needs to be built as a 64-bit application. 
    /// 
    /// To configure this, go to your project's properties in Microsoft Visual Studio.
    /// In the "Build" settings, set "Platform target" to "x64".
    /// 
    /// When using LibOpenBLT from before OpenBLT version 1.14 or if you rebuild the 
    /// LibOpenBLT shared library yourself as 32-bit, then your project that uses this
    /// wrapper class needs to be built as a 32-bit application.
    /// 
    /// To configure this, go to your project's properties in Microsoft Visual Studio.
    /// In the "Build" settings, set "Platform target" to "x86".
    /// </remarks>
    public static class Lib
    {
        /// <summary>
        /// The name of the shared library file. Luckily there is no need to specify the
        /// file extension (.dll on Windows and .so on Linux), so this wrapper class
        /// should be cross-platform.
        /// </summary>
        /// <remarks>
        /// Note that this file and the other related run-time library files must be
        /// located either someone accessible on the searchpath or in the same
        /// directory as your application's executable. 
        /// For an overview of the LibOpenBLT run-time library files, refer to:
        /// https://www.feaser.com/openblt/doku.php?id=manual:libopenblt#run-time_libraries
        /// </remarks>
        private const String LIBNAME = "libopenblt";

        /// <summary>
        /// Function return value for when everything went okay.
        /// </summary>
        public const UInt32 RESULT_OK = 0;

        /// <summary>
        /// Function return value for when a generic error occured.
        /// </summary>
        public const UInt32 RESULT_ERROR_GENERIC = 1;

        /// <summary>
        /// Wrapper for the version module of LibOpenBLT.
        /// </summary>
        public static class Version
        {
            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern UInt32 BltVersionGetNumber();

            /// <summary>
            /// Obtains the version number of the library as an integer. The number has two 
            /// digits for major-, minor-, and patch-version.Version 1.05.12 would for
            /// example return 10512.
            /// </summary>
            /// <returns>Library version number as an integer.</returns>
            /// <example>
            /// <code>
            /// Console.WriteLine("LibOpenBLT version number: {0}", OpenBLT.Lib.Version.GetNumber());
            /// </code>
            /// </example>
            public static UInt32 GetNumber()
            {
                return BltVersionGetNumber();
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern IntPtr BltVersionGetString();

            /// <summary>
            /// Obtains the version number of the library as a null-terminated string. Version
            /// 1.05.12 would for example return "1.05.12".
            /// </summary>
            /// <returns>Library version number as a string.</returns>
            /// <example>
            /// <code>
            /// Console.WriteLine("LibOpenBLT version string: {0}", OpenBLT.Lib.Version.GetString());
            /// </code>
            /// </example>
            public static String GetString()
            {
                return Marshal.PtrToStringAnsi(BltVersionGetString());
            }
        }

        /// <summary>
        /// Wrapper for the session module of LibOpenBLT.
        /// </summary>
        public static class Session
        {
            /// <summary>
            /// XCP protocol version 1.0. XCP is a universal measurement and calibration
            /// communication protocol. It contains functionality for reading, programming,
            /// and erasing (non-volatile) memory making it a good fit for bootloader
            /// purposes.
            /// </summary>
            private const UInt32 SESSION_XCP_V10 = 0;

            /// <summary>
            /// Transport layer for the XCP v1.0 protocol that uses RS-232 serial
            /// communication for data exchange.
            /// </summary>
            private const UInt32 TRANSPORT_XCP_V10_RS232 = 0;

            /// <summary>
            /// Transport layer for the XCP v1.0 protocol that uses Controller Area Network
            /// (CAN) for data exchange.
            /// </summary>            
            private const UInt32 TRANSPORT_XCP_V10_CAN = 1;

            /// <summary>
            /// Transport layer for the XCP v1.0 protocol that uses USB Bulk for data 
            /// exchange.
            /// </summary>
            private const UInt32 TRANSPORT_XCP_V10_USB = 2;

            /// <summary>
            /// Transport layer for the XCP v1.0 protocol that uses TCP/IP for data
            /// exchange.
            /// </summary>
            private const UInt32 TRANSPORT_XCP_V10_NET = 3;

            /// <summary>
            /// Transport layer for the XCP v1.0 protocol that uses Modbus RTU for data
            /// exchange.
            /// </summary>
            private const UInt32 TRANSPORT_XCP_V10_MBRTU = 4;

            /// <summary>
            /// Structure layout of the XCP version 1.0 session settings.
            /// </summary>
            public struct SessionSettingsXcpV10
            {
                /// <summary>
                /// Command response timeout in milliseconds.
                /// </summary>
                public UInt16 timeoutT1;

                /// <summary>
                /// Start programming timeout in milliseconds.
                /// </summary>
                public UInt16 timeoutT3;

                /// <summary>
                /// Erase memory timeout in milliseconds.
                /// </summary>
                public UInt16 timeoutT4;

                /// <summary>
                /// Program memory and reset timeout in milliseconds.
                /// </summary>
                public UInt16 timeoutT5;

                /// <summary>
                /// Connect response timeout in milliseconds.
                /// </summary>
                public UInt16 timeoutT6;

                /// <summary>
                /// Busy wait timer timeout in milliseonds.
                /// </summary>
                public UInt16 timeoutT7;

                /// <summary>
                /// Seed/key algorithm library filename.
                /// </summary>
                public String seedKeyFile;

                /// <summary>
                /// Connection mode parameter in XCP connect command.
                /// </summary>
                public Byte connectMode;
            }

            /// <summary>
            /// Unmanaged structure layout of the XCP version 1.0 session settings. 
            /// </summary>
            /// <remarks>
            /// Only used internally when calling the API function inside the DLL.
            /// </remarks>
            [StructLayout(LayoutKind.Sequential)]
            private struct SessionSettingsXcpV10Unmanaged
            {
                public UInt16 timeoutT1;
                public UInt16 timeoutT3;
                public UInt16 timeoutT4;
                public UInt16 timeoutT5;
                public UInt16 timeoutT6;
                public UInt16 timeoutT7;
                public IntPtr seedKeyFile;
                public Byte connectMode;
            }

            /// <summary>
            /// Structure layout of the XCP version 1.0 RS232 transport layer settings.
            /// </summary>
            /// <remarks>
            /// The portName field is platform dependent. On Linux based systems this should be
            /// the filename of the tty-device, such as "/dev/tty0". On Windows based systems
            /// it should be the name of the COM-port, such as "COM1".
            /// </remarks>
            public struct TransportSettingsXcpV10Rs232
            {
                /// <summary>
                /// Communication port name such as /dev/tty0.
                /// </summary>
                public String portName;

                /// <summary>
                /// Communication speed in bits/sec.
                /// </summary>
                public UInt32 baudrate;
            }

            /// <summary>
            /// Unmanaged structure layout of the XCP version 1.0 RS232 transport layer settings.
            /// </summary>
            /// <remarks>
            /// Only used internally when calling the API function inside the DLL.
            /// </remarks>
            [StructLayout(LayoutKind.Sequential)]
            private struct TransportSettingsXcpV10Rs232Unmanaged
            {
                public IntPtr portName;
                public UInt32 baudrate;
            }

            /// <summary>
            /// Structure layout of the XCP version 1.0 CAN transport layer settings.
            /// </summary>
            /// <remarks>
            /// The deviceName field is platform dependent.On Linux based systems this should
            /// be the socketCAN interface name such as "can0". The terminal command "ip addr"
            /// can be issued to view a list of interfaces that are up and available. Under
            /// Linux it is assumed that the socketCAN interface is already configured on the
            /// system, before using the OpenBLT library.When baudrate is configured when
            /// bringing up the system, so the baudrate field in this structure is don't care
            /// when using the library on a Linux was system. On Windows based systems, the
            /// device name is a name that is pre-defined by this library for the supported
            /// CAN adapters. The device name should be one of the following: "peak_pcanusb",
            /// "kvaser_leaflight", or "lawicel_canusb". Field use extended is a boolean
            /// field.When set to 0, the specified transmitId and receiveId are assumed to
            /// be 11-bit standard CAN identifier. If the field is True, these identifiers 
            /// are assumed to be 29-bit extended CAN identifiers.
            /// </remarks>
            public struct TransportSettingsXcpV10Can
            {
                /// <summary>
                /// Device name such as can0, peak_pcanusb etc.
                /// </summary>
                public String deviceName;

                /// <summary>
                /// Channel on the device to use.
                /// </summary>
                public UInt32 deviceChannel;

                /// <summary>
                /// Communication speed in bits/sec.
                /// </summary>
                public UInt32 baudrate;

                /// <summary>
                /// Transmit CAN identifier.
                /// </summary>
                public UInt32 transmitId;

                /// <summary>
                /// Receive CAN identifier.
                /// </summary>
                public UInt32 receiveId;

                /// <summary>
                /// Boolean to configure 29-bit CAN identifiers.
                /// </summary>
                public Boolean useExtended;
            }

            /// <summary>
            /// Unmanaged structure layout of the XCP version 1.0 CAN transport layer settings.
            /// </summary>
            /// <remarks>
            /// Only used internally when calling the API function inside the DLL.
            /// </remarks>
            [StructLayout(LayoutKind.Sequential)]
            private struct TransportSettingsXcpV10CanUnmanaged
            {
                public IntPtr deviceName;
                public UInt32 deviceChannel;
                public UInt32 baudrate;
                public UInt32 transmitId;
                public UInt32 receiveId;
                public UInt32 useExtended;
            }

            /// <summary>
            /// Structure layout of the XCP version 1.0 NET transport layer settings.
            /// </summary>
            /// <remarks>
            /// The address field can be set to either the IP address or the hostname, such
            /// as "192.168.178.23" or "mymicro.mydomain.com". The port should be set to the
            /// TCP port number that the bootloader target listens on.
            /// </remarks>
            public struct TransportSettingsXcpV10Net
            {
                /// <summary>
                /// Target IP-address or hostname on the network.
                /// </summary>
                public String address;

                /// <summary>
                /// TCP port to use.
                /// </summary>
                public UInt16 port;
            }

            /// <summary>
            /// Unmanaged structure layout of the XCP version 1.0 TCP/IP transport layer settings.
            /// </summary>
            /// <remarks>
            /// Only used internally when calling the API function inside the DLL.
            /// </remarks>
            [StructLayout(LayoutKind.Sequential)]
            private struct TransportSettingsXcpV10NetUnmanaged
            {
                public IntPtr address;
                public UInt16 port;
            }
            
            /// <summary>
            /// Structure layout of the XCP version 1.0 Modbus RTU transport layer settings.
            /// </summary>
            /// <remarks>
            /// The portName field is platform dependent. On Linux based systems this should be
            /// the filename of the tty-device, such as "/dev/tty0". On Windows based systems
            /// it should be the name of the COM-port, such as "COM1".
            /// </remarks>
            public struct TransportSettingsXcpV10MbRtu
            {
                /// <summary>
                /// Communication port name such as /dev/tty0.
                /// </summary>
                public String portName;

                /// <summary>
                /// Communication speed in bits/sec.
                /// </summary>
                public UInt32 baudrate;

                /// <summary>
                /// Parity (0 for none, 1 for odd, 2 for even).
                /// </summary>
                public Byte parity;

                /// <summary>
                /// Stopbits (1 for one, 2 for two stopbits).
                /// </summary>
                public Byte stopbits;

                /// <summary>
                /// Destination address (receiver node ID).
                /// </summary>
                public Byte destinationAddr;
            }

            /// <summary>
            /// Unmanaged structure layout of the XCP version 1.0 Modbus RTU transport layer settings.
            /// </summary>
            /// <remarks>
            /// Only used internally when calling the API function inside the DLL.
            /// </remarks>
            [StructLayout(LayoutKind.Sequential)]
            private struct TransportSettingsXcpV10MbRtuUnmanaged
            {
                public IntPtr portName;
                public UInt32 baudrate;
                public Byte parity;
                public Byte stopbits;
                public Byte destinationAddr;
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern void BltSessionInit(UInt32 sessionType, IntPtr sessionSettings, UInt32 transportType, IntPtr transportSettings);

            /// <summary>
            /// Initializes the firmware update session for the XCP v1.0 communication
            /// protocol and RS232 as the transport layer. This function is typically
            /// called once at the start of the firmware update.
            /// </summary>
            /// <param name="sessionSettings">XCP V1.0 protocol settings</param>
            /// <param name="transportSettings">RS232 transport layer settings</param>
            /// <example>
            /// <code>
            ///  OpenBLT.Lib.Session.SessionSettingsXcpV10 sessionSettings;
            ///  sessionSettings.timeoutT1 = 1000;
            ///  sessionSettings.timeoutT3 = 2000;
            ///  sessionSettings.timeoutT4 = 10000;
            ///  sessionSettings.timeoutT5 = 1000;
            ///  sessionSettings.timeoutT6 = 50;
            ///  sessionSettings.timeoutT7 = 2000;
            ///  sessionSettings.seedKeyFile = "";
            ///  sessionSettings.connectMode = 0;
            ///  
            ///  OpenBLT.Lib.Session.TransportSettingsXcpV10Rs232 transportSettings;
            ///  transportSettings.portName = "COM8";
            ///  transportSettings.baudrate = 57600;
            ///  
            ///  OpenBLT.Lib.Session.Init(sessionSettings, transportSettings);
            /// </code>
            /// </example>
            public static void Init(SessionSettingsXcpV10 sessionSettings, TransportSettingsXcpV10Rs232 transportSettings)
            {
                // Copy the managed session settings to an unmanaged structure.
                SessionSettingsXcpV10Unmanaged sessionSettingsUnmanaged;
                sessionSettingsUnmanaged.timeoutT1 = sessionSettings.timeoutT1;
                sessionSettingsUnmanaged.timeoutT3 = sessionSettings.timeoutT3;
                sessionSettingsUnmanaged.timeoutT4 = sessionSettings.timeoutT4;
                sessionSettingsUnmanaged.timeoutT5 = sessionSettings.timeoutT5;
                sessionSettingsUnmanaged.timeoutT6 = sessionSettings.timeoutT6;
                sessionSettingsUnmanaged.timeoutT7 = sessionSettings.timeoutT7;
                // Convert string to unmanged string.
                sessionSettingsUnmanaged.seedKeyFile = (IntPtr)Marshal.StringToHGlobalAnsi(sessionSettings.seedKeyFile);
                sessionSettingsUnmanaged.connectMode = sessionSettings.connectMode;

                // Copy the managed transport settings to an unmanaged structure.
                TransportSettingsXcpV10Rs232Unmanaged transportSettingsUnmanaged;
                // Convert string to unmanaged string.
                transportSettingsUnmanaged.portName = (IntPtr)Marshal.StringToHGlobalAnsi(transportSettings.portName);
                transportSettingsUnmanaged.baudrate = transportSettings.baudrate;

                // The structures are now formatted to be converted to unmanaged memory. Start by allocating
                // memory on the heap for this.
                IntPtr sessionSettingsUnmanagedPtr = Marshal.AllocHGlobal(Marshal.SizeOf(sessionSettingsUnmanaged));
                IntPtr transportSettingsUnmanagedPtr = Marshal.AllocHGlobal(Marshal.SizeOf(transportSettingsUnmanaged));

                // Assert the heap allocations.
                Debug.Assert(sessionSettingsUnmanaged.seedKeyFile != IntPtr.Zero);
                Debug.Assert(transportSettingsUnmanaged.portName != IntPtr.Zero);
                Debug.Assert(sessionSettingsUnmanagedPtr != IntPtr.Zero);
                Debug.Assert(transportSettingsUnmanagedPtr != IntPtr.Zero);

                // Only continue if all the heap allocations were successful.
                if ((sessionSettingsUnmanaged.seedKeyFile != IntPtr.Zero) &&
                     (transportSettingsUnmanaged.portName != IntPtr.Zero) &&
                     (sessionSettingsUnmanagedPtr != IntPtr.Zero) &&
                     (transportSettingsUnmanagedPtr != IntPtr.Zero))
                {
                    // Copy the structures to unmanaged memory.
                    Marshal.StructureToPtr(sessionSettingsUnmanaged, sessionSettingsUnmanagedPtr, false);
                    Marshal.StructureToPtr(transportSettingsUnmanaged, transportSettingsUnmanagedPtr, false);

                    // Call the API function inside the DLL.
                    BltSessionInit(SESSION_XCP_V10, sessionSettingsUnmanagedPtr, TRANSPORT_XCP_V10_RS232, transportSettingsUnmanagedPtr);

                    // Free memory allocated on the heap.
                    Marshal.FreeHGlobal(transportSettingsUnmanagedPtr);
                    Marshal.FreeHGlobal(sessionSettingsUnmanagedPtr);
                    Marshal.FreeHGlobal(transportSettingsUnmanaged.portName);
                    Marshal.FreeHGlobal(sessionSettingsUnmanaged.seedKeyFile);
                }
            }

            /// <summary>
            /// Initializes the firmware update session for the XCP v1.0 communication
            /// protocol and CAN as the transport layer. This function is typically
            /// called once at the start of the firmware update.
            /// </summary>
            /// <param name="sessionSettings">XCP V1.0 protocol settings</param>
            /// <param name="transportSettings">CAN transport layer settings</param>
            /// <example>
            /// <code>
            ///  OpenBLT.Lib.Session.SessionSettingsXcpV10 sessionSettings;
            ///  sessionSettings.timeoutT1 = 1000;
            ///  sessionSettings.timeoutT3 = 2000;
            ///  sessionSettings.timeoutT4 = 10000;
            ///  sessionSettings.timeoutT5 = 1000;
            ///  sessionSettings.timeoutT6 = 50;
            ///  sessionSettings.timeoutT7 = 2000;
            ///  sessionSettings.seedKeyFile = "";
            ///  sessionSettings.connectMode = 0;
            ///  
            ///  OpenBLT.Lib.Session.TransportSettingsXcpV10Can transportSettings;
            ///  transportSettings.deviceName = "peak_pcanusb";
            ///  transportSettings.deviceChannel = 0;
            ///  transportSettings.baudrate = 500000;
            ///  transportSettings.transmitId = 0x667;
            ///  transportSettings.receiveId = 0x7E1;
            ///  transportSettings.useExtended = false;
            ///  
            ///  OpenBLT.Lib.Session.Init(sessionSettings, transportSettings);
            /// </code>
            /// </example>
            public static void Init(SessionSettingsXcpV10 sessionSettings, TransportSettingsXcpV10Can transportSettings)
            {
                // Copy the managed session settings to an unmanaged structure.
                SessionSettingsXcpV10Unmanaged sessionSettingsUnmanaged;
                sessionSettingsUnmanaged.timeoutT1 = sessionSettings.timeoutT1;
                sessionSettingsUnmanaged.timeoutT3 = sessionSettings.timeoutT3;
                sessionSettingsUnmanaged.timeoutT4 = sessionSettings.timeoutT4;
                sessionSettingsUnmanaged.timeoutT5 = sessionSettings.timeoutT5;
                sessionSettingsUnmanaged.timeoutT6 = sessionSettings.timeoutT6;
                sessionSettingsUnmanaged.timeoutT7 = sessionSettings.timeoutT7;
                // Convert string to unmanged string.
                sessionSettingsUnmanaged.seedKeyFile = (IntPtr)Marshal.StringToHGlobalAnsi(sessionSettings.seedKeyFile);
                sessionSettingsUnmanaged.connectMode = sessionSettings.connectMode;

                // Copy the managed transport settings to an unmanaged structure.
                TransportSettingsXcpV10CanUnmanaged transportSettingsUnmanaged;
                // Convert string to unmanaged string.
                transportSettingsUnmanaged.deviceName = (IntPtr)Marshal.StringToHGlobalAnsi(transportSettings.deviceName);
                transportSettingsUnmanaged.deviceChannel = transportSettings.deviceChannel;
                transportSettingsUnmanaged.baudrate = transportSettings.baudrate;
                transportSettingsUnmanaged.transmitId = transportSettings.transmitId;
                transportSettingsUnmanaged.receiveId = transportSettings.receiveId;
                transportSettingsUnmanaged.useExtended = 0;
                if (transportSettings.useExtended)
                {
                    transportSettingsUnmanaged.useExtended = 1;
                }

                // The structures are now formatted to be converted to unmanaged memory. Start by allocating
                // memory on the heap for this.
                IntPtr sessionSettingsUnmanagedPtr = Marshal.AllocHGlobal(Marshal.SizeOf(sessionSettingsUnmanaged));
                IntPtr transportSettingsUnmanagedPtr = Marshal.AllocHGlobal(Marshal.SizeOf(transportSettingsUnmanaged));

                // Assert the heap allocations.
                Debug.Assert(sessionSettingsUnmanaged.seedKeyFile != IntPtr.Zero);
                Debug.Assert(transportSettingsUnmanaged.deviceName != IntPtr.Zero);
                Debug.Assert(sessionSettingsUnmanagedPtr != IntPtr.Zero);
                Debug.Assert(transportSettingsUnmanagedPtr != IntPtr.Zero);

                // Only continue if all the heap allocations were successful.
                if ((sessionSettingsUnmanaged.seedKeyFile != IntPtr.Zero) &&
                     (transportSettingsUnmanaged.deviceName != IntPtr.Zero) &&
                     (sessionSettingsUnmanagedPtr != IntPtr.Zero) &&
                     (transportSettingsUnmanagedPtr != IntPtr.Zero))
                {
                    // Copy the structures to unmanaged memory.
                    Marshal.StructureToPtr(sessionSettingsUnmanaged, sessionSettingsUnmanagedPtr, false);
                    Marshal.StructureToPtr(transportSettingsUnmanaged, transportSettingsUnmanagedPtr, false);

                    // Call the API function inside the DLL.
                    BltSessionInit(SESSION_XCP_V10, sessionSettingsUnmanagedPtr, TRANSPORT_XCP_V10_CAN, transportSettingsUnmanagedPtr);

                    // Free memory allocated on the heap.
                    Marshal.FreeHGlobal(transportSettingsUnmanagedPtr);
                    Marshal.FreeHGlobal(sessionSettingsUnmanagedPtr);
                    Marshal.FreeHGlobal(transportSettingsUnmanaged.deviceName);
                    Marshal.FreeHGlobal(sessionSettingsUnmanaged.seedKeyFile);
                }
            }

            /// <summary>
            /// Initializes the firmware update session for the XCP v1.0 communication
            /// protocol and USB as the transport layer. This function is typically
            /// called once at the start of the firmware update.
            /// </summary>
            /// <remarks>
            /// Note that the USB transport layer does not need any configuration
            /// settings. Therefore, this function only needs the session
            /// settings as a parameter.
            /// </remarks>
            /// <param name="sessionSettings">XCP V1.0 protocol settings</param>
            /// <example>
            /// <code>
            ///  OpenBLT.Lib.Session.SessionSettingsXcpV10 sessionSettings;
            ///  sessionSettings.timeoutT1 = 1000;
            ///  sessionSettings.timeoutT3 = 2000;
            ///  sessionSettings.timeoutT4 = 10000;
            ///  sessionSettings.timeoutT5 = 1000;
            ///  sessionSettings.timeoutT6 = 50;
            ///  sessionSettings.timeoutT7 = 2000;
            ///  sessionSettings.seedKeyFile = "";
            ///  sessionSettings.connectMode = 0;
            ///  
            ///  OpenBLT.Lib.Session.Init(sessionSettings);
            /// </code>
            /// </example>
            public static void Init(SessionSettingsXcpV10 sessionSettings)
            {
                // Copy the managed session settings to an unmanaged structure.
                SessionSettingsXcpV10Unmanaged sessionSettingsUnmanaged;
                sessionSettingsUnmanaged.timeoutT1 = sessionSettings.timeoutT1;
                sessionSettingsUnmanaged.timeoutT3 = sessionSettings.timeoutT3;
                sessionSettingsUnmanaged.timeoutT4 = sessionSettings.timeoutT4;
                sessionSettingsUnmanaged.timeoutT5 = sessionSettings.timeoutT5;
                sessionSettingsUnmanaged.timeoutT6 = sessionSettings.timeoutT6;
                sessionSettingsUnmanaged.timeoutT7 = sessionSettings.timeoutT7;
                // Convert string to unmanged string.
                sessionSettingsUnmanaged.seedKeyFile = (IntPtr)Marshal.StringToHGlobalAnsi(sessionSettings.seedKeyFile);
                sessionSettingsUnmanaged.connectMode = sessionSettings.connectMode;

                // Note that the USB transport layer does not require any settings. The settings structure is
                // now formatted to be converted to unmanaged memory. Start by allocating memory on the heap for this.
                IntPtr sessionSettingsUnmanagedPtr = Marshal.AllocHGlobal(Marshal.SizeOf(sessionSettingsUnmanaged));

                // Assert the heap allocations.
                Debug.Assert(sessionSettingsUnmanaged.seedKeyFile != IntPtr.Zero);
                Debug.Assert(sessionSettingsUnmanagedPtr != IntPtr.Zero);

                // Only continue if all the heap allocations were successful.
                if ((sessionSettingsUnmanaged.seedKeyFile != IntPtr.Zero) &&
                     (sessionSettingsUnmanagedPtr != IntPtr.Zero))
                {
                    // Copy the structure to unmanaged memory.
                    Marshal.StructureToPtr(sessionSettingsUnmanaged, sessionSettingsUnmanagedPtr, false);

                    // Call the API function inside the DLL.
                    BltSessionInit(SESSION_XCP_V10, sessionSettingsUnmanagedPtr, TRANSPORT_XCP_V10_USB, IntPtr.Zero);

                    // Free memory allocated on the heap.
                    Marshal.FreeHGlobal(sessionSettingsUnmanagedPtr);
                    Marshal.FreeHGlobal(sessionSettingsUnmanaged.seedKeyFile);
                }
            }

            /// <summary>
            /// Initializes the firmware update session for the XCP v1.0 communication
            /// protocol and TCP/IP as the transport layer. This function is typically
            /// called once at the start of the firmware update.
            /// </summary>
            /// <param name="sessionSettings">XCP V1.0 protocol settings</param>
            /// <param name="transportSettings">TCP/IP transport layer settings</param>
            /// <example>
            /// <code>
            ///  OpenBLT.Lib.Session.SessionSettingsXcpV10 sessionSettings;
            ///  sessionSettings.timeoutT1 = 1000;
            ///  sessionSettings.timeoutT3 = 2000;
            ///  sessionSettings.timeoutT4 = 10000;
            ///  sessionSettings.timeoutT5 = 1000;
            ///  sessionSettings.timeoutT6 = 50;
            ///  sessionSettings.timeoutT7 = 2000;
            ///  sessionSettings.seedKeyFile = "";
            ///  sessionSettings.connectMode = 0;
            ///  
            ///  OpenBLT.Lib.Session.TransportSettingsXcpV10Net transportSettings;
            ///  transportSettings.address = "192.168.178.30";
            ///  transportSettings.port = 1000;
            ///  
            ///  OpenBLT.Lib.Session.Init(sessionSettings, transportSettings);
            /// </code>
            /// </example>
            public static void Init(SessionSettingsXcpV10 sessionSettings, TransportSettingsXcpV10Net transportSettings)
            {
                // Copy the managed session settings to an unmanaged structure.
                SessionSettingsXcpV10Unmanaged sessionSettingsUnmanaged;
                sessionSettingsUnmanaged.timeoutT1 = sessionSettings.timeoutT1;
                sessionSettingsUnmanaged.timeoutT3 = sessionSettings.timeoutT3;
                sessionSettingsUnmanaged.timeoutT4 = sessionSettings.timeoutT4;
                sessionSettingsUnmanaged.timeoutT5 = sessionSettings.timeoutT5;
                sessionSettingsUnmanaged.timeoutT6 = sessionSettings.timeoutT6;
                sessionSettingsUnmanaged.timeoutT7 = sessionSettings.timeoutT7;
                // Convert string to unmanged string.
                sessionSettingsUnmanaged.seedKeyFile = (IntPtr)Marshal.StringToHGlobalAnsi(sessionSettings.seedKeyFile);
                sessionSettingsUnmanaged.connectMode = sessionSettings.connectMode;

                // Copy the managed transport settings to an unmanaged structure.
                TransportSettingsXcpV10NetUnmanaged transportSettingsUnmanaged;
                // Convert string to unmanaged string.
                transportSettingsUnmanaged.address = (IntPtr)Marshal.StringToHGlobalAnsi(transportSettings.address);
                transportSettingsUnmanaged.port = transportSettings.port;

                // The structures are now formatted to be converted to unmanaged memory. Start by allocating
                // memory on the heap for this.
                IntPtr sessionSettingsUnmanagedPtr = Marshal.AllocHGlobal(Marshal.SizeOf(sessionSettingsUnmanaged));
                IntPtr transportSettingsUnmanagedPtr = Marshal.AllocHGlobal(Marshal.SizeOf(transportSettingsUnmanaged));

                // Assert the heap allocations.
                Debug.Assert(sessionSettingsUnmanaged.seedKeyFile != IntPtr.Zero);
                Debug.Assert(transportSettingsUnmanaged.address != IntPtr.Zero);
                Debug.Assert(sessionSettingsUnmanagedPtr != IntPtr.Zero);
                Debug.Assert(transportSettingsUnmanagedPtr != IntPtr.Zero);

                // Only continue if all the heap allocations were successful.
                if ((sessionSettingsUnmanaged.seedKeyFile != IntPtr.Zero) &&
                     (transportSettingsUnmanaged.address != IntPtr.Zero) &&
                     (sessionSettingsUnmanagedPtr != IntPtr.Zero) &&
                     (transportSettingsUnmanagedPtr != IntPtr.Zero))
                {
                    // Copy the structures to unmanaged memory.
                    Marshal.StructureToPtr(sessionSettingsUnmanaged, sessionSettingsUnmanagedPtr, false);
                    Marshal.StructureToPtr(transportSettingsUnmanaged, transportSettingsUnmanagedPtr, false);

                    // Call the API function inside the DLL.
                    BltSessionInit(SESSION_XCP_V10, sessionSettingsUnmanagedPtr, TRANSPORT_XCP_V10_NET, transportSettingsUnmanagedPtr);

                    // Free memory allocated on the heap.
                    Marshal.FreeHGlobal(transportSettingsUnmanagedPtr);
                    Marshal.FreeHGlobal(sessionSettingsUnmanagedPtr);
                    Marshal.FreeHGlobal(transportSettingsUnmanaged.address);
                    Marshal.FreeHGlobal(sessionSettingsUnmanaged.seedKeyFile);
                }
            }
            
            /// <summary>
            /// Initializes the firmware update session for the XCP v1.0 communication
            /// protocol and Modbus RTU as the transport layer. This function is typically
            /// called once at the start of the firmware update.
            /// </summary>
            /// <param name="sessionSettings">XCP V1.0 protocol settings</param>
            /// <param name="transportSettings">Modbus RTU transport layer settings</param>
            /// <example>
            /// <code>
            ///  OpenBLT.Lib.Session.SessionSettingsXcpV10 sessionSettings;
            ///  sessionSettings.timeoutT1 = 1000;
            ///  sessionSettings.timeoutT3 = 2000;
            ///  sessionSettings.timeoutT4 = 10000;
            ///  sessionSettings.timeoutT5 = 1000;
            ///  sessionSettings.timeoutT6 = 50;
            ///  sessionSettings.timeoutT7 = 2000;
            ///  sessionSettings.seedKeyFile = "";
            ///  sessionSettings.connectMode = 0;
            ///  
            ///  OpenBLT.Lib.Session.TransportSettingsXcpV10MbRtu transportSettings;
            ///  transportSettings.portName = "COM8";
            ///  transportSettings.baudrate = 57600;
            ///  transportSettings.parity = 2;
            ///  transportSettings.stopbits = 1;
            ///  transportSettings.destinationAddr = 1;
            ///  
            ///  OpenBLT.Lib.Session.Init(sessionSettings, transportSettings);
            /// </code>
            /// </example>
            public static void Init(SessionSettingsXcpV10 sessionSettings, TransportSettingsXcpV10MbRtu transportSettings)
            {
                // Copy the managed session settings to an unmanaged structure.
                SessionSettingsXcpV10Unmanaged sessionSettingsUnmanaged;
                sessionSettingsUnmanaged.timeoutT1 = sessionSettings.timeoutT1;
                sessionSettingsUnmanaged.timeoutT3 = sessionSettings.timeoutT3;
                sessionSettingsUnmanaged.timeoutT4 = sessionSettings.timeoutT4;
                sessionSettingsUnmanaged.timeoutT5 = sessionSettings.timeoutT5;
                sessionSettingsUnmanaged.timeoutT6 = sessionSettings.timeoutT6;
                sessionSettingsUnmanaged.timeoutT7 = sessionSettings.timeoutT7;
                // Convert string to unmanged string.
                sessionSettingsUnmanaged.seedKeyFile = (IntPtr)Marshal.StringToHGlobalAnsi(sessionSettings.seedKeyFile);
                sessionSettingsUnmanaged.connectMode = sessionSettings.connectMode;

                // Copy the managed transport settings to an unmanaged structure.
                TransportSettingsXcpV10MbRtuUnmanaged transportSettingsUnmanaged;
                // Convert string to unmanaged string.
                transportSettingsUnmanaged.portName = (IntPtr)Marshal.StringToHGlobalAnsi(transportSettings.portName);
                transportSettingsUnmanaged.baudrate = transportSettings.baudrate;
                transportSettingsUnmanaged.parity = transportSettings.parity;
                transportSettingsUnmanaged.stopbits = transportSettings.stopbits;
                transportSettingsUnmanaged.destinationAddr = transportSettings.destinationAddr;

                // The structures are now formatted to be converted to unmanaged memory. Start by allocating
                // memory on the heap for this.
                IntPtr sessionSettingsUnmanagedPtr = Marshal.AllocHGlobal(Marshal.SizeOf(sessionSettingsUnmanaged));
                IntPtr transportSettingsUnmanagedPtr = Marshal.AllocHGlobal(Marshal.SizeOf(transportSettingsUnmanaged));

                // Assert the heap allocations.
                Debug.Assert(sessionSettingsUnmanaged.seedKeyFile != IntPtr.Zero);
                Debug.Assert(transportSettingsUnmanaged.portName != IntPtr.Zero);
                Debug.Assert(sessionSettingsUnmanagedPtr != IntPtr.Zero);
                Debug.Assert(transportSettingsUnmanagedPtr != IntPtr.Zero);

                // Only continue if all the heap allocations were successful.
                if ((sessionSettingsUnmanaged.seedKeyFile != IntPtr.Zero) &&
                     (transportSettingsUnmanaged.portName != IntPtr.Zero) &&
                     (sessionSettingsUnmanagedPtr != IntPtr.Zero) &&
                     (transportSettingsUnmanagedPtr != IntPtr.Zero))
                {
                    // Copy the structures to unmanaged memory.
                    Marshal.StructureToPtr(sessionSettingsUnmanaged, sessionSettingsUnmanagedPtr, false);
                    Marshal.StructureToPtr(transportSettingsUnmanaged, transportSettingsUnmanagedPtr, false);

                    // Call the API function inside the DLL.
                    BltSessionInit(SESSION_XCP_V10, sessionSettingsUnmanagedPtr, TRANSPORT_XCP_V10_MBRTU, transportSettingsUnmanagedPtr);

                    // Free memory allocated on the heap.
                    Marshal.FreeHGlobal(transportSettingsUnmanagedPtr);
                    Marshal.FreeHGlobal(sessionSettingsUnmanagedPtr);
                    Marshal.FreeHGlobal(transportSettingsUnmanaged.portName);
                    Marshal.FreeHGlobal(sessionSettingsUnmanaged.seedKeyFile);
                }
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern void BltSessionTerminate();

            /// <summary>
            /// Terminates the firmware update session. This function is typically called
            /// once at the end of the firmware update.
            /// </summary>
            /// <example>
            /// <code>
            /// OpenBLT.Lib.Session.Terminate();
            /// </code>
            /// </example>
            public static void Terminate()
            {
                BltSessionTerminate();
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern UInt32 BltSessionStart();

            /// <summary>
            /// Terminates the firmware update session. This function is typically called
            /// once at the end of the firmware update.
            /// </summary>
            /// <returns>RESULT_OK if successful, RESULT_ERROR_xxx otherwise.</returns>
            /// <example>
            /// <code>
            /// if (OpenBLT.Lib.Session.Start() != OpenBLT.Lib.RESULT_OK)
            /// {
            ///     Console.WriteLine("Could not connect to the target.");
            /// }
            /// </code>
            /// </example>
            public static UInt32 Start()
            {
                return BltSessionStart();
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern void BltSessionStop();

            /// <summary>
            /// Terminates the firmware update session. This function is typically called
            /// once at the end of the firmware update.
            /// </summary>
            /// <example>
            /// <code>
            /// OpenBLT.Lib.Session.Stop();
            /// </code>
            /// </example>
            public static void Stop()
            {
                BltSessionStop();
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern UInt32 BltSessionClearMemory(UInt32 address, UInt32 len);

            /// <summary>
            /// Requests the target to erase the specified range of memory on the target.
            /// </summary>
            /// <remarks>
            /// Note that the target automatically aligns this to the erasable memory
            /// block sizes. This typically results in more memory being erased than the
            /// range that was specified here. Refer to the target implementation for
            /// details.
            /// </remarks>
            /// <param name="address">The starting memory address for the erase operation.</param>
            /// <param name="len">The total number of bytes to erase from memory.</param>
            /// <returns>RESULT_OK if successful, RESULT_ERROR_xxx otherwise.</returns>
            /// <example>
            /// <code>
            /// if (OpenBLT.Lib.Session.ClearMemory(0x08004000, 8196) != OpenBLT.Lib.RESULT_OK)
            /// {
            ///     Console.WriteLine("Could not erase memory.");
            /// }
            /// </code>
            /// </example>
            public static UInt32 ClearMemory(UInt32 address, Int32 len)
            {
                // Note that parameter length was made 32-bit signed, because you typically erase the
                // length of a segment. When obtaining the segment info with GetSegment(), you would
                // then pass the length of the segment's data-array into this function, which is 32-bit
                // signed.
                return BltSessionClearMemory(address, (UInt32)len);
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern UInt32 BltSessionWriteData(UInt32 address, UInt32 len, IntPtr data);

            /// <summary>
            /// Requests the target to program the specified data to memory.
            /// </summary>
            /// <remarks>
            /// Note that it is the responsibility of the application to make sure the
            /// memory range was erased beforehand. The length of the data array
            /// determines how many bytes are programmed.
            /// </remarks>
            /// <param name="address">The starting memory address for the write operation.</param>
            /// <param name="data">Byte array with data to write.</param>
            /// <returns>RESULT_OK if successful, RESULT_ERROR_xxx otherwise.</returns>
            /// <example>
            /// <code>
            /// byte[] firmwareDataAt08006000 = new byte[]
            /// {
            ///     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            ///     0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            ///     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            ///     0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
            /// };
            /// byte[] flashData = new byte[8];
            /// 
            /// Array.Copy(firmwareDataAt08006000, 24, flashData, 0, 8);
            /// 
            /// if (OpenBLT.Lib.Session.WriteData(0x08006000 + 24, flashData) != OpenBLT.Lib.RESULT_OK)
            /// {
            ///    Console.WriteLine("Could not program 8 bytes at 0x08006018 in memory.");
            /// }
            /// </code>
            /// </example>
            public static UInt32 WriteData(UInt32 address, byte[] data)
            {
                UInt32 result = RESULT_ERROR_GENERIC;

                // Determine data size.
                Int32 dataSize = Marshal.SizeOf(data[0]) * data.Length;

                // Allocate memory on the heap for storing the data in unmanaged memory.
                IntPtr dataPtr = Marshal.AllocHGlobal(dataSize);

                // Only continue if the allocation was successful.
                if (dataPtr != IntPtr.Zero)
                {
                    // Copy the data to unmanaged memory.
                    Marshal.Copy(data, 0, dataPtr, data.Length);
                    // Write the data to memory on the target.
                    result = BltSessionWriteData(address, (UInt32)data.Length, dataPtr);
                    // Free the unmanaged memory.
                    Marshal.FreeHGlobal(dataPtr);
                }

                // Give the result back to the caller.
                return result;
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern UInt32 BltSessionReadData(UInt32 address, UInt32 len, IntPtr data);

            /// <summary>
            /// Requests the target to upload the specified range from memory and store its
            /// contents in the specified data buffer.
            /// </summary>
            /// <remarks>
            /// Note that the length of the data array determines how many bytes are read.
            /// </remarks>
            /// <param name="address">The starting memory address for the read operation.</param>
            /// <param name="data">Byte array where the uploaded data should be stored.</param>
            /// <returns>RESULT_OK if successful, RESULT_ERROR_xxx otherwise.</returns>
            /// <example>
            /// <code>
            /// 
            /// </code>
            /// </example>
            public static UInt32 ReadData(UInt32 address, byte[] data)
            {
                UInt32 result = RESULT_ERROR_GENERIC;

                // Determine data size.
                Int32 dataSize = Marshal.SizeOf(data[0]) * data.Length;

                // Allocate memory on the heap for storing the data in unmanaged memory.
                IntPtr dataPtr = Marshal.AllocHGlobal(dataSize);

                // Only continue if the allocation was successful.
                if (dataPtr != IntPtr.Zero)
                {
                    // Read the data to memory on the target.
                    result = BltSessionReadData(address, (UInt32)data.Length, dataPtr);

                    // Copy read data in unmanaged memory back to the data array.
                    Marshal.Copy(dataPtr, data, 0, dataSize);

                    // Free the unmanaged memory.
                    Marshal.FreeHGlobal(dataPtr);
                }

                // Give the result back to the caller.
                return result;
            }
        }

        /// <summary>
        /// Wrapper for the firmware module of LibOpenBLT.
        /// </summary>
        public static class Firmware
        {
            /// <summary>
            /// The S-record parser enables writing and reading firmware data to and from
            /// file formatted as Motorola S-record. This is a widely known file format
            /// and pretty much all microcontroller compiler toolchains include
            /// functionality to output or convert the firmware's data as an S-record.
            /// </summary>
            public const UInt32 FIRMWARE_PARSER_SRECORD = 0;

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern void BltFirmwareInit(UInt32 parserType);

            /// <summary>
            /// Initializes the firmware data module for a specified firmware file parser.
            /// </summary>
            /// <param name="parserType">
            /// The firmware file parser to use in this module. It should be a
            /// PARSER_xxx value.
            /// </param>
            /// <example>
            /// <code>
            /// OpenBLT.Lib.Firmware.Init(OpenBLT.Lib.Firmware.FIRMWARE_PARSER_SRECORD);
            /// </code>
            /// </example>
            public static void Init(UInt32 parserType)
            {
                BltFirmwareInit(parserType);
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern void BltFirmwareTerminate();

            /// <summary>
            /// Terminates the firmware data module. Typically called at the end of the
            /// program when the firmware data module is no longer needed.
            /// </summary>
            /// <example>
            /// <code>
            /// OpenBLT.Lib.Firmware.Terminate();
            /// </code>
            /// </example>
            public static void Terminate()
            {
                BltFirmwareTerminate();
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern UInt32 BltFirmwareLoadFromFile(String firmwareFile, UInt32 addressOffset);

            /// <summary>
            /// Loads firmware data from the specified file using the firmware file parser
            /// that was specified during the initialization of this module.
            /// </summary>
            /// <param name="firmwareFile">Filename of the firmware file to load.</param>
            /// <param name="addressOffset">
            /// Optional memory address offset to add when loading the firmware data from
            /// the file.This is typically only useful when loading firmware data from a
            /// binary formatted firmware file.
            /// </param>
            /// <returns>RESULT_OK if successful, RESULT_ERROR_xxx otherwise.</returns>
            /// <example>
            /// <code>
            /// if (OpenBLT.Lib.Firmware.LoadFromFile("demoprog.srec", 0) != OpenBLT.Lib.RESULT_OK)
            /// {
            ///     Console.WriteLine("Could not load the firmware file.");
            /// }
            /// </code>
            /// </example>
            public static UInt32 LoadFromFile(String firmwareFile, UInt32 addressOffset)
            {
                return BltFirmwareLoadFromFile(firmwareFile, addressOffset);
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern UInt32 BltFirmwareSaveToFile(String firmwareFile);

            /// <summary>
            /// Writes firmware data to the specified file using the firmware file parser
            /// that was specified during the initialization of this module.
            /// </summary>
            /// <param name="firmwareFile">Filename of the firmware file to write to.</param>
            /// <returns>RESULT_OK if successful, RESULT_ERROR_xxx otherwise.</returns>
            /// <example>
            /// <code>
            /// if (OpenBLT.Lib.Firmware.SaveToFile("myfirmware.srec") != OpenBLT.Lib.RESULT_OK)
            /// {
            ///     Console.WriteLine("Could not save the firmware file.");
            /// }
            /// </code>
            /// </example>
            public static UInt32 SaveToFile(String firmwareFile)
            {
                return BltFirmwareSaveToFile(firmwareFile);
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern UInt32 BltFirmwareGetSegmentCount();

            /// <summary>
            /// Obtains the number of firmware data segments that are currently present
            /// in the firmware data module.
            /// </summary>
            /// <returns>The total number of segments.</returns>
            /// <example>
            /// <code>
            /// UInt32 segmentCount = OpenBLT.Lib.Firmware.GetSegmentCount();
            /// Console.WriteLine("Number of segments: {0}", segmentCount);
            /// </code>
            /// </example>
            public static UInt32 GetSegmentCount()
            {
                return BltFirmwareGetSegmentCount();
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern IntPtr BltFirmwareGetSegment(UInt32 idx, out UInt32 address, out UInt32 len);

            /// <summary>
            /// Obtains the contents of the firmware data segment that was specified by
            /// the index parameter.
            /// </summary>
            /// <param name="idx">
            /// The segment index. It should be a value greater or equal to zero and
            /// smaller than the value returned by GetSegmentCount().
            /// </param>
            /// <returns>Tuple with segment data and base address.</returns>
            /// <example>
            /// <code>
            /// var segment = OpenBLT.Lib.Firmware.GetSegment(0);
            /// Console.WriteLine("Base address: {0:X}h", segment.address);
            /// Console.WriteLine("Length: {0}", segment.data.Length);
            /// Console.WriteLine("First data: {0:X2}h {1:X2}h {2:X2}h", segment.data[0], segment.data[1], segment.data[2]);
            /// </code>
            /// </example>
            public static (byte[] data, UInt32 address) GetSegment(UInt32 idx)
            {
                byte[] data = new byte[0];
                UInt32 address = 0;
                UInt32 len;
                IntPtr dataPtr;

                // Obtain the segment info.
                dataPtr = BltFirmwareGetSegment(idx, out address, out len);

                // Note that the DLL function returns a 32-bit unsigned. It is subsequently
                // used in functions where it is assumed to be 32-bit signed. A segments will
                // never be larger than the 32-bit signed maximum value, so that will work
                // fine. Still good to double-check though.
                Debug.Assert(len <= Int32.MaxValue);

                // Only continue if the returned data pointer is not a NULL pointer and when
                // the segment has a non-zero length.
                if ((dataPtr != IntPtr.Zero) && (len > 0) && (len <= Int32.MaxValue))
                {
                    // Resize the array such that it can store all bytes from the segment.
                    Array.Resize(ref data, (Int32)len);
                    // Copy the segment data to the resulting data array.
                    Marshal.Copy(dataPtr, data, 0, (Int32)len);
                }

                // Give the result back to the caller.
                return (data, address);
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern UInt32 BltFirmwareAddData(UInt32 address, UInt32 len, IntPtr data);

            /// <summary>
            /// Adds data to the segments that are currently present in the firmware data
            /// module.If the data overlaps with already existing data, the existing data
            /// gets overwritten.The size of a segment is automatically adjusted or a new
            /// segment gets created, if necessary.
            /// </summary>
            /// <param name="address">Base address of the firmware data.</param>
            /// <param name="data">Array with data bytes that should be added.</param>
            /// <returns>RESULT_OK if successful, RESULT_ERROR_xxx otherwise.</returns>
            /// <example>
            /// <code>
            /// byte[] newData = new byte[] { 0, 1, 2, 3, 4, 5, 6, 7 };
            /// OpenBLT.Lib.Firmware.AddData(0x08100000, newData);
            /// </code>
            /// </example>
            public static UInt32 AddData(UInt32 address, byte[] data)
            {
                UInt32 result = RESULT_ERROR_GENERIC;

                // Determine data size.
                Int32 dataSize = Marshal.SizeOf(data[0]) * data.Length;

                // Allocate memory on the heap for storing the data in unmanaged memory.
                IntPtr dataPtr = Marshal.AllocHGlobal(dataSize);

                // Only continue if the allocation was successful.
                if (dataPtr != IntPtr.Zero)
                {
                    // Copy the data to unmanaged memory.
                    Marshal.Copy(data, 0, dataPtr, data.Length);
                    // Add the data to the already loaded firmware data.
                    result = BltFirmwareAddData(address, (UInt32)data.Length, dataPtr);
                    // Free the unmanaged memory.
                    Marshal.FreeHGlobal(dataPtr);
                }

                // Give the result back to the caller.
                return result;
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern UInt32 BltFirmwareRemoveData(UInt32 address, UInt32 len);

            /// <summary>
            /// Removes data from the segments that are currently present in the firmware 
            /// data module.The size of a segment is automatically adjusted or removed, if
            /// necessary.
            /// </summary>
            /// <param name="address">Base address of the firmware data.</param>
            /// <param name="len">Number of bytes to remove.</param>
            /// <returns>RESULT_OK if successful, RESULT_ERROR_xxx otherwise.</returns>
            /// <example>
            /// <code>
            /// OpenBLT.Lib.Firmware.RemoveData(0x08100000, 8);
            /// </code>
            /// </example>
            public static UInt32 RemoveData(UInt32 address, UInt32 len)
            {
                return BltFirmwareRemoveData(address, len);
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            private static extern void BltFirmwareClearData();

            /// <summary>
            /// Clears all data and segments that are currently present in the firmware
            /// data module.
            /// </summary>
            /// <example>
            /// <code>
            /// OpenBLT.Lib.Firmware.ClearData();
            /// </code>
            /// </example>
            public static void ClearData()
            {
                BltFirmwareClearData();
            }
        }

        /// <summary>
        /// Wrapper for the utility module of LibOpenBLT.
        /// </summary>
        public static class Util
        {
            /// <summary>
            /// Wrapper for the CRC utilities of LibOpenBLT.
            /// </summary>
            public static class Crc
            {
                [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
                private static extern UInt16 BltUtilCrc16Calculate(IntPtr data, UInt32 len);

                /// <summary>
                /// Calculates a 16-bit CRC value over the specified data.
                /// </summary>
                /// <param name="data">Array with bytes over which the CRC16 should be calculated.</param>
                /// <returns>The 16-bit CRC value.</returns>
                /// <example>
                /// <code>
                /// byte[] crcData = new byte[] { 0x55, 0xAA, 0x00, 0xFF };
                /// UInt16 crc16 = OpenBLT.Lib.Util.Crc.Calculate16(crcData);
                /// </code>
                /// </example>
                public static UInt16 Calculate16(byte[] data)
                {
                    UInt16 result = 0;

                    // Determine data size.
                    Int32 dataSize = Marshal.SizeOf(data[0]) * data.Length;

                    // Allocate memory on the heap for storing the data in unmanaged memory.
                    IntPtr dataPtr = Marshal.AllocHGlobal(dataSize);

                    // Only continue if the allocation was successful.
                    if (dataPtr != IntPtr.Zero)
                    {
                        // Copy the data to unmanaged memory.
                        Marshal.Copy(data, 0, dataPtr, data.Length);
                        // Calculate the CRC16 over the data.
                        result = BltUtilCrc16Calculate(dataPtr, (UInt32)data.Length);
                        // Free the unmanaged memory.
                        Marshal.FreeHGlobal(dataPtr);
                    }

                    // Give the result back to the caller.
                    return result;
                }

                [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
                private static extern UInt32 BltUtilCrc32Calculate(IntPtr data, UInt32 len);

                /// <summary>
                /// Calculates a 32-bit CRC value over the specified data.
                /// </summary>
                /// <param name="data">Array with bytes over which the CRC32 should be calculated.</param>
                /// <returns>The 32-bit CRC value.</returns>
                /// <example>
                /// <code>
                /// byte[] crcData = new byte[] { 0x55, 0xAA, 0x00, 0xFF };
                /// UInt32 crc32 = OpenBLT.Lib.Util.Crc.Calculate32(crcData);
                /// </code>
                /// </example>
                public static UInt32 Calculate32(byte[] data)
                {
                    UInt32 result = 0;

                    // Determine data size.
                    Int32 dataSize = Marshal.SizeOf(data[0]) * data.Length;

                    // Allocate memory on the heap for storing the data in unmanaged memory.
                    IntPtr dataPtr = Marshal.AllocHGlobal(dataSize);

                    // Only continue if the allocation was successful.
                    if (dataPtr != IntPtr.Zero)
                    {
                        // Copy the data to unmanaged memory.
                        Marshal.Copy(data, 0, dataPtr, data.Length);
                        // Calculate the CRC32 over the data.
                        result = BltUtilCrc32Calculate(dataPtr, (UInt32)data.Length);
                        // Free the unmanaged memory.
                        Marshal.FreeHGlobal(dataPtr);
                    }

                    // Give the result back to the caller.
                    return result;
                }

            }

            /// <summary>
            /// Wrapper for the time utilities of LibOpenBLT.
            /// </summary>
            public static class Time
            {
                [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
                private static extern Int32 BltUtilTimeGetSystemTime();

                /// <summary>
                /// Get the system time in milliseconds.
                /// </summary>
                /// <returns>
                /// Time in milliseconds.
                /// </returns>
                /// <example>
                /// <code>
                /// Console.WriteLine("Current system time: {0}", OpenBLT.Lib.Util.Time.GetSystemTime());
                /// </code>
                /// </example>
                public static Int32 GetSystemTime()
                {
                    return BltUtilTimeGetSystemTime();
                }

                [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
                private static extern void BltUtilTimeDelayMs(UInt16 delay);

                /// <summary>
                /// Performs a delay of the specified amount of milliseconds.
                /// </summary>
                /// <param name="delay">Delay time in milliseconds.</param>
                /// <example>
                /// <code>
                /// OpenBLT.Lib.Util.Time.DelayMs(1000);
                /// </code>
                /// </example>
                public static void DelayMs(UInt16 delay)
                {
                    BltUtilTimeDelayMs(delay);
                }
            }

            /// <summary>
            /// Wrapper for the cryptography utilities of LibOpenBLT.
            /// </summary>
            public static class Crypto
            {
                /// <summary>
                /// Wrapper for the AES256 cryptography utilities of LibOpenBLT.
                /// </summary>
                public static class Aes256
                {
                    [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
                    private static extern UInt32 BltUtilCryptoAes256Encrypt(IntPtr data, UInt32 len, IntPtr key);

                    /// <summary>
                    /// Encrypts the bytes in the specified data-array, using the specified
                    /// 256-bit (32 bytes) key.The results are written back into the same array.
                    /// </summary>
                    /// <remarks>
                    /// The number of bytes in the data-array must be a multiple of 16, as this
                    /// is the AES256 minimal block size.
                    /// </remarks>
                    /// <param name="data">
                    /// Byte array with data to encrypt. The encrypted bytes are stored in the
                    /// same array.
                    /// </param>
                    /// <param name="key">The 256-bit encryption key as a array of 32 bytes.</param>
                    /// <returns>RESULT_OK if successful, RESULT_ERROR_xxx otherwise.</returns>
                    /// <example>
                    /// <code>
                    /// byte[] cryptoKey = new byte[]
                    /// {
                    ///     0x83, 0x23, 0x44, 0x28, 0x47, 0x2B, 0x4B, 0x62,
                    ///     0x50, 0x65, 0x53, 0x68, 0x56, 0x6D, 0x59, 0x71,
                    ///     0x33, 0x74, 0x36, 0x77, 0x39, 0x7A, 0x24, 0x43,
                    ///     0x26, 0x46, 0x29, 0x48, 0x40, 0x4D, 0x69, 0x39
                    /// };
                    /// 
                    /// byte[] originalData = new byte[]
                    /// {
                    ///     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    ///     0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                    ///     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                    ///     0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
                    /// };
                    /// 
                    /// OpenBLT.Lib.Util.Crypto.Aes256.Encrypt(originalData, cryptoKey);
                    /// </code>
                    /// </example>
                    public static UInt32 Encrypt(byte[] data, byte[] key)
                    {
                        UInt32 result = RESULT_ERROR_GENERIC;

                        // Determine data and key size.
                        Int32 dataSize = Marshal.SizeOf(data[0]) * data.Length;
                        Int32 keySize = Marshal.SizeOf(key[0]) * key.Length;

                        // Only continue if the key has at least 32 bytes (256-bit key).
                        if (keySize >= 32)
                        {
                            // Allocate memory on the heap for storing the data and key in unmanaged memory.
                            IntPtr dataPtr = Marshal.AllocHGlobal(dataSize);
                            IntPtr keyPtr = Marshal.AllocHGlobal(keySize);

                            // Only continue if the allocation was successful.
                            if ((dataPtr != IntPtr.Zero) && (keyPtr != IntPtr.Zero))
                            {
                                // Copy the data and key to unmanaged memory.
                                Marshal.Copy(data, 0, dataPtr, data.Length);
                                Marshal.Copy(key, 0, keyPtr, key.Length);

                                // Encrypt the data using the key. Note that the encrypted data overwrites
                                // the data in unmanaged memory.
                                result = BltUtilCryptoAes256Encrypt(dataPtr, (UInt32)dataSize, keyPtr);

                                // Copy encrypted data in unmanaged memory back to the data array.
                                Marshal.Copy(dataPtr, data, 0, dataSize);

                                // Free the unmanaged memory.
                                Marshal.FreeHGlobal(dataPtr);
                                Marshal.FreeHGlobal(keyPtr);
                            }
                        }

                        // Give the result back to the caller.
                        return result;
                    }

                    [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
                    private static extern UInt32 BltUtilCryptoAes256Decrypt(IntPtr data, UInt32 len, IntPtr key);

                    /// <summary>
                    /// Decrypts the bytes in the specified data-array, using the specified
                    /// 256-bit (32 bytes) key.The results are written back into the same array.
                    /// </summary>
                    /// <remarks>
                    /// The number of bytes in the data-array must be a multiple of 16, as this
                    /// is the AES256 minimal block size.
                    /// </remarks>
                    /// <param name="data">
                    /// Byte array with data to decrypt. The decrypted bytes are stored in the
                    /// same array.
                    /// </param>
                    /// <param name="key">The 256-bit decryption key as a array of 32 bytes.</param>
                    /// <returns>RESULT_OK if successful, RESULT_ERROR_xxx otherwise.</returns>
                    /// <example>
                    /// <code>
                    /// byte[] cryptoKey = new byte[]
                    /// {
                    ///     0x83, 0x23, 0x44, 0x28, 0x47, 0x2B, 0x4B, 0x62,
                    ///     0x50, 0x65, 0x53, 0x68, 0x56, 0x6D, 0x59, 0x71,
                    ///     0x33, 0x74, 0x36, 0x77, 0x39, 0x7A, 0x24, 0x43,
                    ///     0x26, 0x46, 0x29, 0x48, 0x40, 0x4D, 0x69, 0x39
                    /// };
                    /// 
                    /// byte[] encryptedData = new byte[]
                    /// {
                    ///     0x43, 0x34, 0xd1, 0x54, 0xa2, 0xf0, 0x48, 0x76,
                    ///     0x0e, 0x30, 0xf8, 0xa0, 0x92, 0x4f, 0xbc, 0xce,
                    ///     0x06, 0x9c, 0x29, 0xf7, 0x24, 0x58, 0x79, 0x4e,
                    ///     0x94, 0xeb, 0xd0, 0x35, 0xfd, 0xae, 0xf9, 0x88
                    /// };
                    /// 
                    /// OpenBLT.Lib.Util.Crypto.Aes256.Decrypt(encryptedData, cryptoKey);
                    /// </code>
                    /// </example>
                    public static UInt32 Decrypt(byte[] data, byte[] key)
                    {
                        UInt32 result = RESULT_ERROR_GENERIC;

                        // Determine data and key size.
                        Int32 dataSize = Marshal.SizeOf(data[0]) * data.Length;
                        Int32 keySize = Marshal.SizeOf(key[0]) * key.Length;

                        // Only continue if the key has at least 32 bytes (256-bit key).
                        if (keySize >= 32)
                        {
                            // Allocate memory on the heap for storing the data and key in unmanaged memory.
                            IntPtr dataPtr = Marshal.AllocHGlobal(dataSize);
                            IntPtr keyPtr = Marshal.AllocHGlobal(keySize);

                            // Only continue if the allocation was successful.
                            if ((dataPtr != IntPtr.Zero) && (keyPtr != IntPtr.Zero))
                            {
                                // Copy the data and key to unmanaged memory.
                                Marshal.Copy(data, 0, dataPtr, data.Length);
                                Marshal.Copy(key, 0, keyPtr, key.Length);

                                // Decrypt the data using the key. Note that the decrypted data overwrites
                                // the data in unmanaged memory.
                                result = BltUtilCryptoAes256Decrypt(dataPtr, (UInt32)dataSize, keyPtr);

                                // Copy Decrypted data in unmanaged memory back to the data array.
                                Marshal.Copy(dataPtr, data, 0, dataSize);

                                // Free the unmanaged memory.
                                Marshal.FreeHGlobal(dataPtr);
                                Marshal.FreeHGlobal(keyPtr);
                            }
                        }

                        // Give the result back to the caller.
                        return result;
                    }
                }
            }
        }
    }
}
