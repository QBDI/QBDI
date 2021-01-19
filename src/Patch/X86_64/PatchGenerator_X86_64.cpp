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
#include <vector>
#include <memory>

#include "X86InstrInfo.h"

#include "Patch/X86_64/Layer2_X86_64.h"
#include "Patch/X86_64/PatchGenerator_X86_64.h"
#include "Patch/X86_64/RelocatableInst_X86_64.h"
#include "Utility/LogSys.h"


namespace QBDI {

RelocatableInst::SharedPtrVec GetOperand::generate(const llvm::MCInst* inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {
    if(inst->getOperand(op).isReg()) {
        return {NoReloc(movrr(temp_manager->getRegForTemp(temp), inst->getOperand(op).getReg()))};
    }
    else if(inst->getOperand(op).isImm()) {
        return {Mov(temp_manager->getRegForTemp(temp), Constant(inst->getOperand(op).getImm()))};
    }
    else {
        LogError("GetOperand::generate", "Invalid operand type for GetOperand()");
        return {};
    }
}

RelocatableInst::SharedPtrVec GetConstant::generate(const llvm::MCInst* inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {

    return {Mov(temp_manager->getRegForTemp(temp), cst)};
}

RelocatableInst::SharedPtrVec GetPCOffset::generate(const llvm::MCInst* inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {
    if(type == ConstantType) {
        return{Mov(temp_manager->getRegForTemp(temp), Constant(address + instSize + cst))};
    }
    else if(type == OperandType) {
        // FIXME: Implement for register operand
        return {Mov(
            temp_manager->getRegForTemp(temp),
            Constant(address + instSize + inst->getOperand(op).getImm()))
        };
    }
    _QBDI_UNREACHABLE();
}

RelocatableInst::SharedPtrVec GetReadAddress::generate(const llvm::MCInst* inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {
    // Check if this instruction does indeed read something
    unsigned size = getReadSize(*inst);
    if(size > 0) {
        uint64_t TSFlags = MCII->get(inst->getOpcode()).TSFlags;
        unsigned FormDesc = TSFlags & llvm::X86II::FormMask;
        unsigned memIndex = llvm::X86II::getMemoryOperandNo(TSFlags);

        // If it is a stack read, return RSP value
        if(isStackRead(*inst)) {
            if (inst->getOpcode() == llvm::X86::LEAVE || inst->getOpcode() == llvm::X86::LEAVE64) {
                return {Mov(temp_manager->getRegForTemp(temp), Reg(REG_BP))};
            } else {
                return {Mov(temp_manager->getRegForTemp(temp), Reg(REG_SP))};
            }
        }
        // specific case for instruction type cmpsb and movsb
        else if (FormDesc == llvm::X86II::RawFrmDstSrc ||
                 FormDesc == llvm::X86II::RawFrmDst ||
                 FormDesc == llvm::X86II::RawFrmSrc) {
            RequireAction("GetReadAddress::generate", index < 2 && "Wrong index", abort());
            unsigned int reg;
            if (FormDesc == llvm::X86II::RawFrmSrc ||
                (FormDesc == llvm::X86II::RawFrmDstSrc && index == 0)) {
                // (R|E)SI
                reg = Reg(4);
                Require("GetReadAddress::generate", reg == llvm::X86::RSI || reg == llvm::X86::ESI);
            } else {
                // (R|E)DI
                reg = Reg(5);
                Require("GetReadAddress::generate", reg == llvm::X86::RDI || reg == llvm::X86::EDI);
            }
            return {NoReloc(movrr(temp_manager->getRegForTemp(temp), reg))};
        }
        // Else replace the instruction with a LEA on the same address
        else if (memIndex >= 0) {
            // Scan for LLVM X86 address encoding
            for(unsigned i = memIndex; i + 4 <= inst->getNumOperands(); i++) {
                if(inst->getOperand(i + 0).isReg() && inst->getOperand(i + 1).isImm() &&
                   inst->getOperand(i + 2).isReg() && inst->getOperand(i + 3).isImm() &&
                   inst->getOperand(i + 4).isReg()) {
                    // If it uses PC as a base register, substitute PC
                    if(inst->getOperand(i + 0).getReg() == Reg(REG_PC)) {
                        return {
                            Mov(
                                temp_manager->getRegForTemp(0xFFFFFFFF),
                                Constant(address + instSize)
                            ),
                            NoReloc(lea(
                                temp_manager->getRegForTemp(temp),
                                temp_manager->getRegForTemp(0xFFFFFFFF),
                                inst->getOperand(i + 1).getImm(),
                                inst->getOperand(i + 2).getReg(),
                                inst->getOperand(i + 3).getImm(),
                                inst->getOperand(i + 4).getReg())
                            )
                        };
                    }
                    else {
                        return {
                            NoReloc(lea(
                                temp_manager->getRegForTemp(temp),
                                inst->getOperand(i + 0).getReg(),
                                inst->getOperand(i + 1).getImm(),
                                inst->getOperand(i + 2).getReg(),
                                inst->getOperand(i + 3).getImm(),
                                inst->getOperand(i + 4).getReg()
                            ))
                        };
                    }
                }
            }
            RequireAction("GetReadAddress::generate", false && "No memory address found in the instruction", abort());
        }
    }
    RequireAction("GetReadAddress::generate", false && "Called on an instruction which does not make read access", abort());
}

RelocatableInst::SharedPtrVec GetWriteAddress::generate(const llvm::MCInst* inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {
    // Check if this instruction does indeed read something
    unsigned size = getWriteSize(*inst);
    if(size > 0) {
        uint64_t TSFlags = MCII->get(inst->getOpcode()).TSFlags;
        unsigned FormDesc = TSFlags & llvm::X86II::FormMask;
        unsigned memIndex = llvm::X86II::getMemoryOperandNo(TSFlags);

        // If it is a stack read, return RSP value
        if(isStackWrite(*inst)) {
            if (inst->getOpcode() == llvm::X86::ENTER) {
                return {Mov(temp_manager->getRegForTemp(temp), Reg(REG_BP))};
            } else {
                return {Mov(temp_manager->getRegForTemp(temp), Reg(REG_SP))};
            }
        }
        else if (FormDesc == llvm::X86II::RawFrmDstSrc ||
                 FormDesc == llvm::X86II::RawFrmDst ||
                 FormDesc == llvm::X86II::RawFrmSrc) {
            unsigned int reg;
            if (FormDesc == llvm::X86II::RawFrmSrc) {
                // (R|E)SI
                reg = Reg(4);
                Require("GetWriteAddress::generate", reg == llvm::X86::RSI || reg == llvm::X86::ESI);
            } else {
                // (R|E)DI
                reg = Reg(5);
                Require("GetWriteAddress::generate", reg == llvm::X86::RDI || reg == llvm::X86::EDI);
            }
            return {NoReloc(movrr(temp_manager->getRegForTemp(temp), reg))};
        }
        // Else replace the instruction with a LEA on the same address
        else if (memIndex >= 0) {
            // Scan for LLVM X86 address encoding
            for(unsigned i = memIndex; i + 4 <= inst->getNumOperands(); i++) {
                if(inst->getOperand(i + 0).isReg() && inst->getOperand(i + 1).isImm() &&
                   inst->getOperand(i + 2).isReg() && inst->getOperand(i + 3).isImm() &&
                   inst->getOperand(i + 4).isReg()) {
                    // If it uses PC as a base register, substitute PC
                    if(inst->getOperand(i + 0).getReg() == Reg(REG_PC)) {
                        return {
                            Mov(
                                temp_manager->getRegForTemp(0xFFFFFFFF),
                                Constant(address + instSize)
                            ),
                            NoReloc(lea(
                                temp_manager->getRegForTemp(temp),
                                temp_manager->getRegForTemp(0xFFFFFFFF),
                                inst->getOperand(i + 1).getImm(),
                                inst->getOperand(i + 2).getReg(),
                                inst->getOperand(i + 3).getImm(),
                                inst->getOperand(i + 4).getReg())
                            )
                        };
                    }
                    else {
                        return {
                            NoReloc(lea(
                                temp_manager->getRegForTemp(temp),
                                inst->getOperand(i + 0).getReg(),
                                inst->getOperand(i + 1).getImm(),
                                inst->getOperand(i + 2).getReg(),
                                inst->getOperand(i + 3).getImm(),
                                inst->getOperand(i + 4).getReg()
                            ))
                        };
                    }
                }
            }
            RequireAction("GetWriteAddress::generate", false && "No memory address found in the instruction", abort());
        }
    }
    RequireAction("GetWriteAddress::generate", false && "Called on an instruction which does not make write access", abort());
}

RelocatableInst::SharedPtrVec GetReadValue::generate(const llvm::MCInst* inst,
     rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {
    const unsigned size = getReadSize(*inst);
    if(size > 0) {
        uint64_t TSFlags = MCII->get(inst->getOpcode()).TSFlags;
        unsigned FormDesc = TSFlags & llvm::X86II::FormMask;
        unsigned memIndex = llvm::X86II::getMemoryOperandNo(TSFlags);

        unsigned dst = temp_manager->getRegForTemp(temp);
        if(is_bits_64 && size < sizeof(rword)) {
            dst = temp_manager->getSizedSubReg(dst, 4);
        } else if (size > sizeof(rword)) {
            return {NoReloc(movri(dst, 0))};
        }

        if(isStackRead(*inst)) {
            llvm::MCInst readinst;
            unsigned int stack_register = REG_SP;
            if (inst->getOpcode() == llvm::X86::LEAVE || inst->getOpcode() == llvm::X86::LEAVE64) {
                stack_register = REG_BP;
            }
            if(size == 8) {
                readinst = mov64rm(dst, Reg(stack_register), 1, 0, 0, 0);
            }
            else if(size == 4) {
                readinst = mov32rm(dst, Reg(stack_register), 1, 0, 0, 0);
            }
            else if(size == 2) {
                readinst = mov32rm16(dst, Reg(stack_register), 1, 0, 0, 0);
            }
            else if(size == 1) {
                readinst = mov32rm8(dst, Reg(stack_register), 1, 0, 0, 0);
            }
            return {NoReloc(std::move(readinst))};
        }
        else if (FormDesc == llvm::X86II::RawFrmDstSrc ||
                 FormDesc == llvm::X86II::RawFrmDst ||
                 FormDesc == llvm::X86II::RawFrmSrc) {

            RequireAction("GetReadValue::generate", index < 2 && "Wrong index", abort());
            unsigned int reg;
            if (FormDesc == llvm::X86II::RawFrmSrc ||
                (FormDesc == llvm::X86II::RawFrmDstSrc && index == 0)) {
                // (R|E)SI
                reg = Reg(4);
                Require("GetReadValue::generate", reg == llvm::X86::RSI || reg == llvm::X86::ESI);
            } else {
                // (R|E)DI
                reg = Reg(5);
                Require("GetReadValue::generate", reg == llvm::X86::RDI || reg == llvm::X86::EDI);
            }

            if(size == 8) {
                return {NoReloc(mov64rm(dst, reg, 1, 0, 0, 0))};
            }
            else if(size == 4) {
                return {NoReloc(mov32rm(dst, reg, 1, 0, 0, 0))};
            }
            else if(size == 2) {
                return {NoReloc(mov32rm16(dst, reg, 1, 0, 0, 0))};
            }
            else if(size == 1) {
                return {NoReloc(mov32rm8(dst, reg, 1, 0, 0, 0))};
            }
        }
        else if (memIndex >= 0) {
            for(unsigned i = memIndex; i + 4 <= inst->getNumOperands(); i++) {
                if(inst->getOperand(i + 0).isReg() && inst->getOperand(i + 1).isImm() &&
                   inst->getOperand(i + 2).isReg() && inst->getOperand(i + 3).isImm() &&
                   inst->getOperand(i + 4).isReg()) {
                    llvm::MCInst readinst;

                    unsigned base = inst->getOperand(i + 0).getReg();
                    rword scale = inst->getOperand(i + 1).getImm();
                    unsigned offset = inst->getOperand(i + 2).getReg();
                    rword displacement = inst->getOperand(i + 3).getImm();
                    unsigned seg = inst->getOperand(i + 4).getReg();

                    if(inst->getOperand(i + 0).getReg() == Reg(REG_PC)) {
                        base = temp_manager->getRegForTemp(0xFFFFFFFF);
                    }

                    if(size == 8) {
                        readinst = mov64rm(dst, base, scale, offset, displacement, seg);
                    }
                    else if(size == 4) {
                        readinst = mov32rm(dst, base, scale, offset, displacement, seg);
                    }
                    else if(size == 2) {
                        readinst = mov32rm16(dst, base, scale, offset, displacement, seg);
                    }
                    else if(size == 1) {
                        readinst = mov32rm8(dst, base, scale, offset, displacement, seg);
                    }

                    if(inst->getOperand(i + 0).getReg() == Reg(REG_PC)) {
                        return {
                            Mov(
                                temp_manager->getRegForTemp(0xFFFFFFFF),
                                Constant(address + instSize)
                            ),
                            NoReloc(std::move(readinst))
                        };
                    }
                    else {
                        return {
                            NoReloc(std::move(readinst))
                        };
                    }
                }
            }
            RequireAction("GetReadValue::generate", false && "No memory address found in the instruction", abort());
        }
    }
    RequireAction("GetReadValue::generate", false && "Called on an instruction which does not make read access", abort());
}

RelocatableInst::SharedPtrVec GetWriteValue::generate(const llvm::MCInst* inst,
     rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {
    const unsigned size = getWriteSize(*inst);
    if(size > 0) {
        uint64_t TSFlags = MCII->get(inst->getOpcode()).TSFlags;
        unsigned FormDesc = TSFlags & llvm::X86II::FormMask;
        unsigned memIndex = llvm::X86II::getMemoryOperandNo(TSFlags);

        unsigned src_addr = temp_manager->getRegForTemp(temp);
        unsigned dst = src_addr;
        if(is_bits_64 && size < sizeof(rword)) {
            dst = temp_manager->getSizedSubReg(dst, 4);
        } else if (size > sizeof(rword)) {
            return {NoReloc(movri(dst, 0))};
        }

        if(isStackWrite(*inst)) {
            llvm::MCInst readinst;
            unsigned int stack_register = REG_SP;
            if (inst->getOpcode() == llvm::X86::ENTER) {
                stack_register = REG_BP;
            }
            if(size == 8) {
                readinst = mov64rm(dst, Reg(stack_register), 1, 0, 0, 0);
            }
            else if(size == 4) {
                readinst = mov32rm(dst, Reg(stack_register), 1, 0, 0, 0);
            }
            else if(size == 2) {
                readinst = mov32rm16(dst, Reg(stack_register), 1, 0, 0, 0);
            }
            else if(size == 1) {
                readinst = mov32rm8(dst, Reg(stack_register), 1, 0, 0, 0);
            }
            return {NoReloc(std::move(readinst))};
        }
        else if (FormDesc == llvm::X86II::RawFrmDstSrc ||
                 FormDesc == llvm::X86II::RawFrmDst ||
                 FormDesc == llvm::X86II::RawFrmSrc) {
            // As the address is already in temp, just dereference it.
            if(size == 8) {
                return {NoReloc(mov64rm(dst, src_addr, 1, 0, 0, 0))};
            }
            else if(size == 4) {
                return {NoReloc(mov32rm(dst, src_addr, 1, 0, 0, 0))};
            }
            else if(size == 2) {
                return {NoReloc(mov32rm16(dst, src_addr, 1, 0, 0, 0))};
            }
            else if(size == 1) {
                return {NoReloc(mov32rm8(dst, src_addr, 1, 0, 0, 0))};
            }
        }
        else if (memIndex >= 0) {
            for(unsigned i = memIndex; i + 4 <= inst->getNumOperands(); i++) {
                if(inst->getOperand(i + 0).isReg() && inst->getOperand(i + 1).isImm() &&
                   inst->getOperand(i + 2).isReg() && inst->getOperand(i + 3).isImm() &&
                   inst->getOperand(i + 4).isReg()) {

                    unsigned seg = inst->getOperand(i + 4).getReg();

                    if(size == 8) {
                        return {NoReloc(mov64rm(dst, src_addr, 1, 0, 0, seg))};
                    }
                    else if(size == 4) {
                        return {NoReloc(mov32rm(dst, src_addr, 1, 0, 0, seg))};
                    }
                    else if(size == 2) {
                        return {NoReloc(mov32rm16(dst, src_addr, 1, 0, 0, seg))};
                    }
                    else if(size == 1) {
                        return {NoReloc(mov32rm8(dst, src_addr, 1, 0, 0, seg))};
                    }
                }
            }
            RequireAction("GetWriteValue::generate", false && "No memory address found in the instruction", abort());
        }
    }
    RequireAction("GetWriteValue::generate", false && "Called on an instruction which does not make write access", abort());
}

RelocatableInst::SharedPtrVec GetInstId::generate(const llvm::MCInst* inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {

    return {InstId(movri(temp_manager->getRegForTemp(temp), 0), 1)};
}

RelocatableInst::SharedPtrVec ReadTemp::generate(const llvm::MCInst* inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {

    if(type == OffsetType) {
        return {Mov(temp_manager->getRegForTemp(temp), offset)};
    }
    else if(type == ShadowType) {
        return {Mov(temp_manager->getRegForTemp(temp), shadow)};
    }
    _QBDI_UNREACHABLE();
}

RelocatableInst::SharedPtrVec WriteTemp::generate(const llvm::MCInst* inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {

    if(type == OffsetType) {
        return {Mov(offset, temp_manager->getRegForTemp(temp))};
    }
    else if(type == ShadowType) {
        return {Mov(shadow, temp_manager->getRegForTemp(temp))};
    }
    _QBDI_UNREACHABLE();
}


RelocatableInst::SharedPtrVec SaveReg::generate(const llvm::MCInst* inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {

    return {Mov(offset, reg)};
}

RelocatableInst::SharedPtrVec LoadReg::generate(const llvm::MCInst* inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {

    return {Mov(reg, offset)};
}

RelocatableInst::SharedPtrVec JmpEpilogue::generate(const llvm::MCInst* inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {

    return {EpilogueRel(jmp(0), 0, -1)};
}

RelocatableInst::SharedPtrVec SimulateCall::generate(const llvm::MCInst* inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {
    RelocatableInst::SharedPtrVec patch;

    append(patch, WriteTemp(temp, Offset(Reg(REG_PC))).generate(inst, address, instSize, MCII, temp_manager, nullptr));
    append(patch, GetPCOffset(temp, Constant(0)).generate(inst, address, instSize, MCII, temp_manager, nullptr));
    patch.push_back(Pushr(temp_manager->getRegForTemp(temp)));

    return {patch};
}

RelocatableInst::SharedPtrVec SimulateRet::generate(const llvm::MCInst* inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {
    RelocatableInst::SharedPtrVec patch;

    patch.push_back(Popr(temp_manager->getRegForTemp(temp)));
    if(inst->getNumOperands() == 1 && inst->getOperand(0).isImm()) {
        patch.push_back(Add(Reg(REG_SP), Constant(inst->getOperand(0).getImm())));
    }
    append(patch, WriteTemp(temp, Offset(Reg(REG_PC))).generate(inst, address, instSize, MCII, temp_manager, nullptr));

    return {patch};
}

}
