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
#ifndef LAYER2_X86_64_H
#define LAYER2_X86_64_H

#include <memory>
#include <stdint.h>

#include "llvm/MC/MCInst.h"

#include "QBDI/State.h"
#include "Patch/Types.h"

namespace QBDI {

class RelocatableInst;

// low level layer 2

llvm::MCInst mov32rr(unsigned int dst, unsigned int src);

llvm::MCInst mov32ri(unsigned int reg, rword imm);

llvm::MCInst mov32mr(unsigned int base, rword scale, unsigned int offset,
                     rword displacement, unsigned int seg, unsigned int src);

llvm::MCInst mov32rm8(unsigned int dst, unsigned int base, rword scale,
                      unsigned int offset, rword displacement,
                      unsigned int seg);

llvm::MCInst mov32rm16(unsigned int dst, unsigned int base, rword scale,
                       unsigned int offset, rword displacement,
                       unsigned int seg);

llvm::MCInst mov32rm(unsigned int dst, unsigned int base, rword scale,
                     unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst movzx32rr8(unsigned int dst, unsigned int src);

llvm::MCInst mov64rr(unsigned int dst, unsigned int src);

llvm::MCInst mov64ri(unsigned int reg, rword imm);

llvm::MCInst mov64mr(unsigned int base, rword scale, unsigned int offset,
                     rword displacement, unsigned int seg, unsigned int src);

llvm::MCInst mov64rm(unsigned int dst, unsigned int base, rword scale,
                     unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst movzx64rr8(unsigned int dst, unsigned int src);

llvm::MCInst test32ri(unsigned int base, uint32_t imm);

llvm::MCInst test64ri32(unsigned int base, uint32_t imm);

llvm::MCInst je(int32_t offset);

llvm::MCInst jne(int32_t offset);

llvm::MCInst jmp32m(unsigned int base, rword offset);

llvm::MCInst jmp64m(unsigned int base, rword offset);

llvm::MCInst jmp(rword offset);

llvm::MCInst fxsave(unsigned int base, rword offset);

llvm::MCInst fxrstor(unsigned int base, rword offset);

llvm::MCInst vextractf128(unsigned int base, rword offset, unsigned int src,
                          uint8_t regoffset);

llvm::MCInst vinsertf128(unsigned int dst, unsigned int base, rword offset,
                         uint8_t regoffset);

llvm::MCInst push32r(unsigned int reg);

llvm::MCInst push64r(unsigned int reg);

llvm::MCInst pop32r(unsigned int reg);

llvm::MCInst pop64r(unsigned int reg);

llvm::MCInst addr32i(unsigned int dst, unsigned int src, rword imm);

llvm::MCInst addr64i(unsigned int dst, unsigned int src, rword imm);

llvm::MCInst lea32(unsigned int dst, unsigned int base, rword scale,
                   unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst lea64(unsigned int dst, unsigned int base, rword scale,
                   unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst pushf32();

llvm::MCInst pushf64();

llvm::MCInst popf32();

llvm::MCInst popf64();

llvm::MCInst ret();

llvm::MCInst rdfsbase(unsigned int reg);

llvm::MCInst rdgsbase(unsigned int reg);

llvm::MCInst wrfsbase(unsigned int reg);

llvm::MCInst wrgsbase(unsigned int reg);

llvm::MCInst nop();

// low level layer 2 architecture abtraction

llvm::MCInst movrr(unsigned int dst, unsigned int src);

llvm::MCInst movri(unsigned int dst, rword imm);

llvm::MCInst movmr(unsigned int base, rword scale, unsigned int offset,
                   rword disp, unsigned int seg, unsigned int src);

llvm::MCInst movrm(unsigned int dst, unsigned int base, rword scale,
                   unsigned int offset, rword disp, unsigned int seg);

llvm::MCInst movzxrr8(unsigned int dst, unsigned int src);

llvm::MCInst testri(unsigned int base, uint32_t imm);

llvm::MCInst pushr(unsigned int reg);

llvm::MCInst popr(unsigned int reg);

llvm::MCInst addri(unsigned int dst, unsigned int src, rword imm);

llvm::MCInst lea(unsigned int dst, unsigned int base, rword scale,
                 unsigned int offset, rword disp, unsigned int seg);

llvm::MCInst popf();

llvm::MCInst pushf();

llvm::MCInst jmpm(unsigned int base, rword offset);

// high level layer 2

std::unique_ptr<RelocatableInst> Mov(Reg dst, Reg src);

std::unique_ptr<RelocatableInst> Mov(Reg reg, Constant cst);

std::unique_ptr<RelocatableInst> Mov(Offset offset, Reg reg);

std::unique_ptr<RelocatableInst> Mov(Shadow shadow, Reg reg,
                                     bool create = true);

std::unique_ptr<RelocatableInst> Mov(Reg reg, Offset offset);

std::unique_ptr<RelocatableInst> Mov(Reg reg, Shadow shadow);

std::unique_ptr<RelocatableInst> JmpM(Offset offset);

std::unique_ptr<RelocatableInst> Fxsave(Offset offset);

std::unique_ptr<RelocatableInst> Fxrstor(Offset offset);

std::unique_ptr<RelocatableInst> Vextractf128(Offset offset, unsigned int src,
                                              Constant regoffset);

std::unique_ptr<RelocatableInst> Vinsertf128(unsigned int dst, Offset offset,
                                             Constant regoffset);

std::unique_ptr<RelocatableInst> Pushr(Reg reg);

std::unique_ptr<RelocatableInst> Popr(Reg reg);

std::unique_ptr<RelocatableInst> Add(Reg reg, Constant cst);

std::unique_ptr<RelocatableInst> Pushf();

std::unique_ptr<RelocatableInst> Popf();

std::unique_ptr<RelocatableInst> Ret();

std::unique_ptr<RelocatableInst> Test(Reg reg, unsigned int value);

std::unique_ptr<RelocatableInst> Je(int32_t offset);

std::unique_ptr<RelocatableInst> Jne(int32_t offset);

std::unique_ptr<RelocatableInst> Rdfsbase(Reg reg);

std::unique_ptr<RelocatableInst> Rdgsbase(Reg reg);

std::unique_ptr<RelocatableInst> Wrfsbase(Reg reg);

std::unique_ptr<RelocatableInst> Wrgsbase(Reg reg);

} // namespace QBDI

#endif
