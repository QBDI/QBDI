/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2026 Quarkslab
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

bool InITBlock::test(const Patch &patch, const LLVMCPU &llvmcpu) const {
  return patch.metadata.archMetadata.posITblock > 0;
}

bool LastInITBlock::test(const Patch &patch, const LLVMCPU &llvmcpu) const {
  return patch.metadata.archMetadata.posITblock == 1;
}

} // namespace QBDI
