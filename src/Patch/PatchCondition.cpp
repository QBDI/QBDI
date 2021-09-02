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
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"

#include "Engine/LLVMCPU.h"
#include "Patch/InstInfo.h"
#include "Patch/PatchCondition.h"
#include "Utility/String.h"

namespace QBDI {

bool MnemonicIs::test(const llvm::MCInst &inst, rword address, rword instSize,
                      const LLVMCPU &llvmcpu) const {
  return QBDI::String::startsWith(
      mnemonic.c_str(), llvmcpu.getMCII().getName(inst.getOpcode()).data());
}

bool OpIs::test(const llvm::MCInst &inst, rword address, rword instSize,
                const LLVMCPU &llvmcpu) const {
  return inst.getOpcode() == op;
}

bool UseReg::test(const llvm::MCInst &inst, rword address, rword instSize,
                  const LLVMCPU &llvmcpu) const {
  for (unsigned int i = 0; i < inst.getNumOperands(); i++) {
    const llvm::MCOperand &op = inst.getOperand(i);
    if (op.isReg() && op.getReg() == (unsigned int)reg) {
      return true;
    }
  }
  return false;
}

bool DoesReadAccess::test(const llvm::MCInst &inst, rword address,
                          rword instSize, const LLVMCPU &llvmcpu) const {
  return getReadSize(inst) > 0;
}

bool DoesWriteAccess::test(const llvm::MCInst &inst, rword address,
                           rword instSize, const LLVMCPU &llvmcpu) const {
  return getWriteSize(inst) > 0;
}

} // namespace QBDI
