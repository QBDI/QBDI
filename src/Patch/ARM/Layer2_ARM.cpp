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
#include "Layer2_ARM.h"

namespace QBDI {

llvm::MCInst ldri12(unsigned int reg, unsigned int base, rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::LDRi12);
    inst.addOperand(llvm::MCOperand::createReg(reg));
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(offset));
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

llvm::MCInst stri12(unsigned int reg, unsigned int base, rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::STRi12);
    inst.addOperand(llvm::MCOperand::createReg(reg));
    inst.addOperand(llvm::MCOperand::createReg(base));
    inst.addOperand(llvm::MCOperand::createImm(offset));
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

llvm::MCInst adr(unsigned int reg, rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::ADR);
    inst.addOperand(llvm::MCOperand::createReg(reg));
    inst.addOperand(llvm::MCOperand::createImm(offset));
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

llvm::MCInst b(rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::Bcc);
    inst.addOperand(llvm::MCOperand::createImm(offset));
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

llvm::MCInst mov(unsigned int dst, unsigned int src) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::MOVr);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(src));
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

llvm::MCInst pop(unsigned int reg, int64_t cond) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::LDR_POST_IMM);
    inst.addOperand(llvm::MCOperand::createReg(reg));
    inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::SP));
    inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::SP));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createImm(4));
    inst.addOperand(llvm::MCOperand::createImm(cond));
    if(cond == 14)
        inst.addOperand(llvm::MCOperand::createReg(0));
    else
        inst.addOperand(llvm::MCOperand::createReg(3));

    return inst;
}

llvm::MCInst push(unsigned int reg, int64_t cond) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::STR_PRE_IMM);
    inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::SP));
    inst.addOperand(llvm::MCOperand::createReg(reg));
    inst.addOperand(llvm::MCOperand::createReg(llvm::ARM::SP));
    inst.addOperand(llvm::MCOperand::createImm(-4));
    inst.addOperand(llvm::MCOperand::createImm(cond));
    if(cond == 14)
        inst.addOperand(llvm::MCOperand::createReg(0));
    else
        inst.addOperand(llvm::MCOperand::createReg(3));

    return inst;
}

llvm::MCInst add(unsigned int dst, unsigned int src) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::ADDrr);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(src));
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

RelocatableInst::SharedPtr Str(Reg reg, Reg base, Offset offset) {
    return NoReloc(stri12(reg, base, offset));
}

RelocatableInst::SharedPtr Str(Reg reg, Offset offset) {
    return DataBlockRel(stri12(reg, Reg(REG_PC), 0), 2, offset - 8);
}

RelocatableInst::SharedPtr Str(Reg reg, Constant constant) {
    return MemoryConstant(stri12(reg, Reg(REG_PC), 0), 2, constant);
}

RelocatableInst::SharedPtr Ldr(Reg reg, Reg base, Offset offset) {
    return NoReloc(ldri12(reg, base, offset));
}

RelocatableInst::SharedPtr Ldr(Reg reg, Offset offset) {
    return DataBlockRel(ldri12(reg, Reg(REG_PC), 0), 2, offset - 8);
}

RelocatableInst::SharedPtr Ldr(Reg reg, Constant constant) {
    return MemoryConstant(ldri12(reg, Reg(REG_PC), 0), 2, constant);
};

RelocatableInst::SharedPtr Vstrs(unsigned int reg, unsigned int base, rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::VSTRS);
    inst.addOperand(llvm::MCOperand::createReg(reg));
    inst.addOperand(llvm::MCOperand::createReg(base));

    // LLVM consistency
    Require("Vstrs", offset % 4 == 0);
    inst.addOperand(llvm::MCOperand::createImm(offset/4));

    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return NoReloc(inst);
}

RelocatableInst::SharedPtr Vldrs(unsigned int reg, unsigned int base, rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::VLDRS);
    inst.addOperand(llvm::MCOperand::createReg(reg));
    inst.addOperand(llvm::MCOperand::createReg(base));
    // LLVM consistency
    Require("Vldrs", offset % 4 == 0);
    inst.addOperand(llvm::MCOperand::createImm(offset/4));

    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return NoReloc(inst);
}

RelocatableInst::SharedPtr Adr(Reg reg, rword offset) {
    return NoReloc(adr(reg, offset));
}

RelocatableInst::SharedPtr Adr(Reg reg, Offset offset) {
    return DataBlockRel(adr(reg, 0), 1, offset - 8);
}

RelocatableInst::SharedPtr Msr(Reg reg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::MSR);
    inst.addOperand(llvm::MCOperand::createImm(9));
    inst.addOperand(llvm::MCOperand::createReg(reg));
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return NoReloc(inst);
}

RelocatableInst::SharedPtr Mrs(Reg reg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::MRS);
    inst.addOperand(llvm::MCOperand::createReg(reg));
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return NoReloc(inst);
}

RelocatableInst::SharedPtr Popr(Reg reg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::LDR_POST_IMM);
    inst.addOperand(llvm::MCOperand::createReg(reg));
    inst.addOperand(llvm::MCOperand::createReg(Reg(REG_SP)));
    inst.addOperand(llvm::MCOperand::createReg(Reg(REG_SP)));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createImm(4));
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return NoReloc(inst);
}

RelocatableInst::SharedPtr Pushr(Reg reg) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::STR_PRE_IMM);
    inst.addOperand(llvm::MCOperand::createReg(Reg(REG_SP)));
    inst.addOperand(llvm::MCOperand::createReg(reg));
    inst.addOperand(llvm::MCOperand::createReg(Reg(REG_SP)));
    inst.addOperand(llvm::MCOperand::createImm(-4));
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return NoReloc(inst);
}

}
