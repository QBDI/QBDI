/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2023 Quarkslab
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
#include <stddef.h>
#include <stdlib.h>

#include "ExecBlock/Context.h"
#include "Patch/AARCH64/Layer2_AARCH64.h"
#include "Patch/AARCH64/PatchGenerator_AARCH64.h"
#include "Patch/AARCH64/RelocatableInst_AARCH64.h"
#include "Patch/InstrRules.h"
#include "Patch/Patch.h"
#include "Patch/PatchGenerator.h"
#include "Patch/RelocatableInst.h"
#include "Patch/Types.h"
#include "Utility/LogSys.h"

namespace QBDI {

/* Genreate a series of RelocatableInst which when appended to an
 * instrumentation code trigger a break to host. It receive in argument a
 * temporary reg which will be used for computations then finally restored.
 */
RelocatableInst::UniquePtrVec getBreakToHost(Reg temp, const Patch &patch,
                                             bool restore) {
  RelocatableInst::UniquePtrVec breakToHost;

  size_t patchSize = 12;
  if (restore) {
    patchSize += 4;
  }

  // compute address after JmpEpilogue
  breakToHost.push_back(Adr(temp, patchSize));
  // set address in hostState.selector
  append(breakToHost,
         SaveReg(temp, Offset(offsetof(Context, hostState.selector)))
             .genReloc(*patch.llvmcpu));
  if (restore) {
    // Restore the temporary register
    append(breakToHost, LoadReg(temp, Offset(temp)).genReloc(*patch.llvmcpu));
  }

  append(breakToHost, JmpEpilogue().genReloc(*patch.llvmcpu));

  // add target when callback return CONTINUE
  append(breakToHost, TargetPrologue().genReloc(patch));

  return breakToHost;
}

} // namespace QBDI
