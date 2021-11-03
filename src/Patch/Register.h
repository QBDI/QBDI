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
#ifndef REGISTER_H
#define REGISTER_H

#include <cstddef>
#include <map>
#include <stdint.h>

#include "QBDI/Bitmask.h"

namespace llvm {
class MCInst;
} // namespace llvm

namespace QBDI {
class LLVMCPU;

extern const unsigned int GPR_ID[];
extern const unsigned int FLAG_ID[];
extern const unsigned int SEG_ID[];
extern const std::map<unsigned int, int16_t> FPR_ID;
extern const unsigned int size_GPR_ID;
extern const unsigned int size_FLAG_ID;
extern const unsigned int size_SEG_ID;

// Get register size
uint8_t getRegisterSize(unsigned reg);
// Get the number of real register in the LLVM register
uint8_t getRegisterPacked(unsigned reg);
// Get the position of a LLVM register in GPRState
size_t getGPRPosition(unsigned reg);
// Get the full size register (AX => RAX for example in x86_64)
unsigned getUpperRegister(unsigned reg, size_t pos = 0);

typedef enum {
  RegisterUsed = 0x1,
  RegisterSet = 0x2,
  RegisterBoth = 0x3
} RegisterUsage;

_QBDI_ENABLE_BITMASK_OPERATORS(RegisterUsage)

/* Add register not declared by llvm in the RegisterUsageMap
 *
 * This method is called by getUsedGPR and must be implemented by each target
 * to fix missing declaration of LLVM.
 */
void fixLLVMUsedGPR(const llvm::MCInst &inst, const LLVMCPU &llvmcpu,
                    std::map<unsigned, RegisterUsage> &);

/* Get General Register used and set by an instruction (needed for TempManager)
 *
 * If a GPR is not in the map, the instruction doesn't not used/set the register
 *
 * /!\ LLVM may not include all usage of stack register (mostly on call/ret
 * instruction)
 */
std::map<unsigned, RegisterUsage> getUsedGPR(const llvm::MCInst &inst,
                                             const LLVMCPU &llvmcpu);

// Add a register in the register usage Map
void addRegisterInMap(std::map<unsigned, RegisterUsage> &m, unsigned reg,
                      RegisterUsage usage);

}; // namespace QBDI

#endif // REGISTER_H
