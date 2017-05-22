#!/bin/bash          
# Run doxygen
doxygen DoxyfileHtmlLibOpenBLT
# View the results in a webbrowser
xdg-open ./output/LibOpenBLT/html/index.html
