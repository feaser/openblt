:: Batch file to generate a reference manual from the comments in the source code, with
:: Doxygen. The following tools should be installed and added to the path:
:: - DoxyGen (doxygen.exe)
:: - Graphviz (dot.exe)
:: - MikTex (pdflatex.exe)
if exist ..\ReferenceManuals\RM_BootCommander.pdf del ..\ReferenceManuals\RM_BootCommander.pdf
doxygen.exe DoxyfileBootCommander
call .\output\BootCommander\latex\make.bat
call copy .\output\BootCommander\latex\refman.pdf ..\ReferenceManuals\RM_BootCommander.pdf

