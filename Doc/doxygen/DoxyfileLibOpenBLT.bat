:: Batch file to generate a reference manual from the comments in the source code, with
:: Doxygen. The following tools should be installed and added to the path:
:: - DoxyGen (doxygen.exe)
:: - Graphviz (dot.exe)
:: - MikTex (pdflatex.exe)
:: - HTML Help Workshop compiler (hhc.exe)
if exist ..\RM_LibOpenBLT.pdf del ..\RM_LibOpenBLT.pdf
if exist ..\RM_LibOpenBLT.chm del ..\RM_LibOpenBLT.chm
doxygen.exe DoxyfileLibOpenBLT
:: call .\output\LibOpenBLT\latex\make.bat
:: call copy .\output\LibOpenBLT\latex\refman.pdf ..\RM_LibOpenBLT.pdf
:: Remove the comment on the next line to automatically open the generated PDF file
:: call start "C:\Program Files (x86)\Adobe\Reader 11.0\Reader\AcroRd32.exe" ..\RM_LibOpenBLT.pdf
:: Remove the comment on the next line to automatically open the generated CHM file
:: start .\..\RM_LibOpenBLT.chm
