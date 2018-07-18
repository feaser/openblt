#!/bin/bash

# Shell script to generate a reference manual from the comments in the source code, with
# Doxygen. The following tools should be installed and accessible on the default path:
# - Doxygen
# - Graphviz
# - Make
# - Texlive-Latex
#
# On Debian based systems: sudo apt install make doxygen graphviz texlive-latex-base 
#                                           texlive-latex-extra
# On openSUSE based systems: sudo zypper install make doxygen graphviz texlive-latex 
#                                        texlive-tabu texlive-tocloft
rm -rf ../ReferenceManuals/RM_LibOpenBLT.pdf
doxygen DoxyfileLibOpenBLT
cd output/LibOpenBLT/latex/
make
cp refman.pdf ../../../../ReferenceManuals/RM_LibOpenBLT.pdf
cd ../../../
