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
x86, x86-64, ARM and AArch64 architectures. In addition of C/C++ API, Python and JS/frida bindings are
available to script QBDI. Information about what is a DBI framework and how QBDI
works can be found in the `documentation introduction <https://qbdi.readthedocs.io/en/stable/intro.html>`_.

QBDI modularity means it doesn't contain a preferred injection method and it is designed to be
used in conjunction with an external injection tool. QBDI includes a tiny (``LD_PRELOAD`` based)
Linux and macOS injector for dynamic executables (QBDIPreload).
QBDI is also fully integrated with `Frida <https://frida.re>`_, a reference dynamic instrumentation toolkit,
allowing anybody to use their combined powers.

A current limitation is that QBDI doesn't handle signals, multithreading (it doesn't deal with new
threads creation) and C++ exception mechanisms.
However, those system-dependent features will probably not be part of the core library (KISS),
and should be integrated as a new layer (to be determined how).

Status
++++++

.. role:: green
.. role:: yellow
.. role:: orange
.. role:: red

=======   ==============================   ========================   =================================
CPU       Operating Systems                Execution                  Memory Access Information
=======   ==============================   ========================   =================================
x86-64    Android, Linux, macOS, Windows   :green:`Supported`         :green:`Supported`
x86       Android, Linux, macOS, Windows   :green:`Supported`         :green:`Supported`
ARM       Linux, Android, iOS              :orange:`Planned (*)`      :orange:`Planned (*)`
AArch64   Android, Linux, macOS            :orange:`Supported (*)`    :orange:`Supported (*)`
=======   ==============================   ========================   =================================

\* The ARM and AArch64 instruction sets are supported but not public. Contact us if you want to try them.

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

C/C++/Frida APIs
++++++++++++++++

Every new QBDI version is compiled and made available on the Github `release page <https://github.com/QBDI/QBDI/releases>`_.

Python API (PyQBDI)
+++++++++++++++++++

PyQBDI is available through PyPI. The wheel package can be either `downloaded <https://pypi.org/project/PyQBDI/#files>`_ or installed with the following command:

    pip install PyQBDI

The PyQBDI package is self-contained so completely independent from the C/C++ package.

Devel packages
++++++++++++++

There is no strict development timeline or scheduled release plan for the QBDI project.
All the new features and fixes are merged onto the ``dev-next`` branch.
Devel packages can be downloaded in the artefacts of:

- `Appveyor <https://ci.appveyor.com/project/QBDI/qbdi/branch/dev-next>`_ for windows packages (C/C++ API and PyQBDI)
- `Github Actions <https://github.com/QBDI/QBDI/actions?query=workflow%3A%22Tests+and+Package+Linux%22+branch%3Adev-next>`_ for Linux C/C++/frida API (based on ubuntu)
- `Github Actions <https://github.com/QBDI/QBDI/actions?query=workflow%3A%22Tests+and+Package+OSX%22+branch%3Adev-next>`_ for OSX C/C++/frida API
- `Github Actions <https://github.com/QBDI/QBDI/actions?query=workflow%3A%22Package+Android%22+branch%3Adev-next>`_ for android C/C++/frida API
- `Github Actions <https://github.com/QBDI/QBDI/actions?query=workflow%3A%22PyQBDI+Linux+package%22+branch%3Adev-next>`_ for Linux PyQBDI
- `Github Actions <https://github.com/QBDI/QBDI/actions?query=workflow%3A%22PyQBDI+OSX+package%22+branch%3Adev-next>`_ for OSX PyQBDI

Compilation
===========
.. compil

To build this project, the following dependencies are needed on your system:

- cmake >= 3.12
- ninja or make
- C++17 toolchain (gcc, clang, Visual Studio 2019, ...)

The compilation is a two-step process:

* local library distribution of LLVM is built.
* QBDI is built using the LLVM library.

This local built of LLVM is required because QBDI uses private APIs not exported by regular LLVM installations and
because our code is only compatible with a specific version of those APIs. This first step is
cached and only needs to be run once, subsequent builds only need to repeat the second step.

QBDI build system relies on CMake and requires to pass build configuration flags. To help with
this step we provide shell scripts for common build configurations which follow the naming pattern
``config-OS-ARCH.sh``. Modifying these scripts is necessary if you want to compile in debug mode or
cross-compile QBDI.

Linux
+++++

x86-64
^^^^^^

Create a new directory at the root of the source tree, and execute the Linux configuration script::

    mkdir build
    cd build
    ../cmake/config/config-linux-X86_64.sh
    ninja

x86
^^^

You can follow the same instructions as for x86-64 but instead, use the ``config-linux-X86.sh`` configuration script.

macOS
+++++

Compiling QBDI on macOS requires a few things:

* A modern version of **macOS** (like Sierra)
* **Xcode** (from the *App Store* or *Apple Developer Tools*)
* the **Command Line Tools** (``xcode-select --install``)
* a package manager (preferably **MacPorts**, but *HomeBrew* should also be fine)
* some packages (``port install cmake wget ninja``)

Once requirements are met, create a new directory at the root of the source tree, and execute the macOS configuration script::

    mkdir build
    cd build
    ../cmake/config/config-macOS-X86_64.sh
    ninja

Windows
+++++++

Building on Windows requires a pure Windows installation of *Python 3*
(from the official packages, this is mandatory) in order to build our dependencies
(we really hope to improve this in the future).
It also requires an up-to-date CMake and Ninja.

First of all, the Visual Studio environment must be set up. This can be done with a command such as::

    "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64


Then, the following commands must be run::

    mkdir build
    cd build
    python ../cmake/config/config-win-X86_64.py
    ninja

Android
+++++++

Cross-compiling for Android requires the NDK to be installed on your workstation. For now, it has only been tested under Linux.
If not already installed, you can download the latest Android NDK package through the `official website <https://developer.android.com/ndk/downloads>`_.
Afterwards, the ``config-android-*.sh`` configuration script needs to be customised to match your NDK installation
directory and the target platform:

* ``NDK_PATH`` should point to your Android NDK

At this point, you should be able to continue following the instructions of the Linux section since the procedure is the same.

PyQBDI compilation
++++++++++++++++++

The PyQDBI library (apart from the wheel package) can be built by solely passing the **'-DQBDI_TOOLS_PYQBDI=ON'** option to the CMake build system.

However, if you want to build the wheel package, you have to compile the LLVM libraries beforehand.
Once done, you can run these commands::

    python -m pip install --upgrade pip
    python -m pip install setuptools wheel
    python setup.py bdist_wheel

A 32-bit version of Python is mandatory for the X86 architecture whereas a 64-bit one is required for the X86-64 architecture.

.. compil-end

About the ARM support
=====================

QBDI supports the ARM architecture up to its `0.6.2 <https://github.com/QBDI/QBDI/releases/tag/v0.6.2>`_ version.
Unfortunately, the ARM architecture hasn't been recently tested so is now marked as deprecated.
