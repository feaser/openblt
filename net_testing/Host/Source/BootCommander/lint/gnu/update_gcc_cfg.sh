#!/usr/bin/env bash
#
# Bash script to update the lint configuration files for the installed version of GCC.
#

# Create temporary empty source-file
touch empty.c

# Generate header file with preprocessor configuration macros to use by lint.
gcc -E -dM empty.c >lint_cmac.h

# Generate lint configuration file with compiler include paths.
gcc -c -v empty.c 2>&1 | awk '/#include <...> search starts here:/{flag=1;next}/End of search list./{flag=0}flag' > gcc-include-path.lnt
sed -i 's# /#--i/#g' gcc-include-path.lnt

# Remove temporary empty source-file
rm -f empty.c
rm -f empty.o

