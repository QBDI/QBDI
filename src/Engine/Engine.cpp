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
#include <bitset>

#include "Engine/Engine.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"

#include "ExecBlock/ExecBlockManager.h"
#include "ExecBroker/ExecBroker.h"
#include "Patch/InstInfo.h"
#include "Patch/InstMetadata.h"
#include "Patch/InstrRule.h"
#include "Patch/InstrRules.h"
#include "Patch/Patch.h"
#include "Patch/PatchRule.h"
#include "Patch/PatchRules.h"
#include "Patch/RegisterSize.h"
#include "Patch/Types.h"
#include "Utility/Assembly.h"
#include "Utility/LogSys.h"
#include "Utility/System.h"

#include "QBDI/Errors.h"
#include "QBDI/Platform.h"

// Mask to identify VM events
#define EVENTID_VM_MASK (1UL << 30)

namespace QBDI {

Engine::Engine(const std::string &_cpu, const std::vector<std::string> &_mattrs,
               Options opts, VMInstanceRef vminstance)
    : cpu(_cpu), mattrs(_mattrs), vminstance(vminstance), instrRulesCounter(0),
      vmCallbacksCounter(0), options(opts), eventMask(VMEvent::NO_EVENT),
      running(false) {
  init();
}

Engine::~Engine() = default;

// may need to reinit the Engine during a copy
void Engine::init() {
  static bool firstInit = true;

  std::string error;
  std::string featuresStr;
  const llvm::Target *processTarget;

  if (firstInit) {
    firstInit = false;
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllDisassemblers();
  }

  // Build features string
  if (cpu.empty()) {
    cpu = QBDI::getHostCPUName();
    // If API is broken on ARM, we are facing big problems...
    if constexpr (is_arm) {
      QBDI_REQUIRE(!cpu.empty() && cpu != "generic");
    }
  }
  if (mattrs.empty()) {
    mattrs = getHostCPUFeatures();
  }
  if (!mattrs.empty()) {
    llvm::SubtargetFeatures features;
    for (unsigned i = 0; i != mattrs.size(); ++i) {
      features.AddFeature(mattrs[i]);
    }
    featuresStr = features.getString();
  }

  // lookup target
  tripleName = llvm::Triple::normalize(llvm::sys::getDefaultTargetTriple());
  llvm::Triple processTriple(tripleName);
  processTarget = llvm::TargetRegistry::lookupTarget(tripleName, error);
  QBDI_DEBUG("Initialized LLVM for target {}", tripleName.c_str());

  // Allocate all LLVM classes
  llvm::MCTargetOptions MCOptions;
  MRI = std::unique_ptr<llvm::MCRegisterInfo>(
      processTarget->createMCRegInfo(tripleName));
  MAI = std::unique_ptr<llvm::MCAsmInfo>(
      processTarget->createMCAsmInfo(*MRI, tripleName, MCOptions));
  MOFI = std::make_unique<llvm::MCObjectFileInfo>();
  MCTX = std::make_unique<llvm::MCContext>(MAI.get(), MRI.get(), MOFI.get());
  MOFI->InitMCObjectFileInfo(processTriple, false, *MCTX);
  MCII = std::unique_ptr<llvm::MCInstrInfo>(processTarget->createMCInstrInfo());
  MSTI = std::unique_ptr<llvm::MCSubtargetInfo>(
      processTarget->createMCSubtargetInfo(tripleName, cpu, featuresStr));
  QBDI_DEBUG("Initialized LLVM subtarget with cpu {} and features {}",
             cpu.c_str(), featuresStr.c_str());

  auto MAB = std::unique_ptr<llvm::MCAsmBackend>(
      processTarget->createMCAsmBackend(*MSTI, *MRI, MCOptions));
  MCE = std::unique_ptr<llvm::MCCodeEmitter>(
      processTarget->createMCCodeEmitter(*MCII, *MRI, *MCTX));
  // Allocate QBDI classes
  assembly = std::make_unique<Assembly>(*MCTX, std::move(MAB), *MCII,
                                        processTarget, *MSTI, options);
  blockManager = std::make_unique<ExecBlockManager>(*assembly, vminstance);
  execBroker = std::make_unique<ExecBroker>(*assembly, vminstance);

  // Get default Patch rules for this architecture
  patchRules = getDefaultPatchRules();

  gprState = std::make_unique<GPRState>();
  fprState = std::make_unique<FPRState>();
  curGPRState = gprState.get();
  curFPRState = fprState.get();

  initGPRState();
  initFPRState();

  curExecBlock = nullptr;
}

void Engine::reinit(const std::string &cpu_,
                    const std::vector<std::string> &mattrs_, Options opts) {
  QBDI_REQUIRE_ACTION(not running && "Cannot reInit a running Engine", abort());

  // clear callbacks
  instrRules.clear();
  vmCallbacks.clear();
  instrRulesCounter = 0;
  vmCallbacksCounter = 0;
  eventMask = VMEvent::NO_EVENT;

  // clear state
  gprState.reset();
  curGPRState = nullptr;
  fprState.reset();
  curFPRState = nullptr;
  patchRules.clear();

  // close internal object
  execBroker.reset();
  blockManager.reset();
  assembly.reset();

  // close all llvm references
  MCE.reset();
  MSTI.reset();
  MCII.reset();
  MCTX.reset();
  MOFI.reset();
  MAI.reset();
  MRI.reset();

  cpu = cpu_;
  mattrs = mattrs_;
  tripleName = "";
  options = opts;

  init();
}

Engine::Engine(const Engine &other)
    : cpu(other.cpu), mattrs(other.mattrs), vminstance(nullptr),
      options(other.options), running(false) {
  this->init();
  *this = other;
}

Engine &Engine::operator=(const Engine &other) {
  QBDI_REQUIRE_ACTION(not running && "Cannot assign a running Engine", abort());
  this->clearAllCache();

  if (cpu != other.cpu || mattrs != other.mattrs) {
    reinit(other.cpu, other.mattrs, other.options);
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
    assembly->setOptions(options);

    // need to recreate all ExecBlock
    if (((this->options ^ options) &
         (Options::OPT_DISABLE_FPR | Options::OPT_DISABLE_OPTIONAL_FPR)) != 0) {
      const RangeSet<rword> instrumentationRange =
          execBroker->getInstrumentedRange();

      blockManager = std::make_unique<ExecBlockManager>(*assembly, vminstance);
      execBroker = std::make_unique<ExecBroker>(*assembly, vminstance);

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
  execBroker->changeVMInstanceRef(vminstance);

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
  const llvm::ArrayRef<uint8_t> code((uint8_t *)start, (size_t)-1);
  bool basicBlockEnd = false;
  rword i = 0;
  QBDI_DEBUG("Patching basic block at address 0x{:x}", start);

  // Get Basic block
  while (basicBlockEnd == false) {
    llvm::MCInst inst;
    llvm::MCDisassembler::DecodeStatus dstatus;
    rword address;
    Patch patch;
    uint64_t instSize = 0;

    // Aggregate a complete patch
    do {
      // Disassemble
      dstatus = assembly->getInstruction(inst, instSize, code.slice(i), i);
      address = start + i;
      QBDI_REQUIRE_ACTION(llvm::MCDisassembler::Success == dstatus, abort());
      QBDI_DEBUG_BLOCK({
        std::string disass = assembly->showInst(inst, address);
        QBDI_DEBUG("Patching 0x{:x} {}", address, disass.c_str());
      });
      // Patch & merge
      for (uint32_t j = 0; j < patchRules.size(); j++) {
        if (patchRules[j].canBeApplied(inst, address, instSize, MCII.get())) {
          QBDI_DEBUG("Patch rule {:x} applied", j);
          if (patch.insts.size() == 0) {
            patch = patchRules[j].generate(inst, address, instSize, MCII.get(),
                                           MRI.get());
          } else {
            QBDI_DEBUG("Previous instruction merged");
            patch = patchRules[j].generate(inst, address, instSize, MCII.get(),
                                           MRI.get(), &patch);
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
  QBDI_DEBUG(
      "Instrumenting sequence [0x{:x}, 0x{:x}] in basic block [0x{:x}, 0x{:x}]",
      basicBlock.front().metadata.address,
      basicBlock[patchEnd - 1].metadata.address,
      basicBlock.front().metadata.address, basicBlock.back().metadata.address);

  for (size_t i = 0; i < patchEnd; i++) {
    Patch &patch = basicBlock[i];
    QBDI_DEBUG_BLOCK({
      std::string disass =
          assembly->showInst(patch.metadata.inst, patch.metadata.address);
      QBDI_DEBUG("Instrumenting 0x{:x} {}", patch.metadata.address,
                 disass.c_str());
    });
    // Instrument
    for (const auto &item : instrRules) {
      const InstrRule *rule = item.second.get();
      if (rule->tryInstrument(patch, MCII.get(), MRI.get(),
                              assembly.get())) { // Push MCII
        QBDI_DEBUG("Instrumentation rule {:x} applied", item.first);
      }
    }
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
                               return a.second->getPriority() <
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
