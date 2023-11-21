Python wrapper for LibOpenBLT
=============================
The OpenBLT Host Library (`LibOpenBLT <https://www.feaser.com/openblt/doku.php?id=manual:libopenblt>`_) contains an application programming interface (API) for communicating with a microcontroller target, running the OpenBLT bootloader, for making firmware updates on the target. The goal of the OpenBLT Host Library is to empower you to quickly and efficiently create your own firmware update tool, in case you don’t prefer the standard MicroBoot and BootCommander tools that are included in the OpenBLT bootloader package.

This package contains the python wrapper for LibOpenBLT, which enables you to quickly develop your own firmware update tool in the Python programming language.

Install the package
-------------------
Run the following command from the directory that contains the **setup.py** file (assuming setuptools is installed):

::

    python setup.py install

Alternatively, you can use **pip** by running this command from the directory that contains the **setup.py** file:

::

    pip install .


Using the package
-----------------
Basic code snippet to call the BltVersionGetString()-function which displays the version of LibOpenBLT:

::

    import openblt

    print('LibOpenBLT version:', openblt.version_get_string())

Have a look at the function headers inside openblt.lib for details on how to call the functions, including examples. A video tutorial about getting started with the Python bindings is available in this `blog article <https://www.feaser.com/en/blog/?p=208>`_.


Run-time libraries
------------------
Copy the LibOpenBLT related run-time libraries into your python program's directory. Refer to the following section on the OpenBLT Wiki for a overview of these run-time libraries:

https://www.feaser.com/openblt/doku.php?id=manual:libopenblt#run-time_libraries.

These run-time libraries can be found in the ./Host directory of the OpenBLT bootloader package. These run-time libraries should also be included, when distributing your program.

Specific on Windows
------------------
Under Microsoft Windows, the LibOpenBLT shared library (libopenblt.dll) is 64-bit ever since OpenBLT version 1.14. Therefore you need to run your Python application, that makes use of LibOpenBLT, using the 64-bit Python interpreter.

If you use the LibOpenBLT shared library from before OpenBLT version 1.14 or if you rebuilt it yourself as 32-bit, you need to run your Python application using the 32-bit Python interpreter.
