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
#include <stdlib.h>

#include "llvm/MC/MCInst.h"

#include "QBDI/Platform.h"
#include "Patch/InstMetadata.h"
#include "Patch/InstTransform.h"
#include "Patch/Patch.h"
#include "Patch/PatchGenerator.h"
#include "Patch/Register.h"
#include "Patch/RelocatableInst.h"
#include "Patch/TempManager.h"
#include "Utility/LogSys.h"

namespace QBDI {

// ModifyInstruction
// =================

ModifyInstruction::ModifyInstruction(InstTransform::UniquePtrVec &&transforms)
    : transforms(std::forward<InstTransform::UniquePtrVec>(transforms)){};

std::unique_ptr<PatchGenerator> ModifyInstruction::clone() const {
  return ModifyInstruction::unique(cloneVec(transforms));
};

RelocatableInst::UniquePtrVec
ModifyInstruction::generate(const llvm::MCInst *inst, rword address,
                            rword instSize, TempManager *temp_manager,
                            Patch *toMerge) const {

  llvm::MCInst a(*inst);
  for (const auto &t : transforms) {
    t->transform(a, address, instSize, temp_manager);
  }

  RelocatableInst::UniquePtrVec out;
  if (toMerge != nullptr) {
    RelocatableInst::UniquePtrVec t;
    t.swap(toMerge->insts);
    toMerge->metadata.patchSize = 0;
    append(out, std::move(t));
  }
  out.push_back(NoReloc::unique(std::move(a)));
  return out;
}

// DoNotInstrument
// ===============

RelocatableInst::UniquePtrVec
DoNotInstrument::generate(const llvm::MCInst *inst, rword address,
                          rword instSize, TempManager *temp_manager,
                          Patch *toMerge) const {

  return {};
}

// GetOperand
// ==========

RelocatableInst::UniquePtrVec
GetOperand::generate(const llvm::MCInst *inst, rword address, rword instSize,
                     TempManager *temp_manager, Patch *toMerge) const {
  if (inst->getOperand(op).isReg()) {
    return conv_unique<RelocatableInst>(MovReg::unique(
        temp_manager->getRegForTemp(temp), inst->getOperand(op).getReg()));
  } else if (inst->getOperand(op).isImm()) {
    return conv_unique<RelocatableInst>(
        LoadImm::unique(temp_manager->getRegForTemp(temp),
                        Constant(inst->getOperand(op).getImm())));
  } else {
    QBDI_ERROR("Invalid operand type for GetOperand()");
    return {};
  }
}

// GetConstant
// ===========

RelocatableInst::UniquePtrVec
GetConstant::generate(const llvm::MCInst *inst, rword address, rword instSize,
                      TempManager *temp_manager, Patch *toMerge) const {

  return conv_unique<RelocatableInst>(
      LoadImm::unique(temp_manager->getRegForTemp(temp), cst));
}

// ReadTemp
// ========

RelocatableInst::UniquePtrVec ReadTemp::generate(const llvm::MCInst *inst,
                                                 rword address, rword instSize,
                                                 TempManager *temp_manager,
                                                 Patch *toMerge) const {

  if (type == OffsetType) {
    return conv_unique<RelocatableInst>(
        LoadDataBlock::unique(temp_manager->getRegForTemp(temp), offset));
  } else if (type == ShadowType) {
    return conv_unique<RelocatableInst>(
        LoadShadow::unique(temp_manager->getRegForTemp(temp), shadow));
  }
  _QBDI_UNREACHABLE();
}

// WriteTemp
// =========

RelocatableInst::UniquePtrVec WriteTemp::generate(const llvm::MCInst *inst,
                                                  rword address, rword instSize,
                                                  TempManager *temp_manager,
                                                  Patch *toMerge) const {

  if (type == OffsetType) {
    return conv_unique<RelocatableInst>(
        StoreDataBlock::unique(temp_manager->getRegForTemp(temp), offset));
  } else if (type == ShadowType) {
    return conv_unique<RelocatableInst>(
        StoreShadow::unique(temp_manager->getRegForTemp(temp), shadow, true));
  } else if (type == OperandType) {
    QBDI_REQUIRE_ACTION(inst->getOperand(operand).isReg(), abort());
    int regNo = getGPRPosition(inst->getOperand(operand).getReg());
    QBDI_REQUIRE_ACTION(regNo != -1, abort());
    return conv_unique<RelocatableInst>(
        MovReg::unique(Reg(regNo), temp_manager->getRegForTemp(temp)));
  }
  _QBDI_UNREACHABLE();
}

// LoadReg
// =======

RelocatableInst::UniquePtrVec LoadReg::generate(const llvm::MCInst *inst,
                                                rword address, rword instSize,
                                                TempManager *temp_manager,
                                                Patch *toMerge) const {

  return conv_unique<RelocatableInst>(LoadDataBlock::unique(reg, offset));
}

// SaveReg
// =======

RelocatableInst::UniquePtrVec SaveReg::generate(const llvm::MCInst *inst,
                                                rword address, rword instSize,
                                                TempManager *temp_manager,
                                                Patch *toMerge) const {

  return conv_unique<RelocatableInst>(StoreDataBlock::unique(reg, offset));
}

// CopyReg
// =======

RelocatableInst::UniquePtrVec CopyReg::generate(const llvm::MCInst *inst,
                                                rword address, rword instSize,
                                                TempManager *temp_manager,
                                                Patch *toMerge) const {

  return conv_unique<RelocatableInst>(
      MovReg::unique(reg, temp_manager->getRegForTemp(temp)));
}

// GetInstId
// =========

RelocatableInst::UniquePtrVec GetInstId::generate(const llvm::MCInst *inst,
                                                  rword address, rword instSize,
                                                  TempManager *temp_manager,
                                                  Patch *toMerge) const {

  return conv_unique<RelocatableInst>(
      InstId::unique(temp_manager->getRegForTemp(temp)));
}

} // namespace QBDI
