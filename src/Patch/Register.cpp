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
#include <sstream>
#include <utility>

#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"

#include "devVariable.h"
#include "Engine/LLVMCPU.h"
#include "Patch/InstInfo.h"
#include "Patch/Register.h"
#include "Patch/Types.h"

#include "Utility/LogSys.h"

#if DEBUG_INST_OPERAND
#define DEBUG_REGISTER QBDI_DEBUG
#else
#define DEBUG_REGISTER(...)
#endif

namespace QBDI {

static void addRegisterInfo(std::array<RegisterUsage, NUM_GPR> &regArr,
                            std::map<RegLLVM, RegisterUsage> &regMap,
                            RegLLVM reg_, RegisterUsage usage) {

  for (int i = 0; i < getRegisterPacked(reg_); i++) {

    RegLLVM reg = getUpperRegister(reg_, i);
    int id = getGPRPosition(reg);
    if (id < 0 or ((unsigned)id) >= NUM_GPR) {
      auto e = regMap.find(reg);
      if (e != regMap.end()) {
        regMap[reg] = usage | e->second;
      } else {
        regMap[reg] = usage;
      }
    } else {
      regArr[id] |= usage;
    }
  }
}

void getUsedGPR(const llvm::MCInst &inst, const LLVMCPU &llvmcpu,
                std::array<RegisterUsage, NUM_GPR> &regUsage,
                std::map<RegLLVM, RegisterUsage> &regUsageExtra) {

  const llvm::MCInstrInfo &MCII = llvmcpu.getMCII();
  const llvm::MCInstrDesc &desc = MCII.get(inst.getOpcode());
  unsigned opIsUsedBegin = desc.getNumDefs();
  unsigned opIsUsedEnd = inst.getNumOperands();
  if (desc.isVariadic() and variadicOpsIsWrite(inst)) {
    if (desc.getNumOperands() < 1) {
      opIsUsedEnd = 0;
    } else {
      opIsUsedEnd = desc.getNumOperands() - 1;
    }
  }
  DEBUG_REGISTER(
      "Opcode : {}, Variadic : {}, opIsUsedBegin : {}, opIsUsedEnd : {}",
      MCII.getName(inst.getOpcode()), desc.isVariadic(), opIsUsedBegin,
      opIsUsedEnd);

  for (unsigned int i = 0; i < inst.getNumOperands(); i++) {
    const llvm::MCOperand &op = inst.getOperand(i);
    if (op.isReg()) {
      RegLLVM regNum = op.getReg();
      if (regNum == /* llvm::X86|AArch64::NoRegister */ 0) {
        DEBUG_REGISTER("{} Reg {}", i, regNum.getValue());
        continue;
      }
      if (i < opIsUsedBegin or opIsUsedEnd <= i) {
        DEBUG_REGISTER("{} Reg Set {}", i, llvmcpu.getRegisterName(regNum));
        addRegisterInfo(regUsage, regUsageExtra, regNum, RegisterSet);
      } else {
        DEBUG_REGISTER("{} Reg Used {}", i, llvmcpu.getRegisterName(regNum));
        addRegisterInfo(regUsage, regUsageExtra, regNum, RegisterUsed);
      }
    } else if (op.isImm()) {
      DEBUG_REGISTER("{} Imm 0x{:x}", i, op.getImm());
    } else {
      DEBUG_REGISTER("{} Unknown", i);
    }
  }

  for (const unsigned implicitRegs : desc.implicit_uses()) {
    if (implicitRegs) {
      DEBUG_REGISTER("Reg ImplicitUses {}",
                     llvmcpu.getRegisterName(implicitRegs));
      addRegisterInfo(regUsage, regUsageExtra, implicitRegs, RegisterUsed);
    }
  }

  for (const unsigned implicitRegs : desc.implicit_defs()) {
    if (implicitRegs) {
      DEBUG_REGISTER("Reg ImplicitDefs {}",
                     llvmcpu.getRegisterName(implicitRegs));
      addRegisterInfo(regUsage, regUsageExtra, implicitRegs, RegisterSet);
    }
  }

  fixLLVMUsedGPR(inst, llvmcpu, regUsage, regUsageExtra);

  QBDI_DEBUG_BLOCK({
    std::ostringstream oss;
    bool first = true;
    for (unsigned i = 0; i < NUM_GPR; i++) {
      if (regUsage[i] == 0) {
        continue;
      }
      if (not first) {
        oss << ", ";
      } else {
        first = false;
      }
      oss << llvmcpu.getRegisterName(GPR_ID[i]);
      switch (regUsage[i] & RegisterBoth) {
        case RegisterUsed:
          oss << " (r-)";
          break;
        case RegisterSet:
          oss << " (-w)";
          break;
        case RegisterBoth:
          oss << " (rw)";
          break;
      }
    }
    for (const auto &e : regUsageExtra) {
      if (not first) {
        oss << ", ";
      } else {
        first = false;
      }
      oss << llvmcpu.getRegisterName(e.first);
      switch (e.second & RegisterBoth) {
        case RegisterUsed:
          oss << " (r-)";
          break;
        case RegisterSet:
          oss << " (-w)";
          break;
        case RegisterBoth:
          oss << " (rw)";
          break;
      }
    }
    DEBUG_REGISTER("Found Register Usage : {}", oss.str());
  });
}

} // namespace QBDI
