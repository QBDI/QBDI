/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
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
#include <stdlib.h>

#include "llvm/MC/MCInst.h"

#include "QBDI/Platform.h"
#include "Engine/LLVMCPU.h"
#include "Patch/InstMetadata.h"
#include "Patch/InstTransform.h"
#include "Patch/Patch.h"
#include "Patch/PatchGenerator.h"
#include "Patch/Register.h"
#include "Patch/RelocatableInst.h"
#include "Patch/TempManager.h"
#include "Patch/Types.h"
#include "Utility/LogSys.h"

namespace QBDI {

template <typename T>
RelocatableInst::UniquePtrVec
PureEval<T>::generate(const Patch &patch, TempManager &temp_manager) const {
  return this->genReloc(*patch.llvmcpu);
}

template RelocatableInst::UniquePtrVec
PureEval<AutoClone<PatchGenerator, PatchGenFlags>>::generate(
    const Patch &patch, TempManager &temp_manager) const;
template RelocatableInst::UniquePtrVec
PureEval<AutoClone<PatchGenerator, LoadReg>>::generate(
    const Patch &patch, TempManager &temp_manager) const;
template RelocatableInst::UniquePtrVec
PureEval<AutoClone<PatchGenerator, SaveReg>>::generate(
    const Patch &patch, TempManager &temp_manager) const;
template RelocatableInst::UniquePtrVec
PureEval<AutoClone<PatchGenerator, JmpEpilogue>>::generate(
    const Patch &patch, TempManager &temp_manager) const;

// ModifyInstruction
// =================

ModifyInstruction::ModifyInstruction(InstTransform::UniquePtrVec &&transforms)
    : transforms(std::forward<InstTransform::UniquePtrVec>(transforms)){};

std::unique_ptr<PatchGenerator> ModifyInstruction::clone() const {
  return ModifyInstruction::unique(cloneVec(transforms));
};

RelocatableInst::UniquePtrVec
ModifyInstruction::generate(const Patch &patch,
                            TempManager &temp_manager) const {

  llvm::MCInst a(patch.metadata.inst);
  for (const auto &t : transforms) {
    t->transform(a, patch.metadata.address, patch.metadata.instSize,
                 temp_manager);
  }
  return conv_unique<RelocatableInst>(NoReloc::unique(std::move(a)));
}

// PatchGenFlags
// =============

RelocatableInst::UniquePtrVec
PatchGenFlags::genReloc(const LLVMCPU &llvmcpu) const {
  return {};
}

// GetOperand
// ==========

RelocatableInst::UniquePtrVec
GetOperand::generate(const Patch &patch, TempManager &temp_manager) const {
  const llvm::MCInst &inst = patch.metadata.inst;
  Reg destReg = reg;
  if (type == TmpType) {
    destReg = temp_manager.getRegForTemp(temp);
  }
  QBDI_REQUIRE_ABORT_PATCH(op < inst.getNumOperands(), patch,
                           "Invalid operand {}", op);
  if (inst.getOperand(op).isReg()) {
    return conv_unique<RelocatableInst>(
        MovReg::unique(destReg, inst.getOperand(op).getReg()));
  } else if (inst.getOperand(op).isImm()) {
    return conv_unique<RelocatableInst>(
        LoadImm::unique(destReg, Constant(inst.getOperand(op).getImm())));
  } else {
    QBDI_ERROR("Invalid operand type for GetOperand()");
    return {};
  }
}

// WriteOperand
// ============

RelocatableInst::UniquePtrVec
WriteOperand::generate(const Patch &patch, TempManager &temp_manager) const {
  const llvm::MCInst &inst = patch.metadata.inst;

  QBDI_REQUIRE_ABORT_PATCH(op < inst.getNumOperands(), patch,
                           "Invalid operand {}", op);
  if (inst.getOperand(op).isReg()) {
    return conv_unique<RelocatableInst>(
        StoreDataBlock::unique(inst.getOperand(op).getReg(), offset));
  } else {
    QBDI_ERROR("Invalid operand type for WriteOperand()");
    return {};
  }
}

// GetConstant
// ===========

RelocatableInst::UniquePtrVec
GetConstant::generate(const Patch &patch, TempManager &temp_manager) const {

  return conv_unique<RelocatableInst>(
      LoadImm::unique(temp_manager.getRegForTemp(temp), cst));
}

// GetConstantMap
// ==============

RelocatableInst::UniquePtrVec
GetConstantMap::generate(const Patch &patch, TempManager &temp_manager) const {
  const llvm::MCInst &inst = patch.metadata.inst;
  const auto it = opcodeMap.find(inst.getOpcode());
  QBDI_REQUIRE_ABORT_PATCH(it != opcodeMap.end(), temp_manager.getPatch(),
                           "Opcode not found");

  return conv_unique<RelocatableInst>(
      LoadImm::unique(temp_manager.getRegForTemp(temp), it->second));
}

// ReadTemp
// ========

RelocatableInst::UniquePtrVec
ReadTemp::generate(const Patch &patch, TempManager &temp_manager) const {

  if (type == OffsetType) {
    return conv_unique<RelocatableInst>(
        LoadDataBlock::unique(temp_manager.getRegForTemp(temp), offset));
  } else if (type == ShadowType) {
    return conv_unique<RelocatableInst>(
        LoadShadow::unique(temp_manager.getRegForTemp(temp), shadow));
  }
  _QBDI_UNREACHABLE();
}

// WriteTemp
// =========

RelocatableInst::UniquePtrVec
WriteTemp::generate(const Patch &patch, TempManager &temp_manager) const {

  if (type == OffsetType) {
    return conv_unique<RelocatableInst>(
        StoreDataBlock::unique(temp_manager.getRegForTemp(temp), offset));
  } else if (type == ShadowType) {
    return conv_unique<RelocatableInst>(
        StoreShadow::unique(temp_manager.getRegForTemp(temp), shadow, true));
  } else if (type == OperandType) {
    const llvm::MCInst &inst = patch.metadata.inst;
    QBDI_REQUIRE_ABORT_PATCH(operand < inst.getNumOperands(), patch,
                             "Invalid operand {}", operand);
    QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operand).isReg(), patch,
                             "Unexpected operand type");
    int regNo = getGPRPosition(inst.getOperand(operand).getReg());
    QBDI_REQUIRE_ABORT_PATCH(regNo != -1, patch, "Unexpected GPRregister {}",
                             inst.getOperand(operand).getReg());
    return conv_unique<RelocatableInst>(
        MovReg::unique(Reg(regNo), temp_manager.getRegForTemp(temp)));
  }
  _QBDI_UNREACHABLE();
}

// LoadReg
// =======

RelocatableInst::UniquePtrVec LoadReg::genReloc(const LLVMCPU &llvmcpu) const {

  return conv_unique<RelocatableInst>(LoadDataBlock::unique(reg, offset));
}

// SaveTemp
// ========

RelocatableInst::UniquePtrVec
SaveTemp::generate(const Patch &patch, TempManager &temp_manager) const {

  Reg reg = temp_manager.getRegForTemp(temp);
  return conv_unique<RelocatableInst>(
      StoreDataBlock::unique(reg, reg.offset()));
}

// SaveReg
// =======

RelocatableInst::UniquePtrVec SaveReg::genReloc(const LLVMCPU &llvmcpu) const {

  return conv_unique<RelocatableInst>(StoreDataBlock::unique(reg, offset));
}

// CopyReg
// =======

RelocatableInst::UniquePtrVec
CopyReg::generate(const Patch &patch, TempManager &temp_manager) const {

  if (type == Reg2Temp) {
    return conv_unique<RelocatableInst>(
        MovReg::unique(temp_manager.getRegForTemp(destTemp), src));
  } else if (type == Reg2Reg) {
    return conv_unique<RelocatableInst>(MovReg::unique(destReg, src));
  }
  _QBDI_UNREACHABLE();
}

// CopyTemp
// ========

RelocatableInst::UniquePtrVec
CopyTemp::generate(const Patch &patch, TempManager &temp_manager) const {

  if (type == Temp2Temp) {
    return conv_unique<RelocatableInst>(MovReg::unique(
        temp_manager.getRegForTemp(destTemp), temp_manager.getRegForTemp(src)));
  } else if (type == Temp2Reg) {
    return conv_unique<RelocatableInst>(
        MovReg::unique(destReg, temp_manager.getRegForTemp(src)));
  }
  _QBDI_UNREACHABLE();
}

// GetInstId
// =========

RelocatableInst::UniquePtrVec
GetInstId::generate(const Patch &patch, TempManager &temp_manager) const {

  return conv_unique<RelocatableInst>(
      InstId::unique(temp_manager.getRegForTemp(temp)));
}

// TargetPrologue
// ==============

RelocatableInst::UniquePtrVec
TargetPrologue::generate(const Patch &patch, TempManager &temp_manager) const {
  return genReloc(patch);
}

} // namespace QBDI
