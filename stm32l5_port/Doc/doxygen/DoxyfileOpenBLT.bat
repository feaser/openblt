:: Batch file to generate a reference manual from the comments in the source code, with
:: Doxygen. The following tools should be installed and added to the path:
:: - DoxyGen (doxygen.exe)
:: - Graphviz (dot.exe)
:: - MikTex (pdflatex.exe)
if exist ..\ReferenceManuals\RM_OpenBLT.pdf del ..\ReferenceManuals\RM_OpenBLT.pdf
doxygen.exe DoxyfileOpenBLT
call .\output\OpenBLT\latex\make.bat
call copy .\output\OpenBLT\latex\refman.pdf ..\ReferenceManuals\RM_OpenBLT.pdf

