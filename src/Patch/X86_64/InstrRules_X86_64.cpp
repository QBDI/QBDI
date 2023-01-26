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
#include "Patch/InstrRules.h"
#include "Patch/Patch.h"
#include "Patch/PatchGenerator.h"
#include "Patch/RelocatableInst.h"
#include "Patch/Types.h"
#include "Patch/X86_64/Layer2_X86_64.h"
#include "Patch/X86_64/RelocatableInst_X86_64.h"

#include "QBDI/Config.h"
#include "Utility/LogSys.h"

namespace QBDI {
class Patch;

/* Generate a series of RelocatableInst which when appended to an
 * instrumentation code trigger a break to host. It receive in argument a
 * temporary reg which will be used for computations then finally restored.
 */
RelocatableInst::UniquePtrVec getBreakToHost(Reg temp, const Patch &patch,
                                             bool restore) {
  RelocatableInst::UniquePtrVec breakToHost;

  QBDI_REQUIRE_ABORT(restore, "X86 don't have a temporary register");

  // Use the temporary register to compute RIP + offset which is the address
  // which will follow this patch and where the execution needs to be resumed
  if constexpr (is_x86)
    breakToHost.push_back(SetRegtoPCRel::unique(temp, 22));
  else
    breakToHost.push_back(Add(temp, Reg(REG_PC), 19));
  // Set the selector to this address so the execution can be resumed when the
  // exec block will be reexecuted
  append(breakToHost,
         SaveReg(temp, Offset(offsetof(Context, hostState.selector)))
             .genReloc(*patch.llvmcpu));
  // Restore the temporary register
  append(breakToHost, LoadReg(temp, Offset(temp)).genReloc(*patch.llvmcpu));
  // Jump to the epilogue to break to the host
  append(breakToHost, JmpEpilogue().genReloc(*patch.llvmcpu));

  // add target when callback return CONTINUE
  append(breakToHost, TargetPrologue().genReloc(patch));

  return breakToHost;
}

} // namespace QBDI
