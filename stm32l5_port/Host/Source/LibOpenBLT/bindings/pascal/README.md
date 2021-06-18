### Pascal wrapper for LibOpenBLT
The OpenBLT Host Library ([LibOpenBLT](https://www.feaser.com/openblt/doku.php?id=manual:libopenblt)) contains an application programming interface (API) for communicating with a microcontroller target, running the OpenBLT bootloader, for making firmware updates on the target. The goal of the OpenBLT Host Library is to empower you to quickly and efficiently create your own firmware update tool, in case you don’t prefer the standard MicroBoot and BootCommander tools that are included in the OpenBLT bootloader package.

This directory contains the Pascal wrapper for LibOpenBLT, which enables you to quickly develop your own firmware update tool in the Pascal programming language, for example with the [Lazarus IDE](https://www.lazarus-ide.org/) or [Delphi](https://www.embarcadero.com/products/delphi).

### Using the wrapper
To use the Pascal wrapper for LibOpenBLT, copy the file **openblt.pas** from this directory to where you have the sources of your Lazarus or Delphi project. Next, add **OpenBlt** to your unit's **uses** section. Now you can call all the **BltXxx** functions and procedures of LibOpenBLT.

Here is a minimal Pascal console program that demonstrates how to display the version of LibOpenBLT:

```pascal
program MyBootTool;

uses OpenBlt;

begin
  writeln('LibOpenBLT version: ' + BltVersionGetString);
end.
```

### Run-time libraries
Copy the LibOpenBLT related run-time libraries into the directory where your program's executable is located. Refer to the following section on the OpenBLT Wiki for a overview of these run-time libraries:

https://www.feaser.com/openblt/doku.php?id=manual:libopenblt#run-time_libraries

These run-time libraries can be found in the ./Host directory of the OpenBLT bootloader package. These run-time libraries should also be included, when distributing your program.

### Lazarus and Delphi specifics on Windows

Under Microsoft Windows, the LibOpenBLT shared library (**libopenblt.dll**) is 32-bit. This means that the application you develop needs to build as a 32-bit application as well. 

When using Lazarus, an easy solution is to install the 32-bit version of Lazarus. That way your own application automatically builds as a 32-bit application as well. If you already installed the 64-bit version of Lazarus, then you can go to the *Project Options* and set **Target OS** to **Win32** and **Target CPU Family** to **i386**.

When using Delphi, go to the *Project Manager* and configure just **32-bit Windows platform (Win32)** for setting **Target Platforms**.

### Lazarus specifics on Linux
When running your program under Linux, you will most likely get the following error:
```
error while loading shared libraries: libopenblt.so: cannot open shared object file.
```
As opposed to Windows, Linux does not add the directory that your program is running from to the search path (LD_LIBARY_PATH). This is the reason why it cannot locate the LibOpenBLT run-time libraries.

This can easily be solved by adding the program's directory to the search path in the settings of your Lazarus project.

- From the Lazarus menu select: **Project -> Project Options..**
- Go to the section **Compiler Options -> Compilation and Linking**
- Check the checkbox **Pass options to linker with "-k", delimiter is space**
- Enter the following in the edit box underneath the checkbox:
```
-R ./
```
