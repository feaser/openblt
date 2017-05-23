#!/bin/bash          
# Run doxygen
doxygen DoxyfileHtmlBootCommander
# View the results in a webbrowser
xdg-open ./output/BootCommander/html/index.html
