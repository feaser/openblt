cmake_minimum_required(VERSION 3.15)

# Configure the toolchain file. Set before the project() line.
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/.cmake/arm-cortex-gcc-toolchain.cmake) 

# Configure C and C++ standards.
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Specify overall project name.
project(demoprog_xmc4700 LANGUAGES C CXX ASM)

# Include the OpenBLT component for the uIP TCP/IP stack
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../Source uip)

# Include other third party components.
include("${CMAKE_SOURCE_DIR}/.cmake/Cmsis.cmake")
include("${CMAKE_SOURCE_DIR}/.cmake/XmcLib.cmake")

# Configure project include directories.
set(PROJECT_INCLUDES
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/lib"
    "${CMAKE_CURRENT_LIST_DIR}/lib/uip"
)

# Configure project sources.
set(PROJECT_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/boot.c"
    "${CMAKE_CURRENT_LIST_DIR}/boot.h"
    "${CMAKE_CURRENT_LIST_DIR}/led.c"
    "${CMAKE_CURRENT_LIST_DIR}/led.h"
    "${CMAKE_CURRENT_LIST_DIR}/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/net.c"
    "${CMAKE_CURRENT_LIST_DIR}/shared_params.c"
    "${CMAKE_CURRENT_LIST_DIR}/startup_XMC4700.s"
    "${CMAKE_CURRENT_LIST_DIR}/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/lib/syscalls.c"
    "${CMAKE_CURRENT_LIST_DIR}/lib/system_XMC4700.c"
    "${CMAKE_CURRENT_LIST_DIR}/lib/uip/clock-arch.c"
    "${CMAKE_CURRENT_LIST_DIR}/lib/uip/netdev.c"
)

# Configure project libraries.
set(PROJECT_LIBRARIES
    infineon-xmc4-cmsis
    infineon-xmc-lib
    openblt-net-uip 
)

# Configure project macro definitions.
set(PROJECT_MACROS
    XMC4700_F144x2048
    XMC_ETH_PHY_KSZ8081RNB
)

# Configure project specific CPU/compiler flags and linker script.
set(PROJECT_CPU_FLAGS -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard)
set(PROJECT_COMPILER_FLAGS -Wall -Wextra -Wno-unused-parameter -Wno-implicit-fallthrough)
set(PROJECT_LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/linker_script.ld")

# Default to the Debug build type if none other specified.
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()

# Set target executable.
add_executable(${PROJECT_NAME})

# Add include directories.
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_INCLUDES}
)

# Add sources.
target_sources(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCES}
)

# Add additional compiler options.
target_compile_options(${PROJECT_NAME} PRIVATE
    ${PROJECT_CPU_FLAGS}
    ${PROJECT_COMPILER_FLAGS}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    ${PROJECT_MACROS}
)

# Add libraries to include.
target_link_libraries(${PROJECT_NAME}
    ${PROJECT_LIBRARIES}
)

# Add additional link options.
target_link_options(${PROJECT_NAME} PRIVATE
    ${PROJECT_CPU_FLAGS}
    -T "${PROJECT_LINKER_SCRIPT}"
    -nostartfiles    
    --specs=nano.specs
    --specs=nosys.specs 
)

# Generate a map file (optional).
arm_cortex_gcc_generate_map(${PROJECT_NAME})

# Generate an additional firmware file in the S-record format (optional).
arm_cortex_gcc_generate_extra_output(${PROJECT_NAME} .srec srec)

# Show ROM/RAM usage information at the end of the build (optional).
arm_cortex_gcc_display_size(${PROJECT_NAME})

