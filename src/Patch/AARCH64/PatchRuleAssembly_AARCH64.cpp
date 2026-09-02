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
#include "AArch64InstrInfo.h"

#include "Engine/LLVMCPU.h"
#include "Patch/AARCH64/InstTransform_AARCH64.h"
#include "Patch/AARCH64/Layer2_AARCH64.h"
#include "Patch/AARCH64/MemoryAccess_AARCH64.h"
#include "Patch/AARCH64/PatchGenerator_AARCH64.h"
#include "Patch/InstTransform.h"
#include "Patch/PatchCondition.h"
#include "Patch/PatchRule.h"
#include "Patch/PatchRuleAssembly.h"

#include "Utility/AARCH64/AssemblyProbe.h"
#include "Utility/LogSys.h"

#include "QBDI/Options.h"

namespace QBDI {

namespace {

std::vector<PatchRule> getDefaultPatchRules(Options opts) {
  std::vector<PatchRule> rules;
  bool bypassPauth = ((opts & Options::OPT_BYPASS_PAUTH) != 0);
  bool hostHasPauth = hostSupportsPAuthProbe();

  /* Rule #0: Restore all register for SVC and BRK
   *
   * In AARCH64, all registers are not restore for all instruction.
   * X28 is only restore if needed and a random register (SR) hold the address
   * of the datablock. However, for instruction that create an interrupt, we
   * should restore all registers.
   */
  rules.emplace_back(
      OpIsIn<llvm::AArch64::SVC, llvm::AArch64::BRK>::unique(),
      conv_unique<PatchGenerator>(
          FullRegisterRestore::unique(true),
          ModifyInstruction::unique(InstTransform::UniquePtrVec()),
          FullRegisterReset::unique(true)));

  /* Rule #1: Simulate RET and BR
   * Target:  RET REG64 Xn
   * Patch:   DataBlock[Offset(PC)] := Xn
   */
  rules.emplace_back(OpIsIn<llvm::AArch64::RET, llvm::AArch64::BR>::unique(),
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
  rules.emplace_back(OpIsIn<llvm::AArch64::Bcc, llvm::AArch64::BCcc>::unique(),
                     conv_unique<PatchGenerator>(
                         GetPCOffset::unique(Temp(0), Operand(1)),
                         ModifyInstruction::unique(conv_unique<InstTransform>(
                             SetOperand::unique(Operand(1), Constant(8 / 4)))),
                         GetPCOffset::unique(Temp(0), Constant(4)),
                         WriteTemp::unique(Temp(0), Offset(Reg(REG_PC))),
                         SaveX28IfSet::unique()));

  /* Rule #6: Simulate CMPBR (compare-and-branch)
   * Target:    CBcc Rt, Rm|imm, label
   * Patch:     Temp(0) := PC + Operand(2)
   *        --- CBcc Rt, Rm|imm, label -> CBcc Rt, Rm|imm, 8
   *        |   Temp(0) := PC + 4
   *        --> DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      OpIsIn<llvm::AArch64::CBGTWrr, llvm::AArch64::CBGTXrr,
             llvm::AArch64::CBGEWrr, llvm::AArch64::CBGEXrr,
             llvm::AArch64::CBHIWrr, llvm::AArch64::CBHIXrr,
             llvm::AArch64::CBHSWrr, llvm::AArch64::CBHSXrr,
             llvm::AArch64::CBEQWrr, llvm::AArch64::CBEQXrr,
             llvm::AArch64::CBNEWrr, llvm::AArch64::CBNEXrr,
             llvm::AArch64::CBGTWri, llvm::AArch64::CBGTXri,
             llvm::AArch64::CBLTWri, llvm::AArch64::CBLTXri,
             llvm::AArch64::CBHIWri, llvm::AArch64::CBHIXri,
             llvm::AArch64::CBLOWri, llvm::AArch64::CBLOXri,
             llvm::AArch64::CBEQWri, llvm::AArch64::CBEQXri,
             llvm::AArch64::CBNEWri, llvm::AArch64::CBNEXri,
             llvm::AArch64::CBHGTWrr, llvm::AArch64::CBHGEWrr,
             llvm::AArch64::CBHHIWrr, llvm::AArch64::CBHHSWrr,
             llvm::AArch64::CBHEQWrr, llvm::AArch64::CBHNEWrr,
             llvm::AArch64::CBBGTWrr, llvm::AArch64::CBBGEWrr,
             llvm::AArch64::CBBHIWrr, llvm::AArch64::CBBHSWrr,
             llvm::AArch64::CBBEQWrr, llvm::AArch64::CBBNEWrr>::unique(),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(2)),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(2), Constant(8 / 4)))),
          GetPCOffset::unique(Temp(0), Constant(4)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC))),
          SaveX28IfSet::unique()));

  /* Rule #7: Simulate ADR and ADRP
   * Target:  ADR Xn, IMM
   * Patch:   Xn := PC + Operand(1)
   *
   * Target:  ADRP Xn, IMM
   * Patch:   Xn := (PC & ~0xfff) + (Operand(1) * 0x1000)
   */
  rules.emplace_back(
      OpIsIn<llvm::AArch64::ADRP, llvm::AArch64::ADR>::unique(),
      conv_unique<PatchGenerator>(GetPCOffset::unique(Operand(0), Operand(1)),
                                  SaveX28IfSet::unique()));

  /* Rule #8: Simulate TBZ and TBNZ
   * Target:    TBNZ Xn, #imm, label
   * Patch:     Temp(0) := PC + Operand(2)
   *        --- TBNZ Xn, #imm, label -> TBNZ Xn, #imm, 8
   *        |   Temp(0) := PC + 4
   *        --> DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(OpIsIn<llvm::AArch64::TBNZX, llvm::AArch64::TBNZW,
                            llvm::AArch64::TBZX, llvm::AArch64::TBZW>::unique(),
                     conv_unique<PatchGenerator>(
                         GetPCOffset::unique(Temp(0), Operand(2)),
                         ModifyInstruction::unique(conv_unique<InstTransform>(
                             SetOperand::unique(Operand(2), Constant(8 / 4)))),
                         GetPCOffset::unique(Temp(0), Constant(4)),
                         WriteTemp::unique(Temp(0), Offset(Reg(REG_PC))),
                         SaveX28IfSet::unique()));

  /* Rule #9: Simulate CBZ and CBNZ
   * Target:    TBNZ Xn, #imm, label
   * Patch:     Temp(0) := PC + Operand(2)
   *        --- TBNZ Xn, #imm, label -> TBNZ Xn, #imm, 8
   *        |   Temp(0) := PC + 4
   *        --> DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(OpIsIn<llvm::AArch64::CBNZX, llvm::AArch64::CBNZW,
                            llvm::AArch64::CBZX, llvm::AArch64::CBZW>::unique(),
                     conv_unique<PatchGenerator>(
                         GetPCOffset::unique(Temp(0), Operand(1)),
                         ModifyInstruction::unique(conv_unique<InstTransform>(
                             SetOperand::unique(Operand(1), Constant(8 / 4)))),
                         GetPCOffset::unique(Temp(0), Constant(4)),
                         WriteTemp::unique(Temp(0), Offset(Reg(REG_PC))),
                         SaveX28IfSet::unique()));

  /* Rule #10: Simulate load literal
   * Target:    LDR Xn, label
   * Patch:     Operand(0) := LDR(PC + Operand(1))
   */
  rules.emplace_back(
      OpIsIn<llvm::AArch64::LDRSl, llvm::AArch64::LDRDl, llvm::AArch64::LDRQl,
             llvm::AArch64::LDRXl, llvm::AArch64::LDRWl,
             llvm::AArch64::LDRSWl>::unique(),
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

  /* Rule #11: Simulate BRAA, BRAB, BRAAZ, BRABZ, RETAA, RETAB, RETAASPPCi,
   * RETABSPPCi, RETAASPPCr, RETABSPPCr
   * Target:  BRAA Xn, Xm
   * Patch:   DataBlock[Offset(PC)] := Authia(Xn, Xm)
   */
  rules.emplace_back(
      OpIsIn<llvm::AArch64::BRAA, llvm::AArch64::BRAB, llvm::AArch64::BRAAZ,
             llvm::AArch64::BRABZ, llvm::AArch64::RETAA, llvm::AArch64::RETAB,
             llvm::AArch64::RETAASPPCi, llvm::AArch64::RETABSPPCi,
             llvm::AArch64::RETAASPPCr, llvm::AArch64::RETABSPPCr>::unique(),
      conv_unique<PatchGenerator>(
          GetAddrAuth::unique(Temp(0), bypassPauth),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC))),
          SaveX28IfSet::unique()));

  /* Rule #12: Simulate BLRAA, BLRAB, BLRAAZ, BLRABZ
   * Target:  BLRAA Xn, Xm
   * Patch:   DataBlock[Offset(PC)] := Authia(Xn, Xm)
   *          SimulateLink(Temp(0))
   */
  rules.emplace_back(
      OpIsIn<llvm::AArch64::BLRAA, llvm::AArch64::BLRAB, llvm::AArch64::BLRAAZ,
             llvm::AArch64::BLRABZ>::unique(),
      conv_unique<PatchGenerator>(
          GetAddrAuth::unique(Temp(0), bypassPauth),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC))),
          SimulateLink::unique(Temp(0)), SaveX28IfSet::unique()));

  /* Rule #13: Sign LR for PACIASPPC, PACIBSPPC
   * Target:  PACIASPPC
   * Patch:   X17 := LR; X16 := SP; X15 := &PACIASPPC
   *          PACIA171615
   *          LR := X17
   */
  rules.emplace_back(
      OpIsIn<llvm::AArch64::PACIASPPC, llvm::AArch64::PACIBSPPC>::unique(),
      conv_unique<PatchGenerator>(SignLRSPPC::unique(Temp(0), Temp(1), Temp(2)),
                                  SaveX28IfSet::unique()));

  /* Rule #14: PACM state tracking
   */
  rules.emplace_back(
      OpIs::unique(llvm::AArch64::PACM),
      conv_unique<PatchGenerator>(
          GetConstant::unique(Temp(0), Constant(1)),
          WriteTemp::unique(Temp(0), Offset(offsetof(Context, gprState.pacm))),
          SaveX28IfSet::unique()));

  /* Rule #15: Reassert PACM before PACIA1716, PACIB1716
   */
  rules.emplace_back(
      OpIsIn<llvm::AArch64::PACIA1716, llvm::AArch64::PACIB1716>::unique(),
      conv_unique<PatchGenerator>(
          ReadTemp::unique(Temp(0), Offset(offsetof(Context, gprState.pacm))),
          GenCbz::unique(Temp(0), Constant(16)),
          GetConstant::unique(Temp(0), Constant(0)),
          WriteTemp::unique(Temp(0), Offset(offsetof(Context, gprState.pacm))),
          GenPACM::unique(),
          ModifyInstruction::unique(InstTransform::UniquePtrVec()),
          SaveX28IfSet::unique()));

  /* Rule #16: Sign LR for PACIASP, PACIBSP. If PACM,
   * 	       transform it to PACIA1716/PACIB1716 with PC in X15
   * Target:  PACIASP
   * Patch:     Temp := Context.gprState.pacm
   *        --- if Temp == 0 -> skip to PACIASP
   *        |   Context.gprState.pacm := 0
   *        |   X17 := LR; X16 := SP; X15 := &PACIASP
   *        |   PACM; PACIA1716
   *        |   LR := X17
   *        |   branch over PACIASP
   *        --> PACIASP
   */
  rules.emplace_back(
      OpIsIn<llvm::AArch64::PACIASP, llvm::AArch64::PACIBSP>::unique(),
      conv_unique<PatchGenerator>(SignLRSP::unique(Temp(0), Temp(1), Temp(2)),
                                  SaveX28IfSet::unique()));

  if (bypassPauth) {

    /* Rule #17: Replace AUTDA, AUTDB, AUTIA, AUTIB
     * Target:  AUTDA Xn, Xm
     * Patch:   XPACD Xn
     */
    rules.emplace_back(
        OpIsIn<llvm::AArch64::AUTDA, llvm::AArch64::AUTDB, llvm::AArch64::AUTIA,
               llvm::AArch64::AUTIB>::unique(),
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

    /* Rule #18: Replace AUTDZA, AUTDZB, AUTIZA, AUTIZB
     * Target:  AUTDZA Xn
     * Patch:   XPACD Xn
     */
    rules.emplace_back(
        OpIsIn<llvm::AArch64::AUTDZA, llvm::AArch64::AUTDZB,
               llvm::AArch64::AUTIZA, llvm::AArch64::AUTIZB>::unique(),
        conv_unique<PatchGenerator>(
            ModifyInstruction::unique(conv_unique<InstTransform>(
                ReplaceOpcode::unique(std::map<unsigned, unsigned>({
                    {llvm::AArch64::AUTDZA, llvm::AArch64::XPACD},
                    {llvm::AArch64::AUTDZB, llvm::AArch64::XPACD},
                    {llvm::AArch64::AUTIZA, llvm::AArch64::XPACI},
                    {llvm::AArch64::AUTIZB, llvm::AArch64::XPACI},
                })))),
            SaveX28IfSet::unique()));

    /* Rule #19: Replace AUTIA1716, AUTIB1716, AUTIA171615, AUTIB171615
     * Target:  AUTIA1716
     * Patch:   XPAID X17
     */
    if (hostHasPauth) {
      rules.emplace_back(
          OpIsIn<llvm::AArch64::AUTIA1716, llvm::AArch64::AUTIB1716,
                 llvm::AArch64::AUTIA171615,
                 llvm::AArch64::AUTIB171615>::unique(),
          conv_unique<PatchGenerator>(
              ModifyInstruction::unique(conv_unique<InstTransform>(
                  SetOpcode::unique(llvm::AArch64::XPACI),
                  AddOperand::unique(Operand(0), Reg(17)),
                  AddOperand::unique(Operand(1), Reg(17)))),
              SaveX28IfSet::unique()));
    }

    /* Rule #20: Replace AUTIASP, AUTIAZ, AUTIBSP, AUTIBZ
     * Target:  AUTIASP
     * Patch:   XPAID X30
     */
    if (hostHasPauth) {
      rules.emplace_back(
          OpIsIn<llvm::AArch64::AUTIASP, llvm::AArch64::AUTIAZ,
                 llvm::AArch64::AUTIBSP, llvm::AArch64::AUTIBZ>::unique(),
          conv_unique<PatchGenerator>(
              ModifyInstruction::unique(conv_unique<InstTransform>(
                  SetOpcode::unique(llvm::AArch64::XPACI),
                  AddOperand::unique(Operand(0), Reg(30)),
                  AddOperand::unique(Operand(1), Reg(30)))),
              SaveX28IfSet::unique()));
    }

    /* Rule #21: Replace AUTIASPPCi, AUTIBSPPCi, AUTIASPPCr, AUTIBSPPCr
     * Target:  AUTIASPPCi label / AUTIASPPCr Xm
     * Patch:   XPACI X30
     */
    rules.emplace_back(
        OpIsIn<llvm::AArch64::AUTIASPPCi, llvm::AArch64::AUTIBSPPCi,
               llvm::AArch64::AUTIASPPCr, llvm::AArch64::AUTIBSPPCr>::unique(),
        conv_unique<PatchGenerator>(
            ModifyInstruction::unique(conv_unique<InstTransform>(
                SetOpcode::unique(llvm::AArch64::XPACI),
                RemoveOperand::unique(Operand(0)),
                AddOperand::unique(Operand(0), Reg(30)),
                AddOperand::unique(Operand(1), Reg(30)))),
            SaveX28IfSet::unique()));

    /* Rule #22: Replace LDRAA, LDRAB indexed
     * Target:  LDRAA Xn, [Xm, #imm]
     * Patch:   LDR Xn, [(unauth Xm + imm), 0]
     */
    rules.emplace_back(OpIsIn<llvm::AArch64::LDRAAindexed,
                              llvm::AArch64::LDRABindexed>::unique(),
                       conv_unique<PatchGenerator>(
                           GetAddrAuth::unique(Temp(0), true),
                           ModifyInstruction::unique(conv_unique<InstTransform>(
                               SetOpcode::unique(llvm::AArch64::LDRXui),
                               SetOperand::unique(Operand(1), Temp(0)),
                               SetOperand::unique(Operand(2), Constant(0)))),
                           SaveX28IfSet::unique()));

    /* Rule #23: Replace LDRAA, LDRAB writeback
     * Target:  LDRAA Xn, [Xm, #imm]!
     * Patch:   Xm := (unauth Xm + imm)
     *          LDR Xn, [Xm, 0]
     */
    rules.emplace_back(OpIsIn<llvm::AArch64::LDRAAwriteback,
                              llvm::AArch64::LDRABwriteback>::unique(),
                       conv_unique<PatchGenerator>(
                           GetAddrAuth::unique(Operand(2), true),
                           ModifyInstruction::unique(conv_unique<InstTransform>(
                               SetOpcode::unique(llvm::AArch64::LDRXui),
                               RemoveOperand::unique(Operand(0)),
                               SetOperand::unique(Operand(2), Constant(0)))),
                           SaveX28IfSet::unique()));
  } else {
    /* Rule #17: Simulate AUTIASPPCi, AUTIBSPPCi
     * Target:  AUTIASPPCi label
     * Patch:   Temp(0) := PC + Operand(0)
     *          AUTIASPPCi label -> AUTIASPPCr Temp(0)
     */
    rules.emplace_back(
        OpIsIn<llvm::AArch64::AUTIASPPCi, llvm::AArch64::AUTIBSPPCi>::unique(),
        conv_unique<PatchGenerator>(
            GetPCOffset::unique(Temp(0), Operand(0)),
            ModifyInstruction::unique(conv_unique<InstTransform>(
                ReplaceOpcode::unique(std::map<unsigned, unsigned>({
                    {llvm::AArch64::AUTIASPPCi, llvm::AArch64::AUTIASPPCr},
                    {llvm::AArch64::AUTIBSPPCi, llvm::AArch64::AUTIBSPPCr},
                })),
                SetOperand::unique(Operand(0), Temp(0)))),
            SaveX28IfSet::unique()));

    /* Rule #18: Reassert PACM before AUTIASP, AUTIBSP, AUTIA1716, AUTIB1716
     */
    rules.emplace_back(
        OpIsIn<llvm::AArch64::AUTIASP, llvm::AArch64::AUTIBSP,
               llvm::AArch64::AUTIA1716, llvm::AArch64::AUTIB1716>::unique(),
        conv_unique<PatchGenerator>(
            ReadTemp::unique(Temp(0), Offset(offsetof(Context, gprState.pacm))),
            GenCbz::unique(Temp(0), Constant(16)),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTemp::unique(Temp(0),
                              Offset(offsetof(Context, gprState.pacm))),
            GenPACM::unique(),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            SaveX28IfSet::unique()));
  }

  if ((opts & Options::OPT_DISABLE_LOCAL_MONITOR) == 0) {
    /* Rule #19: Clear local monitor state
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

    /* Rule #20: Clear local monitor state on SVC
     */
    rules.emplace_back(
        OpIs::unique(llvm::AArch64::SVC),
        conv_unique<PatchGenerator>(
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            // for SVC, we need to backup the value of Temp(0) after the syscall
            SaveTemp::unique(Temp(0), true),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTemp::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            SaveX28IfSet::unique()));

    /* Rule #21: exclusive load 1 byte register
     */
    rules.emplace_back(
        OpIsIn<llvm::AArch64::LDXRB, llvm::AArch64::LDAXRB>::unique(),
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

    /* Rule #22: exclusive load 1 halfword register
     */
    rules.emplace_back(
        OpIsIn<llvm::AArch64::LDXRH, llvm::AArch64::LDAXRH>::unique(),
        conv_unique<PatchGenerator>(
            GetConstant::unique(Temp(0), Constant(2)),
            WriteTemp::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            WriteOperand::unique(
                Operand(1),
                Offset(offsetof(Context, gprState.localMonitor.addr))),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            SaveX28IfSet::unique()));

    /* Rule #23: exclusive load 1 word register
     */
    rules.emplace_back(
        OpIsIn<llvm::AArch64::LDXRW, llvm::AArch64::LDAXRW>::unique(),
        conv_unique<PatchGenerator>(
            GetConstant::unique(Temp(0), Constant(4)),
            WriteTemp::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            WriteOperand::unique(
                Operand(1),
                Offset(offsetof(Context, gprState.localMonitor.addr))),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            SaveX28IfSet::unique()));

    /* Rule #24: exclusive load 1 doubleword register
     */
    rules.emplace_back(
        OpIsIn<llvm::AArch64::LDXRX, llvm::AArch64::LDAXRX>::unique(),
        conv_unique<PatchGenerator>(
            GetConstant::unique(Temp(0), Constant(8)),
            WriteTemp::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            WriteOperand::unique(
                Operand(1),
                Offset(offsetof(Context, gprState.localMonitor.addr))),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            SaveX28IfSet::unique()));

    /* Rule #25: exclusive load 2 word registers
     */
    rules.emplace_back(
        OpIsIn<llvm::AArch64::LDXPW, llvm::AArch64::LDAXPW>::unique(),
        conv_unique<PatchGenerator>(
            GetConstant::unique(Temp(0), Constant(0x800)),
            WriteTemp::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            WriteOperand::unique(
                Operand(2),
                Offset(offsetof(Context, gprState.localMonitor.addr))),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            SaveX28IfSet::unique()));

    /* Rule #26: exclusive load 2 doubleword registers
     */
    rules.emplace_back(
        OpIsIn<llvm::AArch64::LDXPX, llvm::AArch64::LDAXPX>::unique(),
        conv_unique<PatchGenerator>(
            GetConstant::unique(Temp(0), Constant(16)),
            WriteTemp::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            WriteOperand::unique(
                Operand(2),
                Offset(offsetof(Context, gprState.localMonitor.addr))),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            SaveX28IfSet::unique()));

    /* Rule #27: exclusive store register(s)
     */
    rules.emplace_back(
        And::unique(conv_unique<PatchCondition>(
            OpIsIn<llvm::AArch64::STXRB, llvm::AArch64::STLXRB,
                   llvm::AArch64::STXRH, llvm::AArch64::STLXRH,
                   llvm::AArch64::STXRW, llvm::AArch64::STLXRW,
                   llvm::AArch64::STXRX, llvm::AArch64::STLXRX,
                   llvm::AArch64::STXPW, llvm::AArch64::STLXPW,
                   llvm::AArch64::STXPX, llvm::AArch64::STLXPX>::unique(),
            OperandIs::unique(0, RegLLVM(llvm::AArch64::WZR)))),
        conv_unique<PatchGenerator>(
            CondExclusifLoad::unique(Temp(0), Temp(1)),
            ModifyInstruction::unique(conv_unique<InstTransform>(
                SetOperandW::unique(Operand(0), Temp(0)))),
            WriteTemp::unique(Temp(0), Shadow(MEM_EXCLUSIVE_STATUS_TAG)),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTemp::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            SaveX28IfSet::unique()));

    /* Rule #28: exclusive store register(s)
     */
    rules.emplace_back(
        And::unique(conv_unique<PatchCondition>(
            OpIsIn<llvm::AArch64::STXRB, llvm::AArch64::STLXRB,
                   llvm::AArch64::STXRH, llvm::AArch64::STLXRH,
                   llvm::AArch64::STXRW, llvm::AArch64::STLXRW,
                   llvm::AArch64::STXRX, llvm::AArch64::STLXRX,
                   llvm::AArch64::STXPW, llvm::AArch64::STLXPW,
                   llvm::AArch64::STXPX, llvm::AArch64::STLXPX>::unique(),
            Not::unique(OperandIs::unique(0, RegLLVM(llvm::AArch64::WZR))))),
        conv_unique<PatchGenerator>(
            CondExclusifLoad::unique(Temp(0), Temp(1)),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            WriteOperand::unique(Operand(0), Shadow(MEM_EXCLUSIVE_STATUS_TAG)),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTemp::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            SaveX28IfSet::unique()));
  } else {

    /* Rule #19: exclusive store register(s)
     * status register is XZR
     */
    rules.emplace_back(
        And::unique(conv_unique<PatchCondition>(
            OpIsIn<llvm::AArch64::STXRB, llvm::AArch64::STLXRB,
                   llvm::AArch64::STXRH, llvm::AArch64::STLXRH,
                   llvm::AArch64::STXRW, llvm::AArch64::STLXRW,
                   llvm::AArch64::STXRX, llvm::AArch64::STLXRX,
                   llvm::AArch64::STXPW, llvm::AArch64::STLXPW,
                   llvm::AArch64::STXPX, llvm::AArch64::STLXPX>::unique(),
            OperandIs::unique(0, RegLLVM(llvm::AArch64::WZR)))),
        conv_unique<PatchGenerator>(
            ModifyInstruction::unique(conv_unique<InstTransform>(
                SetOperandW::unique(Operand(0), Temp(0)))),
            WriteTemp::unique(Temp(0), Shadow(MEM_EXCLUSIVE_STATUS_TAG)),
            SaveX28IfSet::unique()));

    /* Rule #20: exclusive store register(s)
     * status register isn't XZR
     */
    rules.emplace_back(
        And::unique(conv_unique<PatchCondition>(
            OpIsIn<llvm::AArch64::STXRB, llvm::AArch64::STLXRB,
                   llvm::AArch64::STXRH, llvm::AArch64::STLXRH,
                   llvm::AArch64::STXRW, llvm::AArch64::STLXRW,
                   llvm::AArch64::STXRX, llvm::AArch64::STLXRX,
                   llvm::AArch64::STXPW, llvm::AArch64::STLXPW,
                   llvm::AArch64::STXPX, llvm::AArch64::STLXPX>::unique(),
            Not::unique(OperandIs::unique(0, RegLLVM(llvm::AArch64::WZR))))),
        conv_unique<PatchGenerator>(
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            WriteOperand::unique(Operand(0), Shadow(MEM_EXCLUSIVE_STATUS_TAG)),
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

bool isSupportedInstruction(const Patch &patch, const char *&reason) {
  [[maybe_unused]] const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;
  switch (inst.getOpcode()) {
    case llvm::AArch64::ERET:
    case llvm::AArch64::ERETAA:
    case llvm::AArch64::ERETAB:
      reason =
          "ERET is a privileged instruction (EL1+ only), not "
          "supported by QBDI";
      return false;
    case llvm::AArch64::DRPS:
      reason = "DRPS is only valid in Debug state, not supported by QBDI";
      return false;
    default:
      return true;
  }
}

void handlePreviousPACM(Patch &pacmPatch, unsigned opcode, Options opts) {
  switch (opcode) {
    case llvm::AArch64::PACIASP:
    case llvm::AArch64::PACIBSP:
    case llvm::AArch64::PACIA1716:
    case llvm::AArch64::PACIB1716:
      return;
    case llvm::AArch64::AUTIASP:
    case llvm::AArch64::AUTIBSP:
    case llvm::AArch64::AUTIA1716:
    case llvm::AArch64::AUTIB1716:
    case llvm::AArch64::RETAA:
    case llvm::AArch64::RETAB:
      if ((opts & Options::OPT_BYPASS_PAUTH) == 0) {
        return;
      }
      break;
    default: {
      const LLVMCPU &llvmcpu = *pacmPatch.llvmcpu;
      QBDI_WARN(
          "Suspicious usage of PACM at 0x{:x}: not followed by an "
          "instruction whose behavior PACM may affect (found {})",
          pacmPatch.metadata.address, llvmcpu.getInstOpcodeName(opcode));
      break;
    }
  }

  const LLVMCPU &llvmcpu = *pacmPatch.llvmcpu;
  static const PatchRule safePACMRule(
      OpIs::unique(llvm::AArch64::PACM),
      conv_unique<PatchGenerator>(
          ModifyInstruction::unique(InstTransform::UniquePtrVec())));

  Patch safePatch(pacmPatch.metadata.inst, pacmPatch.metadata.address,
                  pacmPatch.metadata.instSize, llvmcpu);
  safePACMRule.apply(safePatch, llvmcpu);
  pacmPatch = std::move(safePatch);
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

PatchRuleResult PatchRuleAssembly::generate(const llvm::MCInst &inst,
                                            rword address, uint32_t instSize,
                                            const LLVMCPU &llvmcpu,
                                            std::vector<Patch> &patchList,
                                            const char *&unsupportedReason) {

  Patch instPatch{inst, address, instSize, llvmcpu};

  if (not isSupportedInstruction(instPatch, unsupportedReason)) {
    return PatchRuleResult::UNSUPPORTED;
  }

  if (not patchList.empty() &&
      patchList.back().metadata.inst.getOpcode() == llvm::AArch64::PACM) {
    handlePreviousPACM(patchList.back(), inst.getOpcode(), options);
  }

  for (uint32_t j = 0; j < patchRules.size(); j++) {
    if (patchRules[j].canBeApplied(instPatch, llvmcpu)) {
      QBDI_DEBUG("Patch rule {} applied", j);

      patchRules[j].apply(instPatch, llvmcpu);
      patchList.push_back(std::move(instPatch));
      Patch &patch = patchList.back();

      if (patch.metadata.modifyPC) {
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
  if (not patchList.empty() &&
      patchList.back().metadata.inst.getOpcode() == llvm::AArch64::PACM) {
    patchList.pop_back();
  }
  return true;
}

} // namespace QBDI
