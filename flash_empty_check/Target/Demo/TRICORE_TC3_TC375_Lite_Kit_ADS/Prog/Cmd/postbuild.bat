:: **************************************************************************************
:: Batch-file for adjusting the linker generated firmware S-record, such that it can be
:: programmed with the OpenBLT bootloader.
::
:: Usage:   postbuild.bat <firmware S-record>
::
:: Example: postbuild.bat demoprog_tc375.srec
::
:: Detailed description
:: --------------------
:: The linker generated S-record is not yet usable for a firmware update with the OpenBLT
:: bootloader for the following reasons:
::
:: 1) In scatters the firmware's program code over both cached and non-cached flash
::    regions.
:: 2) It includes the "boot mode header" (BMHD) tables for the user configuraton block
::    region.
::
:: The OpenBLT bootloader expects the program code to be located in just the non-cached
:: memory range in the S-record. Futhermore, when initially flashing the OpenBLT
:: bootloader, it already included the BMHD tables such that the bootloader always runs
:: first, after a poweron or software reset event. Therefore the data for the UCB region
:: needs to be removed.
::
:: This batch-file calls the "srec_cat" utility to move the cached program code into the
:: non-cached region and strips out the UCB data.
::
:: Reference
:: ---------
:: Memory map with flash regions for all Infineon AURIX TC3xxx devices:
:: - up to 16Mb program flash
:: - up to 1Mb data flash 0
:: - up to 128kb data flash 1
::
:: |--------------------------------|------------|------------|
:: | Name                           | Start      | End        |
:: |--------------------------------|------------|------------|
:: | Program flash (cached)         | 0x80000000 | 0x80FFFFFF |
:: |--------------------------------|------------|------------|
:: | Program flash (non-cached)     | 0xA0000000 | 0xA0FFFFFF |
:: |--------------------------------|------------|------------|
:: | Data flash 0                   | 0xAF000000 | 0xAF0FFFFF |
:: |--------------------------------|------------|------------|
:: | User configuration block (UCB) | 0xAF400000 | 0xAF405FFF |
:: |--------------------------------|------------|------------|
:: | Data flash 1                   | 0xAFC00000 | 0xAFC1FFFF |
:: |--------------------------------|------------|------------|
::
:: **************************************************************************************

:: Turn echo off.
@echo off

:: Obtain the parameters. The first one is the name of the original S-record that the
:: linker created.
set srec_artifact=%1


:: Make sure the parameters are not empty.
if [%1] == [] goto ERROR

:: Update the user.
@echo on
@echo.
@echo Running post-build operations on: %srec_artifact% 
@echo off

:: Perform cleanup in case the last run got aborted and these files are still around.
if exist "cached_moved_to_noncached.srec" del /Q /F  "cached_moved_to_noncached.srec"
if exist "noncached_and_data.srec" del /Q /F  "noncached_and_data.srec"

:: Create a temporary S-record with just cached program flash data and move it to where
:: the non-cached program flash data appears in the memory map. This means an offset of:
:: 0xA0000000 - 0x80000000 = 0x200000000.
"%~dp0srec_cat" %srec_artifact% -crop 0x80000000 0x81000000 -offset 0x20000000 -o cached_moved_to_noncached.srec -execution-start-address=0xA0000000

:: Create a temporary S-record with just non-cached program flash and data flash. This
:: strips out the UCB data.
"%~dp0srec_cat" %srec_artifact% -crop 0xA0000000 0xA1000000 0xAF000000 0xAF100000 0xAFC00000 0xAFC20000 -o noncached_and_data.srec -execution-start-address=0xA0000000

:: Now overlay the two temporary S-records to create the final S-record with cached
:: program flash moved to the non-cached region, and the UCB data removed. Overwrite the
:: original S-record.
"%~dp0srec_cat" cached_moved_to_noncached.srec noncached_and_data.srec -o %srec_artifact%

:: Perform cleanup of the temporary S-records.
if exist "cached_moved_to_noncached.srec" del /Q /F  "cached_moved_to_noncached.srec"
if exist "noncached_and_data.srec" del /Q /F  "noncached_and_data.srec"

:: Update the user.
@echo on
@echo Finished patching: %srec_artifact% 
@echo.
@echo off

:: All done.
goto EXIT

:ERROR
:: Display the error info.
@echo on
@echo [ERROR] Batch-file parameters are invalid.
@echo off

:EXIT
:: Turn echo on again.
@echo on
