/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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
#include <cstdint>
#include <map>
#include <string.h>
#include <string>
#include <utility>

#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/MCRegisterInfo.h"

#include "Engine/LLVMCPU.h"
#include "Patch/InstInfo.h"
#include "Patch/InstMetadata.h"
#include "Patch/Register.h"
#include "Patch/Types.h"
#include "Utility/InstAnalysis_prive.h"
#include "Utility/LogSys.h"

#include "QBDI/Bitmask.h"
#include "QBDI/Config.h"
#include "QBDI/InstAnalysis.h"
#include "QBDI/State.h"

#ifndef QBDI_PLATFORM_WINDOWS
#if defined(QBDI_PLATFORM_LINUX) && !defined(__USE_GNU)
#define __USE_GNU
#endif
#include <dlfcn.h>
#endif

namespace QBDI {
namespace InstructionAnalysis {

static bool isFlagRegister(RegLLVM regNo) {
  if (regNo == /* llvm::X86|ARM::NoRegister */ 0) {
    return false;
  }
  if (GPR_ID[REG_FLAG] == regNo) {
    return true;
  }
  for (size_t j = 0; j < size_FLAG_ID; j++) {
    if (regNo == FLAG_ID[j]) {
      return true;
    }
  }
  return false;
}

void analyseRegister(OperandAnalysis &opa, RegLLVM regNo,
                     const llvm::MCRegisterInfo &MRI) {
  opa.regName = MRI.getName(regNo.getValue());
  if (opa.regName != nullptr && opa.regName[0] == '\x00') {
    opa.regName = nullptr;
  }
  opa.value = regNo.getValue();
  opa.size = 0;
  opa.flag = OPERANDFLAG_NONE;
  opa.regOff = 0;
  opa.regCtxIdx = -1;
  opa.type = OPERAND_INVALID;
  opa.regAccess = REGISTER_UNUSED;
  if (regNo == /* llvm::X86|ARM::NoRegister */ 0) {
    return;
  }
  // try to match register in our GPR context
  size_t gprIndex = getGPRPosition(regNo);
  if (gprIndex != ((size_t)-1)) {
    if (MRI.isSubRegisterEq(GPR_ID[gprIndex].getValue(), regNo.getValue())) {
      if (GPR_ID[gprIndex] != regNo) {
        opa.regOff = getRegisterBaseOffset(regNo);
      }
      opa.regCtxIdx = gprIndex;
      opa.size = getRegisterSize(regNo);
      opa.type = OPERAND_GPR;
      if (!opa.size) {
        QBDI_WARN("register {} ({}) with size null", regNo.getValue(),
                  opa.regName);
      }
      return;
    }
    if (getRegisterPacked(regNo) <= 1) {
      QBDI_WARN("register {} ({}) has index {} but isn't a subregister",
                regNo.getValue(), opa.regName, gprIndex);
    }
  }
  // try to match register in our FPR context
  auto it = FPR_ID.find(regNo);
  if (it != FPR_ID.end()) {
    opa.regCtxIdx = it->second;
    opa.regOff = 0;
    opa.size = getRegisterSize(regNo);
    opa.type = OPERAND_FPR;
    if (!opa.size) {
      QBDI_WARN("register {} ({}) with size null", regNo.getValue(),
                opa.regName);
    }
    return;
  }
  for (uint16_t j = 0; j < size_SEG_ID; j++) {
    if (regNo == SEG_ID[j]) {
      opa.regCtxIdx = -1;
      opa.regOff = 0;
      opa.size = getRegisterSize(regNo);
      opa.type = OPERAND_SEG;
      if (!opa.size) {
        QBDI_WARN("register {} ({}) with size null", regNo.getValue(),
                  opa.regName);
      }
      return;
    }
  }
  if (getRegisterPacked(regNo) > 1) {
    // multiple register, the operand analyse is handled by the target
    opa.regCtxIdx = -1;
    opa.regOff = 0;
    opa.size = 0;
    opa.type = OPERAND_INVALID;
  } else {
    // unsupported register
    QBDI_WARN("Unknown register {} : {}", regNo.getValue(), opa.regName);
    opa.regCtxIdx = -1;
    opa.regOff = 0;
    opa.size = 0;
    opa.type = OPERAND_SEG;
  }
  return;
}

void breakPackedRegister(InstAnalysis *instAnalysis, OperandAnalysis &opa,
                         RegLLVM regNo, const llvm::MCRegisterInfo &MRI) {

  size_t gprIndex = getGPRPosition(regNo);

  // packed GPR
  if (gprIndex != (size_t)-1) {
    opa.regCtxIdx = gprIndex;
    opa.size = getRegisterSize(regNo);
    opa.type = OPERAND_GPR;
    opa.regName = MRI.getName(getPackedRegister(regNo, 0).getValue());
    if (opa.regName != nullptr && opa.regName[0] == '\x00') {
      opa.regName = nullptr;
    }

    for (unsigned p = 1; p < getRegisterPacked(regNo); p++) {
      OperandAnalysis &opa2 = instAnalysis->operands[instAnalysis->numOperands];
      memcpy(&opa2, &opa, sizeof(OperandAnalysis));
      opa2.regCtxIdx = getGPRPosition(getPackedRegister(regNo, p));
      opa2.regName = MRI.getName(getPackedRegister(regNo, p).getValue());
      if (opa2.regName != nullptr && opa2.regName[0] == '\x00') {
        opa2.regName = nullptr;
      }
      QBDI_REQUIRE(opa2.regCtxIdx != -1);
      instAnalysis->numOperands++;
    }
    return;
  }
  // packed FPR
  auto it = FPR_ID.find(getPackedRegister(regNo, 0));
  if (it != FPR_ID.end()) {
    opa.regCtxIdx = it->second;
    opa.size = getRegisterSize(regNo);
    opa.type = OPERAND_FPR;
    opa.regName = MRI.getName(getPackedRegister(regNo, 0).getValue());
    if (opa.regName != nullptr && opa.regName[0] == '\x00') {
      opa.regName = nullptr;
    }
    for (unsigned p = 1; p < getRegisterPacked(regNo); p++) {
      OperandAnalysis &opa2 = instAnalysis->operands[instAnalysis->numOperands];
      memcpy(&opa2, &opa, sizeof(OperandAnalysis));
      auto it2 = FPR_ID.find(getPackedRegister(regNo, p));
      if (it2 != FPR_ID.end()) {
        opa2.regCtxIdx = it2->second;
      } else {
        opa2.regCtxIdx = -1;
      }
      opa2.regName = MRI.getName(getPackedRegister(regNo, p).getValue());
      if (opa2.regName != nullptr && opa2.regName[0] == '\x00') {
        opa2.regName = nullptr;
      }
      instAnalysis->numOperands++;
    }
    return;
  }
  // unsupported Packed Register
  QBDI_WARN("Unknown register {} : {}", regNo.getValue(), opa.regName);
  opa.type = OPERAND_SEG;
  return;
}

void tryMergeCurrentRegister(InstAnalysis *instAnalysis) {
  OperandAnalysis &opa = instAnalysis->operands[instAnalysis->numOperands - 1];
  if ((opa.type != QBDI::OPERAND_GPR && opa.type != QBDI::OPERAND_FPR) ||
      opa.regCtxIdx < 0) {
    return;
  }
  if ((opa.flag & QBDI::OPERANDFLAG_IMPLICIT) == 0) {
    return;
  }
  for (uint16_t j = 0; j < instAnalysis->numOperands - 1; j++) {
    OperandAnalysis &pop = instAnalysis->operands[j];
    if (pop.type != opa.type || pop.flag != opa.flag) {
      continue;
    }
    if (pop.regCtxIdx == opa.regCtxIdx && pop.size == opa.size &&
        pop.regOff == opa.regOff) {
      // merge current one into previous one
      pop.regAccess |= opa.regAccess;
      memset(&opa, 0, sizeof(OperandAnalysis));
      instAnalysis->numOperands--;
      break;
    }
  }
}

void analyseImplicitRegisters(InstAnalysis *instAnalysis,
                              llvm::ArrayRef<llvm::MCPhysReg> implicitRegs,
                              RegisterAccessType type,
                              const llvm::MCRegisterInfo &MRI) {
  // Iteration style copied from LLVM code
  for (const llvm::MCPhysReg &regNo : implicitRegs) {
    if (isFlagRegister(regNo)) {
      instAnalysis->flagsAccess |= type;
      continue;
    }
    OperandAnalysis topa;
    analyseRegister(topa, regNo, MRI);
    // we found a GPR (as size is only known for GPR)
    // TODO: add support for more registers
    if (topa.size != 0 && topa.type != OPERAND_INVALID) {
      // fill a new operand analysis
      OperandAnalysis &opa = instAnalysis->operands[instAnalysis->numOperands];
      opa = topa;
      opa.regAccess = type;
      opa.flag |= OPERANDFLAG_IMPLICIT;
      instAnalysis->numOperands++;
      // try to merge with a previous one
      tryMergeCurrentRegister(instAnalysis);
    }
  }
}

void analyseOperands(InstAnalysis *instAnalysis, const llvm::MCInst &inst,
                     const LLVMCPU &llvmcpu) {
  if (!instAnalysis) {
    // no instruction analysis
    return;
  }
  const llvm::MCInstrDesc &desc = llvmcpu.getMCII().get(inst.getOpcode());
  const llvm::MCRegisterInfo &MRI = llvmcpu.getMRI();

  instAnalysis->numOperands = 0; // updated later because we could skip some
  instAnalysis->operands = NULL;
  // number of first def operand that are tied to a later used operand
  unsigned operandBias = getBias(desc);
  // Analysis of instruction operands
  unsigned numOperands = inst.getNumOperands();
  unsigned numOperandsMax =
      numOperands + desc.NumImplicitDefs + desc.NumImplicitUses - operandBias;
  // (R|E)SP are missing for RET and CALL in x86
  numOperandsMax += getAdditionnalOperandNumber(inst, desc);
  // packedRegister
  for (unsigned i = 0; i < inst.getNumOperands(); i++) {
    const llvm::MCOperand &op = inst.getOperand(i);
    if (op.isReg() and getRegisterPacked(op.getReg()) > 1) {
      numOperandsMax += getRegisterPacked(op.getReg()) - 1;
    }
  }
  if (numOperandsMax == 0) {
    // no operand to analyse
    return;
  }
  instAnalysis->operands = new OperandAnalysis[numOperandsMax]();
  // limit operandDescription
  unsigned maxOperandDesc = desc.getNumOperands();
  if (desc.isVariadic()) {
    QBDI_REQUIRE(maxOperandDesc >= 1 or numOperands == 0);
    maxOperandDesc--;
  } else {
    QBDI_REQUIRE(numOperands == maxOperandDesc);
  }
  // find written registers
  std::bitset<32> regWrites;
  for (unsigned i = 0, e = desc.getNumDefs(); i != e; ++i) {
    const llvm::MCOperand &op = inst.getOperand(i);
    if (op.isReg()) {
      regWrites.set(i, true);
    }
  }
  if (desc.isVariadic() and variadicOpsIsWrite(inst)) {
    for (unsigned i = maxOperandDesc, e = inst.getNumOperands(); i != e; ++i) {
      const llvm::MCOperand &op = inst.getOperand(i);
      if (op.isReg()) {
        regWrites.set(i, true);
      }
    }
  }
  // for each instruction operands
  for (unsigned i = operandBias; i < numOperands; i++) {
    const llvm::MCOperand &op = inst.getOperand(i);
    // if the instruction is variadic, the opdesc for the variadic operand is
    // the last opdesc
    const llvm::MCOperandInfo &opdesc =
        desc.operands()[std::min(i, maxOperandDesc)];
    // fill a new operand analysis
    QBDI_REQUIRE(instAnalysis->numOperands < numOperandsMax);
    OperandAnalysis &opa = instAnalysis->operands[instAnalysis->numOperands];
    // reinitialise the opa if a previous iteration has write some value
    memset(&opa, 0, sizeof(OperandAnalysis));
    opa.regCtxIdx = -1;
    if (!op.isValid()) {
      continue;
    }
    if (op.isReg()) {
      unsigned int regNo = op.getReg();
      int tied_to = desc.getOperandConstraint(i, llvm::MCOI::TIED_TO);

      // if the operand is tied_to the previous operand, add the access to the
      // operand.
      if (tied_to != -1 and operandBias == 0) {
        QBDI_REQUIRE(tied_to == i - 1);
        QBDI_REQUIRE(inst.getOperand(i - 1).isReg());
        if constexpr (is_arm)
          QBDI_REQUIRE(not isFlagOperand(inst.getOpcode(), i, opdesc));
        QBDI_REQUIRE(not isFlagRegister(regNo));
        instAnalysis->operands[instAnalysis->numOperands - 1].regAccess |=
            regWrites.test(i) ? REGISTER_WRITE : REGISTER_READ;
        continue;
      }
      // don't reject regNo == 0 tp keep the same position of operand
      // ex : "lea rax, [rbx+10]" and "lea rax, [rbx+4*rcx+10]" will have the
      // same number of operand
      if constexpr (is_arm) {
        if (isFlagOperand(inst.getOpcode(), i, opdesc)) {
          continue;
        }
      } else {
        if (isFlagRegister(regNo)) {
          instAnalysis->flagsAccess |=
              regWrites.test(i) ? REGISTER_WRITE : REGISTER_READ;
          continue;
        }
      }
      // fill the operand analysis
      analyseRegister(opa, regNo, MRI);
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
          QBDI_WARN("Not supported operandType {} for register operand",
                    opdesc.OperandType);
          continue;
      }
      if (regNo != 0) {
        opa.regAccess = regWrites.test(i) ? REGISTER_WRITE : REGISTER_READ;

        // verify if the register is allocate in the same place as anothe
        // register
        if (tied_to != -1 and operandBias != 0) {
          opa.regAccess |=
              regWrites.test(tied_to) ? REGISTER_WRITE : REGISTER_READ;
        }
      }
      instAnalysis->numOperands++;
      if (opa.type == OPERAND_INVALID and getRegisterPacked(regNo) > 1) {
        breakPackedRegister(instAnalysis, opa, regNo, MRI);
        QBDI_REQUIRE(instAnalysis->numOperands <= numOperandsMax);
      }
    } else if (op.isImm()) {
      if (isFlagOperand(inst.getOpcode(), i, opdesc)) {
        continue;
      }
      // fill the operand analysis
      switch (opdesc.OperandType) {
        case llvm::MCOI::OPERAND_IMMEDIATE:
          opa.size = getImmediateSize(inst, llvmcpu);
          break;
        case llvm::MCOI::OPERAND_MEMORY:
          opa.flag |= OPERANDFLAG_ADDR;
          opa.size = sizeof(rword);
          break;
        case llvm::MCOI::OPERAND_PCREL:
          opa.size = getImmediateSize(inst, llvmcpu);
          opa.flag |= OPERANDFLAG_PCREL;
          break;
        case llvm::MCOI::OPERAND_UNKNOWN:
          opa.flag |= OPERANDFLAG_UNDEFINED_EFFECT;
          opa.size = sizeof(rword);
          break;
        default:
          QBDI_WARN("Not supported operandType {} for immediate operand",
                    opdesc.OperandType);
          continue;
      }
      if (opdesc.isPredicate()) {
        opa.type = OPERAND_PRED;
      } else {
        opa.type = OPERAND_IMM;
      }
      opa.value = getFixedOperandValue(inst, llvmcpu, i, op.getImm());
      instAnalysis->numOperands++;
    }
  }

  // analyse implicit registers (R/W)
  analyseImplicitRegisters(instAnalysis, desc.implicit_uses(), REGISTER_READ,
                           MRI);
  analyseImplicitRegisters(instAnalysis, desc.implicit_defs(), REGISTER_WRITE,
                           MRI);

  // (R|E)SP are missing for RET and CALL in x86
  getAdditionnalOperand(instAnalysis, inst, desc, MRI);

  // reduce operands buffer if possible
  if (instAnalysis->numOperands < numOperandsMax) {
    OperandAnalysis *oldbuff = instAnalysis->operands;
    if (instAnalysis->numOperands == 0) {
      instAnalysis->operands = nullptr;
    } else {
      instAnalysis->operands = new OperandAnalysis[instAnalysis->numOperands]();
      memcpy(instAnalysis->operands, oldbuff,
             sizeof(OperandAnalysis) * instAnalysis->numOperands);
    }
    delete[] oldbuff;
  }
}

} // namespace InstructionAnalysis

void InstAnalysisDestructor::operator()(InstAnalysis *ptr) const {
  if (ptr == nullptr) {
    return;
  }
  if (ptr->operands != nullptr) {
    delete[] ptr->operands;
  }
  if (ptr->disassembly != nullptr) {
    delete[] ptr->disassembly;
  }
  delete ptr;
}

const InstAnalysis *analyzeInstMetadata(const InstMetadata &instMetadata,
                                        AnalysisType type,
                                        const LLVMCPU &llvmcpu) {

  InstAnalysis *instAnalysis = instMetadata.analysis.get();
  if (instAnalysis == nullptr) {
    instAnalysis = new InstAnalysis;
    // set all values to NULL/0/false
    memset(instAnalysis, 0, sizeof(InstAnalysis));
    instMetadata.analysis.reset(instAnalysis);
  }

  uint32_t oldType = instAnalysis->analysisType;
  uint32_t newType = oldType | type;
  uint32_t missingType = oldType ^ newType;

  if (missingType == 0) {
    return instAnalysis;
  }

  instAnalysis->analysisType = newType;

  const llvm::MCInstrInfo &MCII = llvmcpu.getMCII();
  const llvm::MCInst &inst = instMetadata.inst;
  const llvm::MCInstrDesc &desc = MCII.get(inst.getOpcode());

  if (missingType & ANALYSIS_DISASSEMBLY) {
    std::string buffer = llvmcpu.showInst(inst, instMetadata.address);
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
    // support for prefix that isn't include in instruction (like LOCK_PREFIX)
    if (instMetadata.prefix.size() > 0) {
      std::string prefix;
      for (const auto &prefixInst : instMetadata.prefix) {
        prefix += llvmcpu.showInst(prefixInst, instMetadata.address) + "\t";
      }
      buffer = prefix + buffer;
    }
#endif

    int len = buffer.size() + 1;
    instAnalysis->disassembly = new char[len];
    strncpy(instAnalysis->disassembly, buffer.c_str(), len);
    buffer.clear();
  }

  if (missingType & ANALYSIS_INSTRUCTION) {
    instAnalysis->address = instMetadata.address;
    instAnalysis->instSize = instMetadata.instSize;
    instAnalysis->cpuMode = instMetadata.cpuMode;
    instAnalysis->affectControlFlow = instMetadata.modifyPC;
    instAnalysis->isBranch = desc.isBranch();
    instAnalysis->isCall = desc.isCall();
    instAnalysis->isReturn = desc.isReturn();
    instAnalysis->isCompare = desc.isCompare();
    instAnalysis->isPredicable = desc.isPredicable();
    instAnalysis->isMoveImm = desc.isMoveImmediate();
    instAnalysis->loadSize = getReadSize(inst, llvmcpu);
    instAnalysis->storeSize = getWriteSize(inst, llvmcpu);
    instAnalysis->mayLoad =
        instAnalysis->loadSize != 0 || unsupportedRead(inst);
    instAnalysis->mayStore =
        instAnalysis->storeSize != 0 || unsupportedWrite(inst);
    instAnalysis->opcode_LLVM = inst.getOpcode();
    instAnalysis->mayLoad_LLVM = desc.mayLoad();
    instAnalysis->mayStore_LLVM = desc.mayStore();
    instAnalysis->mnemonic = llvmcpu.getInstOpcodeName(inst);

    InstructionAnalysis::analyseCondition(instAnalysis, inst, desc, llvmcpu);
  }

  if (missingType & ANALYSIS_OPERANDS) {
    if constexpr (is_arm) {
      // for arm, analyseCondition analyse the usage of the flags
      if (not(missingType & ANALYSIS_INSTRUCTION)) {
        InstructionAnalysis::analyseCondition(instAnalysis, inst, desc,
                                              llvmcpu);
      }
    }
    // analyse operands (immediates / registers)
    InstructionAnalysis::analyseOperands(instAnalysis, inst, llvmcpu);
  }

  if (missingType & ANALYSIS_SYMBOL) {
    // find nearest symbol (if any)
#ifndef QBDI_PLATFORM_WINDOWS
    Dl_info info;
    const char *ptr;

    int ret = dladdr((void *)instAnalysis->address, &info);
    if (ret != 0) {
      if (info.dli_sname) {
        instAnalysis->symbolName = info.dli_sname;
        instAnalysis->symbolOffset =
            instAnalysis->address - (rword)info.dli_saddr;
      }
      if (info.dli_fname) {
        // dirty basename, but thead safe
        if ((ptr = strrchr(info.dli_fname, '/')) != nullptr) {
          instAnalysis->moduleName = ptr + 1;
        }
      }
    }
#endif
  }

  return instAnalysis;
}

} // namespace QBDI
