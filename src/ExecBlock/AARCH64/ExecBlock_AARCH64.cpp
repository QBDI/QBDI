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
#include <memory>
#include <stdint.h>
#include <vector>

#include "llvm/Support/Memory.h"

#include "QBDI/Config.h"
#include "QBDI/State.h"
#include "QBDI/VM.h"
#include "Engine/LLVMCPU.h"
#include "ExecBlock/AARCH64/Context_AARCH64.h"
#include "ExecBlock/ExecBlock.h"
#include "Patch/AARCH64/ExecBlockPatch_AARCH64.h"
#include "Patch/Patch.h"
#include "Patch/RelocatableInst.h"
#include "Utility/LogSys.h"

#if defined(QBDI_PLATFORM_WINDOWS)
extern "C" void qbdi_runCodeBlock(void *codeBlock, QBDI::rword execflags);
#else
extern void qbdi_runCodeBlock(void *codeBlock,
                              QBDI::rword execflags) asm("__qbdi_runCodeBlock");
#endif

namespace QBDI {

static const uint32_t MINIMAL_BLOCK_SIZE = 0xc;

void ExecBlock::selectSeq(uint16_t seqID) {
  QBDI_REQUIRE(seqID < seqRegistry.size());
  currentSeq = seqID;
  currentInst = seqRegistry[currentSeq].startInstID;
  context->hostState.currentSROffset =
      seqRegistry[currentSeq].sr.scratchRegisterOffset;
  context->hostState.selector =
      reinterpret_cast<rword>(codeBlock.base()) +
      static_cast<rword>(instRegistry[currentInst].offset);
  context->hostState.executeFlags = seqRegistry[currentSeq].executeFlags;
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

  // Set BaseValue in ScratchRegister
  context->hostState.scratchRegisterValue =
      QBDI_GPR_GET(&context->gprState, context->hostState.currentSROffset);
  QBDI_GPR_SET(&context->gprState, context->hostState.currentSROffset,
               getDataBlockBase());

  // Restore errno and Execute
  if (not llvmCPUs.hasOptions(Options::OPT_DISABLE_ERRNO_BACKUP)) {
    errno = vminstance->getErrno();
    qbdi_runCodeBlock(codeBlock.base(), context->hostState.executeFlags);
    vminstance->setErrno(errno);
  } else {
    qbdi_runCodeBlock(codeBlock.base(), context->hostState.executeFlags);
  }

  // Restore
  QBDI_GPR_SET(&context->gprState, context->hostState.currentSROffset,
               context->hostState.scratchRegisterValue);
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

  // backup the current thumbScratchRegister. If the patch need a new SR, but
  // cannot be apply, we must restore it to write the Terminator and the
  // JmpEpilogue
  ScratchRegisterInfo backupSR = srInfo;

  // if the patch is the first Patch that doesn't not use the current SR,
  // Don't JIT it and end the sequence now
  if (&p == srInfo.endSRPatch) {
    QBDI_DEBUG("Change the ScratchRegister (old : {})",
               llvmcpu.getRegisterName(backupSR.writeScratchRegister));
    initScratchRegisterForPatch(seqCurrent, seqEnd);
    if (not applyRelocatedInst(
            changeScratchRegister(llvmcpu, backupSR.writeScratchRegister,
                                  srInfo.writeScratchRegister),
            &tagRegistry, llvmcpu, MINIMAL_BLOCK_SIZE)) {
      QBDI_DEBUG("Not enough space left: rollback");
      srInfo = backupSR;
      return false;
    }
  }

  if (not applyRelocatedInst(p.insts, &tagRegistry, llvmcpu,
                             MINIMAL_BLOCK_SIZE)) {
    QBDI_DEBUG("Not enough space left: rollback");
    srInfo = backupSR;
    return false;
  }
  return true;
}

void ExecBlock::initScratchRegisterForPatch(
    std::vector<Patch>::const_iterator seqStart,
    std::vector<Patch>::const_iterator seqEnd) {

  std::set<RegLLVM> freeRegister(&GPR_ID[0], &GPR_ID[AVAILABLE_GPR]);

  if constexpr (is_osx) {
    // x18 is reserved by the platform, we can used it
    freeRegister.erase(GPR_ID[18]);
  }

  auto seqIt = seqStart;
  bool splitSequence = false;
  while (seqIt != seqEnd) {
    // compute the reminding free register after the current instruction
    // 1. remove the register used by the TempManager
    std::set<RegLLVM> delta;
    std::set_difference(freeRegister.begin(), freeRegister.end(),
                        seqIt->tempReg.begin(), seqIt->tempReg.end(),
                        std::inserter(delta, delta.end()));

    // 2. remove the register used by the instruction
    for (unsigned i = 0; i < AVAILABLE_GPR; i++) {
      if (seqIt->regUsage[i] != 0) {
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
  srInfo.writeScratchRegister =
      *std::max_element(freeRegister.begin(), freeRegister.end());

  QBDI_DEBUG_BLOCK({
    const LLVMCPU &llvmcpu = llvmCPUs.getCPU(seqStart->metadata.cpuMode);
    QBDI_DEBUG("Select {} as Scratch Register",
               llvmcpu.getRegisterName(srInfo.writeScratchRegister));
  });
}

void ExecBlock::finalizeScratchRegisterForPatch() {
  instRegistry.back().sr.scratchRegisterOffset =
      getGPRPosition(srInfo.writeScratchRegister);
}

} // namespace QBDI
