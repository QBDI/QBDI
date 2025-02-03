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
#ifndef CONTEXT_ARM_H
#define CONTEXT_ARM_H

#include "QBDI/State.h"

namespace QBDI {

/*! ARM Host context.
 */
struct QBDI_ALIGNED(4) HostState {
  rword sp;
  rword selector;
  rword callback;
  rword data;
  rword origin;
  rword exchange;
  rword executeFlags;
  // for thumb only
  rword scratchRegisterValue;
  rword currentSROffset;
};

/*! ARM Execution context.
 */
struct QBDI_ALIGNED(4) Context {
  // hostState needs to be first for relative addressing range reasons
  HostState hostState;
  FPRState fprState;
  GPRState gprState;
};

} // namespace QBDI

#endif // CONTEXT_ARM_H
