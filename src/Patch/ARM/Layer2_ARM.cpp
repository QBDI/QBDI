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

llvm::MCInst t2ldri12(unsigned int reg, unsigned int base, rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::t2LDRi12);
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

llvm::MCInst t2stri12(unsigned int reg, unsigned int base, rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::t2STRi12);
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

llvm::MCInst t2adr(unsigned int reg, rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::t2ADR);
    inst.addOperand(llvm::MCOperand::createReg(reg));
    inst.addOperand(llvm::MCOperand::createImm(offset));
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

llvm::MCInst blxi(rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::BLXi);
    inst.addOperand(llvm::MCOperand::createImm(offset));

    return inst;
}

llvm::MCInst tblxi(rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::tBLXi);
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createImm(offset));

    return inst;
}

llvm::MCInst bl(rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::BL);
    inst.addOperand(llvm::MCOperand::createImm(offset));
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

llvm::MCInst tbl(rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::tBL);
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createImm(offset));

    return inst;
}

llvm::MCInst movr(unsigned int dst, unsigned int src) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::MOVr);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(src));
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

llvm::MCInst tmovr(unsigned int dst, unsigned int src) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::tMOVr);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createReg(src));
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

llvm::MCInst movi16(unsigned int dst, int64_t imm) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::MOVi16);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createImm(imm));
    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return inst;
}

llvm::MCInst t2movi16(unsigned int dst, int64_t imm) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::t2MOVi16);
    inst.addOperand(llvm::MCOperand::createReg(dst));
    inst.addOperand(llvm::MCOperand::createImm(imm));
    inst.addOperand(llvm::MCOperand::createImm(14));
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

RelocatableInst::SharedPtr Str(CPUMode cpuMode, Reg reg, Reg base, Offset offset) {
    if(cpuMode == CPUMode::ARM) {
        return NoReloc(stri12(reg, base, offset));
    }
    else if(cpuMode == CPUMode::Thumb) {
        return NoReloc(t2stri12(reg, base, offset));
    }
    _QBDI_UNREACHABLE();
}

RelocatableInst::SharedPtr Str(CPUMode cpuMode, Reg reg, Offset offset) {
    if(cpuMode == CPUMode::ARM) {
        return DataBlockRel(stri12(reg, Reg(REG_PC), 0), 2, offset);
    }
    else if(cpuMode == CPUMode::Thumb) {
        return DataBlockRel(t2stri12(reg, Reg(REG_PC), 0), 2, offset);
    }
    _QBDI_UNREACHABLE();
}

RelocatableInst::SharedPtr Str(CPUMode cpuMode, Reg reg, Constant constant) {
    if(cpuMode == CPUMode::ARM) {
        return MemoryConstant(stri12(reg, Reg(REG_PC), 0), 2, constant);
    }
    else if(cpuMode == CPUMode::Thumb) {
        return MemoryConstant(t2stri12(reg, Reg(REG_PC), 0), 2, constant);
    }
    _QBDI_UNREACHABLE();
}

RelocatableInst::SharedPtr Ldr(CPUMode cpuMode, Reg reg, Reg base, Offset offset) {
    if(cpuMode == CPUMode::ARM) {
        return NoReloc(ldri12(reg, base, offset));
    }
    else if(cpuMode == CPUMode::Thumb) {
        return NoReloc(t2ldri12(reg, base, offset));
    }
    _QBDI_UNREACHABLE();
}

RelocatableInst::SharedPtr Ldr(CPUMode cpuMode, Reg reg, Offset offset) {
    if(cpuMode == CPUMode::ARM) {
        return DataBlockRel(ldri12(reg, Reg(REG_PC), 0), 2, offset);
    }
    else if(cpuMode == CPUMode::Thumb) {
        return DataBlockRel(t2ldri12(reg, Reg(REG_PC), 0), 2, offset);
    }
    _QBDI_UNREACHABLE();
}

RelocatableInst::SharedPtr Ldr(CPUMode cpuMode, Reg reg, Constant constant) {
    if(cpuMode == CPUMode::ARM) {
        return MemoryConstant(ldri12(reg, Reg(REG_PC), 0), 2, constant);
    }
    else if(cpuMode == CPUMode::Thumb) {
        return MemoryConstant(t2ldri12(reg, Reg(REG_PC), 0), 2, constant);
    }
    _QBDI_UNREACHABLE();
};

RelocatableInst::SharedPtr LdrInstID(CPUMode cpuMode, Reg reg) {
    if(cpuMode == CPUMode::ARM) {
        return InstId(ldri12(reg, Reg(REG_PC), 0), 2);
    }
    else if(cpuMode == CPUMode::Thumb) {
        return InstId(t2ldri12(reg, Reg(REG_PC), 0), 2);
    }
    _QBDI_UNREACHABLE();
}

RelocatableInst::SharedPtr Mov(CPUMode cpuMode, Reg dst, Reg src) {
    if(cpuMode == CPUMode::ARM) {
        return NoReloc(movr(dst, src));
    }
    else if(cpuMode == CPUMode::Thumb) {
        return NoReloc(tmovr(dst, src));
    }
    _QBDI_UNREACHABLE();
}

RelocatableInst::SharedPtr Mov(CPUMode cpuMode, Reg dst, unsigned int src) {
    if(cpuMode == CPUMode::ARM) {
        return NoReloc(movr(dst, src));
    }
    else if(cpuMode == CPUMode::Thumb) {
        return NoReloc(tmovr(dst, src));
    }
    _QBDI_UNREACHABLE();
}

RelocatableInst::SharedPtr Mov(CPUMode cpuMode, Reg dst, Constant constant) {
    if(cpuMode == CPUMode::ARM) {
        return NoReloc(movi16(dst, constant));
    }
    else if(cpuMode == CPUMode::Thumb) {
        return NoReloc(t2movi16(dst, constant));
    }
    _QBDI_UNREACHABLE();
}

RelocatableInst::SharedPtr Vstrs(unsigned int reg, unsigned int base, rword offset) {
    llvm::MCInst inst;

    inst.setOpcode(llvm::ARM::VSTRS);
    inst.addOperand(llvm::MCOperand::createReg(reg));
    inst.addOperand(llvm::MCOperand::createReg(base));

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

    Require("Vldrs", offset % 4 == 0);
    inst.addOperand(llvm::MCOperand::createImm(offset/4));

    inst.addOperand(llvm::MCOperand::createImm(14));
    inst.addOperand(llvm::MCOperand::createReg(0));

    return NoReloc(inst);
}

RelocatableInst::SharedPtr Adr(CPUMode cpuMode, Reg reg, rword offset) {
    if(cpuMode == CPUMode::ARM) {
        return NoReloc(adr(reg, offset));
    }
    else if(cpuMode == CPUMode::Thumb) {
        return AdjustPCAlign(t2adr(reg, offset), 1);
    }
    _QBDI_UNREACHABLE();
}

RelocatableInst::SharedPtr Adr(CPUMode cpuMode, Reg reg, Offset offset) {
    if(cpuMode == CPUMode::ARM) {
        return DataBlockRel(adr(reg, 0), 1, offset);
    }
    else if(cpuMode == CPUMode::Thumb) {
        return DataBlockRel(t2adr(reg, 0), 1, offset);
    }
    _QBDI_UNREACHABLE();
}

RelocatableInst::SharedPtr BlxEpilogue(CPUMode cpuMode) {
    if(cpuMode == CPUMode::ARM) {
        return EpilogueRel(blxi(0), 0, 0);
    }
    else if(cpuMode == CPUMode::Thumb) {
        return EpilogueRel(tblxi(0), 2, 0);
    }
    _QBDI_UNREACHABLE();
}

RelocatableInst::SharedPtr BlEpilogue(CPUMode cpuMode) {
    if(cpuMode == CPUMode::ARM) {
        return EpilogueRel(bl(0), 0, 0);
    }
    else if(cpuMode == CPUMode::Thumb) {
        return EpilogueRel(tbl(0), 2, 0);
    }
    _QBDI_UNREACHABLE();
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
