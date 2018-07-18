:: Batch file to generate a reference manual from the comments in the source code, with
:: Doxygen. The following tools should be installed and added to the path:
:: - DoxyGen (doxygen.exe)
:: - Graphviz (dot.exe)
:: - MikTex (pdflatex.exe)
if exist ..\ReferenceManuals\RM_SeedNKey.pdf del ..\ReferenceManuals\RM_SeedNKey.pdf
doxygen.exe DoxyfileSeedNKey
call .\output\SeedNKey\latex\make.bat
call copy .\output\SeedNKey\latex\refman.pdf ..\ReferenceManuals\RM_SeedNKey.pdf

