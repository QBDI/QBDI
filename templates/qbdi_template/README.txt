Introduction
============

A simple template to easily create an instrumentation tool
linking QBDI at compile time.


Compilation
===========

Linux / macOS
-------------

$ # unix
$ mkdir build
$ cd build
$ cmake ..
$ make


Windows
-------

$ # win
$ mkdir build
$ cd build
$ cmake -G "Visual Studio 12 2013 Win64" -DCMAKE_BUILD_TYPE=Release ..
$ MSBuild.exe /p:Configuration=Release ALL_BUILD.vcxproj

If compilation is successful, test binary will be in Release directory.
If compiled dynamically, it will need QBDI.dll (from lib directory).

Note 1: Visual Studio version must match the one installed on your system.
Note 2: under cygwin, QBDI.dll needs to have executable bit set.
