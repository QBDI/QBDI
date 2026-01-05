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
#include "AArch64InstrInfo.h"

#include "Engine/LLVMCPU.h"
#include "Patch/AARCH64/Layer2_AARCH64.h"
#include "Patch/AARCH64/PatchGenerator_AARCH64.h"
#include "Patch/InstTransform.h"
#include "Patch/PatchCondition.h"
#include "Patch/PatchRule.h"
#include "Patch/PatchRuleAssembly.h"

#include "Utility/LogSys.h"
#include "Utility/System.h"

#include "QBDI/Options.h"

namespace QBDI {

namespace {

std::vector<PatchRule> getDefaultPatchRules(Options opts) {
  std::vector<PatchRule> rules;
  bool bypassPauth = ((opts & Options::OPT_BYPASS_PAUTH) != 0);

  /* Rule #0: Restore all register for SVC and BRK
   *
   * In AARCH64, all registers are not restore for all instruction.
   * X28 is only restore if needed and a random register (SR) hold the address
   * of the datablock. However, for instruction that create an interrupt, we
   * should restore all registers.
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::AArch64::SVC),
                                             OpIs::unique(llvm::AArch64::BRK))),
      conv_unique<PatchGenerator>(
          FullRegisterRestore::unique(true),
          ModifyInstruction::unique(InstTransform::UniquePtrVec()),
          FullRegisterReset::unique(true)));

  /* Rule #1: Simulate RET and BR
   * Target:  RET REG64 Xn
   * Patch:   DataBlock[Offset(PC)] := Xn
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::AArch64::RET),
                                             OpIs::unique(llvm::AArch64::BR))),
      conv_unique<PatchGenerator>(
          GetOperand::unique(Temp(0), Operand(0)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC))),
          SaveX28IfSet::unique()));

  /* Rule #2: Simulate BLR
   * Target:  BLR REG64 Xn
   * Patch:   DataBlock[Offset(RIP)] := Xn
   *          SimulateLink(Temp(0))
   */
  rules.emplace_back(OpIs::unique(llvm::AArch64::BLR),
                     conv_unique<PatchGenerator>(
                         GetOperand::unique(Temp(0), Operand(0)),
                         WriteTemp::unique(Temp(0), Offset(Reg(REG_PC))),
                         SimulateLink::unique(Temp(0)),
                         SaveX28IfSet::unique()));

  /* Rule #3: Simulate BL
   * Target:  BL IMM
   * Patch:   DataBlock[Offset(RIP)] := PC + Operand(0)
   *          SimulateLink(Temp(0))
   */
  rules.emplace_back(OpIs::unique(llvm::AArch64::BL),
                     conv_unique<PatchGenerator>(
                         GetPCOffset::unique(Temp(0), Operand(0)),
                         WriteTemp::unique(Temp(0), Offset(Reg(REG_PC))),
                         SimulateLink::unique(Temp(0)),
                         SaveX28IfSet::unique()));

  /* Rule #4: Simulate B
   * Target:  B IMM
   * Patch:   DataBlock[Offset(RIP)] := PC + Operand(0)
   */
  rules.emplace_back(OpIs::unique(llvm::AArch64::B),
                     conv_unique<PatchGenerator>(
                         GetPCOffset::unique(Temp(0), Operand(0)),
                         WriteTemp::unique(Temp(0), Offset(Reg(REG_PC))),
                         SaveX28IfSet::unique()));

  /* Rule #5: Simulate Bcc
   * Target:  Bcc IMM
   * Patch:     Temp(0) := PC + Operand(0)
   *        --- Bcc IMM -> BCC 8
   *        |   Temp(0) := PC + 4
   *        --> DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::AArch64::Bcc), OpIs::unique(llvm::AArch64::BCcc))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(1)),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(1), Constant(8 / 4)))),
          GetPCOffset::unique(Temp(0), Constant(4)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC))),
          SaveX28IfSet::unique()));

  /* Rule #6: Simulate ADR and ADRP
   * Target:  ADR Xn, IMM
   * Patch:   Xn := PC + Operand(1)
   *
   * Target:  ADRP Xn, IMM
   * Patch:   Xn := (PC & ~0xfff) + (Operand(1) * 0x1000)
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::AArch64::ADRP),
                                             OpIs::unique(llvm::AArch64::ADR))),
      conv_unique<PatchGenerator>(GetPCOffset::unique(Operand(0), Operand(1)),
                                  SaveX28IfSet::unique()));

  /* Rule #7: Simulate TBZ and TBNZ
   * Target:    TBNZ Xn, #imm, label
   * Patch:     Temp(0) := PC + Operand(2)
   *        --- TBNZ Xn, #imm, label -> TBNZ Xn, #imm, 8
   *        |   Temp(0) := PC + 4
   *        --> DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::AArch64::TBNZX),
          OpIs::unique(llvm::AArch64::TBNZW), OpIs::unique(llvm::AArch64::TBZX),
          OpIs::unique(llvm::AArch64::TBZW))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(2)),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(2), Constant(8 / 4)))),
          GetPCOffset::unique(Temp(0), Constant(4)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC))),
          SaveX28IfSet::unique()));

  /* Rule #8: Simulate CBZ and CBNZ
   * Target:    TBNZ Xn, #imm, label
   * Patch:     Temp(0) := PC + Operand(2)
   *        --- TBNZ Xn, #imm, label -> TBNZ Xn, #imm, 8
   *        |   Temp(0) := PC + 4
   *        --> DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::AArch64::CBNZX),
          OpIs::unique(llvm::AArch64::CBNZW), OpIs::unique(llvm::AArch64::CBZX),
          OpIs::unique(llvm::AArch64::CBZW))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(1)),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(1), Constant(8 / 4)))),
          GetPCOffset::unique(Temp(0), Constant(4)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC))),
          SaveX28IfSet::unique()));

  /* Rule #9: Simulate load literal
   * Target:    LDR Xn, label
   * Patch:     Operand(0) := LDR(PC + Operand(1))
   */
  rules.emplace_back(
      Or::unique(
          conv_unique<PatchCondition>(OpIs::unique(llvm::AArch64::LDRSl),
                                      OpIs::unique(llvm::AArch64::LDRDl),
                                      OpIs::unique(llvm::AArch64::LDRQl),
                                      OpIs::unique(llvm::AArch64::LDRXl),
                                      OpIs::unique(llvm::AArch64::LDRWl),
                                      OpIs::unique(llvm::AArch64::LDRSWl))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(1)),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              ReplaceOpcode::unique(std::map<unsigned, unsigned>({
                  {llvm::AArch64::LDRSl, llvm::AArch64::LDRSui},
                  {llvm::AArch64::LDRDl, llvm::AArch64::LDRDui},
                  {llvm::AArch64::LDRQl, llvm::AArch64::LDRQui},
                  {llvm::AArch64::LDRXl, llvm::AArch64::LDRXui},
                  {llvm::AArch64::LDRWl, llvm::AArch64::LDRWui},
                  {llvm::AArch64::LDRSWl, llvm::AArch64::LDRSWui},
              })),
              AddOperand::unique(Operand(1), Temp(0)),
              SetOperand::unique(Operand(2), Constant(0)))),
          SaveX28IfSet::unique()));

  /* Rule #10: Simulate BRAA, BRAB, BRAAZ, BRABZ, RETAA, RETAB
   * Target:  BRAA Xn, Xm
   * Patch:   DataBlock[Offset(PC)] := Authia(Xn, Xm)
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::AArch64::BRAA), OpIs::unique(llvm::AArch64::BRAB),
          OpIs::unique(llvm::AArch64::BRAAZ),
          OpIs::unique(llvm::AArch64::BRABZ),
          OpIs::unique(llvm::AArch64::RETAA),
          OpIs::unique(llvm::AArch64::RETAB))),
      conv_unique<PatchGenerator>(
          GetAddrAuth::unique(Temp(0), bypassPauth),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC))),
          SaveX28IfSet::unique()));

  /* Rule #11: Simulate BLRAA, BLRAB, BLRAAZ, BLRABZ
   * Target:  BLRAA Xn, Xm
   * Patch:   DataBlock[Offset(PC)] := Authia(Xn, Xm)
   *          SimulateLink(Temp(0))
   */
  rules.emplace_back(
      Or::unique(
          conv_unique<PatchCondition>(OpIs::unique(llvm::AArch64::BLRAA),
                                      OpIs::unique(llvm::AArch64::BLRAB),
                                      OpIs::unique(llvm::AArch64::BLRAAZ),
                                      OpIs::unique(llvm::AArch64::BLRABZ))),
      conv_unique<PatchGenerator>(
          GetAddrAuth::unique(Temp(0), bypassPauth),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC))),
          SimulateLink::unique(Temp(0)), SaveX28IfSet::unique()));

  if (bypassPauth) {

    /* Rule #12: Replace AUTDA, AUTDB, AUTIA, AUTIB
     * Target:  AUTDA Xn, Xm
     * Patch:   XPACD Xn
     */
    rules.emplace_back(
        Or::unique(
            conv_unique<PatchCondition>(OpIs::unique(llvm::AArch64::AUTDA),
                                        OpIs::unique(llvm::AArch64::AUTDB),
                                        OpIs::unique(llvm::AArch64::AUTIA),
                                        OpIs::unique(llvm::AArch64::AUTIB))),
        conv_unique<PatchGenerator>(
            ModifyInstruction::unique(conv_unique<InstTransform>(
                ReplaceOpcode::unique(std::map<unsigned, unsigned>({
                    {llvm::AArch64::AUTDA, llvm::AArch64::XPACD},
                    {llvm::AArch64::AUTDB, llvm::AArch64::XPACD},
                    {llvm::AArch64::AUTIA, llvm::AArch64::XPACI},
                    {llvm::AArch64::AUTIB, llvm::AArch64::XPACI},
                })),
                RemoveOperand::unique(Operand(2)))),
            SaveX28IfSet::unique()));

    /* Rule #13: Replace AUTDZA, AUTDZB, AUTIZA, AUTIZB
     * Target:  AUTDZA Xn
     * Patch:   XPACD Xn
     */
    rules.emplace_back(
        Or::unique(
            conv_unique<PatchCondition>(OpIs::unique(llvm::AArch64::AUTDZA),
                                        OpIs::unique(llvm::AArch64::AUTDZB),
                                        OpIs::unique(llvm::AArch64::AUTIZA),
                                        OpIs::unique(llvm::AArch64::AUTIZB))),
        conv_unique<PatchGenerator>(
            ModifyInstruction::unique(conv_unique<InstTransform>(
                ReplaceOpcode::unique(std::map<unsigned, unsigned>({
                    {llvm::AArch64::AUTDZA, llvm::AArch64::XPACD},
                    {llvm::AArch64::AUTDZB, llvm::AArch64::XPACD},
                    {llvm::AArch64::AUTIZA, llvm::AArch64::XPACI},
                    {llvm::AArch64::AUTIZB, llvm::AArch64::XPACI},
                })))),
            SaveX28IfSet::unique()));

    /* Rule #14: Replace AUTIA1716, AUTIB1716
     * Target:  AUTIA1716
     * Patch:   XPAID X17
     */
    rules.emplace_back(Or::unique(conv_unique<PatchCondition>(
                           OpIs::unique(llvm::AArch64::AUTIA1716),
                           OpIs::unique(llvm::AArch64::AUTIB1716))),
                       conv_unique<PatchGenerator>(
                           ModifyInstruction::unique(conv_unique<InstTransform>(
                               SetOpcode::unique(llvm::AArch64::XPACI),
                               AddOperand::unique(Operand(0), Reg(17)),
                               AddOperand::unique(Operand(1), Reg(17)))),
                           SaveX28IfSet::unique()));

    /* Rule #15: Replace AUTIASP, AUTIAZ, AUTIBSP, AUTIBZ
     * Target:  AUTIASP
     * Patch:   XPAID X30
     */
    rules.emplace_back(Or::unique(conv_unique<PatchCondition>(
                           OpIs::unique(llvm::AArch64::AUTIASP),
                           OpIs::unique(llvm::AArch64::AUTIAZ),
                           OpIs::unique(llvm::AArch64::AUTIBSP),
                           OpIs::unique(llvm::AArch64::AUTIBZ))),
                       conv_unique<PatchGenerator>(
                           ModifyInstruction::unique(conv_unique<InstTransform>(
                               SetOpcode::unique(llvm::AArch64::XPACI),
                               AddOperand::unique(Operand(0), Reg(30)),
                               AddOperand::unique(Operand(1), Reg(30)))),
                           SaveX28IfSet::unique()));

    /* Rule #16: Replace LDRAA, LDRAB indexed
     * Target:  LDRAA Xn, [Xm, #imm]
     * Patch:   LDR Xn, [(unauth Xm + imm), 0]
     */
    rules.emplace_back(Or::unique(conv_unique<PatchCondition>(
                           OpIs::unique(llvm::AArch64::LDRAAindexed),
                           OpIs::unique(llvm::AArch64::LDRABindexed))),
                       conv_unique<PatchGenerator>(
                           GetAddrAuth::unique(Temp(0), true),
                           ModifyInstruction::unique(conv_unique<InstTransform>(
                               SetOpcode::unique(llvm::AArch64::LDRXui),
                               SetOperand::unique(Operand(1), Temp(0)),
                               SetOperand::unique(Operand(2), Constant(0)))),
                           SaveX28IfSet::unique()));

    /* Rule #17: Replace LDRAA, LDRAB writeback
     * Target:  LDRAA Xn, [Xm, #imm]!
     * Patch:   Xm := (unauth Xm + imm)
     *          LDR Xn, [Xm, 0]
     */
    rules.emplace_back(Or::unique(conv_unique<PatchCondition>(
                           OpIs::unique(llvm::AArch64::LDRAAwriteback),
                           OpIs::unique(llvm::AArch64::LDRABwriteback))),
                       conv_unique<PatchGenerator>(
                           GetAddrAuth::unique(Operand(2), true),
                           ModifyInstruction::unique(conv_unique<InstTransform>(
                               SetOpcode::unique(llvm::AArch64::LDRXui),
                               RemoveOperand::unique(Operand(0)),
                               SetOperand::unique(Operand(2), Constant(0)))),
                           SaveX28IfSet::unique()));
  }

  if ((opts & Options::OPT_DISABLE_LOCAL_MONITOR) == 0) {
    /* Rule #12: Clear local monitor state
     */
    rules.emplace_back(
        OpIs::unique(llvm::AArch64::CLREX),
        conv_unique<PatchGenerator>(
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTemp::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            SaveX28IfSet::unique()));

    /* Rule #13: Clear local monitor state on SVC
     */
    rules.emplace_back(
        OpIs::unique(llvm::AArch64::SVC),
        conv_unique<PatchGenerator>(
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            // for SVC, we need to backup the value of Temp(0) after the syscall
            SaveTemp::unique(Temp(0)),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTemp::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            SaveX28IfSet::unique()));

    /* Rule #14: exclusive load 1 register
     */
    rules.emplace_back(
        Or::unique(
            conv_unique<PatchCondition>(OpIs::unique(llvm::AArch64::LDXRB),
                                        OpIs::unique(llvm::AArch64::LDXRH),
                                        OpIs::unique(llvm::AArch64::LDXRW),
                                        OpIs::unique(llvm::AArch64::LDXRX),
                                        OpIs::unique(llvm::AArch64::LDAXRB),
                                        OpIs::unique(llvm::AArch64::LDAXRH),
                                        OpIs::unique(llvm::AArch64::LDAXRW),
                                        OpIs::unique(llvm::AArch64::LDAXRX))),
        conv_unique<PatchGenerator>(
            GetConstant::unique(Temp(0), Constant(1)),
            WriteTemp::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            WriteOperand::unique(
                Operand(1),
                Offset(offsetof(Context, gprState.localMonitor.addr))),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            SaveX28IfSet::unique()));

    /* Rule #15: exclusive load 2 register
     */
    rules.emplace_back(
        Or::unique(
            conv_unique<PatchCondition>(OpIs::unique(llvm::AArch64::LDXPW),
                                        OpIs::unique(llvm::AArch64::LDXPX),
                                        OpIs::unique(llvm::AArch64::LDAXPW),
                                        OpIs::unique(llvm::AArch64::LDAXPX))),
        conv_unique<PatchGenerator>(
            GetConstant::unique(Temp(0), Constant(1)),
            WriteTemp::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            WriteOperand::unique(
                Operand(2),
                Offset(offsetof(Context, gprState.localMonitor.addr))),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            SaveX28IfSet::unique()));

    /* Rule #16: exclusive store
     */
    rules.emplace_back(
        Or::unique(
            conv_unique<PatchCondition>(OpIs::unique(llvm::AArch64::STXRB),
                                        OpIs::unique(llvm::AArch64::STXRH),
                                        OpIs::unique(llvm::AArch64::STXRW),
                                        OpIs::unique(llvm::AArch64::STXRX),
                                        OpIs::unique(llvm::AArch64::STXPW),
                                        OpIs::unique(llvm::AArch64::STXPX),
                                        OpIs::unique(llvm::AArch64::STLXRB),
                                        OpIs::unique(llvm::AArch64::STLXRH),
                                        OpIs::unique(llvm::AArch64::STLXRW),
                                        OpIs::unique(llvm::AArch64::STLXRX),
                                        OpIs::unique(llvm::AArch64::STLXPW),
                                        OpIs::unique(llvm::AArch64::STLXPX))),
        conv_unique<PatchGenerator>(
            CondExclusifLoad::unique(Temp(0)),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTemp::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            SaveX28IfSet::unique()));
  }

  // rules.push_back(
  //    PatchRule(
  //      Or({
  //        OpIs(llvm::AArch64::PRFMui),
  //      }),
  //      {
  //        DoNotInstrument(),
  //        //NopGen(),
  //        ModifyInstruction({})
  //      })
  //);

  // Should we ?
  // rules.push_back(
  //    PatchRule(
  //      IsAtomic(),
  //      {
  //        DoNotInstrument(),
  //        ModifyInstruction({})
  //      }
  //    )
  //);

  rules.emplace_back(True::unique(), conv_unique<PatchGenerator>(
                                         ModifyInstruction::unique(
                                             InstTransform::UniquePtrVec()),
                                         SaveX28IfSet::unique()));

  return rules;
}

} // namespace

PatchRuleAssembly::PatchRuleAssembly(Options opts)
    : patchRules(getDefaultPatchRules(opts)), options(opts) {}

PatchRuleAssembly::~PatchRuleAssembly() = default;

bool PatchRuleAssembly::changeOptions(Options opts) {
  const Options needRecreate =
      Options::OPT_DISABLE_FPR | Options::OPT_DISABLE_OPTIONAL_FPR |
      Options::OPT_DISABLE_LOCAL_MONITOR | Options::OPT_BYPASS_PAUTH |
      Options::OPT_DISABLE_MEMORYACCESS_VALUE;
  if ((opts & needRecreate) != (options & needRecreate)) {
    patchRules = getDefaultPatchRules(opts);
    options = opts;
    return true;
  }
  options = opts;
  return false;
}

bool PatchRuleAssembly::generate(const llvm::MCInst &inst, rword address,
                                 uint32_t instSize, const LLVMCPU &llvmcpu,
                                 std::vector<Patch> &patchList) {

  Patch instPatch{inst, address, instSize, llvmcpu};

  for (uint32_t j = 0; j < patchRules.size(); j++) {
    if (patchRules[j].canBeApplied(instPatch, llvmcpu)) {
      QBDI_DEBUG("Patch rule {} applied", j);

      patchRules[j].apply(instPatch, llvmcpu);
      patchList.push_back(std::move(instPatch));
      Patch &patch = patchList.back();

      return patch.metadata.modifyPC;
    }
  }
  QBDI_ABORT("Not PatchRule found {}", instPatch);
}

bool PatchRuleAssembly::earlyEnd(const LLVMCPU &llvmcpu,
                                 std::vector<Patch> &patchList) {
  return true;
}

} // namespace QBDI
