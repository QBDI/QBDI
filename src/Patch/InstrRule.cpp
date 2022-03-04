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
#include <stdint.h>
#include <stdlib.h>
#include <utility>

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
  TempManager tempManager(patch, true);

  // Generate the instrumentation code from the original instruction context
  for (const PatchGenerator::UniquePtr &g : patchGen) {
    append(instru, g->generate(&patch, &tempManager, nullptr));
  }

  // In case we break to the host, we need to ensure the value of PC in the
  // context is correct. This value needs to be set when instrumenting before
  // the instruction or when instrumenting after an instruction which does not
  // set PC.
  if (breakToHost) {
    if (position == InstPosition::PREINST || patch.metadata.modifyPC == false) {
      switch (position) {
        // In PREINST PC is set to the current address
        case InstPosition::PREINST:
          append(instru, GetConstant(Temp(0), Constant(patch.metadata.address))
                             .generate(&patch, &tempManager, nullptr));
          break;
        // In POSTINST PC is set to the next instruction address
        case InstPosition::POSTINST:
          append(instru, GetConstant(Temp(0), Constant(patch.metadata.address +
                                                       patch.metadata.instSize))
                             .generate(&patch, &tempManager, nullptr));
          break;
      }
      append(instru,
             SaveReg(tempManager.getRegForTemp(0), Offset(Reg(REG_PC))));
    }
  }

  // The breakToHost code requires one temporary register. If none were
  // allocated by the instrumentation we thus need to add one.
  if (breakToHost && tempManager.getUsedRegisterNumber() == 0) {
    tempManager.getRegForTemp(Temp(0));
  }

  // Get all used temporary register
  Reg::Vec usedRegisters = tempManager.getUsedRegisters();

  // In the break to host case the first used register is not restored and
  // instead given to the break to host code as a scratch. It will later be
  // restored by the break to host code.
  if (breakToHost) {
    for (uint32_t i = 1; i < usedRegisters.size(); i++) {
      if (tempManager.shouldRestore(usedRegisters[i])) {
        append(instru, LoadReg(usedRegisters[i], Offset(usedRegisters[i])));
        prepend(instru, SaveReg(usedRegisters[i], Offset(usedRegisters[i])));
      }
    }
    bool restoreLast = tempManager.shouldRestore(usedRegisters[0]);
    if (restoreLast) {
      prepend(instru, SaveReg(usedRegisters[0], Offset(usedRegisters[0])));
    }
    append(instru, getBreakToHost(usedRegisters[0], patch, restoreLast));
  }
  // Normal case where we append the temporary register restoration code to the
  // instrumentation
  else {
    for (uint32_t i = 0; i < usedRegisters.size(); i++) {
      if (tempManager.shouldRestore(usedRegisters[i])) {
        append(instru, LoadReg(usedRegisters[i], Offset(usedRegisters[i])));
        prepend(instru, SaveReg(usedRegisters[i], Offset(usedRegisters[i])));
      }
    }
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
  return condition->test(patch.metadata.inst, patch.metadata.address,
                         patch.metadata.instSize, llvmcpu);
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
  return condition->test(patch.metadata.inst, patch.metadata.address,
                         patch.metadata.instSize, llvmcpu);
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

InstrRuleUser::InstrRuleUser(InstrRuleCallback cbk, AnalysisType analysisType,
                             void *cbk_data, VMInstanceRef vm,
                             RangeSet<rword> range, int priority)
    : AutoClone<InstrRule, InstrRuleUser>(priority), cbk(cbk),
      analysisType(analysisType), cbk_data(cbk_data), vm(vm),
      range(std::move(range)) {}

InstrRuleUser::~InstrRuleUser() = default;

bool InstrRuleUser::tryInstrument(Patch &patch, const LLVMCPU &llvmcpu) const {
  if (!range.contains(
          Range<rword>(patch.metadata.address,
                       patch.metadata.address + patch.metadata.instSize))) {
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
    instrument(patch, getCallbackGenerator(cbkToAdd.cbk, cbkToAdd.data), true,
               cbkToAdd.position, cbkToAdd.priority,
               (cbkToAdd.position == PREINST) ? RelocTagPreInstStdCBK
                                              : RelocTagPostInstStdCBK);
  }

  return true;
}

} // namespace QBDI
