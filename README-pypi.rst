Introduction
============

.. image:: https://readthedocs.org/projects/qbdi/badge/?version=stable
    :target: https://qbdi.readthedocs.io/en/stable/?badge=stable
    :alt: Documentation Status

.. image:: https://travis-ci.com/QBDI/QBDI.svg?branch=master
    :target: https://travis-ci.com/QBDI/QBDI

.. image:: https://ci.appveyor.com/api/projects/status/s2qvpu8k8yiau647/branch/master?svg=true
    :target: https://ci.appveyor.com/project/QBDI/qbdi/branch/master

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

Installation
============

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

- `Appveyor <https://ci.appveyor.com/project/QBDI/qbdi/branch/dev-next>`_ for Windows packages PyQBDI
- `Github Actions PyQBDI Linux <https://github.com/QBDI/QBDI/actions?query=workflow%3A%22PyQBDI+Linux+package%22+branch%3Adev-next>`_ for Linux PyQBDI
- `Github Actions PyQBDI OSX <https://github.com/QBDI/QBDI/actions?query=workflow%3A%22PyQBDI+OSX+package%22+branch%3Adev-next>`_ for OSX PyQBDI
