
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
#include <errno.h>
#include <memory>
#include <stdint.h>
#include <vector>

#include "llvm/Support/Memory.h"

#include "QBDI/Config.h"
#include "QBDI/State.h"
#include "QBDI/VM.h"
#include "Engine/LLVMCPU.h"
#include "ExecBlock/ExecBlock.h"
#include "ExecBlock/X86_64/Context_X86_64.h"
#include "Patch/Patch.h"
#include "Patch/RelocatableInst.h"
#include "Utility/LogSys.h"
#include "Utility/System.h"

#if defined(QBDI_PLATFORM_WINDOWS)
extern "C" void qbdi_runCodeBlock(void *codeBlock, QBDI::rword execflags);
#else
extern void qbdi_runCodeBlock(void *codeBlock,
                              QBDI::rword execflags) asm("__qbdi_runCodeBlock");
#endif

static const uint32_t MINIMAL_BLOCK_SIZE = 64;

namespace QBDI {

void ExecBlock::selectSeq(uint16_t seqID) {
  QBDI_REQUIRE(seqID < seqRegistry.size());
  currentSeq = seqID;
  currentInst = seqRegistry[currentSeq].startInstID;
  context->hostState.selector =
      reinterpret_cast<rword>(codeBlock.base()) +
      static_cast<rword>(instRegistry[currentInst].offset);
  context->hostState.executeFlags = seqRegistry[currentSeq].executeFlags;
}

void ExecBlock::run() {
  if constexpr (is_ios) {
    if (isRWRXSupported()) {
      if (not isRX()) {
        makeRX();
      }
    } else {
      // Pages are RWX on iOS
      llvm::sys::Memory::InvalidateInstructionCache(codeBlock.base(),
                                                    codeBlock.allocatedSize());
    }
  } else {
    if (not isRX()) {
      makeRX();
    }
  }
  if (not llvmCPUs.hasOptions(Options::OPT_DISABLE_ERRNO_BACKUP)) {
    errno = vminstance->getErrno();
    qbdi_runCodeBlock(codeBlock.base(), context->hostState.executeFlags);
    vminstance->setErrno(errno);
  } else {
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

  if (not applyRelocatedInst(p.insts, &tagRegistry, llvmcpu,
                             MINIMAL_BLOCK_SIZE)) {
    QBDI_DEBUG("Not enough space left: rollback");
    return false;
  }
  return true;
}

void ExecBlock::initScratchRegisterForPatch(
    std::vector<Patch>::const_iterator seqStart,
    std::vector<Patch>::const_iterator seqEnd) {}

void ExecBlock::finalizeScratchRegisterForPatch() {}

} // namespace QBDI
