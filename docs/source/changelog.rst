CHANGELOG
=========

Next Version
------------

* Fix android compilation (`#126 <https://github.com/QBDI/QBDI/pull/126>`_)
* Fix instrumentation of Pusha and Popa on X86 (`#127 <https://github.com/QBDI/QBDI/pull/127>`_)
* Fix getBBMemoryAccess (`#128 <https://github.com/QBDI/QBDI/pull/128>`_)
  * Improve the documentation of getBBMemoryAccess
  * Add recordMemoryAccess callback before any InstCallback
* Refactor ExecBlockManager to work with unaligned instruction on X86 and X86-64 (`#129 <https://github.com/QBDI/QBDI/pull/129>`_)
* Drop early support for ARM. The support hasn't been tested since 0.6.2.
* MemoryAccess Support for SIMD instruction and some missing instructions (`#131 <https://github.com/QBDI/QBDI/pull/131>`_)
* Improve InstAnalysis (`#131 <https://github.com/QBDI/QBDI/pull/131>`_)

Some API has changed compared to version 0.7.1 that make the old compiled code incompatible with the new release:

* ``VM::addInstrRule()``: the API has been drop in favor of a new version that dosn't need internals headers.

* ``MemoryAccess``: the structure has changed to introduce ``flags``.

    The MamoryAccess has been refactor to support the access of SIMD instruction.

    * ``MEMORY_UNKNOWN_SIZE``: the size of the access is unknown. This may be the case when the instruction has the REP prefix in X86.
    * ``MEMORY_UNKNOWN_VALUE``: The value of the access isn't capture by QBDI. This happend when the access cannot be fold in the ``value`` field
      or the size is unkwnown before the access.

    Futhermore, almost all the access are now available before the instruction. Some flags are remove after the instruction.

* ``InstAnalysis``: the structure has changed to introduce ``loadSize``, ``storeSize`` and ``condition``, available with the analysis type ``ANALYSIS_INSTRUCTION``.

    The fields ``loadSize`` and ``storeSize`` return the size of the expected access of the instruction.

    Due to the update of LLVM, the some mnemonic has changed. All the conditionnal branch (like ``JE_n``) become ``JCC_n``.
    The effective condition is available in ``condition`` field.

* ``InstAnalysis``: the semantics of the field ``mayLoad`` and ``mayStore`` are changed.

    These fields are now handle without LLVM and are synchronized with the accesses detect by the MemoryAccess API.

* ``InstAnalysis``: the structure has changed to introduce ``flagsAccess``, available with the analysis type ``ANALYSIS_OPERANDS``.

    The field containts the access of the instruction to the generic flag register (EFLAGS in X86).

* ``OperandAnalysis``: Add two new type for float and segment register.

* ``OperandAnalysis``: The type ``OPERAND_INVALID`` is now return when a register isn't specified in order to have the same number of
  operand for any instruction with the same Mnemonic.

  Here is an example of output:

  .. code:: text

      lea rax, [rax+4*rax+12345678]
          [0] optype: GPR, flag: NONE, value: ??, size: 8, name: "RAX", regOff: 0, regCtxIdx: 0, regaccess : -w
          [1] optype: GPR, flag: UNDEFINED_EFFECT, valud: ??, size: 8, name: "RAX", regOff: 0, regCtxIdx: 0, regaccess : r-
          [2] optype: IMM, flag: UNDEFINED_EFFECT, value: 4, size: 8, name: (null), regOff: ??, regCtxIdx: ??, regaccess : --
          [3] optype: GPR, flag: UNDEFINED_EFFECT, value: ??, size: 8, name: "RAX", regOff: 0, regCtxIdx: 0, regaccess : r-
          [4] optype: IMM, flag: UNDEFINED_EFFECT, value: 12345678, size: 4, name: (null), regOff: ??, regCtxIdx: ??, regaccess : --
          [5] optype: INVALID, flag: UNDEFINED_EFFECT, value: ??, size: ??, name: (null),  regOff: ??, regCtxIdx: ??, regaccess : --

      lea rax, gs:[rbx+12345678]
          [0] optype: GPR, flag: NONE, value: ??, size: 8, name: "RAX", regOff: 0, regCtxIdx: 0, regaccess : -w
          [1] optype: GPR, flag: UNDEFINED_EFFECT, valud: ??, size: 8, name: "RBX", regOff: 0, regCtxIdx: 1, regaccess : r-
          [2] optype: IMM, flag: UNDEFINED_EFFECT, value: 1, size: 8, name: (null), regOff: ??, regCtxIdx: ??, regaccess : --
          [3] optype: INVALID, flag: UNDEFINED_EFFECT, value: ??, size: ??, name: (null), regOff: ??, regCtxIdx: ??, regaccess : --
          [4] optype: IMM, flag: UNDEFINED_EFFECT, value: 12345678, size: 4, name: (null), regOff: ??, regCtxIdx: ??, regaccess : --
          [5] optype: SEG, flag: UNDEFINED_EFFECT, value: ??, size: 2, name: "GS",  regOff: ??, regCtxIdx: ??, regaccess : r-

Internal update:

* Update LLVM to LLVM 10.0.1 (`#104 <https://github.com/QBDI/QBDI/pull/104>`_ and `#139 <https://github.com/QBDI/QBDI/pull/139>`_)
* Reduce LLVM library included in QBDI static library and reduce QBDI package size (`#139 <https://github.com/QBDI/QBDI/pull/139>`_ and `#70 <https://github.com/QBDI/QBDI/issues/70>`_)
* Replace GTest by `Catch2 <https://github.com/catchorg/Catch2>`_ (`#140 <https://github.com/QBDI/QBDI/pull/140>`_)
* Refactor code and switch to cpp17 (`#140 <https://github.com/QBDI/QBDI/pull/140>`_)

Version 0.7.1
-------------

2020-02-27 QBDI Team <qbdi@quarkslab.com>

* Refactor PyQBDI, support python3, PyQBDI without Preload (`#67 <https://github.com/QBDI/QBDI/issues/67>`_,
  `#121 <https://github.com/QBDI/QBDI/pull/121>`_)
* Remove ncurses dependency (`#123 <https://github.com/QBDI/QBDI/pull/123>`_)
* Fix initFPRState (`#114 <https://github.com/QBDI/QBDI/issues/114>`_)


Version 0.7.0
-------------

2019-09-10 QBDI Team <qbdi@quarkslab.com>

* Add support for the x86 architecture
* Add new platforms related to Android: android-X86 and android-X86_64
* Improve :c:type:`MemoryMap` structure by adding the module's full path if available
  (`#62 <https://github.com/QBDI/QBDI/issues/62>`_, `#71 <https://github.com/QBDI/QBDI/issues/71>`_)
* Create docker images for QBDI (available on DockerHub `qbdi/qbdi <https://hub.docker.com/r/qbdi/qbdi>`_)
  (`#56 <https://github.com/QBDI/QBDI/pull/56>`_)
* Fix and improve operands analysis involved in memory accesses (`#58 <https://github.com/QBDI/QBDI/issues/58>`_) :

  In the previous version, the output of the instruction analysis for **some** instructions did not contain the information
  related to memory accesses.

  For instance, the *operand analysis* of ``cmp MEM, IMM`` misses information about the first operand:

  .. code:: text

      cmp dword ptr [rbp + 4 * rbx - 4], 12345678
          [0] optype: 1, value : 12345678, size: 8, regOff: 0, regCtxIdx: 0, regName: (null), regaccess : 0

  This issue has been fixed and the :c:type:`OperandAnalysis` structure contains a new  attribute: ``flag``,
  which is used to distinct :c:type:`OperandAnalysis` involved in memory accesses from the others.

  Here is an example of output:

  .. code:: text

      cmp dword ptr [rbp + 4*rbx - 4], 12345678
          [0] optype: 2, flag: 1, value : 48, size: 8, regOff: 0, regCtxIdx: 14, regName: RBP, regaccess : 1
          [1] optype: 1, flag: 1, value : 4, size: 8, regOff: 0, regCtxIdx: 0, regName: (null), regaccess : 0
          [2] optype: 2, flag: 1, value : 49, size: 8, regOff: 0, regCtxIdx: 1, regName: RBX, regaccess : 1
          [3] optype: 1, flag: 1, value : -4, size: 8, regOff: 0, regCtxIdx: 0, regName: (null), regaccess : 0
          [4] optype: 1, flag: 0, value : 12345678, size: 4, regOff: 0, regCtxIdx: 0, regName: (null), regaccess : 0
      mov rax, qword ptr [rbp - 4]
          [0] optype: 2, flag: 0, value : 47, size: 8, regOff: 0, regCtxIdx: 0, regName: RAX, regaccess : 2
          [1] optype: 2, flag: 1, value : 48, size: 8, regOff: 0, regCtxIdx: 14, regName: RBP, regaccess : 1
          [2] optype: 1, flag: 1, value : 1, size: 8, regOff: 0, regCtxIdx: 0, regName: (null), regaccess : 0
          [3] optype: 1, flag: 1, value : -4, size: 8, regOff: 0, regCtxIdx: 0, regName: (null), regaccess : 0
      mov rax, qword ptr [4*rbx]
          [0] optype: 2, flag: 0, value : 47, size: 8, regOff: 0, regCtxIdx: 0, regName: RAX, regaccess : 2
          [1] optype: 1, flag: 1, value : 4, size: 8, regOff: 0, regCtxIdx: 0, regName: (null), regaccess : 0
          [2] optype: 2, flag: 1, value : 49, size: 8, regOff: 0, regCtxIdx: 1, regName: RBX, regaccess : 1
          [3] optype: 1, flag: 1, value : 0, size: 8, regOff: 0, regCtxIdx: 0, regName: (null), regaccess : 0
      jne -6115
          [0] optype: 1, flag: 2, value : -6115, size: 4, regOff: 0, regCtxIdx: 0, regName: (null), regaccess : 0
      lea rax, [rbp + 4*rbx - 4]
          [0] optype: 2, flag: 0, value : 47, size: 8, regOff: 0, regCtxIdx: 0, regName: RAX, regaccess : 2
          [1] optype: 2, flag: 4, value : 48, size: 8, regOff: 0, regCtxIdx: 14, regName: RBP, regaccess : 1
          [2] optype: 1, flag: 4, value : 4, size: 8, regOff: 0, regCtxIdx: 0, regName: (null), regaccess : 0
          [3] optype: 2, flag: 4, value : 49, size: 8, regOff: 0, regCtxIdx: 1, regName: RBX, regaccess : 1
          [4] optype: 1, flag: 4, value : -4, size: 8, regOff: 0, regCtxIdx: 0, regName: (null), regaccess : 0


Version 0.6.2
-------------

2018-10-19 Cedric TESSIER <ctessier@quarkslab.com>

* Add support for a public CI (based on Travis and AppVeyor)
* Fix instruction operands analysis (`#57 <https://github.com/QBDI/QBDI/issues/57>`_,
  `#59 <https://github.com/QBDI/QBDI/pull/59>`_)
* Add missing MEMORY_READ enum value in Python bindings (`#61 <https://github.com/QBDI/QBDI/issues/61>`_)
* Fix cache misbehavior on corner cases (`#49 <https://github.com/QBDI/QBDI/issues/49>`_,
  `#51 <https://github.com/QBDI/QBDI/pull/51>`_)
* Add missing memory access instructions on x86_64 (`#45 <https://github.com/QBDI/QBDI/issues/45>`_,
  `#47 <https://github.com/QBDI/QBDI/issues/47>`_, `#72 <https://github.com/QBDI/QBDI/pull/72>`_)
* Enable asserts in Debug builds (`#48 <https://github.com/QBDI/QBDI/issues/48>`_)

Version 0.6.1
-------------

2018-03-22 Charles HUBAIN <chubain@quarkslab.com>

* Fixing a performance regression with the addCodeAddrCB (`#42 <https://github.com/QBDI/QBDI/issues/42>`_):

  Since 0.6, this API would trigger a complete cache flush forcing the engine to regenerate
  all the instrumented code after each call. Since this API is used inside VM:run(), this
  had the effect of completely canceling precaching optimization where used.

* Fixing support for AVX host without AVX2 support (`#19 <https://github.com/QBDI/QBDI/issues/19>`_):

  Context switching was wrongly using AVX2 instructions instead of AVX instructions causing
  segfaults under hosts supporting AVX but not AVX2.

Version 0.6
-----------

2018-03-02 Charles HUBAIN <chubain@quarkslab.com>

* Important performance improvement in the core engine (`#30 <https://github.com/QBDI/QBDI/pull/30>`_)
  **This slightly changes the behavior of VMEvents.**
* Fix the addCodeAddrCB API (`#37 <https://github.com/QBDI/QBDI/pull/37>`_)
* atexit and getCurrentProcessMap in python bindings (`#35 <https://github.com/QBDI/QBDI/pull/35>`_)
* Fix getInstAnalysis on BASIC_BLOCK_ENTRY (`#28 <https://github.com/QBDI/QBDI/issues/28>`_)
* Various documentation improvements (`#34 <https://github.com/QBDI/QBDI/pull/34>`_,
  `#37 <https://github.com/QBDI/QBDI/pull/37>`_, `#38 <https://github.com/QBDI/QBDI/pull/38>`_,
  `#40 <https://github.com/QBDI/QBDI/pull/40>`_)
  and an API uniformisation (`#29 <https://github.com/QBDI/QBDI/issues/29>`_)

Version 0.5
-----------

2017-12-22 Cedric TESSIER <ctessier@quarkslab.com>

* Official public release!

Version 0.5 RC3
---------------

2017-12-10 Cedric TESSIER <ctessier@quarkslab.com>

* Introducing pyqbdi, full featured python bindings based on QBDIPreload library
* Revising variadic API to include more friendly prototypes
* Various bug, compilation and documentation fixes

Version 0.5 RC2
---------------

2017-10-30 Charles HUBAIN <chubain@quarkslab.com>

* Apache 2 licensing
* New QBDIPreload library for easier dynamic injection under linux and macOS
* Various bug, compilation and documentation fixes
* Big tree cleanup

Version 0.5 RC1
---------------

2017-10-09 Charles HUBAIN <chubain@quarkslab.com>

* New Frida bindings
* Upgrade to LLVM 5.0
* Support for AVX registers
* New callback helpers on mnemonics and memory accesses
* Basic block precaching API
* Automatic cache invalidation when a new instrumentation is added
* Instruction and sequence level cache avoids needless retranslation
* Upgrade of the validator which now supports Linux and macOS

Version 0.4
-----------

2017-01-06 Charles HUBAIN <chubain@quarkslab.com>

* Basic Instruction Shadows concept
* Memory access PatchDSL statements with support under X86_64 (non SIMD memory access only)
* Shadow based memory access API and instrumentation
* C and C++ API stabilization
* Out-of-tree build and SDK
* Overhaul of the entire documentation with a complete PatchDSL explanation and a split
  between user and developer documentation.

Version 0.3
-----------

2016-04-29 Charles HUBAIN <chubain@quarkslab.com>

* Partial ARM support, sufficient to run simple program e.g cat, ls, ...
* Instrumentation filtering system, ExecBroker, allowing the engine to switch between non
  instrumented and instrumented execution
* Complex execution validation system under linux which allows to do instruction per instruction
  compared execution between a non instrumented and an instrumented instance of a program
* New callback system for Engine related event e.g basic block entry / exit, ExecBroker
  transfer / return.
* New (internal) logging system, LogSys, which allows to do priority and tag based filtering of the debug logs.

Version 0.2
-----------

2016-01-29 Charles HUBAIN <chubain@quarkslab.com>

* Upgrade to LLVM 3.7
* Complete X86_64 patching support
* Support of Windows X86_64
* Basic callback based instrumentation
* Usable C++ and C API
* User documentation with examples
* Uniformisation of PatchDSL

Version 0.1
-----------

2015-10-09 Charles HUBAIN <chubain@quarkslab.com>

* Ported the PatchDSL from the minijit PoC
* Corrected several design flaws in the PatchDSL
* Implemented a comparated execution test setup to prove the execution via the JIT yields the
  same registers and stack state as a normal execution
* Basic patching working for ARM and X86_64 architectures as shown by the compared execution
  tests

Version 0.0
-----------

2015-09-17 Charles HUBAIN <chubain@quarkslab.com>

* Working dependency system for LLVM and Google Test
* ExecBlock working and tested on linux-X86_64, linux-ARM, android-ARM and macOS-X86_64
* Deployed buildbot infrastructure for automated build and test on linux-X86_64 and linux-ARM
