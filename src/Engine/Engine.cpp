/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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
#include "Patch/PatchRuleAssembly.h"
#include "Utility/LogSys.h"

#include "QBDI/Bitmask.h"
#include "QBDI/Config.h"
#include "QBDI/Errors.h"
#include "QBDI/PtrAuth.h"
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

  // Get Patch rules Assembly for this architecture
  patchRuleAssembly = std::make_unique<PatchRuleAssembly>(options);

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

  // Get Patch rules Assembly for this architecture
  patchRuleAssembly = std::make_unique<PatchRuleAssembly>(options);

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
  QBDI_REQUIRE_ABORT(not running, "Cannot assign a running Engine");
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
  QBDI_REQUIRE_ABORT(not running, "Cannot setOptions on a running Engine");
  if (options != this->options) {
    QBDI_DEBUG("Change Options from {:x} to {:x}", this->options, options);
    clearAllCache();
    llvmCPUs->setOptions(options);

    // need to recreate all ExecBlock
    if (patchRuleAssembly->changeOptions(options)) {
      const RangeSet<rword> instrumentationRange =
          execBroker->getInstrumentedRange();

      blockManager = std::make_unique<ExecBlockManager>(*llvmCPUs, vminstance);
      execBroker = blockManager->getExecBroker();

      execBroker->setInstrumentedRange(instrumentationRange);
    }
    this->options = options;
  }
}

void Engine::changeVMInstanceRef(VMInstanceRef vminstance) {
  QBDI_REQUIRE_ABORT(not running,
                     "Cannot changeVMInstanceRef on a running Engine");
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
  QBDI_REQUIRE_ACTION(gprState, return);
  *(this->curGPRState) = *gprState;
}

void Engine::setFPRState(const FPRState *fprState) {
  QBDI_REQUIRE_ACTION(fprState, return);
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
  execBroker->addInstrumentedRange(Range<rword>(start, end, real_addr_t()));
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
  execBroker->removeInstrumentedRange(Range<rword>(start, end, real_addr_t()));
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
  QBDI_REQUIRE_ABORT(start == strip_ptrauth(start),
                     "Internal Error, unsupported authenticated pointer");

  std::vector<Patch> basicBlock;
  const LLVMCPU &llvmcpu = llvmCPUs->getCPU(curCPUMode);

  // if the first address is within the execution range,
  // stop the basic if the dissassembler went out of the range
  size_t sizeCode = (size_t)-1;
  const Range<rword> *curRange =
      execBroker->getInstrumentedRange().getElementRange(start);
  if (curRange != nullptr) {
    sizeCode = curRange->end() - start;
  }

  const llvm::ArrayRef<uint8_t> code((uint8_t *)start, sizeCode);
  rword address = start;
  QBDI_DEBUG("Patching basic block at address 0x{:x}", start);

  bool endLoop = false;
  // Get Basic block
  do {
    llvm::MCInst inst;
    uint64_t instSize;
    // Disassemble
    bool dstatus = llvmcpu.getInstruction(inst, instSize,
                                          code.slice(address - start), address);

    // handle disassembly error
    if (not dstatus) {
      QBDI_DEBUG("Bump into invalid instruction at address {:x}", address);

      // Current instruction is invalid, stop the basic block right here
      bool rollbackOK = patchRuleAssembly->earlyEnd(llvmcpu, basicBlock);

      // if fail to rollback or no Patch has been generated : fail
      if ((not rollbackOK) or (basicBlock.size() == 0)) {
        size_t sizeDump = start + sizeCode - address;
        if (sizeDump > 16) {
          sizeDump = 16;
        }
        QBDI_ABORT(
            "Disassembly error : fail to parse address 0x{:x} (CPUMode {}) "
            "({:n})",
            address, curCPUMode,
            spdlog::to_hex(reinterpret_cast<uint8_t *>(address),
                           reinterpret_cast<uint8_t *>(address + sizeDump)));
      } else {
        endLoop = true;
        break;
      }
    }
    QBDI_DEBUG("Disassembly address 0x{:x} ({:n})", address,
               spdlog::to_hex(reinterpret_cast<uint8_t *>(address),
                              reinterpret_cast<uint8_t *>(address + instSize)));

    // Generate Patch for this instruction
    QBDI_REQUIRE_ABORT(dstatus, "Unexpected dissassembly status");
    QBDI_DEBUG_BLOCK({
      std::string disass = llvmcpu.showInst(inst, address);
      QBDI_DEBUG("Patching 0x{:x} {}", address, disass.c_str());
    });
    endLoop = not patchRuleAssembly->generate(inst, address, instSize, llvmcpu,
                                              basicBlock);
    address += instSize;
  } while (endLoop);

  QBDI_REQUIRE_ABORT(basicBlock.size() > 0,
                     "No instruction to dissassemble found");

  QBDI_DEBUG("Basic block starting at address 0x{:x} ended at address 0x{:x}",
             start, basicBlock.back().metadata.endAddress());

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
    QBDI_DEBUG("Instrumenting {}", patch);

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
  blockManager->writeBasicBlock(std::move(basicBlock), patchEnd);
}

bool Engine::precacheBasicBlock(rword pc) {
  QBDI_REQUIRE_ABORT(pc == strip_ptrauth(pc),
                     "Internal Error, unsupported authenticated pointer");
  QBDI_REQUIRE_ABORT(not running,
                     "Cannot precacheBasicBlock on a running Engine");
  if (blockManager->isFlushPending()) {
    // Commit the flush
    blockManager->flushCommit();
  }
#if defined(QBDI_ARCH_ARM)
  curCPUMode = pc & 1 ? CPUMode::Thumb : CPUMode::ARM;
  pc &= (~1);
#endif
  if (blockManager->getExecBlock(pc, curCPUMode) != nullptr) {
    // already in cache
    return false;
  }
  running = true;
  handleNewBasicBlock(pc);
  running = false;
  return true;
}

bool Engine::run(rword start, rword stop) {
  QBDI_REQUIRE_ABORT(
      start == strip_ptrauth(start),
      "Internal Error, unsupported authenticated pointer for start pointer");
  QBDI_REQUIRE_ABORT(
      stop == strip_ptrauth(stop),
      "Internal Error, unsupported authenticated pointer for stop pointer");
  QBDI_REQUIRE_ABORT(not running, "Cannot run an already running Engine");

  rword currentPC = start;
  bool hasRan = false;
  bool warnAuthPC = true;
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
    if (execBroker->isInstrumented(currentPC) == false and
        execBroker->canTransferExecution(curGPRState)) {

#if defined(QBDI_ARCH_ARM)
      // Handle ARM mode
      // If we switch to the execbroker without an exchange, keep the current
      // mode
      bool changeCPUMode = curExecBlock == nullptr ||
                           curExecBlock->getContext()->hostState.exchange == 1;
      if (not changeCPUMode) {
        if (curCPUMode == CPUMode::Thumb) {
          currentPC = currentPC | 1;
        } else {
          currentPC = currentPC & (~1);
        }
      }
#endif

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

#if defined(QBDI_ARCH_ARM)
      // Handle ARM mode switching, only at the start of an execution or when
      // the guest signal an exchange.
      bool changeCPUMode = curExecBlock == nullptr ||
                           curExecBlock->getContext()->hostState.exchange == 1;
      if (changeCPUMode) {
        curCPUMode = currentPC & 1 ? CPUMode::Thumb : CPUMode::ARM;
        QBDI_DEBUG("CPUMode set to {}",
                   curCPUMode == CPUMode::ARM ? "ARM" : "Thumb");
      } else if (curCPUMode == CPUMode::ARM) {
        QBDI_REQUIRE_ABORT((currentPC & 1) == 0,
                           "Unexpected address in ARM mode");
      } else {
        QBDI_REQUIRE_ABORT((currentPC & 1) == 1,
                           "Unexpected address in Thumb mode");
      }
      currentPC = currentPC & (~1);
#endif

      QBDI_DEBUG("Executing 0x{:x} through DBI in mode {}", currentPC,
                 curCPUMode);

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
      curExecBlock = blockManager->getProgrammedExecBlock(currentPC, curCPUMode,
                                                          &currentSequence);
      if (curExecBlock == nullptr) {
        QBDI_DEBUG(
            "Cache miss for 0x{:x}, patching & instrumenting new basic block",
            currentPC);
        handleNewBasicBlock(currentPC);
        // Signal a new basic block
        event |= BASIC_BLOCK_NEW;
        // Set new basic block as current
        curExecBlock = blockManager->getProgrammedExecBlock(
            currentPC, curCPUMode, &currentSequence);
        QBDI_REQUIRE_ABORT(curExecBlock != nullptr,
                           "Fail to instrument the next basic block");
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
    if (action != CONTINUE) {
      basicBlockBeginAddr = 0;
      basicBlockEndAddr = 0;
      curExecBlock = nullptr;
    }
    // Get next block PC
    currentPC = QBDI_GPR_GET(curGPRState, REG_PC);
    rword noAuthPC = strip_ptrauth(currentPC);
    if (currentPC != noAuthPC) {
      if (warnAuthPC) {
        QBDI_WARN(
            "REG_PC value should not be authenticated pointer (get 0x{:x}, "
            "should be 0x{:x})",
            currentPC, noAuthPC);
        warnAuthPC = false;
      }
      currentPC = noAuthPC;
    }

    QBDI_DEBUG("Next address to execute is 0x{:x} (stop is 0x{:x})", currentPC,
               stop);
  } while (currentPC != stop);

  // Copy final context
  *gprState = *curGPRState;
  *fprState = *curFPRState;
  curGPRState = gprState.get();
  curFPRState = fprState.get();
  curExecBlock = nullptr;
  running = false;

  if (blockManager->isFlushPending()) {
    blockManager->flushCommit();
  }

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

InstrRule *Engine::getInstrRule(uint32_t id) {
  auto it = std::find_if(
      instrRules.begin(), instrRules.end(),
      [id](const std::pair<uint32_t, std::unique_ptr<InstrRule>> &el) {
        return id == el.first;
      });
  if (it == instrRules.end()) {
    return nullptr;
  } else {
    return it->second.get();
  }
}

uint32_t Engine::addVMEventCB(VMEvent mask, VMCallback cbk, void *data) {
  uint32_t id = vmCallbacksCounter++;
  QBDI_REQUIRE_ACTION(id < EVENTID_VM_MASK, return VMError::INVALID_EVENTID);
  vmCallbacks.emplace_back(id, CallbackRegistration{mask, cbk, data});
  eventMask |= mask;
  return id | EVENTID_VM_MASK;
}

bool Engine::setVMEventCB(uint32_t id, VMCallback cbk, void *data) {
  auto it =
      std::find_if(vmCallbacks.begin(), vmCallbacks.end(),
                   [id](const std::pair<uint32_t, CallbackRegistration> &el) {
                     return id == (el.first | EVENTID_VM_MASK);
                   });
  if (it == vmCallbacks.end()) {
    return false;
  } else {
    it->second.cbk = cbk;
    it->second.data = data;
    return true;
  }
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

#if defined(QBDI_ARCH_ARM)
  CPUMode cpumode = address & 1 ? CPUMode::Thumb : CPUMode::ARM;
  address &= (~1);
#else
  CPUMode cpumode = CPUMode::DEFAULT;
#endif
  const ExecBlock *block = blockManager->getExecBlock(address, cpumode);
  if (block == nullptr) {
    // not in cache
    return nullptr;
  }
  uint16_t instID = block->getInstID(address, cpumode);
  QBDI_REQUIRE_ACTION(instID != NOT_FOUND, return nullptr);
  return block->getInstAnalysis(instID, type);
}

std::optional<std::pair<const ExecBlock *, uint16_t>>
Engine::getPatchInfoOfJit(rword address) const {
  rword pageAddress = address & ~(ExecBlock::getPageSize() - 1);
  QBDI_DEBUG("Search Patch address 0x{:x} with page address 0x{:x}", address,
             pageAddress);
  auto *e = blockManager->getExecBlockFromJitAddress(pageAddress);

  if (e == nullptr) {
    QBDI_DEBUG("No ExecBlock with page address 0x{:x}", pageAddress);
    return {};
  }
  uint16_t instId = e->getPatchAddressOfJit(address);
  if (instId == NOT_FOUND) {
    QBDI_DEBUG("No Instruction in execBlock 0x{:x} with JIT address 0x{:x}",
               reinterpret_cast<uintptr_t>(e), address);
    return {{e, NOT_FOUND}};
  } else {
    QBDI_DEBUG(
        "Found Instruction {} in execBlock 0x{:x} with JIT address 0x{:x}",
        instId, reinterpret_cast<uintptr_t>(e), address);
    return {{e, instId}};
  }
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
  blockManager->clearCache(Range<rword>(start, end, real_addr_t()));
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

uint32_t Engine::getNbExecBlock() const {
  return blockManager->getNbExecBlock();
}

void Engine::reduceCacheTo(uint32_t nb) {
  blockManager->reduceCacheTo(nb);
  if (not running && blockManager->isFlushPending()) {
    blockManager->flushCommit();
  }
}

} // namespace QBDI
