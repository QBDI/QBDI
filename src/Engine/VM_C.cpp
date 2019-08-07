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
#include "Platform.h"
#include "Errors.h"
#include "VM_C.h"
#include "VM.h"
#include "Utility/LogSys.h"

namespace QBDI {

void qbdi_initVM(VMInstanceRef* instance, const char* cpu, const char** mattrs) {
    RequireAction("VM_C::initVM", instance, return);

    *instance = nullptr;
    std::string cpuStr = "";
    std::vector<std::string> mattrsStr;

    if(cpu != nullptr) {
        cpuStr += cpu;
    }

    if(mattrs != nullptr) {
        for(unsigned i = 0; mattrs[i] != nullptr; i++) {
            mattrsStr.push_back(std::string(mattrs[i]));
        }
    }

    *instance = static_cast<VMInstanceRef>(new VM(cpuStr, mattrsStr));
}


void qbdi_terminateVM(VMInstanceRef instance) {
    RequireAction("VM_C::terminateVM", instance, return);
    delete static_cast<VM*>(instance);
}

void qbdi_addInstrumentedRange(VMInstanceRef instance, rword start, rword end) {
    RequireAction("VM_C::addInstrumentedRange", instance, return);
    static_cast<VM*>(instance)->addInstrumentedRange(start, end);
}

bool qbdi_addInstrumentedModule(VMInstanceRef instance, const char* name) {
    RequireAction("VM_C::addInstrumentedModule", instance, return false);
    return static_cast<VM*>(instance)->addInstrumentedModule(std::string(name));
}

bool qbdi_addInstrumentedModuleFromAddr(VMInstanceRef instance, rword addr) {
    RequireAction("VM_C::addInstrumentedModuleFromAddr", instance, return false);
    return static_cast<VM*>(instance)->addInstrumentedModuleFromAddr(addr);
}

bool qbdi_instrumentAllExecutableMaps(VMInstanceRef instance) {
    RequireAction("VM_C::instrumentAllExecutableMaps", instance, return false);
    return static_cast<VM*>(instance)->instrumentAllExecutableMaps();
}

void qbdi_removeInstrumentedRange(VMInstanceRef instance, rword start, rword end) {
    RequireAction("VM_C::removeInstrumentedRange", instance, return);
    static_cast<VM*>(instance)->removeInstrumentedRange(start, end);
}

void qbdi_removeAllInstrumentedRanges(VMInstanceRef instance) {
    RequireAction("VM_C::removeAllInstrumentedRanges", instance, return);
    static_cast<VM*>(instance)->removeAllInstrumentedRanges();
}

bool qbdi_removeInstrumentedModule(VMInstanceRef instance, const char* name) {
    RequireAction("VM_C::removeInstrumentedModule", instance, return false);
    return static_cast<VM*>(instance)->removeInstrumentedModule(std::string(name));
}

bool qbdi_removeInstrumentedModuleFromAddr(VMInstanceRef instance, rword addr) {
    RequireAction("VM_C::removeInstrumentedModuleFromAddr", instance, return false);
    return static_cast<VM*>(instance)->removeInstrumentedModuleFromAddr(addr);
}

bool qbdi_run(VMInstanceRef instance, rword start, rword stop) {
    RequireAction("VM_C::run", instance, return false);
    return static_cast<VM*>(instance)->run(start, stop);
}

bool qbdi_call(VMInstanceRef instance, rword* retval, rword function, uint32_t argNum, ...) {
    RequireAction("VM_C::call", instance, return false);
    va_list ap;
    va_start(ap, argNum);
    bool res = static_cast<VM*>(instance)->callV(retval, function, argNum, ap);
    va_end(ap);
    return res;
}

bool qbdi_callV(VMInstanceRef instance, rword* retval, rword function, uint32_t argNum, va_list ap) {
    RequireAction("VM_C::callV", instance, return false);
    return static_cast<VM*>(instance)->callV(retval, function, argNum, ap);
}

bool qbdi_callA(VMInstanceRef instance, rword* retval, rword function, uint32_t argNum, const rword* args) {
    RequireAction("VM_C::callA", instance, return false);
    return static_cast<VM*>(instance)->callA(retval, function, argNum, args);
}

GPRState* qbdi_getGPRState(VMInstanceRef instance) {
    RequireAction("VM_C::getGPRState", instance, return nullptr);
    return static_cast<VM*>(instance)->getGPRState();
}

FPRState* qbdi_getFPRState(VMInstanceRef instance) {
    RequireAction("VM_C::getFPRState", instance, return nullptr);
    return static_cast<VM*>(instance)->getFPRState();
}

void qbdi_setGPRState(VMInstanceRef instance, GPRState* gprState) {
    RequireAction("VM_C::setGPRState", instance, return);
    static_cast<VM*>(instance)->setGPRState(gprState);
}

void qbdi_setFPRState(VMInstanceRef instance, FPRState* gprState) {
    RequireAction("VM_C::setFPRState", instance, return);
    static_cast<VM*>(instance)->setFPRState(gprState);
}

uint32_t qbdi_addMnemonicCB(VMInstanceRef instance, const char* mnemonic, InstPosition pos, InstCallback cbk, void *data) {
    RequireAction("VM_C::addMnemonicCB", instance, return VMError::INVALID_EVENTID);
    return static_cast<VM*>(instance)->addMnemonicCB(mnemonic, pos, cbk, data);
}

uint32_t qbdi_addCodeCB(VMInstanceRef instance, InstPosition pos, InstCallback cbk, void *data) {
    RequireAction("VM_C::addCodeCB", instance, return VMError::INVALID_EVENTID);
    return static_cast<VM*>(instance)->addCodeCB(pos, cbk, data);
}

uint32_t qbdi_addCodeAddrCB(VMInstanceRef instance, rword address, InstPosition pos, InstCallback cbk, void *data) {
    RequireAction("VM_C::addCodeAddrCB", instance, return VMError::INVALID_EVENTID);
    return static_cast<VM*>(instance)->addCodeAddrCB(address, pos, cbk, data);
}

uint32_t qbdi_addCodeRangeCB(VMInstanceRef instance, rword start, rword end, InstPosition pos, InstCallback cbk, void *data) {
    RequireAction("VM_C::addCodeRangeCB", instance, return VMError::INVALID_EVENTID);
    return static_cast<VM*>(instance)->addCodeRangeCB(start, end, pos, cbk, data);
}

uint32_t qbdi_addMemAccessCB(VMInstanceRef instance, MemoryAccessType type, InstCallback cbk, void *data) {
    RequireAction("VM_C::addMemAccessCB", instance, return VMError::INVALID_EVENTID);
    return static_cast<VM*>(instance)->addMemAccessCB(type, cbk, data);
}

uint32_t qbdi_addMemAddrCB(VMInstanceRef instance, rword address, MemoryAccessType type, InstCallback cbk, void *data) {
    RequireAction("VM_C::addMemAddrCB", instance, return VMError::INVALID_EVENTID);
    return static_cast<VM*>(instance)->addMemAddrCB(address, type, cbk, data);
}

uint32_t qbdi_addMemRangeCB(VMInstanceRef instance, rword start, rword end, MemoryAccessType type, InstCallback cbk, void *data) {
    RequireAction("VM_C::addMemRangeCB", instance, return VMError::INVALID_EVENTID);
    return static_cast<VM*>(instance)->addMemRangeCB(start, end, type, cbk, data);
}

uint32_t qbdi_addVMEventCB(VMInstanceRef instance, VMEvent mask, VMCallback cbk, void *data) {
    RequireAction("VM_C::addVMEventCB", instance, return VMError::INVALID_EVENTID);
    return static_cast<VM*>(instance)->addVMEventCB(mask, cbk, data);
}

bool qbdi_deleteInstrumentation(VMInstanceRef instance, uint32_t id) {
    RequireAction("VM_C::deleteInstrumentation", instance, return false);
    return static_cast<VM*>(instance)->deleteInstrumentation(id);
}

void qbdi_deleteAllInstrumentations(VMInstanceRef instance) {
    RequireAction("VM_C::deleteAllInstrumentations", instance, return);
    static_cast<VM*>(instance)->deleteAllInstrumentations();
}

const InstAnalysis* qbdi_getInstAnalysis(VMInstanceRef instance, AnalysisType type) {
    RequireAction("VM_C::getInstAnalysis", instance, return nullptr);
    return static_cast<VM*>(instance)->getInstAnalysis(type);
}

bool qbdi_recordMemoryAccess(VMInstanceRef instance, MemoryAccessType type) {
    RequireAction("VM_C::recordMemoryAccess", instance, return false);
    return static_cast<VM*>(instance)->recordMemoryAccess(type);
}

MemoryAccess* qbdi_getInstMemoryAccess(VMInstanceRef instance, size_t* size) {
    RequireAction("VM_C::getInstMemoryAccess", instance, return nullptr);
    RequireAction("VM_C::getInstMemoryAccess", size, return nullptr);
    *size = 0;
    std::vector<MemoryAccess> ma_vec = static_cast<VM*>(instance)->getInstMemoryAccess();
    // Do not allocate if no shadows
    if(ma_vec.size() == 0) {
        return NULL;
    }
    // Allocate and copy
    *size = ma_vec.size();
    MemoryAccess* ma_arr = static_cast<MemoryAccess*>(malloc(*size * sizeof(MemoryAccess)));
    for(size_t i = 0; i < *size; i++) {
        ma_arr[i] = ma_vec[i];
    }
    return ma_arr;
}

MemoryAccess* qbdi_getBBMemoryAccess(VMInstanceRef instance, size_t* size) {
    RequireAction("VM_C::getBBMemoryAccess", instance, return nullptr);
    RequireAction("VM_C::getBBMemoryAccess", size, return nullptr);
    *size = 0;
    std::vector<MemoryAccess> ma_vec = static_cast<VM*>(instance)->getBBMemoryAccess();
    // Do not allocate if no shadows
    if(ma_vec.size() == 0) {
        return NULL;
    }
    // Allocate and copy
    *size = ma_vec.size();
    MemoryAccess* ma_arr = static_cast<MemoryAccess*>(malloc(*size * sizeof(MemoryAccess)));
    for(size_t i = 0; i < *size; i++) {
        ma_arr[i] = ma_vec[i];
    }
    return ma_arr;
}

bool qbdi_precacheBasicBlock(VMInstanceRef instance, rword pc) {
    RequireAction("VM_C::precacheBasicBlock", instance, return false);
    return static_cast<VM*>(instance)->precacheBasicBlock(pc);
}

void qbdi_clearAllCache(VMInstanceRef instance) {
    static_cast<VM*>(instance)->clearAllCache();
}

void qbdi_clearCache(VMInstanceRef instance, rword start, rword end) {
    static_cast<VM*>(instance)->clearCache(start, end);
}

typedef struct {
    QBDI_InstrumentCallback cbk;
    void* data;
} VMC_CallbackData;

std::vector<InstrumentDataCBK> VMC_callback(VMInstanceRef vm, const InstAnalysis *inst, void* _data) {
    VMC_CallbackData* data = static_cast<VMC_CallbackData*>(_data);
    size_t len = 0;
    InstrumentDataCBK* instrumentCBK = data->cbk(vm, inst, data->data, &len);

    std::vector<InstrumentDataCBK> result;
    if (instrumentCBK != nullptr) {
        for (size_t i = 0; i < len; i++) {
            result.push_back(instrumentCBK[i]);
        }
        free(instrumentCBK);
    }
    return result;
}

uint32_t qbdi_addInstrRule(VMInstanceRef instance, QBDI_InstrumentCallback cbk, AnalysisType type, void* data) {
    RequireAction("VM_C::addInstrRule", instance, return VMError::INVALID_EVENTID);
    return static_cast<VM*>(instance)->addInstrRule(VMC_callback, type, new VMC_CallbackData{cbk, data});
}

uint32_t qbdi_addInstrRuleRange(VMInstanceRef instance, rword start, rword end, QBDI_InstrumentCallback cbk, AnalysisType type, void* data) {
    RequireAction("VM_C::addInstrRuleRange", instance, return VMError::INVALID_EVENTID);
    return static_cast<VM*>(instance)->addInstrRuleRange(start, end, VMC_callback, type, new VMC_CallbackData{cbk, data});
}

}
