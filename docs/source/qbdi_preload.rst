QBDIPreload
===========

The examples from the API sections demonstrated a use case where the instrumented code, QBDI and 
the instrumentation are compiled in the same binary. This, however, only works for a very limited 
amount of real-world scenarios. Other scenarios require some injection tool that can load QBDI and 
your instrumentation code in another binary.

QBDIPreload is a small utility library that provides code injection capabilities using dynamic 
library injection. It currently only works under **linux** using the ``LD_PRELOAD`` mechanism and 
**macOS** using the ``DYLD_INSERT_LIBRARIES`` mechanism. For other platforms please look into using 
:ref:`frida-bindings` instead.

QBDIPreload exploits these library injection mechanisms to hijack the normal program startup. 
During the hijacking process QBDIPreload will callback your code allowing you to setup and start 
your instrumentation. The compilation should produce a dynamic library (``.so`` under **linux**, 
``.dylib`` under **macOS**) which should then be added to the matching environment variable 
(``LD_PRELOAD`` under **linux** and ``DYLD_INSERT_LIBRARIES`` under **macOS**) when running the 
target binary.

You can look at the :ref:`qbdi_preload_template` for a working example with build and usage 
instructions.

.. note::
   Please note that QBDIPreload does not allow to instrument a binary before the main function 
   (inside the loader and the library constructors / init).


QBDIPreload API
---------------

Initialization
""""""""""""""

Initialization is performed using a constructor::

    #include <QBDIPreload.h>

    QBDIPRELOAD_INIT;

.. doxygendefine:: QBDIPRELOAD_INIT
    :project: TOOLS


Return Codes
""""""""""""

.. doxygendefine:: QBDIPRELOAD_NO_ERROR
    :project: TOOLS
    
.. doxygendefine::  QBDIPRELOAD_NOT_HANDLED
    :project: TOOLS
    
.. doxygendefine:: QBDIPRELOAD_ERR_STARTUP_FAILED
    :project: TOOLS
    
    
User callbacks
""""""""""""""

Each of these functions must be declared and will be called (in the same order than in
this documentation) by QBDIPreload during the hijacking process.

.. doxygenfunction:: qbdipreload_on_start
    :project: TOOLS

.. doxygenfunction:: qbdipreload_on_premain
    :project: TOOLS
    
.. doxygenfunction:: qbdipreload_on_main
    :project: TOOLS

.. doxygenfunction:: qbdipreload_on_run
    :project: TOOLS    
    
.. doxygenfunction:: qbdipreload_on_exit
    :project: TOOLS    
    
Helpers
"""""""

:c:func:`qbdipreload_hook_main` can be used to hook any address
as `main` during the hijacking process.

.. doxygenfunction:: qbdipreload_hook_main
    :project: TOOLS

Contexts related helpers allow to convert a platform dependent GPR or FPR state structure to a QBDI structure.
Under **linux** both functions should receive a ``ucontext_t*`` and under **macOS** they should 
receive a ``x86_thread_state64_t*`` or a ``x86_float_state64_t*``. Please look into QBDIPreload 
source code for more information.

.. doxygenfunction:: qbdipreload_threadCtxToGPRState
    :project: TOOLS

.. doxygenfunction:: qbdipreload_floatCtxToFPRState
    :project: TOOLS
 
.. _qbdi_preload_template:

QBDIPreload Template
--------------------

To get started with QBDIPreload you can follow those few simple steps:


.. code-block:: bash

    $ mkdir QBDIPreload && cd QBDIPreload
    $ qbdi-preload-template
    $ mkdir build && cd build
    $ cmake ..
    $ make

This will simply build the default QBDIPreload template (which prints instruction address and 
disassembly) and can be executed doing the following under **linux**: 

.. code-block:: bash

    $ LD_PRELOAD=./libqbdi_tracer.so /bin/ls

Or the following under **macOS**:

.. code-block:: bash

    $ cp /bin/ls ./ls
    $ sudo DYLD_INSERT_LIBRARIES=./libqbdi_tracer.so ./ls

.. note::

   Please note that, under **macOS**, the *System Integrity Protection* (SIP) will prevent you from 
   instrumenting system binaries. You must either use a local copy of the binary or disable SIP.
