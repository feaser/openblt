Python wrapper for LibOpenBLT
=============================
The OpenBLT Host Library (LibOpenBLT) contains an application programming interface (API) for communicating with a microcontroller target, running the OpenBLT bootloader, for making firmware updates on the target. The goal of the OpenBLT Host Library is to empower you to quickly and efficiently create your own firmware update tool, in case you don’t prefer the standard MicroBoot and BootCommander tools that are included in the OpenBLT bootloader package.

This package contains the python wrapper for LibOpenBLT, which enables you to quickly develop your own firmware update tool in the Python programming language.

Install the package
-----------------
Run the following command from the directory that contains the **setup.py** file:

::

    python setup.py install


Using the package
-----------------
Basic code snippet to call the BltVersionGetNumber()-function which displays the version of LibOpenBLT:

::

    import openblt

    print('LibOpenBLT version:', openblt.lib.BltVersionGetNumber())


Run-time libraries
------------------
Copy the LibOpenBLT related run-time libraries into your python program's directory. Refer to the following section on the OpenBLT Wiki for a overview of these run-time libraries:

https://www.feaser.com/openblt/doku.php?id=manual:libopenblt#run-time_libraries.

These run-time libraries can be found in the .\Host directory of the OpenBLT bootloader package. These run-time libraries should also be included, when distributing your program.
