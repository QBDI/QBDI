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
#include <stdint.h>
#include <stdlib.h>
#include <utility>

#include "Engine/VM_internal.h"
#include "Patch/InstMetadata.h"
#include "Patch/InstrRule.h"
#include "Patch/InstrRules.h"
#include "Patch/Patch.h"
#include "Patch/PatchCondition.h"
#include "Patch/PatchGenerator.h"
#include "Patch/RelocatableInst.h"
#include "Patch/TempManager.h"
#include "Patch/Types.h"
#include "Utility/InstAnalysis_prive.h"
#include "Utility/LogSys.h"

namespace QBDI {

// InstrRule
// =========

void InstrRule::instrument(Patch &patch,
                           const PatchGenerator::UniquePtrVec &patchGen,
                           bool breakToHost, InstPosition position,
                           int priority, RelocatableInstTag tag) const {

  if (patchGen.size() == 0 && breakToHost == false) {
    QBDI_DEBUG("Empty patch Generator");
    return;
  }

  /* The instrument function needs to handle several different cases. An
   * instrumentation can be either prepended or appended to the patch and, in
   * each case, can trigger a break to host.
   */
  RelocatableInst::UniquePtrVec instru;
  TempManager tempManager(patch);

  // Generate the instrumentation code from the original instruction context
  for (const PatchGenerator::UniquePtr &g : patchGen) {
    append(instru, g->generate(patch, tempManager));
  }

  // In case we break to the host, we need to ensure the value of PC in the
  // context is correct. This value needs to be set when instrumenting before
  // the instruction or when instrumenting after an instruction which does not
  // set PC.
  if (breakToHost) {
    if (position == InstPosition::PREINST || patch.metadata.modifyPC == false) {
      rword address;
      switch (position) {
        // In PREINST PC is set to the current address
        case InstPosition::PREINST:
          address = patch.metadata.address;
          break;
        // In POSTINST PC is set to the next instruction address
        case InstPosition::POSTINST:
          address = patch.metadata.endAddress();
          break;
        default:
          QBDI_ABORT("Invalid position {} {}", position, patch);
      }
#if defined(QBDI_ARCH_ARM)
      if (patch.metadata.cpuMode == CPUMode::Thumb) {
        address |= 1;
      }
#endif
      append(
          instru,
          GetConstant(Temp(0), Constant(address)).generate(patch, tempManager));
      append(instru, SaveReg(tempManager.getRegForTemp(0), Offset(Reg(REG_PC)))
                         .genReloc(*patch.llvmcpu));
    }
  }

  // The breakToHost code requires one temporary register. If none were
  // allocated by the instrumentation we thus need to add one.
  if (breakToHost && tempManager.getUsedRegisterNumber() == 0) {
    tempManager.getRegForTemp(Temp(0));
  }

  RelocatableInst::UniquePtrVec saveReg, restoreReg;
  Reg::Vec unrestoredReg;

  // In the break to host case the first used register is not restored and
  // instead given to the break to host code as a scratch. It will later be
  // restored by the break to host code.
  if (breakToHost) {
    tempManager.generateSaveRestoreInstructions(1, saveReg, restoreReg,
                                                unrestoredReg);
    QBDI_REQUIRE(unrestoredReg.size() >= 1);

    prepend(instru, std::move(saveReg));
    append(instru, std::move(restoreReg));
    append(instru, getBreakToHost(unrestoredReg[0], patch,
                                  tempManager.shouldRestore(unrestoredReg[0])));
  }
  // Normal case where we append the temporary register restoration code to the
  // instrumentation
  else {
    tempManager.generateSaveRestoreInstructions(0, saveReg, restoreReg,
                                                unrestoredReg);
    prepend(instru, std::move(saveReg));
    append(instru, std::move(restoreReg));
  }

  // add Tag
  instru.insert(instru.begin(), RelocTag::unique(tag));

  QBDI_DEBUG(
      "Insert {} PatchGen with priority {}, position {} ({}) and tag 0x{:x}",
      instru.size(), priority,
      (position == PREINST) ? "PREINST"
                            : ((position == POSTINST) ? "POSTINST" : ""),
      position, tag);

  // Add the result to the patch
  // The result is added in a pending list that is sorted by priority
  // The pending list is flush in the Patch when all InstrRule has been apply
  patch.addInstsPatch(position, priority, std::move(instru));
}

// InstrRuleBasicCBK
// =================

InstrRuleBasicCBK::InstrRuleBasicCBK(PatchConditionUniquePtr &&condition,
                                     InstCallback cbk, void *data,
                                     InstPosition position, bool breakToHost,
                                     int priority, RelocatableInstTag tag)
    : AutoUnique<InstrRule, InstrRuleBasicCBK>(priority),
      condition(std::forward<PatchConditionUniquePtr>(condition)),
      patchGen(getCallbackGenerator(cbk, data)), position(position),
      breakToHost(breakToHost), tag(tag), cbk(cbk), data(data) {}

InstrRuleBasicCBK::~InstrRuleBasicCBK() = default;

bool InstrRuleBasicCBK::canBeApplied(const Patch &patch,
                                     const LLVMCPU &llvmcpu) const {
  return condition->test(patch, llvmcpu);
}

bool InstrRuleBasicCBK::changeDataPtr(void *new_data) {
  data = new_data;
  patchGen = getCallbackGenerator(cbk, data);
  return true;
}

std::unique_ptr<InstrRule> InstrRuleBasicCBK::clone() const {
  return InstrRuleBasicCBK::unique(condition->clone(), cbk, data, position,
                                   breakToHost, priority);
};

RangeSet<rword> InstrRuleBasicCBK::affectedRange() const {
  return condition->affectedRange();
}

// InstrRuleDynamic
// ================

InstrRuleDynamic::InstrRuleDynamic(PatchConditionUniquePtr &&condition,
                                   PatchGenMethod patchGenMethod,
                                   InstPosition position, bool breakToHost,
                                   int priority, RelocatableInstTag tag)
    : AutoUnique<InstrRule, InstrRuleDynamic>(priority),
      condition(std::forward<PatchConditionUniquePtr>(condition)),
      patchGenMethod(patchGenMethod), position(position),
      breakToHost(breakToHost), tag(tag) {}

InstrRuleDynamic::~InstrRuleDynamic() = default;

bool InstrRuleDynamic::canBeApplied(const Patch &patch,
                                    const LLVMCPU &llvmcpu) const {
  return condition->test(patch, llvmcpu);
}

std::unique_ptr<InstrRule> InstrRuleDynamic::clone() const {
  return InstrRuleDynamic::unique(condition->clone(), patchGenMethod, position,
                                  breakToHost, priority);
};

RangeSet<rword> InstrRuleDynamic::affectedRange() const {
  return condition->affectedRange();
}

// InstrRuleUser
// =============

InstrRuleUser::InstrRuleUser(InstrRuleCallback cbk, AnalysisType analysisType_,
                             void *cbk_data, VMInstanceRef vm,
                             RangeSet<rword> range, int priority)
    : AutoClone<InstrRule, InstrRuleUser>(priority), cbk(cbk),
      analysisType(analysisType_), cbk_data(cbk_data), vm(vm),
      range(std::move(range)) {

  if ((analysisType & AnalysisType::ANALYSIS_JIT) != 0) {
    QBDI_WARN("Can't use analysis type ANALYSIS_JIT with InstrRuleCallback");
    analysisType ^= AnalysisType::ANALYSIS_JIT;
  }
}

InstrRuleUser::~InstrRuleUser() = default;

bool InstrRuleUser::tryInstrument(Patch &patch, const LLVMCPU &llvmcpu) const {
  if (!range.contains(Range<rword>(
          patch.metadata.address,
          patch.metadata.address + patch.metadata.instSize, real_addr_t()))) {
    return false;
  }

  QBDI_DEBUG("Call user InstrCB at {} with analysisType 0x{:x}",
             reinterpret_cast<void *>(cbk), analysisType);

  const InstAnalysis *ana =
      analyzeInstMetadata(patch.metadata, analysisType, llvmcpu);

  std::vector<InstrRuleDataCBK> vec = cbk(vm, ana, cbk_data);

  QBDI_DEBUG("InstrCB return {} callback(s)", vec.size());

  if (vec.size() == 0) {
    return false;
  }

  for (const InstrRuleDataCBK &cbkToAdd : vec) {
    if (cbkToAdd.lambdaCbk == nullptr) {
      instrument(patch, getCallbackGenerator(cbkToAdd.cbk, cbkToAdd.data), true,
                 cbkToAdd.position, cbkToAdd.priority,
                 (cbkToAdd.position == PREINST) ? RelocTagPreInstStdCBK
                                                : RelocTagPostInstStdCBK);
    } else {
      patch.userInstCB.emplace_back(
          std::make_unique<InstCbLambda>(cbkToAdd.lambdaCbk));
      instrument(patch,
                 getCallbackGenerator(InstCBLambdaProxy,
                                      patch.userInstCB.back().get()),
                 true, cbkToAdd.position, cbkToAdd.priority,
                 (cbkToAdd.position == PREINST) ? RelocTagPreInstStdCBK
                                                : RelocTagPostInstStdCBK);
    }
  }

  return true;
}

} // namespace QBDI
