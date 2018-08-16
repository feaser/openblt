:: Batch file to generate a reference manual from the comments in the source code, with
:: Doxygen. The following tools should be installed and added to the path:
:: - DoxyGen (doxygen.exe)
:: - Graphviz (dot.exe)
:: - MikTex (pdflatex.exe)
if exist ..\ReferenceManuals\RM_LibOpenBLT.pdf del ..\ReferenceManuals\RM_LibOpenBLT.pdf
doxygen.exe DoxyfileLibOpenBLT
call .\output\LibOpenBLT\latex\make.bat
call copy .\output\LibOpenBLT\latex\refman.pdf ..\ReferenceManuals\RM_LibOpenBLT.pdf

