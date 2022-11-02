
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
#include <sstream>

#include "QBDI/State.h"
#include "Engine/LLVMCPU.h"
#include "ExecBlock/ARM/Context_ARM.h"
#include "ExecBlock/ExecBlock.h"
#include "Patch/ARM/ExecBlockPatch_ARM.h"
#include "Patch/Patch.h"
#include "Patch/RelocatableInst.h"
#include "Utility/LogSys.h"

extern void qbdi_runCodeBlock(void *codeBlock,
                              QBDI::rword execflags) asm("__qbdi_runCodeBlock")
    __attribute__((target("arm")));

namespace QBDI {

static const uint32_t MINIMAL_BLOCK_SIZE = 0x1c;

void ExecBlock::selectSeq(uint16_t seqID) {
  QBDI_REQUIRE(seqID < seqRegistry.size());
  currentSeq = seqID;
  currentInst = seqRegistry[currentSeq].startInstID;
  srInfo.cpuMode = seqRegistry[currentSeq].cpuMode;
  rword selector = reinterpret_cast<rword>(codeBlock.base()) +
                   static_cast<rword>(instRegistry[currentInst].offset);
  if (srInfo.cpuMode == CPUMode::Thumb) {
    context->hostState.currentSROffset =
        seqRegistry[currentSeq].sr.scratchRegisterOffset;
    selector |= 1;
  } else {
    selector &= (~1);
  }
  context->hostState.selector = selector;
  context->hostState.executeFlags = seqRegistry[currentSeq].executeFlags;
  context->hostState.exchange = static_cast<rword>(0);
}

void ExecBlock::run() {
  // Pages are RWX on iOS
  if constexpr (is_ios) {
    llvm::sys::Memory::InvalidateInstructionCache(codeBlock.base(),
                                                  codeBlock.allocatedSize());
  } else {
    if (not isRX()) {
      makeRX();
    }
  }

  if (srInfo.cpuMode == CPUMode::Thumb) {
    // Set BaseValue in ScratchRegister
    context->hostState.scratchRegisterValue =
        QBDI_GPR_GET(&context->gprState, context->hostState.currentSROffset);
    QBDI_GPR_SET(&context->gprState, context->hostState.currentSROffset,
                 getDataBlockBase());

    // Execute
    qbdi_runCodeBlock(codeBlock.base(), context->hostState.executeFlags);

    // Restore
    QBDI_GPR_SET(&context->gprState, context->hostState.currentSROffset,
                 context->hostState.scratchRegisterValue);
  } else {
    // Execute
    qbdi_runCodeBlock(codeBlock.base(), context->hostState.executeFlags);
  }
}

bool ExecBlock::writePatch(std::vector<Patch>::const_iterator seqCurrent,
                           std::vector<Patch>::const_iterator seqEnd,
                           const LLVMCPU &llvmcpu) {
  const Patch &p = *seqCurrent;
  QBDI_REQUIRE(p.finalize);

  if (getEpilogueOffset() <= MINIMAL_BLOCK_SIZE) {
    isFull = true;
    return false;
  }
  if (llvmcpu == CPUMode::ARM) {
    codeStream.align(4);
    // the code should be able to allocate and access to at least one shadow
    // In ARM mode, LDR can have a range of -4095 to 4095. As PC is always read
    // as PC+8, we should be able to access the first available shadow with
    // PC+8+4092.
    while (codeStream.current_pos() <
           (sizeof(Context) - 4 + sizeof(rword) * shadowIdx)) {
      // In ARM, [0, 0, 0, 0] is a NOP instruction : andeq r0, r0, r0
      codeStream.write_zeros(4);
    }
  }

  // backup the current thumbScratchRegister. If the patch need a new SR, but
  // cannot be apply, we must restore it to write the Terminator and the
  // JmpEpilogue
  ScratchRegisterInfo backupSR = srInfo;

  // if the patch is the first Patch that doesn't not use the current SR,
  // Don't JIT it and end the sequence now
  if (llvmcpu == CPUMode::Thumb and &p == srInfo.endSRPatch) {
    QBDI_DEBUG("Change the ScratchRegister (old : {})",
               llvmcpu.getRegisterName(backupSR.thumbScratchRegister));
    initScratchRegisterForPatch(seqCurrent, seqEnd);
    if (not applyRelocatedInst(
            changeScratchRegister(llvmcpu, backupSR.thumbScratchRegister,
                                  srInfo.thumbScratchRegister),
            &tagRegistry, llvmcpu, MINIMAL_BLOCK_SIZE + epilogueSize)) {
      QBDI_DEBUG("Not enough space left: rollback");
      srInfo = backupSR;
      return false;
    }
  }

  if (not applyRelocatedInst(p.insts, &tagRegistry, llvmcpu,
                             MINIMAL_BLOCK_SIZE + epilogueSize)) {
    QBDI_DEBUG("Not enough space left: rollback");
    srInfo = backupSR;
    return false;
  }
  return true;
}

void ExecBlock::initScratchRegisterForPatch(
    std::vector<Patch>::const_iterator seqStart,
    std::vector<Patch>::const_iterator seqEnd) {

  if (seqStart->metadata.cpuMode == CPUMode::ARM) {
    // no need of ScratchRegister.
    srInfo.thumbScratchRegister = GPR_ID[0];
    srInfo.endSRPatch = nullptr;
    return;
  }

  std::set<RegLLVM> freeRegister(&GPR_ID[0], &GPR_ID[AVAILABLE_GPR]);

  auto seqIt = seqStart;
  bool splitSequence = false;
  while (seqIt != seqEnd) {
    // compute the reminding free register after the current instruction
    // 1. remove the register used by the TempManager
    std::set<RegLLVM> delta;
    std::set_difference(freeRegister.begin(), freeRegister.end(),
                        seqIt->tempReg.begin(), seqIt->tempReg.end(),
                        std::inserter(delta, delta.end()));

    // 2. remove the register used by the instruction and not allow to be a
    //    scratch register (case with LDM/STM instruction)
    for (unsigned i = 0; i < AVAILABLE_GPR; i++) {
      if ((seqIt->regUsage[i] & RegisterUsage::RegisterBoth) != 0 and
          (seqIt->regUsage[i] & RegisterUsage::RegisterSavedScratch) == 0) {
        delta.erase(GPR_ID[i]);
      }
    }

    // check if some free register remain.
    if (delta.empty()) {
      splitSequence = true;
      break;
    } else {
      freeRegister = std::move(delta);
      seqIt++;
    }
  }

  QBDI_REQUIRE_ABORT(not freeRegister.empty(),
                     "ScratchRegister internal error");
  QBDI_REQUIRE_ABORT(seqIt != seqStart, "ScratchRegister internal error");

  // Set the position to split the sequence if a global scratch register is not
  // avaialble
  if (splitSequence) {
    QBDI_DEBUG(
        "No Scratch register found for the whole sequence."
        "Split at a distance of {} / {}",
        std::distance(seqStart, seqIt), std::distance(seqStart, seqEnd));
    srInfo.endSRPatch = &*seqIt;
  } else {
    QBDI_REQUIRE_ABORT(seqIt == seqEnd, "ScratchRegister internal error");
    QBDI_DEBUG("Scratch register found for the {} Patch.",
               std::distance(seqStart, seqEnd));
    srInfo.endSRPatch = nullptr;
  }

  // get a free register
  // In order to improve the debug, we select the higher free register
  srInfo.thumbScratchRegister =
      *std::max_element(freeRegister.begin(), freeRegister.end());

  QBDI_DEBUG_BLOCK({
    const LLVMCPU &llvmcpu = llvmCPUs.getCPU(seqStart->metadata.cpuMode);
    QBDI_DEBUG("Select {} as Scratch Register",
               llvmcpu.getRegisterName(srInfo.thumbScratchRegister));
  });
}

void ExecBlock::finalizeScratchRegisterForPatch() {
  instRegistry.back().sr.scratchRegisterOffset =
      getGPRPosition(srInfo.thumbScratchRegister);
}

} // namespace QBDI
