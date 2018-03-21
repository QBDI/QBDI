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
#include "Patch/X86_64/Layer2_X86_64.h"

namespace QBDI {

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

llvm::MCInst pushr(unsigned int reg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::PUSH64r);
    inst.addOperand(llvm::MCOperand::createReg(reg));

    return inst;
}

llvm::MCInst popr(unsigned int reg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::POP64r);
    inst.addOperand(llvm::MCOperand::createReg(reg));

    return inst;
}

llvm::MCInst addri(unsigned int dst, unsigned int src, rword imm) {
    llvm::MCInst inst;

    // We use LEA to avoid flags to be modified
    inst.setOpcode(llvm::X86::LEA64r);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(src));
    inst.addOperand(llvm::MCOperand::createImm(1));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createImm(imm));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

llvm::MCInst lea(unsigned int dst, unsigned int base, rword scale, unsigned int offset, rword displacement, unsigned int seg) {
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

llvm::MCInst popf() {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::POPF64);

    return inst;
}

llvm::MCInst pushf() {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::PUSHF64);

    return inst;
}

llvm::MCInst ret() {
    llvm::MCInst inst;

    inst.setOpcode(llvm::X86::RETQ);

    return inst;
}

RelocatableInst::SharedPtr Mov(Reg dst, Reg src) {
    return NoReloc(mov64rr(dst, src));
}

RelocatableInst::SharedPtr Mov(Reg reg, Constant cst) {
    return NoReloc(mov64ri(reg, cst));
}

RelocatableInst::SharedPtr Mov(Offset offset, Reg reg) {
    return DataBlockRel(mov64mr(Reg(REG_PC), 0, 0, 0, 0, reg), 3, offset - 7);
}

RelocatableInst::SharedPtr Mov(Reg reg, Offset offset) {
    return DataBlockRel(mov64rm(reg, Reg(REG_PC), 0, 0, 0, 0), 4, offset - 7);
}

RelocatableInst::SharedPtr Jmp64m(Offset offset) {
    return DataBlockRel(jmp64m(Reg(REG_PC), 0), 3, offset - 6);
}

RelocatableInst::SharedPtr Fxsave(Offset offset) {
    return DataBlockRel(fxsave(Reg(REG_PC), 0), 3, offset-7);
}

RelocatableInst::SharedPtr Fxrstor(Offset offset) {
    return DataBlockRel(fxrstor(Reg(REG_PC), 0), 3, offset-7);
}

RelocatableInst::SharedPtr Vextractf128(Offset offset, unsigned int src, Constant regoffset) {
    return DataBlockRel(vextractf128(Reg(REG_PC), 0, src, regoffset), 3, offset-10);
}

RelocatableInst::SharedPtr Vinsertf128(unsigned int dst, Offset offset, Constant regoffset) {
    return DataBlockRel(vinsertf128(dst, Reg(REG_PC), 0, regoffset), 5, offset-10);
}

RelocatableInst::SharedPtr Pushr(Reg reg) {
    return NoReloc(pushr(reg));
}

RelocatableInst::SharedPtr Popr(Reg reg) {
    return NoReloc(popr(reg));
}

RelocatableInst::SharedPtr Add(Reg reg, Constant cst) {
    return NoReloc(addri(reg, reg, cst));
}

RelocatableInst::SharedPtr Pushf() {
    return NoReloc(pushf());
}

RelocatableInst::SharedPtr Popf() {
    return NoReloc(popf());
}

RelocatableInst::SharedPtr Ret() {
    return NoReloc(ret());
}

}
