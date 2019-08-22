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
#include "Patch/X86_64/RelocatableInst_X86_64.h"

namespace QBDI {

#ifdef QBDI_ARCH_X86_64
#define movrr mov64rr
#define movri mov64ri
#define movmr mov64mr
#define movrm mov64rm
#define pushr push64r
#define popr pop64r
#define addri addr64i
#define lea lea64
#define popf popf64
#define pushf pushf64
#define jmpm jmp64m
#define callm call64m

#else /* QBDI_ARCH_X86 */
#define movrr mov32rr
#define movri mov32ri
#define movmr mov32mr
#define movrm mov32rm
#define pushr push32r
#define popr pop32r
#define addri addr32i
#define lea lea32
#define popf popf32
#define pushf pushf32
#define jmpm jmp32m
#define callm call32m

#endif

// low level layer 2

llvm::MCInst mov64rr(unsigned int dst, unsigned int src);

llvm::MCInst mov64ri(unsigned int reg, rword imm);

llvm::MCInst mov64mr(unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg, unsigned int src);

llvm::MCInst mov32rr(unsigned int dst, unsigned int src);

llvm::MCInst mov32ri(unsigned int reg, rword imm);

llvm::MCInst mov32mr(unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg, unsigned int src);

llvm::MCInst mov32rm8(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst mov32rm16(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst mov32rm(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst mov64rm(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst jmp32m(unsigned int base, rword offset);

llvm::MCInst jmp64m(unsigned int base, rword offset);

llvm::MCInst call32m(unsigned int base, rword offset);

llvm::MCInst call64m(unsigned int base, rword offset);

llvm::MCInst fxsave(unsigned int base, rword offset);

llvm::MCInst fxrstor(unsigned int base, rword offset);

llvm::MCInst vextractf128(unsigned int base, rword offset, unsigned int src, uint8_t regoffset);

llvm::MCInst vinsertf128(unsigned int dst, unsigned int base, rword offset, uint8_t regoffset);

llvm::MCInst push32r(unsigned int reg);

llvm::MCInst push64r(unsigned int reg);

llvm::MCInst pop32r(unsigned int reg);

llvm::MCInst pop64r(unsigned int reg);

llvm::MCInst addr32i(unsigned int reg, rword imm);

llvm::MCInst addr64i(unsigned int reg, rword imm);

llvm::MCInst lea32(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst lea64(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg);

llvm::MCInst pushf32();

llvm::MCInst pushf64();

llvm::MCInst popf32();

llvm::MCInst popf64();

llvm::MCInst jmp(rword offset);

llvm::MCInst ret();

// high level layer 2

RelocatableInst::SharedPtr Mov(Reg dst, Reg src);

RelocatableInst::SharedPtr Mov(Reg reg, Constant cst);

RelocatableInst::SharedPtr Mov(Offset offset, Reg reg);

RelocatableInst::SharedPtr Mov(Reg reg, Offset offset);

RelocatableInst::SharedPtr Lea(Reg reg, Offset offset);

RelocatableInst::SharedPtr JmpM(Offset offset);

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
