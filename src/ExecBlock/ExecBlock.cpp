/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <system_error>

#include "llvm/ADT/Optional.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/Process.h"

#include "devVariable.h"
#include "Engine/LLVMCPU.h"
#include "ExecBlock/Context.h"
#include "ExecBlock/ExecBlock.h"
#include "Patch/ExecBlockFlags.h"
#include "Patch/ExecBlockPatch.h"
#include "Patch/Patch.h"
#include "Patch/PatchGenerator.h"
#include "Patch/Register.h"
#include "Patch/RelocatableInst.h"
#include "Patch/Types.h"
#include "Utility/InstAnalysis_prive.h"
#include "Utility/LogSys.h"
#include "Utility/System.h"
#include "Utility/memory_ostream.h"

#include "QBDI/Options.h"
#include "QBDI/State.h"

namespace QBDI {

ExecBlock::ExecBlock(
    const LLVMCPUs &llvmCPUs, VMInstanceRef vminstance,
    const std::vector<std::unique_ptr<RelocatableInst>> *execBlockPrologue,
    const std::vector<std::unique_ptr<RelocatableInst>> *execBlockEpilogue,
    uint32_t epilogueSize_)
    : vminstance(vminstance), llvmCPUs(llvmCPUs), epilogueSize(epilogueSize_),
      isFull(false) {

  // Allocate memory blocks
  std::error_code ec;
  // iOS now use 16k superpages, but as JIT mecanisms are totally differents
  // on this platform, we can enforce a 4k "virtual" page size
  uint64_t pageSize =
      is_ios ? 4096
             : llvm::expectedToOptional(llvm::sys::Process::getPageSize())
                   .getValueOr(4096);
  unsigned mflags = PF::MF_READ | PF::MF_WRITE;

  if constexpr (is_ios)
    mflags |= PF::MF_EXEC;

  // Allocate 2 pages block
  codeBlock = QBDI::allocateMappedMemory(2 * pageSize, nullptr, mflags, ec);
  QBDI_REQUIRE_ABORT(codeBlock.base() != nullptr, "allocation fail");
  // Split it in two blocks
  dataBlock = llvm::sys::MemoryBlock(
      reinterpret_cast<void *>(reinterpret_cast<uint64_t>(codeBlock.base()) +
                               pageSize),
      pageSize);
  codeBlock = llvm::sys::MemoryBlock(codeBlock.base(), pageSize);
  QBDI_DEBUG("codeBlock @ 0x{:x} | dataBlock @ 0x{:x} | pageSize {} bytes",
             reinterpret_cast<rword>(codeBlock.base()),
             reinterpret_cast<rword>(dataBlock.base()), pageSize);

  // Other initializations
  context = static_cast<Context *>(dataBlock.base());
  shadows = reinterpret_cast<rword *>(
      reinterpret_cast<rword>(dataBlock.base()) + sizeof(Context));
  shadowIdx = 0;
  currentSeq = 0;
  currentInst = 0;
  codeStream.set_stream(codeBlock);
  pageState = RW;

  std::vector<std::unique_ptr<RelocatableInst>> execBlockPrologue_;
  std::vector<std::unique_ptr<RelocatableInst>> execBlockEpilogue_;

  const LLVMCPU &llvmcpu = llvmCPUs.getCPU(CPUMode::DEFAULT);

  if (execBlockPrologue == nullptr) {
    execBlockPrologue_ = getExecBlockPrologue(llvmcpu);
    execBlockPrologue = &execBlockPrologue_;
  }
  if (execBlockEpilogue == nullptr) {
    execBlockEpilogue_ = getExecBlockEpilogue(llvmcpu);
    execBlockEpilogue = &execBlockEpilogue_;
  }

  if (epilogueSize == 0) {
    for (const auto &inst : *execBlockEpilogue) {
      epilogueSize += inst->getSize(llvmcpu);
    }
    QBDI_DEBUG("Detect Epilogue size: {}", epilogueSize);
  }
  // JIT prologue and epilogue
  codeStream.seek(codeBlock.allocatedSize() - epilogueSize);
  QBDI_REQUIRE_ABORT(
      applyRelocatedInst(*execBlockEpilogue, nullptr, llvmcpu, 0),
      "Fail to write Epilogue");
  QBDI_REQUIRE_ABORT(codeStream.current_pos() == codeBlock.allocatedSize(),
                     "Wrong Epilogue Size");

  codeStream.seek(0);
  QBDI_REQUIRE_ABORT(
      applyRelocatedInst(*execBlockPrologue, nullptr, llvmcpu, epilogueSize),
      "Fail to write Prologue");
}

ExecBlock::~ExecBlock() {
  // Reunite the 2 blocks before freeing them
  codeBlock = llvm::sys::MemoryBlock(
      codeBlock.base(), codeBlock.allocatedSize() + dataBlock.allocatedSize());
  QBDI::releaseMappedMemory(codeBlock);
}

void ExecBlock::changeVMInstanceRef(VMInstanceRef vminstance) {
  this->vminstance = vminstance;
}

void ExecBlock::show() const {
  rword i;
  uint64_t instSize;
  bool dstatus;
  llvm::ArrayRef<uint8_t> jitCode(
      static_cast<const uint8_t *>(codeBlock.base()), codeStream.current_pos());
  size_t mode = 0;
  const LLVMCPU *llvmcpu = &llvmCPUs.getCPU((CPUMode)mode);

  fprintf(stderr, "---- JIT CODE ----\n");
  for (i = 0; i < jitCode.size(); i += instSize) {
    llvm::MCInst inst;
    std::string disass;

    dstatus = llvmcpu->getInstruction(inst, instSize, jitCode.slice(i), i);
    if constexpr (CPUMode::COUNT > 1) {
      if (CPUMode::COUNT > 1 and not dstatus) { // Try to switch mode
        mode = (mode + 1) % CPUMode::COUNT;
        llvmcpu = &llvmCPUs.getCPU((CPUMode)mode);
        dstatus = llvmcpu->getInstruction(inst, instSize, jitCode.slice(i), i);
      }
    }
    QBDI_REQUIRE_ACTION(dstatus, break);

    disass =
        llvmcpu->showInst(inst, reinterpret_cast<rword>(codeBlock.base()) + i);
    fprintf(stderr, "%s\n", disass.c_str());
  }

  fprintf(stderr, "---- CONTEXT ----\n");
  for (i = 0; i < NUM_GPR; i++) {
    fprintf(stderr, "%s=0x%016" PRIRWORD " ",
            llvmcpu->getRegisterName(GPR_ID[i]),
            QBDI_GPR_GET(&context->gprState, i));
    if (i % 4 == 0)
      fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n");

  fprintf(stderr, "---- SHADOWS ----\n[");
  for (i = 0; i + 1 < shadowIdx; i++)
    fprintf(stderr, "0x%016" PRIRWORD ", ", shadows[i]);
  if (shadowIdx > 0)
    fprintf(stderr, "0x%016" PRIRWORD, shadows[i - 1]);
  fprintf(stderr, "]\n");
}

VMAction ExecBlock::execute() {
  QBDI_DEBUG("Executing ExecBlock 0x{:x} programmed with selector at 0x{:x}",
             reinterpret_cast<uintptr_t>(this), context->hostState.selector);

  do {
    context->hostState.callback = static_cast<rword>(0);
    context->hostState.data = static_cast<rword>(0);

    QBDI_DEBUG("Execution of ExecBlock 0x{:x} resumed at 0x{:x}",
               reinterpret_cast<uintptr_t>(this), context->hostState.selector);
    run();

    if (context->hostState.callback != 0) {
      currentInst = context->hostState.origin;
      rword currentPC = QBDI_GPR_GET(&context->gprState, REG_PC);

      QBDI_DEBUG("Callback request by ExecBlock 0x{:x} for callback 0x{:x}",
                 reinterpret_cast<uintptr_t>(this),
                 context->hostState.callback);
      QBDI_REQUIRE(currentInst < instMetadata.size());

      VMAction r =
          (reinterpret_cast<InstCallback>(context->hostState.callback))(
              vminstance, &context->gprState, &context->fprState,
              (void *)context->hostState.data);

      switch (r) {
        case CONTINUE:
          QBDI_DEBUG("Callback 0x{:x} returned CONTINUE",
                     context->hostState.callback);
          if (QBDI_GPR_GET(&context->gprState, REG_PC) != currentPC) {
            QBDI_WARN(
                "Callback returned CONTINUE but change PC: Ignore new value");
          }
          break;
        case SKIP_INST:
          QBDI_DEBUG("Callback 0x{:x} returned SKIP_INST",
                     context->hostState.callback);
          if (not instMetadata[currentInst].modifyPC and
              QBDI_GPR_GET(&context->gprState, REG_PC) != currentPC) {
            QBDI_WARN(
                "Callback returned SKIP_INST but change PC: Ignore new value");
          }
          if (currentPC == instMetadata[currentInst].address) {
            context->hostState.selector =
                reinterpret_cast<rword>(codeBlock.base()) +
                static_cast<rword>(instRegistry[currentInst].offsetSkip);
          } else {
            QBDI_WARN(
                "POSTINST callback returned SKIP_INST: Use CONTINUE instead");
          }
          break;
        case SKIP_PATCH:
          QBDI_DEBUG("Callback 0x{:x} returned SKIP_PATCH",
                     context->hostState.callback);
          if (not instMetadata[currentInst].modifyPC and
              QBDI_GPR_GET(&context->gprState, REG_PC) != currentPC) {
            QBDI_WARN(
                "Callback returned SKIP_PATCH but change PC: Ignore new value");
          }
          if (instMetadata[currentInst].modifyPC) {
            QBDI_WARN(
                "Callback returned SKIP on instruction that change PC. Use "
                "BREAK_TO_VM instead.");
            return BREAK_TO_VM;
          } else if (currentInst == seqRegistry[currentSeq].endInstID) {
            rword next_address = instMetadata[currentInst].address +
                                 instMetadata[currentInst].instSize;
#if defined(QBDI_ARCH_ARM)
            if (instMetadata[currentInst].cpuMode == CPUMode::Thumb) {
              next_address |= 1;
            }
#endif
            QBDI_GPR_SET(&context->gprState, REG_PC, next_address);
            return BREAK_TO_VM;
          } else {
            // go to currentInst + 1
            currentInst += 1;
            context->hostState.selector =
                reinterpret_cast<rword>(codeBlock.base()) +
                static_cast<rword>(instRegistry[currentInst].offset);
          }
          break;
        case BREAK_TO_VM:
          QBDI_DEBUG("Callback 0x{:x} returned BREAK_TO_VM",
                     context->hostState.callback);
          return BREAK_TO_VM;
        case STOP:
          QBDI_DEBUG("Callback 0x{:x} returned STOP",
                     context->hostState.callback);
          return STOP;
      }
    }
  } while (context->hostState.callback != 0);
  currentInst = seqRegistry[currentSeq].endInstID;

  return CONTINUE;
}
bool ExecBlock::applyRelocatedInst(
    const std::vector<std::unique_ptr<RelocatableInst>> &reloc,
    std::vector<TagInfo> *tags, const LLVMCPU &llvmcpu, unsigned limit) {

  for (const RelocatableInst::UniquePtr &inst : reloc) {
    if (inst->getTag() != RelocatableInstTag::RelocInst) {
      QBDI_DEBUG("RelocTag 0x{:x}", inst->getTag());
      if (tags != nullptr) {
        tags->push_back(
            TagInfo{static_cast<uint16_t>(inst->getTag()),
                    static_cast<uint16_t>(codeStream.current_pos())});
      }
      continue;
    } else if (codeBlock.allocatedSize() - codeStream.current_pos() > limit) {
#ifdef CHECK_INSTRUCTION_SIZE
      unsigned pos = codeStream.current_pos();
#endif
      llvmcpu.writeInstruction(inst->reloc(this, llvmcpu), codeStream);

#ifdef CHECK_INSTRUCTION_SIZE
      unsigned instSize = inst->getSize(llvmcpu);
      if (codeStream.current_pos() - pos != instSize) {
        QBDI_ABORT(
            "getSize doesn't return the good size "
            "(result: {}, expected: {})",
            instSize, codeStream.current_pos() - pos);
      }
#endif
    } else {
      QBDI_DEBUG("Not enough space left: rollback");
      return false;
    }
  }
  return true;
}

SeqWriteResult
ExecBlock::writeSequence(std::vector<Patch>::const_iterator seqIt,
                         std::vector<Patch>::const_iterator seqEnd) {
  rword startOffset = (rword)codeStream.current_pos();
  uint16_t startInstID = getNextInstID();
  uint16_t seqID = getNextSeqID();
  uint8_t executeFlags = 0;
  unsigned patchWritten = 0;

  // Check if there's enough space left
  if (isFull) {
    QBDI_DEBUG("ExecBlock 0x{:x} is full", reinterpret_cast<uintptr_t>(this));
    return {EXEC_BLOCK_FULL, 0, 0};
  }

  // Refuse to write empty sequence
  if (seqIt == seqEnd) {
    QBDI_WARN("Attempting to write empty sequence");
    return {EXEC_BLOCK_FULL, 0, 0};
  }

  CPUMode cpuMode = seqIt->metadata.cpuMode;
  const LLVMCPU &llvmcpu = llvmCPUs.getCPU(cpuMode);

  QBDI_DEBUG("Attempting to write {} patches to ExecBlock 0x{:x}",
             std::distance(seqIt, seqEnd), reinterpret_cast<uintptr_t>(this));
  // Pages are RWX on iOS
  // Ensure code block is RW
  if constexpr (not is_ios) {
    makeRW();
  }
  initScratchRegisterForPatch(seqIt, seqEnd);
  bool needTerminator = true;
  // JIT the basic block instructions patch per patch
  // A patch correspond to an original instruction and should be written in its
  // entierty
  while (seqIt != seqEnd) {
    rword rollbackOffset = codeStream.current_pos();
    uint32_t rollbackShadowIdx = shadowIdx;
    size_t rollbackShadowRegistry = shadowRegistry.size();
    size_t rollbackTagRegistry = tagRegistry.size();

    QBDI_DEBUG_BLOCK({
      std::string disass =
          llvmcpu.showInst(seqIt->metadata.inst, seqIt->metadata.address);
      QBDI_DEBUG(
          "Attempting to write patch of {} RelocatableInst to ExecBlock 0x{:x} "
          "for instruction {:x}: {}",
          seqIt->metadata.patchSize, reinterpret_cast<uintptr_t>(this),
          seqIt->metadata.address, disass.c_str());
    });

    // Attempt to write a complete patch. If not, rollback to the last complete
    // patch written
    if (not writePatch(seqIt, seqEnd, llvmcpu)) {

      QBDI_DEBUG("Rolling back to offset 0x{:x}", rollbackOffset);

      QBDI_REQUIRE_ABORT(codeStream.current_pos() <=
                             codeBlock.allocatedSize() - epilogueSize,
                         "Internal Error, Overflow in Epilogue");
      // Seek to the last complete patch written and terminate it with a
      // terminator
      codeStream.seek(rollbackOffset);
      // free shadows and tag allocated by the rollbacked code
      shadowIdx = rollbackShadowIdx;
      shadowRegistry.resize(rollbackShadowRegistry);
      tagRegistry.resize(rollbackTagRegistry);
      // It's a NULL rollback, don't terminate it
      if (rollbackOffset == startOffset) {
        QBDI_DEBUG("NULL rollback, nothing written to ExecBlock 0x{:x}",
                   reinterpret_cast<uintptr_t>(this));
        return {EXEC_BLOCK_FULL, 0, 0};
      }
      needTerminator = true;
      break;
    } else {
      // Complete instruction was written, we add the metadata
      // Move the analysis of the instruction in the cached metadata
      instMetadata.push_back(seqIt->metadata.lightCopy());
      instMetadata.back().analysis.reset(seqIt->metadata.analysis.release());
      // Register instruction
      instRegistry.push_back(InstInfo{
          seqID, 0, 0, static_cast<uint16_t>(rollbackShadowRegistry),
          static_cast<uint16_t>(shadowRegistry.size() - rollbackShadowRegistry),
          static_cast<uint16_t>(rollbackTagRegistry),
          static_cast<uint16_t>(tagRegistry.size() - rollbackTagRegistry)});
      // compute begin of the new instruction (writePatch can add extra data
      // to perform the transition from the previous instruction, and we should
      // skip it)
      std::vector<TagInfo> beginPatchTag =
          queryTagByInst(instRegistry.size() - 1, RelocTagPatchBegin);
      QBDI_REQUIRE_ABORT(beginPatchTag.size() == 1,
                         "Internal Error: begin tag not found");
      instRegistry.back().offset = beginPatchTag[0].offset;
      // compute offsetSkip of the new instruction
      std::vector<TagInfo> endInstPatchTag =
          queryTagByInst(instRegistry.size() - 1, RelocTagPatchInstEnd);
      QBDI_REQUIRE_ABORT(endInstPatchTag.size() == 1,
                         "Internal Error: end tag not found");
      instRegistry.back().offsetSkip = endInstPatchTag[0].offset;
      // set the scratch Register if needed
      finalizeScratchRegisterForPatch();
      // Update indexes
      needTerminator = not seqIt->metadata.modifyPC;
      executeFlags |= seqIt->metadata.execblockFlags;
      seqIt++;
      patchWritten += 1;
    }
  }
  // The last instruction of the sequence doesn't end with a change of RIP/PC,
  // add a Terminator
  if (needTerminator) {
    QBDI_DEBUG(
        "Writting terminator to ExecBlock 0x{:x} to finish non-exit sequence",
        reinterpret_cast<uintptr_t>(this));
    RelocatableInst::UniquePtrVec terminator =
        getTerminator(llvmcpu, instMetadata.back().endAddress());
    QBDI_REQUIRE_ABORT(
        applyRelocatedInst(terminator, nullptr, llvmcpu, epilogueSize),
        "Fail to write Terminator");
  }
  // JIT the jump to epilogue
  RelocatableInst::UniquePtrVec jmpEpilogue = JmpEpilogue().genReloc(llvmcpu);
  QBDI_REQUIRE_ABORT(
      applyRelocatedInst(jmpEpilogue, nullptr, llvmcpu, epilogueSize),
      "Fail to write jmpEpilogue");
  // change the flag of the basicblock
  if (llvmcpu.getOptions() & Options::OPT_DISABLE_FPR) {
    executeFlags = 0;
  } else if (llvmcpu.getOptions() & Options::OPT_DISABLE_OPTIONAL_FPR) {
    executeFlags = defaultExecuteFlags;
  }
  // Register sequence
  uint16_t endInstID = getNextInstID() - 1;
  seqRegistry.push_back(SeqInfo{startInstID, endInstID, executeFlags, cpuMode,
                                instRegistry[startInstID].sr});
  // Return write results
  unsigned bytesWritten =
      static_cast<unsigned>(codeStream.current_pos() - startOffset);
  QBDI_REQUIRE_ABORT(codeStream.current_pos() <=
                         codeBlock.allocatedSize() - epilogueSize,
                     "Internal Error, Overflow in Epilogue");
  QBDI_DEBUG("End write sequence in basicblock 0x{:x} with execFlags : {:x}",
             reinterpret_cast<uintptr_t>(this), executeFlags);
  return SeqWriteResult{seqID, bytesWritten, patchWritten};
}

uint16_t ExecBlock::splitSequence(uint16_t instID) {
  QBDI_REQUIRE(instID < instRegistry.size());
  uint16_t seqID = instRegistry[instID].seqID;
  seqRegistry.push_back(SeqInfo{
      instID, seqRegistry[seqID].endInstID, seqRegistry[seqID].executeFlags,
      seqRegistry[seqID].cpuMode, instRegistry[instID].sr});
  return getNextSeqID() - 1;
}

void ExecBlock::makeRX() {
  if (not isRX()) {
    QBDI_DEBUG("Making ExecBlock 0x{:x} RX", reinterpret_cast<uintptr_t>(this));
    QBDI_REQUIRE_ABORT(!llvm::sys::Memory::protectMappedMemory(
                           codeBlock, PF::MF_READ | PF::MF_EXEC),
                       "Fail to set the page permission to RX");
    pageState = RX;
  }
}

void ExecBlock::makeRW() {
  if (not isRW()) {
    QBDI_DEBUG("Making ExecBlock 0x{:x} RW", reinterpret_cast<uintptr_t>(this));
    QBDI_REQUIRE_ABORT(!llvm::sys::Memory::protectMappedMemory(
                           codeBlock, PF::MF_READ | PF::MF_WRITE),
                       "Fail to set the page permission to RW");
    pageState = RW;
  }
}

uint16_t ExecBlock::newShadow(uint16_t tag) {
  uint16_t id = shadowIdx++;
  QBDI_REQUIRE_ABORT(id * sizeof(rword) <
                         dataBlock.allocatedSize() - sizeof(Context),
                     "Shadow allocation fail");
  if (tag != ShadowReservedTag::Untagged) {
    QBDI_DEBUG("Registering new tagged shadow {} for instID {} wih tag {:x}",
               id, getNextInstID(), tag);
    shadowRegistry.push_back({getNextInstID(), tag, id});
  }
  return id;
}

uint16_t ExecBlock::getLastShadow(uint16_t tag) {
  uint16_t nextInstID = getNextInstID();

  for (auto it = shadowRegistry.crbegin(); it != shadowRegistry.crend(); ++it) {
    if (it->instID == nextInstID && it->tag == tag) {
      return it->shadowID;
    }
  }
  QBDI_ABORT("Cannot found shadow tag {:x} for the current instruction", tag);
}

void ExecBlock::setShadow(uint16_t id, rword v) {
  QBDI_REQUIRE_ABORT(id * sizeof(rword) <
                         dataBlock.allocatedSize() - sizeof(Context),
                     "Invalid shadow ID");
  QBDI_DEBUG("Set shadow {} to 0x{:x}", id, v);
  shadows[id] = v;
}

rword ExecBlock::getShadow(uint16_t id) const {
  QBDI_REQUIRE_ABORT(id * sizeof(rword) <
                         dataBlock.allocatedSize() - sizeof(Context),
                     "Invalid shadow ID");
  return shadows[id];
}

rword ExecBlock::getShadowOffset(uint16_t id) const {
  rword offset = sizeof(Context) + id * sizeof(rword);
  QBDI_REQUIRE_ABORT(offset < dataBlock.allocatedSize(), "Invalid shadow ID");
  return offset;
}

uint16_t ExecBlock::getInstID(rword address, CPUMode cpuMode) const {
  for (size_t i = 0; i < instMetadata.size(); i++) {
    if (instMetadata[i].address == address and
        instMetadata[i].cpuMode == cpuMode) {
      return (uint16_t)i;
    }
  }
  return NOT_FOUND;
}

const InstMetadata &ExecBlock::getInstMetadata(uint16_t instID) const {
  QBDI_REQUIRE(instID < instMetadata.size());
  return instMetadata[instID];
}

rword ExecBlock::getInstAddress(uint16_t instID) const {
  QBDI_REQUIRE(instID < instMetadata.size());
  return instMetadata[instID].address;
}

rword ExecBlock::getInstInstrumentedAddress(uint16_t instID) const {
  QBDI_REQUIRE(instID < instMetadata.size());
  return reinterpret_cast<rword>(codeBlock.base()) +
         static_cast<rword>(instRegistry[instID].offset);
}

const llvm::MCInst &ExecBlock::getOriginalMCInst(uint16_t instID) const {
  QBDI_REQUIRE(instID < instMetadata.size());
  return instMetadata[instID].inst;
}

const InstAnalysis *ExecBlock::getInstAnalysis(uint16_t instID,
                                               AnalysisType type) const {
  QBDI_REQUIRE(instID < instMetadata.size());
  return analyzeInstMetadata(instMetadata[instID], type,
                             llvmCPUs.getCPU(instMetadata[instID].cpuMode));
}

uint16_t ExecBlock::getSeqID(rword address, CPUMode cpuMode) const {
  for (size_t i = 0; i < seqRegistry.size(); i++) {
    if (instMetadata[seqRegistry[i].startInstID].address == address and
        instMetadata[seqRegistry[i].startInstID].cpuMode == cpuMode) {
      return (uint16_t)i;
    }
  }
  return NOT_FOUND;
}

uint16_t ExecBlock::getSeqID(uint16_t instID) const {
  QBDI_REQUIRE(instID < instRegistry.size());
  return instRegistry[instID].seqID;
}

uint16_t ExecBlock::getSeqStart(uint16_t seqID) const {
  QBDI_REQUIRE(seqID < seqRegistry.size());
  return seqRegistry[seqID].startInstID;
}

uint16_t ExecBlock::getSeqEnd(uint16_t seqID) const {
  QBDI_REQUIRE(seqID < seqRegistry.size());
  return seqRegistry[seqID].endInstID;
}

const llvm::ArrayRef<ShadowInfo>
ExecBlock::getShadowByInst(uint16_t instID) const {
  QBDI_REQUIRE(instID < instRegistry.size());
  QBDI_REQUIRE(instRegistry[instID].shadowOffset <= shadowRegistry.size());
  QBDI_REQUIRE(instRegistry[instID].shadowOffset +
                   instRegistry[instID].shadowSize <=
               shadowRegistry.size());

  if (instRegistry[instID].shadowOffset == shadowRegistry.size()) {
    return llvm::ArrayRef<ShadowInfo>{};
  } else {
    return llvm::ArrayRef<ShadowInfo>{
        &shadowRegistry[instRegistry[instID].shadowOffset],
        instRegistry[instID].shadowSize};
  }
}

std::vector<ShadowInfo> ExecBlock::queryShadowByInst(uint16_t instID,
                                                     uint16_t tag) const {
  std::vector<ShadowInfo> result;

  for (const auto &reg : shadowRegistry) {
    if ((instID == ANY || reg.instID == instID) &&
        (tag == ANY || reg.tag == tag)) {
      result.push_back(reg);
    }
  }

  return result;
}

std::vector<ShadowInfo> ExecBlock::queryShadowBySeq(uint16_t seqID,
                                                    uint16_t tag) const {
  std::vector<ShadowInfo> result;

  if (seqID == ANY) {
    for (const auto &reg : shadowRegistry) {
      if (tag == ANY || reg.tag == tag) {
        result.push_back(reg);
      }
    }
  } else {
    uint16_t firstInstID = getSeqStart(seqID);
    uint16_t lastInstID = getSeqEnd(seqID);
    for (const auto &reg : shadowRegistry) {
      if (firstInstID <= reg.instID && reg.instID <= lastInstID &&
          (tag == ANY || reg.tag == tag)) {
        result.push_back(reg);
      }
    }
  }

  return result;
}

const llvm::ArrayRef<TagInfo> ExecBlock::getTagByInst(uint16_t instID) const {
  QBDI_REQUIRE(instID < instRegistry.size());
  QBDI_REQUIRE(instRegistry[instID].tagOffset <= tagRegistry.size());
  QBDI_REQUIRE(instRegistry[instID].tagOffset + instRegistry[instID].tagSize <=
               tagRegistry.size());

  if (instRegistry[instID].tagOffset == tagRegistry.size()) {
    return llvm::ArrayRef<TagInfo>{};
  } else {
    return llvm::ArrayRef<TagInfo>{&tagRegistry[instRegistry[instID].tagOffset],
                                   instRegistry[instID].tagSize};
  }
}

std::vector<TagInfo> ExecBlock::queryTagByInst(uint16_t instID,
                                               uint16_t tag) const {
  std::vector<TagInfo> result;

  for (const auto &reg : getTagByInst(instID)) {
    if (reg.tag == tag) {
      result.push_back(reg);
    }
  }

  return result;
}

float ExecBlock::occupationRatio() const {
  return static_cast<float>(codeBlock.allocatedSize() - getEpilogueOffset()) /
         static_cast<float>(codeBlock.allocatedSize());
}

const LLVMCPU &ExecBlock::getLLVMCPUByInst(uint16_t instID) const {
  QBDI_REQUIRE(instID < instRegistry.size());
  return llvmCPUs.getCPU(instMetadata[instID].cpuMode);
}

} // namespace QBDI
