.. currentmodule:: pyqbdi

Basic block events
==================

Introduction
------------

The :ref:`Instrument Callback <api_desc_InstCallback>` allows inserting callbacks on all or specific instruction.
With a callback at each instruction, it's trivial to follow the execution pointer and obtain a trace of the execution.
However, the performances are slow, as the execution should be stopped at each instruction from executing the callback.
For some traces, a higher-level callback can have better performances.

The :ref:`api_desc_VMCallback` is called when some conditions are reached during the execution. This tutorial
introduces 3 `VMEvent`:

- ``BASIC_BLOCK_NEW`` : this event is triggered when a new basic block has been instrumented and add to the cache. It can be used to create
  coverage of the execution.
- ``BASIC_BLOCK_ENTRY``: this event is triggered before the execution of a basic block.
- ``BASIC_BLOCK_EXIT``: this event is triggered after the execution of a basic block.

A basic block in QBDI
---------------------

QBDI doesn't analyse the whole program before the run. Basic blocks are detected dynamically and may not match the basic block given by other tools.
In QBDI, a basic block is a sequence of consecutive instructions that doesn't change the instruction pointer except the last one.
Any instruction that may change the instruction pointer (method call, jump, conditional jump, method return, ...) are always the
last instruction of a basic block.

QBDI detects the beginning of a basic block:

- when the instruction is the first to be executed in QBDI;
- after the end of the previous basic block;
- potentially if the user changes the callback during the execution (add a new callback, clear the cache, return ``BREAK_TO_VM``, ...)

Due to the dynamic detection of the basic block, some basic block can overlap others.
For the follow code, QBDI can detect 4 different basic blocks. If the first jump is taken:

- The begin of the method, between 0x10e9 and 0x1103;
- The block between 0x1103 and 0x1109;
- The L2 block between 0x1115 and 0x111f.

If the first jump isn't taken:

- The begin of the method, between 0x10e9 and 0x1103;
- A block composed of L1 and L2 between 0x1109 and 0x111f.

.. code:: asm

    // foo:
    // 10e9:
    push   rbp
    mov    rbp,rsp
    mov    DWORD PTR [rbp-0x14],edi
    mov    edx,DWORD PTR [rbp-0x14]
    mov    eax,edx
    shl    eax,0x2
    add    eax,edx
    mov    DWORD PTR [rbp-0x4],eax
    cmp    DWORD PTR [rbp-0x14],0xa
    jle    1109 <L1>
    // 1103:
    add    DWORD PTR [rbp-0x4],0x33
    jmp    1115 <L2>
    // 1109: L1:
    mov    eax,DWORD PTR [rbp-0x4]
    imul   eax,eax
    add    eax,0x57
    mov    DWORD PTR [rbp-0x4],eax
    // 1115: L2:
    mov    edx,DWORD PTR [rbp-0x4]
    mov    eax,DWORD PTR [rbp-0x14]
    add    eax,edx
    pop    rbp
    ret
    // 111f:


Get basic block information
---------------------------

To receive basic block information, a ``VMCallback`` should be registered to the VM with ``addVMEventCB`` for
one of ``BASIC_BLOCK_*`` events. Once a registered event occurs, the callback is called with a description of the VM (``VMState``).

The address of the current basic block can be retrieved with ``VMState.basicBlockStart`` and ``VMState.basicBlockEnd``.

.. note::

    ``BASIC_BLOCK_NEW`` can be triggered with ``BASIC_BLOCK_ENTRY``. A callback registered with both events will
    only be called once. You can retrieve the events that triggered the callback in ``VMState.event``.

The follow example registers the three events in the VM and displays the basic block's address when one event triggers.

C basic block information
+++++++++++++++++++++++++

Reference: :cpp:type:`VMCallback`, :cpp:func:`qbdi_addVMEventCB`, :cpp:struct:`VMState`

.. code:: c

    VMAction vmcbk(VMInstanceRef vm, const VMState* vmState, GPRState* gprState, FPRState* fprState, void* data) {

        printf("start:0x%" PRIRWORD ", end:0x%" PRIRWORD "%s%s%s\n",
            vmState->basicBlockStart,
            vmState->basicBlockEnd,
            (vmState->event & QBDI_BASIC_BLOCK_NEW)? " BASIC_BLOCK_NEW":"",
            (vmState->event & QBDI_BASIC_BLOCK_ENTRY)? " BASIC_BLOCK_ENTRY":"",
            (vmState->event & QBDI_BASIC_BLOCK_EXIT)? " BASIC_BLOCK_EXIT":"");
        return QBDI_CONTINUE;
    }

    qbdi_addVMEventCB(vm, QBDI_BASIC_BLOCK_NEW | QBDI_BASIC_BLOCK_ENTRY | QBDI_BASIC_BLOCK_EXIT, vmcbk, NULL);

C++ basic block information
+++++++++++++++++++++++++++

Reference: :cpp:type:`QBDI::VMCallback`, :cpp:func:`QBDI::VM::addVMEventCB`, :cpp:struct:`QBDI::VMState`

.. code:: cpp

    QBDI::VMAction vmcbk(QBDI::VMInstanceRef vm, const QBDI::VMState* vmState, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* data) {

        std::cout << std::setbase(16) << "start:0x" << vmState->basicBlockStart
                  << ", end:0x" << vmState->basicBlockEnd;
        if (vmState->event & QBDI::BASIC_BLOCK_NEW) {
            std::cout << " BASIC_BLOCK_NEW";
        }
        if (vmState->event & QBDI::BASIC_BLOCK_ENTRY) {
            std::cout << " BASIC_BLOCK_ENTRY";
        }
        if (vmState->event & QBDI::BASIC_BLOCK_EXIT) {
            std::cout << " BASIC_BLOCK_EXIT";
        }
        std::cout << std::endl;
        return QBDI::CONTINUE;
    }

    vm.addVMEventCB(QBDI::BASIC_BLOCK_NEW | QBDI::BASIC_BLOCK_ENTRY | QBDI::BASIC_BLOCK_EXIT, vmcbk, nullptr);

PyQBDI basic block information
++++++++++++++++++++++++++++++

Reference: :py:func:`pyqbdi.VMCallback`, :py:func:`pyqbdi.VM.addVMEventCB`, :py:class:`pyqbdi.VMState`

.. code:: python

    def vmcbk(vm, vmState, gpr, fpr, data):
        # user callback code

        print("start:0x{:x}, end:0x{:x} {}".format(
            vmState.basicBlockStart,
            vmState.basicBlockEnd,
            vmState.event & (pyqbdi.BASIC_BLOCK_NEW | pyqbdi.BASIC_BLOCK_ENTRY | pyqbdi.BASIC_BLOCK_EXIT) ))
        return pyqbdi.CONTINUE

    vm.addVMEventCB(pyqbdi.BASIC_BLOCK_NEW | pyqbdi.BASIC_BLOCK_ENTRY | pyqbdi.BASIC_BLOCK_EXIT, vmcbk, None)

Frida/QBDI basic block information
++++++++++++++++++++++++++++++++++

Reference: :js:func:`VMCallback`, :js:func:`QBDI.addVMEventCB`, :js:class:`VMState`

.. code:: js

    var vmcbk = vm.newVMCallback(function(vm, state, gpr, fpr, data) {
        var msg = "start:0x" + state.basicBlockStart.toString(16) + ", end:0x" + state.basicBlockEnd.toString(16);
        if (state.event & VMEvent.BASIC_BLOCK_NEW) {
            msg = msg + " BASIC_BLOCK_NEW";
        }
        if (state.event & VMEvent.BASIC_BLOCK_ENTRY) {
            msg = msg + " BASIC_BLOCK_ENTRY";
        }
        if (state.event & VMEvent.BASIC_BLOCK_EXIT) {
            msg = msg + " BASIC_BLOCK_EXIT";
        }
        console.log(msg);
        return VMAction.CONTINUE;
    });

    vm.addVMEventCB(VMEvent.BASIC_BLOCK_NEW | VMEvent.BASIC_BLOCK_ENTRY | VMEvent.BASIC_BLOCK_EXIT, vmcbk, null);


Basic block coverage
--------------------

To perform a code coverage, ``BASIC_BLOCK_NEW`` can be used to detect the new basic block JITed by QBDI.
Some precautions should be taken:

- If a vm is reused to different coverage, the cache must be clear between each run to have independent coverage.
  However, if you reused the vm to perform incremental coverage, we can keep the cache between runs.
- The coverage only covers the instrumented range.
- This method supposes that the execution of the basic block won't trigger an interruption (exception, signal, ...).
- Code has no overlapping instructions or other forms of obfuscation.

For more precise coverage, ``BASIC_BLOCK_ENTRY`` or ``BASIC_BLOCK_EXIT`` can be used instead of ``BASIC_BLOCK_NEW``, but
the callback may be called more than once for each basic block.


C coverage
++++++++++

.. code:: c

    // your own coverage library
    #include "mycoverage.h"

    VMAction covcbk(VMInstanceRef vm, const VMState* vmState, GPRState* gprState, FPRState* fprState, void* data) {

        myCoverageAdd( (myCoverageData*) data, vmState->basicBlockStart, vmState->basicBlockEnd);
        return QBDI_CONTINUE;
    }

    myCoverageData cover;

    qbdi_addVMEventCB(vm, QBDI_BASIC_BLOCK_NEW, covcbk, &cover);

    // run the VM
    // ....

    // print the coverage
    myCoveragePrint(&cover);


C++ coverage
++++++++++++

QBDI has a tiny range set class (:cpp:class:`QBDI::RangeSet`), usable only with the C++ API.

.. code:: cpp

    QBDI::VMAction covcbk(QBDI::VMInstanceRef vm, const QBDI::VMState* vmState, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* data) {

        QBDI::RangeSet<QBDI::rword>* rset = static_cast<QBDI::RangeSet<QBDI::rword>*>(data);
        rset->add({vmState->basicBlockStart, vmState->basicBlockEnd});

        return QBDI::CONTINUE;
    }

    QBDI::RangeSet<QBDI::rword> rset;

    vm.addVMEventCB(QBDI::BASIC_BLOCK_NEW, covcbk, &rset);

    // run the VM
    // ....

    // print the coverage
    for (const auto r: rset.getRanges()) {
        std::cout << std::setbase(16) << "0x" << r.start() << " to 0x" << r.end() << std::endl;
    }

PyQBDI coverage
+++++++++++++++

.. code:: python

    def covcbk(vm, vmState, gpr, fpr, data):

        if vmState.basicBlockEnd not in data['cov'] or vmState.basicBlockStart < data['cov'][vmState.basicBlockEnd][0]:
            data['cov'][vmState.basicBlockEnd] = (vmState.basicBlockStart, vmState.basicBlockEnd)
        return pyqbdi.CONTINUE


    cov = {"cov": {}}

    vm.addVMEventCB(pyqbdi.BASIC_BLOCK_NEW, covcbk, cov)

    # run the VM
    # ....

    for _, c in cov['cov'].items():
        print(f"0x{c[0]:x} to 0x{c[1]:x}")

In addition, a coverage script that generates DRCOV coverage is available in `examples/pyqbdi/coverage.py <https://github.com/QBDI/QBDI/blob/master/examples/pyqbdi/coverage.py>`_.


Frida/QBDI coverage
+++++++++++++++++++

.. code:: js

    var covcbk = vm.newVMCallback(function(vm, state, gpr, fpr, cov) {
        if ( (! cov[state.basicBlockEnd]) || state.basicBlockStart < cov[state.basicBlockEnd][0] ) {
            cov[state.basicBlockEnd] = [state.basicBlockStart, state.basicBlockEnd]
        }
        return VMAction.CONTINUE;
    });

    var cov = {};

    vm.addVMEventCB(VMEvent.BASIC_BLOCK_NEW, covcbk, cov);

    // run the VM
    // ....

    for(var c in cov){
        console.log("0x" + cov[c][0].toString(16) + " to 0x" + cov[c][1].toString(16));
    }

Edge coverage
-------------

The ``BASIC_BLOCK_EXIT`` event can be used to detect the edge between basic block. As the event is triggered after the execution of the basic block,
the next address can be found in the GPRState. The address pair ``(state.basicBlockEnd, gpr.rip)`` is the edge to store in the coverage.

