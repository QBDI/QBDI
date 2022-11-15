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
#include "QBDI/State.h"
#include "ExecBlock/Context.h"
#include "ExecBlock/ExecBlock.h"
#include "ExecBroker/ExecBroker.h"
#include "Patch/ExecBlockFlags.h"
#include "Utility/LogSys.h"

namespace QBDI {

void ExecBroker::initExecBrokerSequences(const LLVMCPUs &llvmCPUs) {}

rword *ExecBroker::getReturnPoint(GPRState *gprState) const {
  static int SCAN_DISTANCE = 3;
  rword *ptr = reinterpret_cast<rword *>(gprState->sp);

  if (isInstrumented(gprState->lr)) {
    QBDI_DEBUG("Found instrumented return address in LR register");
    return &(gprState->lr);
  }

  for (int i = 0; i < SCAN_DISTANCE; i++) {
    if (isInstrumented(ptr[i])) {
      QBDI_DEBUG("Found instrumented return address on the stack at {:p}",
                 reinterpret_cast<void *>(&(ptr[i])));
      return &(ptr[i]);
    }
  }
  QBDI_DEBUG("No instrumented return address found on the stack (lr : 0x{:x})",
             gprState->lr);
  return nullptr;
}

bool ExecBroker::transferExecution(rword addr, GPRState *gprState,
                                   FPRState *fprState) {
  rword hook = 0;

  // Backup / Patch return address
  hook = transferBlock->getCurrentPC() + transferBlock->getEpilogueOffset();
  rword *ptr = getReturnPoint(gprState);
  rword hookedAddress = *ptr;
  *ptr = hook;
  QBDI_DEBUG(
      "TransferExecution: Patched {:} hooking return address 0x{:06x} with "
      "0x{:06x}",
      reinterpret_cast<void *>(ptr), hookedAddress, hook);

  // Write transfer state
  transferBlock->getContext()->gprState = *gprState;
  transferBlock->getContext()->fprState = *fprState;
  transferBlock->getContext()->hostState.selector = addr;
  transferBlock->getContext()->hostState.executeFlags = defaultExecuteFlags;
  // Execute transfer
  QBDI_DEBUG("Transfering execution to 0x{:x} using transferBlock 0x{:x}", addr,
             reinterpret_cast<uintptr_t>(&*transferBlock));

  transferBlock->run();

  // Read transfer result
  *gprState = transferBlock->getContext()->gprState;
  *fprState = transferBlock->getContext()->fprState;

  // Restore original return
  QBDI_GPR_SET(gprState, REG_PC, hookedAddress);

  return true;
}

} // namespace QBDI
