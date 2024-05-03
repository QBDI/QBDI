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
#ifndef INSTINFO_H
#define INSTINFO_H

#include <stdint.h>
#include "QBDI/State.h"

namespace llvm {
class MCInst;
} // namespace llvm

namespace QBDI {
class LLVMCPU;

unsigned getInstSize(const llvm::MCInst &inst, const LLVMCPU &llvmcpu);
unsigned getReadSize(const llvm::MCInst &inst, const LLVMCPU &llvmcpu);
unsigned getWriteSize(const llvm::MCInst &inst, const LLVMCPU &llvmcpu);
unsigned getImmediateSize(const llvm::MCInst &inst, const LLVMCPU &llvmcpu);
sword getFixedOperandValue(const llvm::MCInst &inst, const LLVMCPU &llvmcpu,
                           unsigned index, int64_t value);

bool unsupportedRead(const llvm::MCInst &inst);
bool unsupportedWrite(const llvm::MCInst &inst);

bool variadicOpsIsWrite(const llvm::MCInst &inst);

}; // namespace QBDI

#endif // INSTCLASSES_H
