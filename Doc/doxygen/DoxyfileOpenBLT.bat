:: Batch file to generate a reference manual from the comments in the source code, with
:: Doxygen. The following tools should be installed and added to the path:
:: - DoxyGen (doxygen.exe)
:: - Graphviz (dot.exe)
:: - MikTex (pdflatex.exe)
:: - HTML Help Workshop compiler (hhc.exe)
if exist ..\RM_OpenBLT.pdf del ..\RM_OpenBLT.pdf
if exist ..\RM_OpenBLT.chm del ..\RM_OpenBLT.chm
doxygen.exe DoxyfileOpenBLT
:: call .\output\OpenBLT\latex\make.bat
:: call copy .\output\OpenBLT\latex\refman.pdf ..\RM_OpenBLT.pdf
:: Remove the comment on the next line to automatically open the generated PDF file
:: call start "C:\Program Files (x86)\Adobe\Reader 11.0\Reader\AcroRd32.exe" ..\RM_OpenBLT.pdf
:: Remove the comment on the next line to automatically open the generated CHM file
:: start .\..\RM_OpenBLT.chm
