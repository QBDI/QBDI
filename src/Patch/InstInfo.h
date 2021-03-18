/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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

namespace llvm {
  class MCInst;
  class MCInstrDesc;
}

namespace QBDI {

unsigned getReadSize(const llvm::MCInst& inst);
unsigned getWriteSize(const llvm::MCInst& inst);
bool isStackRead(const llvm::MCInst& inst);
bool isStackWrite(const llvm::MCInst& inst);

unsigned getImmediateSize(const llvm::MCInst& inst, const llvm::MCInstrDesc& desc);

// The TempManager will allow to reuse some register
// when the method return True.
bool useAllRegisters(const llvm::MCInst& inst);

bool unsupportedRead(const llvm::MCInst& inst);

};

#endif // INSTCLASSES_H
