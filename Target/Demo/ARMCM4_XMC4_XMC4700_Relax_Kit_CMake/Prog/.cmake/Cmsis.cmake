# Create interface library for the Infineon XMC4 CMSIS library.
add_library(infineon-xmc4-cmsis INTERFACE)

target_include_directories(infineon-xmc4-cmsis INTERFACE 
    "${CMAKE_SOURCE_DIR}/lib/cmsis"
)
