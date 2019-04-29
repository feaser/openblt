# This file contains functions and configurations for generating PC-Lint build
# targets for your CMake projects.

set(PC_LINT_EXECUTABLE "C:/Lint/lint-nt.exe" CACHE STRING "full path to the pc-lint executable. NOT the generated lin.bat")
set(PC_LINT_CONFIG_DIR "${PROJECT_SOURCE_DIR}/lint/msvc" CACHE STRING "full path to the directory containing pc-lint configuration files")
set(PC_LINT_USER_FLAGS "-b" CACHE STRING "additional pc-lint command line options -- some flags of pc-lint cannot be set in option files (most notably -b)")

# a phony target which causes all available *_LINT targets to be executed
add_custom_target(ALL_LINT)

# add_pc_lint(target source1 [source2 ...])
#
# Takes a list of source files and generates a build target which can be used
# for linting all files
#
# The generated lint commands assume that a top-level config file named
# 'std.lnt' resides in the configuration directory 'PC_LINT_CONFIG_DIR'. This
# config file must include all other config files. This is standard lint
# behaviour.
#
# Parameters:
#  - target: the name of the target to which the sources belong. You will get a
#            new build target named ${target}_LINT
#  - source1 ... : a list of source files to be linted. Just pass the same list
#            as you passed for add_executable or add_library. Everything except
#            C and CPP files (*.c, *.cpp, *.cxx) will be filtered out.
#
# Example:
#  If you have a CMakeLists.txt which generates an executable like this:
#
#    set(MAIN_SOURCES main.c foo.c bar.c)
#    add_executable(main ${MAIN_SOURCES})
#
#  include this file
#
#    include(/path/to/pc_lint.cmake)
#
#  and add a line to generate the main_LINT target
#
#   if(COMMAND add_pc_lint)
#    add_pc_lint(main ${MAIN_SOURCES})
#   endif(COMMAND add_pc_lint)
#
function(add_pc_lint target)
    get_directory_property(lint_include_directories INCLUDE_DIRECTORIES)
    get_directory_property(lint_defines COMPILE_DEFINITIONS)

    # let's get those elephants across the alps
    # prepend each include directory with "-i"; also quotes the directory
    set(lint_include_directories_transformed)
    foreach(include_dir ${lint_include_directories})
        list(APPEND lint_include_directories_transformed -i"${include_dir}")
    endforeach(include_dir)

    # prepend each definition with "-d"
    set(lint_defines_transformed)
    foreach(definition ${lint_defines})
        list(APPEND lint_defines_transformed -d${definition})
    endforeach(definition)
        
    # list of all commands, one for each given source file
    set(pc_lint_commands)

    foreach(sourcefile ${ARGN})
        # only include c and cpp files
        if( sourcefile MATCHES \\.c$|\\.cxx$|\\.cpp$ )
            # make filename absolute
            get_filename_component(sourcefile_abs ${sourcefile} ABSOLUTE)
            # create command line for linting one source file and add it to the list of commands
            list(APPEND pc_lint_commands
                COMMAND ${PC_LINT_EXECUTABLE}
                -i"${PC_LINT_CONFIG_DIR}" std.lnt
                "-u" ${PC_LINT_USER_FLAGS}
                ${lint_include_directories_transformed}
                ${lint_defines_transformed}
                ${sourcefile_abs})
        endif()
    endforeach(sourcefile)

    # add a custom target consisting of all the commands generated above
    add_custom_target(${target}_LINT ${pc_lint_commands} VERBATIM)
    # make the ALL_LINT target depend on each and every *_LINT target
    add_dependencies(ALL_LINT ${target}_LINT)

endfunction(add_pc_lint) 
