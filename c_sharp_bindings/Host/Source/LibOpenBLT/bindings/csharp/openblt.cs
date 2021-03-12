using System;
using System.Runtime.InteropServices;

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
        /// Wrapper for the version module of LibOpenBLT.
        /// </summary>
        public static class Version
        {
            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            static extern UInt32 BltVersionGetNumber();
            /// <summary>
            /// Obtains the version number of the library as an integer. The number has two 
            /// digits for major-, minor-, and patch-version.Version 1.05.12 would for
            /// example return 10512.
            /// </summary>
            /// <returns>Library version number as an integer.</returns>
            public static UInt32 GetNumber()
            {
                return BltVersionGetNumber();
            }

            [DllImport(LIBNAME, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            public static extern IntPtr BltVersionGetString();
            /// <summary>
            /// Obtains the version number of the library as a null-terminated string. Version
            /// 1.05.12 would for example return "1.05.12".
            /// </summary>
            /// <returns>Library version number as a string.</returns>
            public static String GetString()
            {
                return Marshal.PtrToStringAnsi(BltVersionGetString());
            }
        }
    }
}
