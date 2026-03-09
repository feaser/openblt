#****************************************************************************************
#  Description: CMake interface library targets for the OpenBLT bootloader.
#    File Name: CMakeLists.txt
#
#----------------------------------------------------------------------------------------
#                          C O P Y R I G H T
#----------------------------------------------------------------------------------------
#   Copyright (c) 2026  by Feaser    http://www.feaser.com    All rights reserved
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
#****************************************************************************************


# This CMakeLists.txt defines CMake interface targets for the OpenBLT bootloader. With
# the help of these interface targets, users who prefer CMake for building their
# software, can quickly and easily integrate the OpenBLT bootloader into their project
# for creating the bootloader for their microcontroller target. 
#
# You can register these interface targets by including this file with the 
# "add_subdirectory()" command in your project's CMakeLists.txt file. Then list the
# interface targets to include in your "target_link_libraries()" command.
#
# You typically need to include three interface targets in your "target_link_libraries()"
# command:
#
# - One for the hardware independent sources, called "openblt-core".
# - One for the microcontroller family specific sources, e.g. STM32F4.
# - One for the compiler specific toolchain, e.g. GCC, IAR, Keil, etc.
#
# To conveniently find the names for the microcontroller an compiler toolchain specific
# interface targets to use, they follow a fixed naming convention, which resembles the
# directory structure of the OpenBLT bootloader project. The interface target names
# always start with "openblt-", followed by the directory name in lower case and
# underscores are replaced by dashes.
#
# Let's assume you would like to create a bootloader for an STM32F4 microcontroller that
# you can build with the GNU ARM embedded GCC toolchain. The directory structure of the
# OpenBLT bootloader for this setup looks like this:
#
# .
# |-- Target
#     |-- Source
#         |-- ARMCM4_STM32F4
#             |-- GCC
#
# The names for the needed interface targets are then:
#
# - openblt-core
# - openblt-armcm4-stm32f4
# - openblt-armcm4-stm32f4-gcc
#
# Assuming that you copied the contents of the OpenBLT bootloader project's
# "./Target/Source" directory to a subdirectory called "third_party/openblt" in your
# project, an example CMakeLists.txt file for your project for this setup looks like
# this:
#
#   --------------- 
#  | CMakeLists.txt|
#  |---------------------------------------------
#  | project(MyBootloader)                       |
#  |                                             |
#  | add_subdirectory(third_party/openblt)       |
#  |                                             |
#  | add_executable(MyBootloader                 |
#  |     main.c                                  |
#  |     hooks.c                                 |
#  |     startup_stm32f4.s                       |
#  | )                                           |
#  |                                             |
#  | target_link_libraries(MyBootloader          |  
#  |     openblt-core                            |
#  |     openblt-armcm4-stm32f4                  |
#  |     openblt-armcm4-stm32f4-gcc              |
#  | )                                           |
#   --------------------------------------------- 
#
# In some cases, you also want to pull in an additional interface target for third party
# components that the OpenBLT bootloader itself relies one. For example, if you want
# to include functionality in your bootloader, such that it can make firmware updates
# from an SD-card, then the OpenBLT bootloader relies on the FatFS software component.
# you can pull in the FatFS software component by adding "openblt-file-fatfs" as well.
# 
# To find out which other third party software components you can pull in, look up the
# "openblt-file-fatfs" in this file to locate the section that lists all the third party
# software components.


#----------------------------------------------------------------------------------------
#           O P E N B L T   C O R E
#----------------------------------------------------------------------------------------
# OpenBLT core platform independent library target.
add_library(openblt-core INTERFACE)

target_sources(openblt-core INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/asserts.c"
    "${CMAKE_CURRENT_LIST_DIR}/backdoor.c"
    "${CMAKE_CURRENT_LIST_DIR}/boot.c"
    "${CMAKE_CURRENT_LIST_DIR}/com.c"
    "${CMAKE_CURRENT_LIST_DIR}/cop.c"
    "${CMAKE_CURRENT_LIST_DIR}/file.c"
    "${CMAKE_CURRENT_LIST_DIR}/infotable.c"
    "${CMAKE_CURRENT_LIST_DIR}/mb.c"
    "${CMAKE_CURRENT_LIST_DIR}/net.c"
    "${CMAKE_CURRENT_LIST_DIR}/xcp.c"   
)

target_include_directories(openblt-core INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   T H I R D   P A R T Y
#----------------------------------------------------------------------------------------
# OpenBLT uIP TCP/IP stack library target. Only needed for firmware updates via TCP/IP.
add_library(openblt-net-uip INTERFACE)

target_sources(openblt-net-uip INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/third_party/uip/uip/uip.c"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/uip/uip/uip_arp.c"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/uip/uip/uip_timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/uip/uip/uiplib.c"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/uip/apps/dhcpc/dhcpc.c"
)

target_include_directories(openblt-net-uip INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/third_party/uip/uip"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/uip/apps/dhcpc"
)

# OpenBLT FatFS library target. Only needed for firmware update from a FAT file system.
add_library(openblt-file-fatfs INTERFACE)

target_sources(openblt-file-fatfs INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/third_party/fatfs/source/ff.c"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/fatfs/source/ffsystem.c"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/fatfs/source/ffunicode.c"
)

target_include_directories(openblt-file-fatfs INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/third_party/fatfs/source"
)

# OpenBLT TinyUSB library target. Only needed for firmware update via USB device.
add_library(openblt-usb-tinyusb INTERFACE)

target_sources(openblt-usb-tinyusb INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/third_party/tinyusb/src/tusb.c"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/tinyusb/src/class/vendor/vendor_device.c"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/tinyusb/src/common/tusb_fifo.c"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/tinyusb/src/device/usbd.c"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/tinyusb/src/device/usbd_control.c"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/tinyusb/src/portable/usb_board.c"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/tinyusb/src/portable/synopsys/dwc2/dwc2_common.c"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/tinyusb/src/portable/synopsys/dwc2/dcd_dwc2.c"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/tinyusb/src/portable/mentor/musb/dcd_musb.c"
)

target_include_directories(openblt-usb-tinyusb INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/third_party/tinyusb/src"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 C 0   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32C0 platform dependent library target.
add_library(openblt-armcm0-stm32c0 INTERFACE)

target_sources(openblt-armcm0-stm32c0 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32C0/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32C0/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32C0/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32C0/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32C0/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32C0/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32C0/timer.c"
)

target_include_directories(openblt-armcm0-stm32c0 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32C0"
)

# OpenBLT ST STM32C0 GCC toolchain dependent library target.
add_library(openblt-armcm0-stm32c0-gcc INTERFACE)

target_sources(openblt-armcm0-stm32c0-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32C0/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm0-stm32c0-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32C0/GCC"
)

# OpenBLT ST STM32C0 IAR toolchain dependent library target.
add_library(openblt-armcm0-stm32c0-iar INTERFACE)

target_sources(openblt-armcm0-stm32c0-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32C0/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm0-stm32c0-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32C0/IAR"
)

# OpenBLT ST STM32C0 Keil toolchain dependent library target.
add_library(openblt-armcm0-stm32c0-keil INTERFACE)

target_sources(openblt-armcm0-stm32c0-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32C0/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm0-stm32c0-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32C0/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 F 0   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32F0 platform dependent library target.
add_library(openblt-armcm0-stm32f0 INTERFACE)

target_sources(openblt-armcm0-stm32f0 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32F0/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32F0/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32F0/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32F0/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32F0/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32F0/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32F0/timer.c"
)

target_include_directories(openblt-armcm0-stm32f0 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32F0"
)

# OpenBLT ST STM32F0 GCC toolchain dependent library target.
add_library(openblt-armcm0-stm32f0-gcc INTERFACE)

target_sources(openblt-armcm0-stm32f0-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32F0/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm0-stm32f0-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32F0/GCC"
)

# OpenBLT ST STM32F0 IAR toolchain dependent library target.
add_library(openblt-armcm0-stm32f0-iar INTERFACE)

target_sources(openblt-armcm0-stm32f0-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32F0/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm0-stm32f0-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32F0/IAR"
)

# OpenBLT ST STM32F0 Keil toolchain dependent library target.
add_library(openblt-armcm0-stm32f0-keil INTERFACE)

target_sources(openblt-armcm0-stm32f0-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32F0/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm0-stm32f0-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32F0/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 G 0   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32G0 platform dependent library target.
add_library(openblt-armcm0-stm32g0 INTERFACE)

target_sources(openblt-armcm0-stm32g0 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32G0/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32G0/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32G0/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32G0/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32G0/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32G0/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32G0/timer.c"
)

target_include_directories(openblt-armcm0-stm32g0 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32G0"
)

# OpenBLT ST STM32G0 GCC toolchain dependent library target.
add_library(openblt-armcm0-stm32g0-gcc INTERFACE)

target_sources(openblt-armcm0-stm32g0-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32G0/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm0-stm32g0-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32G0/GCC"
)

# OpenBLT ST STM32G0 IAR toolchain dependent library target.
add_library(openblt-armcm0-stm32g0-iar INTERFACE)

target_sources(openblt-armcm0-stm32g0-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32G0/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm0-stm32g0-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32G0/IAR"
)

# OpenBLT ST STM32G0 Keil toolchain dependent library target.
add_library(openblt-armcm0-stm32g0-keil INTERFACE)

target_sources(openblt-armcm0-stm32g0-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32G0/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm0-stm32g0-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32G0/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 L 0   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32L0 platform dependent library target.
add_library(openblt-armcm0-stm32l0 INTERFACE)

target_sources(openblt-armcm0-stm32l0 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32L0/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32L0/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32L0/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32L0/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32L0/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32L0/timer.c"
)

target_include_directories(openblt-armcm0-stm32l0 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32L0"
)

# OpenBLT ST STM32L0 GCC toolchain dependent library target.
add_library(openblt-armcm0-stm32l0-gcc INTERFACE)

target_sources(openblt-armcm0-stm32l0-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32L0/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm0-stm32l0-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32L0/GCC"
)

# OpenBLT ST STM32L0 IAR toolchain dependent library target.
add_library(openblt-armcm0-stm32l0-iar INTERFACE)

target_sources(openblt-armcm0-stm32l0-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32L0/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm0-stm32l0-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32L0/IAR"
)

# OpenBLT ST STM32L0 Keil toolchain dependent library target.
add_library(openblt-armcm0-stm32l0-keil INTERFACE)

target_sources(openblt-armcm0-stm32l0-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32L0/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm0-stm32l0-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_STM32L0/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 H 5   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32H5 platform dependent library target.
add_library(openblt-armcm33-stm32h5 INTERFACE)

target_sources(openblt-armcm33-stm32h5 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32H5/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32H5/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32H5/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32H5/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32H5/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32H5/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32H5/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32H5/usb.c"
)

target_include_directories(openblt-armcm33-stm32h5 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32H5"
)

# OpenBLT ST STM32H5 GCC toolchain dependent library target.
add_library(openblt-armcm33-stm32h5-gcc INTERFACE)

target_sources(openblt-armcm33-stm32h5-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32H5/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm33-stm32h5-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32H5/GCC"
)

# OpenBLT ST STM32H5 IAR toolchain dependent library target.
add_library(openblt-armcm33-stm32h5-iar INTERFACE)

target_sources(openblt-armcm33-stm32h5-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32H5/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm33-stm32h5-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32H5/IAR"
)

# OpenBLT ST STM32H5 Keil toolchain dependent library target.
add_library(openblt-armcm33-stm32h5-keil INTERFACE)

target_sources(openblt-armcm33-stm32h5-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32H5/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm33-stm32h5-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32H5/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 L 5   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32L5 platform dependent library target.
add_library(openblt-armcm33-stm32l5 INTERFACE)

target_sources(openblt-armcm33-stm32l5 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32L5/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32L5/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32L5/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32L5/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32L5/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32L5/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32L5/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32L5/usb.c"
)

target_include_directories(openblt-armcm33-stm32l5 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32L5"
)

# OpenBLT ST STM32L5 GCC toolchain dependent library target.
add_library(openblt-armcm33-stm32l5-gcc INTERFACE)

target_sources(openblt-armcm33-stm32l5-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32L5/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm33-stm32l5-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32L5/GCC"
)

# OpenBLT ST STM32L5 IAR toolchain dependent library target.
add_library(openblt-armcm33-stm32l5-iar INTERFACE)

target_sources(openblt-armcm33-stm32l5-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32L5/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm33-stm32l5-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32L5/IAR"
)

# OpenBLT ST STM32L5 Keil toolchain dependent library target.
add_library(openblt-armcm33-stm32l5-keil INTERFACE)

target_sources(openblt-armcm33-stm32l5-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32L5/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm33-stm32l5-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32L5/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 U 5   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32U5 platform dependent library target.
add_library(openblt-armcm33-stm32u5 INTERFACE)

target_sources(openblt-armcm33-stm32u5 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32U5/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32U5/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32U5/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32U5/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32U5/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32U5/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32U5/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32U5/usb.c"
)

target_include_directories(openblt-armcm33-stm32u5 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32U5"
)

# OpenBLT ST STM32U5 GCC toolchain dependent library target.
add_library(openblt-armcm33-stm32u5-gcc INTERFACE)

target_sources(openblt-armcm33-stm32u5-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32U5/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm33-stm32u5-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32U5/GCC"
)

# OpenBLT ST STM32U5 IAR toolchain dependent library target.
add_library(openblt-armcm33-stm32u5-iar INTERFACE)

target_sources(openblt-armcm33-stm32u5-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32U5/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm33-stm32u5-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32U5/IAR"
)

# OpenBLT ST STM32U5 Keil toolchain dependent library target.
add_library(openblt-armcm33-stm32u5-keil INTERFACE)

target_sources(openblt-armcm33-stm32u5-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32U5/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm33-stm32u5-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM33_STM32U5/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 F 1   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32F1 platform dependent library target.
add_library(openblt-armcm3-stm32f1 INTERFACE)

target_sources(openblt-armcm3-stm32f1 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F1/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F1/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F1/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F1/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F1/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F1/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F1/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F1/usb.c"
)

target_include_directories(openblt-armcm3-stm32f1 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F1"
)

# OpenBLT ST STM32F1 GCC toolchain dependent library target.
add_library(openblt-armcm3-stm32f1-gcc INTERFACE)

target_sources(openblt-armcm3-stm32f1-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F1/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm3-stm32f1-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F1/GCC"
)

# OpenBLT ST STM32F1 IAR toolchain dependent library target.
add_library(openblt-armcm3-stm32f1-iar INTERFACE)

target_sources(openblt-armcm3-stm32f1-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F1/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm3-stm32f1-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F1/IAR"
)

# OpenBLT ST STM32F1 Keil toolchain dependent library target.
add_library(openblt-armcm3-stm32f1-keil INTERFACE)

target_sources(openblt-armcm3-stm32f1-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F1/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm3-stm32f1-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F1/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 F 2   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32F2 platform dependent library target.
add_library(openblt-armcm3-stm32f2 INTERFACE)

target_sources(openblt-armcm3-stm32f2 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F2/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F2/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F2/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F2/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F2/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F2/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F2/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F2/usb.c"
)

target_include_directories(openblt-armcm3-stm32f2 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F2"
)

# OpenBLT ST STM32F2 GCC toolchain dependent library target.
add_library(openblt-armcm3-stm32f2-gcc INTERFACE)

target_sources(openblt-armcm3-stm32f2-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F2/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm3-stm32f2-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F2/GCC"
)

# OpenBLT ST STM32F2 IAR toolchain dependent library target.
add_library(openblt-armcm3-stm32f2-iar INTERFACE)

target_sources(openblt-armcm3-stm32f2-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F2/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm3-stm32f2-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F2/IAR"
)

# OpenBLT ST STM32F2 Keil toolchain dependent library target.
add_library(openblt-armcm3-stm32f2-keil INTERFACE)

target_sources(openblt-armcm3-stm32f2-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F2/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm3-stm32f2-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32F2/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 L 1   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32L1 platform dependent library target.
add_library(openblt-armcm3-stm32l1 INTERFACE)

target_sources(openblt-armcm3-stm32l1 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32L1/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32L1/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32L1/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32L1/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32L1/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32L1/timer.c"
)

target_include_directories(openblt-armcm3-stm32l1 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32L1"
)

# OpenBLT ST STM32L1 GCC toolchain dependent library target.
add_library(openblt-armcm3-stm32l1-gcc INTERFACE)

target_sources(openblt-armcm3-stm32l1-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32L1/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm3-stm32l1-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32L1/GCC"
)

# OpenBLT ST STM32L1 IAR toolchain dependent library target.
add_library(openblt-armcm3-stm32l1-iar INTERFACE)

target_sources(openblt-armcm3-stm32l1-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32L1/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm3-stm32l1-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32L1/IAR"
)

# OpenBLT ST STM32L1 Keil toolchain dependent library target.
add_library(openblt-armcm3-stm32l1-keil INTERFACE)

target_sources(openblt-armcm3-stm32l1-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32L1/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm3-stm32l1-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_STM32L1/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 F 3   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32F3 platform dependent library target.
add_library(openblt-armcm4-stm32f3 INTERFACE)

target_sources(openblt-armcm4-stm32f3 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F3/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F3/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F3/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F3/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F3/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F3/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F3/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F3/usb.c"
)

target_include_directories(openblt-armcm4-stm32f3 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F3"
)

# OpenBLT ST STM32F3 GCC toolchain dependent library target.
add_library(openblt-armcm4-stm32f3-gcc INTERFACE)

target_sources(openblt-armcm4-stm32f3-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F3/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm4-stm32f3-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F3/GCC"
)

# OpenBLT ST STM32F3 IAR toolchain dependent library target.
add_library(openblt-armcm4-stm32f3-iar INTERFACE)

target_sources(openblt-armcm4-stm32f3-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F3/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm4-stm32f3-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F3/IAR"
)

# OpenBLT ST STM32F3 Keil toolchain dependent library target.
add_library(openblt-armcm4-stm32f3-keil INTERFACE)

target_sources(openblt-armcm4-stm32f3-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F3/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm4-stm32f3-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F3/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 F 4   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32F4 platform dependent library target.
add_library(openblt-armcm4-stm32f4 INTERFACE)

target_sources(openblt-armcm4-stm32f4 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F4/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F4/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F4/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F4/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F4/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F4/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F4/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F4/usb.c"
)

target_include_directories(openblt-armcm4-stm32f4 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F4"
)

# OpenBLT ST STM32F4 GCC toolchain dependent library target.
add_library(openblt-armcm4-stm32f4-gcc INTERFACE)

target_sources(openblt-armcm4-stm32f4-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F4/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm4-stm32f4-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F4/GCC"
)

# OpenBLT ST STM32F4 IAR toolchain dependent library target.
add_library(openblt-armcm4-stm32f4-iar INTERFACE)

target_sources(openblt-armcm4-stm32f4-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F4/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm4-stm32f4-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F4/IAR"
)

# OpenBLT ST STM32F4 Keil toolchain dependent library target.
add_library(openblt-armcm4-stm32f4-keil INTERFACE)

target_sources(openblt-armcm4-stm32f4-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F4/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm4-stm32f4-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32F4/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 G 4   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32G4 platform dependent library target.
add_library(openblt-armcm4-stm32g4 INTERFACE)

target_sources(openblt-armcm4-stm32g4 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32G4/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32G4/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32G4/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32G4/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32G4/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32G4/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32G4/timer.c"
)

target_include_directories(openblt-armcm4-stm32g4 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32G4"
)

# OpenBLT ST STM32G4 GCC toolchain dependent library target.
add_library(openblt-armcm4-stm32g4-gcc INTERFACE)

target_sources(openblt-armcm4-stm32g4-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32G4/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm4-stm32g4-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32G4/GCC"
)

# OpenBLT ST STM32G4 IAR toolchain dependent library target.
add_library(openblt-armcm4-stm32g4-iar INTERFACE)

target_sources(openblt-armcm4-stm32g4-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32G4/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm4-stm32g4-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32G4/IAR"
)

# OpenBLT ST STM32G4 Keil toolchain dependent library target.
add_library(openblt-armcm4-stm32g4-keil INTERFACE)

target_sources(openblt-armcm4-stm32g4-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32G4/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm4-stm32g4-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32G4/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 L 4   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32L4 platform dependent library target.
add_library(openblt-armcm4-stm32l4 INTERFACE)

target_sources(openblt-armcm4-stm32l4 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32L4/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32L4/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32L4/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32L4/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32L4/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32L4/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32L4/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32L4/usb.c"
)

target_include_directories(openblt-armcm4-stm32l4 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32L4"
)

# OpenBLT ST STM32L4 GCC toolchain dependent library target.
add_library(openblt-armcm4-stm32l4-gcc INTERFACE)

target_sources(openblt-armcm4-stm32l4-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32L4/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm4-stm32l4-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32L4/GCC"
)

# OpenBLT ST STM32L4 IAR toolchain dependent library target.
add_library(openblt-armcm4-stm32l4-iar INTERFACE)

target_sources(openblt-armcm4-stm32l4-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32L4/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm4-stm32l4-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32L4/IAR"
)

# OpenBLT ST STM32L4 Keil toolchain dependent library target.
add_library(openblt-armcm4-stm32l4-keil INTERFACE)

target_sources(openblt-armcm4-stm32l4-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32L4/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm4-stm32l4-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_STM32L4/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 F 7   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32F7 platform dependent library target.
add_library(openblt-armcm7-stm32f7 INTERFACE)

target_sources(openblt-armcm7-stm32f7 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32F7/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32F7/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32F7/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32F7/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32F7/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32F7/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32F7/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32F7/usb.c"
)

target_include_directories(openblt-armcm7-stm32f7 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32F7"
)

# OpenBLT ST STM32F7 GCC toolchain dependent library target.
add_library(openblt-armcm7-stm32f7-gcc INTERFACE)

target_sources(openblt-armcm7-stm32f7-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32F7/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm7-stm32f7-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32F7/GCC"
)

# OpenBLT ST STM32F7 IAR toolchain dependent library target.
add_library(openblt-armcm7-stm32f7-iar INTERFACE)

target_sources(openblt-armcm7-stm32f7-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32F7/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm7-stm32f7-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32F7/IAR"
)

# OpenBLT ST STM32F7 Keil toolchain dependent library target.
add_library(openblt-armcm7-stm32f7-keil INTERFACE)

target_sources(openblt-armcm7-stm32f7-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32F7/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm7-stm32f7-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32F7/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S T   S T M 3 2 H 7   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT ST STM32H7 platform dependent library target.
add_library(openblt-armcm7-stm32h7 INTERFACE)

target_sources(openblt-armcm7-stm32h7 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32H7/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32H7/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32H7/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32H7/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32H7/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32H7/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32H7/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32H7/usb.c"
)

target_include_directories(openblt-armcm7-stm32h7 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32H7"
)

# OpenBLT ST STM32H7 GCC toolchain dependent library target.
add_library(openblt-armcm7-stm32h7-gcc INTERFACE)

target_sources(openblt-armcm7-stm32h7-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32H7/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm7-stm32h7-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32H7/GCC"
)

# OpenBLT ST STM32H7 IAR toolchain dependent library target.
add_library(openblt-armcm7-stm32h7-iar INTERFACE)

target_sources(openblt-armcm7-stm32h7-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32H7/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm7-stm32h7-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32H7/IAR"
)

# OpenBLT ST STM32H7 Keil toolchain dependent library target.
add_library(openblt-armcm7-stm32h7-keil INTERFACE)

target_sources(openblt-armcm7-stm32h7-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32H7/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm7-stm32h7-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM7_STM32H7/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   N X P   S 3 2 K 1 1   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT NXP S32K11 platform dependent library target.
add_library(openblt-armcm0-s32k11 INTERFACE)

target_sources(openblt-armcm0-s32k11 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_S32K11/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_S32K11/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_S32K11/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_S32K11/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_S32K11/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_S32K11/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_S32K11/timer.c"
)

target_include_directories(openblt-armcm0-s32k11 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_S32K11"
)

# OpenBLT NXP S32K11 GCC toolchain dependent library target.
add_library(openblt-armcm0-s32k11-gcc INTERFACE)

target_sources(openblt-armcm0-s32k11-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_S32K11/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm0-s32k11-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_S32K11/GCC"
)

# OpenBLT NXP S32K11 IAR toolchain dependent library target.
add_library(openblt-armcm0-s32k11-iar INTERFACE)

target_sources(openblt-armcm0-s32k11-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_S32K11/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm0-s32k11-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_S32K11/IAR"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   N X P   S 3 2 K 1 4   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT NXP S32K14 platform dependent library target.
add_library(openblt-armcm4-s32k14 INTERFACE)

target_sources(openblt-armcm4-s32k14 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_S32K14/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_S32K14/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_S32K14/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_S32K14/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_S32K14/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_S32K14/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_S32K14/timer.c"
)

target_include_directories(openblt-armcm4-s32k14 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_S32K14"
)

# OpenBLT NXP S32K14 GCC toolchain dependent library target.
add_library(openblt-armcm4-s32k14-gcc INTERFACE)

target_sources(openblt-armcm4-s32k14-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_S32K14/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm4-s32k14-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_S32K14/GCC"
)

# OpenBLT NXP S32K14 IAR toolchain dependent library target.
add_library(openblt-armcm4-s32k14-iar INTERFACE)

target_sources(openblt-armcm4-s32k14-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_S32K14/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm4-s32k14-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_S32K14/IAR"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   N X P   H C S 1 2   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT NXP HCS12 platform dependent library target.
add_library(openblt-hcs12 INTERFACE)

target_sources(openblt-hcs12 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/HCS12/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/HCS12/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/HCS12/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/HCS12/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/HCS12/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/HCS12/timer.c"
)

target_include_directories(openblt-hcs12 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/HCS12"
)

# OpenBLT NXP HCS12 standard flash library target.
add_library(openblt-hcs12-flash-standard INTERFACE)

target_sources(openblt-hcs12-flash-standard INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/HCS12/flash.c"
)

# OpenBLT NXP HCS12 ECC flash library target.
add_library(openblt-hcs12-flash-ecc INTERFACE)

target_sources(openblt-hcs12-flash-ecc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/HCS12/flash_ecc.c"
)

# OpenBLT NXP HCS12 CodeWarrior toolchain dependent library target.
add_library(openblt-hcs12-codewarrior INTERFACE)

target_sources(openblt-hcs12-codewarrior INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/HCS12/CodeWarrior/cpu_comp.c"
)

target_include_directories(openblt-hcs12-codewarrior INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/HCS12/CodeWarrior"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   S I L I C O N   L A B S   E F M 3 2   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT Silicon Labs EFM32 platform dependent library target.
add_library(openblt-armcm3-efm32 INTERFACE)

target_sources(openblt-armcm3-efm32 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_EFM32/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_EFM32/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_EFM32/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_EFM32/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_EFM32/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_EFM32/timer.c"
)

target_include_directories(openblt-armcm3-efm32 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_EFM32"
)

# OpenBLT Silicon Labs EFM32 GCC toolchain dependent library target.
add_library(openblt-armcm3-efm32-gcc INTERFACE)

target_sources(openblt-armcm3-efm32-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_EFM32/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm3-efm32-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_EFM32/GCC"
)

# OpenBLT Silicon Labs EFM32 IAR toolchain dependent library target.
add_library(openblt-armcm3-efm32-iar INTERFACE)

target_sources(openblt-armcm3-efm32-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_EFM32/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm3-efm32-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_EFM32/IAR"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   T E X A S   I N S T R U M E N T S   L M 3 S   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT Texas Instruments LM3S platform dependent library target.
add_library(openblt-armcm3-lm3s INTERFACE)

target_sources(openblt-armcm3-lm3s INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_LM3S/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_LM3S/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_LM3S/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_LM3S/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_LM3S/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_LM3S/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_LM3S/timer.c"
)

target_include_directories(openblt-armcm3-lm3s INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_LM3S"
)

# OpenBLT Texas Instruments LM3S GCC toolchain dependent library target.
add_library(openblt-armcm3-lm3s-gcc INTERFACE)

target_sources(openblt-armcm3-lm3s-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_LM3S/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm3-lm3s-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_LM3S/GCC"
)

# OpenBLT Texas Instruments LM3S IAR toolchain dependent library target.
add_library(openblt-armcm3-lm3s-iar INTERFACE)

target_sources(openblt-armcm3-lm3s-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_LM3S/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm3-lm3s-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM3_LM3S/IAR"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   T E X A S   I N S T R U M E N T S   T M 4 C   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT Texas Instruments TM4C platform dependent library target.
add_library(openblt-armcm4-tm4c INTERFACE)

target_sources(openblt-armcm4-tm4c INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_TM4C/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_TM4C/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_TM4C/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_TM4C/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_TM4C/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_TM4C/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_TM4C/usb.c"
)

target_include_directories(openblt-armcm4-tm4c INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_TM4C"
)

# OpenBLT Texas Instruments TM4C IAR toolchain dependent library target.
add_library(openblt-armcm4-tm4c-iar INTERFACE)

target_sources(openblt-armcm4-tm4c-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_TM4C/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm4-tm4c-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_TM4C/IAR"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   I N F I N E O N   X M C 1   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT Infineon XMC1 platform dependent library target.
add_library(openblt-armcm0-xmc1 INTERFACE)

target_sources(openblt-armcm0-xmc1 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_XMC1/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_XMC1/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_XMC1/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_XMC1/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_XMC1/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_XMC1/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_XMC1/timer.c"
)

target_include_directories(openblt-armcm0-xmc1 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_XMC1"
)

# OpenBLT Infineon XMC1 GCC toolchain dependent library target.
add_library(openblt-armcm0-xmc1-gcc INTERFACE)

target_sources(openblt-armcm0-xmc1-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_XMC1/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm0-xmc1-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_XMC1/GCC"
)

# OpenBLT Infineon XMC1 IAR toolchain dependent library target.
add_library(openblt-armcm0-xmc1-iar INTERFACE)

target_sources(openblt-armcm0-xmc1-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_XMC1/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm0-xmc1-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_XMC1/IAR"
)

# OpenBLT Infineon XMC1 Keil toolchain dependent library target.
add_library(openblt-armcm0-xmc1-keil INTERFACE)

target_sources(openblt-armcm0-xmc1-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_XMC1/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm0-xmc1-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM0_XMC1/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   I N F I N E O N   X M C 4   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT Infineon XMC4 platform dependent library target.
add_library(openblt-armcm4-xmc4 INTERFACE)

target_sources(openblt-armcm4-xmc4 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_XMC4/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_XMC4/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_XMC4/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_XMC4/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_XMC4/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_XMC4/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_XMC4/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_XMC4/usb.c" 
)

target_include_directories(openblt-armcm4-xmc4 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_XMC4"
)

# OpenBLT Infineon XMC4 GCC toolchain dependent library target.
add_library(openblt-armcm4-xmc4-gcc INTERFACE)

target_sources(openblt-armcm4-xmc4-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_XMC4/GCC/cpu_comp.c"
)

target_include_directories(openblt-armcm4-xmc4-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_XMC4/GCC"
)

# OpenBLT Infineon XMC4 IAR toolchain dependent library target.
add_library(openblt-armcm4-xmc4-iar INTERFACE)

target_sources(openblt-armcm4-xmc4-iar INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_XMC4/IAR/cpu_comp.c"
)

target_include_directories(openblt-armcm4-xmc4-iar INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_XMC4/IAR"
)

# OpenBLT Infineon XMC4 Keil toolchain dependent library target.
add_library(openblt-armcm4-xmc4-keil INTERFACE)

target_sources(openblt-armcm4-xmc4-keil INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_XMC4/Keil/cpu_comp.c"
)

target_include_directories(openblt-armcm4-xmc4-keil INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/ARMCM4_XMC4/Keil"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   I N F I N E O N  T R I C O R E   T C 2   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT Infineon Tricore TC2 platform dependent library target.
add_library(openblt-tricore-tc2 INTERFACE)

target_sources(openblt-tricore-tc2 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC2/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC2/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC2/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC2/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC2/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC2/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC2/timer.c"
)

target_include_directories(openblt-tricore-tc2 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC2"
)

# OpenBLT Infineon Tricore TC2 Tasking toolchain dependent library target.
add_library(openblt-tricore-tc2-tasking INTERFACE)

target_sources(openblt-tricore-tc2-tasking INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC2/Tasking/cpu_comp.c"
)

target_include_directories(openblt-tricore-tc2-tasking INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC2/Tasking"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   I N F I N E O N  T R I C O R E   T C 3   P O R T
#----------------------------------------------------------------------------------------
# OpenBLT Infineon Tricore TC3 platform dependent library target.
add_library(openblt-tricore-tc3 INTERFACE)

target_sources(openblt-tricore-tc3 INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC3/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC3/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC3/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC3/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC3/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC3/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC3/timer.c"
)

target_include_directories(openblt-tricore-tc3 INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC3"
)

# OpenBLT Infineon Tricore TC3 Tasking toolchain dependent library target.
add_library(openblt-tricore-tc3-tasking INTERFACE)

target_sources(openblt-tricore-tc3-tasking INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC3/Tasking/cpu_comp.c"
)

target_include_directories(openblt-tricore-tc3-tasking INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/TRICORE_TC3/Tasking"
)


#----------------------------------------------------------------------------------------
#           O P E N B L T   N E W   P O R T   T E M P L A T E
#----------------------------------------------------------------------------------------
# OpenBLT new port template platform dependent library target.
add_library(openblt-template INTERFACE)

target_sources(openblt-template INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/_template/can.c"
    "${CMAKE_CURRENT_LIST_DIR}/_template/cpu.c"
    "${CMAKE_CURRENT_LIST_DIR}/_template/flash.c"
    "${CMAKE_CURRENT_LIST_DIR}/_template/mbrtu.c"
    "${CMAKE_CURRENT_LIST_DIR}/_template/nvm.c"
    "${CMAKE_CURRENT_LIST_DIR}/_template/rs232.c"
    "${CMAKE_CURRENT_LIST_DIR}/_template/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/_template/usb.c"
)

target_include_directories(openblt-template INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/_template"
)

# OpenBLT new port template GCC toolchain dependent library target.
add_library(openblt-template-gcc INTERFACE)

target_sources(openblt-template-gcc INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/_template/GCC/cpu_comp.c"
)

target_include_directories(openblt-template-gcc INTERFACE 
    "${CMAKE_CURRENT_LIST_DIR}/_template/GCC"
)

#--------------------------------- end of CMakeLists.txt --------------------------------
