/*
 * This stub is used to export QBDI as a dynamic library.
 */
#include <QBDI.h>
#include <QBDI/VM_C.h>
#include <QBDI/Memory.h>

#include <stddef.h>
#ifdef QBDI_OS_WIN
#include <windows.h>
#define _QBDI_EXPORT_GLOBAL_PROPERTY const
#else
#define _QBDI_EXPORT_GLOBAL_PROPERTY static const
#endif


#define FORCE_EXPORT(NAME) \
    _QBDI_EXPORT_GLOBAL_PROPERTY void* dummy__##NAME _QBDI_FORCE_USE = (const void*) &(QBDI::NAME);
#define FORCE_EXPORT_C(NAME) FORCE_EXPORT(qbdi_##NAME)

// VM
FORCE_EXPORT_C(initVM)
FORCE_EXPORT_C(terminateVM)
FORCE_EXPORT_C(addInstrumentedRange)
FORCE_EXPORT_C(addInstrumentedModule)
FORCE_EXPORT_C(addInstrumentedModuleFromAddr)
FORCE_EXPORT_C(instrumentAllExecutableMaps)
FORCE_EXPORT_C(removeInstrumentedRange)
FORCE_EXPORT_C(removeInstrumentedModule)
FORCE_EXPORT_C(removeInstrumentedModuleFromAddr)
FORCE_EXPORT_C(removeAllInstrumentedRanges)
FORCE_EXPORT_C(run)
FORCE_EXPORT_C(call)
FORCE_EXPORT_C(callV)
FORCE_EXPORT_C(callA)
FORCE_EXPORT_C(getGPRState)
FORCE_EXPORT_C(getFPRState)
FORCE_EXPORT_C(setGPRState)
FORCE_EXPORT_C(setFPRState)
FORCE_EXPORT_C(addInstrRule)
FORCE_EXPORT_C(addInstrRuleRange)
FORCE_EXPORT_C(addMnemonicCB)
FORCE_EXPORT_C(addMemAccessCB)
FORCE_EXPORT_C(addMemAddrCB)
FORCE_EXPORT_C(addMemRangeCB)
FORCE_EXPORT_C(addCodeCB)
FORCE_EXPORT_C(addCodeAddrCB)
FORCE_EXPORT_C(addCodeRangeCB)
FORCE_EXPORT_C(addVMEventCB)
FORCE_EXPORT_C(deleteInstrumentation)
FORCE_EXPORT_C(deleteAllInstrumentations)
FORCE_EXPORT_C(getInstAnalysis)
FORCE_EXPORT_C(recordMemoryAccess)
FORCE_EXPORT_C(getInstMemoryAccess)
FORCE_EXPORT_C(getBBMemoryAccess)
FORCE_EXPORT_C(precacheBasicBlock)
FORCE_EXPORT_C(clearCache)
FORCE_EXPORT_C(clearAllCache)

// Logs
FORCE_EXPORT_C(setLogOutput)
FORCE_EXPORT_C(addLogFilter)

// Memory
FORCE_EXPORT(getCurrentProcessMaps)
FORCE_EXPORT(getRemoteProcessMaps)
FORCE_EXPORT_C(alignedAlloc)
FORCE_EXPORT_C(alignedFree)
FORCE_EXPORT_C(allocateVirtualStack)
FORCE_EXPORT_C(simulateCall)
FORCE_EXPORT_C(simulateCallV)
FORCE_EXPORT_C(simulateCallA)
static const void* dummy__getModuleNames1 _QBDI_FORCE_USE = (const void*) (std::vector<std::string> (*)())&(QBDI::getModuleNames);
static const void* dummy__getModuleNames2 _QBDI_FORCE_USE = (const void*) (char** (*)(size_t*))&(QBDI::getModuleNames);

// Helpers
#ifdef __cplusplus
namespace QBDI {
extern "C" {
#endif

struct StructDesc {
    uint32_t size;
    uint32_t items;
    uint32_t offsets[30];
};

struct StructDesc MemoryAccessDesc {
    sizeof(MemoryAccess),
    5,
    {
        offsetof(MemoryAccess, instAddress),
        offsetof(MemoryAccess, accessAddress),
        offsetof(MemoryAccess, value),
        offsetof(MemoryAccess, size),
        offsetof(MemoryAccess, type)
    }
};

struct StructDesc VMStateDesc {
    sizeof(VMState),
    6,
    {
        offsetof(VMState, event),
        offsetof(VMState, sequenceStart),
        offsetof(VMState, sequenceEnd),
        offsetof(VMState, basicBlockStart),
        offsetof(VMState, basicBlockEnd),
        offsetof(VMState, lastSignal),
    }
};

struct StructDesc OperandAnalysisDesc {
    sizeof(OperandAnalysis),
    7,
    {
        offsetof(OperandAnalysis, type),
        offsetof(OperandAnalysis, value),
        offsetof(OperandAnalysis, size),
        offsetof(OperandAnalysis, regOff),
        offsetof(OperandAnalysis, regCtxIdx),
        offsetof(OperandAnalysis, regName),
        offsetof(OperandAnalysis, regAccess),
    }
};

struct StructDesc InstAnalysisDesc {
    sizeof(InstAnalysis),
    17,
    {
        offsetof(InstAnalysis, mnemonic),
        offsetof(InstAnalysis, disassembly),
        offsetof(InstAnalysis, address),
        offsetof(InstAnalysis, instSize),
        offsetof(InstAnalysis, affectControlFlow),
        offsetof(InstAnalysis, isBranch),
        offsetof(InstAnalysis, isCall),
        offsetof(InstAnalysis, isReturn),
        offsetof(InstAnalysis, isCompare),
        offsetof(InstAnalysis, isPredicable),
        offsetof(InstAnalysis, mayLoad),
        offsetof(InstAnalysis, mayStore),
        offsetof(InstAnalysis, numOperands),
        offsetof(InstAnalysis, operands),
        offsetof(InstAnalysis, symbol),
        offsetof(InstAnalysis, symbolOffset),
        offsetof(InstAnalysis, module),
    }
};

#ifdef QBDI_OS_WIN
QBDI_EXPORT BOOLEAN WINAPI DllMain(IN HINSTANCE hDllHandle, IN DWORD nReason, IN LPVOID Reserved) {
    switch (nReason) {
        case DLL_PROCESS_ATTACH:
            //  For optimization.
            DisableThreadLibraryCalls(hDllHandle);
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
#endif // QBDI_OS_WIN

QBDI_EXPORT rword qbdi_getGPR(GPRState* state, uint32_t rid) {
    return QBDI_GPR_GET(state, rid);
}

QBDI_EXPORT void qbdi_setGPR(GPRState* state, uint32_t rid, rword val) {
    QBDI_GPR_SET(state, rid, val);
}

QBDI_EXPORT struct StructDesc* qbdi_getMemoryAccessStructDesc() {
    return &MemoryAccessDesc;
}

QBDI_EXPORT struct StructDesc* qbdi_getVMStateStructDesc() {
    return &VMStateDesc;
}

QBDI_EXPORT struct StructDesc* qbdi_getOperandAnalysisStructDesc() {
    return &OperandAnalysisDesc;
}

QBDI_EXPORT struct StructDesc* qbdi_getInstAnalysisStructDesc() {
    return &InstAnalysisDesc;
}

#ifdef __cplusplus
} // "C"
} // QBDI::
#endif

FORCE_EXPORT(getVersion);
FORCE_EXPORT(qbdi_getGPR);
FORCE_EXPORT(qbdi_setGPR);
FORCE_EXPORT(qbdi_getMemoryAccessStructDesc);
FORCE_EXPORT(qbdi_getVMStateStructDesc);
FORCE_EXPORT(qbdi_getInstAnalysisStructDesc);
