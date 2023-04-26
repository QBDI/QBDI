.. currentmodule:: pyqbdi

Basic block events
==================

Introduction
------------

The :ref:`Instrument Callback <api_desc_InstCallback>` can insert callbacks on all or specific instructions.
With a callback on every instruction, it's trivial to follow the execution pointer and obtain a trace of the execution.
However, performances are bad because the execution is stopped on each instruction for the callback to be run.
For some traces, a higher-level callback may have better performances.

The :ref:`api_desc_VMCallback` is called when some conditions are reached during the execution. This tutorial
introduces 3 `VMEvent`:

- ``BASIC_BLOCK_NEW`` is triggered when a new basic block has been instrumented and added to the cache. It can be used to create
  coverage of the execution.
- ``BASIC_BLOCK_ENTRY`` is triggered before the execution of a basic block.
- ``BASIC_BLOCK_EXIT`` is triggered after the execution of a basic block.

A basic block in QBDI
---------------------

QBDI doesn't analyze the whole program before the run. Basic blocks are dynamically detected and so may not match basic blocks given by other tools.
In QBDI, a basic block is a sequence of consecutive instructions that do not modify the instruction pointer except for the last one.
Any instruction that may modify the instruction pointer (method call, jump, conditional jump, method return, ...) are always the
last instruction of a basic block.

For QBDI, is the beginning for a basic block:

- the very first instruction to be executed in QBDI;
- the first instruction to be executed after the end of the previous basic block;
- the first instruction to be executed if the user modifies the execution flow (add a new callback, clear the cache, return ``BREAK_TO_VM``, ...)

Due to the dynamic detection of the basic block, basic blocks may overlap each other.
This behavior can be observed in the following code:

.. code:: nasm

    # BB
       push rbp                          # 0x1000
       mov  rbp, rsp                     # 0x1001
       mov  dword ptr [rbp - 0x14], edi  # 0x1004
       mov  edx, dword ptr [rbp - 0x14]  # 0x1007
       mov  eax, edx                     # 0x100a
       shl  eax, 2                       # 0x100c
       add  eax, edx                     # 0x100f
       mov  dword ptr [rbp - 4], eax     # 0x1011
       cmp  dword ptr [rbp - 0x14], 0xa  # 0x1014
       jle  0x1027                       # 0x1018
    # BB
       add  dword ptr [rbp - 4], 0x33    # 0x101e
       jmp  0x1033                       # 0x1022
    # BB
       mov  eax, dword ptr [rbp - 4]     # 0x1027
       imul eax, eax                     # 0x102a
       add  eax, 0x57                    # 0x102d
       mov  dword ptr [rbp - 4], eax     # 0x1030
    # BB
       mov  edx, dword ptr [rbp - 4]     # 0x1033
       mov  eax, dword ptr [rbp - 0x14]  # 0x1036
       add  eax, edx                     # 0x1039
       pop  rbp                          # 0x103b
       ret                               # 0x103c

In this snippet, QBDI can detect 4 different basic blocks. If the first jump isn't taken:

- The begin of the method, between 0x1000 and 0x101e;
- The block between 0x101e and 0x1027;
- The last block between 0x1033 and 0x103d.

If the first jump is taken:

- The begin of the method, between 0x1000 and 0x101e;
- The last block between 0x1027 and 0x103d.


Getting basic block information
-------------------------------

To receive basic block information, a ``VMCallback`` should be registered to the VM with ``addVMEventCB`` for
one of ``BASIC_BLOCK_*`` events. Once a registered event occurs, the callback is ran with a description of the VM (``VMState``).

The address of the current basic block can be retrieved with ``VMState.basicBlockStart`` and ``VMState.basicBlockEnd``.

.. note::

    A callback may register for both ``BASIC_BLOCK_NEW`` and ``BASIC_BLOCK_ENTRY`` events but would be called only once would these two events happen at the same time.
    You can retrieve the events that triggered the callback in ``VMState.event``.

The following example registers for the three events in the VM and displays the basic block's bounds.

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

Reference: :js:func:`VMCallback`, :js:func:`VM.addVMEventCB`, :js:class:`VMState`

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

To perform code coverage, ``BASIC_BLOCK_NEW`` can be used to detect the new basic block JITed by QBDI.
However, it wouldn't work in the following cases:

- If the code jumps outside of the instrumented range.
- If the code triggers an interruption (exception, signal, ...)
- If the code uses overlapping instructions or other forms of obfuscation.

Moreover, if a VM is reused from an execution to another, cache will be kept
and so coverage would be incremental. Clear the cache between every run to have
independent coverage results

For more precise coverage, a user may register ``BASIC_BLOCK_ENTRY`` or ``BASIC_BLOCK_EXIT`` events and handle deduplication themselves.


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
    for (const auto &r: rset.getRanges()) {
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

The ``BASIC_BLOCK_EXIT`` event can be used to detect the edge between basic blocks. As the event is triggered at the end of a basic block (ie. after instruction pointer is modified),
the next address can be found in the GPRState. So, the couple ``(state.basicBlockEnd, gpr.rip)`` is the edge to store in the coverage.
