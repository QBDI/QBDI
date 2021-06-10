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

#include "X86InstrInfo.h"
#include "llvm/MC/MCInst.h"

#include "Patch/X86_64/Layer2_X86_64.h"
#include "Patch/X86_64/RelocatableInst_X86_64.h"

#include "QBDI/Config.h"

namespace QBDI {

llvm::MCInst mov32rr(unsigned int dst, unsigned int src) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::MOV32rr);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(src));

    return inst;
}

llvm::MCInst mov32ri(unsigned int reg, rword imm) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::MOV32ri);
    inst.addOperand(llvm::MCOperand::createReg(reg));
    inst.addOperand(llvm::MCOperand::createImm(imm));

    return inst;
}

llvm::MCInst mov32mr(unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg, unsigned int src) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::MOV32mr);
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(scale));
    inst.addOperand(llvm::MCOperand::createReg(offset));
    inst.addOperand(llvm::MCOperand::createImm(displacement));
    inst.addOperand(llvm::MCOperand::createReg(seg));
    inst.addOperand(llvm::MCOperand::createReg(src));

    return inst;
}

llvm::MCInst mov32rm8(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::MOVZX32rm8);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(scale));
    inst.addOperand(llvm::MCOperand::createReg(offset));
    inst.addOperand(llvm::MCOperand::createImm(displacement));
    inst.addOperand(llvm::MCOperand::createReg(seg));

    return inst;
}

llvm::MCInst mov32rm16(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::MOVZX32rm16);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(scale));
    inst.addOperand(llvm::MCOperand::createReg(offset));
    inst.addOperand(llvm::MCOperand::createImm(displacement));
    inst.addOperand(llvm::MCOperand::createReg(seg));

    return inst;
}

llvm::MCInst mov32rm(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::MOV32rm);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(scale));
    inst.addOperand(llvm::MCOperand::createReg(offset));
    inst.addOperand(llvm::MCOperand::createImm(displacement));
    inst.addOperand(llvm::MCOperand::createReg(seg));

    return inst;
}

llvm::MCInst movzx32rr8(unsigned int dst, unsigned int src) {

    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::MOVZX32rr8);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(src));

    return inst;
}

llvm::MCInst mov64rr(unsigned int dst, unsigned int src) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::MOV64rr);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(src));

    return inst;
}

llvm::MCInst mov64ri(unsigned int reg, rword imm) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::MOV64ri);
    inst.addOperand(llvm::MCOperand::createReg(reg));
    inst.addOperand(llvm::MCOperand::createImm(imm));

    return inst;
}

llvm::MCInst mov64mr(unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg, unsigned int src) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::MOV64mr);
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(scale));
    inst.addOperand(llvm::MCOperand::createReg(offset));
    inst.addOperand(llvm::MCOperand::createImm(displacement));
    inst.addOperand(llvm::MCOperand::createReg(seg));
    inst.addOperand(llvm::MCOperand::createReg(src));

    return inst;
}

llvm::MCInst mov64rm(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::MOV64rm);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(scale));
    inst.addOperand(llvm::MCOperand::createReg(offset));
    inst.addOperand(llvm::MCOperand::createImm(displacement));
    inst.addOperand(llvm::MCOperand::createReg(seg));

    return inst;
}

llvm::MCInst movzx64rr8(unsigned int dst, unsigned int src) {

    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::MOVZX64rr8);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(src));

    return inst;
}

llvm::MCInst test32ri(unsigned int base, uint32_t imm) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::TEST32ri);
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(imm));

    return inst;
}

llvm::MCInst test64ri32(unsigned int base, uint32_t imm) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::TEST64ri32);
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(imm));

    return inst;
}

llvm::MCInst jmp32m(unsigned int base, rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::JMP32m);
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(1));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createImm(offset));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

llvm::MCInst jmp64m(unsigned int base, rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::JMP64m);
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(1));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createImm(offset));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

llvm::MCInst je(int32_t offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::JCC_4);
    inst.addOperand(llvm::MCOperand::createImm(offset));
    inst.addOperand(llvm::MCOperand::createImm(llvm::X86::CondCode::COND_E));

    return inst;
}

llvm::MCInst jne(int32_t offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::JCC_4);
    inst.addOperand(llvm::MCOperand::createImm(offset));
    inst.addOperand(llvm::MCOperand::createImm(llvm::X86::CondCode::COND_NE));

    return inst;
}


llvm::MCInst jmp(rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::JMP_4);
    inst.addOperand(llvm::MCOperand::createImm(offset));

    return inst;
}

llvm::MCInst fxsave(unsigned int base, rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::FXSAVE);
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(1));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createImm(offset));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

llvm::MCInst fxrstor(unsigned int base, rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::FXRSTOR);
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(1));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createImm(offset));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

llvm::MCInst vextractf128(unsigned int base, rword offset, unsigned int src, uint8_t regoffset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::VEXTRACTF128mr);
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(1));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createImm(offset));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createReg(src));
    inst.addOperand(llvm::MCOperand::createImm(regoffset));

    return inst;
}

llvm::MCInst vinsertf128(unsigned int dst, unsigned int base, rword offset, uint8_t regoffset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::VINSERTF128rm);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(1));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createImm(offset));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createImm(1));

    return inst;
}

llvm::MCInst push32r(unsigned int reg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::PUSH32r);
    inst.addOperand(llvm::MCOperand::createReg(reg));

    return inst;
}

llvm::MCInst push64r(unsigned int reg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::PUSH64r);
    inst.addOperand(llvm::MCOperand::createReg(reg));

    return inst;
}

llvm::MCInst pop32r(unsigned int reg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::POP32r);
    inst.addOperand(llvm::MCOperand::createReg(reg));

    return inst;
}

llvm::MCInst pop64r(unsigned int reg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::POP64r);
    inst.addOperand(llvm::MCOperand::createReg(reg));

    return inst;
}

llvm::MCInst addr32i(unsigned int dst, unsigned int src, rword imm) {

    // We use LEA to avoid flags to be modified
    return lea32(dst, src, 1, 0, imm, 0);
}

llvm::MCInst addr64i(unsigned int dst, unsigned int src, rword imm) {

    // We use LEA to avoid flags to be modified
    return lea64(dst, src, 1, 0, imm, 0);
}

llvm::MCInst lea32(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::LEA32r);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(scale));
    inst.addOperand(llvm::MCOperand::createReg(offset));
    inst.addOperand(llvm::MCOperand::createImm(displacement));
    inst.addOperand(llvm::MCOperand::createReg(seg));

    return inst;
}

llvm::MCInst lea64(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::LEA64r);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(scale));
    inst.addOperand(llvm::MCOperand::createReg(offset));
    inst.addOperand(llvm::MCOperand::createImm(displacement));
    inst.addOperand(llvm::MCOperand::createReg(seg));

    return inst;
}

llvm::MCInst popf32() {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::POPF32);

    return inst;
}

llvm::MCInst popf64() {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::POPF64);

    return inst;
}

llvm::MCInst pushf32() {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::PUSHF32);

    return inst;
}

llvm::MCInst pushf64() {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::PUSHF64);

    return inst;
}

llvm::MCInst ret() {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::RETQ);

    return inst;
}

llvm::MCInst movrr(unsigned int dst, unsigned int src) {
    if constexpr(is_x86_64)
        return mov64rr(dst, src);
    else
        return mov32rr(dst, src);
}

llvm::MCInst movri(unsigned int dst, rword imm) {
    if constexpr(is_x86_64)
        return mov64ri(dst, imm);
    else
        return mov32ri(dst, imm);
}

llvm::MCInst movmr(unsigned int base, rword scale, unsigned int offset, rword disp, unsigned int seg, unsigned int src) {
    if constexpr(is_x86_64)
        return mov64mr(base, scale, offset, disp, seg, src);
    else
        return mov32mr(base, scale, offset, disp, seg, src);
}

llvm::MCInst movrm(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword disp, unsigned int seg) {
    if constexpr(is_x86_64)
        return mov64rm(dst, base, scale, offset, disp, seg);
    else
        return mov32rm(dst, base, scale, offset, disp, seg);
}

llvm::MCInst movzxrr8(unsigned int dst, unsigned int src) {
    if constexpr(is_x86_64)
        return movzx64rr8(dst, src);
    else
        return movzx32rr8(dst, src);
}

llvm::MCInst testri(unsigned int base, uint32_t imm) {
    if constexpr(is_x86_64)
        return test64ri32(base, imm);
    else
        return test32ri(base, imm);
}

llvm::MCInst pushr(unsigned int reg) {
    if constexpr(is_x86_64)
        return push64r(reg);
    else
        return push32r(reg);
}

llvm::MCInst popr(unsigned int reg) {
    if constexpr(is_x86_64)
        return pop64r(reg);
    else
        return pop32r(reg);
}

llvm::MCInst addri(unsigned int dst, unsigned int src, rword imm) {
    if constexpr(is_x86_64)
        return addr64i(dst, src, imm);
    else
        return addr32i(dst, src, imm);
}

llvm::MCInst lea(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword disp, unsigned int seg) {
    if constexpr(is_x86_64)
        return lea64(dst, base, scale, offset, disp, seg);
    else
        return lea32(dst, base, scale, offset, disp, seg);
}

llvm::MCInst popf() {
    if constexpr(is_x86_64)
        return popf64();
    else
        return popf32();
}

llvm::MCInst pushf() {
    if constexpr(is_x86_64)
        return pushf64();
    else
        return pushf32();
}

llvm::MCInst jmpm(unsigned int base, rword offset) {
    if constexpr(is_x86_64)
        return jmp64m(base, offset);
    else
        return jmp32m(base, offset);
}

RelocatableInst::UniquePtr Mov(Reg dst, Reg src) {
    return NoReloc::unique(movrr(dst, src));
}

RelocatableInst::UniquePtr Mov(Reg reg, Constant cst) {
    return NoReloc::unique(movri(reg, cst));
}

RelocatableInst::UniquePtr Mov(Offset offset, Reg reg) {
    return DataBlockRelx86(movmr(0, 0, 0, 0, 0, reg), 0, offset, 7);
}

RelocatableInst::UniquePtr Mov(Shadow shadow, Reg reg, bool create) {
    return TaggedShadowx86(movmr(0, 0, 0, 0, 0, reg), 0, shadow.getTag(), 7, create);
}

RelocatableInst::UniquePtr Mov(Reg reg, Offset offset) {
    return DataBlockRelx86(movrm(reg, 0, 0, 0, 0, 0), 1, offset, 7);
}

RelocatableInst::UniquePtr Mov(Reg reg, Shadow shadow) {
    return TaggedShadowx86(movrm(reg, 0, 0, 0, 0, 0), 1, shadow.getTag(), 7, false);
}

RelocatableInst::UniquePtr JmpM(Offset offset) {
    return DataBlockRelx86(jmpm(0, 0), 0, offset, 6);
}

RelocatableInst::UniquePtr Fxsave(Offset offset) {
    return DataBlockRelx86(fxsave(0, 0), 0, offset, 7);
}

RelocatableInst::UniquePtr Fxrstor(Offset offset) {
    return DataBlockRelx86(fxrstor(0, 0), 0, offset, 7);
}

RelocatableInst::UniquePtr Vextractf128(Offset offset, unsigned int src, Constant regoffset) {
    return DataBlockRelx86(vextractf128(0, 0, src, regoffset), 0, offset, 10);
}

RelocatableInst::UniquePtr Vinsertf128(unsigned int dst, Offset offset, Constant regoffset) {
    return DataBlockRelx86(vinsertf128(dst, 0, 0, regoffset), 2, offset, 10);
}

RelocatableInst::UniquePtr Pushr(Reg reg) {
    return NoReloc::unique(pushr(reg));
}

RelocatableInst::UniquePtr Popr(Reg reg) {
    return NoReloc::unique(popr(reg));
}

RelocatableInst::UniquePtr Add(Reg reg, Constant cst) {
    return NoReloc::unique(addri(reg, reg, cst));
}

RelocatableInst::UniquePtr Pushf() {
    return NoReloc::unique(pushf());
}

RelocatableInst::UniquePtr Popf() {
    return NoReloc::unique(popf());
}

RelocatableInst::UniquePtr Ret() {
    return NoReloc::unique(ret());
}

RelocatableInst::UniquePtr Test(Reg reg, unsigned int value) {
    return NoReloc::unique(testri(reg, value));
}

RelocatableInst::UniquePtr Je(int32_t offset) {
    return NoReloc::unique(je(offset));
}

RelocatableInst::UniquePtr Jne(int32_t offset) {
    return NoReloc::unique(jne(offset));
}

}
