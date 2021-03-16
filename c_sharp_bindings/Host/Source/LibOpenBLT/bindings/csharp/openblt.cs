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
    /// is 32-bit. For this reason, whatever project uses this wrapper class needs to
    /// be built as a 32-bit application. 
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
            public const UInt32 PARSER_SRECORD = 0;

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
            /// OpenBLT.Lib.Firmware.Init(OpenBLT.Lib.Firmware.PARSER_SRECORD);
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
            /// <param name="address">Output variable where the segment's base address will be written to.</param>
            /// <param name="len">Output variable where the segment's length will be written to.</param>
            /// <returns>Managed byte array with segment data if successful, empty byte array otherwise.</returns>
            /// <example>
            /// <code>
            /// UInt32 segmentAddr;
            /// UInt32 segmentLen;
            /// byte[] segmentData;
            /// 
            /// segmentData = OpenBLT.Lib.Firmware.GetSegment(0, out segmentAddr, out segmentLen);
            /// Console.WriteLine("Base address: {0:X}h", segmentAddr);
            /// Console.WriteLine("Length: {0}", segmentLen);
            /// Console.WriteLine("First data: {0:X2}h {1:X2}h {2:X2}h", segmentData[0], segmentData[1], segmentData[2]);
            /// </code>
            /// </example>
            public static byte[] GetSegment(UInt32 idx, out UInt32 address, out UInt32 len)
            {
                // Initialize the result as an empty byte array.
                byte[] result = new byte[0];

                // Obtain the segment info.
                IntPtr dataPtr = BltFirmwareGetSegment(idx, out address, out len);

                // Note that the DLL function returns a 32-bit unsigned. It is subsequently
                // used in functions where it is assumed to be 32-bit signed. A segments will
                // never be larger than the 32-bit signed maximum value, so that will work
                // fine. Still good to double-check though.
                Debug.Assert(len <= Int32.MaxValue);

                // Only continue if the returned data pointer is not a NULL pointer and when
                // the segment has a non-zero length.
                if ( (dataPtr != IntPtr.Zero) && (len > 0) && (len <= Int32.MaxValue))
                {
                    // Resize the array such that it can store all byte from the segment.
                    Array.Resize(ref result, (Int32)len);
                    // Copy the segment data to the resulting data array.
                    Marshal.Copy(dataPtr, result, 0, (Int32)len);
                }

                // Give the result back to the caller.
                return result;
            }

        }
    }
}
