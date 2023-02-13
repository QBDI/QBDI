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
#include <algorithm>
#include <stddef.h>
#include <vector>

#include "X86InstrInfo.h"

#include "QBDI/Config.h"
#include "QBDI/Options.h"
#include "QBDI/State.h"
#include "Engine/LLVMCPU.h"
#include "ExecBlock/Context.h"
#include "Patch/InstTransform.h"
#include "Patch/PatchCondition.h"
#include "Patch/PatchGenerator.h"
#include "Patch/PatchRule.h"
#include "Patch/PatchRuleAssembly.h"
#include "Patch/PatchUtils.h"
#include "Patch/RelocatableInst.h"
#include "Patch/Types.h"
#include "Patch/X86_64/ExecBlockFlags_X86_64.h"
#include "Patch/X86_64/Layer2_X86_64.h"
#include "Patch/X86_64/PatchGenerator_X86_64.h"
#include "Utility/LogSys.h"
#include "Utility/System.h"

namespace QBDI {

namespace {

enum PatchGeneratorFlagsX86_64 {
  MergeFlag = PatchGeneratorFlags::ArchSpecificFlags
};

std::vector<PatchRule> getDefaultPatchRules(Options opts) {
  std::vector<PatchRule> rules;

  /* Rule #0: Avoid instrumenting instruction prefixes.
   * Target:  X86 prefixes (LOCK, REP and other REX prefixes).
   * Patch:   Output the unmodified MCInst but flag the patch as "do not
   * instrument".
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::X86::LOCK_PREFIX),
          OpIs::unique(llvm::X86::REX64_PREFIX),
          OpIs::unique(llvm::X86::REP_PREFIX),
          OpIs::unique(llvm::X86::REPNE_PREFIX),
          OpIs::unique(llvm::X86::DATA16_PREFIX),
          OpIs::unique(llvm::X86::CS_PREFIX),
          OpIs::unique(llvm::X86::SS_PREFIX),
          OpIs::unique(llvm::X86::DS_PREFIX),
          OpIs::unique(llvm::X86::ES_PREFIX),
          OpIs::unique(llvm::X86::FS_PREFIX),
          OpIs::unique(llvm::X86::GS_PREFIX),
          OpIs::unique(llvm::X86::XACQUIRE_PREFIX),
          OpIs::unique(llvm::X86::XRELEASE_PREFIX))),
      conv_unique<PatchGenerator>(
          PatchGenFlags::unique(PatchGeneratorFlagsX86_64::MergeFlag),
          ModifyInstruction::unique(InstTransform::UniquePtrVec())));

  /* Rule #1: Simulate jmp to memory value using RIP addressing.
   * Target:  JMP *[RIP + IMM]
   * Patch:   Temp(0) := RIP + Constant(0)
   *          JMP *[RIP + IMM] --> MOV Temp(1), [Temp(0) + IMM]
   *          DataBlock[Offset(RIP)] := Temp(1)
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::X86::JMP64m),
                                              UseReg::unique(Reg(REG_PC)))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0)),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)),
              SetOpcode::unique(llvm::X86::MOV64rm),
              AddOperand::unique(Operand(0), Temp(1)))),
          WriteTemp::unique(Temp(1), Offset(Reg(REG_PC)))));

  /* Rule #2: Simulate call to memory value using RIP addressing.
   * Target:  CALL *[RIP + IMM]
   * Patch:   Temp(0) := RIP + Constant(0)
   *          CALL *[RIP + IMM] --> MOV Temp(1), [Temp(0) + IMM]
   *          SimulateCall(Temp(1))
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::X86::CALL64m),
                                              UseReg::unique(Reg(REG_PC)))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0)),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)),
              SetOpcode::unique(llvm::X86::MOV64rm),
              AddOperand::unique(Operand(0), Temp(1)))),
          SimulateCall::unique(Temp(1))));

  /* Rule #3: Generic RIP patching.
   * Target:  Any instruction with RIP as operand, e.g. LEA RAX, [RIP + 1]
   * Patch:   Temp(0) := rip
   *          LEA RAX, [RIP + IMM] --> LEA RAX, [Temp(0) + IMM]
   */
  rules.emplace_back(
      UseReg::unique(Reg(REG_PC)),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0)),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0))))));

  /* Rule #4: Simulate JMP to memory value.
   * Target:  JMP *MEM
   * Patch:   JMP *MEM --> MOV Temp(0), MEM
   *          DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::X86::JMP32m),
                                             OpIs::unique(llvm::X86::JMP64m))),
      conv_unique<PatchGenerator>(
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOpcode::unique(is_x86 ? llvm::X86::MOV32rm
                                       : llvm::X86::MOV64rm),
              AddOperand::unique(Operand(0), Temp(0)))),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #5: Simulate CALL to memory value.
   * Target:  CALL MEM
   * Patch:   CALL MEM --> MOV Temp(0), MEM
   *          SimulateCall(Temp(1))
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::X86::CALL32m),
                                             OpIs::unique(llvm::X86::CALL64m))),
      conv_unique<PatchGenerator>(
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOpcode::unique(is_x86 ? llvm::X86::MOV32rm
                                       : llvm::X86::MOV64rm),
              AddOperand::unique(Operand(0), Temp(0)))),
          SimulateCall::unique(Temp(0))));

  /* Rule #6: Simulate JMP to constant value.
   * Target:  JMP IMM
   * Patch:   Temp(0) := RIP + Operand(0)
   *          DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::X86::JMP_1),
                                             OpIs::unique(llvm::X86::JMP_2),
                                             OpIs::unique(llvm::X86::JMP_4))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(0)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #7: Simulate JMP to register value.
   * Target:  JMP REG
   * Patch:   Temp(0) := Operand(0)
   *          DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::X86::JMP32r),
                                             OpIs::unique(llvm::X86::JMP64r))),
      conv_unique<PatchGenerator>(
          GetOperand::unique(Temp(0), Operand(0)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #8: Simulate CALL to register value.
   * Target:  CALL REG
   * Patch:   Temp(0) := Operand(0)
   *          SimulateCall(Temp(0))
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::X86::CALL32r),
                                             OpIs::unique(llvm::X86::CALL64r))),
      conv_unique<PatchGenerator>(GetOperand::unique(Temp(0), Operand(0)),
                                  SimulateCall::unique(Temp(0))));

  /* Rule #9: Simulate Jcc IMM8.
   * Target:  Jcc IMM8
   * Patch:     Temp(0) := RIP + Operand(0)
   *         ---Jcc IMM8 --> Jcc END
   *         |  Temp(0) := RIP + Constant(0)
   *         -->END: DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::X86::JCC_1), OpIs::unique(llvm::X86::LOOP),
          OpIs::unique(llvm::X86::LOOPE), OpIs::unique(llvm::X86::LOOPNE))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(0)),
          ModifyInstruction::unique(
              conv_unique<InstTransform>(SetOperand::unique(
                  Operand(0),
                  Constant(is_x86 ? 6 : 11)) // Offset to jump the next load.
                                         )),
          GetPCOffset::unique(Temp(0), Constant(0)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #10: Simulate Jcc IMM16.
   * Target:  Jcc IMM16
   * Patch:     Temp(0) := RIP + Operand(0)
   *         ---Jcc IMM16 --> Jcc END
   *         |  Temp(0) := RIP + Constant(0)
   *         -->END: DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      OpIs::unique(llvm::X86::JCC_2),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(0)),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(0), Constant(is_x86 ? 7 : 12)))),
          GetPCOffset::unique(Temp(0), Constant(0)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #11: Simulate Jcc IMM32.
   * Target:  Jcc IMM32
   * Patch:     Temp(0) := RIP + Operand(0)
   *         ---Jcc IMM32 --> Jcc END
   *         |  Temp(0) := RIP + Constant(0)
   *         -->END: DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      OpIs::unique(llvm::X86::JCC_4),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(0)),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(0), Constant(is_x86 ? 9 : 14)))),
          GetPCOffset::unique(Temp(0), Constant(0)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #12: Simulate CALL to constant offset.
   * Target:   CALL IMM
   * Patch:    Temp(0) := RIP + Operand(0)
   *           SimulateCall(Temp(0))
   */
  rules.emplace_back(
      Or::unique(
          conv_unique<PatchCondition>(OpIs::unique(llvm::X86::CALL64pcrel32),
                                      OpIs::unique(llvm::X86::CALLpcrel16),
                                      OpIs::unique(llvm::X86::CALLpcrel32))),
      conv_unique<PatchGenerator>(GetPCOffset::unique(Temp(0), Operand(0)),
                                  SimulateCall::unique(Temp(0))));

  /* Rule #13: Simulate return.
   * Target:   RET
   * Patch:    SimulateRet(Temp(0))
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::X86::RET32), OpIs::unique(llvm::X86::RET64),
          OpIs::unique(llvm::X86::RET16), OpIs::unique(llvm::X86::RETI32),
          OpIs::unique(llvm::X86::RETI64), OpIs::unique(llvm::X86::RETI16))),
      conv_unique<PatchGenerator>(SimulateRet::unique(Temp(0))));

  /* Rule #14: Default rule for every other instructions.
   * Target:   *
   * Patch:    Output original unmodified instructions.
   */
  rules.emplace_back(True::unique(),
                     conv_unique<PatchGenerator>(ModifyInstruction::unique(
                         InstTransform::UniquePtrVec())));

  return rules;
}

} // namespace

PatchRuleAssembly::PatchRuleAssembly(Options opts)
    : patchRules(getDefaultPatchRules(opts)), options(opts),
      mergePending(false) {}

PatchRuleAssembly::~PatchRuleAssembly() = default;

void PatchRuleAssembly::reset() { mergePending = false; }

bool PatchRuleAssembly::changeOptions(Options opts) {
  // reset the current state. Options cannot be change during the Engine::patch
  // method
  reset();

  const Options needRecreate = Options::OPT_DISABLE_FPR |
#if defined(QBDI_ARCH_X86_64)
                               Options::OPT_ENABLE_FS_GS |
#endif
                               Options::OPT_DISABLE_OPTIONAL_FPR |
                               Options::OPT_DISABLE_MEMORYACCESS_VALUE;
  if ((opts & needRecreate) != (options & needRecreate)) {
    patchRules = getDefaultPatchRules(opts);
    options = opts;
    return true;
  }
  options = opts;
  return false;
}

static void setRegisterSaved(Patch &patch) {

  if constexpr (is_x86) {
    switch (patch.metadata.inst.getOpcode()) {
      case llvm::X86::PUSHA16:
      case llvm::X86::PUSHA32:
      case llvm::X86::POPA16:
      case llvm::X86::POPA32:
        // allows TmpManager to reuse the register
        for (unsigned i = 0; i < AVAILABLE_GPR; i++) {
          patch.regUsage[i] |= RegisterUsage::RegisterSaved;
        }
        break;
      default:
        break;
    }
  }

  return;
}

bool PatchRuleAssembly::generate(const llvm::MCInst &inst, rword address,
                                 uint32_t instSize, const LLVMCPU &llvmcpu,
                                 std::vector<Patch> &patchList) {

  Patch instPatch{inst, address, instSize, llvmcpu};
  setRegisterSaved(instPatch);

  for (uint32_t j = 0; j < patchRules.size(); j++) {
    if (patchRules[j].canBeApplied(instPatch, llvmcpu)) {
      QBDI_DEBUG("Patch rule {} applied", j);
      if (mergePending) {
        QBDI_REQUIRE_ABORT(patchList.size() > 0, "No previous patch to merge");
        QBDI_DEBUG("Previous instruction merged");

        // 1. generate the patch for the current instruction
        patchRules[j].apply(instPatch, llvmcpu);

        // 2. get insert position
        int position = -1;
        for (auto &p : instPatch.patchGenFlags) {
          if (p.second == PatchGeneratorFlags::ModifyInstructionBeginFlags) {
            position = p.first;
            break;
          }
        }
        QBDI_REQUIRE_ABORT_PATCH(
            position != -1, instPatch,
            "Fail to get the position to insert the new patch");

        // 3. add the instruction to merge at the flags ModifyInstructionFlags
        Patch &mergePatch = patchList.back();
        instPatch.insertAt(position, std::move(mergePatch.insts));

        // 4. keep some metadata
        instPatch.metadata.address = mergePatch.metadata.address;
        instPatch.metadata.instSize += mergePatch.metadata.instSize;
        instPatch.metadata.execblockFlags |= mergePatch.metadata.execblockFlags;

        // 5. replace the patch
        mergePatch = std::move(instPatch);

      } else {
        patchRules[j].apply(instPatch, llvmcpu);
        patchList.push_back(std::move(instPatch));
      }
      Patch &patch = patchList.back();
      mergePending = false;
      for (auto &p : patch.patchGenFlags) {
        mergePending |= (p.second == PatchGeneratorFlagsX86_64::MergeFlag);
      }

      if (mergePending) {
        return false;
      } else if (patch.metadata.modifyPC) {
        reset();
        return true;
      } else {
        return false;
      }
    }
  }
  QBDI_ABORT_PATCH(instPatch, "Not PatchRule found for:");
}

bool PatchRuleAssembly::earlyEnd(const LLVMCPU &llvmcpu,
                                 std::vector<Patch> &patchList) {
  if (mergePending) {
    if (patchList.size() == 0) {
      QBDI_CRITICAL("Cannot remove pending Patch");
      return false;
    }
    patchList.pop_back();
  }
  reset();
  return true;
}

} // namespace QBDI
