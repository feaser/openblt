# Basic system confirmation and set the target type to a static library to skip the
# compile test, which would otherwise fail for a cross-compiler.
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Attempt to locate and configure compiler binaries. It this fails, add the location
# of your arm-none-eabi-gcc to your $PATH.
find_program(C_COMPILER arm-none-eabi-gcc)
find_program(CXX_COMPILER arm-none-eabi-g++)
find_program(AR arm-none-eabi-ar)
find_program(OBJCOPY arm-none-eabi-objcopy)
find_program(OBJDUMP arm-none-eabi-objdump)
find_program(SIZE arm-none-eabi-size)

# Map the found compiler binaries to the CMAKE variables.
set(CMAKE_ASM_COMPILER ${C_COMPILER})
set(CMAKE_C_COMPILER ${C_COMPILER})
set(CMAKE_CXX_COMPILER ${CXX_COMPILER})
set(CMAKE_AR ${AR})
set(CMAKE_OBJCOPY ${OBJCOPY})
set(CMAKE_OBJDUMP ${OBJDUMP})
set(CMAKE_SIZE ${SIZE})

# Set suffix to configure the extension of the output binary.
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")

# Set generic assember, compiler and linker flags.
set(CMAKE_ASM_FLAGS "-fdata-sections -ffunction-sections -x assembler-with-cpp")
set(CMAKE_C_FLAGS "-fdata-sections -ffunction-sections")
set(CMAKE_CXX_FLAGS "-fdata-sections -ffunction-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections")
# Set extra flags for the DEBUG build type.
set(CMAKE_C_FLAGS_DEBUG "-Og -g")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g")
# Set extra flags for the RELEASE build type.
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
# Set extra flags for the RELWITHDEBINFO build type.
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
# Set extra flags for the MINSIZEREL build type.
set(CMAKE_C_FLAGS_MINSIZEREL "-Os")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os")

# Utility function to generate an additional output file in the specified format.
# Example usage: 
#  arm_cortex_gcc_generate_extra_output(${PROJECT_NAME} .srec srec)
#  arm_cortex_gcc_generate_extra_output(${PROJECT_NAME} .hex ihex)
#  arm_cortex_gcc_generate_extra_output(${PROJECT_NAME} .bin binary)
function(arm_cortex_gcc_generate_extra_output TARGET suffix type)
    add_custom_command(
      TARGET ${TARGET}
      POST_BUILD
      COMMAND ${CMAKE_OBJCOPY} -O ${type} ${TARGET}${CMAKE_EXECUTABLE_SUFFIX_C} ${TARGET}${suffix}
      BYPRODUCTS ${TARGET}${suffix}
      COMMENT "Generating ${type} file ${CMAKE_PROJECT_NAME}${suffix}"
    )
endfunction()

# Utility function to display ROM/RAM size information.
# Example usage: 
#  arm_cortex_gcc_display_size(${PROJECT_NAME})
function(arm_cortex_gcc_display_size TARGET)
    add_custom_command(
        TARGET ${TARGET}
        POST_BUILD
        COMMAND ${CMAKE_SIZE} ${TARGET}${CMAKE_EXECUTABLE_SUFFIX_C}
        COMMENT "Size information: "
    )
endfunction()


# Utility function to generate a MAP file.
# Example usage: 
#  arm_cortex_gcc_generate_map(${PROJECT_NAME})
function(arm_cortex_gcc_generate_map TARGET)
    target_link_options(${TARGET} PRIVATE
        -Wl,-Map=${TARGET}.map
    )
    set_property(
        TARGET ${TARGET}
        APPEND
        PROPERTY ADDITIONAL_CLEAN_FILES ${TARGET}.map
    )
endfunction()




