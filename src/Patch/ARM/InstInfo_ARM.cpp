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
#include "InstInfo_ARM.h"
#include "Utility/LogSys.h"

namespace QBDI {

void initMemAccessInfo() {
    // TODO
    LogWarning("initMemAccessInfo", "This architecture does not support memory access information");
}

unsigned getReadSize(const llvm::MCInst* inst) {
    LogWarning("getReadSize", "This architecture does not support memory access information");
    return 0;
}

unsigned getWriteSize(const llvm::MCInst* inst) {
    LogWarning("getWriteSize", "This architecture does not support memory access information");
    return 0;
}

bool isStackRead(const llvm::MCInst* inst) {
    LogWarning("isStackRead", "This architecture does not support memory access information");
    return false;
}

bool isStackWrite(const llvm::MCInst* inst) {
    LogWarning("isStackWrite", "This architecture does not support memory access information");
    return false;
}

bool isDoubleRead(const llvm::MCInst* inst) {
    LogWarning("isDoubleRead", "This architecture does not support memory access information");
    return false;
}

unsigned getImmediateSize(const llvm::MCInst* inst, const llvm::MCInstrDesc* desc) {
    return sizeof(rword);
}

bool useAllRegisters(const llvm::MCInst* inst) {
    return false;
}

};
