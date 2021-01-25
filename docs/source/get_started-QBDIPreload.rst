Get started with QBDIPreload
============================

QBDIPreload is a small utility library that provides code injection capabilities using dynamic library injection.
It works on Linux with ``LD_PRELOAD`` mechanism and on MacOS with ``DYLD_INSERT_LIBRARIES`` mechanism.

With QBDIPreload, you can instrument the main method of an executable dynamically linked.

Main hook process
-----------------

With QBDIPreload, the user must define a constructor and some method that will be called during the hook
of the main function.

The constructor of QBDIPreload is defined in the macro :c:var:`QBDIPRELOAD_INIT`. The macro must be placed
once in the user code. The user function :cpp:func:`qbdipreload_on_start` and :cpp:func:`qbdipreload_on_premain`
are called during the hook. They must return :c:var:`QBDIPRELOAD_NOT_HANDLED` when you don't change the hook process.

.. code:: c

    #include "QBDIPreload.h"

    QBDIPRELOAD_INIT;

    int qbdipreload_on_start(void *main) {
        return QBDIPRELOAD_NOT_HANDLED;
    }

    int qbdipreload_on_premain(void *gprCtx, void *fpuCtx) {
        return QBDIPRELOAD_NOT_HANDLED;
    }

Main Hook and instrumentation
-----------------------------

Once the main method is hooked, two methods are called: :cpp:func:`qbdipreload_on_main` and :cpp:func:`qbdipreload_on_run`.
The user may capture the executable arguments with :cpp:func:`qbdipreload_on_main`.
:cpp:func:`qbdipreload_on_run` is called after :cpp:func:`qbdipreload_on_main` with ready to run virtual machine.
The user needs to add his callback before running the provided VM.


.. code:: c

    static VMAction onInstruction(VMInstanceRef vm, GPRState *gprState, FPRState *fprState, void *data) {
        // ...
        return QBDI_CONTINUE;
    }

    int qbdipreload_on_main(int argc, char** argv) {
        return QBDIPRELOAD_NOT_HANDLED;
    }

    int qbdipreload_on_run(VMInstanceRef vm, rword start, rword stop) {
        // add user callbacks
        qbdi_addCodeCB(vm, QBDI_PREINST, onInstruction, NULL);

        // run the VM
        qbdi_run(vm, start, stop);
        return QBDIPRELOAD_NO_ERROR;
    }

.. note::
   QBDIPreload automatically takes care of blacklisting instrumentation of the C standard library
   and the OS loader as described in :ref:`intro_limitations`.

Exit Hook
---------

QBDIPreload includes a hook on standard exit method (``exit`` and ``_exit``). The hook isn't triggers if the
executable exits with a direct syscall, or segfault.

The method :cpp:func:`qbdipreload_on_exit` is called during the hook and allows to properly save the recorded data
when the executable exit.

.. code:: c

    int qbdipreload_on_exit(int status) {
        return QBDIPRELOAD_NO_ERROR;
    }

Compilation and Execution
-------------------------

Your preload script should be statically linked with QBDIPreload library and QBDI library.

To execute your script, run the following command:

.. code:: bash

    # On Linux
    $ LD_PRELOAD=./libqbdi_mytracer.so <executable> [<parameters> ...]

    # On MacOS
    $ sudo DYLD_INSERT_LIBRARIES=./libqbdi_mytracer.so <executable> [<parameters> ...]


Fully working example
---------------------

You can find a fully working example below, based on the precedent explanations.

.. _get_started-preload-c:

C preload
+++++++++

.. include:: ../../examples/c/tracer_preload.c
   :code:

.. _get_started-preload-cpp:

C++ preload
+++++++++++

.. include:: ../../examples/cpp/tracer_preload.cpp
   :code:

.. _qbdi_preload_template:

Template
--------

A simple template of QBDIPreload is included in the package with QBDIPreload. To begin a new project with QBDIPreload, you can run the follow commands:

.. code:: bash

    $ mkdir QBDIPreload && cd QBDIPreload
    $ qbdi-preload-template
    $ mkdir build && cd build
    $ cmake ..
    $ make

This will simply build the default QBDIPreload template (which prints instruction address and disassembly)

