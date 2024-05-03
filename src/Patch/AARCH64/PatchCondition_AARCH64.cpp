/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
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
#include "Patch/AARCH64/PatchCondition_AARCH64.h"
#include "Engine/LLVMCPU.h"
#include "Patch/AARCH64/InstInfo_AARCH64.h"
#include "Patch/InstInfo.h"
#include "Utility/LogSys.h"

namespace QBDI {

bool IsMOPSReadPrologue::test(const Patch &patch,
                              const LLVMCPU &llvmcpu) const {
  return isMOPSPrologue(patch.metadata.inst) and
         unsupportedRead(patch.metadata.inst);
}

bool IsMOPSWritePrologue::test(const Patch &patch,
                               const LLVMCPU &llvmcpu) const {
  return isMOPSPrologue(patch.metadata.inst) and
         unsupportedWrite(patch.metadata.inst);
}

} // namespace QBDI
