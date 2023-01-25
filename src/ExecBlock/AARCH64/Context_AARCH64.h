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
#ifndef CONTEXT_AARCH64_H
#define CONTEXT_AARCH64_H

#include "QBDI/State.h"

namespace QBDI {

/*! AArch64 Host context.
 */
struct QBDI_ALIGNED(8) HostState {
  // backup of the scratch Register
  rword scratchRegisterValue;
  rword currentSROffset;
  // prologue epilogue backup
  rword sp;
  // jump at the end of prologue
  rword selector;
  // parameter for InstCallback
  // also used by the ExecBroker
  union {
    rword callback;
    rword brokerAddr;
  };
  rword data;
  rword origin;
  rword tpidr;
  // unused
  rword executeFlags;
};

/*! AArch64 Execution context.
 */
struct QBDI_ALIGNED(8) Context {
  // hostState needs to be first for relative addressing range reasons
  HostState hostState;
  GPRState gprState;
  FPRState fprState;
};

} // namespace QBDI

#endif // CONTEXT_AARCH64_H
