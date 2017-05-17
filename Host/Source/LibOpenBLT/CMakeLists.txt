#****************************************************************************************
# \file         CMakeLists.txt
# \brief        CMake descriptor file for the OpenBLT host library.
# \ingroup      LibOpenBLT
# \internal
#----------------------------------------------------------------------------------------
#                          C O P Y R I G H T
#----------------------------------------------------------------------------------------
#   Copyright (c) 2017  by Feaser    http://www.feaser.com    All rights reserved
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


#****************************************************************************************
# Project configuration
#****************************************************************************************
# Specify the project name
project(LibOpenBLT)

# Build debug version by default
set(CMAKE_BUILD_TYPE "Debug")


#****************************************************************************************
# Options
#****************************************************************************************
# Add option with default value to enable the generation and building of the static
# library. It can be overridden on the command line when CMake is called using the 
# following parameter: -DBUILD_STATIC=OFF
option(BUILD_STATIC "Configurable to enable/disable building of the static library" ON)

# Add option with default value to enable the generation and building of the shared
# library. It can be overridden on the command line when CMake is called using the 
# following parameter: -DBUILD_SHARED=OFF
option(BUILD_SHARED "Configurable to enable/disable building of the shared library" ON)

# Add option with default value to disable the generation of the PC-lint target. It can 
# be overridden on the command line when CMake is called using the following parameter:
# -DLINT_ENABLED=ON  
option(LINT_ENABLED "Configurable to enable/disable the PC-lint target" OFF)


#****************************************************************************************
# Directories
#****************************************************************************************
# Set the port directory, which is platform specific
if(WIN32)
  set(PROJECT_PORT_DIR ${PROJECT_SOURCE_DIR}/port/windows)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_WIN32 -D_CRT_SECURE_NO_WARNINGS")
elseif(UNIX)
  set(PROJECT_PORT_DIR ${PROJECT_SOURCE_DIR}/port/linux)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_LINUX")
endif(WIN32)

# Set the output directory
set (PROJECT_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../..)

# Set the output directory for the generic no-config case (e.g. with mingw)
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIRECTORY} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIRECTORY} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIRECTORY} )
# Set the output directory for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_OUTPUT_DIRECTORY} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_OUTPUT_DIRECTORY} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_OUTPUT_DIRECTORY} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )


#***************************************************************************************
# Includes
#****************************************************************************************
# Set include directories
include_directories("${PROJECT_SOURCE_DIR}" "${PROJECT_PORT_DIR}")


#***************************************************************************************
# Files
#****************************************************************************************
# Get header files from the root directory and the port directory.
file(GLOB INCS_ROOT "*.h")
file(GLOB INCS_PORT "${PROJECT_PORT_DIR}/*.h")
set(INCS ${INCS_ROOT} ${INCS_PORT})

# Add sources
set(
  LIB_SRCS 
  openblt.c
  session.c
  xcploader.c
  xcptpuart.c
  firmware.c
  srecparser.c
  util.c
  ${PROJECT_PORT_DIR}/timeutil.c
  ${PROJECT_PORT_DIR}/serialport.c
  ${INCS}
)


#***************************************************************************************
# Targets
#****************************************************************************************
# Only generate the static library taget if the option is enabled. Use 
# "make openblt_static" to individually build the static library. 
if(BUILD_STATIC)
  add_library(openblt_static STATIC ${LIB_SRCS})
  SET_TARGET_PROPERTIES(openblt_static PROPERTIES OUTPUT_NAME openblt CLEAN_DIRECT_OUTPUT 1)
endif(BUILD_STATIC)

# Only generate the shared library taget if the option is enabled. Use 
# "make openblt_shared" to build individually  shared library.
if(BUILD_SHARED)
  add_library(openblt_shared SHARED ${LIB_SRCS})
  SET_TARGET_PROPERTIES(openblt_shared PROPERTIES OUTPUT_NAME openblt CLEAN_DIRECT_OUTPUT 1)
endif(BUILD_SHARED)

# Only generate the PC-lint taget if the option is enabled. Use "make openblt_LINT" to 
# lint the project sources
if(LINT_ENABLED)
  # Include PC-lint configuration file for the correct compiler. Currently GNU GCC and
  # Microsoft Visual Studio are supported.
  if(CMAKE_C_COMPILER_ID MATCHES GNU)  
    include(${PROJECT_SOURCE_DIR}/lint/gnu/pc_lint.cmake)
  elseif(CMAKE_C_COMPILER_ID MATCHES MSVC)
    include(${PROJECT_SOURCE_DIR}/lint/msvc/pc_lint.cmake)
  endif()

  # Generate the PC-lint target.  
  if(COMMAND add_pc_lint)
    add_pc_lint(openblt ${LIB_SRCS})
  endif(COMMAND add_pc_lint)
endif(LINT_ENABLED)


#*********************************** end of CMakeLists.txt ****************************** 
