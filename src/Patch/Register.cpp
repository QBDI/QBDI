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
#include <sstream>
#include <utility>

#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"

#include "Engine/LLVMCPU.h"
#include "Patch/Register.h"

#include "Utility/LogSys.h"

namespace QBDI {

void addRegisterInMap(std::map<unsigned, RegisterUsage> &m, unsigned reg,
                      RegisterUsage usage) {
  auto e = m.find(reg);
  if (e != m.end()) {
    m[reg] = usage | e->second;
  } else {
    m[reg] = usage;
  }
}

std::map<unsigned, RegisterUsage> getUsedGPR(const llvm::MCInst &inst,
                                             const LLVMCPU &llvmcpu) {
  std::map<unsigned, RegisterUsage> res{};

  const llvm::MCInstrDesc &desc = llvmcpu.getMCII().get(inst.getOpcode());
  unsigned e = desc.isVariadic() ? inst.getNumOperands() : desc.getNumDefs();

  for (unsigned int i = 0; i < inst.getNumOperands(); i++) {
    const llvm::MCOperand &op = inst.getOperand(i);
    if (op.isReg()) {
      unsigned regNum = op.getReg();
      if (regNum == /* llvm::X86|AArch64::NoRegister */ 0) {
        continue;
      }
      if (i < e) {
        addRegisterInMap(res, getUpperRegister(regNum), RegisterSet);
      } else {
        addRegisterInMap(res, getUpperRegister(regNum), RegisterUsed);
      }
    }
  }

  for (const uint16_t *implicitRegs = desc.getImplicitUses();
       implicitRegs && *implicitRegs; ++implicitRegs) {
    addRegisterInMap(res, getUpperRegister(*implicitRegs), RegisterUsed);
  }

  for (const uint16_t *implicitRegs = desc.getImplicitDefs();
       implicitRegs && *implicitRegs; ++implicitRegs) {
    addRegisterInMap(res, getUpperRegister(*implicitRegs), RegisterSet);
  }

  fixLLVMUsedGPR(inst, llvmcpu, res);

  QBDI_DEBUG_BLOCK({
    std::ostringstream oss;
    bool first = true;
    for (const auto &e : res) {
      if (not first) {
        oss << ", ";
      } else {
        first = false;
      }
      oss << llvmcpu.getRegisterName(e.first);
      switch (e.second) {
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
    QBDI_DEBUG("Found Register Usage : {}", oss.str());
  });

  return res;
}

} // namespace QBDI
