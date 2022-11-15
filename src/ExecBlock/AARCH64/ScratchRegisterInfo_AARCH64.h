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
#ifndef SCRATCHREGISTERINFO_AARCH64_H
#define SCRATCHREGISTERINFO_AARCH64_H

#include <stdint.h>

#include "Patch/Types.h"

namespace QBDI {
class Patch;

struct ScratchRegisterInfo {
  // WriteSequence variable
  const Patch *endSRPatch;
  RegLLVM writeScratchRegister;
};

struct ScratchRegisterSeqInfo {
  uint8_t scratchRegisterOffset;
};

} // namespace QBDI

#endif // SCRATCHREGISTERINFO_AARCH64_H
