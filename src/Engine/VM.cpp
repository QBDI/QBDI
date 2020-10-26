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

// Use llvm::ArrayRef
#include "llvm/ADT/APInt.h"

#include "VM.h"
#include "Range.h"
#include "Errors.h"
#include "Memory.hpp"

#include "Engine/Engine.h"
#include "ExecBlock/ExecBlock.h"
#include "Patch/InstInfo.h"
#include "Patch/InstrRule.h"
#include "Patch/InstrRules.h"
#include "Patch/PatchCondition.h"
#include "Patch/PatchGenerator.h"
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

static VMAction memReadGate(VMInstanceRef vm, GPRState* gprState, FPRState* fprState, void* data) {
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

static VMAction memWriteGate(VMInstanceRef vm, GPRState* gprState, FPRState* fprState, void* data) {
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

static VMAction stopCallback(VMInstanceRef vm, GPRState* gprState, FPRState* fprState, void* data) {
    return VMAction::STOP;
}

VM::VM(const std::string& cpu, const std::vector<std::string>& mattrs) :
    memoryLoggingLevel(0), memCBID(0), memReadGateCBID(VMError::INVALID_EVENTID), memWriteGateCBID(VMError::INVALID_EVENTID) {
    engine = std::make_unique<Engine>(cpu, mattrs, this);
    memCBInfos = std::make_unique<std::vector<std::pair<uint32_t, MemCBInfo>>>();
}

VM::~VM() = default;

VM::VM(VM&& vm) :
        engine(std::move(vm.engine)),
        memoryLoggingLevel(vm.memoryLoggingLevel),
        memCBInfos(std::move(vm.memCBInfos)),
        memCBID(vm.memCBID),
        memReadGateCBID(vm.memReadGateCBID),
        memWriteGateCBID(vm.memWriteGateCBID) {

    engine->changeVMInstanceRef(this);
}

VM& VM::operator=(VM&& vm) {
    engine = std::move(vm.engine);
    memoryLoggingLevel = vm.memoryLoggingLevel;
    memCBInfos = std::move(vm.memCBInfos);
    memCBID = vm.memCBID;
    memReadGateCBID = vm.memReadGateCBID;
    memWriteGateCBID = vm.memWriteGateCBID;

    engine->changeVMInstanceRef(this);

    return *this;
}

VM::VM(const VM& vm) :
        engine(std::make_unique<Engine>(*vm.engine)),
        memoryLoggingLevel(vm.memoryLoggingLevel),
        memCBInfos(std::make_unique<std::vector<std::pair<uint32_t, MemCBInfo>>>(*vm.memCBInfos)),
        memCBID(vm.memCBID),
        memReadGateCBID(vm.memReadGateCBID),
        memWriteGateCBID(vm.memWriteGateCBID) {

    engine->changeVMInstanceRef(this);
}


VM& VM::operator=(const VM& vm) {
    *engine = *vm.engine;
    *memCBInfos = *vm.memCBInfos;

    memoryLoggingLevel = vm.memoryLoggingLevel;
    memCBID = vm.memCBID;
    memReadGateCBID = vm.memReadGateCBID;
    memWriteGateCBID = vm.memWriteGateCBID;

    engine->changeVMInstanceRef(this);

    return *this;
}

GPRState* VM::getGPRState() const {
    return engine->getGPRState();
}

FPRState* VM::getFPRState() const {
    return engine->getFPRState();
}


void VM::setGPRState(const GPRState* gprState) {
    RequireAction("VM::setGPRState", gprState != nullptr, return);
    engine->setGPRState(gprState);
}

void VM::setFPRState(const FPRState* fprState) {
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
  std::vector<rword> args (argNum);
    for(uint32_t i = 0; i < argNum; i++) {
        args[i] = va_arg(ap, rword);
    }

    bool res = this->callA(retval, function, argNum, args.data());

    return res;
}

uint32_t VM::addInstrRule(InstrumentCallback cbk, AnalysisType type, void* data) {
    RangeSet<rword> r;
    r.add(Range<rword>(0, (rword) -1));
    return engine->addInstrRule(InstrRuleUser(cbk, type, data, this, r, MIDDLEPASS));
}

uint32_t VM::addInstrRuleRange(rword start, rword end, InstrumentCallback cbk, AnalysisType type, void* data) {
    RangeSet<rword> r;
    r.add(Range<rword>(start, end));
    return engine->addInstrRule(InstrRuleUser(cbk, type, data, this, r, MIDDLEPASS));
}

uint32_t VM::addInstrRuleRangeSet(RangeSet<rword> range, InstrumentCallback cbk, AnalysisType type, void* data) {
    return engine->addInstrRule(InstrRuleUser(cbk, type, data, this, range, MIDDLEPASS));
}

uint32_t VM::addMnemonicCB(const char* mnemonic, InstPosition pos, InstCallback cbk, void *data) {
    RequireAction("VM::addMnemonicCB", mnemonic != nullptr, return VMError::INVALID_EVENTID);
    RequireAction("VM::addMnemonicCB", cbk != nullptr, return VMError::INVALID_EVENTID);
    return engine->addInstrRule(InstrRuleBasic(
        MnemonicIs(mnemonic),
        getCallbackGenerator(cbk, data),
        pos,
        true,
        MIDDLEPASS
    ));
}

uint32_t VM::addCodeCB(InstPosition pos, InstCallback cbk, void *data) {
    RequireAction("VM::addCodeCB", cbk != nullptr, return VMError::INVALID_EVENTID);
    return engine->addInstrRule(InstrRuleBasic(
        True(),
        getCallbackGenerator(cbk, data),
        pos,
        true,
        MIDDLEPASS
    ));
}

uint32_t VM::addCodeAddrCB(rword address, InstPosition pos, InstCallback cbk, void *data) {
    RequireAction("VM::addCodeAddrCB", cbk != nullptr, return VMError::INVALID_EVENTID);
    return engine->addInstrRule(InstrRuleBasic(
        AddressIs(address),
        getCallbackGenerator(cbk, data),
        pos,
        true,
        MIDDLEPASS
    ));
}

uint32_t VM::addCodeRangeCB(rword start, rword end, InstPosition pos, InstCallback cbk, void *data) {
    RequireAction("VM::addCodeRangeCB", start < end, return VMError::INVALID_EVENTID);
    RequireAction("VM::addCodeRangeCB", cbk != nullptr, return VMError::INVALID_EVENTID);
    return engine->addInstrRule(InstrRuleBasic(
        InstructionInRange(start, end),
        getCallbackGenerator(cbk, data),
        pos,
        true,
        MIDDLEPASS
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
                true,
                MIDDLEPASS
            ));
        case MEMORY_WRITE:
            return engine->addInstrRule(InstrRuleBasic(
                DoesWriteAccess(),
                getCallbackGenerator(cbk, data),
                InstPosition::POSTINST,
                true,
                MIDDLEPASS
            ));
        case MEMORY_READ_WRITE:
            return engine->addInstrRule(InstrRuleBasic(
                Or({
                    DoesReadAccess(),
                    DoesWriteAccess(),
                }),
                getCallbackGenerator(cbk, data),
                InstPosition::POSTINST,
                true,
                MIDDLEPASS
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
        memReadGateCBID = addMemAccessCB(MEMORY_READ, memReadGate, memCBInfos.get());
    }
    if((type & MEMORY_WRITE) && memWriteGateCBID == VMError::INVALID_EVENTID) {
        memWriteGateCBID = addMemAccessCB(MEMORY_READ_WRITE, memWriteGate, memCBInfos.get());
    }
    uint32_t id = memCBID++;
    RequireAction("VM::addMemRangeCB", id < EVENTID_VIRTCB_MASK, return VMError::INVALID_EVENTID);
    memCBInfos->emplace_back(id, MemCBInfo {type, {start, end}, cbk, data});
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
    if constexpr(not (is_x86_64 or is_x86))
        return false;

    if(type & MEMORY_READ && !(memoryLoggingLevel & MEMORY_READ)) {
        memoryLoggingLevel |= MEMORY_READ;
        engine->addInstrRule(getMemReadPreInstrRule());
        engine->addInstrRule(getMemReadPostInstrRule());
    }
    if(type & MEMORY_WRITE && !(memoryLoggingLevel & MEMORY_WRITE)) {
        memoryLoggingLevel |= MEMORY_WRITE;
        engine->addInstrRule(getMemWritePreInstrRule());
        engine->addInstrRule(getMemWritePostInstrRule());
    }
    return true;
}

/* decode a standard MemAccess for a execBlock
 *
 * @param[in]  curExecBlock     the current basicBlock
 * @param[out] access           the access to complete
 * @param[in]  shadows          iterator on the shadows (the first item
 *
 * @return true  when success
 */
static bool decodeDefaultMemAccess(const ExecBlock& curExecBlock, MemoryAccess& access,
                                   llvm::ArrayRef<ShadowInfo> shadows) {

    const uint16_t instID = shadows[0].instID;
    access = MemoryAccess();
    access.flags = MEMORY_NO_FLAGS;

    // look at two shadows
    if (shadows.size() < 1)
        return false;

    // the first one is MEM_READ_ADDRESS_TAG or MEM_WRITE_ADDRESS_TAG
    if (shadows[0].tag == MEM_READ_ADDRESS_TAG) {
        access.type = MEMORY_READ;
        access.size = getReadSize(curExecBlock.getOriginalMCInst(instID));
    } else if (shadows[0].tag == MEM_WRITE_ADDRESS_TAG) {
        access.type = MEMORY_WRITE;
        access.size = getWriteSize(curExecBlock.getOriginalMCInst(instID));
    } else
        return false;

    access.accessAddress = curExecBlock.getShadow(shadows[0].shadowID);
    access.instAddress = curExecBlock.getInstAddress(instID);

    // if the access size is bigger than rword, the value cannot be store in a shadow.
    // Don't seach for a MEM_VALUE_TAG with MEMORY_UNKNOWN_VALUE
    if (access.size > sizeof(rword)) {
        access.flags |= MEMORY_UNKNOWN_VALUE;
        return true;
    }

    // need a second shadow to take the value
    if (shadows.size() < 2)
        return false;

    // the second shadow reference the same instruction
    if (access.instAddress != curExecBlock.getInstAddress(shadows[1].instID)) {
        LogError(
            "decodeDefaultMemAccess",
            "An address shadow is not followed by a shadow for the same instruction for instruction at address %" PRIx64,
            access.instAddress
        );
        return false;
    }

    // the second shadow must be a MEM_VALUE_TAG
    if(shadows[1].tag == MEM_VALUE_TAG) {
        access.value = curExecBlock.getShadow(shadows[1].shadowID);
        return true;
    } else {
        LogError(
            "decodeDefaultMemAccess",
            "An address shadow is not followed by a value shadow for instruction at address %" PRIx64,
            access.instAddress
        );
        return false;
    }
}

/* decode a complexe MemAccess for a execBlock
 *
 * @param[in]  curExecBlock     the current basicBlock
 * @param[out] access           the access to complete
 * @param[in]  shadows          iterator on the shadows (the first item
 * @param[in]  searchPostTag    find the stop tag (if in postInst)
 *
 * @return true  when success
 */
static bool decodeComplexMemAccess(const ExecBlock& curExecBlock, MemoryAccess& access,
                                   llvm::ArrayRef<ShadowInfo> shadows, bool searchPostTag) {

    const uint16_t instID = shadows[0].instID;
    access = MemoryAccess();
    access.flags = MEMORY_UNKNOWN_VALUE;
    access.value = 0;
    uint16_t expectedEndTag;

    // look at two shadows minimum
    if (shadows.size() < 2)
        return false;

    // the first one is MEM_READ_START_ADDRESS_1_TAG, MEM_READ_START_ADDRESS_2_TAG
    // or MEM_WRITE_START_ADDRESS_TAG
    switch (shadows[0].tag) {
        case MEM_READ_START_ADDRESS_1_TAG:
            expectedEndTag = MEM_READ_STOP_ADDRESS_1_TAG;
            access.type = MEMORY_READ;
            access.size = getReadSize(curExecBlock.getOriginalMCInst(instID));
            break;
        case MEM_READ_START_ADDRESS_2_TAG:
            expectedEndTag = MEM_READ_STOP_ADDRESS_2_TAG;
            access.type = MEMORY_READ;
            access.size = getReadSize(curExecBlock.getOriginalMCInst(instID));
            break;
        case MEM_WRITE_START_ADDRESS_TAG:
            expectedEndTag = MEM_WRITE_STOP_ADDRESS_TAG;
            access.type = MEMORY_WRITE;
            access.size = getWriteSize(curExecBlock.getOriginalMCInst(instID));
            break;
        default:
            return false;
    }

    // accessAddress is the start address
    access.accessAddress = curExecBlock.getShadow(shadows[0].shadowID);
    access.instAddress = curExecBlock.getInstAddress(instID);

    if (not searchPostTag) {
        access.flags |= MEMORY_UNKNOWN_SIZE;
        return true;
    }

    size_t foundStopIndex = 0;
    for (size_t i = 1; i < shadows.size() && access.instAddress == curExecBlock.getInstAddress(shadows[i].instID); i++) {
        if (shadows[i].tag == expectedEndTag) {
            foundStopIndex = i;
            break;
        }
    }

    if (foundStopIndex == 0) {
        LogError(
            "decodeComplexMemAccess",
            "Doesn't found shadow %x after shadow %x for instruction at address %" PRIx64,
            shadows[0].tag, expectedEndTag, access.instAddress
        );
        return false;
    }

    // the stop address is load by the access
    rword stopAddress = curExecBlock.getShadow(shadows[foundStopIndex].shadowID);

    if (stopAddress >= access.accessAddress) {
        access.size = stopAddress - access.accessAddress;
    } else {
        // the stopAddress is lesser than the begin address, this may be the case
        // in X86 with REP prefix and DF=1
        // In this case, the memory have been access between [stopAddress + accessSize, access.accessAddress + accessSize)
        rword realSize = access.accessAddress - stopAddress;
        access.accessAddress = stopAddress + access.size;
        access.size = realSize;
    }
    return true;
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

    auto access = MemoryAccess();
    for (size_t i = 0; i < shadows.size(); i++) {

        bool addAccess = false;
        switch (shadows[i].tag) {
            default:
                break;
            case MEM_WRITE_ADDRESS_TAG:
                if (engine->isPreInst())
                    break;
                addAccess = decodeDefaultMemAccess(*curExecBlock, access, llvm::ArrayRef<ShadowInfo>(shadows.data() + i, shadows.size() - i));
                break;
            case MEM_READ_ADDRESS_TAG:
                addAccess = decodeDefaultMemAccess(*curExecBlock, access, llvm::ArrayRef<ShadowInfo>(shadows.data() + i, shadows.size() - i));
                break;
            case MEM_READ_START_ADDRESS_1_TAG:
            case MEM_READ_START_ADDRESS_2_TAG:
            case MEM_WRITE_START_ADDRESS_TAG:
                addAccess = decodeComplexMemAccess(*curExecBlock, access,
                    llvm::ArrayRef<ShadowInfo>(shadows.data() + i, shadows.size() - i),
                    not engine->isPreInst());
                break;
        }

        if (addAccess)
            memAccess.push_back(access);
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

    auto access = MemoryAccess();
    for (size_t i = 0; i < shadows.size() && shadows[i].instID <= instID; i++) {

        bool addAccess = false;
        switch (shadows[i].tag) {
            default:
                break;
            case MEM_WRITE_ADDRESS_TAG:
                if (engine->isPreInst() && shadows[i].instID == instID)
                    break;
                addAccess = decodeDefaultMemAccess(*curExecBlock, access, llvm::ArrayRef<ShadowInfo>(shadows.data() + i, shadows.size() - i));
                break;
            case MEM_READ_ADDRESS_TAG:
                addAccess = decodeDefaultMemAccess(*curExecBlock, access, llvm::ArrayRef<ShadowInfo>(shadows.data() + i, shadows.size() - i));
                break;
            case MEM_READ_START_ADDRESS_1_TAG:
            case MEM_READ_START_ADDRESS_2_TAG:
            case MEM_WRITE_START_ADDRESS_TAG:
                addAccess = decodeComplexMemAccess(*curExecBlock, access,
                    llvm::ArrayRef<ShadowInfo>(shadows.data() + i, shadows.size() - i),
                    not (engine->isPreInst() && shadows[i].instID == instID));
                break;
        }

        if (addAccess)
            memAccess.push_back(access);
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
