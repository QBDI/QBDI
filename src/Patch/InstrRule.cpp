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
#include "Patch/InstrRule.h"
#include "Patch/InstrRules.h"
#include "Patch/Patch.h"
#include "Patch/PatchGenerator.h"
#include "Patch/RelocatableInst.h"
#include "Utility/InstAnalysis_prive.h"
#include "Utility/LogSys.h"

namespace QBDI {

void InstrRule::instrument(Patch &patch, const llvm::MCInstrInfo *MCII,
                           const llvm::MCRegisterInfo *MRI,
                           const PatchGenerator::UniquePtrVec &patchGen,
                           bool breakToHost, InstPosition position) const {

  if (patchGen.size() == 0 && breakToHost == false) {
    return;
  }

  /* The instrument function needs to handle several different cases. An
   * instrumentation can be either prepended or appended to the patch and, in
   * each case, can trigger a break to host.
   */
  RelocatableInst::UniquePtrVec instru;
  TempManager tempManager(patch.metadata.inst, MCII, MRI, true);

  // Generate the instrumentation code from the original instruction context
  for (const PatchGenerator::UniquePtr &g : patchGen) {
    append(instru,
           g->generate(&patch.metadata.inst, patch.metadata.address,
                       patch.metadata.instSize, MCII, &tempManager, nullptr));
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
          append(instru,
                 GetConstant(Temp(0), Constant(patch.metadata.address))
                     .generate(&patch.metadata.inst, patch.metadata.address,
                               patch.metadata.instSize, MCII, &tempManager,
                               nullptr));
          break;
        // In POSTINST PC is set to the next instruction address
        case InstPosition::POSTINST:
          append(instru,
                 GetConstant(Temp(0), Constant(patch.metadata.address +
                                               patch.metadata.instSize))
                     .generate(&patch.metadata.inst, patch.metadata.address,
                               patch.metadata.instSize, MCII, &tempManager,
                               nullptr));
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
  // Prepend the temporary register saving code to the instrumentation
  Reg::Vec usedRegisters = tempManager.getUsedRegisters();
  for (uint32_t i = 0; i < usedRegisters.size(); i++) {
    prepend(instru, SaveReg(usedRegisters[i], Offset(usedRegisters[i])));
  }

  // In the break to host case the first used register is not restored and
  // instead given to the break to host code as a scratch. It will later be
  // restored by the break to host code.
  if (breakToHost) {
    for (uint32_t i = 1; i < usedRegisters.size(); i++) {
      append(instru, LoadReg(usedRegisters[i], Offset(usedRegisters[i])));
    }
    append(instru, getBreakToHost(usedRegisters[0]));
  }
  // Normal case where we append the temporary register restoration code to the
  // instrumentation
  else {
    for (uint32_t i = 0; i < usedRegisters.size(); i++) {
      append(instru, LoadReg(usedRegisters[i], Offset(usedRegisters[i])));
    }
  }

  // The resulting instrumentation is either appended or prepended as per the
  // InstPosition
  if (position == PREINST) {
    patch.prepend(std::move(instru));
  } else if (position == POSTINST) {
    patch.append(std::move(instru));
  } else {
    QBDI_ERROR("Invalid position 0x{:x}", position);
    abort();
  }
}

bool InstrRuleBasic::canBeApplied(const Patch &patch,
                                  const llvm::MCInstrInfo *MCII) const {
  return condition->test(patch.metadata.inst, patch.metadata.address,
                         patch.metadata.instSize, MCII);
}

bool InstrRuleDynamic::canBeApplied(const Patch &patch,
                                    const llvm::MCInstrInfo *MCII) const {
  return condition->test(patch.metadata.inst, patch.metadata.address,
                         patch.metadata.instSize, MCII);
}

bool InstrRuleUser::tryInstrument(Patch &patch, const llvm::MCInstrInfo *MCII,
                                  const llvm::MCRegisterInfo *MRI,
                                  const Assembly *assembly) const {
  if (!range.contains(
          Range<rword>(patch.metadata.address,
                       patch.metadata.address + patch.metadata.instSize))) {
    return false;
  }

  QBDI_DEBUG("Call user InstrCB at 0x{:x} with analysisType 0x{:x}", cbk,
             analysisType);

  const InstAnalysis *ana =
      analyzeInstMetadata(patch.metadata, analysisType, *assembly);

  std::vector<InstrRuleDataCBK> vec = cbk(vm, ana, cbk_data);

  QBDI_DEBUG("InstrCB return {} callback(s)", vec.size());

  if (vec.size() == 0) {
    return false;
  }

  for (const InstrRuleDataCBK &cbkToAdd : vec) {
    instrument(patch, MCII, MRI,
               getCallbackGenerator(cbkToAdd.cbk, cbkToAdd.data), true,
               cbkToAdd.position);
  }

  return true;
}

} // namespace QBDI
