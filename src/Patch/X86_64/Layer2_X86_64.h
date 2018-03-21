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

#include "Patch/Types.h"
#include "Patch/RelocatableInst.h"

namespace QBDI {

// low level layer 2

llvm::MCInst mov64rr(unsigned int dst, unsigned int src);

llvm::MCInst mov64ri(unsigned int reg, rword imm);

llvm::MCInst mov64mr(unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg, unsigned int src);

llvm::MCInst mov32rm8(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst mov32rm16(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst mov32rm(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst mov64rm(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst jmp64m(unsigned int base, rword offset);

llvm::MCInst fxsave(unsigned int base, rword offset);

llvm::MCInst fxrstor(unsigned int base, rword offset);

llvm::MCInst vextractf128(unsigned int base, rword offset, unsigned int src, uint8_t regoffset);

llvm::MCInst vinsertf128(unsigned int dst, unsigned int base, rword offset, uint8_t regoffset);

llvm::MCInst pushr(unsigned int reg);

llvm::MCInst popr(unsigned int reg);

llvm::MCInst addri(unsigned int reg, rword imm);

llvm::MCInst lea(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst pushf();

llvm::MCInst popf();

llvm::MCInst jmp(rword offset);

llvm::MCInst ret();

// high level layer 2

RelocatableInst::SharedPtr Mov(Reg dst, Reg src);

RelocatableInst::SharedPtr Mov(Reg reg, Constant cst);

RelocatableInst::SharedPtr Mov(Offset offset, Reg reg);

RelocatableInst::SharedPtr Mov(Reg reg, Offset offset);

RelocatableInst::SharedPtr Jmp64m(Offset offset);

RelocatableInst::SharedPtr Fxsave(Offset offset);

RelocatableInst::SharedPtr Fxrstor(Offset offset);

RelocatableInst::SharedPtr Vextractf128(Offset offset, unsigned int src, Constant regoffset);

RelocatableInst::SharedPtr Vinsertf128(unsigned int dst, Offset offset, Constant regoffset);

RelocatableInst::SharedPtr Pushr(Reg reg);

RelocatableInst::SharedPtr Popr(Reg reg);

RelocatableInst::SharedPtr Add(Reg reg, Constant cst);

RelocatableInst::SharedPtr Pushf();

RelocatableInst::SharedPtr Popf();

RelocatableInst::SharedPtr Ret();

}

#endif
