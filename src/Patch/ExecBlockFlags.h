/*
 * This file is part of QBDI.
 *
 * Copyright 2017 Quarkslab
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

#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "Platform.h"

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
#include "Patch/X86_64/ExecBlockFlags_X86_64.h"
#elif defined(QBDI_ARCH_ARM)
#include "Patch/ARM/ExecBlockFlags_ARM.h"
#endif

namespace QBDI {

uint8_t getExecBlockFlags(const llvm::MCInst& inst, const llvm::MCInstrInfo* MCII, const llvm::MCRegisterInfo* MRI);

extern const uint8_t defaultExecuteFlags;

}

#endif
