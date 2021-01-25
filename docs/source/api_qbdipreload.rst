.. _qbdipreload_api:

QBDIPreload API
===============

Introduction
------------

QBDIPreload is a small utility library that provides code injection capabilities using dynamic
library injection. It currently only works under **Linux** using the ``LD_PRELOAD`` mechanism and
**MacOS** using the ``DYLD_INSERT_LIBRARIES`` mechanism. For other platforms please look into using
:ref:`frida-qbdi-api` instead.

QBDIPreload exploits these library injection mechanisms to hijack the normal program startup.
During the hijacking process QBDIPreload will call your code allowing you to setup and start
your instrumentation. The compilation should produce a dynamic library (``.so`` under **Linux**,
``.dylib`` under **macOS**) which should then be added to the matching environment variable
(``LD_PRELOAD`` under **Linux** and ``DYLD_INSERT_LIBRARIES`` under **macOS**) when running the
target binary.

You can look at the :ref:`qbdi_preload_template` for a working example with build and usage
instructions.

.. note::
   QBDIPreload automatically takes care of blacklisting instrumentation of the C standard library
   and the OS loader as described in :ref:`intro_limitations`.

.. note::
   Please note that QBDIPreload does not allow instrumenting a binary before the main function
   (inside the loader and the library constructors / init) as explained in :ref:`intro_limitations`.

.. note::
   QBDIPreload is used with ``LD_PRELOAD`` or ``DYLD_INSERT_LIBRARIES`` mechanism to inject in the target
   process. The limitations of these mechanisms are applied on QBDIPreload (cannot inject suid binary, ...).


Initialization
--------------

.. doxygendefine:: QBDIPRELOAD_INIT
    :project: QBDIPRELOAD


Return Codes
------------

.. doxygendefine:: QBDIPRELOAD_NO_ERROR
    :project: QBDIPRELOAD

.. doxygendefine:: QBDIPRELOAD_NOT_HANDLED
    :project: QBDIPRELOAD

.. doxygendefine:: QBDIPRELOAD_ERR_STARTUP_FAILED
    :project: QBDIPRELOAD

User callbacks
--------------

.. doxygenfunction:: qbdipreload_on_start
    :project: QBDIPRELOAD

.. doxygenfunction:: qbdipreload_on_premain
    :project: QBDIPRELOAD

.. doxygenfunction:: qbdipreload_on_main
    :project: QBDIPRELOAD

.. doxygenfunction:: qbdipreload_on_run
    :project: QBDIPRELOAD

.. doxygenfunction:: qbdipreload_on_exit
    :project: QBDIPRELOAD

Helpers
-------

.. doxygenfunction:: qbdipreload_hook_main
    :project: QBDIPRELOAD

.. doxygenfunction:: qbdipreload_threadCtxToGPRState
    :project: QBDIPRELOAD

.. doxygenfunction:: qbdipreload_floatCtxToFPRState
    :project: QBDIPRELOAD
