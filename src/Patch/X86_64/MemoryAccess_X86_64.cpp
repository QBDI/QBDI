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
#include <memory>
#include <stddef.h>
#include <stdint.h>
#include <utility>
#include <vector>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"

#include "Engine/LLVMCPU.h"
#include "ExecBlock/ExecBlock.h"
#include "Patch/InstInfo.h"
#include "Patch/InstMetadata.h"
#include "Patch/InstrRule.h"
#include "Patch/MemoryAccess.h"
#include "Patch/Patch.h"
#include "Patch/PatchCondition.h"
#include "Patch/PatchGenerator.h"
#include "Patch/PatchUtils.h"
#include "Patch/Types.h"
#include "Patch/X86_64/InstInfo_X86_64.h"
#include "Patch/X86_64/PatchGenerator_X86_64.h"
#include "Utility/LogSys.h"

#include "QBDI/Bitmask.h"
#include "QBDI/Callback.h"
#include "QBDI/State.h"

namespace llvm {
class MCInstrDesc;
}

namespace QBDI {

enum MemoryTag : uint16_t {
  MEM_READ_ADDRESS_TAG = MEMORY_TAG_BEGIN + 0,
  MEM_WRITE_ADDRESS_TAG = MEMORY_TAG_BEGIN + 1,

  MEM_READ_VALUE_TAG = MEMORY_TAG_BEGIN + 2,
  MEM_WRITE_VALUE_TAG = MEMORY_TAG_BEGIN + 3,

  MEM_READ_0_BEGIN_ADDRESS_TAG = MEMORY_TAG_BEGIN + 4,
  MEM_READ_1_BEGIN_ADDRESS_TAG = MEMORY_TAG_BEGIN + 5,
  MEM_WRITE_BEGIN_ADDRESS_TAG = MEMORY_TAG_BEGIN + 6,

  MEM_READ_0_END_ADDRESS_TAG = MEMORY_TAG_BEGIN + 7,
  MEM_READ_1_END_ADDRESS_TAG = MEMORY_TAG_BEGIN + 8,
  MEM_WRITE_END_ADDRESS_TAG = MEMORY_TAG_BEGIN + 9,
};

void analyseMemoryAccessAddrValue(const ExecBlock &curExecBlock,
                                  llvm::ArrayRef<ShadowInfo> &shadows,
                                  std::vector<MemoryAccess> &dest) {
  if (shadows.size() < 1) {
    return;
  }

  auto access = MemoryAccess();
  access.flags = MEMORY_NO_FLAGS;

  uint16_t expectValueTag;
  const llvm::MCInst &inst = curExecBlock.getOriginalMCInst(shadows[0].instID);
  switch (shadows[0].tag) {
    default:
      return;
    case MEM_READ_ADDRESS_TAG:
      access.type = MEMORY_READ;
      access.size = getReadSize(inst);
      expectValueTag = MEM_READ_VALUE_TAG;
      if (isMinSizeRead(inst)) {
        access.flags |= MEMORY_MINIMUM_SIZE;
      }
      break;
    case MEM_WRITE_ADDRESS_TAG:
      access.type = MEMORY_WRITE;
      access.size = getWriteSize(inst);
      expectValueTag = MEM_WRITE_VALUE_TAG;
      if (isMinSizeRead(inst)) {
        access.flags |= MEMORY_MINIMUM_SIZE;
      }
      break;
  }

  access.accessAddress = curExecBlock.getShadow(shadows[0].shadowID);
  access.instAddress = curExecBlock.getInstAddress(shadows[0].instID);

  if (access.size > sizeof(rword)) {
    access.flags |= MEMORY_UNKNOWN_VALUE;
    access.value = 0;
    dest.push_back(std::move(access));
    return;
  }

  size_t index = 0;
  // search the index of MEM_x_VALUE_TAG. For most instruction, it's the next
  // shadow.
  do {
    index += 1;
    if (index >= shadows.size()) {
      QBDI_ERROR("Not found shadow tag {:x} for instruction {:x}",
                 expectValueTag, access.instAddress);
      return;
    }
    QBDI_REQUIRE_ACTION(shadows[0].instID == shadows[index].instID, return );
  } while (shadows[index].tag != expectValueTag);

  access.value = curExecBlock.getShadow(shadows[index].shadowID);

  dest.push_back(std::move(access));
}

void analyseMemoryAccessAddrRange(const ExecBlock &curExecBlock,
                                  llvm::ArrayRef<ShadowInfo> &shadows,
                                  bool postInst,
                                  std::vector<MemoryAccess> &dest) {
  if (shadows.size() < 1) {
    return;
  }
  auto access = MemoryAccess();
  access.flags = MEMORY_NO_FLAGS;

  uint16_t expectValueTag;
  unsigned accessAtomicSize;
  switch (shadows[0].tag) {
    default:
      return;
    case MEM_READ_0_BEGIN_ADDRESS_TAG:
      access.type = MEMORY_READ;
      expectValueTag = MEM_READ_0_END_ADDRESS_TAG;
      accessAtomicSize =
          getReadSize(curExecBlock.getOriginalMCInst(shadows[0].instID));
      break;
    case MEM_READ_1_BEGIN_ADDRESS_TAG:
      access.type = MEMORY_READ;
      expectValueTag = MEM_READ_1_END_ADDRESS_TAG;
      accessAtomicSize =
          getReadSize(curExecBlock.getOriginalMCInst(shadows[0].instID));
      break;
    case MEM_WRITE_BEGIN_ADDRESS_TAG:
      access.type = MEMORY_WRITE;
      expectValueTag = MEM_WRITE_END_ADDRESS_TAG;
      accessAtomicSize =
          getWriteSize(curExecBlock.getOriginalMCInst(shadows[0].instID));
      break;
  }

  access.instAddress = curExecBlock.getInstAddress(shadows[0].instID);
  access.flags |= MEMORY_UNKNOWN_VALUE;
  access.value = 0;

  if (!postInst) {
    access.accessAddress = curExecBlock.getShadow(shadows[0].shadowID);
    access.flags |= MEMORY_UNKNOWN_SIZE;
    access.size = 0;
    dest.push_back(std::move(access));
    return;
  }

  size_t index = 0;
  // search the index of MEM_x_VALUE_TAG. For most instruction, it's the next
  // shadow.
  do {
    index += 1;
    if (index >= shadows.size()) {
      QBDI_ERROR("Not found shadow tag {:x} for instruction {:x}",
                 expectValueTag, access.instAddress);
      return;
    }
    QBDI_REQUIRE_ACTION(shadows[0].instID == shadows[index].instID, return );
  } while (shadows[index].tag != expectValueTag);

  rword beginAddress = curExecBlock.getShadow(shadows[0].shadowID);
  rword endAddress = curExecBlock.getShadow(shadows[index].shadowID);

  if (endAddress >= beginAddress) {
    access.accessAddress = beginAddress;
    access.size = endAddress - beginAddress;
  } else {
    // the endAddress is lesser than the begin address, this may be the case
    // in X86 with REP prefix and DF=1
    // In this case, the memory have been access between [endAddress +
    // accessSize, beginAddress + accessAtomicSize)
    access.accessAddress = endAddress + accessAtomicSize;
    access.size = beginAddress - endAddress;
  }

  dest.push_back(std::move(access));
}

void analyseMemoryAccess(const ExecBlock &curExecBlock, uint16_t instID,
                         bool afterInst, std::vector<MemoryAccess> &dest) {

  llvm::ArrayRef<ShadowInfo> shadows = curExecBlock.getShadowByInst(instID);
  QBDI_DEBUG("Got {} shadows for Instruction {:x}", shadows.size(), instID);

  while (!shadows.empty()) {
    QBDI_REQUIRE(shadows[0].instID == instID);

    switch (shadows[0].tag) {
      default:
        break;
      case MEM_READ_ADDRESS_TAG:
        analyseMemoryAccessAddrValue(curExecBlock, shadows, dest);
        break;
      case MEM_WRITE_ADDRESS_TAG:
        if (afterInst) {
          analyseMemoryAccessAddrValue(curExecBlock, shadows, dest);
        }
        break;
      case MEM_READ_0_BEGIN_ADDRESS_TAG:
      case MEM_READ_1_BEGIN_ADDRESS_TAG:
        analyseMemoryAccessAddrRange(curExecBlock, shadows, afterInst, dest);
        break;
      case MEM_WRITE_BEGIN_ADDRESS_TAG:
        if (afterInst) {
          analyseMemoryAccessAddrRange(curExecBlock, shadows, afterInst, dest);
        }
        break;
    }
    shadows = shadows.drop_front();
  }
}

static const PatchGenerator::UniquePtrVec &
generatePreReadInstrumentPatch(Patch &patch, const LLVMCPU &llvmcpu) {

  // REP prefix
  if (hasREPPrefix(patch.metadata.inst)) {
    if (isDoubleRead(patch.metadata.inst)) {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0), 0),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_0_BEGIN_ADDRESS_TAG)),
          GetReadAddress::unique(Temp(0), 1),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_1_BEGIN_ADDRESS_TAG)));
      return r;
    } else {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_0_BEGIN_ADDRESS_TAG)));
      return r;
    }
  }
  // instruction with double read
  else if (isDoubleRead(patch.metadata.inst)) {
    if (getReadSize(patch.metadata.inst) > sizeof(rword)) {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0), 0),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          GetReadAddress::unique(Temp(0), 1),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)));
      return r;
    } else {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0), 0),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          GetReadValue::unique(Temp(0), 0),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_VALUE_TAG)),
          GetReadAddress::unique(Temp(0), 1),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          GetReadValue::unique(Temp(0), 1),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_VALUE_TAG)));
      return r;
    }
  } else {
    if (getReadSize(patch.metadata.inst) > sizeof(rword)) {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)));
      return r;
    } else {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          GetReadValue::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_VALUE_TAG)));
      return r;
    }
  }
}

static const PatchGenerator::UniquePtrVec &
generatePostReadInstrumentPatch(Patch &patch, const LLVMCPU &llvmcpu) {

  // REP prefix
  if (hasREPPrefix(patch.metadata.inst)) {
    if (isDoubleRead(patch.metadata.inst)) {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0), 0),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_0_END_ADDRESS_TAG)),
          GetReadAddress::unique(Temp(0), 1),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_1_END_ADDRESS_TAG)));
      return r;
    } else {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_0_END_ADDRESS_TAG)));
      return r;
    }
  } else {
    static const PatchGenerator::UniquePtrVec r;
    return r;
  }
}

static const PatchGenerator::UniquePtrVec &
generatePreWriteInstrumentPatch(Patch &patch, const LLVMCPU &llvmcpu) {

  const llvm::MCInstrDesc &desc =
      llvmcpu.getMCII().get(patch.metadata.inst.getOpcode());

  if (hasREPPrefix(patch.metadata.inst)) {
    static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
        GetWriteAddress::unique(Temp(0)),
        WriteTemp::unique(Temp(0), Shadow(MEM_WRITE_BEGIN_ADDRESS_TAG)));
    return r;
  }
  // Some instruction need to have the address get before the instruction
  else if (mayChangeWriteAddr(patch.metadata.inst, desc) &&
           !isStackWrite(patch.metadata.inst)) {
    static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
        GetWriteAddress::unique(Temp(0)),
        WriteTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)));
    return r;
  } else {
    static const PatchGenerator::UniquePtrVec r;
    return r;
  }
}

static const PatchGenerator::UniquePtrVec &
generatePostWriteInstrumentPatch(Patch &patch, const LLVMCPU &llvmcpu) {

  const llvm::MCInstrDesc &desc =
      llvmcpu.getMCII().get(patch.metadata.inst.getOpcode());

  if (hasREPPrefix(patch.metadata.inst)) {
    static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
        GetWriteAddress::unique(Temp(0)),
        WriteTemp::unique(Temp(0), Shadow(MEM_WRITE_END_ADDRESS_TAG)));
    return r;
  }
  // Some instruction need to have the address get before the instruction
  else if (mayChangeWriteAddr(patch.metadata.inst, desc) &&
           !isStackWrite(patch.metadata.inst)) {
    if (getWriteSize(patch.metadata.inst) > sizeof(rword)) {
      static const PatchGenerator::UniquePtrVec r;
      return r;
    } else {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
          GetWriteValue::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_WRITE_VALUE_TAG)));
      return r;
    }
  } else {
    if (getWriteSize(patch.metadata.inst) > sizeof(rword)) {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetWriteAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)));
      return r;
    } else {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetWriteAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
          GetWriteValue::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_WRITE_VALUE_TAG)));
      return r;
    }
  }
}

std::vector<std::unique_ptr<InstrRule>> getInstrRuleMemAccessRead() {
  return conv_unique<InstrRule>(
      InstrRuleDynamic::unique(
          DoesReadAccess::unique(), generatePreReadInstrumentPatch, PREINST,
          false, PRIORITY_MEMACCESS_LIMIT + 1, RelocTagPreInstMemAccess),
      InstrRuleDynamic::unique(
          DoesReadAccess::unique(), generatePostReadInstrumentPatch, POSTINST,
          false, PRIORITY_MEMACCESS_LIMIT + 1, RelocTagPostInstMemAccess));
}

std::vector<std::unique_ptr<InstrRule>> getInstrRuleMemAccessWrite() {
  return conv_unique<InstrRule>(
      InstrRuleDynamic::unique(
          DoesWriteAccess::unique(), generatePreWriteInstrumentPatch, PREINST,
          false, PRIORITY_MEMACCESS_LIMIT, RelocTagPreInstMemAccess),
      InstrRuleDynamic::unique(
          DoesWriteAccess::unique(), generatePostWriteInstrumentPatch, POSTINST,
          false, PRIORITY_MEMACCESS_LIMIT, RelocTagPostInstMemAccess));
}

} // namespace QBDI
