
Compilation From Source
=======================

.. include:: ../../README.rst
    :start-after: .. compil
    :end-before: .. compil-end

CMake Parameters
++++++++++++++++

The compilation of QBDI can be configured with the command line. Each parameter
should be placed on the command line with the form ``-D<param>=<value>``.

* ``QBDI_PLATFORM`` (mandatory) : Target platform of the compilation
  (supported: ``windows``, ``linux``, ``android``, ``osx``)
* ``QBDI_ARCH`` (mandatory) : Target architecture of the compilation
  (supported: ``X86_64``, ``X86``)
* ``QBDI_CCACHE`` (default ON) : enable compilation optimisation with ccache or sccache.
* ``QBDI_DISABLE_AVX`` (default OFF) : disable the support of AVX instruction
  on X86 and X86_64
* ``QBDI_ASAN`` (default OFF) : compile with ASAN to detect memory leak in QBDI.
* ``QBDI_LOG_DEBUG`` (default OFF) : enable the debug level of the logging
  system. Note that the support of this level has an impact on the performances,
  even if this log level is not enabled.
* ``QBDI_STATIC_LIBRARY`` (default ON) : build the static library of QBDI. Note
  than some subproject need ``QBDI_STATIC_LIBRARY`` (test, PyQBDI, ...)
* ``QBDI_SHARED_LIBRARY`` (default ON) : build the shared library of QBDI. Note
  than some subproject need ``QBDI_SHARED_LIBRARY`` (Frida/QBDI, examples, ...)
* ``QBDI_TEST`` (default ON) : build the tests suite
* ``QBDI_BENCHMARK`` (default OFF) : build the benchmark tools
* ``QBDI_TOOLS_QBDIPRELOAD`` (default ON on supported platform) : build
  QBDIPreload static library (supported on Linux and OSX).
* ``QBDI_TOOLS_VALIDATOR`` (default ON on supported platform) : build
  the validator library (supported on Linux and OSX).
* ``QBDI_TOOLS_PYQBDI`` (default ON on X86_64) : build PyQBDI library.
  Supported on Linux, Windows and OSX.
* ``QBDI_TOOLS_FRIDAQBDI`` (default ON) : add Frida/QBDI in the package.


