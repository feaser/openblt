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
#  Function import specifications
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


BltFirmwareInit = None
if hasattr(sharedLibraryHandle, 'BltFirmwareInit'):
    BltFirmwareInit = sharedLibraryHandle.BltFirmwareInit
    BltFirmwareInit.argtypes = [ctypes.c_uint32]
    BltFirmwareInit.restype = None


BltFirmwareTerminate = None
if hasattr(sharedLibraryHandle, 'BltFirmwareTerminate'):
    BltFirmwareTerminate = sharedLibraryHandle.BltFirmwareTerminate
    BltFirmwareTerminate.argtypes = []
    BltFirmwareTerminate.restype = None


BltFirmwareLoadFromFile = None
if hasattr(sharedLibraryHandle, 'BltFirmwareLoadFromFile'):
    BltFirmwareLoadFromFile = sharedLibraryHandle.BltFirmwareLoadFromFile
    BltFirmwareLoadFromFile.argtypes = [ctypes.c_char_p, ctypes.c_uint32]
    BltFirmwareLoadFromFile.restype = ctypes.c_uint32


BltFirmwareSaveToFile = None
if hasattr(sharedLibraryHandle, 'BltFirmwareSaveToFile'):
    BltFirmwareSaveToFile = sharedLibraryHandle.BltFirmwareSaveToFile
    BltFirmwareSaveToFile.argtypes = [ctypes.c_char_p]
    BltFirmwareSaveToFile.restype = ctypes.c_uint32


BltFirmwareGetSegmentCount = None
if hasattr(sharedLibraryHandle, 'BltFirmwareGetSegmentCount'):
    BltFirmwareGetSegmentCount = sharedLibraryHandle.BltFirmwareGetSegmentCount
    BltFirmwareGetSegmentCount.argtypes = []
    BltFirmwareGetSegmentCount.restype = ctypes.c_uint32


BltFirmwareGetSegment = None
if hasattr(sharedLibraryHandle, 'BltFirmwareGetSegment'):
    BltFirmwareGetSegment = sharedLibraryHandle.BltFirmwareGetSegment
    BltFirmwareGetSegment.argtypes = [ctypes.c_uint32, ctypes.POINTER(ctypes.c_uint32),
                                      ctypes.POINTER(ctypes.c_uint32)]
    BltFirmwareGetSegment.restype = ctypes.POINTER(ctypes.c_uint8)


BltFirmwareAddData = None
if hasattr(sharedLibraryHandle, 'BltFirmwareAddData'):
    BltFirmwareAddData = sharedLibraryHandle.BltFirmwareAddData
    BltFirmwareAddData.argtypes = [ctypes.c_uint32, ctypes.c_uint32,
                                   ctypes.POINTER(ctypes.c_uint8)]
    BltFirmwareAddData.restype = ctypes.c_uint32


BltFirmwareRemoveData = None
if hasattr(sharedLibraryHandle, 'BltFirmwareRemoveData'):
    BltFirmwareRemoveData = sharedLibraryHandle.BltFirmwareRemoveData
    BltFirmwareRemoveData.argtypes = [ctypes.c_uint32, ctypes.c_uint32]
    BltFirmwareRemoveData.restype = ctypes.c_uint32


BltFirmwareClearData = None
if hasattr(sharedLibraryHandle, 'BltFirmwareClearData'):
    BltFirmwareClearData = sharedLibraryHandle.BltFirmwareClearData
    BltFirmwareClearData.argtypes = []
    BltFirmwareClearData.restype = None


# ***************************************************************************************
#  Constant declarations
# ***************************************************************************************
BLT_RESULT_OK = 0
BLT_RESULT_ERROR_GENERIC = 1


# ***************************************************************************************
#              V E R S I O N   I N F O R M A T I O N
# ***************************************************************************************
# ***************************************************************************************
#  Functions
# ***************************************************************************************
def version_get_number():
    """
    Obtains the version number of the library as an integer. The number has two digits
    for major-, minor-, and patch-version. Version 1.05.12 would for example return 
    10512.
    
    :returns: Library version number as an integer.
    :rtype: int

    :Example:
    ::
    
        import openblt
        
        print('LibOpenBLT version:', openblt.version_get_number())
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

    :Example:
    ::
    
        import openblt
        
        print('LibOpenBLT version:', openblt.version_get_string())
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


# ***************************************************************************************
#              F I R M W A R E   D A T A
# ***************************************************************************************
# ***************************************************************************************
#  Constant declarations
# ***************************************************************************************
BLT_FIRMWARE_PARSER_SRECORD = 0


# ***************************************************************************************
#  Functions
# ***************************************************************************************
def firmware_init(parser_type=BLT_FIRMWARE_PARSER_SRECORD):
    """
    Initializes the firmware data module for a specified firmware file parser.
    
    :param parser_type: The firmware file parser to use in this module. It should be a
           BLT_FIRMWARE_PARSER_xxx value.

    :Example:
    ::
    
        import openblt
        
        openblt.firmware_init(openblt.BLT_FIRMWARE_PARSER_SRECORD)
    """
    # Check if the shared library function could be imported.
    if BltFirmwareInit is not None:
        # Call the function in the shared library
        BltFirmwareInit(ctypes.c_uint32(parser_type))


def firmware_terminate():
    """
    Terminates the firmware data module. Typically called at the end of the program when 
    the firmware data module is no longer needed.

    :Example:
    ::
    
        import openblt
        
        openblt.firmware_terminate()
    """
    # Check if the shared library function could be imported.
    if BltFirmwareTerminate is not None:
        # Call the function in the shared library
        BltFirmwareTerminate()


def firmware_load_from_file(firmware_file, address_offset=0):
    """
    Loads firmware data from the specified file using the firmware file parser that was
    specified during the initialization of this module.

    :param firmware_file: Filename of the firmware file to load.
    :param address_offset: Optional memory address offset to add when loading the 
           firmware data from the file. This is typically only useful when loading
           firmware data from a binary formatted firmware file.
    :returns: BLT_RESULT_OK if successful, BLT_RESULT_ERROR_xxx otherwise.
    :rtype: int

    :Example:
    ::
    
        import openblt
        
        if openblt.firmware_load_from_file('my_firmware.srec', 0) != openblt.BLT_RESULT_OK:
            print('[ERROR] Could not open firmware file.')
    """
    # Initialize the result.
    result = BLT_RESULT_ERROR_GENERIC
    # Check if the shared library function could be imported.
    if BltFirmwareLoadFromFile is not None:
        # Call the function in the shared library
        result = BltFirmwareLoadFromFile(ctypes.c_char_p(firmware_file.encode('utf-8')),
                                         ctypes.c_uint32(address_offset))
    # Give the result back to the caller.
    return result


def firmware_save_to_file(firmware_file):
    """
    Writes firmware data to the specified file using the firmware file parser that was
    specified during the initialization of this module.

    :param firmware_file: Filename of the firmware file to write to.
    :returns: BLT_RESULT_OK if successful, BLT_RESULT_ERROR_xxx otherwise.
    :rtype: int

    :Example:
    ::
    
        import openblt
        
        if openblt.firmware_save_to_file('new_firmwware.srec') != openblt.BLT_RESULT_OK:
            print('[ERROR] Could not save firmware file.')
    """
    # Initialize the result.
    result = BLT_RESULT_ERROR_GENERIC
    # Check if the shared library function could be imported.
    if BltFirmwareSaveToFile is not None:
        # Call the function in the shared library
        result = BltFirmwareSaveToFile(ctypes.c_char_p(firmware_file.encode('utf-8')))
    # Give the result back to the caller.
    return result


def firmware_get_segment_count():
    """
    Obtains the number of firmware data segments that are currently present in the 
    firmware data module.

    :returns: The total number of segments.
    :rtype: int

    :Example:
    ::
    
        import openblt
        
        print('Number of data segments = ', openblt.firmware_get_segment_count())
    """
    # Initialize the result.
    result = 0
    # Check if the shared library function could be imported.
    if BltFirmwareGetSegmentCount is not None:
        # Call the function in the shared library
        result = BltFirmwareGetSegmentCount()
    # Give the result back to the caller.
    return result


def firmware_get_segment(idx):
    """
    Obtains the contents of the firmware data segment that was specified by the index
    parameter. Note that the returned segment data is a copy.

    :param idx: The segment index. It should be a value greater or equal to zero and
           smaller than the value returned by firmware_get_segment_count.
    :returns: Tuple with the segment data list, address and length.
    :rtype: tuple
    
    :Example:
    ::
    
        import openblt
        
        segment_data, segment_address, segment_len = openblt.firmware_get_segment(0)
        print('First segment address =', hex(segment_address))
        print('First segment length  =', segment_len)
        print('First Segment data =')
        segment_data_str = '\t'
        for idx in range(0, segment_len):
            segment_data_str += '{:02x}'.format(segment_data[idx]) + ' '
            if ((idx+1) % 32) == 0:
                print(segment_data_str)
                segment_data_str = '\t'
        if segment_data_str != '':
            print(segment_data_str)
    """
    # Initialize the result values.
    result_data = []
    result_address = int(0)
    result_len = int(0)
    # Check if the shared library function could be imported.
    if BltFirmwareGetSegment is not None:
        segment_address = ctypes.c_uint32(0)
        segment_len = ctypes.c_uint32(0)
        # Call the function in the shared library
        segment_data = BltFirmwareGetSegment(ctypes.c_uint32(idx),
                                             ctypes.byref(segment_address),
                                             ctypes.byref(segment_len))
        # Store the result values.
        result_address = segment_address.value
        result_len = segment_len.value
        for i in range(result_len):
            result_data.append(segment_data[i])
    # Give the result back to the caller as a tuple.
    return result_data, result_address, result_len


def firmware_add_data(address, len, data):
    """
    Adds data to the segments that are currently present in the firmware data module. If
    the data overlaps with already existing data, the existing data gets overwritten. The
    size of a segment is automatically adjusted or a new segment gets created, 
    if necessary.

    :param address: Base address of the firmware data.
    :param len: Number of bytes to add.
    :param data: List with data bytes that should be added.
    :returns: BLT_RESULT_OK if successful, BLT_RESULT_ERROR_xxx otherwise.
    :rtype: int

    :Example:
    ::

        import openblt
    
        new_data = [ 0x55, 0xaa, 0x11, 0xee ]
        if openblt.firmware_add_data(0x8002000, len(new_data), new_data) != openblt.BLT_RESULT_OK:
            print('[ERROR] Could not add firmware data.')
    """
    # Initialize the result.
    result = BLT_RESULT_ERROR_GENERIC
    # Check if the shared library function could be imported.
    if BltFirmwareAddData is not None:
        # Convert the list to a c-byte array
        new_data_type = ctypes.c_uint8 * len
        new_data = new_data_type()
        for i in range(0, len):
            new_data[i] = ctypes.c_uint8(data[i])
        # Call the function in the shared library
        result = BltFirmwareAddData(ctypes.c_uint32(address), ctypes.c_uint32(len),
                                    new_data)
    # Give the result back to the caller.
    return result


def firmware_remove_data(address, len):
    """
    Removes data from the segments that are currently present in the firmware data 
    module. The size of a segment is automatically adjusted or removed, if necessary.

    :param address: Base address of the firmware data.
    :param len: Number of bytes to remove.
    :returns: BLT_RESULT_OK if successful, BLT_RESULT_ERROR_xxx otherwise.
    :rtype: int

    :Example:
    ::

        import openblt
    
        if openblt.firmware_remove_data(0x8002000, 4) != openblt.BLT_RESULT_OK:
            print('[ERROR] Could not remove firmware data.')
    """
    # Initialize the result.
    result = BLT_RESULT_ERROR_GENERIC
    # Check if the shared library function could be imported.
    if BltFirmwareRemoveData is not None:
        # Call the function in the shared library
        result = BltFirmwareRemoveData(ctypes.c_uint32(address), ctypes.c_uint32(len))
    # Give the result back to the caller.
    return result


def firmware_clear_data():
    """
    Clears all data and segments that are currently present in the firmware data module.

    :Example:
    ::

        import openblt
    
        openblt.firmware_clear_data()
    """
    # Check if the shared library function could be imported.
    if BltFirmwareClearData is not None:
        # Call the function in the shared library
        BltFirmwareClearData()


# ********************************* end of lib.py ***************************************
