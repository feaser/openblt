#!/usr/bin/env python
"""
Setuptools setup specification for package 'openblt'.
"""
__docformat__ = 'reStructuredText'
# ***************************************************************************************
#  File Name: setup.py
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
from setuptools import setup


# ***************************************************************************************
#  Implementation
# ***************************************************************************************
setup ( \
    name = 'openblt',
    version = '1.3.7',
    description = 'Python wrapper for the OpenBLT host library (LibOpenBLT).',
    long_description = open('README.rst', 'r').read(),
    author = 'Frank Voorburg',
    author_email = 'voorburg@feaser.com',
    url = 'https://www.feaser.com/openblt/doku.php?id=manual:libopenblt',
    packages = ['openblt'],
)


# ********************************* end of setup.py *************************************
