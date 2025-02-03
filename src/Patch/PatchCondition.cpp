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
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"

#include "Engine/LLVMCPU.h"
#include "Patch/InstInfo.h"
#include "Patch/PatchCondition.h"
#include "Utility/String.h"

namespace QBDI {

bool MnemonicIs::test(const Patch &patch, const LLVMCPU &llvmcpu) const {
  return QBDI::String::startsWith(
      mnemonic.c_str(), llvmcpu.getInstOpcodeName(patch.metadata.inst));
}

bool OpIs::test(const Patch &patch, const LLVMCPU &llvmcpu) const {
  return patch.metadata.inst.getOpcode() == op;
}

bool UseReg::test(const Patch &patch, const LLVMCPU &llvmcpu) const {
  for (unsigned int i = 0; i < patch.metadata.inst.getNumOperands(); i++) {
    const llvm::MCOperand &op = patch.metadata.inst.getOperand(i);
    if (op.isReg() && op.getReg() == ((RegLLVM)reg)) {
      return true;
    }
  }
  return false;
}

bool DoesReadAccess::test(const Patch &patch, const LLVMCPU &llvmcpu) const {
  return getReadSize(patch.metadata.inst, llvmcpu) > 0;
}

bool DoesWriteAccess::test(const Patch &patch, const LLVMCPU &llvmcpu) const {
  return getWriteSize(patch.metadata.inst, llvmcpu) > 0;
}

bool HasOptions::test(const Patch &patch, const LLVMCPU &llvmcpu) const {
  return llvmcpu.hasOptions(opts);
}

} // namespace QBDI
