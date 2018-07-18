:: Batch file to generate a reference manual from the comments in the source code, with
:: Doxygen. The following tools should be installed and added to the path:
:: - DoxyGen (doxygen.exe)
:: - Graphviz (dot.exe)
:: - MikTex (pdflatex.exe)
if exist ..\RM_LibOpenBLT.pdf del ..\RM_LibOpenBLT.pdf
if exist ..\RM_LibOpenBLT.chm del ..\RM_LibOpenBLT.chm
doxygen.exe DoxyfileLibOpenBLT
call .\output\LibOpenBLT\latex\make.bat
call copy .\output\LibOpenBLT\latex\refman.pdf ..\RM_LibOpenBLT.pdf

