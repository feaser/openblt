"""
Implements python wrappers for the OpenBLT host library (LibOpenBLT) 
functions.
"""
__docformat__ = 'reStructuredText'
# ***************************************************************************************
#  File Name: lib.py
#
# ---------------------------------------------------------------------------------------
#                           C O P Y R I G H T
# ---------------------------------------------------------------------------------------
#    Copyright (c) 2018  by Feaser    http://www.feaser.com    All rights reserved
#
# ---------------------------------------------------------------------------------------
#                             L I C E N S E
# ---------------------------------------------------------------------------------------
#  This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as published by the Free
#  Software Foundation, either version 3 of the License, or (at your option) any later
#  version.
#
#  OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
#  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE. See the GNU General Public License for more details.
#
#  You have received a copy of the GNU General Public License along with OpenBLT. It
#  should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
#
# ***************************************************************************************


# ***************************************************************************************
#  Imports
# ***************************************************************************************
import ctypes
import sys


# ***************************************************************************************
#  Implementation
# ***************************************************************************************
# Determine the file extension of the share library, which is platform dependent.
if "win" in sys.platform:
    sharedLibraryExt = '.dll'
else:
    sharedLibraryExt = '.so'


# Initialize the shared library to just its filename without a path. This assumes that
# the LibOpenBLT shared library file and possible other run-time libraries that it
# depends on are located in the directory from where your program runs or somewhere on
# the system's library path. Refer to the section on the OpenBLT Wiki for an overview of
# these run-time libraries:
#   https://www.feaser.com/openblt/doku.php?id=manual:libopenblt#run-time_libraries.
sharedLibrary = 'libopenblt' + sharedLibraryExt


# Get a handle to the shared library.
sharedLibraryHandle = ctypes.CDLL(sharedLibrary)


# ***************************************************************************************
#  Ctypes function import specifications
# ***************************************************************************************
BltVersionGetNumber = None
if hasattr(sharedLibraryHandle, 'BltVersionGetNumber'):
    BltVersionGetNumber = sharedLibraryHandle.BltVersionGetNumber
    BltVersionGetNumber.argtypes = []
    BltVersionGetNumber.restype = ctypes.c_uint32


BltVersionGetString = None
if hasattr(sharedLibraryHandle, 'BltVersionGetString'):
    BltVersionGetString = sharedLibraryHandle.BltVersionGetString
    BltVersionGetString.argtypes = []
    BltVersionGetString.restype = ctypes.c_char_p


# ***************************************************************************************
#              V E R S I O N   I N F O R M A T I O N
# ***************************************************************************************
def version_get_number():
    """
    Obtains the version number of the library as an integer. The number has two digits
    for major-, minor-, and patch-version. Version 1.05.12 would for example return 
    10512.
    
    :returns: Library version number as an integer.
    :rtype: int
    """
    # Initialize the result.
    result = 0
    # Check if the shared library function could be imported.
    if BltVersionGetNumber is not None:
        # Call the function in the shared library.
        result = BltVersionGetNumber()
    # Give the result back to the caller.
    return result


def version_get_string():
    """
    Obtains the version number of the library as a string. Version 1.05.12 would for 
    example return "1.05.12".

    :returns: Library version number as a string.
    :rtype: string
    """
    # Initialize the result
    result = ''
    # Check if the shared library function could be imported.
    if BltVersionGetString is not None:
        # Call the function in the shared library
        result = BltVersionGetString()
        # Decode the null terminated character string to a UTF-8 string.
        result = result.decode('utf-8')
    # Give the result back to the caller.
    return result


# ********************************* end of lib.py ***************************************
