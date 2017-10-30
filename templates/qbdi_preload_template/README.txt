Introduction
============

A simple template to easily create an instrumentation tool based
on libQBDIPreload.


Compilation
===========

Linux / macOS
-------------

$ # unix
$ mkdir build
$ cd build
$ cmake ..
$ make

Usage
=====

macOS
-----

sudo DYLD_INSERT_LIBRARIES=./libqbdi_tracer.dylib ./mytarget

Linux
-----

LD_PRELOAD=./libqbdi_tracer.so ./mytarget
