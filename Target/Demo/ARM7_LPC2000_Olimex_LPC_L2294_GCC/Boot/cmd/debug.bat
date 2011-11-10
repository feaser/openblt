@echo off
REM Establishes a JTAG connection with the CPU so that a GDB or Insight
REM debugging session can be started. Settings are for Remote/TCP
REM with hostname localhost on port 3333.
"C:\Program Files (x86)\OpenOCD\0.4.0\bin\openocd.exe" -f debug.cfg