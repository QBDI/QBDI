Introduction
============

.. image:: https://readthedocs.org/projects/qbdi/badge/?version=stable
    :target: https://qbdi.readthedocs.io/en/stable/?badge=stable
    :alt: Documentation Status

.. image:: https://img.shields.io/github/v/release/QBDI/QBDI
    :target: https://github.com/QBDI/QBDI/releases

.. image:: https://img.shields.io/pypi/pyversions/PyQBDI
    :target: https://pypi.org/project/PyQBDI/

.. image:: https://img.shields.io/pypi/v/PyQBDI
    :target: https://pypi.org/project/PyQBDI/

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
ARM       Android, Linux                   :yellow:`Supported (*)`    :yellow:`Supported (*)`
AArch64   Android, Linux, macOS            :yellow:`Supported (*)`    :yellow:`Supported (*)`
=======   ==============================   ========================   =================================

\* The ARM and AArch64 instruction sets are supported but in early support.

Installation
============

Python API (PyQBDI)
+++++++++++++++++++

PyQBDI is available through PyPI. The wheel package can be either `downloaded <https://pypi.org/project/PyQBDI/#files>`__ or installed with the following command:

    pip install PyQBDI

The PyQBDI package is self-contained so completely independent from the C/C++ package.

Devel packages
++++++++++++++

There is no strict development timeline or scheduled release plan for the QBDI project.
All the new features and fixes are merged onto the ``dev-next`` branch.
Devel packages can be downloaded in the artefacts of `Github Actions <https://github.com/QBDI/QBDI/actions/workflows/python.yml?query=branch%3Adev-next>`__.

Compilation
===========

The PyQDBI library (apart from the wheel package) can be built by solely passing the **'-DQBDI_TOOLS_PYQBDI=ON'** option to the CMake build system.

However, if you want to build the wheel package, you can run these commands::

    git clone https://github.com/QBDI/QBDI.git
    python -m pip install --upgrade pip
    python -m pip install setuptools wheel build
    python -m build -w

A 32-bit version of Python is mandatory for the X86 architecture whereas a 64-bit one is required for the X86-64 architecture.
