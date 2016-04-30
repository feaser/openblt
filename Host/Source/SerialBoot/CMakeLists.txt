#****************************************************************************************
# \file         CMakeLists.txt
# \brief        CMake descriptor file for SerialBoot command line demonstration program.
# \ingroup      SerialBoot
# \internal
#----------------------------------------------------------------------------------------
#                          C O P Y R I G H T
#----------------------------------------------------------------------------------------
#   Copyright (c) 2014  by Feaser    http://www.feaser.com    All rights reserved
#
#----------------------------------------------------------------------------------------
#                            L I C E N S E
#----------------------------------------------------------------------------------------
# This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE. See the GNU General Public License for more details.
#
# You have received a copy of the GNU General Public License along with OpenBLT. It 
# should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
# 
# \endinternal
#****************************************************************************************

# Specify the version being used aswell as the language
cmake_minimum_required(VERSION 2.8)

# Specify the project name
project(SerialBoot)

# Set the port directory, which is platform specific
IF(WIN32)
  set(PROJECT_PORT_DIR ${PROJECT_SOURCE_DIR}/port/win32)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_WIN32 -D_CRT_SECURE_NO_WARNINGS")
ELSEIF(UNIX)
  set(PROJECT_PORT_DIR ${PROJECT_SOURCE_DIR}/port/linux)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_LINUX")
ENDIF(WIN32)

# Build debug version by default
set(CMAKE_BUILD_TYPE "Debug")

# Set include directories
include_directories("${PROJECT_SOURCE_DIR}" "${PROJECT_PORT_DIR}" "${PROJECT_SOURCE_DIR}/port")

# Get header files
file(GLOB_RECURSE INCS "*.h")

# Add sources
add_executable(
  SerialBoot 
  main.c 
  xcpmaster.c 
  srecord.c 
  ${PROJECT_PORT_DIR}/xcptransport.c
  ${PROJECT_PORT_DIR}/timeutil.c
  ${INCS}
)


#*********************************** end of CMakeLists.txt ******************************