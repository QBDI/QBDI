/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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
#include <algorithm>
#include <cstdint>
#include <string.h>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInst.h"

#include "Engine/Engine.h"
#include "Engine/LLVMCPU.h"

#include "ExecBlock/Context.h"
#include "ExecBlock/ExecBlock.h"
#include "ExecBlock/ExecBlockManager.h"
#include "ExecBroker/ExecBroker.h"
#include "Patch/InstMetadata.h"
#include "Patch/InstrRule.h"
#include "Patch/Patch.h"
#include "Patch/PatchRule.h"
#include "Patch/PatchRules.h"
#include "Utility/LogSys.h"

#include "QBDI/Bitmask.h"
#include "QBDI/Config.h"
#include "QBDI/Errors.h"
#include "QBDI/Range.h"
#include "QBDI/State.h"

#include "spdlog/fmt/bin_to_hex.h"

// Mask to identify VM events
#define EVENTID_VM_MASK (1UL << 30)

namespace QBDI {

Engine::Engine(const std::string &_cpu, const std::vector<std::string> &_mattrs,
               Options opts, VMInstanceRef vminstance)
    : vminstance(vminstance), instrRulesCounter(0), vmCallbacksCounter(0),
      curCPUMode(CPUMode::DEFAULT), options(opts), eventMask(VMEvent::NO_EVENT),
      running(false) {

  llvmCPUs = std::make_unique<LLVMCPUs>(_cpu, _mattrs, opts);
  blockManager = std::make_unique<ExecBlockManager>(*llvmCPUs, vminstance);
  execBroker = blockManager->getExecBroker();

  // Get default Patch rules for this architecture
  patchRules = getDefaultPatchRules(options);

  gprState = std::make_unique<GPRState>();
  fprState = std::make_unique<FPRState>();
  curGPRState = gprState.get();
  curFPRState = fprState.get();

  initGPRState();
  initFPRState();

  curExecBlock = nullptr;
}

Engine::~Engine() = default;

Engine::Engine(const Engine &other)
    : vminstance(nullptr), instrRules(),
      instrRulesCounter(other.instrRulesCounter),
      vmCallbacks(other.vmCallbacks),
      vmCallbacksCounter(other.vmCallbacksCounter),
      curCPUMode(CPUMode::DEFAULT), options(other.options),
      eventMask(other.eventMask), running(false) {

  llvmCPUs = std::make_unique<LLVMCPUs>(
      other.llvmCPUs->getCPU(), other.llvmCPUs->getMattrs(), other.options);
  blockManager = std::make_unique<ExecBlockManager>(*llvmCPUs, nullptr);
  execBroker = blockManager->getExecBroker();
  // copy instrumentation range
  execBroker->setInstrumentedRange(other.execBroker->getInstrumentedRange());

  // Get default Patch rules for this architecture
  patchRules = getDefaultPatchRules(options);

  // Copy unique_ptr of instrRules
  for (const auto &r : other.instrRules) {
    instrRules.emplace_back(r.first, r.second->clone());
  }

  gprState = std::make_unique<GPRState>();
  fprState = std::make_unique<FPRState>();
  curGPRState = gprState.get();
  curFPRState = fprState.get();
  setGPRState(other.getGPRState());
  setFPRState(other.getFPRState());

  curExecBlock = nullptr;
}

Engine &Engine::operator=(const Engine &other) {
  QBDI_REQUIRE_ACTION(not running && "Cannot assign a running Engine", abort());
  this->clearAllCache();

  if (not llvmCPUs->isSameCPU(*other.llvmCPUs)) {
    blockManager.reset();

    llvmCPUs = std::make_unique<LLVMCPUs>(
        other.llvmCPUs->getCPU(), other.llvmCPUs->getMattrs(), other.options);

    blockManager = std::make_unique<ExecBlockManager>(*llvmCPUs, nullptr);
    execBroker = blockManager->getExecBroker();
  }

  this->setOptions(other.options);

  // copy the configuration
  instrRules.clear();
  for (const auto &r : other.instrRules) {
    instrRules.emplace_back(r.first, r.second->clone());
  }
  vmCallbacks = other.vmCallbacks;
  instrRulesCounter = other.instrRulesCounter;
  vmCallbacksCounter = other.vmCallbacksCounter;
  eventMask = other.eventMask;

  // copy instrumentation range
  execBroker->setInstrumentedRange(other.execBroker->getInstrumentedRange());

  // copy state
  setGPRState(other.getGPRState());
  setFPRState(other.getFPRState());

  return *this;
}

void Engine::setOptions(Options options) {
  QBDI_REQUIRE_ACTION(not running && "Cannot setOptions on a running Engine",
                      abort());
  if (options != this->options) {
    QBDI_DEBUG("Change Options from {:x} to {:x}", this->options, options);
    clearAllCache();
    llvmCPUs->setOptions(options);

    Options needRecreate =
        Options::OPT_DISABLE_FPR | Options::OPT_DISABLE_OPTIONAL_FPR;

    // need to recreate all ExecBlock
    if (((this->options ^ options) & needRecreate) != 0) {
      const RangeSet<rword> instrumentationRange =
          execBroker->getInstrumentedRange();

      patchRules = getDefaultPatchRules(options);
      blockManager = std::make_unique<ExecBlockManager>(*llvmCPUs, vminstance);
      execBroker = blockManager->getExecBroker();

      execBroker->setInstrumentedRange(instrumentationRange);
    }
    this->options = options;
  }
}

void Engine::changeVMInstanceRef(VMInstanceRef vminstance) {
  QBDI_REQUIRE_ACTION(
      not running && "Cannot changeVMInstanceRef on a running Engine", abort());
  this->vminstance = vminstance;

  blockManager->changeVMInstanceRef(vminstance);

  for (auto &r : instrRules) {
    r.second->changeVMInstanceRef(vminstance);
  }
}

void Engine::initGPRState() { memset(gprState.get(), 0, sizeof(GPRState)); }

void Engine::initFPRState() {
  memset(fprState.get(), 0, sizeof(FPRState));

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
  fprState->rfcw = 0x37F;
  fprState->ftw = 0x0;
  fprState->rsrv1 = 0x0;
  fprState->mxcsr = 0x1F80;
  fprState->mxcsrmask = 0xFFFF;
#endif
}

GPRState *Engine::getGPRState() const { return curGPRState; }

FPRState *Engine::getFPRState() const { return curFPRState; }

void Engine::setGPRState(const GPRState *gprState) {
  QBDI_REQUIRE_ACTION(gprState, return );
  *(this->curGPRState) = *gprState;
}

void Engine::setFPRState(const FPRState *fprState) {
  QBDI_REQUIRE_ACTION(fprState, return );
  *(this->curFPRState) = *fprState;
}

bool Engine::isPreInst() const {
  if (curExecBlock == nullptr) {
    return false;
  }
  uint16_t instID = curExecBlock->getCurrentInstID();
  // By internal convention, PREINST => PC == Current instruction address
  // (not matter of architecture)
  return curExecBlock->getInstAddress(instID) ==
         QBDI_GPR_GET(getGPRState(), REG_PC);
}

void Engine::addInstrumentedRange(rword start, rword end) {
  execBroker->addInstrumentedRange(Range<rword>(start, end));
}

bool Engine::addInstrumentedModule(const std::string &name) {
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

bool Engine::removeInstrumentedModule(const std::string &name) {
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
  const LLVMCPU &llvmcpu = llvmCPUs->getCPU(curCPUMode);
  const llvm::ArrayRef<uint8_t> code((uint8_t *)start, (size_t)-1);
  bool basicBlockEnd = false;
  rword i = 0;
  QBDI_DEBUG("Patching basic block at address 0x{:x}", start);

  // Get Basic block
  while (not basicBlockEnd) {
    llvm::MCInst inst;
    llvm::MCDisassembler::DecodeStatus dstatus;
    rword address;
    Patch patch;
    uint64_t instSize = 0;

    // Aggregate a complete patch
    do {
      // Disassemble
      address = start + i;
      dstatus = llvmcpu.getInstruction(inst, instSize, code.slice(i), address);
      if (llvm::MCDisassembler::Success != dstatus) {
        QBDI_CRITICAL(
            "Disassembly error : fail to parse address 0x{:x} ({:n})", address,
            spdlog::to_hex(reinterpret_cast<uint8_t *>(address),
                           reinterpret_cast<uint8_t *>(address + 16)));
        abort();
      }
      QBDI_REQUIRE_ACTION(llvm::MCDisassembler::Success == dstatus, abort());
      QBDI_DEBUG_BLOCK({
        std::string disass = llvmcpu.showInst(inst, address);
        QBDI_DEBUG("Patching 0x{:x} {}", address, disass.c_str());
      });
      // Patch & merge
      for (uint32_t j = 0; j < patchRules.size(); j++) {
        if (patchRules[j].canBeApplied(inst, address, instSize, llvmcpu)) {
          QBDI_DEBUG("Patch rule {} applied", j);
          if (patch.insts.size() == 0) {
            patch = patchRules[j].generate(inst, address, instSize, llvmcpu);
          } else {
            QBDI_DEBUG("Previous instruction merged");
            patch = patchRules[j].generate(inst, address, instSize, llvmcpu,
                                           &patch);
          }
          break;
        }
      }
      i += instSize;
    } while (patch.metadata.merge);
    QBDI_DEBUG("Patch of size {:x} generated", patch.metadata.patchSize);

    if (patch.metadata.modifyPC) {
      QBDI_DEBUG(
          "Basic block starting at address 0x{:x} ended at address 0x{:x}",
          start, address);
      basicBlockEnd = true;
    }

    basicBlock.push_back(std::move(patch));
  }

  return basicBlock;
}

void Engine::instrument(std::vector<Patch> &basicBlock, size_t patchEnd) {
  const LLVMCPU &llvmcpu = llvmCPUs->getCPU(curCPUMode);
  QBDI_DEBUG(
      "Instrumenting sequence [0x{:x}, 0x{:x}] in basic block [0x{:x}, 0x{:x}]",
      basicBlock.front().metadata.address,
      basicBlock[patchEnd - 1].metadata.address,
      basicBlock.front().metadata.address, basicBlock.back().metadata.address);

  for (size_t i = 0; i < patchEnd; i++) {
    Patch &patch = basicBlock[i];
    QBDI_DEBUG_BLOCK({
      std::string disass =
          llvmcpu.showInst(patch.metadata.inst, patch.metadata.address);
      QBDI_DEBUG("Instrumenting 0x{:x} {}", patch.metadata.address,
                 disass.c_str());
    });
    // Instrument
    for (const auto &item : instrRules) {
      const InstrRule *rule = item.second.get();
      if (rule->tryInstrument(patch, llvmcpu)) {
        QBDI_DEBUG("Instrumentation rule {:x} applied", item.first);
      }
    }
    patch.finalizeInstsPatch();
  }
}

void Engine::handleNewBasicBlock(rword pc) {
  // disassemble and patch new basic block
  Patch::Vec basicBlock = patch(pc);
  // Reserve cache and get uncached instruction
  size_t patchEnd = blockManager->preWriteBasicBlock(basicBlock);
  // instrument uncached instruction
  instrument(basicBlock, patchEnd);
  // Write in the cache
  blockManager->writeBasicBlock(basicBlock, patchEnd);
}

bool Engine::precacheBasicBlock(rword pc) {
  QBDI_REQUIRE_ACTION(
      not running && "Cannot precacheBasicBlock on a running Engine", abort());
  if (blockManager->isFlushPending()) {
    // Commit the flush
    blockManager->flushCommit();
  }
  if (blockManager->getExecBlock(pc) != nullptr) {
    // already in cache
    return false;
  }
  running = true;
  handleNewBasicBlock(pc);
  running = false;
  return true;
}

bool Engine::run(rword start, rword stop) {
  QBDI_REQUIRE_ACTION(not running && "Cannot run an already running Engine",
                      abort());

  rword currentPC = start;
  bool hasRan = false;
  curGPRState = gprState.get();
  curFPRState = fprState.get();

  rword basicBlockBeginAddr = 0;
  rword basicBlockEndAddr = 0;

  // Start address is out of range
  if (!execBroker->isInstrumented(start)) {
    return false;
  }

  running = true;

  // Execute basic block per basic block
  do {
    VMAction action = CONTINUE;

    // If this PC is not instrumented try to transfer execution
    if (execBroker->isInstrumented(currentPC) == false &&
        execBroker->canTransferExecution(curGPRState)) {

      curExecBlock = nullptr;
      basicBlockBeginAddr = 0;
      basicBlockEndAddr = 0;

      QBDI_DEBUG("Executing 0x{:x} through execBroker", currentPC);
      action = signalEvent(EXEC_TRANSFER_CALL, currentPC, nullptr, 0,
                           curGPRState, curFPRState);
      // transfer execution
      if (action == CONTINUE) {
        execBroker->transferExecution(currentPC, curGPRState, curFPRState);
        action = signalEvent(EXEC_TRANSFER_RETURN, currentPC, nullptr, 0,
                             curGPRState, curFPRState);
      }
    }
    // Else execute through DBI
    else {
      VMEvent event = VMEvent::SEQUENCE_ENTRY;
      QBDI_DEBUG("Executing 0x{:x} through DBI", currentPC);

      // Is cache flush pending?
      if (blockManager->isFlushPending()) {
        // Backup fprState and gprState
        *gprState = *curGPRState;
        *fprState = *curFPRState;
        curGPRState = gprState.get();
        curFPRState = fprState.get();
        // Commit the flush
        blockManager->flushCommit();
      }

      // Test if we have it in cache
      SeqLoc currentSequence;
      curExecBlock =
          blockManager->getProgrammedExecBlock(currentPC, &currentSequence);
      if (curExecBlock == nullptr) {
        QBDI_DEBUG(
            "Cache miss for 0x{:x}, patching & instrumenting new basic block",
            currentPC);
        handleNewBasicBlock(currentPC);
        // Signal a new basic block
        event |= BASIC_BLOCK_NEW;
        // Set new basic block as current
        curExecBlock =
            blockManager->getProgrammedExecBlock(currentPC, &currentSequence);
        QBDI_REQUIRE_ACTION(curExecBlock != nullptr, abort());
      }

      if (basicBlockEndAddr == 0) {
        event |= BASIC_BLOCK_ENTRY;
        basicBlockEndAddr = currentSequence.bbEnd;
        basicBlockBeginAddr = currentPC;
      }

      // Set context if necessary
      if (&(curExecBlock->getContext()->gprState) != curGPRState ||
          &(curExecBlock->getContext()->fprState) != curFPRState) {
        curExecBlock->getContext()->gprState = *curGPRState;
        curExecBlock->getContext()->fprState = *curFPRState;
      }
      curGPRState = &(curExecBlock->getContext()->gprState);
      curFPRState = &(curExecBlock->getContext()->fprState);

      action = signalEvent(event, currentPC, &currentSequence,
                           basicBlockBeginAddr, curGPRState, curFPRState);

      if (action == CONTINUE) {
        hasRan = true;
        action = curExecBlock->execute();
        // Signal events if normal exit
        if (action == CONTINUE) {
          if (basicBlockEndAddr == currentSequence.seqEnd) {
            action = signalEvent(SEQUENCE_EXIT | BASIC_BLOCK_EXIT, currentPC,
                                 &currentSequence, basicBlockBeginAddr,
                                 curGPRState, curFPRState);
            basicBlockBeginAddr = 0;
            basicBlockEndAddr = 0;
          } else {
            action = signalEvent(SEQUENCE_EXIT, currentPC, &currentSequence,
                                 basicBlockBeginAddr, curGPRState, curFPRState);
          }
        }
      }
    }
    if (action == STOP) {
      QBDI_DEBUG("Receive STOP Action");
      break;
    }
    if (action == BREAK_TO_VM) {
      basicBlockBeginAddr = 0;
      basicBlockEndAddr = 0;
    }
    // Get next block PC
    currentPC = QBDI_GPR_GET(curGPRState, REG_PC);
    QBDI_DEBUG("Next address to execute is 0x{:x}", currentPC);
  } while (currentPC != stop);

  // Copy final context
  *gprState = *curGPRState;
  *fprState = *curFPRState;
  curGPRState = gprState.get();
  curFPRState = fprState.get();
  curExecBlock = nullptr;
  running = false;

  return hasRan;
}

uint32_t Engine::addInstrRule(std::unique_ptr<InstrRule> &&rule) {
  uint32_t id = instrRulesCounter++;
  QBDI_REQUIRE_ACTION(id < EVENTID_VM_MASK, return VMError::INVALID_EVENTID);

  this->clearCache(rule->affectedRange());

  auto v = std::make_pair(id, std::move(rule));

  // insert rule in instrRules and keep the priority order
  auto it = std::upper_bound(instrRules.begin(), instrRules.end(), v,
                             [](const decltype(instrRules)::value_type &a,
                                const decltype(instrRules)::value_type &b) {
                               return a.second->getPriority() >
                                      b.second->getPriority();
                             });
  instrRules.insert(it, std::move(v));

  return id;
}

uint32_t Engine::addVMEventCB(VMEvent mask, VMCallback cbk, void *data) {
  uint32_t id = vmCallbacksCounter++;
  QBDI_REQUIRE_ACTION(id < EVENTID_VM_MASK, return VMError::INVALID_EVENTID);
  vmCallbacks.emplace_back(id, CallbackRegistration{mask, cbk, data});
  eventMask |= mask;
  return id | EVENTID_VM_MASK;
}

VMAction Engine::signalEvent(VMEvent event, rword currentPC,
                             const SeqLoc *seqLoc, rword basicBlockBegin,
                             GPRState *gprState, FPRState *fprState) {
  if ((event & eventMask) == 0) {
    return CONTINUE;
  }

  VMState vmState{event, currentPC, currentPC, currentPC, currentPC, 0};
  if (seqLoc != nullptr) {
    vmState.basicBlockStart = basicBlockBegin;
    vmState.basicBlockEnd = seqLoc->bbEnd;
    vmState.sequenceStart = seqLoc->seqStart;
    vmState.sequenceEnd = seqLoc->seqEnd;
  }

  VMAction action = CONTINUE;
  for (const auto &item : vmCallbacks) {
    const QBDI::CallbackRegistration &r = item.second;
    if (event & r.mask) {
      vmState.event = event;
      VMAction res = r.cbk(vminstance, &vmState, gprState, fprState, r.data);
      if (res > action) {
        action = res;
      }
    }
  }
  return action;
}

const InstAnalysis *Engine::getInstAnalysis(rword address,
                                            AnalysisType type) const {
  const ExecBlock *block = blockManager->getExecBlock(address);
  if (block == nullptr) {
    // not in cache
    return nullptr;
  }
  uint16_t instID = block->getInstID(address);
  QBDI_REQUIRE_ACTION(instID != NOT_FOUND, return nullptr);
  return block->getInstAnalysis(instID, type);
}

bool Engine::deleteInstrumentation(uint32_t id) {
  if (id & EVENTID_VM_MASK) {
    id &= ~EVENTID_VM_MASK;
    for (size_t i = 0; i < vmCallbacks.size(); i++) {
      if (vmCallbacks[i].first == id) {
        vmCallbacks.erase(vmCallbacks.begin() + i);
        return true;
      }
    }
  } else {
    for (size_t i = 0; i < instrRules.size(); i++) {
      if (instrRules[i].first == id) {
        this->clearCache(instrRules[i].second->affectedRange());
        instrRules.erase(instrRules.begin() + i);
        return true;
      }
    }
  }
  return false;
}

void Engine::deleteAllInstrumentations() {
  // clear cache
  for (const auto &r : instrRules) {
    this->clearCache(r.second->affectedRange());
  }
  instrRules.clear();
  vmCallbacks.clear();
  instrRulesCounter = 0;
  vmCallbacksCounter = 0;
  eventMask = VMEvent::NO_EVENT;
}

void Engine::clearAllCache() { blockManager->clearCache(not running); }

void Engine::clearCache(rword start, rword end) {
  blockManager->clearCache(Range<rword>(start, end));
  if (not running && blockManager->isFlushPending()) {
    blockManager->flushCommit();
  }
}

void Engine::clearCache(RangeSet<rword> rangeSet) {
  blockManager->clearCache(rangeSet);
  if (not running && blockManager->isFlushPending()) {
    blockManager->flushCommit();
  }
}

} // namespace QBDI
