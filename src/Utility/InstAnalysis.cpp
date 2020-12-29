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
#include <bitset>

#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"

#include "ExecBlock/Context.h"
#include "Patch/InstInfo.h"
#include "Patch/InstMetadata.h"
#include "Patch/RegisterSize.h"
#include "Utility/Assembly.h"
#include "Utility/InstAnalysis_prive.h"
#include "Utility/LogSys.h"

#include "State.h"

#ifndef QBDI_PLATFORM_WINDOWS
#if defined(QBDI_PLATFORM_LINUX) && !defined(__USE_GNU)
#define __USE_GNU
#endif
#include <dlfcn.h>
#endif

namespace QBDI {
namespace {

bool isFlagRegister(unsigned int regNo) {
    if (GPR_ID[REG_FLAG] == regNo) {
        return true;
    }
    for(size_t j = 0; j < size_FLAG_ID; j++) {
        if (FLAG_ID[j] != 0 && regNo == FLAG_ID[j]) {
            return true;
        }
    }
    return false;
}

void analyseRegister(OperandAnalysis& opa, unsigned int regNo, const llvm::MCRegisterInfo& MRI) {
    opa.regName = MRI.getName(regNo);
    if (opa.regName != nullptr && opa.regName[0] == '\x00')
        opa.regName = nullptr;
    opa.value = regNo;
    opa.size = 0;
    opa.flag = OPERANDFLAG_NONE;
    opa.regOff = 0;
    opa.regCtxIdx = -1;
    opa.type = OPERAND_INVALID;
    opa.regAccess = REGISTER_UNUSED;
    if (regNo == /* llvm::X86|ARM::NoRegister */ 0)
        return;
    // try to match register in our GPR context
    for (uint16_t j = 0; j < NUM_GPR; j++) {
        if (MRI.isSubRegisterEq(GPR_ID[j], regNo)) {
            if (GPR_ID[j] != regNo) {
                unsigned int subregidx = MRI.getSubRegIndex(GPR_ID[j], regNo);
                opa.regOff = MRI.getSubRegIdxOffset(subregidx);
            }
            opa.regCtxIdx = j;
            opa.size = getRegisterSize(regNo);
            opa.type = OPERAND_GPR;
            if (!opa.size)
                LogWarning("analyseRegister", "register %d (%s) with size null", regNo, opa.regName);
            return;
        }
    }
    std::map<unsigned int, int16_t>::const_iterator it = FPR_ID.find(regNo);
    if (it != FPR_ID.end()) {
        opa.regCtxIdx = it->second;
        opa.regOff = 0;
        opa.size = getRegisterSize(regNo);
        opa.type = OPERAND_FPR;
        if (!opa.size)
            LogWarning("analyseRegister", "register %d (%s) with size null", regNo, opa.regName);
        return;
    }
    for (uint16_t j = 0; j < size_SEG_ID; j++) {
        if (regNo == SEG_ID[j]) {
            opa.regOff = 0;
            opa.size = getRegisterSize(regNo);
            opa.type = OPERAND_SEG;
            if (!opa.size)
                LogWarning("analyseRegister", "register %d (%s) with size null", regNo, opa.regName);
            return;
        }
    }
    LogWarning("analyseRegister", "Unknown register %d : %s", regNo, opa.regName);
}

void tryMergeCurrentRegister(InstAnalysis* instAnalysis) {
    OperandAnalysis& opa = instAnalysis->operands[instAnalysis->numOperands - 1];
    if (opa.type != QBDI::OPERAND_GPR) {
        return;
    }
    if ((opa.flag & QBDI::OPERANDFLAG_IMPLICIT) == 0) {
        return;
    }
    for (uint16_t j = 0; j < instAnalysis->numOperands - 1; j++) {
        OperandAnalysis& pop = instAnalysis->operands[j];
        if (pop.type != opa.type || pop.flag != opa.flag) {
            continue;
        }
        if (pop.regCtxIdx == opa.regCtxIdx &&
                pop.size == opa.size &&
                pop.regOff == opa.regOff) {
            // merge current one into previous one
            pop.regAccess |= opa.regAccess;
            memset(&opa, 0, sizeof(OperandAnalysis));
            instAnalysis->numOperands--;
            break;
        }
    }
}


void analyseImplicitRegisters(InstAnalysis* instAnalysis, const uint16_t* implicitRegs,
                              RegisterAccessType type, const llvm::MCRegisterInfo& MRI) {
    if (!implicitRegs) {
        return;
    }
    // Iteration style copied from LLVM code
    for (; *implicitRegs; ++implicitRegs) {
        OperandAnalysis topa;
        llvm::MCPhysReg regNo = *implicitRegs;
        if (isFlagRegister(regNo)) {
            instAnalysis->flagsAccess |= type;
            continue;
        }
        analyseRegister(topa, *implicitRegs, MRI);
        // we found a GPR (as size is only known for GPR)
        // TODO: add support for more registers
        if (topa.size != 0 && topa.type != OPERAND_INVALID) {
            // fill a new operand analysis
            OperandAnalysis& opa = instAnalysis->operands[instAnalysis->numOperands];
            opa = topa;
            opa.regAccess = type;
            opa.flag |= OPERANDFLAG_IMPLICIT;
            instAnalysis->numOperands++;
            // try to merge with a previous one
            tryMergeCurrentRegister(instAnalysis);
        }
    }
}

void analyseOperands(InstAnalysis* instAnalysis, const llvm::MCInst& inst, const llvm::MCInstrDesc& desc, const llvm::MCRegisterInfo& MRI) {
    if (!instAnalysis) {
        // no instruction analysis
        return;
    }
    instAnalysis->numOperands = 0; // updated later because we could skip some
    instAnalysis->operands = NULL;
    // Analysis of instruction operands
    uint8_t numOperands = inst.getNumOperands();
    uint8_t numOperandsMax = numOperands + desc.getNumImplicitDefs() + desc.getNumImplicitUses();

    // (R|E)SP are missing for RET and CALL in x86
    if constexpr((is_x86_64 or is_x86)) {
        if ((desc.isReturn() and isStackRead(inst)) or (desc.isCall() and isStackWrite(inst)))
            numOperandsMax = numOperandsMax + 2;
    }
    if (numOperandsMax == 0) {
        // no operand to analyse
        return;
    }
    instAnalysis->operands = new OperandAnalysis[numOperandsMax]();
    // find written registers
    std::bitset<16> regWrites;
    for (unsigned i = 0,
            e = desc.isVariadic() ? inst.getNumOperands() : desc.getNumDefs();
            i != e; ++i) {
        const llvm::MCOperand& op = inst.getOperand(i);
        if (op.isReg()) {
            regWrites.set(i, true);
        }
    }
    // for each instruction operands
    for (uint8_t i = 0; i < numOperands; i++) {
        const llvm::MCOperand& op = inst.getOperand(i);
        const llvm::MCOperandInfo& opdesc = desc.OpInfo[i];
        // fill a new operand analysis
        OperandAnalysis& opa = instAnalysis->operands[instAnalysis->numOperands];
        // reinitialise the opa if a previous iteration has write some value
        memset(&opa, 0, sizeof(OperandAnalysis));
        opa.regCtxIdx = -1;
        if (!op.isValid()) {
            continue;
        }
        if (op.isReg()) {
            unsigned int regNo = op.getReg();
            // validate that this is really a register operand, not
            // something else (memory access)
            if (isFlagRegister(regNo)) {
                instAnalysis->flagsAccess |= regWrites.test(i) ? REGISTER_WRITE : REGISTER_READ;
                continue;
            }
            // fill the operand analysis
            analyseRegister(opa, regNo, MRI);
            // we have'nt found a GPR (as size is only known for GPR)
            switch (opdesc.OperandType) {
                case llvm::MCOI::OPERAND_REGISTER:
                    break;
                case llvm::MCOI::OPERAND_MEMORY:
                    opa.flag |= OPERANDFLAG_ADDR;
                    break;
                case llvm::MCOI::OPERAND_UNKNOWN:
                    opa.flag |= OPERANDFLAG_UNDEFINED_EFFECT;
                    break;
                default:
                    LogWarning("ExecBlockManager::analyseOperands",
                            "Not supported operandType %d for register operand", opdesc.OperandType);
            }
            if (opa.type != QBDI::OPERAND_INVALID)
                opa.regAccess = regWrites.test(i) ? REGISTER_WRITE : REGISTER_READ;
            instAnalysis->numOperands++;
        } else if (op.isImm()) {
            // fill the operand analysis
            switch (opdesc.OperandType) {
                case llvm::MCOI::OPERAND_IMMEDIATE:
                    opa.size = getImmediateSize(inst, &desc);
                    break;
                case llvm::MCOI::OPERAND_MEMORY:
                    opa.flag |= OPERANDFLAG_ADDR;
                    opa.size = sizeof(rword);
                    break;
                case llvm::MCOI::OPERAND_PCREL:
                    opa.size = getImmediateSize(inst, &desc);
                    opa.flag |= OPERANDFLAG_PCREL;
                    break;
                case llvm::MCOI::OPERAND_UNKNOWN:
                    opa.flag |= OPERANDFLAG_UNDEFINED_EFFECT;
                    opa.size = sizeof(rword);
                    break;
                default:
                    LogWarning("ExecBlockManager::analyseOperands",
                            "Not supported operandType %d for immediate operand", opdesc.OperandType);
                    continue;
            }
            if (opdesc.isPredicate()) {
                opa.type = OPERAND_PRED;
            } else {
                opa.type = OPERAND_IMM;
            }
            opa.value = static_cast<rword>(op.getImm());
            instAnalysis->numOperands++;
        }
    }

    // analyse implicit registers (R/W)
    analyseImplicitRegisters(instAnalysis, desc.getImplicitUses(), REGISTER_READ, MRI);
    analyseImplicitRegisters(instAnalysis, desc.getImplicitDefs(), REGISTER_WRITE, MRI);


    // (R|E)SP are missing for RET and CALL in x86
    if constexpr((is_x86_64 or is_x86)) {
        if ((desc.isReturn() and isStackRead(inst)) or (desc.isCall() and isStackWrite(inst))) {
            // use SP to read|write the return address
            OperandAnalysis& opa = instAnalysis->operands[instAnalysis->numOperands];
            analyseRegister(opa, GPR_ID[REG_SP], MRI);
            opa.regAccess = REGISTER_READ;
            opa.flag |= OPERANDFLAG_IMPLICIT | OPERANDFLAG_ADDR;
            instAnalysis->numOperands++;
            // try to merge with a previous one
            tryMergeCurrentRegister(instAnalysis);

            // increment or decrement SP
            OperandAnalysis& opa2 = instAnalysis->operands[instAnalysis->numOperands];
            analyseRegister(opa2, GPR_ID[REG_SP], MRI);
            opa2.regAccess = REGISTER_READ_WRITE;
            opa2.flag |= OPERANDFLAG_IMPLICIT;
            instAnalysis->numOperands++;
            // try to merge with a previous one
            tryMergeCurrentRegister(instAnalysis);
        }
    }
}

} // anonymous namespace


void InstAnalysisDestructor::operator()(InstAnalysis* ptr) const {
    if (ptr == nullptr) {
        return;
    }
    delete[] ptr->operands;
    delete[] ptr->disassembly;
    delete ptr;
}

const InstAnalysis* analyzeInstMetadata(const InstMetadata& instMetadata, AnalysisType type,
                                        const Assembly& assembly) {
    InstAnalysis* instAnalysis = instMetadata.analysis.get();
    if (instAnalysis == nullptr) {
        instAnalysis = new InstAnalysis;
        // set all values to NULL/0/false
        memset(instAnalysis, 0, sizeof(InstAnalysis));
        instMetadata.analysis.reset(instAnalysis);
    }

    uint32_t oldType = instAnalysis->analysisType;
    uint32_t newType = oldType | type;
    uint32_t missingAnalysis = oldType ^ newType;

    instAnalysis->analysisType      = newType;

    if (missingAnalysis == 0)
        return instAnalysis;

    const llvm::MCInstrInfo& MCII = assembly.getMCII();
    const llvm::MCRegisterInfo& MRI = assembly.getMRI();

    const llvm::MCInst &inst = instMetadata.inst;
    const llvm::MCInstrDesc &desc = MCII.get(inst.getOpcode());

    if (missingAnalysis & ANALYSIS_DISASSEMBLY) {
        int len = 0;
        std::string buffer;
        llvm::raw_string_ostream bufferOs(buffer);
        assembly.printDisasm(inst, instMetadata.address, bufferOs);
        bufferOs.flush();
        len = buffer.size() + 1;
        instAnalysis->disassembly = new char[len];
        strncpy(instAnalysis->disassembly, buffer.c_str(), len);
        buffer.clear();
    }

    if (missingAnalysis & ANALYSIS_INSTRUCTION) {
        instAnalysis->address           = instMetadata.address;
        instAnalysis->instSize          = instMetadata.instSize;
        instAnalysis->affectControlFlow = instMetadata.modifyPC;
        instAnalysis->isBranch          = desc.isBranch();
        instAnalysis->isCall            = desc.isCall();
        instAnalysis->isReturn          = desc.isReturn();
        instAnalysis->isCompare         = desc.isCompare();
        instAnalysis->isPredicable      = desc.isPredicable();
        instAnalysis->loadSize          = getReadSize(inst);
        instAnalysis->storeSize         = getWriteSize(inst);
        instAnalysis->mayLoad           = (instAnalysis->loadSize != 0) || isUndefinedReadSize(inst);
        instAnalysis->mayStore          = (instAnalysis->storeSize != 0) || isUndefinedWriteSize(inst);
        instAnalysis->mnemonic          = MCII.getName(inst.getOpcode()).data();
        // old predicate, used by the validator
        instAnalysis->mayLoad_LLVM      = desc.mayLoad();
        instAnalysis->mayStore_LLVM     = desc.mayStore();
    }

    if (missingAnalysis & ANALYSIS_OPERANDS) {
        // analyse operands (immediates / registers)
        analyseOperands(instAnalysis, inst, desc, MRI);
    }

    if (missingAnalysis & ANALYSIS_SYMBOL) {
        // find nearest symbol (if any)
#ifndef QBDI_PLATFORM_WINDOWS
        Dl_info info;
        const char* ptr;

        int ret = dladdr((void*) instAnalysis->address, &info);
        if (ret != 0) {
            if (info.dli_sname) {
                instAnalysis->symbol = info.dli_sname;
                instAnalysis->symbolOffset = instAnalysis->address - (rword) info.dli_saddr;
            }
            if (info.dli_fname) {
                // dirty basename, but thead safe
                if((ptr = strrchr(info.dli_fname, '/')) != nullptr) {
                    instAnalysis->module = ptr + 1;
                }
            }
        }
#endif
    }
    return instAnalysis;
}

}
