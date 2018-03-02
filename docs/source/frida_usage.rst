.. highlight:: javascript

Get Started with Frida/QBDI
===========================

To be able to use QBDI bindings while injecting into a process, it is necessary to understand a bit of Frida to perform some common tasks. Through this simple example based on *qbdi-frida-template* we will explain a basic usage of Frida & QBDI.

Common tasks
------------

Most actions described here are listed in the Frida documentation, this is mostly a reminder for those used to interact with Frida.


Read Memory
+++++++++++

Sometimes it may be necessary to have a look at a buffer or specific part of the memory. We rely on Frida to do it.

.. code-block:: javascript

    var arrayPtr = ptr(0xDEADBEEF)
    var size = 0x80
    var buffer = Memory.readByteArray(arrayPtr, size)


Write Memory
++++++++++++

We also need to be able to write memory:

.. code-block:: javascript

    var arrayPtr = ptr(0xDEADBEEF)
    var size = 0x80
    var toWrite = new Uint8Array(size);
    // Fill your buffer eventually
    Memory.writeByteArray(arrayPtr, toWrite)


Allocate an array
+++++++++++++++++

If you have a function that takes a buffer or a string as an input, you might need to allocate a new buffer using Frida:

.. code-block:: javascript

    // allocate and write a 2 bytes buffer
    var buffer = Memory.alloc(2);
    Memory.writeByteArray(buffer, [0x42, 0x42])
    // allocate and write an UTF8 string
    var str = Memory.allocUtf8String("Hello World !");


Initialize a QBDI object
++++++++++++++++++++++++

If `frida-qbdi.js` (or a script requiring it) is successfully loaded in Frida,
a new QBDI object become available.
It provides an object oriented access to the framework features.

.. code-block:: javascript

    // Initialize QBDI
    var vm = new QBDI();
    console.log("QBDI version is " + vm.version.string);
    var state = vm.getGPRState();


Instrument a function with QBDI
+++++++++++++++++++++++++++++++

You can instrument a function using QBDI bindings. They are really close to the C++ ones,
with more information is available in Frida/QBDI :ref:`frida-bindins-api` documentation.

.. code-block:: javascript

    var functionPtr = DebugSymbol.fromName("function_name").address;
    vm.addInstrumentedModule("demo.bin");

    var InstructionCallback = vm.newInstCallback(function(vm, gpr, fpr, data) {
        inst = vm.getInstAnalysis();
        gpr.dump(); // Display context
        console.log("0x" + inst.address.toString(16) + " " + inst.disassembly); // Display instruction
        return VMAction.CONTINUE;
    });
    var iid = vm.addCodeCB(InstPosition.PREINST, instructionCallback, NULL);

    vm.call(functionPtr, []);


If you ever want to pass argument to your callback, this can be done via the **data** argument :

.. code-block:: javascript

    // This callback is used to count the number of basicblocks executed
    var userData = { counter: 0};
    var BasicBlockCallback = vm.newVMCallback(function(vm, evt, gpr, fpr, data) {
        data.counter++;
        return VMAction.CONTINUE;
    });
    vm.addVMEventCB(VMEvent.BASIC_BLOCK_ENTRY, BasicBlockCallback, userData);
    console.log(userData.counter);


Scripts
+++++++

Bindings can simply be used in Frida REPL, or imported in a Frida script, empowering
the bindings with all the `nodejs` ecosystem.

.. code-block:: javascript

    const qbdi = require('/usr/local/share/qbdi/frida-qbdi'); // import QBDI bindings
    qbdi.import(); // Set bindings to global environment

    var vm = new QBDI();
    console.log("QBDI version is " + vm.version.string);


This simple script can be compiled with `frida-compile` utility (see `Frida` documentation).
It will be possible to load it in Frida in place of `frida-qbdi.js`, allowing to
easily create custom instrumentation tools with in-process scripts written in
JavaScript and external control in Python (or any language supported by `Frida`).


Complete example
----------------

If you already had a look at the default instrumentation of the template generated with *qbdi-frida-template* you will be familiar with the following example.
What it does is creating a native call to the *Secret()* function, and instrument it looking for *XOR*.

Source code
+++++++++++

.. literalinclude:: ../../templates/qbdi_frida_template/SimpleXOR.c
    :language: C


Instrumentation code
++++++++++++++++++++

.. literalinclude:: ../../templates/qbdi_frida_template/FridaQBDI_sample.js
    :language: javascript
