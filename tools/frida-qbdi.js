/*
 * This file is part of QBDI.
 *
 * Copyright 2017 Quarkslab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
'use strict';

/*
 * Usage:
 * $ frida -n Twitter -l frida-qbdi.js
 *
 */
var QBDI_MAJOR = 0;
var QBDI_MINOR = 6;
var QBDI_PATCH = 2;
var QBDI_MINIMUM_VERSION = (QBDI_MAJOR << 8) | (QBDI_MINOR << 4) | QBDI_PATCH;
/**data:QBDI_MINIMUM_VERSION
  Minimum version of QBDI to use Frida bindings
 */

if (typeof Duktape === 'object') {
    // Warn about duktape runtime (except on iOS...)
    if (Process.platform !== 'darwin' || Process.arch.indexOf("arm") !== 0) {
        console.warn("[!] Warning: using duktape runtime is much slower...");
        console.warn("    => Frida --enable-jit option should be used");
    }
}

// Provide a generic and "safe" (no exceptions if symbol is not found) way to load
// a library and bind/create a native function
function Binder() {
    this.findLibrary = function(lib, paths) {
        if (lib === undefined) {
            return undefined;
        }
        var cpath = undefined;
        if (paths !== undefined) {
            var cnt = paths.length;
            var found = false;
            // try to find our library
            for (var i = 0; i < cnt; i++) {
                cpath = paths[i] + lib;
                // use Frida file interface to test if file exists...
                try {
                    var fp = new File(cpath, "rb");
                    fp.close();
                    found = true;
                    break;
                } catch(e) {
                    continue;
                }
            }
            if (!found) {
                return undefined;
            }
        } else {
            cpath = lib;
        }
        return cpath;
    }

    function safeNativeFunction(cbk, ret, args) {
        var e = cbk();
        if (!e) {
            return undefined;
        }
        return new NativeFunction(e, ret, args);
    }

    Object.defineProperty(this, 'safeNativeFunction', {
        enumerable: false,
        value: safeNativeFunction
    });
}

Binder.prototype = {
    load: function(lib, paths) {
        var cpath = this.findLibrary(lib, paths);
        if (cpath === undefined) {
            var errmsg = lib + ' library not found...';
            console.error(errmsg);
            throw new Error(errmsg);
        }
        // load library
        var handle = System.dlopen(cpath);
        if (handle.isNull()) {
            var errmsg = 'Failed to load ' + cpath + ' (' + System.dlerror() + ')';
            console.error(errmsg);
            throw new Error(errmsg);
        }
        return cpath;
    },
    bind: function(name, ret, args) {
        return this.safeNativeFunction(function() {
            return Module.findExportByName(null, name);
        }, ret, args);
    }
}


function QBDIBinder() {
    /**attribute:QBDIBinder.QBDI_LIB
      QBDI library name
     */
    Object.defineProperty(this, 'QBDI_LIB', {
        enumerable: true,
        get: function () {
            return {
                'linux': 'libQBDI.so',
                'darwin': 'libQBDI.dylib',
                'windows': 'QBDI.dll',
            }[Process.platform];
        }
    });

    // paths where QBDI library may be
    Object.defineProperty(this, 'QBDI_PATHS', {
        enumerable: true,
        get: function () {
            return [
                // UNIX default paths
                '/usr/lib/',
                '/usr/local/lib/',
                // advised Android path
                '/data/local/tmp/',
                // in case of a local archive
                './',
                './lib',
                // Windows default path
                'C:\\Program Files\\QBDI ' + QBDI_MAJOR + '.' + QBDI_MINOR + '.' + QBDI_PATCH + '\\lib\\'
            ];
        }
    });

    Binder.call(this);
}

QBDIBinder.prototype = Object.create(Binder.prototype, {
    bind: {
        value: function(name, ret, args) {
            var libpath = this.QBDI_LIB;
            return this.safeNativeFunction(function() {
                return Module.findExportByName(libpath, name);
            }, ret, args);
        },
        enumerable: true,
    },
    load: {
        value: function() {
            return Binder.prototype.load.apply(this, [this.QBDI_LIB, this.QBDI_PATHS]);
        },
        enumerable: true,
    }
});
QBDIBinder.prototype.constructor = Binder;

var _binder = new Binder();
var _qbdibinder = new QBDIBinder();


// Needed to load QBDI
var System_C = Object.freeze({
    LoadLibraryEx: _binder.bind('LoadLibraryExA', 'pointer', ['pointer', 'int', 'int']),
    GetLastError: _binder.bind('GetLastError', 'int', []),
    dlopen: _binder.bind('dlopen', 'pointer', ['pointer', 'int']),
    dlerror: _binder.bind('dlerror', 'pointer', []),
    free: _binder.bind('free', 'void', ['pointer']),
});


var System = Object.freeze({
    dlerror: function() {
        if (Process.platform === "windows"){
            var val = System_C.GetLastError();
            if (val === undefined) {
                return undefined;
            }
            return val.toString();
        }
        var strPtr = System_C.dlerror();
        return Memory.readCString(strPtr);

    },
    dlopen: function(library) {
        var RTLD_LOCAL = 0x0;
        var RTLD_LAZY = 0x1;
        var path = Memory.allocUtf8String(library);
        if (Process.platform === "windows"){
            return System_C.LoadLibraryEx(path, 0, 0);
        }
        return System_C.dlopen(path, RTLD_LOCAL | RTLD_LAZY);
    },
    free: function(ptr) {
        System_C.free(ptr);
    }
});

// Load QBDI library
var QBDI_LIB_FULLPATH = _qbdibinder.load();
/**data:QBDI_LIB_FULLPATH
  Fullpath of the QBDI library
 */

// Define rword type and interfaces

/**data:rword
  An alias to Frida uint type with the size of general registers (**uint64** or **uint32**)
 */
var rword = Process.pointerSize === 8 ? 'uint64' : 'uint32';

Memory.readRword = Process.pointerSize === 8 ? Memory.readU64 : Memory.readU32;

// Convert a number to its register-sized representation

/**function:NativePointer.prototype.toRword()
  Convert a NativePointer into a type with the size of a register (``Number`` or ``UInt64``).
 */
NativePointer.prototype.toRword = function() {
    // Nothing better really ?
    if (Process.pointerSize === 8) {
        return uint64("0x" + this.toString(16));
    }
    return parseInt(this.toString(16), 16);
}

/**function:Number.prototype.toRword()
  Convert a number into a type with the size of a register (``Number`` or ``UInt64``).
  Can't be used for numbers > 32 bits, would cause weird results due to IEEE-754.
 */
Number.prototype.toRword = function() {
    if (this > 0x100000000)
    {
        throw new TypeError('For integer > 32 bits, please use Frida uint64 type.');
    }
    if (Process.pointerSize === 8) {
        return uint64(this);
    }
    return this;
}

/**function:UInt64.prototype.toRword()
  An identity function (returning the same ``UInt64`` object).
  It exists only to provide a unified **toRword** interface.
 */
UInt64.prototype.toRword = function() {
    return this;
}

// Some helpers

String.prototype.leftPad = function(paddingValue, paddingLength) {
    paddingLength = paddingLength || paddingValue.length;
    if (paddingLength < this.length) {
        return String(this);
    }
    return String(paddingValue + this).slice(-paddingLength);
};

/**function:String.prototype.toRword()
  Convert a String into a type with the size of a register (``Number`` or ``UInt64``).
 */
String.prototype.toRword = function() {
    return ptr(this).toRword()
};

/**function:hexPointer(ptr)
  This function is used to pretty print a pointer, padded with 0 to the size of a register.

  :param ptr: Pointer you want to pad

  :returns: pointer value as padded string (ex: "0x00004242")
  :rtype: String
 */
function hexPointer(ptr) {
    return ptr.toString(16).leftPad("0000000000000000", Process.pointerSize * 2);
}


//
var QBDI_C = Object.freeze({
    // VM
    initVM: _qbdibinder.bind('qbdi_initVM', 'void', ['pointer', 'pointer', 'pointer']),
    terminateVM: _qbdibinder.bind('qbdi_terminateVM', 'void', ['pointer']),
    addInstrumentedRange: _qbdibinder.bind('qbdi_addInstrumentedRange', 'void', ['pointer', rword, rword]),
    addInstrumentedModule: _qbdibinder.bind('qbdi_addInstrumentedModule', 'uchar', ['pointer', 'pointer']),
    addInstrumentedModuleFromAddr: _qbdibinder.bind('qbdi_addInstrumentedModuleFromAddr', 'uchar', ['pointer', rword]),
    instrumentAllExecutableMaps: _qbdibinder.bind('qbdi_instrumentAllExecutableMaps', 'uchar', ['pointer']),
    removeInstrumentedRange: _qbdibinder.bind('qbdi_removeInstrumentedRange', 'void', ['pointer', rword, rword]),
    removeInstrumentedModule: _qbdibinder.bind('qbdi_removeInstrumentedModule', 'uchar', ['pointer', 'pointer']),
    removeInstrumentedModuleFromAddr: _qbdibinder.bind('qbdi_removeInstrumentedModuleFromAddr', 'uchar', ['pointer', rword]),
    removeAllInstrumentedRanges: _qbdibinder.bind('qbdi_removeAllInstrumentedRanges', 'void', ['pointer']),
    run: _qbdibinder.bind('qbdi_run', 'uchar', ['pointer', rword, rword]),
    call: _qbdibinder.bind('qbdi_call', 'uchar', ['pointer', 'pointer', rword, 'uint32',
                           rword, rword, rword, rword, rword, rword, rword, rword, rword, rword]),
    getGPRState: _qbdibinder.bind('qbdi_getGPRState', 'pointer', ['pointer']),
    getFPRState: _qbdibinder.bind('qbdi_getFPRState', 'pointer', ['pointer']),
    setGPRState: _qbdibinder.bind('qbdi_setGPRState', 'void', ['pointer', 'pointer']),
    setFPRState: _qbdibinder.bind('qbdi_setFPRState', 'void', ['pointer', 'pointer']),
    addMnemonicCB: _qbdibinder.bind('qbdi_addMnemonicCB', 'uint32', ['pointer', 'pointer', 'uint32', 'pointer', 'pointer']),
    addMemAccessCB: _qbdibinder.bind('qbdi_addMemAccessCB', 'uint32', ['pointer', 'uint32', 'pointer', 'pointer']),
    addMemAddrCB: _qbdibinder.bind('qbdi_addMemAddrCB', 'uint32', ['pointer', rword, 'uint32', 'pointer', 'pointer']),
    addMemRangeCB: _qbdibinder.bind('qbdi_addMemRangeCB', 'uint32', ['pointer', rword, rword, 'uint32', 'pointer', 'pointer']),
    addCodeCB: _qbdibinder.bind('qbdi_addCodeCB', 'uint32', ['pointer', 'uint32', 'pointer', 'pointer']),
    addCodeAddrCB: _qbdibinder.bind('qbdi_addCodeAddrCB', 'uint32', ['pointer', rword, 'uint32', 'pointer', 'pointer']),
    addCodeRangeCB: _qbdibinder.bind('qbdi_addCodeRangeCB', 'uint32', ['pointer', rword, rword, 'uint32', 'pointer', 'pointer']),
    addVMEventCB: _qbdibinder.bind('qbdi_addVMEventCB', 'uint32', ['pointer', 'uint32', 'pointer', 'pointer']),
    deleteInstrumentation: _qbdibinder.bind('qbdi_deleteInstrumentation', 'uchar', ['pointer', 'uint32']),
    deleteAllInstrumentations: _qbdibinder.bind('qbdi_deleteAllInstrumentations', 'void', ['pointer']),
    getInstAnalysis: _qbdibinder.bind('qbdi_getInstAnalysis', 'pointer', ['pointer', 'uint32']),
    recordMemoryAccess: _qbdibinder.bind('qbdi_recordMemoryAccess', 'uchar', ['pointer', 'uint32']),
    getInstMemoryAccess: _qbdibinder.bind('qbdi_getInstMemoryAccess', 'pointer', ['pointer', 'pointer']),
    getBBMemoryAccess: _qbdibinder.bind('qbdi_getBBMemoryAccess', 'pointer', ['pointer', 'pointer']),
    // Memory
    allocateVirtualStack: _qbdibinder.bind('qbdi_allocateVirtualStack', 'uchar', ['pointer', 'uint32', 'pointer']),
    alignedAlloc: _qbdibinder.bind('qbdi_alignedAlloc', 'pointer', ['uint32', 'uint32']),
    alignedFree: _qbdibinder.bind('qbdi_alignedFree', 'void', ['pointer']),
    simulateCall: _qbdibinder.bind('qbdi_simulateCall', 'void', ['pointer', rword, 'uint32',
                                   rword, rword, rword, rword, rword, rword, rword, rword, rword, rword]),
    getModuleNames: _qbdibinder.bind('qbdi_getModuleNames', 'pointer', ['pointer']),
    // Logs
    addLogFilter: _qbdibinder.bind('qbdi_addLogFilter', 'void', ['pointer', 'uint32']),
    // Helpers
    getVersion: _qbdibinder.bind('qbdi_getVersion', 'pointer', ['pointer']),
    getGPR: _qbdibinder.bind('qbdi_getGPR', rword, ['pointer', 'uint32']),
    setGPR: _qbdibinder.bind('qbdi_setGPR', 'void', ['pointer', 'uint32', rword]),
    getMemoryAccessStructDesc: _qbdibinder.bind('qbdi_getMemoryAccessStructDesc', 'pointer', []),
    getVMStateStructDesc: _qbdibinder.bind('qbdi_getVMStateStructDesc', 'pointer', []),
    getOperandAnalysisStructDesc: _qbdibinder.bind('qbdi_getOperandAnalysisStructDesc', 'pointer', []),
    getInstAnalysisStructDesc: _qbdibinder.bind('qbdi_getInstAnalysisStructDesc', 'pointer', []),
    precacheBasicBlock: _qbdibinder.bind('qbdi_precacheBasicBlock', 'uchar', ['pointer', rword]),
    clearCache: _qbdibinder.bind('qbdi_clearCache', 'void', ['pointer', rword, rword]),
    clearAllCache: _qbdibinder.bind('qbdi_clearAllCache', 'void', ['pointer']),
});

// Init some globals
if (Process.arch === 'x64') {
    /**data:GPR_NAMES
      An array holding register names.
     */
    /**data:REG_RETURN
      A constant string representing the register carrying the return value of a function.
     */
    /**data:REG_PC
      String of the instruction pointer register.
     */
    /**data:REG_SP
      String of the stack pointer register.
     */
    var GPR_NAMES = ["RAX","RBX","RCX","RDX","RSI","RDI","R8","R9","R10","R11","R12","R13","R14","R15","RBP","RSP","RIP","EFLAGS"];
    var REG_RETURN = "RAX";
    var REG_PC = "RIP";
    var REG_SP = "RSP";
} else if (Process.arch === 'arm') {
    var GPR_NAMES = ["R0","R1","R2","R3","R4","R5","R6","R7","R8","R9","R10","R12","FP","SP","LR","PC","CPSR"];
    var REG_RETURN = "R0";
    var REG_PC = "PC";
    var REG_SP = "SP";
} else if (Process.arch === 'ia32'){
    var GPR_NAMES = ["EAX","EBX","ECX","EDX","ESI","EDI","EBP","ESP","EIP","EFLAGS"];
    var REG_RETURN = "EAX";
    var REG_PC = "EIP";
    var REG_SP = "ESP";
}

/**data:VMError
 */
var VMError = Object.freeze({
    /**attribute:VMError.INVALID_EVENTID
      Returned event is invalid.
     */
    INVALID_EVENTID: 0xffffffff
});

/**data:SyncDirection
  Synchronisation direction between Frida and QBDI GPR contexts
 */
var SyncDirection = Object.freeze({
    /**attribute:SyncDirection.QBDI_TO_FRIDA
      Constant variable used to synchronize QBDI's context to Frida's.

      .. warning:: This is currently not supported due to the lack of context updating in Frida.
     */
    QBDI_TO_FRIDA: 0,
    /**attribute:SyncDirection.FRIDA_TO_QBDI
      Constant variable used to synchronize Frida's context to QBDI's.
     */
    FRIDA_TO_QBDI: 1
});

/**data:VMAction
  The callback results.
 */
var VMAction = Object.freeze({
    /**attribute:VMAction.CONTINUE
      The execution of the basic block continues.
     */
    CONTINUE: 0,
    /**attribute:VMAction.BREAK_TO_VM
      The execution breaks and returns to the VM causing a complete reevaluation of
      the execution state. A :js:data:`VMAction.BREAK_TO_VM` is needed to ensure that modifications of
      the Program Counter or the program code are taken into account.
     */
    BREAK_TO_VM: 1,
    /**attribute:VMAction.STOP
      Stops the execution of the program. This causes the run function to return early.
     */
    STOP: 2
});


/**data:InstPosition
  Position relative to an instruction.
*/
var InstPosition = Object.freeze({
    /**attribute:InstPosition.PREINST
      Positioned **before** the instruction..
     */
    PREINST: 0,
    /**attribute:InstPosition.POSTINST
      Positioned **after** the instruction..
     */
    POSTINST: 1
});

/**data:VMEvent
  Events triggered by the virtual machine.
*/
var VMEvent = Object.freeze({
    /**attribute:VMEvent.SEQUENCE_ENTRY
      Triggered when the execution enters a sequence.
     */
    SEQUENCE_ENTRY     : 1,
    /**attribute:VMEvent.SEQUENCE_EXIT
      Triggered when the execution exits from the current sequence.
     */
    SEQUENCE_EXIT      : 1<<1,
    /**attribute:VMEvent.BASIC_BLOCK_ENTRY
      Triggered when the execution enters a basic block.
     */
    BASIC_BLOCK_ENTRY     : 1<<2,
    /**attribute:VMEvent.BASIC_BLOCK_EXIT
      Triggered when the execution exits from the current basic block.
     */
    BASIC_BLOCK_EXIT      : 1<<3,
    /**attribute:VMEvent.BASIC_BLOCK_NEW
      Triggered when the execution enters a new (~unknown) basic block.
     */
    BASIC_BLOCK_NEW       : 1<<4,
    /**attribute:VMEvent.EXEC_TRANSFER_CALL
      Triggered when the ExecBroker executes an execution transfer.
     */
    EXEC_TRANSFER_CALL    : 1<<5,
    /**attribute:VMEvent.EXEC_TRANSFER_RETURN
       Triggered when the ExecBroker returns from an execution transfer.
     */
    EXEC_TRANSFER_RETURN  : 1<<6,
    /**attribute:VMEvent.SYSCALL_ENTRY
      Not implemented.
     */
    SYSCALL_ENTRY         : 1<<7,
    /**attribute:VMEvent.SYSCALL_EXIT
      Not implemented.
     */
    SYSCALL_EXIT          : 1<<8,
    /**attribute:VMEvent.SIGNAL
      Not implemented.
     */
    SIGNAL                : 1<<9
});

/**data:MemoryAccessType
  Memory access type (read / write / ...)
*/
var MemoryAccessType = Object.freeze({
    /**attribute:MemoryAccessType.MEMORY_READ
      Memory read access.
     */
    MEMORY_READ : 1,
    /**attribute:MemoryAccessType.MEMORY_WRITE
      Memory write access.
     */
    MEMORY_WRITE : 2,
    /**attribute:MemoryAccessType.MEMORY_READ_WRITE
      Memory read/write access.
     */
    MEMORY_READ_WRITE : 3
});

/**data:RegisterAccessType
  Register access type (read / write / rw)
*/
var RegisterAccessType = Object.freeze({
    /**attribute:RegisterAccessType.REGISTER_READ
      Register is read.
     */
    REGISTER_READ : 1,
    /**attribute:RegisterAccessType.REGISTER_WRITE
      Register is written.
     */
    REGISTER_WRITE : 2,
    /**attribute:RegisterAccessType.REGISTER_READ_WRITE
      Register is read/written.
     */
    REGISTER_READ_WRITE : 3
});

/**data:OperandType
  Register access type (read / write / rw)
*/
var OperandType = Object.freeze({
    /**attribute:OperandType.OPERAND_INVALID
      Invalid operand.
     */
    OPERAND_INVALID : 0,
    /**attribute:OperandType.OPERAND_IMM
      Immediate operand.
     */
    OPERAND_IMM : 1,
    /**attribute:OperandType.OPERAND_GPR
      General purpose register operand.
     */
    OPERAND_GPR : 2,
    /**attribute:OperandType.OPERAND_PRED
      Predicate special operand.
     */
    OPERAND_PRED : 3
});

/**data:AnalysisType
  Properties to retrieve during an instruction analysis.
*/
var AnalysisType = Object.freeze({
    /**attribute:AnalysisType.ANALYSIS_INSTRUCTION
      Instruction analysis (address, mnemonic, ...).
     */
    ANALYSIS_INSTRUCTION : 1,
    /**attribute:AnalysisType.ANALYSIS_DISASSEMBLY
      Instruction disassembly.
     */
    ANALYSIS_DISASSEMBLY : 1<<1,
    /**attribute:AnalysisType.ANALYSIS_OPERANDS
      Instruction operands analysis.
     */
    ANALYSIS_OPERANDS : 1<<2,
    /**attribute:AnalysisType.ANALYSIS_SYMBOL
      Instruction nearest symbol (and offset).
     */
    ANALYSIS_SYMBOL : 1<<3
});

/**class:QBDI
 State class
*/
function State(state) {
    var statePtr = null;

    function initialize(s) {
        if (!NativePointer.prototype.isPrototypeOf(s) || s.isNull()) {
            throw new TypeError('Invalid state pointer');
        }
        statePtr = s;
    }

    Object.defineProperty(this, 'ptr', {
        enumerable: false,
        get: function () {
            return statePtr;
        }
    });

    this.toRword = function() {
        return statePtr.toRword();
    }

    this.toString = function() {
        return statePtr.toString();
    }

    initialize.call(this, state);
}


/**class:QBDI
 GPR State class
*/
function GPRState(state) {
    function getGPRId(rid) {
        if (typeof(rid) === 'string') {
            rid = GPR_NAMES.indexOf(rid.toUpperCase());
        }
        if (rid < 0 || rid > GPR_NAMES.length) {
            return undefined;
        }
        return rid;
    }

    this.getRegister = function(rid) {
        /**:GPRState.prototype.getRegister(rid)
          This function is used to get the value of a specific register.

          :param rid: Register (register name or ID can be used e.g : "RAX", "rax", 0)

          :returns: GPR value (ex: 0x42)
          :rtype: ``NativePointer``
         */
        var rid = getGPRId(rid);
        if (rid === null) {
            return undefined;
        }
        return ptr(QBDI_C.getGPR(this.ptr, rid));
    }

    this.setRegister = function(rid, value) {
        /**:GPRState.prototype.setRegister(rid, value)
          This function is used to set the value of a specific register.

          :param rid: Register (register name or ID can be used e.g : "RAX", "rax", 0)
          :param value: Register value (use **strings** for big integers)
        */
        var rid = getGPRId(rid);
        if (rid !== null) {
            QBDI_C.setGPR(this.ptr, rid, value.toRword());
        }
    }

    this.getRegisters = function() {
        /**:GPRState.prototype.getRegisters()
          This function is used to get values of all registers.

          :returns: GPRs of current context (ex: {"RAX":0x42, ...})
          :rtype: {String:rword, ...}
        */
        var regCnt = GPR_NAMES.length;
        var gprs = {};
        for (var i = 0; i < regCnt; i++) {
            gprs[GPR_NAMES[i]] = this.getRegister(i);
        }
        return gprs;
    }

    this.setRegisters = function(gprs) {
        /**:GPRState.prototype.setRegisters(gprs)
          This function is used to set values of all registers.

          :param gprs: Array of register values
        */
        var regCnt = GPR_NAMES.length;
        for (var i = 0; i < regCnt; i++) {
            this.setRegister(i, gprs[GPR_NAMES[i]]);
        }
    }

    this.synchronizeRegister = function(FridaCtx, rid, direction) {
        /**:GPRState.prototype.synchronizeRegister(FridaCtx, rid, direction)
         This function is used to synchronise a specific register between Frida and QBDI.

         :param FridaCtx: Frida context
         :param rid: Register (register name or ID can be used e.g : "RAX", "rax", 0)
         :param direction: Synchronization direction. (:js:data:`FRIDA_TO_QBDI` or :js:data:`QBDI_TO_FRIDA`)

         .. warning:: Currently QBDI_TO_FRIDA is experimental. (E.G : RIP cannot be synchronized)
        */
        if (direction === SyncDirection.FRIDA_TO_QBDI) {
            this.setRegister(rid, FridaCtx[rid.toLowerCase()].toRword());
        }
        else { // FRIDA_TO_QBDI
            FridaCtx[rid.toLowerCase()] = ptr(this.getRegister(rid).toString());
        }
    }

    this.synchronizeContext = function(FridaCtx, direction) {
        /**:GPRState.prototype.synchronizeContext(FridaCtx, direction)
         This function is used to synchronise context between Frida and QBDI.

         :param FridaCtx: Frida context
         :param direction: Synchronization direction. (:js:data:`FRIDA_TO_QBDI` or :js:data:`QBDI_TO_FRIDA`)

         .. warning:: Currently QBDI_TO_FRIDA is not implemented (due to Frida limitations).
        */
        for (var i in GPR_NAMES) {
            if (GPR_NAMES[i] === "EFLAGS") {
                continue;
            }
            this.synchronizeRegister(FridaCtx, GPR_NAMES[i], direction);
        }
        if (direction === SyncDirection.QBDI_TO_FRIDA) {
            throw new Error('Not implemented (does not really work due to Frida)');
        }
    }

    this.pp = function(color) {
        /**:GPRState.prototype.pp([color])
          Pretty print QBDI context.

          :param [color]: Will print a colored version of the context if set.

          :returns: dump of all GPRs in a pretty format
          :rtype: String
        */
        var RED = color ? "\x1b[31m" : "";
        var GREEN = color ? "\x1b[32m" : "";
        var RESET = color ?"\x1b[0m" : "";
        var regCnt = GPR_NAMES.length;
        var regs = this.getRegisters();
        var line = "";
        for (var i = 0; i < regCnt; i++) {
            var name = GPR_NAMES[i];
            if (!(i % 4) && i) {
                line += '\n';
            }
            line+=GREEN; // Will be overwritten by RED if necessary
            if (name === "RIP" | name === "PC"){
                line += RED;
            }
            line += name.leftPad("   ") + RESET + "=0x" + hexPointer(regs[name]) + " ";
        }
        return line;
    }

    this.dump = function(color) {
        /**:GPRState.prototype.dump([color])
          Pretty print and log QBDI context.

          :param [color]: Will print a colored version of the context if set.
        */
        console.log(this.pp(color));
    }

    State.call(this, state);
}
GPRState.prototype = Object.create(State.prototype);
GPRState.prototype.constructor = GPRState;

GPRState.validOrThrow = function(state) {
    if (!GPRState.prototype.isPrototypeOf(state)) {
        throw new TypeError('Invalid GPRState');
    }
}

/**class:QBDI
 FPR State class
*/
function FPRState(state) {
    State.call(this, state);
}
FPRState.prototype = Object.create(State.prototype);
FPRState.prototype.constructor = FPRState;

FPRState.validOrThrow = function(state) {
    if (!FPRState.prototype.isPrototypeOf(state)) {
        throw new TypeError('Invalid FPRState');
    }
}


/**class:QBDI
 QBDI VM class
*/
function QBDI() {
    // QBDI VM Instance pointer
    var vm = null;
    // Cache various remote structure descriptions
    var memoryAccessDesc = null;
    var operandAnalysisStructDesc = null;
    var instAnalysisStructDesc = null;
    var vmStateStructDesc = null;
    // Keep a reference to objects used as user callback data
    var userDataPtrMap = {};
    var userDataIIdMap = {};

    Object.defineProperty(this, 'ptr', {
        enumerable: false,
        get: function () {
            return vm;
        }
    });

    function parseStructDesc(ptr) {
        var desc = {};
        desc.size = Memory.readU32(ptr);
        ptr = ptr.add(4);
        desc.items = Memory.readU32(ptr);
        ptr = ptr.add(4);
        desc.offsets = [];
        for (var i = 0; i < desc.items; i++) {
            var offset = Memory.readU32(ptr);
            ptr = ptr.add(4);
            desc.offsets.push(offset);
        }
        Object.freeze(desc);
        return desc;
    }

    // VM
    function initVM() {
        /**:_QBDI
            Create a new instrumentation virtual machine using "**new QBDI()**"

            :returns:   QBDI virtual machine instance
            :rtype:     object
        */
        var vmPtr = Memory.alloc(Process.pointerSize);
        QBDI_C.initVM(vmPtr, NULL, NULL);
        return Memory.readPointer(vmPtr);
    }

    function terminateVM(v) {
        QBDI_C.terminateVM(v);
    }

    function initialize () {
        // Enforce a minimum QBDI version (API compatibility)
        if (!this.version || this.version.integer < QBDI_MINIMUM_VERSION) {
            throw new Error('Invalid QBDI version !');
        }

        // Create VM instance
        vm = initVM();

        // Parse remote structure descriptions
        memoryAccessDesc = parseStructDesc(QBDI_C.getMemoryAccessStructDesc());
        operandAnalysisStructDesc = parseStructDesc(QBDI_C.getOperandAnalysisStructDesc());
        instAnalysisStructDesc = parseStructDesc(QBDI_C.getInstAnalysisStructDesc());
        vmStateStructDesc = parseStructDesc(QBDI_C.getVMStateStructDesc());
    }

    // add a destructor on garbage collection
    WeakRef.bind(QBDI, function dispose () {
        if (vm !== null) {
            terminateVM(vm);
        }
    });

    this.addInstrumentedRange = function(start, end) {
        /**:QBDI.prototype.addInstrumentedRange(start, end)
            Add an address range to the set of instrumented address ranges.

            :param start:  Start address of the range (included).
            :param end:    End address of the range (excluded).
        */
        QBDI_C.addInstrumentedRange(vm, start.toRword(), end.toRword());
    }

    this.addInstrumentedModule = function(name) {
        /**:QBDI.prototype.addInstrumentedModule(name)
            Add the executable address ranges of a module to the set of instrumented address ranges.

            :param name:   The module's name.

            :returns:   True if at least one range was added to the instrumented ranges.
            :rtype:     boolean
        */
        var namePtr = Memory.allocUtf8String(name);
        return QBDI_C.addInstrumentedModule(vm, namePtr) == true;
    }

    this.addInstrumentedModuleFromAddr = function(addr) {
        /**:QBDI.prototype.addInstrumentedModuleFromAddr(addr)
            Add the executable address ranges of a module to the set of instrumented address ranges. using an address belonging to the module.

            :param addr:   An address contained by module's range.

            :returns:   True if at least one range was removed from the instrumented ranges.
            :rtype:     boolean
        */
        return QBDI_C.addInstrumentedModuleFromAddr(vm, addr.toRword()) == true;
    }

    this.instrumentAllExecutableMaps = function() {
        /**:QBDI.prototype.instrumentAllExecutableMaps()
            Adds all the executable memory maps to the instrumented range set.

            :returns:   True if at least one range was added to the instrumented ranges.
            :rtype:     boolean
        */
        return QBDI_C.instrumentAllExecutableMaps(vm) == true;
    }

    this.removeInstrumentedRange = function(start, end) {
        /**:QBDI.prototype.removeInstrumentedRange(start, end)
            Remove an address range from the set of instrumented address ranges.

            :param start:  Start address of the range (included).
            :param end:    End address of the range (excluded).
        */
        QBDI_C.removeInstrumentedRange(vm, start.toRword(), end.toRword());
    }

    this.removeInstrumentedModule = function(name) {
        /**:QBDI.prototype.removeInstrumentedModule(name)
            Remove the executable address ranges of a module from the set of instrumented address ranges.

            :param name:   The module's name.

            :returns:   True if at least one range was added to the instrumented ranges.
            :rtype:     boolean
        */
        var namePtr = Memory.allocUtf8String(name);
        return QBDI_C.removeInstrumentedModule(vm, namePtr) == true;
    }

    this.removeInstrumentedModuleFromAddr = function(addr) {
        return QBDI_C.removeInstrumentedModuleFromAddr(vm, addr.toRword()) == true;
    }

    this.removeAllInstrumentedRanges = function() {
        QBDI_C.removeAllInstrumentedRanges(vm);
    }

    this.run = function(start, stop) {
        /**:QBDI.prototype.run(start, stop)
            Start the execution by the DBI from a given address (and stop when another is reached).

            :param start:  Address of the first instruction to execute.
            :param stop:   Stop the execution when this instruction is reached.

            :returns:   True if at least one block has been executed.
            :rtype:     boolean
        */
        return QBDI_C.run(vm, start.toRword(), stop.toRword()) == true;
    }

    this.getGPRState = function() {
        /**:QBDI.prototype.getGPRState(state)
            Obtain the current general register state.

            :returns:   An object containing the General Purpose Registers state.
            :rtype:     object
        */
        return new GPRState(QBDI_C.getGPRState(vm));
    }

    this.getFPRState = function() {
        /**:QBDI.prototype.getFPRState(state)
            Obtain the current floating point register state.

            :returns:   An object containing the Floating point Purpose Registers state.
            :rtype:     object
        */
        return new FPRState(QBDI_C.getFPRState(vm));
    }

    this.setGPRState = function(state) {
        /**:QBDI.prototype.setGPRState(state)
            Set the GPR state

            :param state:  Array of register values
        */
        GPRState.validOrThrow(state);
        QBDI_C.setGPRState(vm, state.ptr);
    }

    this.setFPRState = function(state) {
        /**:QBDI.prototype.setFPRState(state)
            Set the FPR state

            :param state:  Array of register values
        */
        FPRState.validOrThrow(state);
        QBDI_C.setFPRState(vm, state.ptr);
    }

    this.precacheBasicBlock = function(pc) {
        /**:QBDI.prototype.precacheBasicBlock(state)
            Pre-cache a known basic block.

            :param pc:  Start address of a basic block

            :returns: True if basic block has been inserted in cache.
            :rtype:     bool
        */
        return QBDI_C.precacheBasicBlock(vm, pc) == true
    }

    this.clearCache = function(start, end) {
        /**:QBDI.prototype.precacheBasicBlock(state)
            Clear a specific address range from the translation cache.

            :param start:  Start of the address range to clear from the cache.
            :param end:    End of the address range to clear from the cache.
        */
        QBDI_C.clearCache(vm, start, end)
    }

    this.clearAllCache = function() {
        /**:QBDI.prototype.precacheBasicBlock(state)
            Clear the entire translation cache.
        */
        QBDI_C.clearAllCache(vm)
    }


    // Retain (~reference) a user data object when an instrumentation is added.
    //
    // If a ``NativePointer`` is given, it will be used as raw user data and the
    // object will not be retained.
    function retainUserData(data, fn) {
        var dataPtr = data || NULL;
        var managed = false;
        if (!NativePointer.prototype.isPrototypeOf(data)) {
            dataPtr = Memory.alloc(4);
            managed = true;
        }
        var iid = fn(dataPtr);
        if (managed) {
            userDataPtrMap[dataPtr] = data;
            userDataIIdMap[iid] = dataPtr;
            Memory.writeU32(dataPtr, iid);
        }
        return iid;
    }

    // Retrieve a user data object from its ``NativePointer`` reference.
    // If pointer is NULL or no data object is found, the ``NativePointer``
    // object will be returned.
    function getUserData(dataPtr) {
        var data = dataPtr;
        if (!data.isNull()) {
            var d = userDataPtrMap[dataPtr];
            if (d !== undefined) {
                data = d;
            }
        }
        return data;
    }

    // Release references to a user data object using the correponding
    // instrumentation id.
    function releaseUserData(id) {
        var dataPtr = userDataIIdMap[id];
        if (dataPtr !== undefined) {
            delete userDataPtrMap[dataPtr];
            delete userDataIIdMap[id];
        }
    }

    // Release all references to user data objects.
    function releaseAllUserData() {
        userDataPtrMap = {};
        userDataIIdMap = {};
    }

    this.addMnemonicCB = function(mnem, pos, cbk, data) {
        /**:QBDI.prototype.addMnemonicCB(mnem, pos, cbk, data)
            Register a callback event if the instruction matches the mnemonic.

            :param mnem:   Mnemonic to match.
            :param pos:    Relative position of the event callback (PreInst / PostInst).
            :param cbk:    A function pointer to the callback.
            :param data:   User defined data passed to the callback.

            :returns:   The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
            :rtype:     integer
        */
        var mnemPtr = Memory.allocUtf8String(mnem);
        return retainUserData(data, function (dataPtr) {
            return QBDI_C.addMnemonicCB(vm, mnemPtr, pos, cbk, dataPtr);
        });
    }

    this.addMemAccessCB = function(type, cbk, data) {
        /**:QBDI.prototype.addMemAccessCB(type, cbk, data)
            Register a callback event for every memory access matching the type bitfield made by the instruction in the range codeStart to codeEnd.

            :param type:   A mode bitfield: either MEMORY_READ, MEMORY_WRITE or both (MEMORY_READ_WRITE).
            :param cbk:    A function pointer to the callback.
            :param data:   User defined data passed to the callback.

            :returns:   The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
            :rtype:     integer
        */
        return retainUserData(data, function (dataPtr) {
            return QBDI_C.addMemAccessCB(vm, type, cbk, dataPtr);
        });
    }

    this.addMemAddrCB = function(addr, type, cbk, data) {
        /**:QBDI.prototype.addMemAddrCB(addr, type, cbk, data)
            Add a virtual callback which is triggered for any memory access at a specific address matching the access type.
            Virtual callbacks are called via callback forwarding by a gate callback triggered on every memory access. This incurs a high performance cost.

            :param addr:   Code address which will trigger the callback.
            :param type:   A mode bitfield: either MEMORY_READ, MEMORY_WRITE or both (MEMORY_READ_WRITE).
            :param cbk:    A function pointer to the callback.
            :param data:   User defined data passed to the callback.

            :returns:   The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
            :rtype:     integer
        */
        return retainUserData(data, function (dataPtr) {
            return QBDI_C.addMemAddrCB(vm, addr.toRword(), type, cbk, dataPtr);
        });
    }

    this.addMemRangeCB = function(start, end, type, cbk, data) {
        /**:QBDI.prototype.addMemRangeCB(start, end, type, cbk, data)
          Add a virtual callback which is triggered for any memory access in a specific address range matching the access type.
          Virtual callbacks are called via callback forwarding by a gate callback triggered on every memory access. This incurs a high performance cost.

          :param start:    Start of the address range which will trigger the callback.
          :param end:      End of the address range which will trigger the callback.
          :param type:     A mode bitfield: either MEMORY_READ, MEMORY_WRITE or both (MEMORY_READ_WRITE).
          :param cbk:      A function pointer to the callback.
          :param data:     User defined data passed to the callback.

          :returns: The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
          :rtype:   integer
        */
        return retainUserData(data, function (dataPtr) {
            return QBDI_C.addMemRangeCB(vm, start.toRword(), end.toRword(), type, cbk, dataPtr);
        });
    }

    this.addCodeCB = function(pos, cbk, data) {
        /**:QBDI.prototype.addCodeCB(pos, cbk, data)
            Register a callback event for a specific instruction event.

            :param pos:    Relative position of the event callback (PreInst / PostInst).
            :param cbk:    A function pointer to the callback.
            :param data:   User defined data passed to the callback.

            :returns:   The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
            :rtype:     integer
        */
        return retainUserData(data, function (dataPtr) {
            return QBDI_C.addCodeCB(vm, pos, cbk, dataPtr);
        });
    }

    this.addCodeAddrCB = function(addr, pos, cbk, data) {
        /**:QBDI.prototype.addCodeAddrCB(addr, pos, cbk, data)
            Register a callback for when a specific address is executed.

            :param addr:   Code address which will trigger the callback.
            :param pos:    Relative position of the event callback (PreInst / PostInst).
            :param cbk:    A function pointer to the callback.
            :param data:   User defined data passed to the callback.

            :returns:   The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
            :rtype:     integer
        */
        return retainUserData(data, function (dataPtr) {
            return QBDI_C.addCodeAddrCB(vm, addr.toRword(), pos, cbk, dataPtr);
        });
    }

    this.addCodeRangeCB = function(start, end, pos, cbk, data) {
        /**:QBDI.prototype.addCodeRangeCB(start, end, pos, cbk, data)
            Register a callback for when a specific address range is executed.

            :param start:  Start of the address range which will trigger the callback.
            :param end:    End of the address range which will trigger the callback.
            :param pos:    Relative position of the event callback (PreInst / PostInst).
            :param cbk:    A function pointer to the callback.
            :param data:   User defined data passed to the callback.

            :returns:   The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
            :rtype:     integer
        */
        return retainUserData(data, function (dataPtr) {
            return QBDI_C.addCodeRangeCB(vm, start.toRword(), end.toRword(), pos, cbk, dataPtr);
        });
    }

    this.addVMEventCB = function(mask, cbk, data) {
        /**:QBDI.prototype.addVMEventCB(mask, cbk, data)
          Register a callback event for a specific VM event.

          :param mask: A mask of VM event type which will trigger the callback.
          :param cbk:  A function pointer to the callback.
          :param data: User defined data passed to the callback.

          :returns: The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
          :rtype:   integer
        */
        return retainUserData(data, function (dataPtr) {
            return QBDI_C.addVMEventCB(vm, mask, cbk, dataPtr);
        });
    }

    this.deleteInstrumentation = function(id) {
        /**:QBDI.prototype.deleteInstrumentation(id)
          Remove an instrumentation.

          :param id:   The id of the instrumentation to remove.
          :returns:     True if instrumentation has been removed.
          :rtype:       boolean
        */
        releaseUserData(id);
        return QBDI_C.deleteInstrumentation(vm, id) == true;
    }

    this.deleteAllInstrumentations = function() {
        /**:QBDI.prototype.deleteAllInstrumentations()
          Remove all the registered instrumentations.
        */
        releaseAllUserData();
        QBDI_C.deleteAllInstrumentations(vm);
    }

    function parseVMState(ptr) {
        var state = {};
        var p = ptr;
        state.event = Memory.readU8(p);
        p = ptr.add(vmStateStructDesc.offsets[1]);
        state.sequenceStart = Memory.readRword(p);
        p = ptr.add(vmStateStructDesc.offsets[2]);
        state.sequenceEnd = Memory.readRword(p);
        p = ptr.add(vmStateStructDesc.offsets[3]);
        state.basicBlockStart = Memory.readRword(p);
        p = ptr.add(vmStateStructDesc.offsets[4]);
        state.basicBlockEnd = Memory.readRword(p);
        p = ptr.add(vmStateStructDesc.offsets[5]);
        state.lastSignal = Memory.readRword(p);
        Object.freeze(state);
        return state;
    }

    function parseOperandAnalysis(ptr) {
        var analysis = {};
        var p = ptr;
        analysis.type = Memory.readU32(p);
        p = ptr.add(operandAnalysisStructDesc.offsets[1]);
        analysis.value = Memory.readRword(p);
        p = ptr.add(operandAnalysisStructDesc.offsets[2]);
        analysis.size = Memory.readU8(p);
        p = ptr.add(operandAnalysisStructDesc.offsets[3]);
        analysis.regOff = Memory.readU8(p);
        p = ptr.add(operandAnalysisStructDesc.offsets[4]);
        analysis.regCtxIdx = Memory.readU16(p);
        p = ptr.add(operandAnalysisStructDesc.offsets[5]);
        var regNamePtr = Memory.readPointer(p);
        if (regNamePtr.isNull()) {
            analysis.regName = undefined;
        } else {
            analysis.regName = Memory.readCString(regNamePtr);
        }
        p = ptr.add(operandAnalysisStructDesc.offsets[6]);
        analysis.regAccess = Memory.readU8(p);
        Object.freeze(analysis);
        return analysis;
    }

    function parseInstAnalysis(ptr) {
        var analysis = {};
        var p = ptr;
        analysis.mnemonic = Memory.readCString(Memory.readPointer(p));
        p = ptr.add(instAnalysisStructDesc.offsets[1]);
        analysis.disassembly = Memory.readCString(Memory.readPointer(p));
        p = ptr.add(instAnalysisStructDesc.offsets[2]);
        analysis.address = Memory.readRword(p);
        p = ptr.add(instAnalysisStructDesc.offsets[3]);
        analysis.instSize = Memory.readU32(p);
        p = ptr.add(instAnalysisStructDesc.offsets[4]);
        analysis.affectControlFlow = Memory.readU8(p) == true;
        p = ptr.add(instAnalysisStructDesc.offsets[5]);
        analysis.isBranch = Memory.readU8(p) == true;
        p = ptr.add(instAnalysisStructDesc.offsets[6]);
        analysis.isCall = Memory.readU8(p) == true;
        p = ptr.add(instAnalysisStructDesc.offsets[7]);
        analysis.isReturn = Memory.readU8(p) == true;
        p = ptr.add(instAnalysisStructDesc.offsets[8]);
        analysis.isCompare = Memory.readU8(p) == true;
        p = ptr.add(instAnalysisStructDesc.offsets[9]);
        analysis.isPredicable = Memory.readU8(p) == true;
        p = ptr.add(instAnalysisStructDesc.offsets[10]);
        analysis.mayLoad = Memory.readU8(p) == true;
        p = ptr.add(instAnalysisStructDesc.offsets[11]);
        analysis.mayStore = Memory.readU8(p) == true;
        p = ptr.add(instAnalysisStructDesc.offsets[12]);
        var numOperands = Memory.readU8(p);
        p = ptr.add(instAnalysisStructDesc.offsets[13]);
        var operandsPtr = Memory.readPointer(p);
        analysis.operands = new Array(numOperands);
        for (var i = 0; i < numOperands; i++) {
            analysis.operands[i] = parseOperandAnalysis(operandsPtr);
            operandsPtr = operandsPtr.add(operandAnalysisStructDesc.size);
        }
        p = ptr.add(instAnalysisStructDesc.offsets[14]);
        var symbolPtr = Memory.readPointer(p);
        if (!symbolPtr.isNull()) {
            analysis.symbol = Memory.readCString(symbolPtr);
        } else {
            analysis.symbol = "";
        }
        p = ptr.add(instAnalysisStructDesc.offsets[15]);
        analysis.symbolOffset = Memory.readU32(p);
        p = ptr.add(instAnalysisStructDesc.offsets[16]);
        var modulePtr = Memory.readPointer(p);
        if (!modulePtr.isNull()) {
            analysis.module = Memory.readCString(modulePtr);
        } else {
            analysis.module = "";
        }
        Object.freeze(analysis);
        return analysis;
    }

    this.getInstAnalysis = function(type) {
        /**:QBDI.prototype.getInstAnalysis()
          Obtain the analysis of an instruction metadata. Analysis results are cached in the VM.
          The validity of the returned pointer is only guaranteed until the end of the callback, else a deepcopy of the structure is required.

          :param [type]: Properties to retrieve during analysis (default to ANALYSIS_INSTRUCTION | ANALYSIS_DISASSEMBLY).

          :returns: A InstAnalysis object containing the analysis result.
          :rtype:   Object
        */
        type = type || (AnalysisType.ANALYSIS_INSTRUCTION | AnalysisType.ANALYSIS_DISASSEMBLY);
        var analysis = QBDI_C.getInstAnalysis(vm, type);
        if (analysis.isNull()) {
            return NULL;
        }
        return parseInstAnalysis(analysis);
    }

    this.recordMemoryAccess = function(type) {
        /**:QBDI.prototype.recordMemoryAccess(type)
          Obtain the memory accesses made by the last executed instruction. Return NULL and a size of 0 if the instruction made no memory access.

          :param type: Memory mode bitfield to activate the logging for: either MEMORY_READ, MEMORY_WRITE or both (MEMORY_READ_WRITE).
        */
        return QBDI_C.recordMemoryAccess(vm, type) == true;
    }

    function parseMemoryAccess(ptr) {
        var access = {};
        var p = ptr;
        access.instAddress = Memory.readRword(p);
        p = ptr.add(memoryAccessDesc.offsets[1]);
        access.accessAddress = Memory.readRword(p);
        p = ptr.add(memoryAccessDesc.offsets[2]);
        access.value = Memory.readRword(p);
        p = ptr.add(memoryAccessDesc.offsets[3]);
        access.size = Memory.readU8(p);
        p = ptr.add(memoryAccessDesc.offsets[4]);
        access.type = Memory.readU8(p);
        Object.freeze(access);
        return access;
    }

    function getMemoryAccess(f) {
        var accesses = [];
        var sizePtr = Memory.alloc(4);
        var accessPtr = f(vm, sizePtr);
        if (accessPtr.isNull()) {
            return [];
        }
        var cnt = Memory.readU32(sizePtr);
        var sSize = memoryAccessDesc.size;
        var p = accessPtr;
        for (var i = 0; i < cnt; i++) {
            var access = parseMemoryAccess(p);
            accesses.push(access);
            p = p.add(sSize);
        }
        System.free(accessPtr);
        return accesses;
    }

    this.getInstMemoryAccess = function() {
        /**:QBDI.prototype.getInstMemoryAccess()
          Obtain the memory accesses made by the last executed instruction. Return NULL and a size of 0 if the instruction made no memory access.

          :returns: An array of memory accesses made by the instruction.
          :rtype:   Array
        */
        return getMemoryAccess(QBDI_C.getInstMemoryAccess);
    }

    this.getBBMemoryAccess = function() {
        /**:QBDI.prototype.getBBMemoryAccess()
          Obtain the memory accesses made by the last executed basic block. Return NULL and a size of 0 if the basic block made no memory access.

        :returns:   An array of memory accesses made by the basic block.
        :rtype:     Array
        */
        return getMemoryAccess(QBDI_C.getBBMemoryAccess);
    }

    // Memory

    this.allocateVirtualStack = function(state, stackSize) {
        /**:QBDI.prototype.allocateVirtualStack(gprs, stackSize)
          Allocate a new stack and setup the GPRState accordingly. The allocated stack needs to be freed with alignedFree().

          :param gprs:      Array of register values
          :param stackSize: Size of the stack to be allocated.
        */
        GPRState.validOrThrow(state);
        var stackPtr = Memory.alloc(Process.pointerSize);
        var ret = QBDI_C.allocateVirtualStack(state.ptr, stackSize, stackPtr);
        if (ret == false) {
            return NULL;
        }
        return Memory.readPointer(stackPtr);
    }


    this.alignedAlloc = function(size, align) {
        /**:QBDI.prototype.alignedAlloc(size, align)
          Allocate a block of memory of a specified sized with an aligned base address.

          :param size:  Allocation size in bytes.
          :param align: Base address alignement in bytes.

          :returns: Pointer to the allocated memory or NULL in case an error was encountered.
          :rtype: rword
        */
        return QBDI_C.alignedAlloc(size, align);
    }

    this.alignedFree = function(ptr) {
        QBDI_C.alignedFree(ptr);
    }

    function formatVAArgs(args) {
        if (args === undefined) {
            args = [];
        }
        var argsCnt = args.length;
        // We are limited to 10 arguments for now
        var fargs = new Array(10);
        var fargsCnt = fargs.length
        for (var i = 0; i < fargsCnt; i++) {
            if (i < argsCnt) {
                fargs[i] = args[i].toRword();
            } else {
                fargs[i] = 0;
            }
        }
        return [argsCnt, fargs];
    }

    this.simulateCall = function(state, retAddr, args) {
        /**:QBDI.prototype.simulateCall(state, retAddr, [args])
          Simulate a call by modifying the stack and registers accordingly.

          :param state:     Array of register values
          :param retAddr:   Return address of the call to simulate.
          :param args:      A variadic list of arguments.
        */
        GPRState.validOrThrow(state);
        retAddr = retAddr.toRword();
        var fargs = formatVAArgs(args);
        // Use this weird construction to work around a bug in the duktape runtime
        var _simulateCall = function(a, b, c, d, e, f, g, h, i, j) {
            QBDI_C.simulateCall(state.ptr, retAddr, fargs[0], a, b, c, d, e, f, g, h, i, j);
        }
        _simulateCall.apply(null, fargs[1]);
    }

    this.getModuleNames = function() {
        /**:QBDI.prototype.getModuleNames()
          Use QBDI engine to retrieve loaded modules.

          :returns: list of module names (ex: ["ls", "libc", "libz"])
          :rtype: [String]
        */
        var sizePtr = Memory.alloc(4);
        var modsPtr = QBDI_C.getModuleNames(sizePtr);
        var size = Memory.readU32(sizePtr);
        if (modsPtr.isNull() || size === 0) {
            return [];
        }
        var mods = [];
        var p = modsPtr;
        for (var i = 0; i < size; i++) {
            var strPtr = Memory.readPointer(p);
            var str = Memory.readCString(strPtr);
            mods.push(str);
            System.free(strPtr);
            p = p.add(Process.pointerSize);
        }
        System.free(modsPtr);
        return mods;
    }

    // Logs
    this.addLogFilter = function(tag, priority) {
        var tagPtr = Memory.allocUtf8String(tag);
        QBDI_C.addLogFilter(tagPtr, priority);
    }

    // Helpers

    this.newInstCallback = function(cbk) {
        /**:QBDI.prototype.newInstCallback(cbk)
          Create a native **Instruction callback** from a JS function.

          :param cbk: an instruction callback (ex: function(vm, gpr, fpr, data) {};)

          :returns: an instruction callback
          :rtype: NativeCallback

          Example:
                >>> var icbk = vm.newInstCallback(function(vm, gpr, fpr, data) {
                >>>   inst = vm.getInstAnalysis();
                >>>   console.log("0x" + inst.address.toString(16) + " " + inst.disassembly);
                >>>   return VMAction.CONTINUE;
                >>> });
         */
        if (typeof(cbk) !== 'function' || cbk.length !== 4) {
            return undefined;
        }
        // Use a closure to provide object
        var vm = this;
        var jcbk = function(vmPtr, gprPtr, fprPtr, dataPtr) {
            var gpr = new GPRState(gprPtr);
            var fpr = new FPRState(fprPtr);
            var data = getUserData(dataPtr);
            return cbk(vm, gpr, fpr, data);
        }
        return new NativeCallback(jcbk, 'int', ['pointer', 'pointer', 'pointer', 'pointer']);
    }

    this.newVMCallback = function(cbk) {
        /**:QBDI.prototype.newVMCallback(cbk)
          Create a native **VM callback** from a JS function.

          :param cbk: a VM callback (ex: function(vm, state, gpr, fpr, data) {};)

          :returns: a VM callback
          :rtype: NativeCallback

          Example:
                >>> var vcbk = vm.newVMCallback(function(vm, evt, gpr, fpr, data) {
                >>>   if (evt.event & VMEvent.EXEC_TRANSFER_CALL) {
                >>>     console.warn("[!] External call to 0x" + evt.basicBlockStart.toString(16));
                >>>   }
                >>>   return VMAction.CONTINUE;
                >>> });
         */
        if (typeof(cbk) !== 'function' || cbk.length !== 5) {
            return undefined;
        }
        // Use a closure to provide object and a parsed event
        var vm = this;
        var jcbk = function(vmPtr, state, gprPtr, fprPtr, dataPtr) {
            var s = parseVMState(state);
            var gpr = new GPRState(gprPtr);
            var fpr = new FPRState(fprPtr);
            var data = getUserData(dataPtr);
            return cbk(vm, s, gpr, fpr, data);
        }
        return new NativeCallback(jcbk, 'int', ['pointer', 'pointer', 'pointer', 'pointer', 'pointer']);
    }

    this.call = function(address, args) {
        /**:QBDI.prototype.call(address[, args])
          Call a function by its address (or through a Frida ``NativePointer``).

          :param address: function address (or Frida ``NativePointer``).
          :param [args]: optional list of arguments

          Arguments can be provided, but their types need to be compatible
          with the ``.toRword()`` interface (like ``NativePointer`` or ``UInt64``).

          Example:
                >>> var vm = new QBDI();
                >>> var state = vm.getGPRState();
                >>> vm.allocateVirtualStack(state, 0x1000000);
                >>> var aFunction = Module.findExportByName(null, "Secret");
                >>> vm.addInstrumentedModuleFromAddr(aFunction);
                >>> vm.call(aFunction, [42]);
         */
        address = address.toRword();
        var fargs = formatVAArgs(args);
        // Use this weird construction to work around a bug in the duktape runtime
        var _call = function(a, b, c, d, e, f, g, h, i, j) {
            var retPtr = Memory.alloc(Process.pointerSize);
            var res = QBDI_C.call(vm, retPtr, address, fargs[0], a, b, c, d, e, f, g, h, i, j);
            if (res == false) {
                throw new EvalError('Execution failed');
            }
            return ptr(Memory.readRword(retPtr));
        }
        return _call.apply(null, fargs[1]);
    }

    /**:QBDI.version
     QBDI version (major, minor, patch).

     {string:String,integer:Number,major:Number,minor:Number,patch:Number}
    */
    Object.defineProperty(this, 'version', {
        enumerable: true,
        get: function () {
            if (!QBDI_C.getVersion) {
                return undefined;
            }
            var version = {};
            var versionPtr = Memory.alloc(4);
            var vStrPtr = QBDI_C.getVersion(versionPtr);
            var vInt = Memory.readU32(versionPtr);
            version.string = Memory.readCString(vStrPtr);
            version.integer = vInt;
            version.major = (vInt >> 8) & 0xf;
            version.minor = (vInt >> 4) & 0xf;
            version.patch = vInt & 0xf;
            Object.freeze(version);
            return version;
        }
    });

    initialize.call(this);
};


// nodejs export
if (typeof(module) !== "undefined") {
    var exports = module.exports = {
        QBDI_LIB_FULLPATH: QBDI_LIB_FULLPATH,
        QBDI: QBDI,
        rword: rword,
        GPR_NAMES: GPR_NAMES,
        REG_RETURN: REG_RETURN,
        REG_PC: REG_PC,
        REG_SP: REG_SP,
        VMError: VMError,
        InstPosition: InstPosition,
        VMAction: VMAction,
        VMEvent: VMEvent,
        AnalysisType: AnalysisType,
        OperandType: OperandType,
        RegisterAccessType: RegisterAccessType,
        MemoryAccessType: MemoryAccessType,
        SyncDirection: SyncDirection,

        // Allow automagic exposure of QBDI interface in nodejs GLOBAL
        import: function() {
            for (var key in this) {
                if (key !== "import") {
                    global[key] = this[key];
                }
            }
        }
    };
}
