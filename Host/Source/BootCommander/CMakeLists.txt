#****************************************************************************************
# \file         CMakeLists.txt
# \brief        CMake descriptor file for BootCommander command line program.
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
project(BootCommander)

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

# Set OpenBLT library related directory locations
set(LIBOPENBLT_INC ${PROJECT_SOURCE_DIR}/../LibOpenBLT)
set(LIBOPENBLT_LIB ${PROJECT_OUTPUT_DIRECTORY})


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
elseif(APPLE)
  set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
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
include_directories("${PROJECT_SOURCE_DIR}" "${LIBOPENBLT_INC}")

# Add search path for the linker
link_directories("${LIBOPENBLT_LIB}")


#***************************************************************************************
# Files
#****************************************************************************************
# Get header files from the root directory
file(GLOB INCS_ROOT "*.h")
set(INCS ${INCS_ROOT})

# Add sources
set(
  PROG_SRCS
  main.c
  ${LIBOPENBLT_INC}/openblt.h
  ${INCS}
)

# Set library name of the OpenBLT Host Library
if(CMAKE_C_COMPILER_ID MATCHES MSVC)
  # When building LibOpenBLT with Microsoft Visual Studio, "lib" was added to the name
  # of the DLL. This needs to be done as well when linking the library.
  set (LIBOPENBLT_LIBNAME libopenblt)
else()
  set (LIBOPENBLT_LIBNAME openblt)
endif()


#***************************************************************************************
# Search path
#****************************************************************************************
# Add the path of the executable to the library search path. This way the LibOpenBLT
# shared library can simply be in the same directory as the BootCommander executable.
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "\$ORIGIN")


#***************************************************************************************
# Targets
#****************************************************************************************
# Set main target. Use "make BootCommander" to individually build the program.
add_executable(
  BootCommander
  ${PROG_SRCS}
)

# Add libraries.
target_link_libraries(BootCommander ${LIBOPENBLT_LIBNAME})

# Only generate the PC-lint taget if the option is enabled. Use "make BootCommander_LINT"
# to lint the project sources
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
    add_pc_lint(BootCommander ${PROG_SRCS})
  endif(COMMAND add_pc_lint)
endif(LINT_ENABLED)


#*********************************** end of CMakeLists.txt ******************************
