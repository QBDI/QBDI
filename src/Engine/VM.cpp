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
#include "VM.h"
#include "Range.h"
#include "Errors.h"
#include "Memory.hpp"

#include "Engine/Engine.h"
#include "Patch/InstrRules.h"
#include "Utility/LogSys.h"

// Mask to identify Virtual Callback events
#define EVENTID_VIRTCB_MASK  (1UL << 31)

namespace QBDI {

struct MemCBInfo {
    MemoryAccessType type;
    Range<rword> range;
    InstCallback cbk;
    void* data;
};

VMAction memReadGate(VMInstanceRef vm, GPRState* gprState, FPRState* fprState, void* data) {
    std::vector<std::pair<uint32_t, MemCBInfo>>* memCBInfos = static_cast<std::vector<std::pair<uint32_t, MemCBInfo>>*>(data);
    std::vector<MemoryAccess> memAccesses = vm->getInstMemoryAccess();
    VMAction action = VMAction::CONTINUE;
    for(const MemoryAccess& memAccess : memAccesses) {
        Range<rword> accessRange(memAccess.accessAddress, memAccess.accessAddress + memAccess.size);
        for(size_t i = 0; i < memCBInfos->size(); i++) {
            // Check access type
            if((*memCBInfos)[i].second.type == MEMORY_READ && (memAccess.type & (*memCBInfos)[i].second.type)) {
                // Check access range
                if((*memCBInfos)[i].second.range.overlaps(accessRange)) {
                    // Forward to virtual callback
                    VMAction ret = (*memCBInfos)[i].second.cbk(vm, gprState, fprState, (*memCBInfos)[i].second.data);
                    // Always keep the most extreme action as the return
                    if(ret > action) {
                        action = ret;
                    }
                }
            }
        }
    }
    return action;
}

VMAction memWriteGate(VMInstanceRef vm, GPRState* gprState, FPRState* fprState, void* data) {
    std::vector<std::pair<uint32_t, MemCBInfo>>* memCBInfos = static_cast<std::vector<std::pair<uint32_t, MemCBInfo>>*>(data);
    std::vector<MemoryAccess> memAccesses = vm->getInstMemoryAccess();
    VMAction action = VMAction::CONTINUE;
    for(const MemoryAccess& memAccess : memAccesses) {
        Range<rword> accessRange(memAccess.accessAddress, memAccess.accessAddress + memAccess.size);
        for(size_t i = 0; i < memCBInfos->size(); i++) {
            // Check access type
            if(((*memCBInfos)[i].second.type & MEMORY_WRITE) && (memAccess.type & (*memCBInfos)[i].second.type)) {
                // Check access range
                if((*memCBInfos)[i].second.range.overlaps(accessRange)) {
                    // Forward to virtual callback
                    VMAction ret = (*memCBInfos)[i].second.cbk(vm, gprState, fprState, (*memCBInfos)[i].second.data);
                    // Always keep the most extreme action as the return
                    if(ret > action) {
                        action = ret;
                    }
                }
            }
        }
    }
    return action;
}

VMAction stopCallback(VMInstanceRef vm, GPRState* gprState, FPRState* fprState, void* data) {
    return VMAction::STOP;
}

VM::VM(const std::string& cpu, const std::vector<std::string>& mattrs) :
    memoryLoggingLevel(0), memCBID(0), memReadGateCBID(VMError::INVALID_EVENTID), memWriteGateCBID(VMError::INVALID_EVENTID) {
    engine = new Engine(cpu, mattrs, this);
    memCBInfos = new std::vector<std::pair<uint32_t, MemCBInfo>>;
}

VM::~VM() {
    delete memCBInfos;
    delete engine;
}

GPRState* VM::getGPRState() const {
    return engine->getGPRState();
}

FPRState* VM::getFPRState() const {
    return engine->getFPRState();
}


void VM::setGPRState(GPRState* gprState) {
    RequireAction("VM::setGPRState", gprState != nullptr, return);
    engine->setGPRState(gprState);
}

void VM::setFPRState(FPRState* fprState) {
    RequireAction("VM::setFPRState", fprState != nullptr, return);
    engine->setFPRState(fprState);
}

void VM::addInstrumentedRange(rword start, rword end) {
    RequireAction("VM::addInstrumentedRange", start < end, return);
    engine->addInstrumentedRange(start, end);
}

bool VM::addInstrumentedModule(const std::string& name) {
    return engine->addInstrumentedModule(name);
}

bool VM::addInstrumentedModuleFromAddr(rword addr) {
    return engine->addInstrumentedModuleFromAddr(addr);
}

bool VM::instrumentAllExecutableMaps() {
    return engine->instrumentAllExecutableMaps();
}

void VM::removeInstrumentedRange(rword start, rword end) {
    RequireAction("VM::removeInstrumentedRange", start < end, return);
    engine->removeInstrumentedRange(start, end);
}

void VM::removeAllInstrumentedRanges() {
    engine->removeAllInstrumentedRanges();
}

bool VM::removeInstrumentedModule(const std::string& name) {
    return engine->removeInstrumentedModule(name);
}

bool VM::removeInstrumentedModuleFromAddr(rword addr) {
    return engine->removeInstrumentedModuleFromAddr(addr);
}

bool VM::run(rword start, rword stop) {
    uint32_t stopCB = addCodeAddrCB(stop, InstPosition::PREINST, stopCallback, nullptr);
    bool ret = engine->run(start, stop);
    deleteInstrumentation(stopCB);
    return ret;
}

#define FAKE_RET_ADDR 42

bool VM::callA(rword* retval, rword function, uint32_t argNum, const rword* args) {
    GPRState* state = nullptr;

    state = getGPRState();
    RequireAction("VM::callA", state != nullptr, abort());

    // a stack pointer must be set in state
    if (QBDI_GPR_GET(state, REG_SP) == 0) {
        return false;
    }
    // push arguments in current context
    simulateCallA(state, FAKE_RET_ADDR, argNum, args);
    // call function
    bool res = run(function, FAKE_RET_ADDR);
    // get return value from current state
    if (retval != nullptr) {
        *retval = QBDI_GPR_GET(state, REG_RETURN);
    }
    return res;
}

bool VM::call(rword* retval, rword function, const std::vector<rword>& args) {
    return this->callA(retval, function, args.size(), args.data());
}

bool VM::callV(rword* retval, rword function, uint32_t argNum, va_list ap) {
    rword* args = new rword[argNum];
    for(uint32_t i = 0; i < argNum; i++) {
        args[i] = va_arg(ap, rword);
    }

    bool res = this->callA(retval, function, argNum, args);

    delete[] args;
    return res;
}

uint32_t VM::addInstrRule(InstrumentCallback cbk, AnalysisType type, void* data) {
    RangeSet<rword> r;
    r.add(Range<rword>(0, (rword) -1));
    return engine->addInstrRule(InstrRuleUser(cbk, type, data, this, r));
}

uint32_t VM::addInstrRuleRange(rword start, rword end, InstrumentCallback cbk, AnalysisType type, void* data) {
    RangeSet<rword> r;
    r.add(Range<rword>(start, end));
    return engine->addInstrRule(InstrRuleUser(cbk, type, data, this, r));
}

uint32_t VM::addInstrRuleRangeSet(RangeSet<rword> range, InstrumentCallback cbk, AnalysisType type, void* data) {
    return engine->addInstrRule(InstrRuleUser(cbk, type, data, this, range));
}

uint32_t VM::addMnemonicCB(const char* mnemonic, InstPosition pos, InstCallback cbk, void *data) {
    RequireAction("VM::addMnemonicCB", mnemonic != nullptr, return VMError::INVALID_EVENTID);
    RequireAction("VM::addMnemonicCB", cbk != nullptr, return VMError::INVALID_EVENTID);
    return engine->addInstrRule(InstrRuleBasic(
        MnemonicIs(mnemonic),
        getCallbackGenerator(cbk, data),
        pos,
        true
    ));
}

uint32_t VM::addCodeCB(InstPosition pos, InstCallback cbk, void *data) {
    RequireAction("VM::addCodeCB", cbk != nullptr, return VMError::INVALID_EVENTID);
    return engine->addInstrRule(InstrRuleBasic(
        True(),
        getCallbackGenerator(cbk, data),
        pos,
        true
    ));
}

uint32_t VM::addCodeAddrCB(rword address, InstPosition pos, InstCallback cbk, void *data) {
    RequireAction("VM::addCodeAddrCB", cbk != nullptr, return VMError::INVALID_EVENTID);
    return engine->addInstrRule(InstrRuleBasic(
        AddressIs(address),
        getCallbackGenerator(cbk, data),
        pos,
        true
    ));
}

uint32_t VM::addCodeRangeCB(rword start, rword end, InstPosition pos, InstCallback cbk, void *data) {
    RequireAction("VM::addCodeRangeCB", start < end, return VMError::INVALID_EVENTID);
    RequireAction("VM::addCodeRangeCB", cbk != nullptr, return VMError::INVALID_EVENTID);
    return engine->addInstrRule(InstrRuleBasic(
        InstructionInRange(start, end),
        getCallbackGenerator(cbk, data),
        pos,
        true
    ));
}

uint32_t VM::addMemAccessCB(MemoryAccessType type, InstCallback cbk, void *data) {
    RequireAction("VM::addMemAccessCB", cbk != nullptr, return VMError::INVALID_EVENTID);
    recordMemoryAccess(type);
    switch(type) {
        case MEMORY_READ:
            return engine->addInstrRule(InstrRuleBasic(
                DoesReadAccess(),
                getCallbackGenerator(cbk, data),
                InstPosition::PREINST,
                true
            ));
        case MEMORY_WRITE:
            return engine->addInstrRule(InstrRuleBasic(
                DoesWriteAccess(),
                getCallbackGenerator(cbk, data),
                InstPosition::POSTINST,
                true
            ));
        case MEMORY_READ_WRITE:
            return engine->addInstrRule(InstrRuleBasic(
                Or({
                    DoesReadAccess(),
                    DoesWriteAccess(),
                }),
                getCallbackGenerator(cbk, data),
                InstPosition::POSTINST,
                true
            ));
        default:
            return VMError::INVALID_EVENTID;
    }
}


uint32_t VM::addMemAddrCB(rword address, MemoryAccessType type, InstCallback cbk, void *data) {
    RequireAction("VM::addMemAddrCB", cbk != nullptr, return VMError::INVALID_EVENTID);
    return addMemRangeCB(address, address + 1, type, cbk, data);
}

uint32_t VM::addMemRangeCB(rword start, rword end, MemoryAccessType type, InstCallback cbk, void *data) {
    RequireAction("VM::addMemRangeCB", start < end, return VMError::INVALID_EVENTID);
    RequireAction("VM::addMemRangeCB", type & MEMORY_READ_WRITE, return VMError::INVALID_EVENTID);
    RequireAction("VM::addMemRangeCB", cbk != nullptr, return VMError::INVALID_EVENTID);
    if((type == MEMORY_READ) && memReadGateCBID == VMError::INVALID_EVENTID) {
        memReadGateCBID = addMemAccessCB(MEMORY_READ, memReadGate, memCBInfos);
    }
    if((type & MEMORY_WRITE) && memWriteGateCBID == VMError::INVALID_EVENTID) {
        memWriteGateCBID = addMemAccessCB(MEMORY_READ_WRITE, memWriteGate, memCBInfos);
    }
    uint32_t id = memCBID++;
    RequireAction("VM::addMemRangeCB", id < EVENTID_VIRTCB_MASK, return VMError::INVALID_EVENTID);
    memCBInfos->push_back(std::make_pair(id, MemCBInfo {type, Range<rword>(start, end), cbk, data}));
    return id | EVENTID_VIRTCB_MASK;
}

uint32_t VM::addVMEventCB(VMEvent mask, VMCallback cbk, void *data) {
    RequireAction("VM::addVMEventCB", mask != 0, return VMError::INVALID_EVENTID);
    RequireAction("VM::addVMEventCB", cbk != nullptr, return VMError::INVALID_EVENTID);
    return engine->addVMEventCB(mask, cbk, data);
}

bool VM::deleteInstrumentation(uint32_t id) {
    if(id & EVENTID_VIRTCB_MASK) {
        id &= ~EVENTID_VIRTCB_MASK;
        for(size_t i = 0; i < memCBInfos->size(); i++) {
            if((*memCBInfos)[i].first == id) {
                memCBInfos->erase(memCBInfos->begin() + i);
                return true;
            }
        }
        return false;
    }
    else {
        return engine->deleteInstrumentation(id);
    }
}

void VM::deleteAllInstrumentations() {
    engine->deleteAllInstrumentations();
    memReadGateCBID = VMError::INVALID_EVENTID;
    memWriteGateCBID = VMError::INVALID_EVENTID;
    memCBInfos->clear();
    memoryLoggingLevel = 0;
}

const InstAnalysis* VM::getInstAnalysis(AnalysisType type) {
    const ExecBlock* curExecBlock = engine->getCurExecBlock();
    RequireAction("VM::getInstAnalysis", curExecBlock != nullptr, return nullptr);
    uint16_t curInstID = curExecBlock->getCurrentInstID();
    const InstMetadata* instMetadata = curExecBlock->getInstMetadata(curInstID);
    return engine->analyzeInstMetadata(instMetadata, type);
}

bool VM::recordMemoryAccess(MemoryAccessType type) {
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
    if(type & MEMORY_READ && !(memoryLoggingLevel & MEMORY_READ)) {
        memoryLoggingLevel |= MEMORY_READ;
        engine->addInstrRule(InstrRuleBasic(
            DoesReadAccess(),
            {
                GetReadAddress(Temp(0)),
                WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                GetReadValue(Temp(0)),
                WriteTemp(Temp(0), Shadow(MEM_VALUE_TAG)),
            },
            PREINST,
            false
        ), true); // force the rule to be the first of PREINST
    }
    if(type & MEMORY_WRITE && !(memoryLoggingLevel & MEMORY_WRITE)) {
        memoryLoggingLevel |= MEMORY_WRITE;
        engine->addInstrRule(InstrRuleBasic(
            DoesWriteAccess(),
            {
                GetWriteAddress(Temp(0)),
                WriteTemp(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                GetWriteValue(Temp(0)),
                WriteTemp(Temp(0), Shadow(MEM_VALUE_TAG)),
            },
            POSTINST,
            false
        ), true); // force the rule to be the first of POSTINST
    }
    return true;
#else
    return false;
#endif
}

std::vector<MemoryAccess> VM::getInstMemoryAccess() const {
    const ExecBlock* curExecBlock = engine->getCurExecBlock();
    if(curExecBlock == nullptr) {
        return {};
    }
    uint16_t instID = curExecBlock->getCurrentInstID();
    std::vector<MemoryAccess> memAccess;
    std::vector<ShadowInfo> shadows = curExecBlock->queryShadowByInst(instID, ANY);
    LogDebug("VM::getInstMemoryAccess", "Got %zu shadows for Instruction %" PRIu16, shadows.size(), instID);

    size_t i = 0;
    while(i < shadows.size()) {
        auto access = MemoryAccess();

        if(shadows[i].tag == MEM_READ_ADDRESS_TAG) {
            access.type = MEMORY_READ;
            access.size = getReadSize(curExecBlock->getOriginalMCInst(instID));
        }
        else if(engine->isPreInst() == false && shadows[i].tag == MEM_WRITE_ADDRESS_TAG) {
            access.type = MEMORY_WRITE;
            access.size = getWriteSize(curExecBlock->getOriginalMCInst(instID));
        }
        else {
            i += 1;
            continue;
        }
        access.accessAddress = curExecBlock->getShadow(shadows[i].shadowID);
        access.instAddress = curExecBlock->getInstAddress(shadows[i].instID);
        i += 1;

        if(i >= shadows.size()) {
            LogError(
                "Engine::getInstMemoryAccess",
                "An address shadow is not followed by a shadow for instruction at address %" PRIx64,
                access.instAddress
            );
            continue;
        }

        if(shadows[i].tag == MEM_VALUE_TAG) {
            access.value = curExecBlock->getShadow(shadows[i].shadowID);
        }
        else {
            LogError(
                "Engine::getInstMemoryAccess",
                "An address shadow is not followed by a value shadow for instruction at address %" PRIx64,
                access.instAddress
            );
            continue;
        }

        // we found our access and its value, record access
        memAccess.push_back(access);
        i += 1;
    }
    return memAccess;
}

std::vector<MemoryAccess> VM::getBBMemoryAccess() const {
    const ExecBlock* curExecBlock = engine->getCurExecBlock();
    if(curExecBlock == nullptr) {
        return {};
    }
    uint16_t bbID = curExecBlock->getCurrentSeqID();
    uint16_t instID = curExecBlock->getCurrentInstID();
    std::vector<MemoryAccess> memAccess;
    std::vector<ShadowInfo> shadows = curExecBlock->queryShadowBySeq(bbID, ANY);
    LogDebug("VM::getBBMemoryAccess", "Got %zu shadows for Basic Block %" PRIu16 " stopping at Instruction %" PRIu16,
             shadows.size(), bbID, instID);

    size_t i = 0;
    while(i < shadows.size() && shadows[i].instID <= instID) {
        auto access = MemoryAccess();

        if(shadows[i].tag == MEM_READ_ADDRESS_TAG) {
            access.type = MEMORY_READ;
            access.size = getReadSize(curExecBlock->getOriginalMCInst(shadows[i].instID));
        }
        else if(engine->isPreInst() == false && shadows[i].tag == MEM_WRITE_ADDRESS_TAG) {
            access.type = MEMORY_WRITE;
            access.size = getWriteSize(curExecBlock->getOriginalMCInst(shadows[i].instID));
        }
        else {
            i += 1;
            continue;
        }
        access.instAddress = curExecBlock->getInstAddress(shadows[i].instID);
        access.accessAddress = curExecBlock->getShadow(shadows[i].shadowID);
        i += 1;

        if(i >= shadows.size() || curExecBlock->getInstAddress(shadows[i-1].instID) != curExecBlock->getInstAddress(shadows[i].instID)) {
            LogError(
                "VM::getBBMemoryAccess",
                "An address shadow is not followed by a shadow for instruction at address %" PRIx64,
                access.instAddress
            );
            continue;
        }

        if(shadows[i].tag == MEM_VALUE_TAG) {
            access.value = curExecBlock->getShadow(shadows[i].shadowID);
        }
        else {
            LogError(
                "Engine::getBBMemoryAccess",
                "An address shadow is not followed by a value shadow for instruction at address %" PRIx64,
                access.instAddress
            );
            continue;
        }

        // we found our access and its value, record access
        memAccess.push_back(access);
        i += 1;
    }
    return memAccess;
}


bool VM::precacheBasicBlock(rword pc) {
    return engine->precacheBasicBlock(pc);
}

void VM::clearAllCache() {
    engine->clearAllCache();
}

void VM::clearCache(rword start, rword end) {
    engine->clearCache(start, end);
}

} // QBDI::
