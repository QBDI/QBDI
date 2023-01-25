/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2023 Quarkslab
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

llvm::MCInst mov32rr(RegLLVM dst, RegLLVM src);

llvm::MCInst mov32ri(RegLLVM reg, rword imm);

llvm::MCInst mov32mr(RegLLVM base, rword scale, RegLLVM offset,
                     rword displacement, RegLLVM seg, RegLLVM src);

llvm::MCInst mov32rm8(RegLLVM dst, RegLLVM base, rword scale, RegLLVM offset,
                      rword displacement, RegLLVM seg);

llvm::MCInst mov32rm16(RegLLVM dst, RegLLVM base, rword scale, RegLLVM offset,
                       rword displacement, RegLLVM seg);

llvm::MCInst mov32rm(RegLLVM dst, RegLLVM base, rword scale, RegLLVM offset,
                     rword displacement, RegLLVM seg);

llvm::MCInst movzx32rr8(RegLLVM dst, RegLLVM src);

llvm::MCInst mov64rr(RegLLVM dst, RegLLVM src);

llvm::MCInst mov64ri(RegLLVM reg, rword imm);

llvm::MCInst mov64ri32(RegLLVM reg, rword imm);

llvm::MCInst mov64mr(RegLLVM base, rword scale, RegLLVM offset,
                     rword displacement, RegLLVM seg, RegLLVM src);

llvm::MCInst mov64rm(RegLLVM dst, RegLLVM base, rword scale, RegLLVM offset,
                     rword displacement, RegLLVM seg);

llvm::MCInst movzx64rr8(RegLLVM dst, RegLLVM src);

llvm::MCInst test32ri(RegLLVM base, uint32_t imm);

llvm::MCInst test64ri32(RegLLVM base, uint32_t imm);

llvm::MCInst je(int32_t offset);

llvm::MCInst jne(int32_t offset);

llvm::MCInst jmp32m(RegLLVM base, rword offset);

llvm::MCInst jmp64m(RegLLVM base, rword offset);

llvm::MCInst jmp(rword offset);

llvm::MCInst fxsave(RegLLVM base, rword offset);

llvm::MCInst fxrstor(RegLLVM base, rword offset);

llvm::MCInst vextractf128(RegLLVM base, rword offset, RegLLVM src,
                          uint8_t regoffset);

llvm::MCInst vinsertf128(RegLLVM dst, RegLLVM base, rword offset,
                         uint8_t regoffset);

llvm::MCInst push32r(RegLLVM reg);

llvm::MCInst push64r(RegLLVM reg);

llvm::MCInst pop32r(RegLLVM reg);

llvm::MCInst pop64r(RegLLVM reg);

llvm::MCInst addr32i(RegLLVM dst, RegLLVM src, rword imm);

llvm::MCInst addr64i(RegLLVM dst, RegLLVM src, rword imm);

llvm::MCInst lea32(RegLLVM dst, RegLLVM base, rword scale, RegLLVM offset,
                   rword displacement, RegLLVM seg);

llvm::MCInst lea64(RegLLVM dst, RegLLVM base, rword scale, RegLLVM offset,
                   rword displacement, RegLLVM seg);

llvm::MCInst pushf32();

llvm::MCInst pushf64();

llvm::MCInst popf32();

llvm::MCInst popf64();

llvm::MCInst ret();

llvm::MCInst rdfsbase(RegLLVM reg);

llvm::MCInst rdgsbase(RegLLVM reg);

llvm::MCInst wrfsbase(RegLLVM reg);

llvm::MCInst wrgsbase(RegLLVM reg);

llvm::MCInst nop();

llvm::MCInst xor32rr(RegLLVM dst, RegLLVM src);

llvm::MCInst xor64rr(RegLLVM dst, RegLLVM src);

// high level layer 2

std::unique_ptr<RelocatableInst> JmpM(Offset offset);

std::unique_ptr<RelocatableInst> Fxsave(Offset offset);

std::unique_ptr<RelocatableInst> Fxrstor(Offset offset);

std::unique_ptr<RelocatableInst> Vextractf128(Offset offset, RegLLVM src,
                                              Constant regoffset);

std::unique_ptr<RelocatableInst> Vinsertf128(RegLLVM dst, Offset offset,
                                             Constant regoffset);

std::unique_ptr<RelocatableInst> Pushr(Reg reg);

std::unique_ptr<RelocatableInst> Popr(Reg reg);

std::unique_ptr<RelocatableInst> Add(Reg dest, Reg src, Constant cst);

std::unique_ptr<RelocatableInst> Pushf();

std::unique_ptr<RelocatableInst> Popf();

std::unique_ptr<RelocatableInst> Ret();

std::unique_ptr<RelocatableInst> Test(Reg reg, uint32_t value);

std::unique_ptr<RelocatableInst> Je(int32_t offset);

std::unique_ptr<RelocatableInst> Jne(int32_t offset);

std::unique_ptr<RelocatableInst> Rdfsbase(Reg reg);

std::unique_ptr<RelocatableInst> Rdgsbase(Reg reg);

std::unique_ptr<RelocatableInst> Wrfsbase(Reg reg);

std::unique_ptr<RelocatableInst> Wrgsbase(Reg reg);

std::unique_ptr<RelocatableInst> Xorrr(RegLLVM dst, RegLLVM src);

std::unique_ptr<RelocatableInst> Lea(RegLLVM dst, RegLLVM base, rword scale,
                                     RegLLVM offset, rword disp, RegLLVM seg);

std::unique_ptr<RelocatableInst> MovzxrAL(Reg dst);

std::unique_ptr<RelocatableInst> Mov64rm(RegLLVM dst, RegLLVM addr,
                                         RegLLVM seg);

std::unique_ptr<RelocatableInst> Mov32rm(RegLLVM dst, RegLLVM addr,
                                         RegLLVM seg);

std::unique_ptr<RelocatableInst> Mov32rm16(RegLLVM dst, RegLLVM addr,
                                           RegLLVM seg);

std::unique_ptr<RelocatableInst> Mov32rm8(RegLLVM dst, RegLLVM addr,
                                          RegLLVM seg);

} // namespace QBDI

#endif
