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
#include "Target/ARM/Utils/ARMBaseInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"

#include "Engine/LLVMCPU.h"
#include "Patch/ARM/PatchCondition_ARM.h"
#include "Patch/InstInfo.h"
#include "Utility/LogSys.h"

namespace QBDI {

bool HasCond::test(const Patch &patch, const LLVMCPU &llvmcpu) const {
  return patch.metadata.archMetadata.cond != llvm::ARMCC::AL;
}

bool OperandIs::test(const Patch &patch, const LLVMCPU &llvmcpu) const {
  if (position >= patch.metadata.inst.getNumOperands()) {
    return false;
  }
  const llvm::MCOperand &op = patch.metadata.inst.getOperand(position);
  if (type == RegType and op.isReg()) {
    return reg == op.getReg();
  } else if (type == ImmType and op.isImm()) {
    return imm == op.getImm();
  } else {
    // OperandIs can be used before the check of the opcode
    // If the operand doesn't have the good type, return false to skip the
    // current Patchrules.
    return false;
  }
}

bool InITBlock::test(const Patch &patch, const LLVMCPU &llvmcpu) const {
  return patch.metadata.archMetadata.posITblock > 0;
}

bool LastInITBlock::test(const Patch &patch, const LLVMCPU &llvmcpu) const {
  return patch.metadata.archMetadata.posITblock == 1;
}

} // namespace QBDI
