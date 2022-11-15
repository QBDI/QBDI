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
#ifndef REGISTER_H
#define REGISTER_H

#include <array>
#include <cstddef>
#include <map>
#include <stdint.h>

#include "QBDI/Bitmask.h"
#include "QBDI/State.h"

namespace llvm {
class MCInst;
} // namespace llvm

namespace QBDI {
class LLVMCPU;
class Patch;
struct RegLLVM;

extern const RegLLVM GPR_ID[];
extern const RegLLVM FLAG_ID[];
extern const RegLLVM SEG_ID[];
extern const std::map<RegLLVM, int16_t> FPR_ID;
extern const unsigned int size_GPR_ID;
extern const unsigned int size_FLAG_ID;
extern const unsigned int size_SEG_ID;

// Get register size
uint8_t getRegisterSize(RegLLVM reg);
// Get the number of real register in the LLVM register
uint8_t getRegisterPacked(RegLLVM reg);
// If packed register, get the space between to packed register
uint8_t getRegisterSpaced(RegLLVM reg);
// Get the position of a LLVM register in GPRState
size_t getGPRPosition(RegLLVM reg);
// Get the full size register (AX => RAX for example in x86_64)
RegLLVM getUpperRegister(RegLLVM reg, size_t pos = 0);
// Get the packed register (R0_R1 pos 1 => R1 for example in ARM)
RegLLVM getPackedRegister(RegLLVM reg, size_t pos = 0);

enum RegisterUsage : uint8_t {
  RegisterUnused = 0,
  RegisterUsed = 0x1,
  RegisterSet = 0x2,
  RegisterBoth = 0x3,
  // RegisterSaved means the instrumentation will load this register from
  // the GPRState if used by the tempManager
  RegisterSaved = 0x4,
  // RegisterSaved means the instrumentation will load this register from
  // the GPRState if used as a Scratch Register
  RegisterSavedScratch = 0x8
};

_QBDI_ENABLE_BITMASK_OPERATORS(RegisterUsage)

/* Add register not declared by llvm in the RegisterUsage
 *
 * This method is called by getUsedGPR and must be implemented by each target
 * to fix missing declaration of LLVM.
 */
void fixLLVMUsedGPR(const llvm::MCInst &inst, const LLVMCPU &llvmcpu,
                    std::array<RegisterUsage, NUM_GPR> &,
                    std::map<RegLLVM, RegisterUsage> &);

/* Get General Register used and set by an instruction (needed for TempManager)
 *
 * If a GPR is not in the map, the instruction doesn't not used/set the register
 *
 * /!\ LLVM may not include all usage of stack register (mostly on call/ret
 * instruction)
 */
void getUsedGPR(const llvm::MCInst &inst, const LLVMCPU &llvmcpu,
                std::array<RegisterUsage, NUM_GPR> &regUsage,
                std::map<RegLLVM, RegisterUsage> &regUsageExtra);

}; // namespace QBDI

#endif // REGISTER_H
