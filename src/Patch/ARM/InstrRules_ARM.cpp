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
#include <stddef.h>
#include <stdlib.h>

#include "ExecBlock/Context.h"
#include "Patch/ARM/Layer2_ARM.h"
#include "Patch/ARM/PatchGenerator_ARM.h"
#include "Patch/ARM/RelocatableInst_ARM.h"
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

  QBDI_REQUIRE_ABORT(restore, "ARM don't have a temporary register {}", patch);

  // compute offset address
  unsigned int patchSize = RelativeAddress(temp, 0xff).getSize(*patch.llvmcpu);

  RelocatableInst::UniquePtrVec tmpVec;
  append(tmpVec, SaveReg(temp, Offset(offsetof(Context, hostState.selector)))
                     .genReloc(*patch.llvmcpu));
  append(tmpVec, LoadReg(temp, Offset(temp)).genReloc(*patch.llvmcpu));
  append(tmpVec, JmpEpilogue().genReloc(*patch.llvmcpu));

  patchSize += getUniquePtrVecSize(tmpVec, *patch.llvmcpu);
  if (patch.metadata.cpuMode == CPUMode::Thumb) {
    patchSize += 1;
  }

  RelocatableInst::UniquePtrVec breakToHost;

  // set temp to the address after JmpEpilogue
  breakToHost.push_back(RelativeAddress::unique(temp, patchSize));

  // set address in hostState.selector
  // Restore the temporary register
  // JumpEpilogue
  append(breakToHost, std::move(tmpVec));

  // add target when callback return CONTINUE
  append(breakToHost, TargetPrologue().genReloc(patch));

  return breakToHost;
}

} // namespace QBDI
