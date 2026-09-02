/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2026 Quarkslab
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
      OpIsIn<llvm::X86::LOCK_PREFIX, llvm::X86::REX64_PREFIX,
             llvm::X86::REP_PREFIX, llvm::X86::REPNE_PREFIX,
             llvm::X86::DATA16_PREFIX, llvm::X86::CS_PREFIX,
             llvm::X86::SS_PREFIX, llvm::X86::DS_PREFIX, llvm::X86::ES_PREFIX,
             llvm::X86::FS_PREFIX, llvm::X86::GS_PREFIX,
             llvm::X86::XACQUIRE_PREFIX, llvm::X86::XRELEASE_PREFIX>::unique(),
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
  rules.emplace_back(OpIsIn<llvm::X86::JMP32m, llvm::X86::JMP64m>::unique(),
                     conv_unique<PatchGenerator>(
                         ModifyInstruction::unique(conv_unique<InstTransform>(
                             SetOpcode::unique(is_x86 ? llvm::X86::MOV32rm
                                                      : llvm::X86::MOV64rm),
                             AddOperand::unique(Operand(0), Temp(0)))),
                         WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #5: Simulate JMP to 16-bit memory value.
   * Target:   JMP16m
   * Patch:    JMP16m MEM --> MOVZX Temp(0), MEM16
   *           DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(OpIs::unique(llvm::X86::JMP16m),
                     conv_unique<PatchGenerator>(
                         ModifyInstruction::unique(conv_unique<InstTransform>(
                             SetOpcode::unique(is_x86 ? llvm::X86::MOVZX32rm16
                                                      : llvm::X86::MOVZX64rm16),
                             AddOperand::unique(Operand(0), Temp(0)))),
                         WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #6: Simulate CALL to memory value.
   * Target:  CALL MEM
   * Patch:   CALL MEM --> MOV Temp(0), MEM
   *          SimulateCall(Temp(1))
   */
  rules.emplace_back(OpIsIn<llvm::X86::CALL32m, llvm::X86::CALL64m>::unique(),
                     conv_unique<PatchGenerator>(
                         ModifyInstruction::unique(conv_unique<InstTransform>(
                             SetOpcode::unique(is_x86 ? llvm::X86::MOV32rm
                                                      : llvm::X86::MOV64rm),
                             AddOperand::unique(Operand(0), Temp(0)))),
                         SimulateCall::unique(Temp(0))));

  /* Rule #7: Simulate CALL to 16-bit memory value.
   * Target:   CALL16m
   * Patch:    CALL16m MEM --> MOVZX Temp(0), MEM16
   *           SimulateCall(Temp(0))
   */
  rules.emplace_back(OpIs::unique(llvm::X86::CALL16m),
                     conv_unique<PatchGenerator>(
                         ModifyInstruction::unique(conv_unique<InstTransform>(
                             SetOpcode::unique(is_x86 ? llvm::X86::MOVZX32rm16
                                                      : llvm::X86::MOVZX64rm16),
                             AddOperand::unique(Operand(0), Temp(0)))),
                         SimulateCall::unique(Temp(0))));

  /* Rule #8: Simulate JMP to constant value.
   * Target:  JMP IMM
   * Patch:   Temp(0) := RIP + Operand(0)
   *          DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      OpIsIn<llvm::X86::JMP_1, llvm::X86::JMP_2, llvm::X86::JMP_4>::unique(),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(0)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #9: Simulate JMP to register value.
   * Target:  JMP REG
   * Patch:   Temp(0) := Operand(0)
   *          DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(OpIsIn<llvm::X86::JMP32r, llvm::X86::JMP64r>::unique(),
                     conv_unique<PatchGenerator>(
                         GetOperand::unique(Temp(0), Operand(0)),
                         WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #10: Simulate JMP to 16-bit register value.
   * Target:   JMP16r
   * Patch:    JMP16r REG16 --> MOVZX Temp(0), REG16
   *           DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(OpIs::unique(llvm::X86::JMP16r),
                     conv_unique<PatchGenerator>(
                         ModifyInstruction::unique(conv_unique<InstTransform>(
                             SetOpcode::unique(is_x86 ? llvm::X86::MOVZX32rr16
                                                      : llvm::X86::MOVZX64rr16),
                             AddOperand::unique(Operand(0), Temp(0)))),
                         WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #11: Simulate CALL to register value.
   * Target:  CALL REG
   * Patch:   Temp(0) := Operand(0)
   *          SimulateCall(Temp(0))
   */
  rules.emplace_back(
      OpIsIn<llvm::X86::CALL32r, llvm::X86::CALL64r>::unique(),
      conv_unique<PatchGenerator>(GetOperand::unique(Temp(0), Operand(0)),
                                  SimulateCall::unique(Temp(0))));

  /* Rule #12: Simulate CALL to 16-bit register value.
   * Target:   CALL16r
   * Patch:    CALL16r REG16 --> MOVZX Temp(0), REG16
   *           SimulateCall(Temp(0))
   */
  rules.emplace_back(OpIs::unique(llvm::X86::CALL16r),
                     conv_unique<PatchGenerator>(
                         ModifyInstruction::unique(conv_unique<InstTransform>(
                             SetOpcode::unique(is_x86 ? llvm::X86::MOVZX32rr16
                                                      : llvm::X86::MOVZX64rr16),
                             AddOperand::unique(Operand(0), Temp(0)))),
                         SimulateCall::unique(Temp(0))));

  /* Rule #13: Simulate Jcc IMM8.
   * Target:  Jcc IMM8
   * Patch:     Temp(0) := RIP + Operand(0)
   *         ---Jcc IMM8 --> Jcc END
   *         |  Temp(0) := RIP + Constant(0)
   *         -->END: DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      OpIsIn<llvm::X86::JCC_1, llvm::X86::LOOP, llvm::X86::LOOPE,
             llvm::X86::LOOPNE, llvm::X86::JRCXZ, llvm::X86::JECXZ,
             llvm::X86::JCXZ>::unique(),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(0)),
          ModifyInstruction::unique(
              conv_unique<InstTransform>(SetOperand::unique(
                  Operand(0),
                  Constant(is_x86 ? 5 : 10)) // Offset to jump the next load.
                                         )),
          GetPCOffset::unique(Temp(0), Constant(0)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #14: Simulate Jcc IMM16.
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
              SetOperand::unique(Operand(0), Constant(is_x86 ? 5 : 10)))),
          GetPCOffset::unique(Temp(0), Constant(0)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #15: Simulate Jcc IMM32.
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
              SetOperand::unique(Operand(0), Constant(is_x86 ? 5 : 10)))),
          GetPCOffset::unique(Temp(0), Constant(0)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #16: Simulate CALL to constant offset.
   * Target:   CALL IMM
   * Patch:    Temp(0) := RIP + Operand(0)
   *           SimulateCall(Temp(0))
   */
  rules.emplace_back(
      OpIsIn<llvm::X86::CALL64pcrel32, llvm::X86::CALLpcrel16,
             llvm::X86::CALLpcrel32>::unique(),
      conv_unique<PatchGenerator>(GetPCOffset::unique(Temp(0), Operand(0)),
                                  SimulateCall::unique(Temp(0))));

  /* Rule #17: Simulate return.
   * Target:   RET
   * Patch:    SimulateRet(Temp(0))
   */
  rules.emplace_back(
      OpIsIn<llvm::X86::RET32, llvm::X86::RET64, llvm::X86::RET16,
             llvm::X86::RETI32, llvm::X86::RETI64, llvm::X86::RETI16>::unique(),
      conv_unique<PatchGenerator>(SimulateRet::unique(Temp(0))));

  /* Rule #18: Simulate far return.
   * Target:   LRET
   * Patch:    SimulateLret(Temp(0))
   */
  rules.emplace_back(
      OpIsIn<llvm::X86::LRET32, llvm::X86::LRET64, llvm::X86::LRET16,
             llvm::X86::LRETI32, llvm::X86::LRETI64,
             llvm::X86::LRETI16>::unique(),
      conv_unique<PatchGenerator>(SimulateLret::unique(Temp(0))));

  /* Rule #19: Default rule for every other instructions.
   * Target:   *
   * Patch:    Output original unmodified instructions.
   */
  rules.emplace_back(True::unique(),
                     conv_unique<PatchGenerator>(ModifyInstruction::unique(
                         InstTransform::UniquePtrVec())));

  return rules;
}

bool isSupportedInstruction(const Patch &patch, const char *&reason) {
  [[maybe_unused]] const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;
  switch (inst.getOpcode()) {
    case llvm::X86::XBEGIN_2:
    case llvm::X86::XBEGIN_4:
      reason = "XBEGIN (Intel TSX) is not supported by QBDI";
      return false;
    case llvm::X86::FARCALL16m:
    case llvm::X86::FARCALL32m:
    case llvm::X86::FARCALL64m:
    case llvm::X86::FARJMP16m:
    case llvm::X86::FARJMP32m:
    case llvm::X86::FARJMP64m:
      reason =
          "Far call/jump reading a segment:offset target from memory "
          "is not supported by QBDI";
      return false;
    case llvm::X86::FARCALL32i:
    case llvm::X86::FARJMP32i:
      reason =
          "Far call/jump with an immediate segment:offset target is "
          "not supported by QBDI";
      return false;
    case llvm::X86::IRET16:
    case llvm::X86::IRET32:
    case llvm::X86::IRET64:
      reason =
          "IRET pops CS/EIP/EFLAGS from the stack and is not "
          "supported by QBDI";
      return false;
    case llvm::X86::CALL16m_NT:
    case llvm::X86::CALL16r_NT:
    case llvm::X86::CALL32m_NT:
    case llvm::X86::CALL32r_NT:
    case llvm::X86::CALL64m_NT:
    case llvm::X86::CALL64r_NT:
    case llvm::X86::JMP16m_NT:
    case llvm::X86::JMP16r_NT:
    case llvm::X86::JMP32m_NT:
    case llvm::X86::JMP32r_NT:
    case llvm::X86::JMP64m_NT:
    case llvm::X86::JMP64r_NT:
      reason =
          "CET no-track (notrack-prefixed) indirect call/jump is not "
          "supported by QBDI";
      return false;
    case llvm::X86::JMP64m_REX:
    case llvm::X86::JMP64r_REX:
      reason =
          "This tail-call-lowering REX-prefixed jump variant is not "
          "supported by QBDI";
      return false;
    case llvm::X86::RET:
    case llvm::X86::IRET:
      reason =
          "RET/IRET without an explicit operand-size suffix is not "
          "supported by QBDI";
      return false;
    case llvm::X86::FARCALL16i:
    case llvm::X86::FARJMP16i:
      reason =
          "Far call/jump with a 16-bit real-mode segment:offset "
          "target is not supported by QBDI";
      return false;
    default:
      return true;
  }
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
      case llvm::X86::CMPXCHG8B:
        patch.regUsage[getGPRPosition(llvm::X86::EAX)] |=
            RegisterUsage::RegisterSaved;
        patch.regUsage[getGPRPosition(llvm::X86::EBX)] |=
            RegisterUsage::RegisterSaved;
        patch.regUsage[getGPRPosition(llvm::X86::ECX)] |=
            RegisterUsage::RegisterSaved;
        patch.regUsage[getGPRPosition(llvm::X86::EDX)] |=
            RegisterUsage::RegisterSaved;
        break;
      default:
        break;
    }
  }

  return;
}

PatchRuleResult PatchRuleAssembly::generate(const llvm::MCInst &inst,
                                            rword address, uint32_t instSize,
                                            const LLVMCPU &llvmcpu,
                                            std::vector<Patch> &patchList,
                                            const char *&unsupportedReason) {

  Patch instPatch{inst, address, instSize, llvmcpu};
  setRegisterSaved(instPatch);

  if (not isSupportedInstruction(instPatch, unsupportedReason)) {
    return PatchRuleResult::UNSUPPORTED;
  }

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
        QBDI_REQUIRE_ABORT(
            position != -1,
            "Fail to get the position to insert the new patch {}", instPatch);

        // 3. add the instruction to merge at the flags ModifyInstructionFlags
        Patch &mergePatch = patchList.back();
        instPatch.insertAt(position, std::move(mergePatch.insts));

        // 4. keep some metadata
        instPatch.metadata.address = mergePatch.metadata.address;
        instPatch.metadata.instSize += mergePatch.metadata.instSize;
        instPatch.metadata.execblockFlags |= mergePatch.metadata.execblockFlags;
        instPatch.metadata.prefix = mergePatch.metadata.prefix;
        instPatch.metadata.prefix.push_back(mergePatch.metadata.inst);

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
        return PatchRuleResult::VALID;
      } else if (patch.metadata.modifyPC) {
        reset();
        return PatchRuleResult::VALID_END_BB;
      } else {
        return PatchRuleResult::VALID;
      }
    }
  }
  return PatchRuleResult::UNSUPPORTED;
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
