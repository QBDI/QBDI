/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2023 Quarkslab
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
export var QBDI_MAJOR = 0;
export var QBDI_MINOR = 10;
export var QBDI_PATCH = 1;
/**
 * Minimum version of QBDI to use Frida bindings
 */
export var QBDI_MINIMUM_VERSION = (QBDI_MAJOR << 16) | (QBDI_MINOR << 8) | QBDI_PATCH;

if (typeof Duktape === 'object') {
    // Warn about duktape runtime (except on iOS...)
    if (Process.platform !== 'darwin' || Process.arch.indexOf("arm") !== 0) {
        console.warn("[!] Warning: using duktape runtime is much slower...");
        console.warn("    => Frida --enable-jit option should be used");
    }
}

// Provide a generic and "safe" (no exceptions if symbol is not found) way to load
// a library and bind/create a native function
class Binder {
    constructor() {}

    findLibrary(lib, paths) {
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

    safeNativeFunction(cbk, ret, args) {
        var e = cbk();
        if (!e) {
            return undefined;
        }
        return new NativeFunction(e, ret, args);
    }

    load(lib, paths) {
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
    }

    bind(name, ret, args) {
        return this.safeNativeFunction(function() {
            return Module.findExportByName(null, name);
        }, ret, args);
    }
}


class QBDIBinder extends Binder {
    /**
     * QBDI library name
     */
    get QBDI_LIB() {
        return {
            'linux': 'libQBDI.so',
            'darwin': 'libQBDI.dylib',
            'windows': 'QBDI.dll',
        }[Process.platform];
    }

    // paths where QBDI library may be
    get QBDI_PATHS() {
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

    bind(name, ret, args) {
        var libpath = this.QBDI_LIB;
        return this.safeNativeFunction(function() {
            return Module.findExportByName(libpath, name);
        }, ret, args);
    }

    load() {
        return super.load(this.QBDI_LIB, this.QBDI_PATHS);
    }
}


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

/**
 * Fullpath of the QBDI library
 */
// Load QBDI library
var QBDI_LIB_FULLPATH = _qbdibinder.load();

// Define rword type and interfaces

/**
 * An alias to Frida uint type with the size of general registers (**uint64** or **uint32**)
 */
export var rword = Process.pointerSize === 8 ? 'uint64' : 'uint32';

Memory.readRword = Process.pointerSize === 8 ? Memory.readU64 : Memory.readU32;

Memory.writeRword = Process.pointerSize === 8 ? Memory.writeU64 : Memory.writeU32;

// Convert a number to its register-sized representation

/**
 * Convert a NativePointer into a type with the size of a register (``Number`` or ``UInt64``).
 */
NativePointer.prototype.toRword = function() {
    // Nothing better really ?
    if (Process.pointerSize === 8) {
        return uint64("0x" + this.toString(16));
    }
    return parseInt(this.toString(16), 16);
}

/**
 * Convert a number into a type with the size of a register (``Number`` or ``UInt64``).
 * Can't be used for numbers > 32 bits, would cause weird results due to IEEE-754.
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

/**
 * An identity function (returning the same ``UInt64`` object).
 * It exists only to provide a unified **toRword** interface.
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

/**
 * Convert a String into a type with the size of a register (``Number`` or ``UInt64``).
 */
String.prototype.toRword = function() {
    return ptr(this).toRword()
};

/**
 * This function is used to pretty print a pointer, padded with 0 to the size of a register.
 *
 * @param ptr Pointer you want to pad
 *
 * @return pointer value as padded string (ex: "0x00004242")
 */
export function hexPointer(ptr) {
    return ptr.toString(16).leftPad("0000000000000000", Process.pointerSize * 2);
}


//
var QBDI_C = Object.freeze({
    // VM
    initVM: _qbdibinder.bind('qbdi_initVM', 'void', ['pointer', 'pointer', 'pointer', rword]),
    terminateVM: _qbdibinder.bind('qbdi_terminateVM', 'void', ['pointer']),
    getOptions: _qbdibinder.bind('qbdi_getOptions', rword, ['pointer']),
    setOptions: _qbdibinder.bind('qbdi_setOptions', 'void', ['pointer', rword]),
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
    switchStackAndCall: _qbdibinder.bind('qbdi_switchStackAndCall', 'uchar', ['pointer', 'pointer', rword, 'uint32', 'uint32',
                           rword, rword, rword, rword, rword, rword, rword, rword, rword, rword]),
    getGPRState: _qbdibinder.bind('qbdi_getGPRState', 'pointer', ['pointer']),
    getFPRState: _qbdibinder.bind('qbdi_getFPRState', 'pointer', ['pointer']),
    setGPRState: _qbdibinder.bind('qbdi_setGPRState', 'void', ['pointer', 'pointer']),
    setFPRState: _qbdibinder.bind('qbdi_setFPRState', 'void', ['pointer', 'pointer']),
    addMnemonicCB: _qbdibinder.bind('qbdi_addMnemonicCB', 'uint32', ['pointer', 'pointer', 'uint32', 'pointer', 'pointer', 'int32']),
    addMemAccessCB: _qbdibinder.bind('qbdi_addMemAccessCB', 'uint32', ['pointer', 'uint32', 'pointer', 'pointer', 'int32']),
    addInstrRule: _qbdibinder.bind('qbdi_addInstrRule', 'uint32', ['pointer', 'pointer', 'uint32', 'pointer']),
    addInstrRuleRange: _qbdibinder.bind('qbdi_addInstrRuleRange', 'uint32', ['pointer', rword, rword, 'pointer', 'uint32', 'pointer']),
    addInstrRuleData: _qbdibinder.bind('qbdi_addInstrRuleData', 'void', ['pointer', 'uint32', 'pointer', 'pointer', 'int32']),
    addMemAddrCB: _qbdibinder.bind('qbdi_addMemAddrCB', 'uint32', ['pointer', rword, 'uint32', 'pointer', 'pointer']),
    addMemRangeCB: _qbdibinder.bind('qbdi_addMemRangeCB', 'uint32', ['pointer', rword, rword, 'uint32', 'pointer', 'pointer']),
    addCodeCB: _qbdibinder.bind('qbdi_addCodeCB', 'uint32', ['pointer', 'uint32', 'pointer', 'pointer', 'int32']),
    addCodeAddrCB: _qbdibinder.bind('qbdi_addCodeAddrCB', 'uint32', ['pointer', rword, 'uint32', 'pointer', 'pointer', 'int32']),
    addCodeRangeCB: _qbdibinder.bind('qbdi_addCodeRangeCB', 'uint32', ['pointer', rword, rword, 'uint32', 'pointer', 'pointer', 'int32']),
    addVMEventCB: _qbdibinder.bind('qbdi_addVMEventCB', 'uint32', ['pointer', 'uint32', 'pointer', 'pointer']),
    deleteInstrumentation: _qbdibinder.bind('qbdi_deleteInstrumentation', 'uchar', ['pointer', 'uint32']),
    deleteAllInstrumentations: _qbdibinder.bind('qbdi_deleteAllInstrumentations', 'void', ['pointer']),
    getInstAnalysis: _qbdibinder.bind('qbdi_getInstAnalysis', 'pointer', ['pointer', 'uint32']),
    getCachedInstAnalysis: _qbdibinder.bind('qbdi_getCachedInstAnalysis', 'pointer', ['pointer', rword, 'uint32']),
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
    setLogPriority: _qbdibinder.bind('qbdi_setLogPriority', 'void', ['uint32']),
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
    var GPR_NAMES_ = ["RAX","RBX","RCX","RDX","RSI","RDI","R8","R9","R10","R11","R12","R13","R14","R15","RBP","RSP","RIP","EFLAGS","FS","GS"];
    var REG_RETURN_ = "RAX";
    var REG_PC_ = "RIP";
    var REG_SP_ = "RSP";
} else if (Process.arch === 'arm64') {
    var GPR_NAMES_ = ["X0","X1","X2","X3","X4","X5","X6","X7","X8","X9","X10","X11","X12","X13","X14","X15","X16","X17","X18","X19","X20","X21","X22","X23","X24","X25","X26","X27","X28","FP","LR","SP","NZCV","PC"];
    var REG_RETURN_ = "X0";
    var REG_PC_ = "PC";
    var REG_SP_ = "SP";
} else if (Process.arch === 'arm') {
    var GPR_NAMES_ = ["R0","R1","R2","R3","R4","R5","R6","R7","R8","R9","R10","R11","R12","SP","LR","PC","CPSR"];
    var REG_RETURN_ = "R0";
    var REG_PC_ = "PC";
    var REG_SP_ = "SP";
} else if (Process.arch === 'ia32'){
    var GPR_NAMES_ = ["EAX","EBX","ECX","EDX","ESI","EDI","EBP","ESP","EIP","EFLAGS"];
    var REG_RETURN_ = "EAX";
    var REG_PC_ = "EIP";
    var REG_SP_ = "ESP";
}

/**
 * An array holding register names.
 */
export var GPR_NAMES = GPR_NAMES_;
/**
 * A constant string representing the register carrying the return value of a function.
 */
export var REG_RETURN = REG_RETURN_;
/**
 * String of the instruction pointer register.
 */
export var REG_PC = REG_PC_;
/**
 * String of the stack pointer register.
 */
export var REG_SP = REG_SP_;

/**
 * Error return by the QBDI VM
 */
export var VMError = Object.freeze({
    /**
     * Returned event is invalid.
     */
    INVALID_EVENTID: 0xffffffff
});

/**
 * Synchronisation direction between Frida and QBDI GPR contexts
 */
export var SyncDirection = Object.freeze({
    /**
     * Constant variable used to synchronize QBDI's context to Frida's.
     *
     * .. warning:: This is currently not supported due to the lack of context updating in Frida.
     */
    QBDI_TO_FRIDA: 0,
    /**
     * Constant variable used to synchronize Frida's context to QBDI's.
     */
    FRIDA_TO_QBDI: 1
});

/**
 * The callback results.
 */
export var VMAction = Object.freeze({
    /**
     * The execution of the basic block continues.
     */
    CONTINUE: 0,
    /**
     * Available only with PREINST InstCallback.  The instruction and the
     * remained PREINST callbacks are skip. The execution continue with the
     * POSTINST instruction.
     *
     * We recommand to used this result with a low priority PREINST callback in
     * order to emulate the instruction without skipping the POSTINST callback.
     */
    SKIP_INST: 1,
    /*!*
     * Available only with InstCallback. The current instruction and the
     * reminding callback (PRE and POST) are skip. The execution continues to
     * the next instruction.
     *
     * For instruction that change the instruction pointer (jump/call/ret),
     * BREAK_TO_VM must be used insted of SKIP.
     *
     * SKIP can break the record of MemoryAccess for the current instruction.
     */
    SKIP_PATCH: 2,
    /**
     * The execution breaks and returns to the VM causing a complete
     * reevaluation of the execution state. A :js:data:`VMAction.BREAK_TO_VM` is
     * needed to ensure that modifications of the Program Counter or the program
     * code are taken into account.
     */
    BREAK_TO_VM: 3,
    /**
     * Stops the execution of the program. This causes the run function to
     * return early.
     */
    STOP: 4
});


/**
 * Position relative to an instruction.
 */
export var InstPosition = Object.freeze({
    /**
     * Positioned **before** the instruction.
     */
    PREINST: 0,
    /**
     * Positioned **after** the instruction.
     */
    POSTINST: 1
});

/**
 * Priority of callback
 */
export var CallbackPriority = Object.freeze({
    /**
     * Default priority for callback.
     */
    PRIORITY_DEFAULT: 0,
    /**
     * Maximum priority if getInstMemoryAccess is used in the callback.
     */
    PRIORITY_MEMACCESS_LIMIT: 0x1000000
});

/**
 * Events triggered by the virtual machine.
 */
export var VMEvent = Object.freeze({
    /**
     * Triggered when the execution enters a sequence.
     */
    SEQUENCE_ENTRY     : 1,
    /**
     * Triggered when the execution exits from the current sequence.
     */
    SEQUENCE_EXIT      : 1<<1,
    /**
     * Triggered when the execution enters a basic block.
     */
    BASIC_BLOCK_ENTRY     : 1<<2,
    /**
     * Triggered when the execution exits from the current basic block.
     */
    BASIC_BLOCK_EXIT      : 1<<3,
    /**
     * Triggered when the execution enters a new (~unknown) basic block.
     */
    BASIC_BLOCK_NEW       : 1<<4,
    /**
     * Triggered when the ExecBroker executes an execution transfer.
     */
    EXEC_TRANSFER_CALL    : 1<<5,
    /**
     * Triggered when the ExecBroker returns from an execution transfer.
     */
    EXEC_TRANSFER_RETURN  : 1<<6,
    /**
     * Not implemented.
     */
    SYSCALL_ENTRY         : 1<<7,
    /**
     * Not implemented.
     */
    SYSCALL_EXIT          : 1<<8,
    /**
     * Not implemented.
     */
    SIGNAL                : 1<<9
});

/**
 * Memory access type (read / write / ...)
 */
export var MemoryAccessType = Object.freeze({
    /**
     * Memory read access.
     */
    MEMORY_READ : 1,
    /**
     * Memory write access.
     */
    MEMORY_WRITE : 2,
    /**
     * Memory read/write access.
     */
    MEMORY_READ_WRITE : 3
});

/**
 * Memory access flags
 */
export var MemoryAccessFlags = Object.freeze({
    /**
     * Empty flag.
     */
    MEMORY_NO_FLAGS : 0,
    /**
     * The size of the access isn't known.
     */
    MEMORY_UNKNOWN_SIZE : 1<<0,
    /**
     * The given size is a minimum size.
     */
    MEMORY_MINIMUM_SIZE : 1<<1,
    /**
     * The value of the access is unknown or hasn't been retrived.
     */
    MEMORY_UNKNOWN_VALUE : 1<<2
});

/**
 * Register access type (read / write / rw)
 */
export var RegisterAccessType = Object.freeze({
    /**
     * Register is read.
     */
    REGISTER_READ : 1,
    /**
     * Register is written.
     */
    REGISTER_WRITE : 2,
    /**
     * Register is read/written.
     */
    REGISTER_READ_WRITE : 3
});

/**
 * Instruction Condition
 */
export var ConditionType = Object.freeze({
    /**
     * The instruction is unconditionnal
     */
    CONDITION_NONE : 0x0,
    /**
     * The instruction is always true
     */
    CONDITION_ALWAYS : 0x2,
    /**
     * The instruction is always false
     */
    CONDITION_NEVER : 0x3,
    /**
     * Equals ( '==' )
     */
    CONDITION_EQUALS : 0x4,
    /**
     * Not Equals ( '!=' )
     */
    CONDITION_NOT_EQUALS : 0x5,
    /**
     * Above ( '>' unsigned )
     */
    CONDITION_ABOVE : 0x6,
    /**
     * Below or Equals ( '<=' unsigned )
     */
    CONDITION_BELOW_EQUALS : 0x7,
    /**
     * Above or Equals ( '>=' unsigned )
     */
    CONDITION_ABOVE_EQUALS : 0x8,
    /**
     * Below ( '<' unsigned )
     */
    CONDITION_BELOW : 0x9,
    /**
     * Great ( '>' signed )
     */
    CONDITION_GREAT : 0xa,
    /**
     * Less or Equals ( '<=' signed )
     */
    CONDITION_LESS_EQUALS : 0xb,
    /**
     * Great or Equals ( '>=' signed )
     */
    CONDITION_GREAT_EQUALS : 0xc,
    /**
     * Less ( '<' signed )
     */
    CONDITION_LESS : 0xd,
    /**
     * Even
     */
    CONDITION_EVEN : 0xe,
    /**
     * Odd
     */
    CONDITION_ODD : 0xf,
    /**
     * Overflow
     */
    CONDITION_OVERFLOW : 0x10,
    /**
     * Not Overflow
     */
    CONDITION_NOT_OVERFLOW : 0x11,
    /**
     * Sign
     */
    CONDITION_SIGN : 0x12,
    /**
     * Not Sign
     */
    CONDITION_NOT_SIGN : 0x13
});


/**
 * Register access type (read / write / rw)
 */
export var OperandType = Object.freeze({
    /**
     * Invalid operand.
     */
    OPERAND_INVALID : 0,
    /**
     * Immediate operand.
     */
    OPERAND_IMM : 1,
    /**
     * General purpose register operand.
     */
    OPERAND_GPR : 2,
    /**
     * Predicate special operand.
     */
    OPERAND_PRED : 3,
    /**
     * Float register operand.
     */
    OPERAND_FPR : 4,
    /**
     * Segment or unsupported register operand
     */
    OPERAND_SEG : 5
});

/**
 * Operand flag
 */
export var OperandFlag = Object.freeze({
    /**
     * No flag
     */
    OPERANDFLAG_NONE : 0,
    /**
     * The operand is used to compute an address
     */
    OPERANDFLAG_ADDR : 1 << 0,
    /**
     * The value of the operand is PC relative
     */
    OPERANDFLAG_PCREL : 1 << 1,
    /**
     * The operand role isn't fully defined
     */
    OPERANDFLAG_UNDEFINED_EFFECT : 1 << 2,
    /**
     * The operand is implicit
     */
    OPERANDFLAG_IMPLICIT : 1 << 3
});

/**
 * Properties to retrieve during an instruction analysis.
 */
export var AnalysisType = Object.freeze({
    /**
     * Instruction analysis (address, mnemonic, ...).
     */
    ANALYSIS_INSTRUCTION : 1,
    /**
     * Instruction disassembly.
     */
    ANALYSIS_DISASSEMBLY : 1<<1,
    /**
     * Instruction operands analysis.
     */
    ANALYSIS_OPERANDS : 1<<2,
    /**
     * Instruction nearest symbol (and offset).
     */
    ANALYSIS_SYMBOL : 1<<3
});

/**
 * QBDI VM Options
 */
export var Options = {
    /**
     * Default value
     */
    NO_OPT : 0,
    /**
     * Disable all operation on FPU (SSE, AVX, SIMD).
     * May break the execution if the target use the FPU.
     */
    OPT_DISABLE_FPR : 1<<0,
    /**
     * Disable context switch optimisation when the target
     * execblock doesn't used FPR.
     */
    OPT_DISABLE_OPTIONAL_FPR : 1<<1,
};
if (Process.arch === 'x64') {
  /**
   * Used the AT&T syntax for instruction disassembly (for X86 and X86_64)
   */
  Options.OPT_ATT_SYNTAX = 1<<24;
  /**
   * Enable Backup/Restore of FS/GS segment. 
   * This option uses the instructions (RD|WR)(FS|GS)BASE that must be 
   * supported by the operating system.
   */
  Options.OPT_ENABLE_FS_GS = 1<<25;
} else if (Process.arch === 'ia32') {
  Options.OPT_ATT_SYNTAX = 1<<24;
} else if (Process.arch === 'arm64') {
  /**
   * Disable the emulation of the local monitor by QBDI
   */
  Options.OPT_DISABLE_LOCAL_MONITOR = 1<<24;
  /**
   * Disable pointeur authentication
   */
  Options.OPT_BYPASS_PAUTH = 1<<25;
  /**
   * Enable BTI on instrumented code
   */
  Options.OPT_ENABLE_BTI = 1<<26;
} else if (Process.arch === 'arm') {
  Options.OPT_DISABLE_LOCAL_MONITOR = 1<<24;
  /**
   * Disable the used of D16-D31 register
   */
  Options.OPT_DISABLE_D16_D31 = 1<<25;
  /**
   * Change between ARM and Thumb as an ARMv4 CPU
   */
  Options.OPT_ARMv4 = 3<<26;
  /**
   * Change between ARM and Thumb as an ARMv5T or ARMv6 CPU
   */
  Options.OPT_ARMv5T_6 = 1<<27;
  /**
   * Change between ARM and Thumb as an ARMv7 CPU (default)
   */
  Options.OPT_ARMv7 = 0;
  Options.OPT_ARM_MASK = 3<<26;
}

Options = Object.freeze(Options);

export class InstrRuleDataCBK {
    /**
     * Object to define an :js:func:`InstCallback` in an :js:func:`InstrRuleCallback`
     *
     * @param {InstPosition} pos       Relative position of the callback (PreInst / PostInst).
     * @param {InstCallback} cbk       A **native** InstCallback returned by :js:func:`VM.newInstCallback`.
     * @param {Object}       data      User defined data passed to the callback.
     * @param {Int}          priority  The priority of the callback.
     */
    constructor(pos, cbk, data, priority = CallbackPriority.PRIORITY_DEFAULT) {
        this.position = pos;
        this.cbk = cbk;
        this.data = data;
        this.priority = priority;
    }
}

class State {
    constructor(state) {
        if (!NativePointer.prototype.isPrototypeOf(state) || state.isNull()) {
            throw new TypeError('Invalid state pointer');
        }
        this.statePtr = state;
    }

    get ptr() {
        return this.statePtr;
    }

    toRword() {
        return this.statePtr.toRword();
    }

    toString() {
        return this.statePtr.toString();
    }
}

/**
 * General Purpose Register context
 */
class GPRState extends State  {
    _getGPRId(rid) {
        if (typeof(rid) === 'string') {
            rid = GPR_NAMES.indexOf(rid.toUpperCase());
        }
        if (rid < 0 || rid > GPR_NAMES.length) {
            return undefined;
        }
        return rid;
    }

    /**
     * This function is used to get the value of a specific register.
     *
     * @param {String|Number} rid Register (register name or ID can be used e.g : "RAX", "rax", 0)
     *
     * @return GPR value (ex: 0x42)
     */
    getRegister(rid) {
        var rid = this._getGPRId(rid);
        if (rid === null) {
            return undefined;
        }
        return ptr(QBDI_C.getGPR(this.ptr, rid));
    }

    /**
     * This function is used to set the value of a specific register.
     *
     * @param {String|Number} rid   Register (register name or ID can be used e.g : "RAX", "rax", 0)
     * @param {String|Number} value Register value (use **strings** for big integers)
     */
    setRegister(rid, value) {
        var rid = this._getGPRId(rid);
        if (rid !== null) {
            QBDI_C.setGPR(this.ptr, rid, value.toRword());
        }
    }

    /**
     * This function is used to get values of all registers.
     *
     * @return GPRs of current context (ex: \{"RAX":0x42, ...\})
     */
    getRegisters() {
        var regCnt = GPR_NAMES.length;
        var gprs = {};
        for (var i = 0; i < regCnt; i++) {
            gprs[GPR_NAMES[i]] = this.getRegister(i);
        }
        return gprs;
    }

    /**
     * This function is used to set values of all registers.
     *
     * @param gprs Array of register values
     */
    setRegisters(gprs) {
        var regCnt = GPR_NAMES.length;
        for (var i = 0; i < regCnt; i++) {
            this.setRegister(i, gprs[GPR_NAMES[i]]);
        }
    }

    /**
     * This function is used to synchronise a specific register between Frida and QBDI.
     *
     * .. warning:: Currently QBDI_TO_FRIDA is experimental. (E.G : RIP cannot be synchronized)
     *
     * @param                   FridaCtx   Frida context
     * @param {String|Number}   rid        Register (register name or ID can be used e.g : "RAX", "rax", 0)
     * @param {SyncDirection}   direction  Synchronization direction. (:js:data:`FRIDA_TO_QBDI` or :js:data:`QBDI_TO_FRIDA`)
     */
    synchronizeRegister(FridaCtx, rid, direction) {
        if (direction === SyncDirection.FRIDA_TO_QBDI) {
            this.setRegister(rid, FridaCtx[rid.toLowerCase()].toRword());
        }
        else { // FRIDA_TO_QBDI
            FridaCtx[rid.toLowerCase()] = ptr(this.getRegister(rid).toString());
        }
    }

    /**
     * This function is used to synchronise context between Frida and QBDI.
     *
     * .. warning:: Currently QBDI_TO_FRIDA is not implemented (due to Frida limitations).
     *
     * @param                   FridaCtx   Frida context
     * @param {SyncDirection}   direction  Synchronization direction. (:js:data:`FRIDA_TO_QBDI` or :js:data:`QBDI_TO_FRIDA`)
     */
    synchronizeContext(FridaCtx, direction) {
        for (var i in GPR_NAMES) {
            if (GPR_NAMES[i] === "EFLAGS" || GPR_NAMES[i] === "FS" || GPR_NAMES[i] === "GS") {
                continue;
            }
            this.synchronizeRegister(FridaCtx, GPR_NAMES[i], direction);
        }
        if (direction === SyncDirection.QBDI_TO_FRIDA) {
            throw new Error('Not implemented (does not really work due to Frida)');
        }
    }

    /**
     * Pretty print QBDI context.
     *
     * @param {bool} [color] Will print a colored version of the context if set.
     *
     * @return dump of all GPRs in a pretty format
     */
    pp(color) {
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

    /**
     * Pretty print and log QBDI context.
     *
     * @param {bool} [color] Will print a colored version of the context if set.
     */
    dump(color) {
        console.log(this.pp(color));
    }

    static validOrThrow(state) {
        if (!GPRState.prototype.isPrototypeOf(state)) {
            throw new TypeError('Invalid GPRState');
         }
    }
}

/**
 * Floating Point Register context
 */
class FPRState extends State {
    static validOrThrow(state) {
        if (!FPRState.prototype.isPrototypeOf(state)) {
            throw new TypeError('Invalid FPRState');
         }
    }
}

export class VM {
    // private member
    #vm = null;
    #memoryAccessDesc = null;
    #operandAnalysisStructDesc = null;
    #instAnalysisStructDesc = null;
    #vmStateStructDesc = null;
    #userDataPtrMap = {};
    #userDataIIdMap = {};
    #userDataPointer = 0;

    /**
     * Create a new instrumentation virtual machine using "**new VM()**"
     */
    constructor() {
        // Enforce a minimum QBDI version (API compatibility)
        if (!this.version || this.version.integer < QBDI_MINIMUM_VERSION) {
            throw new Error('Invalid QBDI version !');
        }

        // Create VM instance
        this.#vm = this._initVM();

        // Cache various remote structure descriptions
        // Parse remote structure descriptions
        this.#memoryAccessDesc = this._parseStructDesc(QBDI_C.getMemoryAccessStructDesc());
        this.#operandAnalysisStructDesc = this._parseStructDesc(QBDI_C.getOperandAnalysisStructDesc());
        this.#instAnalysisStructDesc = this._parseStructDesc(QBDI_C.getInstAnalysisStructDesc());
        this.#vmStateStructDesc = this._parseStructDesc(QBDI_C.getVMStateStructDesc());

        // add a destructor on garbage collection
        // The name of the API change with frida 15.0.0
        if (Number(Frida.version.split(".")[0]) < 15) {
            var that = this;
            WeakRef.bind(VM, function dispose () {
                if (that.ptr !== null) {
                    that._terminateVM(that.ptr);
                }
            });
        } else {
            var that = this;
            Script.bindWeak(VM, function dispose () {
                if (that.ptr !== null) {
                    that._terminateVM(that.ptr);
                }
            });
        }
    }

    get ptr() {
        return this.#vm;
    }

    /**
     * QBDI version (major, minor, patch).
     *
     * {string:String, integer:Number, major:Number, minor:Number, patch:Number}
     */
    get version() {
        if (!QBDI_C.getVersion) {
            return undefined;
        }
        var version = {};
        var versionPtr = Memory.alloc(4);
        var vStrPtr = QBDI_C.getVersion(versionPtr);
        var vInt = Memory.readU32(versionPtr);
        version.string = Memory.readCString(vStrPtr);
        version.integer = vInt;
        version.major = (vInt >> 16) & 0xff;
        version.minor = (vInt >> 8) & 0xff;
        version.patch = vInt & 0xff;
        Object.freeze(version);
        return version;
    }

    /**
     * Get the current options of the VM
     *
     * @return  {Options}  The current option
     */
    getOptions() {
        return QBDI_C.getOptions(this.#vm);
    }

    /**
     * Set the options of the VM
     *
     * @param  {Options}  options  The new options of the VM.
     */
    setOptions(options) {
        QBDI_C.setOptions(this.#vm, options);
    }

    /**
     * Add an address range to the set of instrumented address ranges.
     *
     * @param {String|Number} start  Start address of the range (included).
     * @param {String|Number} end    End address of the range (excluded).
     */
    addInstrumentedRange(start, end) {
        QBDI_C.addInstrumentedRange(this.#vm, start.toRword(), end.toRword());
    }

    /**
     * Add the executable address ranges of a module to the set of instrumented address ranges.
     *
     * @param  {String} name   The module's name.
     *
     * @return {bool} True if at least one range was added to the instrumented ranges.
     */
    addInstrumentedModule(name) {
        var namePtr = Memory.allocUtf8String(name);
        return QBDI_C.addInstrumentedModule(this.#vm, namePtr) == true;
    }

    /**
     * Add the executable address ranges of a module to the set of instrumented address ranges. using an address belonging to the module.
     *
     * @param  {String|Number} addr An address contained by module's range.
     *
     * @return {bool} True if at least one range was removed from the instrumented ranges.
     */
    addInstrumentedModuleFromAddr(addr) {
        return QBDI_C.addInstrumentedModuleFromAddr(this.#vm, addr.toRword()) == true;
    }

    /**
     * Adds all the executable memory maps to the instrumented range set.
     *
     * @return {bool} True if at least one range was added to the instrumented ranges.
     */
    instrumentAllExecutableMaps() {
        return QBDI_C.instrumentAllExecutableMaps(this.#vm) == true;
    }

    /**
     * Remove an address range from the set of instrumented address ranges.
     *
     * @param {String|Number} start  Start address of the range (included).
     * @param {String|Number} end    End address of the range (excluded).
     */
    removeInstrumentedRange(start, end) {
        QBDI_C.removeInstrumentedRange(this.#vm, start.toRword(), end.toRword());
    }

    /**
     * Remove the executable address ranges of a module from the set of instrumented address ranges.
     *
     * @param {String} name   The module's name.
     *
     * @return {bool} True if at least one range was added to the instrumented ranges.
     */
    removeInstrumentedModule(name) {
        var namePtr = Memory.allocUtf8String(name);
        return QBDI_C.removeInstrumentedModule(this.#vm, namePtr) == true;
    }

    /**
     * Remove the executable address ranges of a module from the set of instrumented address ranges using an address belonging to the module.
     *
     * @param {String|Number} addr: An address contained by module's range.
     *
     * @return {bool} True if at least one range was added to the instrumented ranges.
     */
    removeInstrumentedModuleFromAddr(addr) {
        return QBDI_C.removeInstrumentedModuleFromAddr(this.#vm, addr.toRword()) == true;
    }

    /**
     * Remove all instrumented ranges.
     */
    removeAllInstrumentedRanges() {
        QBDI_C.removeAllInstrumentedRanges(this.#vm);
    }

    /**
     * Start the execution by the DBI from a given address (and stop when another is reached).
     *
     * @param {String|Number} start  Address of the first instruction to execute.
     * @param {String|Number} stop   Stop the execution when this instruction is reached.
     *
     * @return {bool} True if at least one block has been executed.
     */
    run(start, stop) {
        return QBDI_C.run(this.#vm, start.toRword(), stop.toRword()) == true;
    }

    /**
     * Obtain the current general register state.
     *
     * @return {GPRState} An object containing the General Purpose Registers state.
     */
    getGPRState() {
        return new GPRState(QBDI_C.getGPRState(this.#vm));
    }

    /**
     * Obtain the current floating point register state.
     *
     * @return {FPRState} An object containing the Floating point Purpose Registers state.
     */
    getFPRState() {
        return new FPRState(QBDI_C.getFPRState(this.#vm));
    }

    /**
     * Set the GPR state
     *
     * @param {GPRState} state  Array of register values
     */
    setGPRState(state) {
        GPRState.validOrThrow(state);
        QBDI_C.setGPRState(this.#vm, state.ptr);
    }

    /**
     * Set the FPR state
     *
     * @param {FPRState} state  Array of register values
     */
    setFPRState(state) {
        FPRState.validOrThrow(state);
        QBDI_C.setFPRState(this.#vm, state.ptr);
    }

    /**
     * Pre-cache a known basic block.
     *
     * @param {String|Number} pc  Start address of a basic block
     *
     * @return {bool} True if basic block has been inserted in cache.
     */
    precacheBasicBlock(pc) {
        return QBDI_C.precacheBasicBlock(this.#vm, pc) == true
    }

    /**
     * Clear a specific address range from the translation cache.
     *
     * @param {String|Number}  start  Start of the address range to clear from the cache.
     * @param {String|Number}  end    End of the address range to clear from the cache.
     */
    clearCache(start, end) {
        QBDI_C.clearCache(this.#vm, start, end)
    }

    /**
     * Clear the entire translation cache.
     */
    clearAllCache() {
        QBDI_C.clearAllCache(this.#vm)
    }


    /**
     * Register a callback event if the instruction matches the mnemonic.
     *
     * @param {String}       mnem      Mnemonic to match.
     * @param {InstPosition} pos       Relative position of the callback (PreInst / PostInst).
     * @param {InstCallback} cbk       A **native** InstCallback returned by :js:func:`VM.newInstCallback`.
     * @param {Object}       data      User defined data passed to the callback.
     * @param {Int}          priority  The priority of the callback.
     *
     * @return {Number} The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
     */
    addMnemonicCB(mnem, pos, cbk, data, priority = CallbackPriority.PRIORITY_DEFAULT) {
        var mnemPtr = Memory.allocUtf8String(mnem);
        var vm = this.#vm;
        return this._retainUserData(data, function (dataPtr) {
            return QBDI_C.addMnemonicCB(vm, mnemPtr, pos, cbk, dataPtr, priority);
        });
    }

    /**
     * Register a callback event for every memory access matching the type bitfield made by the instruction in the range codeStart to codeEnd.
     *
     * @param {MemoryAccessType} type      A mode bitfield: either MEMORY_READ, MEMORY_WRITE or both (MEMORY_READ_WRITE).
     * @param {InstCallback}     cbk       A **native** InstCallback returned by :js:func:`VM.newInstCallback`.
     * @param {Object}           data      User defined data passed to the callback.
     * @param {Int}              priority  The priority of the callback.
     *
     * @return {Number} The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
     */
    addMemAccessCB(type, cbk, data, priority = CallbackPriority.PRIORITY_DEFAULT) {
        var vm = this.#vm;
        return this._retainUserData(data, function (dataPtr) {
            return QBDI_C.addMemAccessCB(vm, type, cbk, dataPtr, priority);
        });
    }

    /**
     * Add a custom instrumentation rule to the VM.
     *
     * @param {InstrRuleCallback}  cbk    A **native** InstrRuleCallback returned by :js:func:`VM.newInstrRuleCallback`.
     * @param {AnalysisType}       type   Analyse type needed for this instruction function pointer to the callback
     * @param {Object}             data   User defined data passed to the callback.
     *
     * @return {Number} The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
     */
    addInstrRule(cbk, type, data) {
        var vm = this.#vm;
        return this._retainUserDataForInstrRuleCB(data, function (dataPtr) {
            return QBDI_C.addInstrRule(vm, cbk, type, dataPtr);
        });
    }

    /**
     * Add a custom instrumentation rule to the VM for a range of address.
     *
     * @param {String|Number}      start  Begin of the range of address where apply the rule
     * @param {String|Number}      end    End of the range of address where apply the rule
     * @param {InstrRuleCallback}  cbk    A **native** InstrRuleCallback returned by :js:func:`VM.newInstrRuleCallback`.
     * @param {AnalysisType}       type   Analyse type needed for this instruction function pointer to the callback
     * @param {Object}             data   User defined data passed to the callback.
     *
     * @return {Number} The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
     */
    addInstrRuleRange(start, end, cbk, type, data) {
        var vm = this.#vm;
        return this._retainUserDataForInstrRuleCB(data, function (dataPtr) {
            return QBDI_C.addInstrRuleRange(vm, start.toRword(), end.toRword(), cbk, type, dataPtr);
        });
    }

    /**
     * Add a virtual callback which is triggered for any memory access at a specific address matching the access type.
     * Virtual callbacks are called via callback forwarding by a gate callback triggered on every memory access. This incurs a high performance cost.
     *
     * @param {String|Number}     addr   Code address which will trigger the callback.
     * @param {MemoryAccessType}  type   A mode bitfield: either MEMORY_READ, MEMORY_WRITE or both (MEMORY_READ_WRITE).
     * @param {InstCallback}      cbk    A **native** InstCallback returned by :js:func:`VM.newInstCallback`.
     * @param {Object}            data   User defined data passed to the callback.
     *
     * @return {Number} The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
     */
    addMemAddrCB(addr, type, cbk, data) {
        var vm = this.#vm;
        return this._retainUserData(data, function (dataPtr) {
            return QBDI_C.addMemAddrCB(vm, addr.toRword(), type, cbk, dataPtr);
        });
    }

    /**
     * Add a virtual callback which is triggered for any memory access in a specific address range matching the access type.
     * Virtual callbacks are called via callback forwarding by a gate callback triggered on every memory access. This incurs a high performance cost.
     *
     * @param {String|Number}     start    Start of the address range which will trigger the callback.
     * @param {String|Number}     end      End of the address range which will trigger the callback.
     * @param {MemoryAccessType}  type     A mode bitfield: either MEMORY_READ, MEMORY_WRITE or both (MEMORY_READ_WRITE).
     * @param {InstCallback}      cbk      A **native** InstCallback returned by :js:func:`VM.newInstCallback`.
     * @param {Object}            data     User defined data passed to the callback.
     *
     * @return {Number} The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
     */
    addMemRangeCB(start, end, type, cbk, data) {
        var vm = this.#vm;
        return this._retainUserData(data, function (dataPtr) {
            return QBDI_C.addMemRangeCB(vm, start.toRword(), end.toRword(), type, cbk, dataPtr);
        });
    }

    /**
     * Register a callback event for a specific instruction event.
     *
     * @param {InstPosition} pos       Relative position of the callback (PreInst / PostInst).
     * @param {InstCallback} cbk       A **native** InstCallback returned by :js:func:`VM.newInstCallback`.
     * @param {Object}       data      User defined data passed to the callback.
     * @param {Int}          priority  The priority of the callback.
     *
     * @return {Number} The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
     */
    addCodeCB(pos, cbk, data, priority = CallbackPriority.PRIORITY_DEFAULT) {
        var vm = this.#vm;
        return this._retainUserData(data, function (dataPtr) {
            return QBDI_C.addCodeCB(vm, pos, cbk, dataPtr, priority);
        });
    }

    /**
     * Register a callback for when a specific address is executed.
     *
     * @param {String|Number} addr      Code address which will trigger the callback.
     * @param {InstPosition}  pos       Relative position of the callback (PreInst / PostInst).
     * @param {InstCallback}  cbk       A **native** InstCallback returned by :js:func:`VM.newInstCallback`.
     * @param {Object}        data      User defined data passed to the callback.
     * @param {Int}           priority  The priority of the callback.
     *
     * @return {Number} The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
     */
    addCodeAddrCB(addr, pos, cbk, data, priority = CallbackPriority.PRIORITY_DEFAULT) {
        var vm = this.#vm;
        return this._retainUserData(data, function (dataPtr) {
            return QBDI_C.addCodeAddrCB(vm, addr.toRword(), pos, cbk, dataPtr, priority);
        });
    }

    /**
     * Register a callback for when a specific address range is executed.
     *
     * @param {String|Number} start     Start of the address range which will trigger the callback.
     * @param {String|Number} end       End of the address range which will trigger the callback.
     * @param {InstPosition}  pos       Relative position of the callback (PreInst / PostInst).
     * @param {InstCallback}  cbk       A **native** InstCallback returned by :js:func:`VM.newInstCallback`.
     * @param {Object}        data      User defined data passed to the callback.
     * @param {Int}           priority  The priority of the callback.
     *
     * @return {Number} The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
     */
    addCodeRangeCB(start, end, pos, cbk, data, priority = CallbackPriority.PRIORITY_DEFAULT) {
        var vm = this.#vm;
        return this._retainUserData(data, function (dataPtr) {
            return QBDI_C.addCodeRangeCB(vm, start.toRword(), end.toRword(), pos, cbk, dataPtr, priority);
        });
    }

    /**
     * Register a callback event for a specific VM event.
     *
     * @param {VMEvent}    mask   A mask of VM event type which will trigger the callback.
     * @param {VMCallback} cbk    A **native** VMCallback returned by :js:func:`VM.newVMCallback`.
     * @param {Object}     data   User defined data passed to the callback.
     *
     * @return {Number} The id of the registered instrumentation (or VMError.INVALID_EVENTID in case of failure).
     */
    addVMEventCB(mask, cbk, data) {
        var vm = this.#vm;
        return this._retainUserData(data, function (dataPtr) {
            return QBDI_C.addVMEventCB(vm, mask, cbk, dataPtr);
        });
    }

    /**
     * Remove an instrumentation.
     *
     * @param   {Number} id   The id of the instrumentation to remove.
     * @return  {bool} True if instrumentation has been removed.
     */
    deleteInstrumentation(id) {
        this._releaseUserData(id);
        return QBDI_C.deleteInstrumentation(this.#vm, id) == true;
    }

    /**
     * Remove all the registered instrumentations.
     */
    deleteAllInstrumentations() {
        this._releaseAllUserData();
        QBDI_C.deleteAllInstrumentations(this.#vm);
    }

    /**
     * Obtain the analysis of the current instruction. Analysis results are cached in the VM.
     * The validity of the returned pointer is only guaranteed until the end of the callback, else a deepcopy of the structure is required.
     *
     * @param {AnalysisType} [type] Properties to retrieve during analysis (default to ANALYSIS_INSTRUCTION | ANALYSIS_DISASSEMBLY).
     *
     * @return {InstAnalysis} A :js:class:`InstAnalysis` object containing the analysis result.
     */
    getInstAnalysis(type) {
        type = type || (AnalysisType.ANALYSIS_INSTRUCTION | AnalysisType.ANALYSIS_DISASSEMBLY);
        var analysis = QBDI_C.getInstAnalysis(this.#vm, type);
        if (analysis.isNull()) {
            return NULL;
        }
        return this._parseInstAnalysis(analysis);
    }

    /**
     * Obtain the analysis of a cached instruction. Analysis results are cached in the VM.
     * The validity of the returned pointer is only guaranteed until the end of the callback, else a deepcopy of the structure is required.
     *
     * @param {String|Number} addr    The address of the instruction to analyse.
     * @param {AnalysisType}  [type]  Properties to retrieve during analysis (default to ANALYSIS_INSTRUCTION | ANALYSIS_DISASSEMBLY).
     *
     * @return {InstAnalysis} A :js:class:`InstAnalysis` object containing the analysis result. null if the instruction isn't in the cache.
     */
    getCachedInstAnalysis(addr, type) {
        type = type || (AnalysisType.ANALYSIS_INSTRUCTION | AnalysisType.ANALYSIS_DISASSEMBLY);
        var analysis = QBDI_C.getCachedInstAnalysis(this.#vm, addr.toRword(), type);
        if (analysis.isNull()) {
            return NULL;
        }
        return this._parseInstAnalysis(analysis);
    }

    /**
     * Obtain the memory accesses made by the last executed instruction. Return NULL and a size of 0 if the instruction made no memory access.
     *
     * @param {MemoryAccessType} type Memory mode bitfield to activate the logging for: either MEMORY_READ, MEMORY_WRITE or both (MEMORY_READ_WRITE).
     */
    recordMemoryAccess(type) {
        return QBDI_C.recordMemoryAccess(this.#vm, type) == true;
    }

    /**
     * Obtain the memory accesses made by the last executed instruction. Return NULL and a size of 0 if the instruction made no memory access.
     *
     * @return {MemoryAccess[]} An array of :js:class:`MemoryAccess` made by the instruction.
     */
    getInstMemoryAccess() {
        return this._getMemoryAccess(QBDI_C.getInstMemoryAccess);
    }

    /**
     * Obtain the memory accesses made by the last executed sequence. Return NULL and a size of 0 if the basic block made no memory access.
     *
     * @return {MemoryAccess[]} An array of :js:class:`MemoryAccess` made by the sequence.
     */
    getBBMemoryAccess() {
        return this._getMemoryAccess(QBDI_C.getBBMemoryAccess);
    }

    // Memory

    /**
     * Allocate a new stack and setup the GPRState accordingly. The allocated stack needs to be freed with alignedFree().
     *
     * @param {GPRState} state      Array of register values
     * @param {Number}   stackSize  Size of the stack to be allocated.
     *
     * @return  Pointer (rword) to the allocated memory or NULL in case an error was encountered.
     */
    allocateVirtualStack(state, stackSize) {
        GPRState.validOrThrow(state);
        var stackPtr = Memory.alloc(Process.pointerSize);
        var ret = QBDI_C.allocateVirtualStack(state.ptr, stackSize, stackPtr);
        if (ret == false) {
            return NULL;
        }
        return Memory.readPointer(stackPtr);
    }


    /**
     * Allocate a block of memory of a specified sized with an aligned base address.
     *
     * @param {Number} size   Allocation size in bytes.
     * @param {Number} align  Base address alignement in bytes.
     *
     * @return  Pointer (rword) to the allocated memory or NULL in case an error was encountered.
     */
    alignedAlloc(size, align) {
        return QBDI_C.alignedAlloc(size, align);
    }

    /**
     * Free a block of aligned memory allocated with alignedAlloc or allocateVirtualStack
     *
     * @param {NativePtr} ptr  Pointer to the allocated memory.
     */
    alignedFree(ptr) {
        QBDI_C.alignedFree(ptr);
    }

    /**
     * Simulate a call by modifying the stack and registers accordingly.
     *
     * @param {GPRState}                state     Array of register values
     * @param {String|Number}           retAddr   Return address of the call to simulate.
     * @param {StringArray|NumberArray} args      A variadic list of arguments.
     */
    simulateCall(state, retAddr, args) {
        GPRState.validOrThrow(state);
        retAddr = retAddr.toRword();
        var fargs = this._formatVAArgs(args);
        // Use this weird construction to work around a bug in the duktape runtime
        var _simulateCall = function(a, b, c, d, e, f, g, h, i, j) {
            QBDI_C.simulateCall(state.ptr, retAddr, fargs[0], a, b, c, d, e, f, g, h, i, j);
        }
        _simulateCall.apply(null, fargs[1]);
    }

    /**
     * Use QBDI engine to retrieve loaded modules.
     *
     * @return list of module names (ex: ["ls", "libc", "libz"])
     */
    getModuleNames() {
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
    setLogPriority(priority) {
        QBDI_C.setLogPriority(priority);
    }

    // Helpers

    /**
     * Create a native **Instruction rule callback** from a JS function.
     *
     * Example:
     *       >>> var icbk = vm.newInstrRuleCallback(function(vm, ana, data) {
     *       >>>   console.log("0x" + ana.address.toString(16) + " " + ana.disassembly);
     *       >>>   return [new InstrRuleDataCBK(InstPosition.POSTINST, printCB, ana.disassembly)];
     *       >>> });
     *
     * @param {InstrRuleCallback} cbk an instruction callback (ex: function(vm, ana, data) {};)
     *
     * @return an native InstrRuleCallback
     */
    newInstrRuleCallback(cbk) {
        if (typeof(cbk) !== 'function' || cbk.length !== 3) {
            return undefined;
        }
        // Use a closure to provide object
        var vm = this;
        var jcbk = function(vmPtr, anaPtr, cbksPtr, dataPtr) {
            var ana = vm._parseInstAnalysis(anaPtr);
            var data = vm._getUserData(dataPtr);
            var res = cbk(vm, ana, data.userdata);
            if (res === null) {
                return;
            }
            if (!Array.isArray(res)) {
                throw new TypeError('Invalid InstrRuleDataCBK Array');
            }
            if (res.length === 0) {
                return;
            }
            for (var i = 0; i < res.length; i++) {
                var d = vm._retainUserDataForInstrRuleCB2(res[i].data, data.id);
                QBDI_C.addInstrRuleData(cbksPtr, res[i].position, res[i].cbk, d, res[i].priority);
            }
        }
        return new NativeCallback(jcbk, 'void', ['pointer', 'pointer', 'pointer', 'pointer']);
    }


    /**
     * Create a native **Instruction callback** from a JS function.
     *
     * Example:
     *       >>> var icbk = vm.newInstCallback(function(vm, gpr, fpr, data) {
     *       >>>   inst = vm.getInstAnalysis();
     *       >>>   console.log("0x" + inst.address.toString(16) + " " + inst.disassembly);
     *       >>>   return VMAction.CONTINUE;
     *       >>> });
     *
     * @param {InstCallback} cbk an instruction callback (ex: function(vm, gpr, fpr, data) {};)
     *
     * @return an native InstCallback
     */
    newInstCallback(cbk) {
        if (typeof(cbk) !== 'function' || cbk.length !== 4) {
            return undefined;
        }
        // Use a closure to provide object
        var vm = this;
        var jcbk = function(vmPtr, gprPtr, fprPtr, dataPtr) {
            var gpr = new GPRState(gprPtr);
            var fpr = new FPRState(fprPtr);
            var data = vm._getUserData(dataPtr);
            return cbk(vm, gpr, fpr, data);
        }
        return new NativeCallback(jcbk, 'int', ['pointer', 'pointer', 'pointer', 'pointer']);
    }

    /**
     * Create a native **VM callback** from a JS function.
     *
     * Example:
     *       >>> var vcbk = vm.newVMCallback(function(vm, evt, gpr, fpr, data) {
     *       >>>   if (evt.event & VMEvent.EXEC_TRANSFER_CALL) {
     *       >>>     console.warn("[!] External call to 0x" + evt.basicBlockStart.toString(16));
     *       >>>   }
     *       >>>   return VMAction.CONTINUE;
     *       >>> });
     *
     * @param {VMCallback} cbk a VM callback (ex: function(vm, state, gpr, fpr, data) {};)
     *
     * @return a native VMCallback
     */
    newVMCallback(cbk) {
        if (typeof(cbk) !== 'function' || cbk.length !== 5) {
            return undefined;
        }
        // Use a closure to provide object and a parsed event
        var vm = this;
        var jcbk = function(vmPtr, state, gprPtr, fprPtr, dataPtr) {
            var s = vm._parseVMState(state);
            var gpr = new GPRState(gprPtr);
            var fpr = new FPRState(fprPtr);
            var data = vm._getUserData(dataPtr);
            return cbk(vm, s, gpr, fpr, data);
        }
        return new NativeCallback(jcbk, 'int', ['pointer', 'pointer', 'pointer', 'pointer', 'pointer']);
    }

    /**
     * Call a function by its address (or through a Frida ``NativePointer``).
     *
     * Arguments can be provided, but their types need to be compatible
     * with the ``.toRword()`` interface (like ``NativePointer`` or ``UInt64``).
     *
     * Example:
     *       >>> var vm = new VM();
     *       >>> var state = vm.getGPRState();
     *       >>> var stackTopPtr = vm.allocateVirtualStack(state, 0x1000000);
     *       >>> var aFunction = Module.findExportByName(null, "Secret");
     *       >>> vm.addInstrumentedModuleFromAddr(aFunction);
     *       >>> vm.call(aFunction, [42]);
     *       >>> vm.alignedFree(stackTopPtr);
     *
     * @param {String|Number}           address function address (or Frida ``NativePointer``).
     * @param {StringArray|NumberArray} [args]  optional list of arguments
     */
    call(address, args) {
        address = address.toRword();
        var fargs = this._formatVAArgs(args);
        var vm = this.#vm;
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

    /**
     * Call a function by its address (or through a Frida ``NativePointer``).
     * QBDI will allocate his one stack to run, while the instrumented code will
     * use the top of the current stack.
     *
     * Arguments can be provided, but their types need to be compatible
     * with the ``.toRword()`` interface (like ``NativePointer`` or ``UInt64``).
     *
     * Example:
     *       >>> var vm = new VM();
     *       >>> var state = vm.getGPRState();
     *       >>> var aFunction = Module.findExportByName(null, "Secret");
     *       >>> vm.addInstrumentedModuleFromAddr(aFunction);
     *       >>> vm.switchStackAndCall(aFunction, [42]);
     *
     * @param {String|Number}           address function address (or Frida ``NativePointer``).
     * @param {StringArray|NumberArray} [args]  optional list of arguments
     * @param {String|Number}           stack size for the engine.
     */
    switchStackAndCall(address, args, stackSize) {
        if (stackSize === null || stackSize === undefined) {
            stackSize = 0x20000;
        }
        address = address.toRword();
        var fargs = this._formatVAArgs(args);
        var vm = this.#vm;
        // Use this weird construction to work around a bug in the duktape runtime
        var _scall = function(a, b, c, d, e, f, g, h, i, j) {
            var retPtr = Memory.alloc(Process.pointerSize);
            var res = QBDI_C.switchStackAndCall(vm, retPtr, address, stackSize, fargs[0], a, b, c, d, e, f, g, h, i, j);
            if (res == false) {
                throw new EvalError('Execution failed');
            }
            return ptr(Memory.readRword(retPtr));
        }
        return _scall.apply(null, fargs[1]);
    }

    ////////////////////
    // private method //
    ////////////////////

    _parseStructDesc(ptr) {
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

    _initVM() {
        var vmPtr = Memory.alloc(Process.pointerSize);
        QBDI_C.initVM(vmPtr, NULL, NULL, 0);
        return Memory.readPointer(vmPtr);
    }

    _terminateVM(v) {
        QBDI_C.terminateVM(v);
    }


    // Retain (~reference) a user data object when an instrumentation is added.
    //
    // If a ``NativePointer`` is given, it will be used as raw user data and the
    // object will not be retained.
    _retainUserData(data, fn) {
        var dataPtr = ptr("0");
        var managed = false;
        if (data !== null && data !== undefined) {
            this.#userDataPointer += 1;
            dataPtr = dataPtr.add(this.#userDataPointer);
            managed = true;
        }
        var iid = fn(dataPtr);
        if (managed) {
            this.#userDataPtrMap[dataPtr] = data;
            this.#userDataIIdMap[iid] = dataPtr;
        }
        return iid;
    }

    _retainUserDataForInstrRuleCB(data, fn) {
        this.#userDataPointer += 1;
        var dataPtr = ptr("0").add(this.#userDataPointer);

        var iid = fn(dataPtr);

        this.#userDataPtrMap[dataPtr] = {userdata: data, id: iid};
        this.#userDataIIdMap[iid] = [dataPtr];
        return iid;
    }

    _retainUserDataForInstrRuleCB2(data, id) {
        if (data !== null && data !== undefined) {
            this.#userDataPointer += 1;
            var dataPtr = ptr("0").add(this.#userDataPointer);

            this.#userDataPtrMap[dataPtr] = data;
            this.#userDataIIdMap[id].push(dataPtr);
            return dataPtr;
        } else {
            return ptr("0");
        }
    }

    // Retrieve a user data object from its ``NativePointer`` reference.
    // If pointer is NULL or no data object is found, the ``NativePointer``
    // object will be returned.
    _getUserData(dataPtr) {
        var data = dataPtr;
        if (!data.isNull()) {
            var d = this.#userDataPtrMap[dataPtr];
            if (d !== undefined) {
                return d;
            }
        }
        return undefined;
    }

    // Release references to a user data object using the correponding
    // instrumentation id.
    _releaseUserData(id) {
        var dataPtr = this.#userDataIIdMap[id];
        if (dataPtr !== undefined) {
            if (Array.isArray(dataPtr)) {
                for (var i = 0; i < dataPtr.length; i++) {
                    delete this.#userDataPtrMap[dataPtr[i]];
                }
            } else {
                delete this.#userDataPtrMap[dataPtr];
            }
            delete this.#userDataIIdMap[id];
        }
    }

    // Release all references to user data objects.
    _releaseAllUserData() {
        this.#userDataPtrMap = {};
        this.#userDataIIdMap = {};
        this.#userDataPointer = 0;
    }

    _formatVAArgs(args) {
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

    _parseMemoryAccess(ptr) {
        var access = {};
        var p = ptr.add(this.#instAnalysisStructDesc.offsets[0]);
        access.instAddress = Memory.readRword(p);
        p = ptr.add(this.#memoryAccessDesc.offsets[1]);
        access.accessAddress = Memory.readRword(p);
        p = ptr.add(this.#memoryAccessDesc.offsets[2]);
        access.value = Memory.readRword(p);
        p = ptr.add(this.#memoryAccessDesc.offsets[3]);
        access.size = Memory.readU16(p);
        p = ptr.add(this.#memoryAccessDesc.offsets[4]);
        access.type = Memory.readU8(p);
        p = ptr.add(this.#memoryAccessDesc.offsets[5]);
        access.flags = Memory.readU8(p);
        Object.freeze(access);
        return access;
    }

    _getMemoryAccess(f) {
        var accesses = [];
        var sizePtr = Memory.alloc(4);
        var accessPtr = f(this.#vm, sizePtr);
        if (accessPtr.isNull()) {
            return [];
        }
        var cnt = Memory.readU32(sizePtr);
        var sSize = this.#memoryAccessDesc.size;
        var p = accessPtr;
        for (var i = 0; i < cnt; i++) {
            var access = this._parseMemoryAccess(p);
            accesses.push(access);
            p = p.add(sSize);
        }
        System.free(accessPtr);
        return accesses;
    }

    _parseVMState(ptr) {
        var state = {};
        var p = ptr.add(this.#instAnalysisStructDesc.offsets[0]);
        state.event = Memory.readU8(p);
        p = ptr.add(this.#vmStateStructDesc.offsets[1]);
        state.sequenceStart = Memory.readRword(p);
        p = ptr.add(this.#vmStateStructDesc.offsets[2]);
        state.sequenceEnd = Memory.readRword(p);
        p = ptr.add(this.#vmStateStructDesc.offsets[3]);
        state.basicBlockStart = Memory.readRword(p);
        p = ptr.add(this.#vmStateStructDesc.offsets[4]);
        state.basicBlockEnd = Memory.readRword(p);
        p = ptr.add(this.#vmStateStructDesc.offsets[5]);
        state.lastSignal = Memory.readRword(p);
        Object.freeze(state);
        return state;
    }

    _parseOperandAnalysis(ptr) {
        var analysis = {};
        var p = ptr.add(this.#instAnalysisStructDesc.offsets[0]);
        analysis.type = Memory.readU32(p);
        p = ptr.add(this.#operandAnalysisStructDesc.offsets[1]);
        analysis.flag = Memory.readU8(p);
        p = ptr.add(this.#operandAnalysisStructDesc.offsets[2]);
        analysis.value = Memory.readRword(p);
        p = ptr.add(this.#operandAnalysisStructDesc.offsets[3]);
        analysis.size = Memory.readU8(p);
        p = ptr.add(this.#operandAnalysisStructDesc.offsets[4]);
        analysis.regOff = Memory.readU8(p);
        p = ptr.add(this.#operandAnalysisStructDesc.offsets[5]);
        analysis.regCtxIdx = Memory.readS16(p);
        p = ptr.add(this.#operandAnalysisStructDesc.offsets[6]);
        var regNamePtr = Memory.readPointer(p);
        if (regNamePtr.isNull()) {
            analysis.regName = undefined;
        } else {
            analysis.regName = Memory.readCString(regNamePtr);
        }
        p = ptr.add(this.#operandAnalysisStructDesc.offsets[7]);
        analysis.regAccess = Memory.readU8(p);
        Object.freeze(analysis);
        return analysis;
    }

    _parseInstAnalysis(ptr) {
        var analysis = {};
        var p = ptr.add(this.#instAnalysisStructDesc.offsets[0]);
        analysis.mnemonic = Memory.readCString(Memory.readPointer(p));
        p = ptr.add(this.#instAnalysisStructDesc.offsets[1]);
        analysis.disassembly = Memory.readCString(Memory.readPointer(p));
        p = ptr.add(this.#instAnalysisStructDesc.offsets[2]);
        analysis.address = Memory.readRword(p);
        p = ptr.add(this.#instAnalysisStructDesc.offsets[3]);
        analysis.instSize = Memory.readU32(p);
        p = ptr.add(this.#instAnalysisStructDesc.offsets[4]);
        analysis.affectControlFlow = Memory.readU8(p) == true;
        p = ptr.add(this.#instAnalysisStructDesc.offsets[5]);
        analysis.isBranch = Memory.readU8(p) == true;
        p = ptr.add(this.#instAnalysisStructDesc.offsets[6]);
        analysis.isCall = Memory.readU8(p) == true;
        p = ptr.add(this.#instAnalysisStructDesc.offsets[7]);
        analysis.isReturn = Memory.readU8(p) == true;
        p = ptr.add(this.#instAnalysisStructDesc.offsets[8]);
        analysis.isCompare = Memory.readU8(p) == true;
        p = ptr.add(this.#instAnalysisStructDesc.offsets[9]);
        analysis.isPredicable = Memory.readU8(p) == true;
        p = ptr.add(this.#instAnalysisStructDesc.offsets[10]);
        analysis.isMoveImm = Memory.readU8(p) == true;
        p = ptr.add(this.#instAnalysisStructDesc.offsets[11]);
        analysis.mayLoad = Memory.readU8(p) == true;
        p = ptr.add(this.#instAnalysisStructDesc.offsets[12]);
        analysis.mayStore = Memory.readU8(p) == true;
        p = ptr.add(this.#instAnalysisStructDesc.offsets[13]);
        analysis.loadSize = Memory.readU32(p);
        p = ptr.add(this.#instAnalysisStructDesc.offsets[14]);
        analysis.storeSize = Memory.readU32(p);
        p = ptr.add(this.#instAnalysisStructDesc.offsets[15]);
        analysis.condition = Memory.readU8(p);
        p = ptr.add(this.#instAnalysisStructDesc.offsets[16]);
        analysis.flagsAccess = Memory.readU8(p);
        p = ptr.add(this.#instAnalysisStructDesc.offsets[17]);
        var numOperands = Memory.readU8(p);
        p = ptr.add(this.#instAnalysisStructDesc.offsets[18]);
        var operandsPtr = Memory.readPointer(p);
        analysis.operands = new Array(numOperands);
        for (var i = 0; i < numOperands; i++) {
            analysis.operands[i] = this._parseOperandAnalysis(operandsPtr);
            operandsPtr = operandsPtr.add(this.#operandAnalysisStructDesc.size);
        }
        p = ptr.add(this.#instAnalysisStructDesc.offsets[19]);
        var symbolPtr = Memory.readPointer(p);
        if (!symbolPtr.isNull()) {
            analysis.symbolName = Memory.readCString(symbolPtr);
        } else {
            analysis.symbolName = "";
        }
        analysis.symbol = analysis.symbolName; // deprecated Name
        p = ptr.add(this.#instAnalysisStructDesc.offsets[20]);
        analysis.symbolOffset = Memory.readU32(p);
        p = ptr.add(this.#instAnalysisStructDesc.offsets[21]);
        var modulePtr = Memory.readPointer(p);
        if (!modulePtr.isNull()) {
            analysis.moduleName = Memory.readCString(modulePtr);
        } else {
            analysis.moduleName = "";
        }
        analysis.module = analysis.moduleName; // deprecated Name
        p = ptr.add(this.#instAnalysisStructDesc.offsets[22]);
        analysis.cpuMode = Memory.readU8(p);
        Object.freeze(analysis);
        return analysis;
    }

};

