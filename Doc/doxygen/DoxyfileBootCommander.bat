:: Batch file to generate a reference manual from the comments in the source code, with
:: Doxygen. The following tools should be installed and added to the path:
:: - DoxyGen (doxygen.exe)
:: - Graphviz (dot.exe)
:: - MikTex (pdflatex.exe)
:: - HTML Help Workshop compiler (hhc.exe)
if exist ..\RM_BootCommander.pdf del ..\RM_BootCommander.pdf
if exist ..\RM_BootCommander.chm del ..\RM_BootCommander.chm
doxygen.exe DoxyfileBootCommander
:: call .\output\BootCommander\latex\make.bat
:: call copy .\output\BootCommander\latex\refman.pdf ..\RM_BootCommander.pdf
:: Remove the comment on the next line to automatically open the generated PDF file
:: call start "C:\Program Files (x86)\Adobe\Reader 11.0\Reader\AcroRd32.exe" ..\RM_BootCommander.pdf
:: Remove the comment on the next line to automatically open the generated CHM file
:: start .\..\RM_BootCommander.chm
