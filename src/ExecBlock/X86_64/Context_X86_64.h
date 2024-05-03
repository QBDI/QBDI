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
#ifndef CONTEXT_X86_64_H
#define CONTEXT_X86_64_H

#include "QBDI/State.h"

namespace QBDI {

/*! X86_64 Host context.
 */
struct QBDI_ALIGNED(8) HostState {
  rword sp;
  rword fs;
  rword gs;
  rword selector;
  rword callback;
  rword data;
  rword origin;
  rword executeFlags;
};

/*! X86_64 Execution context.
 */
struct QBDI_ALIGNED(16) Context {

public:
  // fprState needs to be first for memory alignement reasons
  FPRState fprState;
  GPRState gprState;
  HostState hostState;
};

} // namespace QBDI

#endif // CONTEXT_X86_64_H
