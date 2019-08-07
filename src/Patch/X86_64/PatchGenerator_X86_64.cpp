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

#include "Patch/X86_64/PatchGenerator_X86_64.h"
#include "Patch/X86_64/RelocatableInst_X86_64.h"
#include "Utility/LogSys.h"


namespace QBDI {

RelocatableInst::SharedPtrVec GetOperand::generate(const llvm::MCInst* inst,
    rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) {
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
    rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) {

    return {Mov(temp_manager->getRegForTemp(temp), cst)};
}

RelocatableInst::SharedPtrVec GetPCOffset::generate(const llvm::MCInst* inst,
    rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) {
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
    rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) {
    // Check if this instruction does indeed read something
    if(getReadSize(inst) > 0) {
        // If it is a stack read, return RSP value
        if(isStackRead(inst)) {
            if (inst->getOpcode() == llvm::X86::LEAVE || inst->getOpcode() == llvm::X86::LEAVE64) {
                return {Mov(temp_manager->getRegForTemp(temp), Reg(REG_BP))};
            } else {
                return {Mov(temp_manager->getRegForTemp(temp), Reg(REG_SP))};
            }
        // specific case for instruction type cmpsb and movsb
        } else if ((TSFlags & llvm::X86II::FormMask) == llvm::X86II::RawFrmDstSrc) {
            RequireAction("GetReadAddress::generate", index < 2 && "Wrong index", abort());
            unsigned int reg;
            if (index == 0) {
                // (R|E)SI
                reg = GPR_ID[4];
                Require("GetReadAddress::generate", reg == llvm::X86::RSI || reg == llvm::X86::ESI);
            } else {
                // (R|E)DI
                reg = GPR_ID[5];
                Require("GetReadAddress::generate", reg == llvm::X86::RDI || reg == llvm::X86::EDI);
            }
            return {NoReloc(movrr(temp_manager->getRegForTemp(temp), reg))};
        // Else replace the instruction with a LEA on the same address
        } else {
            // Scan for LLVM X86 address encoding
            for(unsigned i = 0; i + 4 <= inst->getNumOperands(); i++) {
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
    rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) {
    // Check if this instruction does indeed read something
    if(getWriteSize(inst) > 0) {
        // If it is a stack read, return RSP value
        if(isStackWrite(inst)) {
            if (inst->getOpcode() == llvm::X86::ENTER) {
                return {Mov(temp_manager->getRegForTemp(temp), Reg(REG_BP))};
            } else {
                return {Mov(temp_manager->getRegForTemp(temp), Reg(REG_SP))};
            }
        }
        // Else replace the instruction with a LEA on the same address
        else {
            // Scan for LLVM X86 address encoding
            for(unsigned i = 0; i + 4 <= inst->getNumOperands(); i++) {
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
     rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) {
    if(getReadSize(inst) > 0) {
        unsigned size = getReadSize(inst);
        unsigned dst = temp_manager->getRegForTemp(temp);
        if(size < 8) {
            dst = temp_manager->getSizedSubReg(dst, 4);
        } else if (size > sizeof(rword)) {
            // read value size greater than 8 bytes isn't implemented
            return {NoReloc(movri(dst, 0))};
        }
        if(isStackRead(inst)) {
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
            return {NoReloc(readinst)};
        } else if ((TSFlags & llvm::X86II::FormMask) == llvm::X86II::RawFrmDstSrc) {
            llvm::MCInst readinst;
            RequireAction("GetReadAddress::generate", index < 2 && "Wrong index", abort());
            unsigned int reg;
            if (index == 0) {
                // (R|E)SI
                reg = GPR_ID[4];
                Require("GetReadAddress::generate", reg == llvm::X86::RSI || reg == llvm::X86::ESI);
            } else {
                // (R|E)DI
                reg = GPR_ID[5];
                Require("GetReadAddress::generate", reg == llvm::X86::RDI || reg == llvm::X86::EDI);
            }
            if(size == 8) {
                readinst = {mov64rm(dst, reg, 1, 0, 0, 0)};
            }
            else if(size == 4) {
                readinst = {mov32rm(dst, reg, 1, 0, 0, 0)};
            }
            else if(size == 2) {
                readinst = {mov32rm16(dst, reg, 1, 0, 0, 0)};
            }
            else if(size == 1) {
                readinst = {mov32rm8(dst, reg, 1, 0, 0, 0)};
            }
            return {NoReloc(readinst)};
        } else {
            for(unsigned i = 0; i + 4 <= inst->getNumOperands(); i++) {
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
                            NoReloc(readinst)
                        };
                    }
                    else {
                        return {
                            NoReloc(readinst)
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
     rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) {
    if(getWriteSize(inst) > 0) {
        unsigned size = getWriteSize(inst);
        unsigned dst = temp_manager->getRegForTemp(temp);
        if(size < 8) {
            dst = temp_manager->getSizedSubReg(dst, 4);
        } else if (size > sizeof(rword)) {
            // write value size greater than 8 bytes isn't implemented
            return {NoReloc(movri(dst, 0))};
        }

        if(isStackWrite(inst)) {
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
            return {NoReloc(readinst)};
        } else {
            for(unsigned i = 0; i + 4 <= inst->getNumOperands(); i++) {
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
                            NoReloc(readinst)
                        };
                    }
                    else {
                        return {
                            NoReloc(readinst)
                        };
                    }
                }
            }
            RequireAction("GetWriteValue::generate", false && "No memory address found in the instruction", abort());
        }
    }
    RequireAction("GetWriteValue::generate", false && "Called on an instruction which does not make write access", abort());
}

RelocatableInst::SharedPtrVec GetInstId::generate(const llvm::MCInst* inst,
    rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) {

    return {InstId(movri(temp_manager->getRegForTemp(temp), 0), 1)};
}

RelocatableInst::SharedPtrVec WriteTemp::generate(const llvm::MCInst* inst,
    rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) {

    if(type == OffsetType) {
        return {Mov(offset, temp_manager->getRegForTemp(temp))};
    }
    else if(type == ShadowType) {
        return {TaggedShadowx86(
            movmr(0, 0, 0, 0, 0, temp_manager->getRegForTemp(temp)),
            3,
            shadow.getTag(),
            0,
            7
        )};
    }
    _QBDI_UNREACHABLE();
}

RelocatableInst::SharedPtrVec SaveReg::generate(const llvm::MCInst* inst,
    rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) {

    return {Mov(offset, reg)};
}

RelocatableInst::SharedPtrVec LoadReg::generate(const llvm::MCInst* inst,
    rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) {

    return {Mov(reg, offset)};
}

RelocatableInst::SharedPtrVec JmpEpilogue::generate(const llvm::MCInst* inst,
    rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) {

    return {EpilogueRel(jmp(0), 0, -1)};
}

RelocatableInst::SharedPtrVec SimulateCall::generate(const llvm::MCInst* inst,
    rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) {
    RelocatableInst::SharedPtrVec patch;

    append(patch, WriteTemp(temp, Offset(Reg(REG_PC))).generate(inst, address, instSize, temp_manager, nullptr));
    append(patch, GetPCOffset(temp, Constant(0)).generate(inst, address, instSize, temp_manager, nullptr));
    patch.push_back(Pushr(temp_manager->getRegForTemp(temp)));

    return {patch};
}

RelocatableInst::SharedPtrVec SimulateRet::generate(const llvm::MCInst* inst,
    rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) {
    RelocatableInst::SharedPtrVec patch;

    patch.push_back(Popr(temp_manager->getRegForTemp(temp)));
    if(inst->getNumOperands() == 1 && inst->getOperand(0).isImm()) {
        patch.push_back(Add(Reg(REG_SP), Constant(inst->getOperand(0).getImm())));
    }
    append(patch, WriteTemp(temp, Offset(Reg(REG_PC))).generate(inst, address, instSize, temp_manager, nullptr));

    return {patch};
}

PatchGenerator::SharedPtrVec generateReadInstrumentPatch(Patch &patch, llvm::MCInstrInfo* MCII,
                                                         llvm::MCRegisterInfo* MRI) {

    uint64_t TsFlags = MCII->get(patch.metadata.inst.getOpcode()).TSFlags;
    if (isDoubleRead(&patch.metadata.inst)) {
        return {
                    GetReadAddress(Temp(0), 0, TsFlags),
                    WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                    GetReadValue(Temp(0), 0, TsFlags),
                    WriteTemp(Temp(0), Shadow(MEM_VALUE_TAG)),
                    GetReadAddress(Temp(0), 1, TsFlags),
                    WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                    GetReadValue(Temp(0), 1, TsFlags),
                    WriteTemp(Temp(0), Shadow(MEM_VALUE_TAG)),
                };
    } else {
        return {
                    GetReadAddress(Temp(0), 0, TsFlags),
                    WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                    GetReadValue(Temp(0), 0, TsFlags),
                    WriteTemp(Temp(0), Shadow(MEM_VALUE_TAG)),
                };
    }
}

PatchGenerator::SharedPtrVec generateWriteInstrumentPatch(Patch &patch, llvm::MCInstrInfo* MCII,
                                                         llvm::MCRegisterInfo* MRI) {

    uint64_t TsFlags = MCII->get(patch.metadata.inst.getOpcode()).TSFlags;
    return {
                GetWriteAddress(Temp(0), TsFlags),
                WriteTemp(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                GetWriteValue(Temp(0), TsFlags),
                WriteTemp(Temp(0), Shadow(MEM_VALUE_TAG)),
            };
}

}
