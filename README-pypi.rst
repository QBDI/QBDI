Introduction
============

.. image:: https://readthedocs.org/projects/qbdi/badge/?version=stable
    :target: https://qbdi.readthedocs.io/en/stable/?badge=stable
    :alt: Documentation Status

.. image:: https://travis-ci.com/QBDI/QBDI.svg?branch=master
    :target: https://travis-ci.com/QBDI/QBDI

.. image:: https://ci.appveyor.com/api/projects/status/s2qvpu8k8yiau647/branch/master?svg=true
    :target: https://ci.appveyor.com/project/QBDI/qbdi/branch/master

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
x86-64    Android, Linux, macOS, Windows   :green:`Supported`       :yellow:`Partial (only non SIMD)`
x86       Android, Linux, macOS, Windows   :green:`Supported`       :yellow:`Partial (only non SIMD)`
ARM       Linux, Android, iOS              :orange:`Planned (*)`    :orange:`Planned (*)`
AArch64   Android                          :orange:`Planned (*)`    :orange:`Planned (*)`
=======   ==============================   ======================   =================================

.. warning::

   The ARM and AArch64 instruction sets are supported but they still need to be integrated along with x86 and x86-64.


