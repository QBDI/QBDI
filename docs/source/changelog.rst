CHANGELOG
=========

Next release (0.11.1)
---------------------



Version (0.11.0)
----------------

2024-05-17 QBDI Team <qbdi@quarkslab.com>

* Fix ARM instrumentation for 'mov pc, lr' (`#241 <https://github.com/QBDI/QBDI/pull/241>`__)
* Add switchStackAndCall API (`#245 <https://github.com/QBDI/QBDI/pull/245>`__)
* Rename ``QBDI::InstAnalysis::module`` and ``QBDI::InstAnalysis::symbol`` to
  ``QBDI::InstAnalysis::moduleName`` and ``QBDI::InstAnalysis::symbolName``. The
  same changed applied in C, C++, Python and JS API, but Python and JS API
  deprecated but still support the previous name.
* Update LLVM to LLVM17 (`#253 <https://github.com/QBDI/QBDI/pull/253>`_)
* Support copy and pickle for GPRState and FPRState in PyQBDI (`#247 <https://github.com/QBDI/QBDI/pull/247>`_, `#248 <https://github.com/QBDI/QBDI/pull/248>`_)
* Support python 3.12 (`#247 <https://github.com/QBDI/QBDI/pull/247>`_)

Version 0.10.0
--------------

2023-01-26 QBDI Team <qbdi@quarkslab.com>

* Fix Ubuntu package (`#217 <https://github.com/QBDI/QBDI/pull/217>`__)
* Support ARMv7 and AArch64 architecture (`#222 <https://github.com/QBDI/QBDI/pull/222>`__)
* Support python 3.11 (`#222 <https://github.com/QBDI/QBDI/pull/222>`__)
* Support Frida >= 15.2 (`#222 <https://github.com/QBDI/QBDI/pull/222>`__ and `#223 <https://github.com/QBDI/QBDI/pull/223>`_)

Internal update:

* Move windows CI to Github Actions (`#222 <https://github.com/QBDI/QBDI/pull/222>`__)
* Support python build with ``pyproject.toml`` (`#222 <https://github.com/QBDI/QBDI/pull/222>`__)
* Update LLVM to LLVM15 (`#224 <https://github.com/QBDI/QBDI/pull/224>`_)
* Add CI for ARMv7 and AArch64 (`#222 <https://github.com/QBDI/QBDI/pull/222>`__ and `#225 <https://github.com/QBDI/QBDI/pull/225>`_)

Version 0.9.0
-------------

2022-03-31 QBDI Team <qbdi@quarkslab.com>

* Change internal log system (`#174 <https://github.com/QBDI/QBDI/pull/174>`_).

  * The API ``QBDI::addLogFilter`` has been replaced by :cpp:func:`QBDI::setLogPriority`.
  * The API ``QBDI::setLogOutput`` has been replaced by
    :cpp:func:`QBDI::setLogFile`, :cpp:func:`QBDI::setLogConsole` and
    :cpp:func:`QBDI::setLogDefault`.

* Fix templates (`#186 <https://github.com/QBDI/QBDI/pull/186>`_)
* Fix Frida-QBDI for Frida 15.0.0 (`#192 <https://github.com/QBDI/QBDI/pull/192>`_)
* Change behavior of :cpp:func:`QBDI::VM::addInstrumentedModuleFromAddr` to work
  with mmap region (`#193 <https://github.com/QBDI/QBDI/pull/193>`_)
* Add Priority to InstCallback API (`#194 <https://github.com/QBDI/QBDI/pull/194>`_).
  If two or more InstCallback target the same position (PRE or POST) of the same
  instruction, the priority parameter allows to specify which InstCallback should
  be called first. see :cpp:enum:`QBDI::CallbackPriority`
* Support for X86 ``loop``, ``loope`` and ``loopne`` instructions
  (`#200 <https://github.com/QBDI/QBDI/pull/200>`_)
* Add support for ``FS`` and ``GS`` segment in X86_64
  (`#190 <https://github.com/QBDI/QBDI/pull/190>`_). To support the feature, the
  kernel must support ``RDFSBASE``, ``RDGSBASE``, ``WRFSBASE`` and ``WRGSBASE``
  instructions (linux >= 5.9). To enable the support, the option
  :cpp:enumerator:`QBDI::Options::OPT_ENABLE_FS_GS` must be enabled.
* Hide LLVM symbols from shared library and QBDIPreload
  (`#205 <https://github.com/QBDI/QBDI/pull/205>`_)
* Support python 3.10 for PyQBDI
  (`#206 <https://github.com/QBDI/QBDI/pull/206>`_)
* Add VMAction :cpp:enumerator:`QBDI::VMAction::SKIP_INST` and
  :cpp:enumerator:`QBDI::VMAction::SKIP_PATCH`
  (`#197 <https://github.com/QBDI/QBDI/pull/197>`_)

  * :cpp:enumerator:`QBDI::VMAction::SKIP_INST` can be used to emulate the
    instruction with a PREINST callback. When this VMAction is returned by a
    PREINST callback, QBDI will directly jump to the POSTINST callback.
  * :cpp:enumerator:`QBDI::VMAction::SKIP_PATCH` can be used to jump over all
    the reminding callback for the current instruction. If uses in PREINST
    position, the instruction will not be executed.

  The value associated with the existing :cpp:enum:`QBDI::VMAction` has changed.

* Add tutorial for basic block VMEvent (`#165 <https://github.com/QBDI/QBDI/pull/165>`_)
* Support C++ lambda with capture. (`#207 <https://github.com/QBDI/QBDI/pull/207>`_)
  see :cpp:type:`QBDI::InstCbLambda`, :cpp:type:`QBDI::VMCbLambda`
  and :cpp:type:`QBDI::InstrRuleCbLambda`
* Fix a bug where some symbols were missing in QBDIPreload (`#209 <https://github.com/QBDI/QBDI/pull/209>`_)
* Remove new name of libc in QBDIPreload (`#211 <https://github.com/QBDI/QBDI/pull/211>`_)
* Support of some self-modifying code (`#212 <https://github.com/QBDI/QBDI/pull/212>`_).
  QBDI will not crash if invalid instructions are found when handling a new basic
  block.
* Add tutorial for ExecBroker VMEvent (`#166 <https://github.com/QBDI/QBDI/pull/166>`_)
* Change :cpp:func:`QBDI::getVersion` out parameter to return version to the form ``0xMMmmpp`` (`#214 <https://github.com/QBDI/QBDI/pull/214>`_)

Internal update:

* Add static library licenses in LICENSE.txt (`#169 <https://github.com/QBDI/QBDI/pull/169>`_)
* Format code with clang-format and cmake-format (`#175 <https://github.com/QBDI/QBDI/pull/175>`_)
* Fix integer overflow in cache optimisation (`#168 <https://github.com/QBDI/QBDI/pull/168>`_)
* Refactor build system, add llvm build in main cmake with FetchContent,
  move all LLVM object in a new LLVMCPU class, split internal class, split state by architecture
  (`#178 <https://github.com/QBDI/QBDI/pull/178>`_,
  `#179 <https://github.com/QBDI/QBDI/pull/179>`_ and
  `#188 <https://github.com/QBDI/QBDI/pull/188>`_)
* Update LLVM to LLVM 13.0.0, remove zlib dependency
  (`#180 <https://github.com/QBDI/QBDI/pull/189>`_, `#196 <https://github.com/QBDI/QBDI/pull/196>`_)
* Remove empty Patch not associated to an MCInst (`#195 <https://github.com/QBDI/QBDI/pull/195>`_)
* Compile assembly with ``--noexecstack`` to have a ``rw-`` stack when using QBDI
  on linux (`#201 <https://github.com/QBDI/QBDI/pull/201>`_)
* Use build directory to build the documentation (`#213 <https://github.com/QBDI/QBDI/pull/213>`_)
* Use Doxygen 1.9.2 in readthedocs (`#214 <https://github.com/QBDI/QBDI/pull/214>`_)


Version 0.8.0
-------------

2021-02-11 QBDI Team <qbdi@quarkslab.com>

* Fix android compilation (`#126 <https://github.com/QBDI/QBDI/pull/126>`_)
* Fix instrumentation of Pusha and Popa on X86 (`#127 <https://github.com/QBDI/QBDI/pull/127>`_)
* Fix getBBMemoryAccess (`#128 <https://github.com/QBDI/QBDI/pull/128>`_)

  * Improve the documentation of getBBMemoryAccess
  * Add recordMemoryAccess callback before any InstCallback

* Refactor ExecBlockManager to work with unaligned instruction on X86 and X86-64 (`#129 <https://github.com/QBDI/QBDI/pull/129>`_)
* Drop early support for ARM. The support hasn't been tested since 0.6.2.
* Rework cmake package export to import X86 and X86_64 version of QBDI in one CMake (`#146 <https://github.com/QBDI/QBDI/pull/146>`_ and `#132 <https://github.com/QBDI/QBDI/pull/132>`_)
* Add :cpp:func:`QBDI::VM::getCachedInstAnalysis` to retrieve an InstAnalysis from an address. The address must be cached in the VM. (`#148 <https://github.com/QBDI/QBDI/pull/148>`_)
* Change in ``InstAnalysis`` and ``OperandAnalysis`` (`#153 <https://github.com/QBDI/QBDI/pull/153>`_):

  * Add ``InstAnalysis.flagsAccess`` to determine if the instruction uses or sets the flags (``EFLAGS`` register). The analysis ``ANALYSIS_OPERANDS`` is needed to use this field.
  * Change ``InstAnalysis.mayLoad`` and ``InstAnalysis.mayStore`` definition. The field will be true if QBDI detects memory access for the instruction.
  * Add ``InstAnalysis.loadSize`` and ``InstAnalysis.storeSize``. If the instruction will read or write the memory, the expected size of the access is given by these fields.
    The analysis ``ANALYSIS_INSTRUCTION`` is needed to use this field.
  * Add ``InstAnalysis.condition``. With the update of LLVM, the mnemonic for conditional jump (like ``JE_4``) are merged in a unique mnemonic ``JCC_4``.
    This new field will contain the condition.
    The analysis ``ANALYSIS_INSTRUCTION`` is needed to use this field. A new enum ``ConditionType`` has all the possible value.
  * Add ``OPERANDFLAG_IMPLICIT`` for ``OperandAnalysis.flag``. An operand will have this flag when a register is implicit to the instruction.
  * Add ``OPERAND_FPR`` for ``OperandAnalysis.type``. This type is used for floating point registers.
    For this type, ``OperandAnalysis.regCtxIdx`` is the offset in ``FPRState`` or -1 when an offset cannot be provided.
  * Add ``OPERAND_SEG`` for ``OperandAnalysis.type``. This type is used for segments or other unsupported register (like ``SSP``).
  * Change type of ``OperandAnalysis.regCtxIdx`` to signed integer. When the value is less than 0, the index is invalid.
  * Change algorithm for ``OperandAnalysis``. The type ``OPERAND_INVALID`` may be present in the list of operands when a register is unset with the current instruction.
    Many operands may describe the used of the same register when a register is used multiple times for different purposes by the instruction.

* Add Instrumentation rule callback :c:type:`QBDI_InstrRuleDataCBK` and :cpp:type:`QBDI::InstrRuleDataCBK` (`#151 <https://github.com/QBDI/QBDI/pull/151>`_)

  The Instrumentation rule callback receives an InstAnalysis of each instruction during the instrumentation process. Based on this analysis, the callback
  may insert custom InstCallback for each instruction.

  The call order of the callback has changed for the PREINST callback. If an instruction has multiple callbacks in PREINST position, they will be called
  in the reverse order of registration.

* Support SIMD MemoryAccess and change :cpp:struct:`QBDI::MemoryAccess` structure (`#154 <https://github.com/QBDI/QBDI/pull/154>`_)

  * Add :cpp:member:`QBDI::MemoryAccess::flags`. In some cases, QBDI cannot provide all information about the access. This field
    describes the limitation for each access. Three limitations may be reached:

    * :cpp:enumerator:`QBDI::MemoryAccessFlags::MEMORY_UNKNOWN_SIZE`: the size of the access isn't known. Only the address is valid.
      The flag is only set for instruction with REP prefix before the execution of the instruction.
    * :cpp:enumerator:`QBDI::MemoryAccessFlags::MEMORY_MINIMUM_SIZE`: the size isn't the real size of the access, but the expected minimal size.
      This flag is used for instruction with complex access like ``XSAVE`` and ``XRSTOR``.
    * :cpp:enumerator:`QBDI::MemoryAccessFlags::MEMORY_UNKNOWN_VALUE`: the value of the access hasn't been saved.
      The more common reason is that the access size is greater than the size of :cpp:member:`QBDI::MemoryAccess::value`.
      This flag is also used for instruction with REP prefix when the access size cannot be determined during the instrumentation.

  * Fix MemoryAccess for some generic instruction.

* Add VM Options. (`#144 <https://github.com/QBDI/QBDI/pull/144>`_)

  Some options can be provided to the VM to enable or disable some features:

  * :cpp:enumerator:`QBDI::Options::OPT_DISABLE_FPR`: Disable FPRState backup and restore in context switches.
    Only the GPRState will be used.
  * :cpp:enumerator:`QBDI::Options::OPT_DISABLE_OPTIONAL_FPR`: When :cpp:enumerator:`QBDI::Options::OPT_DISABLE_FPR` isn't selected,
    QBDI will detect if a BasicBlock needs FPRState. When BasicBlock doesn't need FPRState, the state will not be restored.
    This option forces the restoration and backup of FPRState to every BasicBlock.
  * :cpp:enumerator:`QBDI::Options::OPT_ATT_SYNTAX` for X86 and X86_64: :cpp:member:`QBDI::InstAnalysis::disassembly` will be in
    AT&T syntax instead of Intel Syntax.

* Rework documentation (`#156 <https://github.com/QBDI/QBDI/pull/156>`_)

Internal update:

* Update LLVM to LLVM 10.0.1 (`#104 <https://github.com/QBDI/QBDI/pull/104>`_ and `#139 <https://github.com/QBDI/QBDI/pull/139>`_)
* Reduce LLVM library included in QBDI static library and reduce QBDI package size (`#139 <https://github.com/QBDI/QBDI/pull/139>`_ and `#70 <https://github.com/QBDI/QBDI/issues/70>`_)
* Replace GTest by `Catch2 <https://github.com/catchorg/Catch2>`_ (`#140 <https://github.com/QBDI/QBDI/pull/140>`_)
* Refactor code and switch to cpp17 (`#140 <https://github.com/QBDI/QBDI/pull/140>`_ and `#155 <https://github.com/QBDI/QBDI/pull/155>`_)
* Use Github Actions to build dev-next package of QBDI (linux, osx and android) and PyQBDI (linux and osx) (`#147 <https://github.com/QBDI/QBDI/pull/147>`_, `#159 <https://github.com/QBDI/QBDI/pull/159>`_)
* Rewrite frida-qbdi.js and use sphinx-js for frida-QBDI documentation (`#146 <https://github.com/QBDI/QBDI/pull/146>`_).
  A version of frida greater or equals to 14.0 is needed to run frida-qbdi.js (need support of ES2019).
* Refactor MemoryAccess Code and add new tests (`#154 <https://github.com/QBDI/QBDI/pull/154>`_)
* Handle VMCallback return value (`#155 <https://github.com/QBDI/QBDI/pull/155>`_)
* Optimize Context Switch and FPRState restoration (`#144 <https://github.com/QBDI/QBDI/pull/144>`_)
* Add commit hash in devel version (`#158 <https://github.com/QBDI/QBDI/pull/158>`_)

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
