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
#ifndef ExecBlockFlags_H
#define ExecBlockFlags_H

#include <memory>
#include <vector>

#include "QBDI/Platform.h"

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
#include "Patch/X86_64/ExecBlockFlags_X86_64.h"
#elif defined(QBDI_ARCH_ARM)
#include "Patch/ARM/ExecBlockFlags_ARM.h"
#elif defined(QBDI_ARCH_AARCH64)
#include "Patch/AARCH64/ExecBlockFlags_AARCH64.h"
#endif

namespace llvm {
class MCInst;
}

namespace QBDI {
class LLVMCPU;

uint8_t getExecBlockFlags(const llvm::MCInst &inst, const LLVMCPU &llvmcpu);

extern const uint8_t defaultExecuteFlags;

} // namespace QBDI

#endif
