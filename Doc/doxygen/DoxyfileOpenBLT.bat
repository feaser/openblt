:: Batch file to generate a reference manual from the comments in the source code, with
:: Doxygen. The following tools should be installed and added to the path:
:: - DoxyGen (doxygen.exe)
:: - Graphviz (dot.exe)
:: - MikTex (pdflatex.exe)
if exist ..\RM_OpenBLT.pdf del ..\RM_OpenBLT.pdf
if exist ..\RM_OpenBLT.chm del ..\RM_OpenBLT.chm
doxygen.exe DoxyfileOpenBLT
call .\output\OpenBLT\latex\make.bat
call copy .\output\OpenBLT\latex\refman.pdf ..\RM_OpenBLT.pdf

