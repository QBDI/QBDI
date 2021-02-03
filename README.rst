.. image:: https://readthedocs.org/projects/qbdi/badge/?version=stable
    :target: https://qbdi.readthedocs.io/en/stable/?badge=stable
    :alt: Documentation Status

.. image:: https://travis-ci.com/QBDI/QBDI.svg?branch=master
    :target: https://travis-ci.com/QBDI/QBDI

.. image:: https://ci.appveyor.com/api/projects/status/s2qvpu8k8yiau647/branch/master?svg=true
    :target: https://ci.appveyor.com/project/QBDI/qbdi/branch/master

.. image:: https://img.shields.io/github/v/release/QBDI/QBDI
    :target: https://github.com/QBDI/QBDI/releases

.. image:: https://img.shields.io/pypi/pyversions/PyQBDI
    :target: https://pypi.org/project/PyQBDI/

.. image:: https://img.shields.io/pypi/v/PyQBDI
    :target: https://pypi.org/project/PyQBDI/

Introduction
============
.. intro

QuarkslaB Dynamic binary Instrumentation (QBDI) is a modular, cross-platform and cross-architecture
DBI framework. It aims to support Linux, macOS, Android, iOS and Windows operating systems running on
x86, x86-64, ARM and AArch64 architectures. Information about what is a DBI framework and how QBDI
works can be found in the user documentation introduction (`User Documentation <https://qbdi.readthedocs.io/en/stable/user.html>`_).

QBDI modularity means it doesn't contain a preferred injection method and it is designed to be
used in conjunction with an external injection tool. QBDI includes a tiny (``LD_PRELOAD`` based)
Linux and macOS injector for dynamic executables (QBDIPreload), which acts as the foundation for our
Python bindings (pyQBDI).
QBDI is also fully integrated with `Frida <https://frida.re>`_, a reference dynamic instrumentation toolkit,
allowing anybody to use their combined powers.

x86-64 support is mature (even if SIMD memory access are not yet reported). The support of x86
is new and some bug may occur. ARM architecture is
a work in progress but already sufficient to execute simple CLI program like *ls* or *cat*.
AArch64 is planned, but currently unsupported.

A current limitation is that QBDI doesn't handle signals, multithreading (it doesn't deal with new
threads creation) and C++ exception mechanisms.
However, those system-dependent features will probably not be part of the core library (KISS),
and should be integrated as a new layer (to be determined how).

Status
------

.. role:: green
.. role:: yellow
.. role:: orange
.. role:: red

=======   ==============================   ======================   =================================
CPU       Operating Systems                Execution                Memory Access Information
=======   ==============================   ======================   =================================
x86-64    Android, Linux, macOS, Windows   :green:`Supported`       :green:`Supported`
x86       Android, Linux, macOS, Windows   :green:`Supported`       :green:`Supported`
ARM       Linux, Android, iOS              :orange:`Planned (*)`    :orange:`Planned (*)`
AArch64   Android                          :orange:`Planned (*)`    :orange:`Planned (*)`
=======   ==============================   ======================   =================================

.. warning::

   The ARM and AArch64 instruction sets are supported but they still need to be integrated along with x86 and x86-64.


**stable**

.. image:: https://readthedocs.org/projects/qbdi/badge/?version=stable
    :target: https://qbdi.readthedocs.io/en/stable/?badge=stable
    :alt: Documentation Status

.. image:: https://travis-ci.com/QBDI/QBDI.svg?branch=master
    :target: https://travis-ci.com/QBDI/QBDI

.. image:: https://ci.appveyor.com/api/projects/status/s2qvpu8k8yiau647/branch/master?svg=true
    :target: https://ci.appveyor.com/project/QBDI/qbdi/branch/master

.. image:: https://github.com/QBDI/QBDI/workflows/Tests%20and%20Package%20Linux/badge.svg?branch=master
    :target: https://github.com/QBDI/QBDI/actions?query=workflow%3A%22Tests+and+Package+Linux%22+branch%3Amaster

.. image:: https://github.com/QBDI/QBDI/workflows/Tests%20and%20Package%20OSX/badge.svg?branch=master
    :target: https://github.com/QBDI/QBDI/actions?query=workflow%3A%22Tests+and+Package+OSX%22+branch%3Amaster

**dev**

.. image:: https://readthedocs.org/projects/qbdi/badge/?version=dev-next
    :target: https://qbdi.readthedocs.io/en/stable/?badge=dev-next
    :alt: Documentation Status

.. image:: https://travis-ci.com/QBDI/QBDI.svg?branch=dev-next
    :target: https://travis-ci.com/QBDI/QBDI/branches

.. image:: https://ci.appveyor.com/api/projects/status/s2qvpu8k8yiau647/branch/dev-next?svg=true
    :target: https://ci.appveyor.com/project/QBDI/qbdi/branch/dev-next

.. image:: https://github.com/QBDI/QBDI/workflows/Tests%20and%20Package%20Linux/badge.svg?branch=dev-next
    :target: https://github.com/QBDI/QBDI/actions?query=workflow%3A%22Tests+and+Package+Linux%22+branch%3Adev-next

.. image:: https://github.com/QBDI/QBDI/workflows/Tests%20and%20Package%20OSX/badge.svg?branch=dev-next
    :target: https://github.com/QBDI/QBDI/actions?query=workflow%3A%22Tests+and+Package+OSX%22+branch%3Adev-next

.. intro-end

Installation
============

C/C++/Frida API
---------------

Each new version of QBDI is compiled and available on Github release page (https://github.com/QBDI/QBDI/releases).

Python API (PyQBDI)
-------------------

PyQBDI is available on Pypi. The wheel package can be downloaded (https://pypi.org/project/PyQBDI/#files) or installed with pip command.::

    pip install PyQBDI

PyQBDI package is independent of C/C++ package.

Compilation
===========
.. compil

To build this project the following dependencies are needed on your system: cmake, make (for Linux
and macOS), ninja (for Android), Visual Studio (for Windows) and a C++11 toolchain for the platform of
your choice.

The compilation is a two-step process:

* A local binary distribution of the dependencies is built.
* QBDI is built using those binaries.

The current dependencies which need to be built are LLVM and Google Test. This local built of
LLVM is required because QBDI uses private APIs not exported by regular LLVM installations and
because our code is only compatible with a specific version of those APIs. This first step is
cached and only needs to be run once, subsequent builds only need to repeat the second step.

QBDI build system relies on CMake and requires to pass build configuration flags. To help with
this step we provide shell scripts for common build configurations which follow the naming pattern
``config-OS-ARCH.sh``. Modifying these scripts is necessary if you want to compile in debug or
cross compile QBDI.

Linux
-----

x86-64
^^^^^^

Create a new directory at the root of the source tree, and execute the Linux configuration script::

    mkdir build
    cd build
    ../cmake/config/config-linux-X86_64.sh

If the build script warns you of missing dependencies for your platform (in the case of a first
compilation), or if you want to rebuild them, execute the following commands::

    make llvm

This will rebuild the binary distribution of those dependencies for your platform. You can
then relaunch the configuration script from above and compile::

    ../cmake/config/config-linux-X86_64.sh
    make -j4

x86
^^^

The previous step can be follow but using the ``config-linux-X86.sh`` configuration script instead.

macOS
-----

Compiling QBDI on macOS requires a few things:

* A modern version of **macOS** (like Sierra)
* **Xcode** (from the *App Store* or *Apple Developer Tools*)
* the **Command Line Tools** (``xcode-select --install``)
* a package manager (preferably **MacPorts**, but *HomeBrew* should also be fine)
* some packages (``port install cmake wget``)

Once requirements are met, create a new directory at the root of the source tree, and execute the macOS configuration script::

    mkdir build
    cd build
    ../cmake/config/config-macOS-X86_64.sh

If the build script warns you of missing dependencies for your platform (in the case of a first
compilation), or if you want to rebuild them, execute the following commands::

    make llvm


This will rebuild the binary distribution of those dependencies for your platform. You can
then relaunch the build script from above and compile::

    ../cmake/config/config-macOS-X86_64.sh
    make -j4

Windows
-------

Building on Windows requires a pure Windows installation of *Python 3*
(from the official packages, this is mandatory) in order to build our dependencies
(we really hope to improve this in the future).
It also requires an up-to-date CMake and Ninja.

First, the Visual Studio environnment must be setup. This is usually achived with a command like::

    "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64


Then the following command should be run::

    mkdir build
    cd build
    python ../cmake/config/config-win-X86_64.py

If the build script warns you of missing dependencies for your platform (in the case of a first
compilation), or if you want to rebuild them, execute the following commands::

    ninja llvm

This will rebuild the binary distribution of those dependencies for your platform. You can
then relaunch the build script from above and compile::

    python ../cmake/config/config-win-X86_64.py
    ninja

Android
-------

Cross-compiling for Android requires the Android NDK and has only been tested under Linux. The
``config-android-X86-64.sh`` configuration script should be customized to match your NDK installation
and target platform:

* ``NDK_PATH`` should point to your Android NDK

From that point on the Linux guide can be followed using this configuration script.

About ARM support
=================

QBDI have some support for ARM achitecture up to version `0.6.2 <https://github.com/QBDI/QBDI/releases/tag/v0.6.2>`_.
The ARM architecture hasn't been tested with more recent release and is now depreciated.

.. compil-end
