#****************************************************************************************
# \file         CMakeLists.txt
# \brief        CMake descriptor file for the XCP Seed and Key shared library.
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
cmake_minimum_required(VERSION 3.15)


#****************************************************************************************
# Project configuration
#****************************************************************************************
# Specify the project name
project(seednkey)

# Build debug version by default
set(CMAKE_BUILD_TYPE "Debug")


#****************************************************************************************
# Options
#****************************************************************************************
# Add option with default value to disable the generation of the PC-lint target. It can 
# be overridden on the command line when CMake is called using the following parameter:
# -DLINT_ENABLED=ON  
option(LINT_ENABLED "Configurable to enable/disable the PC-lint target" OFF)


#****************************************************************************************
# Directories
#****************************************************************************************
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


#****************************************************************************************
# Compiler flags
#****************************************************************************************
# Set platform specific compiler macros PLATFORM_XXX
if(WIN32)
  if(CMAKE_C_COMPILER_ID MATCHES GNU) 
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_WINDOWS -DPLATFORM_32BIT -D_CRT_SECURE_NO_WARNINGS -std=gnu99")
    else()
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_WINDOWS -DPLATFORM_64BIT -D_CRT_SECURE_NO_WARNINGS -std=gnu99")
    endif()
  elseif(CMAKE_C_COMPILER_ID MATCHES MSVC)
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_WINDOWS -DPLATFORM_32BIT -D_CRT_SECURE_NO_WARNINGS")
    else()
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_WINDOWS -DPLATFORM_64BIT -D_CRT_SECURE_NO_WARNINGS")
    endif()
  endif()
elseif(UNIX)
  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_LINUX -DPLATFORM_32BIT -pthread -std=gnu99")
  else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_LINUX -DPLATFORM_64BIT -pthread -std=gnu99")
  endif()
endif(WIN32)

# Configure a statically linked run-time library for msvc
if(WIN32)
  if(CMAKE_C_COMPILER_ID MATCHES MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  endif()
endif(WIN32)


#***************************************************************************************
# Includes
#****************************************************************************************
# Set include directories
include_directories("${PROJECT_SOURCE_DIR}")


#***************************************************************************************
# Files
#****************************************************************************************
# Get header files from the root directory
file(GLOB INCS_ROOT "*.h")
set(INCS ${INCS_ROOT})

# Add sources
set(
  LIB_SRCS
  seednkey.c
  ${INCS}
)


#***************************************************************************************
# Targets
#****************************************************************************************
# Set main target. Use "make seednkey_shared" to individually build the shared library.
add_library(seednkey_shared SHARED ${LIB_SRCS})
if(CMAKE_C_COMPILER_ID MATCHES MSVC)
  # Microsoft Visual Studio does not add "lib" to the name of the DLL, whereas GCC
  # (including MinGW) does. Correct this here.
  SET_TARGET_PROPERTIES(seednkey_shared PROPERTIES OUTPUT_NAME libseednkey CLEAN_DIRECT_OUTPUT 1)
else()
  SET_TARGET_PROPERTIES(seednkey_shared PROPERTIES OUTPUT_NAME seednkey CLEAN_DIRECT_OUTPUT 1)
endif()

# Only generate the PC-lint taget if the option is enabled. Use "make seednkey_LINT" to 
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
    add_pc_lint(seednkey ${LIB_SRCS})
  endif(COMMAND add_pc_lint)
endif(LINT_ENABLED)


#*********************************** end of CMakeLists.txt ****************************** 
