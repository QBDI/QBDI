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
#ifndef LAYER2_X86_64_H
#define LAYER2_X86_64_H

#include <memory>

#include "llvm/MC/MCInst.h"

#include "Patch/Types.h"

namespace QBDI {

class RelocatableInst;

// low level layer 2

llvm::MCInst mov32rr(unsigned int dst, unsigned int src);

llvm::MCInst mov32ri(unsigned int reg, rword imm);

llvm::MCInst mov32mr(unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg, unsigned int src);

llvm::MCInst mov32rm8(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst mov32rm16(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst mov32rm(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst mov64rr(unsigned int dst, unsigned int src);

llvm::MCInst mov64ri(unsigned int reg, rword imm);

llvm::MCInst mov64mr(unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg, unsigned int src);

llvm::MCInst mov64rm(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst jmp32m(unsigned int base, rword offset);

llvm::MCInst jmp64m(unsigned int base, rword offset);

llvm::MCInst jmp(rword offset);

llvm::MCInst fxsave(unsigned int base, rword offset);

llvm::MCInst fxrstor(unsigned int base, rword offset);

llvm::MCInst vextractf128(unsigned int base, rword offset, unsigned int src, uint8_t regoffset);

llvm::MCInst vinsertf128(unsigned int dst, unsigned int base, rword offset, uint8_t regoffset);

llvm::MCInst push32r(unsigned int reg);

llvm::MCInst push64r(unsigned int reg);

llvm::MCInst pop32r(unsigned int reg);

llvm::MCInst pop64r(unsigned int reg);

llvm::MCInst addr32i(unsigned int dst, unsigned int src, rword imm);

llvm::MCInst addr64i(unsigned int dst, unsigned int src, rword imm);

llvm::MCInst lea32(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst lea64(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst xchg32rm(unsigned int reg, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst xchg64rm(unsigned int reg, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst pushf32();

llvm::MCInst pushf64();

llvm::MCInst popf32();

llvm::MCInst popf64();

llvm::MCInst ret();


// /!\ change flags
llvm::MCInst and32ri(unsigned int reg, rword imm);

llvm::MCInst and64ri(unsigned int reg, rword imm);

llvm::MCInst shr32ri(unsigned int reg, rword imm);

llvm::MCInst shr64ri(unsigned int reg, rword imm);


// low level layer 2 architecture abtraction

llvm::MCInst movrr(unsigned int dst, unsigned int src);

llvm::MCInst movri(unsigned int dst, rword imm);

llvm::MCInst movmr(unsigned int base, rword scale, unsigned int offset, rword disp, unsigned int seg, unsigned int src);

llvm::MCInst movrm(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword disp, unsigned int seg);

llvm::MCInst pushr(unsigned int reg);

llvm::MCInst popr(unsigned int reg);

llvm::MCInst addri(unsigned int dst, unsigned int src, rword imm);

llvm::MCInst lea(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword disp, unsigned int seg);

llvm::MCInst xchgrm(unsigned int reg, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst popf();

llvm::MCInst pushf();

llvm::MCInst jmpm(unsigned int base, rword offset);

// /!\ change flags
llvm::MCInst andri(unsigned int reg, rword imm);

llvm::MCInst shrri(unsigned int reg, rword imm);

// high level layer 2

std::shared_ptr<RelocatableInst> Mov(Reg dst, Reg src);

std::shared_ptr<RelocatableInst> Mov(Reg reg, Constant cst);

std::shared_ptr<RelocatableInst> Mov(Offset offset, Reg reg);

std::shared_ptr<RelocatableInst> Mov(Reg reg, Offset offset);

std::shared_ptr<RelocatableInst> Xchg(Reg reg, Offset offset);

std::shared_ptr<RelocatableInst> JmpM(Offset offset);

std::shared_ptr<RelocatableInst> Fxsave(Offset offset);

std::shared_ptr<RelocatableInst> Fxrstor(Offset offset);

std::shared_ptr<RelocatableInst> Vextractf128(Offset offset, unsigned int src, Constant regoffset);

std::shared_ptr<RelocatableInst> Vinsertf128(unsigned int dst, Offset offset, Constant regoffset);

std::shared_ptr<RelocatableInst> Pushr(Reg reg);

std::shared_ptr<RelocatableInst> Popr(Reg reg);

std::shared_ptr<RelocatableInst> Add(Reg reg, Constant cst);

std::shared_ptr<RelocatableInst> Pushf();

std::shared_ptr<RelocatableInst> Popf();

std::shared_ptr<RelocatableInst> Ret();

}

#endif
