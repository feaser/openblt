"""
Package **openblt** is a python wrapper for the OpenBLT Host Library (`LibOpenBLT <https://www.feaser.com/openblt/doku.php?id=manual:libopenblt>`_).  

Have a look at the function headers inside openblt.lib for details on how to call the functions, including examples.
"""
__docformat__ = 'reStructuredText'
# ***************************************************************************************
#  File Name: __init__.py
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
from openblt.lib import BLT_RESULT_OK
from openblt.lib import BLT_RESULT_ERROR_GENERIC


# ***************************************************************************************
#              V E R S I O N   I N F O R M A T I O N
# ***************************************************************************************
from openblt.lib import version_get_number
from openblt.lib import version_get_string


# ***************************************************************************************
#              F I R M W A R E   D A T A
# ***************************************************************************************
from openblt.lib import BLT_FIRMWARE_PARSER_SRECORD
from openblt.lib import firmware_terminate
from openblt.lib import firmware_init
from openblt.lib import firmware_load_from_file
from openblt.lib import firmware_save_to_file
from openblt.lib import firmware_get_segment_count
from openblt.lib import firmware_get_segment
from openblt.lib import firmware_add_data
from openblt.lib import firmware_remove_data
from openblt.lib import firmware_clear_data


# ********************************* end of __init__.py **********************************
