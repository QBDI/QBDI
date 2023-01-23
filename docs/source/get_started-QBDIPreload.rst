.. _get_started-qbdipreload:

QBDIPreload
===========

QBDIPreload is a small utility library that provides code injection capabilities using dynamic library injection.
It works on Linux and macOS respectively with the ``LD_PRELOAD`` and ``DYLD_INSERT_LIBRARIES`` mechanisms.

Thanks to QBDIPreload, you can instrument the main function of an executable that has been dynamically linked.
You can also define various callbacks that are called at specific times throughout the execution.

Main hook process
-----------------

To use QBDIPreload, you must have a minimal codebase: a constructor and several *hook* functions.
Like callbacks, hook functions are directly called by QBDIPreload.

First of all, the constructor of QBDIPreload has to be initialised through declaring the macro :c:var:`QBDIPRELOAD_INIT`.
It's worth noting that this macro must be only defined once in your code.

The :cpp:func:`qbdipreload_on_start` and :cpp:func:`qbdipreload_on_premain` hook functions are called at different stages during the execution of the programme.
They only need to return :c:var:`QBDIPRELOAD_NOT_HANDLED` if you don't want to modify the hook procedure.

.. code:: c

    #include "QBDIPreload.h"

    QBDIPRELOAD_INIT;

    int qbdipreload_on_start(void *main) {
        return QBDIPRELOAD_NOT_HANDLED;
    }

    int qbdipreload_on_premain(void *gprCtx, void *fpuCtx) {
        return QBDIPRELOAD_NOT_HANDLED;
    }

Instrumentation
---------------

Once the main function is hooked by QBDIPreload, two methods are called: :cpp:func:`qbdipreload_on_main` and :cpp:func:`qbdipreload_on_run`.

At this point, you are able to capture the executable arguments inside of the :cpp:func:`qbdipreload_on_main` scope.
The :cpp:func:`qbdipreload_on_run` function is called right afterwards with a ready-to-run QBDI virtual machine as first argument.
Obviously, don't forget to register your callback(s) prior to running the VM.

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

Exit hook
---------

QBDIPreload also intercepts the calls on standard exit functions (``exit`` and ``_exit``).
Typically, these are called when the executable is about to terminate.
If so, the :cpp:func:`qbdipreload_on_exit` method is called and can be used to save some data about the execution you want to keep before exiting.
Note that the hook function is not called if the executable exits with a direct system call or a segmentation fault.

.. code:: c

    int qbdipreload_on_exit(int status) {
        return QBDIPRELOAD_NO_ERROR;
    }

Compilation and execution
-------------------------

Finally, you need to compile your source code to a dynamic library.
Your output binary has to be statically linked with both the QBDIPreload library and the QBDI library.

Then, in order to test it against a target, simply running the following command should do the job:

.. code:: bash

    # on Linux
    LD_BIND_NOW=1 LD_PRELOAD=./libqbdi_mytracer.so <executable> [<parameters> ...]

    # on macOS
    sudo DYLD_BIND_AT_LAUNCH=1 DYLD_INSERT_LIBRARIES=./libqbdi_mytracer.so <executable> [<parameters> ...]

As the loader is not in the instrumentation range, we recommend setting ``LD_BIND_NOW`` or ``DYLD_BIND_AT_LAUNCH``
in order to resolve and bind all symbols before the instrumentation.

Full example
------------

Merging everything we have learnt throughout this tutorial, we are now able to write our C/C++ source code files.
In the following examples, we aim at displaying every executed instruction of the binary we are running against.

.. _get_started-preload-c:

QBDIPreload in C
++++++++++++++++

.. include:: ../../examples/c/tracer_preload.c
   :code:

.. _get_started-preload-cpp:

QBDIPreload in C++
++++++++++++++++++

.. include:: ../../examples/cpp/tracer_preload.cpp
   :code:

.. _qbdi_preload_template:

Generate a template
-------------------

A QBDI template can be considered as a baseline project, a minimal component you can modify and build your instrumentation tool on.
They are provided to help you effortlessly start off a new QBDI based project.
The binary responsible for generating a template is shipped in the release packages and can be used as follows:

.. code:: bash

    mkdir QBDIPreload && cd QBDIPreload
    qbdi-preload-template
    mkdir build && cd build
    cmake ..
    make

MacOS setup
-----------

When QBDIPreload (and also PyQBDIPreload) are used on macOS, the injection can
failed for various reasons.

Copy the target binary
++++++++++++++++++++++

If the binary is a system binary, the
`System Integrity Protection <https://support.apple.com/en-us/HT204899>`_
prevent the injection of library with ``DYLD_INSERT_LIBRARIES``. You can try to
copy the binary in your user home folder before the instrumentation to avoid the
protection.

Run the instrumentation as root
+++++++++++++++++++++++++++++++

The injection of library can be disable for standard user. Run the injection
with ``sudo`` can bypass this limitation.

Instrument ``arm64e`` binary
++++++++++++++++++++++++++++

QBDIPreload is not compatible with ``arm64e`` binary. To verify if your binary is a
``arm64e`` binary, you can execute the followed command:

.. code:: bash

    lipo -archs $BINARY_PATH

You can try to convert your target binary from ``arm64e`` to ``arm64`` with the
followed script. However, this don't work if the target binary use some ``arm64e`` bind
opcode (like ``BIND_OPCODE_THREADED``).

.. code:: python

    import lief
    import subprocess

    binary = lief.MachO.parse(inputfile)
    for index in range(binary.size):
        if binary.at(index).header.cpu_type == lief.MachO.CPU_TYPES.ARM64:
            binary.at(index).header.cpu_subtype ^= (~2)
    binary.write(outputfile)
    subprocess.run(["codesign", "--force", "--sign", "-", outputfile], check=True)

Disable System Integrity Protection
+++++++++++++++++++++++++++++++++++

We recommand to disable the `System Integrity Protection` only as a last resort,
as we successfully inject the QBDIPreload under ``macOS Ventura`` (version
13.1).  However, this may help you to debug the injection and bypass some
signature validation.  You can found the procedure `here
<https://developer.apple.com/documentation/security/disabling_and_enabling_system_integrity_protection>`_.

Don't forget to reenable it afterwards.
