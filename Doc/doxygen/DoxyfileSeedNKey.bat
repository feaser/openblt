:: Batch file to generate a reference manual from the comments in the source code, with
:: Doxygen. The following tools should be installed and added to the path:
:: - DoxyGen (doxygen.exe)
:: - Graphviz (dot.exe)
:: - MikTex (pdflatex.exe)
:: - HTML Help Workshop compiler (hhc.exe)
if exist ..\RM_SeedNKey.pdf del ..\RM_SeedNKey.pdf
if exist ..\RM_SeedNKey.chm del ..\RM_SeedNKey.chm
doxygen.exe DoxyfileSeedNKey
:: call .\output\SeedNKey\latex\make.bat
:: call copy .\output\SeedNKey\latex\refman.pdf ..\RM_SeedNKey.pdf
:: Remove the comment on the next line to automatically open the generated PDF file
:: call start "C:\Program Files (x86)\Adobe\Reader 11.0\Reader\AcroRd32.exe" ..\RM_SeedNKey.pdf
:: Remove the comment on the next line to automatically open the generated CHM file
:: start .\..\RM_SeedNKey.chm
