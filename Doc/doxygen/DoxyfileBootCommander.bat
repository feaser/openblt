:: Batch file to generate a reference manual from the comments in the source code, with
:: Doxygen. The following tools should be installed and added to the path:
:: - DoxyGen (doxygen.exe)
:: - Graphviz (dot.exe)
:: - MikTex (pdflatex.exe)
if exist ..\RM_BootCommander.pdf del ..\RM_BootCommander.pdf
if exist ..\RM_BootCommander.chm del ..\RM_BootCommander.chm
doxygen.exe DoxyfileBootCommander
call .\output\BootCommander\latex\make.bat
call copy .\output\BootCommander\latex\refman.pdf ..\RM_BootCommander.pdf

