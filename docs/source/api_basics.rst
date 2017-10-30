.. role:: red
.. highlight:: c

API Basics
==========

A step-by-step illustrating a basic (yet powerful) usage of QBDI C APIs.

Program initialization
------------------------

First we need a :c:type:`VMInstanceRef` variable and a pointer to the *fake* stack that will be used by the instrumented binary::

  int main(int argc, char** argv) {
      VMInstanceRef vm = NULL;
      uint8_t *fakestack = NULL;


Virtual Machine initialization
------------------------------

We then need to initialize the Virtual Machine itself using :c:func:`qbdi_initVM`::

  qbdi_initVM(&vm, NULL, NULL);


Pointer to VM context
---------------------

In order to initialize the virtual stack properly (see next section), we need a pointer to the virtual machine state (:c:type:`GPRState`), that can be obtained using :c:func:`qbdi_getGPRState`::

  GPRState *state = qbdi_getGPRState(vm);
  assert(state != NULL);


Allocate a virtual stack
------------------------

Now that we have a pointer to the virtual state, we can allocate and initialize a virtual stack::

  bool res = qbdi_allocateVirtualStack(state, STACK_SIZE, &fakestack);
  assert(res == true);


Our first callback function
---------------------------

Now that the virtual machine has been set up, we can start playing with QBDI core features.

We want a callback function to be called every time we encounter an instruction, and that's exactly the purpose of :c:func:`qbdi_addCodeRangeCB`.
Our callback will print current (guest) instruction address and disassembly.
The range is here being defined from the start of the :c:func:`qbdi_secretFunc` until :c:func:`qbdi_secretFunc` *+ 100* (doesn't need to be accurate in that case)::

  VMAction showInstruction(VMInstanceRef vm, GPRState *gprState, FPRState *fprState, void *data) {
      // Obtain an analysis of the instruction from the VM
      const InstAnalysis* instAnalysis = qbdi_getInstAnalysis(vm, QBDI_ANALYSIS_INSTRUCTION | QBDI_ANALYSIS_DISASSEMBLY);
      // Printing disassembly
      printf("0x%" PRIRWORD ": %s\n", instAnalysis->address, instAnalysis->disassembly);
      return QBDI_CONTINUE;
  }

  uint32_t uid = qbdi_addCodeRangeCB(vm, (rword) &secretFunc, (rword) &secretFunc + 100, QBDI_PREINST, showInstruction, vm);
  assert(uid != QBDI_INVALID_EVENTID);

Set instrumented range
----------------------

Callback has been set to a specific range, we now need to enable the instrumentation on a specific code range (like a whole module).
We can use :c:func:`qbdi_addInstrumentedModuleFromAddr`, using our main function address, to add our current executable code range::

  res = qbdi_addInstrumentedModuleFromAddr(vm, (rword) &main);
  assert(res == true);

Setup virtual stack
-------------------

QBDI also provides some helpers, aiming to simplify common usage. We want to use our stack and state to *call* a function.
In fact, this operation will be simulated, and we need to prepare the state to be *like* after a *call* instruction (with arguments
in registers / stack).
:c:func:`qbdi_simulateCall` will do this for us, respecting the ABI of current architecture. It will take a return address (that can be *fake*, acting like
a terminator), and a list of arguments::

  qbdi_simulateCall(state, FAKE_RET_ADDR, 1, (rword) 666);


Run the instrumentation
-----------------------

We can finally run the instrumentation using the :c:func:`qbdi_run` function.
Here it will start the execution at :c:func:`qbdi_secretFunc`, until guest returns to our *fake* address.

Keep in mind that we have previously initialized the state to simulate a call, so QBDI will instrument the equivalent of a call
to *secretFunc(666);* ::

  res = qbdi_run(vm, (rword) secretFunc, (rword) FAKE_RET_ADDR);
  assert(res == true);

  // get return value from current state
  rword retval = QBDI_GPR_GET(state, REG_RETURN);
  printf("[*] retval=0x%"PRIRWORD"\n", retval);


End program properly
--------------------

Finally, we need to free allocated stack and exit the virtual machine properly using :c:func:`qbdi_alignedFree` and :c:func:`qbdi_terminateVM`::

  qbdi_alignedFree(fakestack);
  qbdi_terminateVM(vm);


Fully working example
---------------------

You can find a fully working example below, based on the precedent explanations.

.. include:: ../../templates/qbdi_template/qbdi_template.c
   :code:
