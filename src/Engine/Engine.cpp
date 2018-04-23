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
#include <bitset>

#include "Engine.h"
#include "Errors.h"

#include "llvm/ADT/Triple.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/SourceMgr.h"

#include "Platform.h"
#include "ExecBlock/ExecBlockManager.h"
#include "ExecBroker/ExecBroker.h"
#include "Patch/Types.h"
#include "Patch/Patch.h"
#include "Patch/PatchRule.h"
#include "Patch/InstrRules.h"
#include "Patch/InstInfo.h"
#include "Utility/Assembly.h"
#include "Utility/LogSys.h"


// Mask to identify VM events
#define EVENTID_VM_MASK  (1UL << 30)

namespace QBDI {

Engine::Engine(const std::string& cpu, const std::vector<std::string>& mattrs, VMInstanceRef vminstance)
    : vminstance(vminstance), instrRulesCounter(0), vmCallbacksCounter(0) {
    unsigned i = 0;

    initMemAccessInfo();

    // Allocate LLVM CPUs
    #if defined(QBDI_ARCH_ARM)
    llvmCPU[CPUMode::ARM]     = new LLVMCPU(cpu, "arm", mattrs);
    llvmCPU[CPUMode::Thumb]   = new LLVMCPU(cpu, "thumb", mattrs);
    #elif defined(QBDI_ARCH_X86_64)
    llvmCPU[CPUMode::X86_64] = new LLVMCPU(cpu, "", mattrs);
    #endif
    // Allocate corresponding Assembly
    for(i = 0; i < CPUMode::COUNT; i++) {
        assembly[i] = new Assembly(llvmCPU[i]);
    }
    // Allocate remaining managing classes
    blockManager = new ExecBlockManager(llvmCPU, assembly, vminstance);
    execBroker = new ExecBroker(assembly, vminstance);

    // Get default Patch rules for this architecture
    patchRules = getDefaultPatchRules();

    gprState = std::unique_ptr<GPRState>(new GPRState);
    fprState = std::unique_ptr<FPRState>(new FPRState);
    curGPRState = gprState.get();
    curFPRState = fprState.get();
    curCPUMode = (CPUMode) 0;

    initGPRState();
    initFPRState();

    curExecBlock = nullptr;
}

Engine::~Engine() {
    unsigned i = 0;

    for(i = 0; i < CPUMode::COUNT; i++) {
        delete assembly[i];
        delete llvmCPU[i];
    }
    delete blockManager;
    delete execBroker;
}

void Engine::initGPRState() {
    memset(gprState.get(), 0, sizeof(GPRState));
}

void Engine::initFPRState() {
    memset(fprState.get(), 0, sizeof(FPRState));

    #if defined(QBDI_ARCH_X86_64)
    fprState->rfcw = 0x37F;
    fprState->ftw = 0xFF;
    fprState->rsrv1 = 0xFF;
    fprState->mxcsr = 0x1F80;
    fprState->mxcsrmask = 0xFFFF;
    #endif
}

GPRState* Engine::getGPRState() const {
   return curGPRState;
}

FPRState* Engine::getFPRState() const {
   return curFPRState;
}

void Engine::setGPRState(GPRState* gprState) {
    RequireAction("Engine::setGPRState", gprState, return);
    *(this->curGPRState) = *gprState;
}

void Engine::setFPRState(FPRState* fprState) {
    RequireAction("Engine::setFPRState", fprState, return);
    *(this->curFPRState) = *fprState;
}

bool Engine::isPreInst() const {
    if(curExecBlock == nullptr) {
        return false;
    }
    uint16_t instID = curExecBlock->getCurrentInstID();
    // By internal convention, PREINST => PC == Current instruction address (not matter of architecture)
    return curExecBlock->getInstAddress(instID) == QBDI_GPR_GET(getGPRState(), REG_PC);
}

void Engine::addInstrumentedRange(rword start, rword end) {
    execBroker->addInstrumentedRange(Range<rword>(start, end));
}

bool Engine::addInstrumentedModule(const std::string& name) {
    return execBroker->addInstrumentedModule(name);
}

bool Engine::addInstrumentedModuleFromAddr(rword addr) {
    return execBroker->addInstrumentedModuleFromAddr(addr);
}

bool Engine::instrumentAllExecutableMaps() {
    return execBroker->instrumentAllExecutableMaps();
}

void Engine::removeInstrumentedRange(rword start, rword end) {
    execBroker->removeInstrumentedRange(Range<rword>(start, end));
}

bool Engine::removeInstrumentedModule(const std::string& name) {
    return execBroker->removeInstrumentedModule(name);
}

bool Engine::removeInstrumentedModuleFromAddr(rword addr) {
    return execBroker->removeInstrumentedModuleFromAddr(addr);
}

void Engine::removeAllInstrumentedRanges() {
    execBroker->removeAllInstrumentedRanges();
}

std::vector<Patch> Engine::patch(rword start) {
    std::vector<Patch> basicBlock;
    bool basicBlockEnd = false;
    const llvm::ArrayRef<uint8_t> code((uint8_t*) start, (size_t) -1);
    rword i = 0;
    LogDebug("Engine::patch", "Patching basic block at address 0x%" PRIRWORD, start);

    // Get Basic block
    while(basicBlockEnd == false) {
        llvm::MCInst                        inst;
        llvm::MCDisassembler::DecodeStatus  dstatus;
        rword                               address;
        Patch                               patch;
        uint64_t                            instSize = 0;

        // Aggregate a complete patch
        do {
            // Disassemble
            dstatus = assembly[curCPUMode]->getInstruction(inst, instSize, code.slice(i), i);
            address = start + i;
            RequireAction("Engine::patch", llvm::MCDisassembler::Success == dstatus, abort());
            LogCallback(LogPriority::DEBUG, "Engine::patch", [&] (FILE *log) -> void {
                uint8_t* ptr = (uint8_t*) address;
                std::string disass;
                llvm::raw_string_ostream disassOs(disass);
                assembly[curCPUMode]->printDisasm(inst, disassOs);
                disassOs.flush();
                fprintf(log, "Patching 0x%" PRIRWORD " ", address);
                for(uint32_t j = 0; j < instSize; j++) {
                    fprintf(log, " %02" PRIx8, ptr[j]);
                }
                fprintf(log, " %s", disass.c_str());
            });
            // Patch & merge
            for(uint32_t j = 0; j < patchRules.size(); j++) {
                if(patchRules[j]->canBeApplied(&inst, address, instSize, llvmCPU[curCPUMode])) {
                    LogDebug("Engine::patch", "Patch rule %" PRIu32 " applied", j);
                    if(patch.insts.size() == 0) {
                        patch = patchRules[j]->generate(&inst, address, instSize, curCPUMode, llvmCPU[curCPUMode]);
                    }
                    else {
                        LogDebug("Engine::patch", "Previous instruction merged");
                        patch = patchRules[j]->generate(&inst, address, instSize, curCPUMode, llvmCPU[curCPUMode], &patch);
                    }
                    break;
                }
            }
            i += instSize;
        } while(patch.metadata.merge);
        LogDebug("Engine::patch", "Patch of size %" PRIu32 " generated", patch.metadata.patchSize);

        if(patch.metadata.modifyPC) {
            LogDebug("Engine::patch", "Basic block starting at address 0x%" PRIRWORD " ended at address 0x%" PRIRWORD, start, address);
            basicBlockEnd = true;
        }

        basicBlock.push_back(patch);
    }

    return basicBlock;
}

void Engine::instrument(std::vector<Patch> &basicBlock) {
    LogDebug("Engine::instrument", "Instrumenting basic block [0x%" PRIRWORD ", 0x%" PRIRWORD "]",
             basicBlock.front().metadata.address, basicBlock.back().metadata.address);
    for(Patch& patch : basicBlock) {
        LogCallback(LogPriority::DEBUG, "Engine::instrument", [&] (FILE *log) -> void {
            std::string disass;
            llvm::raw_string_ostream disassOs(disass);
            assembly[curCPUMode]->printDisasm(patch.metadata.inst, disassOs);
            disassOs.flush();
            fprintf(log, "Instrumenting 0x%" PRIRWORD " %s", patch.metadata.address, disass.c_str());
        });
        // Instrument
        for(const auto& item: instrRules) {
            const std::shared_ptr<InstrRule>& rule = item.second;
            if(rule->canBeApplied(patch, llvmCPU[curCPUMode])) { // Push MCII
                rule->instrument(patch, llvmCPU[curCPUMode]);
                LogDebug("Engine::instrument", "Instrumentation rule %" PRIu32 " applied", item.first);
            }
        }
    }
}


void Engine::handleNewBasicBlock(rword pc) {
    // disassemble and patch new basic block
    Patch::Vec basicBlock = patch(pc);
    // instrument it
    instrument(basicBlock);
    // Write it in the cache
    blockManager->writeBasicBlock(basicBlock);
}


bool Engine::precacheBasicBlock(rword pc) {
    if(blockManager->getProgrammedExecBlock(pc) != nullptr) {
        // already in cache
        return false;
    }
    handleNewBasicBlock(pc);
    return true;
}


bool Engine::run(rword start, rword stop) {
    rword         currentPC = start;
    bool          hasRan = false;
    curGPRState = gprState.get();
    curFPRState = fprState.get();

    // Start address is out of range
    if(!execBroker->isInstrumented(start)) {
        return false;
    }

    // Execute basic block per basic block
    do {
        // If this PC is not instrumented try to transfer execution
        if(execBroker->isInstrumented(currentPC) == false &&
           execBroker->canTransferExecution(curGPRState)) {
            curExecBlock = nullptr;
            LogDebug("Engine::run", "Executing 0x%" PRIRWORD " through execBroker", currentPC);
            // transfer execution
            signalEvent(EXEC_TRANSFER_CALL, currentPC, curGPRState, curFPRState);
            execBroker->transferExecution(currentPC, curGPRState, curFPRState);
            signalEvent(EXEC_TRANSFER_RETURN, currentPC, curGPRState, curFPRState);
        }
        // Else execute through DBI
        else {
            LogDebug("Engine::run", "Executing 0x%" PRIRWORD " through DBI", currentPC);
            VMEvent event = VMEvent::SEQUENCE_ENTRY;

            // Is cache flush pending?
            if(blockManager->isFlushPending()) {
                // Backup fprState and gprState
                *gprState = *curGPRState;
                *fprState = *curFPRState;
                curGPRState = gprState.get();
                curFPRState = fprState.get();
                // Commit the flush
                blockManager->flushCommit();
            }

            // Test if we have it in cache
            curExecBlock = blockManager->getProgrammedExecBlock(currentPC);
            if(curExecBlock == nullptr) {
                LogDebug("Engine::run", "Cache miss for 0x%" PRIRWORD ", patching & instrumenting new basic block", currentPC);
                handleNewBasicBlock(currentPC);
                // Signal a new basic block
                event |= BASIC_BLOCK_NEW;
                // Set new basic block as current
                curExecBlock = blockManager->getProgrammedExecBlock(currentPC);
            }

            // Set context if necessary
            if(&(curExecBlock->getContext()->gprState) != curGPRState || &(curExecBlock->getContext()->fprState) != curFPRState) {
                curExecBlock->getContext()->gprState = *curGPRState;
                curExecBlock->getContext()->fprState = *curFPRState;
            }
            curGPRState = &(curExecBlock->getContext()->gprState);
            curFPRState = &(curExecBlock->getContext()->fprState);

            // Signal events
            if ((curExecBlock->getSeqType(curExecBlock->getCurrentSeqID()) & SeqType::Entry) > 0) {
                event |= BASIC_BLOCK_ENTRY;
            }
            signalEvent(event, currentPC, curGPRState, curFPRState);

            // Execute
            hasRan = true;
            switch(curExecBlock->execute()) {
                case CONTINUE:
                case BREAK_TO_VM:
                    break;
                case STOP:
                    *gprState = *curGPRState;
                    *fprState = *curFPRState;
                    curGPRState = gprState.get();
                    curFPRState = fprState.get();
                    return hasRan;
            }

            // Signal events
            event = SEQUENCE_EXIT;
            if ((curExecBlock->getSeqType(curExecBlock->getCurrentSeqID()) & SeqType::Exit) > 0) {
                event |= BASIC_BLOCK_EXIT;
            }
            signalEvent(event, currentPC, curGPRState, curFPRState);
        }
        // Get next block PC
        currentPC = QBDI_GPR_GET(curGPRState, REG_PC);
        LogDebug("Engine::run", "Next address to execute is 0x%" PRIRWORD, currentPC);
    } while(currentPC != stop);

    // Copy final context
    *gprState = *curGPRState;
    *fprState = *curFPRState;
    curGPRState = gprState.get();
    curFPRState = fprState.get();

    return hasRan;
}

uint32_t Engine::addInstrRule(InstrRule rule) {
    uint32_t id = instrRulesCounter++;
    RequireAction("Engine::addInstrRule", id < EVENTID_VM_MASK, return VMError::INVALID_EVENTID);
    blockManager->clearCache(rule.affectedRange());
    switch(rule.getPosition()) {
        case InstPosition::PREINST:
            instrRules.insert(instrRules.begin(), std::make_pair(id, (InstrRule::SharedPtr) rule));
            break;
        case InstPosition::POSTINST:
            instrRules.push_back(std::make_pair(id, (InstrRule::SharedPtr) rule));
            break;
    }
    return id;
}

uint32_t Engine::addVMEventCB(VMEvent mask, VMCallback cbk, void *data) {
    uint32_t id = vmCallbacksCounter++;
    RequireAction("Engine::addVMEventCB", id < EVENTID_VM_MASK, return VMError::INVALID_EVENTID);
    vmCallbacks.push_back(std::make_pair(id, CallbackRegistration {mask, cbk, data}));
    return id | EVENTID_VM_MASK;
}

void Engine::signalEvent(VMEvent event, rword currentPC, GPRState *gprState, FPRState *fprState) {
    static VMState vmState;
    static rword lastUpdatePC = 0;

    for(const auto& item : vmCallbacks) {
        const QBDI::CallbackRegistration& r = item.second;
        if(event & r.mask) {
            if(lastUpdatePC != currentPC) {
                lastUpdatePC = currentPC;
                if(curExecBlock != nullptr) {
                    const SeqLoc* seqLoc    = blockManager->getSeqLoc(currentPC);
                    vmState.basicBlockStart = seqLoc->bbStart;
                    vmState.basicBlockEnd   = seqLoc->bbEnd;
                    vmState.sequenceStart   = seqLoc->seqStart;
                    vmState.sequenceEnd     = seqLoc->seqEnd;
                }
                else {
                    vmState = VMState {event, currentPC, currentPC, currentPC, currentPC, 0};
                }
            }
            vmState.event = event;
            r.cbk(vminstance, &vmState, gprState, fprState, r.data);
        }
    }
}

bool Engine::deleteInstrumentation(uint32_t id) {
    if (id & EVENTID_VM_MASK) {
        id &= ~EVENTID_VM_MASK;
        for(size_t i = 0; i < vmCallbacks.size(); i++) {
            if(vmCallbacks[i].first == id) {
                vmCallbacks.erase(vmCallbacks.begin() + i);
                return true;
            }
        }
    }
    else {
        for(size_t i = 0; i < instrRules.size(); i++) {
            if(instrRules[i].first == id) {
                blockManager->clearCache(instrRules[i].second->affectedRange());
                instrRules.erase(instrRules.begin() + i);
                return true;
            }
        }
    }
    return false;
}

void Engine::deleteAllInstrumentations() {
    instrRules.clear();
    vmCallbacks.clear();
}

const InstAnalysis* Engine::analyzeInstMetadata(const InstMetadata* instMetadata, AnalysisType type) {
    return blockManager->analyzeInstMetadata(instMetadata, type);
}

void Engine::clearAllCache() {
    blockManager->clearCache();
}

void Engine::clearCache(rword start, rword end) {
    blockManager->clearCache(Range<rword>(start, end));
}

} // QBDI::
