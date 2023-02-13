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
#include "ARMInstrInfo.h"

#include "Engine/LLVMCPU.h"
#include "Patch/ARM/InstInfo_ARM.h"
#include "Patch/ARM/Layer2_ARM.h"
#include "Patch/ARM/PatchCondition_ARM.h"
#include "Patch/ARM/PatchGenerator_ARM.h"
#include "Patch/ExecBlockFlags.h"
#include "Patch/InstTransform.h"
#include "Patch/PatchRule.h"
#include "Patch/PatchRuleAssembly.h"

#include "Utility/LogSys.h"
#include "Utility/System.h"

#include "QBDI/Options.h"

namespace QBDI {

namespace {

std::vector<PatchRule> getARMPatchRules(Options opts) {
  std::vector<PatchRule> rules;

  // instruction where PC can be the first (dest) operand only
  const Or PCInst1OpDest(conv_unique<PatchCondition>(
      OpIs::unique(llvm::ARM::LDR_PRE_IMM),
      OpIs::unique(llvm::ARM::LDR_POST_IMM), OpIs::unique(llvm::ARM::MOVi),
      OpIs::unique(llvm::ARM::MOVi16), OpIs::unique(llvm::ARM::MVNi)));

  // instruction where PC can be the first (src) operand
  const Or PCInst1OpSrc(conv_unique<PatchCondition>(
      OpIs::unique(llvm::ARM::CMNri), OpIs::unique(llvm::ARM::CMPri),
      OpIs::unique(llvm::ARM::PLDWi12), OpIs::unique(llvm::ARM::PLDWrs),
      OpIs::unique(llvm::ARM::PLDi12), OpIs::unique(llvm::ARM::PLDrs),
      OpIs::unique(llvm::ARM::TEQri), OpIs::unique(llvm::ARM::TSTri),
      OpIs::unique(llvm::ARM::VLDMDIA), OpIs::unique(llvm::ARM::VLDMSIA),
      OpIs::unique(llvm::ARM::FLDMXIA), OpIs::unique(llvm::ARM::VSTMDIA),
      OpIs::unique(llvm::ARM::VSTMSIA), OpIs::unique(llvm::ARM::FSTMXIA)));

  // instruction where PC can be the second (src) operand
  const Or PCInst1OpSrcOff1(conv_unique<PatchCondition>(
      OpIs::unique(llvm::ARM::LDRBi12), OpIs::unique(llvm::ARM::LDRBrs),
      OpIs::unique(llvm::ARM::LDRH), OpIs::unique(llvm::ARM::LDRSB),
      OpIs::unique(llvm::ARM::LDRSH), OpIs::unique(llvm::ARM::STRBi12),
      OpIs::unique(llvm::ARM::STRBrs), OpIs::unique(llvm::ARM::STRH),
      OpIs::unique(llvm::ARM::STRT_POST_IMM), OpIs::unique(llvm::ARM::VLDRD),
      OpIs::unique(llvm::ARM::VLDRH), OpIs::unique(llvm::ARM::VLDRS),
      OpIs::unique(llvm::ARM::VSTRD), OpIs::unique(llvm::ARM::VSTRH),
      OpIs::unique(llvm::ARM::VSTRS)));

  // instruction where PC can be the third (src) operand
  const Or PCInst1OpSrcOff2(conv_unique<PatchCondition>(
      OpIs::unique(llvm::ARM::LDRD), OpIs::unique(llvm::ARM::STC_OFFSET),
      OpIs::unique(llvm::ARM::STRD), OpIs::unique(llvm::ARM::LDC2L_OFFSET),
      OpIs::unique(llvm::ARM::LDC2L_OPTION),
      OpIs::unique(llvm::ARM::LDC2L_POST), OpIs::unique(llvm::ARM::LDC2L_PRE),
      OpIs::unique(llvm::ARM::LDC2_OFFSET),
      OpIs::unique(llvm::ARM::LDC2_OPTION), OpIs::unique(llvm::ARM::LDC2_POST),
      OpIs::unique(llvm::ARM::LDC2_PRE), OpIs::unique(llvm::ARM::LDCL_OFFSET),
      OpIs::unique(llvm::ARM::LDCL_OPTION), OpIs::unique(llvm::ARM::LDCL_POST),
      OpIs::unique(llvm::ARM::LDCL_PRE), OpIs::unique(llvm::ARM::LDC_OFFSET),
      OpIs::unique(llvm::ARM::LDC_OPTION), OpIs::unique(llvm::ARM::LDC_POST),
      OpIs::unique(llvm::ARM::LDC_PRE)));

  // instruction where PC can be the first/second (src) operands
  const Or PCInst2OpSrc(conv_unique<PatchCondition>(
      OpIs::unique(llvm::ARM::CMNzrr), OpIs::unique(llvm::ARM::CMNzrsi),
      OpIs::unique(llvm::ARM::CMPrr), OpIs::unique(llvm::ARM::STRi12),
      OpIs::unique(llvm::ARM::STRrs), OpIs::unique(llvm::ARM::TEQrr),
      OpIs::unique(llvm::ARM::TSTrr)));

  // instruction where PC can be the first (dest) or/and the second (src)
  // operands only
  const Or PCInst2Op(conv_unique<PatchCondition>(
      OpIs::unique(llvm::ARM::ADCri), OpIs::unique(llvm::ARM::ADDri),
      OpIs::unique(llvm::ARM::ANDri), OpIs::unique(llvm::ARM::BICri),
      OpIs::unique(llvm::ARM::EORri), OpIs::unique(llvm::ARM::LDRi12),
      OpIs::unique(llvm::ARM::LDRrs), OpIs::unique(llvm::ARM::MOVr),
      OpIs::unique(llvm::ARM::MOVsi), OpIs::unique(llvm::ARM::MVNr),
      OpIs::unique(llvm::ARM::ORRri), OpIs::unique(llvm::ARM::RSBri),
      OpIs::unique(llvm::ARM::RSCri), OpIs::unique(llvm::ARM::SBCri),
      OpIs::unique(llvm::ARM::SUBri)));

  // instruction where PC can be the first (dest) or/and the second/third (src)
  // operands only
  const Or PCInst3Op(conv_unique<PatchCondition>(
      OpIs::unique(llvm::ARM::ADCrr), OpIs::unique(llvm::ARM::ADCrsi),
      OpIs::unique(llvm::ARM::ADDrr), OpIs::unique(llvm::ARM::ADDrsi),
      OpIs::unique(llvm::ARM::ANDrr), OpIs::unique(llvm::ARM::ANDrsi),
      OpIs::unique(llvm::ARM::BICrr), OpIs::unique(llvm::ARM::BICrsi),
      OpIs::unique(llvm::ARM::EORrr), OpIs::unique(llvm::ARM::EORrsi),
      OpIs::unique(llvm::ARM::ORRrr), OpIs::unique(llvm::ARM::ORRrsi),
      OpIs::unique(llvm::ARM::RSBrr), OpIs::unique(llvm::ARM::RSBrsi),
      OpIs::unique(llvm::ARM::RSCrr), OpIs::unique(llvm::ARM::RSCrsi),
      OpIs::unique(llvm::ARM::SBCrr), OpIs::unique(llvm::ARM::SBCrsi),
      OpIs::unique(llvm::ARM::SUBrr), OpIs::unique(llvm::ARM::SUBrsi)));

  // ARM instruction
  // ===============

  // Warning: All instructions can be conditionnal, Each patchRule must keep the
  // conditionnal behavior

  /* Rule #0: BX lr | mov pc, lr without condition
   *
   *  str lr, <offset PC>
   *  setExchange
   */
  rules.emplace_back(And::unique(conv_unique<PatchCondition>(
                         Or::unique(conv_unique<PatchCondition>(
                             OpIs::unique(llvm::ARM::BX_RET),
                             OpIs::unique(llvm::ARM::MOVPCLR))),
                         Not::unique(HasCond::unique()))),
                     conv_unique<PatchGenerator>(
                         SaveReg::unique(Reg(REG_LR), Offset(Reg(REG_PC))),
                         SetExchange::unique(Temp(0))));

  /* Rule #1: BX lr | mov pc, lr with condition
   *
   *  mov temp0, <PC-4>
   *  movcc temp0, lr
   *  str temp0, <offset PC>
   *  setExchange
   */
  rules.emplace_back(
      And::unique(
          conv_unique<PatchCondition>(Or::unique(conv_unique<PatchCondition>(
                                          OpIs::unique(llvm::ARM::BX_RET),
                                          OpIs::unique(llvm::ARM::MOVPCLR))),
                                      HasCond::unique())),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          CopyRegCC::unique(Temp(0), Reg(REG_LR)), WritePC::unique(Temp(0)),
          SetExchange::unique(Temp(0))));

  /* Rule #2: BX pc without condition
   *
   *  mov temp0, <PC>
   *  str temp0, <offset PC>
   *  // setExchange not needed as PC will be aligned
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          Or::unique(conv_unique<PatchCondition>(
              OpIs::unique(llvm::ARM::BX), OpIs::unique(llvm::ARM::BX_pred))),
          OperandIs::unique(0, Reg(REG_PC)), Not::unique(HasCond::unique()))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ false),
          WritePC::unique(Temp(0))));

  /* Rule #3: BX pc with condition
   *
   *  mov temp0, <PC-4>
   *  movcc temp0, <PC>
   *  str temp0, <offset PC>
   *  // setExchange not needed as PC will be aligned
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          Or::unique(conv_unique<PatchCondition>(
              OpIs::unique(llvm::ARM::BX), OpIs::unique(llvm::ARM::BX_pred))),
          OperandIs::unique(0, Reg(REG_PC)), HasCond::unique())),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ true),
          WritePC::unique(Temp(0))));

  /* Rule #4: BX reg without condition
   *
   *  mov temp0, reg
   *  str temp0, <offset PC>
   *  setExchange
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          Or::unique(conv_unique<PatchCondition>(
              OpIs::unique(llvm::ARM::BX), OpIs::unique(llvm::ARM::BX_pred))),
          Not::unique(HasCond::unique()))),
      conv_unique<PatchGenerator>(GetOperand::unique(Temp(0), Operand(0)),
                                  WritePC::unique(Temp(0)),
                                  SetExchange::unique(Temp(0))));

  /* Rule #5: BX reg with condition
   *
   *  mov temp0, <PC-4>
   *  movcc temp0, reg
   *  str temp0, <offset PC>
   *  setExchange
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          Or::unique(conv_unique<PatchCondition>(
              OpIs::unique(llvm::ARM::BX), OpIs::unique(llvm::ARM::BX_pred))),
          HasCond::unique())),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          GetOperandCC::unique(Temp(0), Operand(0)), WritePC::unique(Temp(0)),
          SetExchange::unique(Temp(0))));

  /* Rule #6: BLX reg
   *
   *  mov temp0, <PC-4>
   *  movcc temp0, reg
   *  str temp0, <offset PC>
   *  movcc lr, <PC-4>
   *  setExchange
   *
   * note: BLX LR is possible
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::ARM::BLX), OpIs::unique(llvm::ARM::BLX_pred))),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          GetOperandCC::unique(Temp(0), Operand(0)), WritePC::unique(Temp(0)),
          SetExchange::unique(Temp(0)),
          GetNextInstAddr::unique(Reg(REG_LR), /* keepCond */ true)));

  /* Rule #7: BLX imm
   *
   * never cond
   *
   *  mov temp0, <PC-4>
   *  movcc lr, temp0
   *  movcc temp0, <PC+Operand(0)>
   *  str temp0, <offset PC>
   */
  rules.emplace_back(
      OpIs::unique(llvm::ARM::BLXi),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Reg(REG_LR), /* keepCond */ false),
          GetPCOffset::unique(Temp(0), Operand(0), /* keepCond */ false),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #8: BL imm
   *
   *  mov temp0, <PC-4>
   *  movcc lr, temp0
   *  movcc temp0, <PC+Operand(0)>
   *  str temp0, <offset PC>
   *
   *    no SetExchange, as BL doesn't perform the change of mode
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::ARM::BL),
                                             OpIs::unique(llvm::ARM::BL_pred))),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          CopyTempCC::unique(Reg(REG_LR), Temp(0)),
          GetPCOffset::unique(Temp(0), Operand(0), /* keepCond */ true),
          WritePC::unique(Temp(0))));

  /* Rule #9: Bcc with cond
   * - bcc #imm
   *
   *  mov temp0, <PC-4>
   *  movcc temp0, <PC + imm>
   *  str temp0, <offset PC>
   *
   *    no SetExchange, as Bcc doesn't perform the change of mode
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::ARM::Bcc),
                                              HasCond::unique())),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          GetPCOffset::unique(Temp(0), Operand(0), /* keepCond */ true),
          WritePC::unique(Temp(0))));

  /* Rule #10: Bcc without cond
   * - b #imm
   *
   *  mov temp0, <PC + imm>
   *  str temp0, <offset PC>
   *
   *    no SetExchange, as Bcc doesn't perform the change of mode
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::ARM::Bcc),
                                              Not::unique(HasCond::unique()))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(0), /* keepCond */ false),
          WritePC::unique(Temp(0))));

  // local monitor
  // =============

  if ((opts & Options::OPT_DISABLE_LOCAL_MONITOR) == 0) {

    /* Rule #11: Clear local monitor state
     */
    rules.emplace_back(
        Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::ARM::CLREX),
                                               OpIs::unique(llvm::ARM::SVC))),
        conv_unique<PatchGenerator>(
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            // for SVC, we need to backup the value of Temp(0) after the syscall
            SaveTemp::unique(Temp(0)),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable)))));

    /* Rule #12: exclusive load 1 register
     */
    rules.emplace_back(
        Or::unique(conv_unique<PatchCondition>(
            OpIs::unique(llvm::ARM::LDREX), OpIs::unique(llvm::ARM::LDREXB),
            OpIs::unique(llvm::ARM::LDREXD), OpIs::unique(llvm::ARM::LDREXH))),
        conv_unique<PatchGenerator>(
            GetConstantMap::unique(Temp(0),
                                   std::map<unsigned, Constant>({
                                       {llvm::ARM::LDREXB, Constant(1)},
                                       {llvm::ARM::LDREXH, Constant(2)},
                                       {llvm::ARM::LDREX, Constant(4)},
                                       {llvm::ARM::LDREXD, Constant(8)},
                                   })),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            WriteOperandCC::unique(
                Operand(1),
                Offset(offsetof(Context, gprState.localMonitor.addr))),
            ModifyInstruction::unique(InstTransform::UniquePtrVec())));

    /* Rule #13: exclusive store
     */
    rules.emplace_back(
        Or::unique(conv_unique<PatchCondition>(
            OpIs::unique(llvm::ARM::STREX), OpIs::unique(llvm::ARM::STREXB),
            OpIs::unique(llvm::ARM::STREXD), OpIs::unique(llvm::ARM::STREXH))),
        conv_unique<PatchGenerator>(
            CondExclusifLoad::unique(Temp(0), Temp(1)),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable)))));
  }

  // Instruction without PC
  // ======================

  /* Rule #14: instruction to skip (barrier, preload)
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::ARM::PLDWi12), OpIs::unique(llvm::ARM::PLDWrs),
          OpIs::unique(llvm::ARM::PLDi12), OpIs::unique(llvm::ARM::PLDrs),
          OpIs::unique(llvm::ARM::PLIi12), OpIs::unique(llvm::ARM::PLIrs))),
      PatchGenerator::UniquePtrVec());

  /* Rule #15: all other instruction without PC
   *
   * Note: This patch should be at the end of the list. However, as many
   * instruction doesn't used PC, we place it here to apply it early on
   * intructions without PC.
   */
  rules.emplace_back(
      Not::unique(Or::unique(conv_unique<PatchCondition>(
          UseReg::unique(Reg(REG_PC)),
          // ADR is decoded as ADDri
          OpIs::unique(llvm::ARM::ADR),
          // unsupported instruction
          OpIs::unique(llvm::ARM::SETEND), OpIs::unique(llvm::ARM::BXJ)))),
      conv_unique<PatchGenerator>(
          ModifyInstruction::unique(InstTransform::UniquePtrVec())));

  // Instruction with PC : special case LDM / STM
  // ============================================

  /* Rule #16: LDM with PC
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          UseReg::unique(Reg(REG_PC)),
          Or::unique(conv_unique<PatchCondition>(
              OpIs::unique(llvm::ARM::LDMIA), OpIs::unique(llvm::ARM::LDMIB),
              OpIs::unique(llvm::ARM::LDMDA), OpIs::unique(llvm::ARM::LDMDB),
              OpIs::unique(llvm::ARM::LDMIA_UPD),
              OpIs::unique(llvm::ARM::LDMIB_UPD),
              OpIs::unique(llvm::ARM::LDMDA_UPD),
              OpIs::unique(llvm::ARM::LDMDB_UPD))))),
      conv_unique<PatchGenerator>(LDMPatchGen::unique(Temp(0)),
                                  SetExchange::unique(Temp(0))));

  /* Rule #17: STM with PC
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          UseReg::unique(Reg(REG_PC)),
          Or::unique(conv_unique<PatchCondition>(
              OpIs::unique(llvm::ARM::STMIA), OpIs::unique(llvm::ARM::STMIB),
              OpIs::unique(llvm::ARM::STMDA), OpIs::unique(llvm::ARM::STMDB),
              OpIs::unique(llvm::ARM::STMIA_UPD),
              OpIs::unique(llvm::ARM::STMIB_UPD),
              OpIs::unique(llvm::ARM::STMDA_UPD),
              OpIs::unique(llvm::ARM::STMDB_UPD))))),
      conv_unique<PatchGenerator>(STMPatchGen::unique(Temp(0))));

  // Instruction with PC as source
  // =============================

  /* Rule #18: ADD/SUB/... with PC as source only:
   * - both in second and third operand (src only)
   * - both in first and third operand (src only)
   * - in first (src) operand
   * - in second (src) operand
   * - in third (src) operand
   *
   *  mov temp0, <PC>
   *  add r12, temp0, temp0
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(
          And::unique(conv_unique<PatchCondition>(
              Not::unique(OperandIs::unique(1, Reg(REG_PC))),
              OperandIs::unique(1, Reg(REG_PC)),
              OperandIs::unique(2, Reg(REG_PC)), PCInst3Op.clone())),
          And::unique(conv_unique<PatchCondition>(
              OperandIs::unique(0, Reg(REG_PC)),
              OperandIs::unique(1, Reg(REG_PC)),
              Not::unique(OperandIs::unique(2, Reg(REG_PC))),
              PCInst2OpSrc.clone())),
          And::unique(conv_unique<PatchCondition>(
              OperandIs::unique(0, Reg(REG_PC)),
              Not::unique(OperandIs::unique(1, Reg(REG_PC))),
              Not::unique(OperandIs::unique(2, Reg(REG_PC))),
              Or::unique(conv_unique<PatchCondition>(PCInst2OpSrc.clone(),
                                                     PCInst1OpSrc.clone())))),
          And::unique(conv_unique<PatchCondition>(
              Not::unique(OperandIs::unique(0, Reg(REG_PC))),
              OperandIs::unique(1, Reg(REG_PC)),
              Not::unique(OperandIs::unique(2, Reg(REG_PC))),
              Or::unique(conv_unique<PatchCondition>(
                  PCInst1OpSrcOff1.clone(), PCInst2OpSrc.clone(),
                  PCInst2Op.clone(), PCInst3Op.clone())))),
          And::unique(conv_unique<PatchCondition>(
              Not::unique(OperandIs::unique(0, Reg(REG_PC))),
              Not::unique(OperandIs::unique(1, Reg(REG_PC))),
              OperandIs::unique(2, Reg(REG_PC)),
              Or::unique(conv_unique<PatchCondition>(PCInst1OpSrcOff2.clone(),
                                                     PCInst3Op.clone())))))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0))))));

  // Instruction with PC 3 times and dest
  // ====================================

  /* Rule #19: LDR/ADD/SUB/... with PC in first (dst) and second/third (src)
   * operand and without cond
   *  - add pc, pc, pc
   *
   *  mov temp0, <PC>
   *  add temp0, temp0, temp0
   *  str temp0, <offset PC>
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OperandIs::unique(0, Reg(REG_PC)), OperandIs::unique(1, Reg(REG_PC)),
          OperandIs::unique(2, Reg(REG_PC)), PCInst3Op.clone(),
          Not::unique(HasCond::unique()))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #20: LDR/ADD/SUB/... with PC in first (dst) and second/third (src)
   * operand and with cond
   *  - addcc pc, pc, pc
   *
   *  mov temp0, <PC-4>
   *  mov temp1, <PC>
   *  addcc temp0, temp1, temp1
   *  str temp0, <offset PC>
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OperandIs::unique(0, Reg(REG_PC)), OperandIs::unique(1, Reg(REG_PC)),
          OperandIs::unique(2, Reg(REG_PC)), PCInst3Op.clone(),
          HasCond::unique())),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          GetPCOffset::unique(Temp(1), Constant(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(0), Temp(0)),
              SetOperand::unique(Operand(1), Temp(1)),
              SetOperand::unique(Operand(2), Temp(1)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  // Instruction with PC 2 times and dest
  // ====================================

  /* Rule #21: ADD/SUB/... with PC in first (dst) and third (src) operand and
   * without cond
   *  - add pc, r12, pc
   *
   *  mov temp0, <PC>
   *  add temp0, r12, temp0
   *  str temp0, <offset PC>
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OperandIs::unique(0, Reg(REG_PC)),
          Not::unique(OperandIs::unique(1, Reg(REG_PC))),
          OperandIs::unique(2, Reg(REG_PC)), PCInst3Op.clone(),
          Not::unique(HasCond::unique()))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #22: ADD/SUB/... with PC in first (dst) and third (src) operand and
   * with cond
   *  - addcc pc, r12, pc
   *
   *  mov temp0, <PC-4>
   *  mov temp1, <PC>
   *  addcc temp0, r12, temp1
   *  str temp0, <offset PC>
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OperandIs::unique(0, Reg(REG_PC)),
          Not::unique(OperandIs::unique(1, Reg(REG_PC))),
          OperandIs::unique(2, Reg(REG_PC)), PCInst3Op.clone(),
          HasCond::unique())),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          GetPCOffset::unique(Temp(1), Constant(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(0), Temp(0)),
              SetOperand::unique(Operand(2), Temp(1)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #23: LDR/ADD/SUB/... with PC in first (dst) and second (src) operand
   * and without cond
   *  - ldr pc, [pc, #0x80]
   *
   *  mov temp0, <PC>
   *  ldr temp0, [temp0, #0x80]
   *  str temp0, <offset PC>
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OperandIs::unique(0, Reg(REG_PC)), OperandIs::unique(1, Reg(REG_PC)),
          Not::unique(OperandIs::unique(2, Reg(REG_PC))),
          Or::unique(conv_unique<PatchCondition>(PCInst3Op.clone(),
                                                 PCInst2Op.clone())),
          Not::unique(HasCond::unique()))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #24: LDR/ADD/SUB/... with PC in first (dst) and second (src) operand
   * and with cond
   *  - ldrcc pc, [pc, #0x80]
   *
   *  mov temp0, <PC-4>
   *  mov temp1, <PC>
   *  ldrcc temp0, [temp1, #0x80]
   *  str temp0, <offset PC>
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OperandIs::unique(0, Reg(REG_PC)), OperandIs::unique(1, Reg(REG_PC)),
          Not::unique(OperandIs::unique(2, Reg(REG_PC))),
          Or::unique(conv_unique<PatchCondition>(PCInst3Op.clone(),
                                                 PCInst2Op.clone())),
          HasCond::unique())),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          GetPCOffset::unique(Temp(1), Constant(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(0), Temp(0)),
              SetOperand::unique(Operand(1), Temp(1)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  // Instruction with PC 1 times and dst
  // ===================================

  /* Rule #25: LDR/ADD/SUB/... with PC in first (dst) operand and without cond
   *  - ldr pc, [r0, #0x80]
   *
   *  ldr temp0, [r0, #0x80]
   *  str temp0, <offset PC>
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OperandIs::unique(0, Reg(REG_PC)),
          Not::unique(OperandIs::unique(1, Reg(REG_PC))),
          Not::unique(OperandIs::unique(2, Reg(REG_PC))),
          Not::unique(HasCond::unique()),
          Or::unique(conv_unique<PatchCondition>(
              PCInst1OpDest.clone(), PCInst2Op.clone(), PCInst3Op.clone())))),
      conv_unique<PatchGenerator>(
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #26: LDR/ADD/SUB/... with PC in first (dst) operand and with cond
   *  - ldrcc pc, [r0, #0x80]
   *
   *  mov temp0, <PC-4>
   *  ldrcc temp0, [r0, #0x80]
   *  str temp0, <offset PC>
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OperandIs::unique(0, Reg(REG_PC)),
          Not::unique(OperandIs::unique(1, Reg(REG_PC))),
          Not::unique(OperandIs::unique(2, Reg(REG_PC))), HasCond::unique(),
          Or::unique(conv_unique<PatchCondition>(
              PCInst1OpDest.clone(), PCInst2Op.clone(), PCInst3Op.clone())),
          HasCond::unique())),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  return rules;
}

std::vector<PatchRule> getThumbPatchRules(Options opts) {
  std::vector<PatchRule> rules;

  // Warning about Thumb Patch
  // -> an ITBlock can have the condition AL:
  //    -> we should keep the ITBlock of the instruction to keep the same flags
  //       behavior. We can only remove the ITBlock with AL condition on
  //       instruction that have the same semantique outside or inside the AL
  //       ITBlock
  //    -> For any other instruction, we cannot inverse the condition
  //       (as `inv(AL)` is invalid).
  // -> Be carefull about PC behavior:
  //    -> when PC is used as a source, some instruction used `Align(PC, 4)`
  //       instead of PC
  //    -> In thumb, PC is always `instAddress + 4`, regardless of the size
  //       of the instruction
  //    -> When the instruction sets PC, be carefull if the instruction can
  //       change the CPU mode. QBDI has two mechanism that must be used
  //       together:
  //       -> When the instruction can change the mode, SetExchange must be set.
  //          The new mode will be the LSB of PC.
  //       -> otherwise, the LSB bit **must** be set to 1. If the user returns
  //          BREAK_TO_VM in a callback after the instruction, we want the LSB
  //          to be correctly set.
  // -> Some instruction inside an ITBlock can change the flags (CMP, CMN, TST,
  //    ...):
  //    -> The new flags is used for the next instruction on the same ITBlock.
  //       We can therefore safely split an ITBlock.
  //    -> Be carefull when patch an instruction inside an ITBlock. The flags
  //       may be change after the `ModifyInstruction`.
  //    -> note that no instruction seems be able to set PC and the flags at the
  //       same time. All instructions with ALUWritePC doesn't set the flags if
  //       PC is the destination register.

  // instruction where PC can be the first (dest) operand only
  const Or PCInst1OpDest(conv_unique<PatchCondition>(
      OpIs::unique(llvm::ARM::t2LDRi8), OpIs::unique(llvm::ARM::t2LDR_PRE),
      OpIs::unique(llvm::ARM::t2LDR_POST), OpIs::unique(llvm::ARM::t2LDRi12),
      OpIs::unique(llvm::ARM::t2LDRs)));

  // instruction where PC can be the second (src) operand
  const Or PCInst1OpSrcOff1(conv_unique<PatchCondition>(
      OpIs::unique(llvm::ARM::VLDRD), OpIs::unique(llvm::ARM::VLDRH),
      OpIs::unique(llvm::ARM::VLDRS)));

  // instruction where PC can be the third (src) operand
  const Or PCInst1OpSrcOff2(conv_unique<PatchCondition>(
      OpIs::unique(llvm::ARM::t2LDC2L_OFFSET),
      OpIs::unique(llvm::ARM::t2LDC2_OFFSET),
      OpIs::unique(llvm::ARM::t2LDCL_OFFSET),
      OpIs::unique(llvm::ARM::t2LDC_OFFSET), OpIs::unique(llvm::ARM::t2LDRDi8),
      OpIs::unique(llvm::ARM::tADDspr)));

  // instruction where PC can be the first (dest) or/and the second (src)
  // operands only
  const OpIs PCInst2Op(llvm::ARM::tMOVr);

  // instruction where PC can be the first (dest) AND the third (src) operands
  // the first and third operand is always the same
  const OpIs PCInst1Dst3Src(llvm::ARM::tADDrSP);

  // instruction where PC can be the first|second (dest/src) OR the third (src)
  // operands
  // the first and second operand is always the same
  const OpIs PCInst1Dst2SrcOR3Src(llvm::ARM::tADDhirr);

  // Thumb instruction
  // =================

  /* Rule #0: Adr <rx>, <imm>
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::ARM::tADR),
                                             OpIs::unique(llvm::ARM::t2ADR))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Operand(0), Operand(1), /* keepCond */ true)));

  /* Rule #1: Bcc <imm>
   *
   *    no SetExchange, as Bcc doesn't perform the change of mode
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(
          And::unique(conv_unique<PatchCondition>(
              Or::unique(
                  conv_unique<PatchCondition>(OpIs::unique(llvm::ARM::tBcc),
                                              OpIs::unique(llvm::ARM::t2Bcc))),
              Not::unique(InITBlock::unique()))),
          And::unique(conv_unique<PatchCondition>(
              Or::unique(conv_unique<PatchCondition>(
                  OpIs::unique(llvm::ARM::tB), OpIs::unique(llvm::ARM::t2B))),
              LastInITBlock::unique())))),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          GetPCOffset::unique(Temp(0), Operand(0), /* keepCond */ true),
          WritePC::unique(Temp(0))));

  /* Rule #2: B <imm>
   *
   *    no SetExchange, as B doesn't perform the change of mode
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::ARM::tB),
                                                 OpIs::unique(llvm::ARM::t2B))),
          Not::unique(InITBlock::unique()))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(0), /* keepCond */ false),
          WritePC::unique(Temp(0))));

  /* Rule #3: BL <imm>
   *
   *    no SetExchange, as BL doesn't perform the change of mode
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::ARM::tBL),
          Or::unique(conv_unique<PatchCondition>(
              Not::unique(InITBlock::unique()), LastInITBlock::unique())))),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          CopyTempCC::unique(Reg(REG_LR), Temp(0)),
          GetPCOffset::unique(Temp(0), Operand(2), /* keepCond */ true),
          WritePC::unique(Temp(0))));

  /* Rule #4: BLX <imm>
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::ARM::tBLXi),
          Or::unique(conv_unique<PatchCondition>(
              Not::unique(InITBlock::unique()), LastInITBlock::unique())))),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          CopyTempCC::unique(Reg(REG_LR), Temp(0)),
          GetPCOffset::unique(Temp(0), Operand(2), /* keepCond */ true),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #5: BLX <reg>
   *
   * note: BLX LR is possible
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::ARM::tBLXr),
          Or::unique(conv_unique<PatchCondition>(
              Not::unique(InITBlock::unique()), LastInITBlock::unique())))),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          GetOperandCC::unique(Temp(0), Operand(2)), WritePC::unique(Temp(0)),
          SetExchange::unique(Temp(0)),
          GetNextInstAddr::unique(Reg(REG_LR), /* keepCond */ true)));

  /* Rule #6: BXcc pc
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::ARM::tBX),
                                              OperandIs::unique(0, Reg(REG_PC)),
                                              LastInITBlock::unique())),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ true),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #7: BX pc
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::ARM::tBX), OperandIs::unique(0, Reg(REG_PC)),
          Not::unique(InITBlock::unique()))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ false),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #8: BXcc <reg>
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::ARM::tBX),
                                              LastInITBlock::unique())),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          GetOperandCC::unique(Temp(0), Operand(0)), WritePC::unique(Temp(0)),
          SetExchange::unique(Temp(0))));

  /* Rule #9: BX <reg>
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::ARM::tBX), Not::unique(InITBlock::unique()))),
      conv_unique<PatchGenerator>(GetOperand::unique(Temp(0), Operand(0)),
                                  WritePC::unique(Temp(0)),
                                  SetExchange::unique(Temp(0))));

  /* Rule #10: bxaut <reg>
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::ARM::t2BXAUT),
                                              LastInITBlock::unique())),
      conv_unique<PatchGenerator>(
          T2BXAUTPatchGen::unique(), GetOperand::unique(Temp(0), Operand(2)),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #11: bxaut <reg>
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::ARM::t2BXAUT), Not::unique(InITBlock::unique()))),
      conv_unique<PatchGenerator>(
          T2BXAUTPatchGen::unique(),
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          GetOperandCC::unique(Temp(0), Operand(2)), WritePC::unique(Temp(0)),
          SetExchange::unique(Temp(0))));

  /* Rule #12: CBZ|CBNZ <reg>, imm
   *
   *    no SetExchange, as CBZ|CBNZ doesn't perform the change of mode
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          Or::unique(conv_unique<PatchCondition>(
              OpIs::unique(llvm::ARM::tCBNZ), OpIs::unique(llvm::ARM::tCBZ))),
          Not::unique(InITBlock::unique()))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(1), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(1), Constant(2)))),
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          WritePC::unique(Temp(0))));

  /* Rule #13: TBB|TBH [<reg>, <reg>{, LSL #1}]
   *
   *    no SetExchange, as TBB|TBH doesn't perform the change of mode
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          Or::unique(conv_unique<PatchCondition>(
              OpIs::unique(llvm::ARM::t2TBB), OpIs::unique(llvm::ARM::t2TBH))),
          Or::unique(conv_unique<PatchCondition>(
              Not::unique(InITBlock::unique()), LastInITBlock::unique())))),
      conv_unique<PatchGenerator>(T2TBBTBHPatchGen::unique(Temp(0), Temp(1))));

  /* Rule #14: LDR <reg>, [pc, #<imm>]
   */
  rules.emplace_back(
      OpIs::unique(llvm::ARM::tLDRpci),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ false),
          ItPatch::unique(false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOpcode::unique(llvm::ARM::t2LDRi12),
              AddOperand::unique(Operand(1), Temp(0))))));

  /* Rule #15: LDR.w pc, [pc, #<imm>]
   * not in ITBlock
   *
   * need specific rules, because the immediate size is 12 bits when PC, but
   * can be limited to 8 bits when anothers register is used as a source
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::ARM::t2LDRpci), OperandIs::unique(0, Reg(REG_PC)),
          Not::unique(InITBlock::unique()))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(1), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOpcode::unique(llvm::ARM::t2LDRi12),
              SetOperand::unique(Operand(0), Temp(0)),
              AddOperand::unique(Operand(1), Temp(0)),
              SetOperand::unique(Operand(2), Constant(0)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #16: LDR.w pc, [pc, #<imm>]
   * Last in ITBlock
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::ARM::t2LDRpci),
                                              OperandIs::unique(0, Reg(REG_PC)),
                                              LastInITBlock::unique())),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(1), /* keepCond */ false),
          ItPatch::unique(false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOpcode::unique(llvm::ARM::t2LDRi12),
              SetOperand::unique(Operand(0), Temp(0)),
              AddOperand::unique(Operand(1), Temp(0)),
              SetOperand::unique(Operand(2), Constant(0)))),
          GetNextInstAddr::unique(Temp(0), /* keepCond */ true,
                                  /* invCond */ true),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #17: LDR.w <reg>, [pc, #<imm>]
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          Or::unique(
              conv_unique<PatchCondition>(OpIs::unique(llvm::ARM::t2LDRpci),
                                          OpIs::unique(llvm::ARM::t2LDRHpci),
                                          OpIs::unique(llvm::ARM::t2LDRBpci),
                                          OpIs::unique(llvm::ARM::t2LDRSHpci),
                                          OpIs::unique(llvm::ARM::t2LDRSBpci))),
          Not::unique(OperandIs::unique(0, Reg(REG_PC))))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(1), /* keepCond */ false),
          ItPatch::unique(false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              ReplaceOpcode::unique(std::map<unsigned, unsigned>({
                  {llvm::ARM::t2LDRpci, llvm::ARM::t2LDRi12},
                  {llvm::ARM::t2LDRBpci, llvm::ARM::t2LDRBi12},
                  {llvm::ARM::t2LDRHpci, llvm::ARM::t2LDRHi12},
                  {llvm::ARM::t2LDRSBpci, llvm::ARM::t2LDRSBi12},
                  {llvm::ARM::t2LDRSHpci, llvm::ARM::t2LDRSHi12},
              })),
              AddOperand::unique(Operand(1), Temp(0)),
              SetOperand::unique(Operand(2), Constant(0))))));

  // Instruction with PC 2 times
  // ===========================
  //

  /* Rule #18:
   * - PC in the first (dest) and third operand (src)
   * - PC in the first (dest) and second operand (src)
   * not in ITBlock
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OperandIs::unique(0, Reg(REG_PC)), Not::unique(InITBlock::unique()),
          Or::unique(conv_unique<PatchCondition>(
              And::unique(conv_unique<PatchCondition>(
                  Not::unique(OperandIs::unique(1, Reg(REG_PC))),
                  OperandIs::unique(2, Reg(REG_PC)), PCInst1Dst3Src.clone())),
              And::unique(conv_unique<PatchCondition>(
                  OperandIs::unique(1, Reg(REG_PC)),
                  Not::unique(OperandIs::unique(2, Reg(REG_PC))),
                  Or::unique(conv_unique<PatchCondition>(
                      PCInst1Dst2SrcOR3Src.clone(), PCInst2Op.clone())))))))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #19:
   * - PC in the first (dest) and third operand (src)
   * - PC in the first (dest) and second operand (src)
   * Last in ITBlock
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OperandIs::unique(0, Reg(REG_PC)), LastInITBlock::unique(),
          Or::unique(conv_unique<PatchCondition>(
              And::unique(conv_unique<PatchCondition>(
                  Not::unique(OperandIs::unique(1, Reg(REG_PC))),
                  OperandIs::unique(2, Reg(REG_PC)), PCInst1Dst3Src.clone())),
              And::unique(conv_unique<PatchCondition>(
                  OperandIs::unique(1, Reg(REG_PC)),
                  Not::unique(OperandIs::unique(2, Reg(REG_PC))),
                  Or::unique(conv_unique<PatchCondition>(
                      PCInst1Dst2SrcOR3Src.clone(), PCInst2Op.clone())))))))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ false),
          ItPatch::unique(false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)))),
          GetNextInstAddr::unique(Temp(0), /* keepCond */ true,
                                  /* invCond */ true),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  // Instruction with PC 1 time
  // ==========================
  //
  /* Rule #20: ADD... with PC in the first (dest)
   * not in ITBlock
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OperandIs::unique(0, Reg(REG_PC)),
          Not::unique(OperandIs::unique(1, Reg(REG_PC))),
          Not::unique(OperandIs::unique(2, Reg(REG_PC))),
          Not::unique(InITBlock::unique()),
          Or::unique(conv_unique<PatchCondition>(PCInst2Op.clone(),
                                                 PCInst1OpDest.clone())))),
      conv_unique<PatchGenerator>(
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(0), Temp(0)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #21: ADD... with PC in the first (dest)
   * Last in ITBlock
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OperandIs::unique(0, Reg(REG_PC)),
          Not::unique(OperandIs::unique(1, Reg(REG_PC))),
          Not::unique(OperandIs::unique(2, Reg(REG_PC))),
          LastInITBlock::unique(),
          Or::unique(conv_unique<PatchCondition>(PCInst2Op.clone(),
                                                 PCInst1OpDest.clone())))),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          ItPatch::unique(false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(0), Temp(0)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #22:
   * - PC in the second operand (src only)
   * - PC in the third operand (src only)
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          Not::unique(OperandIs::unique(0, Reg(REG_PC))),
          Or::unique(conv_unique<PatchCondition>(
              And::unique(conv_unique<PatchCondition>(
                  OperandIs::unique(1, Reg(REG_PC)),
                  Not::unique(OperandIs::unique(2, Reg(REG_PC))),
                  Or::unique(conv_unique<PatchCondition>(
                      PCInst2Op.clone(), PCInst1OpSrcOff1.clone())))),
              And::unique(conv_unique<PatchCondition>(
                  Not::unique(OperandIs::unique(1, Reg(REG_PC))),
                  OperandIs::unique(2, Reg(REG_PC)),
                  Or::unique(conv_unique<PatchCondition>(
                      PCInst1Dst2SrcOR3Src.clone(),
                      PCInst1OpSrcOff2.clone())))))))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ false),
          ItPatch::unique(false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0))))));

  // Special case: LDM POP PUSH STM
  // ==============================

  /* Rule #23: LDM with PC
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          UseReg::unique(Reg(REG_PC)),
          Or::unique(conv_unique<PatchCondition>(
              Not::unique(InITBlock::unique()), LastInITBlock::unique())),
          Or::unique(conv_unique<PatchCondition>(
              OpIs::unique(llvm::ARM::t2LDMIA),
              OpIs::unique(llvm::ARM::t2LDMDB),
              OpIs::unique(llvm::ARM::t2LDMIA_UPD),
              OpIs::unique(llvm::ARM::t2LDMDB_UPD))))),
      conv_unique<PatchGenerator>(T2LDMPatchGen::unique(Temp(0), true),
                                  SetExchange::unique(Temp(0))));
  /* Rule #24: LDM without PC
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          Not::unique(UseReg::unique(Reg(REG_PC))),
          Or::unique(conv_unique<PatchCondition>(
              OpIs::unique(llvm::ARM::t2LDMIA),
              OpIs::unique(llvm::ARM::t2LDMDB),
              OpIs::unique(llvm::ARM::t2LDMIA_UPD),
              OpIs::unique(llvm::ARM::t2LDMDB_UPD))))),
      conv_unique<PatchGenerator>(T2LDMPatchGen::unique(Temp(0), false)));

  /* Rule #25: POP with PC
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::ARM::tPOP), UseReg::unique(Reg(REG_PC)),
          Or::unique(conv_unique<PatchCondition>(
              Not::unique(InITBlock::unique()), LastInITBlock::unique())))),
      conv_unique<PatchGenerator>(TPopPatchGen::unique(Temp(0)),
                                  SetExchange::unique(Temp(0))));

  /* Rule #26: STM
   *
   * Note: PC and SP cannot be store in Thumb mode
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          Not::unique(UseReg::unique(Reg(REG_PC))),
          Or::unique(conv_unique<PatchCondition>(
              OpIs::unique(llvm::ARM::t2STMIA),
              OpIs::unique(llvm::ARM::t2STMDB),
              OpIs::unique(llvm::ARM::t2STMIA_UPD),
              OpIs::unique(llvm::ARM::t2STMDB_UPD))))),
      conv_unique<PatchGenerator>(T2STMPatchGen::unique(Temp(0))));

  // local monitor
  // =============

  if ((opts & Options::OPT_DISABLE_LOCAL_MONITOR) == 0) {

    /* Rule #27: Clear local monitor state
     */
    rules.emplace_back(
        Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::ARM::t2CLREX),
                                               OpIs::unique(llvm::ARM::tSVC))),
        conv_unique<PatchGenerator>(
            ItPatch::unique(false),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            // for SVC, we need to backup the value of Temp(0) after the syscall
            SaveTemp::unique(Temp(0)),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable)))));

    /* Rule #28: exclusive load 1 register
     */
    rules.emplace_back(
        Or::unique(
            conv_unique<PatchCondition>(OpIs::unique(llvm::ARM::t2LDREXB),
                                        OpIs::unique(llvm::ARM::t2LDREXH))),
        conv_unique<PatchGenerator>(
            GetConstant::unique(Temp(0), Constant(1)),
            GetConstantMap::unique(Temp(0),
                                   std::map<unsigned, Constant>({
                                       {llvm::ARM::t2LDREXB, Constant(1)},
                                       {llvm::ARM::t2LDREXH, Constant(2)},
                                   })),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            WriteOperandCC::unique(
                Operand(1),
                Offset(offsetof(Context, gprState.localMonitor.addr))),
            ItPatch::unique(false),
            ModifyInstruction::unique(InstTransform::UniquePtrVec())));

    /* Rule #29: exclusive load 1 register + offset
     */
    rules.emplace_back(
        OpIs::unique(llvm::ARM::t2LDREX),
        conv_unique<PatchGenerator>(
            GetConstant::unique(Temp(0), Constant(4)),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            AddOperandToTemp::unique(Temp(0), Operand(1), Operand(2)),
            WriteTempCC::unique(
                Temp(0), Offset(offsetof(Context, gprState.localMonitor.addr))),
            ItPatch::unique(false),
            ModifyInstruction::unique(InstTransform::UniquePtrVec())));

    /* Rule #30: exclusive load 2 registers
     */
    rules.emplace_back(
        OpIs::unique(llvm::ARM::t2LDREXD),
        conv_unique<PatchGenerator>(
            GetConstant::unique(Temp(0), Constant(8)),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            WriteOperandCC::unique(
                Operand(2),
                Offset(offsetof(Context, gprState.localMonitor.addr))),
            ItPatch::unique(false),
            ModifyInstruction::unique(InstTransform::UniquePtrVec())));

    /* Rule #31: exclusive store
     */
    rules.emplace_back(
        Or::unique(conv_unique<PatchCondition>(
            OpIs::unique(llvm::ARM::t2STREX), OpIs::unique(llvm::ARM::t2STREXB),
            OpIs::unique(llvm::ARM::t2STREXD),
            OpIs::unique(llvm::ARM::t2STREXH))),
        conv_unique<PatchGenerator>(
            CondExclusifLoad::unique(Temp(0), Temp(1)), ItPatch::unique(false),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable)))));
  }

  // Instruction with no PC
  // ======================

  /* Rule #32: instruction to skip (it, barrier, preload)
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::ARM::t2PLDWi12), OpIs::unique(llvm::ARM::t2PLDWi8),
          OpIs::unique(llvm::ARM::t2PLDWs), OpIs::unique(llvm::ARM::t2PLDi12),
          OpIs::unique(llvm::ARM::t2PLDi8), OpIs::unique(llvm::ARM::t2PLDpci),
          OpIs::unique(llvm::ARM::t2PLDs), OpIs::unique(llvm::ARM::t2PLIi12),
          OpIs::unique(llvm::ARM::t2PLIi8), OpIs::unique(llvm::ARM::t2PLIpci),
          OpIs::unique(llvm::ARM::t2PLIs), OpIs::unique(llvm::ARM::t2IT)

              )),
      PatchGenerator::UniquePtrVec());

  /* Rule #33: all other
   */
  rules.emplace_back(
      Not::unique(Or::unique(conv_unique<PatchCondition>(
          UseReg::unique(Reg(REG_PC)),
          // unsupported instruction
          OpIs::unique(llvm::ARM::tSETEND), OpIs::unique(llvm::ARM::t2BXJ),
          // operand invalid with the current ITBlock
          OpIs::unique(llvm::ARM::t2B), OpIs::unique(llvm::ARM::t2BXAUT),
          OpIs::unique(llvm::ARM::t2Bcc), OpIs::unique(llvm::ARM::t2TBB),
          OpIs::unique(llvm::ARM::t2TBH), OpIs::unique(llvm::ARM::tB),
          OpIs::unique(llvm::ARM::tBL), OpIs::unique(llvm::ARM::tBLXi),
          OpIs::unique(llvm::ARM::tBLXr), OpIs::unique(llvm::ARM::tBX),
          OpIs::unique(llvm::ARM::tBcc), OpIs::unique(llvm::ARM::tCBNZ),
          OpIs::unique(llvm::ARM::tCBZ), OpIs::unique(llvm::ARM::tLDRpci)

              ))),
      conv_unique<PatchGenerator>(
          ItPatch::unique(false),
          ModifyInstruction::unique(InstTransform::UniquePtrVec())));

  return rules;
}

} // namespace

PatchRuleAssembly::PatchRuleAssembly(Options opts)
    : patchRulesARM(getARMPatchRules(opts)),
      patchRulesThumb(getThumbPatchRules(opts)), options(opts),
      itRemainingInst(0), itCond({0}) {}

PatchRuleAssembly::~PatchRuleAssembly() = default;

void PatchRuleAssembly::reset() { itRemainingInst = 0; }

bool PatchRuleAssembly::changeOptions(Options opts) {
  // reset the current state. Options cannot be change during the Engine::patch
  // method
  reset();

  const Options needRecreate =
      Options::OPT_DISABLE_FPR | Options::OPT_DISABLE_OPTIONAL_FPR |
      Options::OPT_DISABLE_D16_D31 | Options::OPT_ARM_MASK |
      Options::OPT_DISABLE_MEMORYACCESS_VALUE;
  if ((opts & needRecreate) != (options & needRecreate)) {
    reset();
    patchRulesARM = getARMPatchRules(opts);
    patchRulesThumb = getThumbPatchRules(opts);
    options = opts;
    return true;
  }
  options = opts;
  return false;
}

void PatchRuleAssembly::patchSTLDMARM(Patch &patch, const LLVMCPU &llvmcpu) {
  // need a least 3 tmpRegister and one SR register
  const unsigned TempRegisterMinimum = 4;

  unsigned usedRegister = 0;
  for (unsigned i = 0; i < AVAILABLE_GPR; i++) {
    if (patch.regUsage[i] != 0 and
        (patch.regUsage[i] & RegisterUsage::RegisterBoth) != 0) {
      usedRegister++;
    }
  }

  if (usedRegister + TempRegisterMinimum <= AVAILABLE_GPR) {
    // the instruction has enought TempRegister
    return;
  }

  QBDI_REQUIRE_ABORT_PATCH(0 < patch.metadata.inst.getNumOperands(), patch,
                           "Invalid instruction");
  QBDI_REQUIRE_ABORT_PATCH(patch.metadata.inst.getOperand(0).isReg(), patch,
                           "Unexpected operand type");

  // don't used the base address as a tempregister
  RegLLVM baseReg = patch.metadata.inst.getOperand(0).getReg();
  bool needSR = (llvmcpu == CPUMode::Thumb);

  if (needSR) {
    // try to set RegisterSavedScratch on a unused register
    for (unsigned i = 0; i < AVAILABLE_GPR; i++) {
      if (patch.regUsage[i] == 0 and GPR_ID[i] != baseReg) {
        patch.regUsage[i] |= RegisterUsage::RegisterSavedScratch;
        needSR = false;
        break;
      }
    }
  }

  for (unsigned i = 0; i < AVAILABLE_GPR; i++) {
    if (patch.regUsage[i] != 0 and
        (patch.regUsage[i] & RegisterUsage::RegisterSavedScratch) == 0 and
        GPR_ID[i] != baseReg) {
      if (needSR) {
        patch.regUsage[i] |= RegisterUsage::RegisterSavedScratch;
        needSR = false;
      } else {
        patch.regUsage[i] |= RegisterUsage::RegisterSaved;
      }
    }
  }

  return;
}

bool PatchRuleAssembly::generate(const llvm::MCInst &inst, rword address,
                                 uint32_t instSize, const LLVMCPU &llvmcpu,
                                 std::vector<Patch> &patchList) {
  if (llvmcpu.getCPUMode() == CPUMode::ARM) {
    return generateARM(inst, address, instSize, llvmcpu, patchList);
  } else {
    return generateThumb(inst, address, instSize, llvmcpu, patchList);
  }
}

bool PatchRuleAssembly::generateARM(const llvm::MCInst &inst, rword address,
                                    uint32_t instSize, const LLVMCPU &llvmcpu,
                                    std::vector<Patch> &patchList) {

  QBDI_REQUIRE_ABORT(itRemainingInst == 0, "Unexpected state");
  Patch instPatch{inst, address, instSize, llvmcpu};
  instPatch.metadata.archMetadata.cond = getCondition(inst, llvmcpu);
  instPatch.metadata.archMetadata.posITblock = 0;

  switch (inst.getOpcode()) {
    case llvm::ARM::LDMIA:
    case llvm::ARM::LDMIB:
    case llvm::ARM::LDMDA:
    case llvm::ARM::LDMDB:
    case llvm::ARM::LDMIA_UPD:
    case llvm::ARM::LDMIB_UPD:
    case llvm::ARM::LDMDA_UPD:
    case llvm::ARM::LDMDB_UPD:
    case llvm::ARM::STMIA:
    case llvm::ARM::STMIB:
    case llvm::ARM::STMDA:
    case llvm::ARM::STMDB:
    case llvm::ARM::STMIA_UPD:
    case llvm::ARM::STMIB_UPD:
    case llvm::ARM::STMDA_UPD:
    case llvm::ARM::STMDB_UPD:
      // LDM and STM can use all register
      // mark some register available for the temp register
      patchSTLDMARM(instPatch, llvmcpu);
      break;
    default:
      break;
  }

  for (uint32_t j = 0; j < patchRulesARM.size(); j++) {
    if (patchRulesARM[j].canBeApplied(instPatch, llvmcpu)) {
      QBDI_DEBUG("Patch ARM rule {} applied", j);

      patchRulesARM[j].apply(instPatch, llvmcpu);
      patchList.push_back(std::move(instPatch));
      Patch &patch = patchList.back();

      if (patch.metadata.modifyPC) {
        reset();
        return true;
      } else {
        return false;
      }
    }
  }
  QBDI_ABORT_PATCH(instPatch, "Not PatchRule found");
}

bool PatchRuleAssembly::generateThumb(const llvm::MCInst &inst, rword address,
                                      uint32_t instSize, const LLVMCPU &llvmcpu,
                                      std::vector<Patch> &patchList) {

  Patch instPatch{inst, address, instSize, llvmcpu};
  instPatch.metadata.archMetadata.cond = getCondition(inst, llvmcpu);
  instPatch.metadata.archMetadata.posITblock = itRemainingInst;

  switch (inst.getOpcode()) {
    case llvm::ARM::t2LDMIA:
    case llvm::ARM::t2LDMDB:
    case llvm::ARM::t2LDMIA_UPD:
    case llvm::ARM::t2LDMDB_UPD:
    case llvm::ARM::t2STMIA:
    case llvm::ARM::t2STMDB:
    case llvm::ARM::t2STMIA_UPD:
    case llvm::ARM::t2STMDB_UPD:
      // LDM and STM can use all register
      // mark some register available for the temp register
      patchSTLDMARM(instPatch, llvmcpu);
      break;
    default:
      break;
  }

  if (inst.getOpcode() == llvm::ARM::t2IT) {
    QBDI_REQUIRE_ABORT_PATCH(
        itRemainingInst == 0, instPatch,
        "IT instruction cannot be inside another IT block");
    QBDI_REQUIRE_ABORT_PATCH(2 == inst.getNumOperands(), instPatch,
                             "Invalid instruction");
    QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(0).isImm(), instPatch,
                             "Unexpected operand type");
    QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(1).isImm(), instPatch,
                             "Unexpected operand type");
    unsigned mask = inst.getOperand(1).getImm();
    switch (mask) {
      case (unsigned)llvm::ARM::PredBlockMask::T:
        itRemainingInst = 1;
        break;
      case (unsigned)llvm::ARM::PredBlockMask::TE:
      case (unsigned)llvm::ARM::PredBlockMask::TT:
        itRemainingInst = 2;
        break;
      case (unsigned)llvm::ARM::PredBlockMask::TTT:
      case (unsigned)llvm::ARM::PredBlockMask::TTE:
      case (unsigned)llvm::ARM::PredBlockMask::TET:
      case (unsigned)llvm::ARM::PredBlockMask::TEE:
        itRemainingInst = 3;
        break;
      case (unsigned)llvm::ARM::PredBlockMask::TTTT:
      case (unsigned)llvm::ARM::PredBlockMask::TTTE:
      case (unsigned)llvm::ARM::PredBlockMask::TTET:
      case (unsigned)llvm::ARM::PredBlockMask::TTEE:
      case (unsigned)llvm::ARM::PredBlockMask::TETT:
      case (unsigned)llvm::ARM::PredBlockMask::TETE:
      case (unsigned)llvm::ARM::PredBlockMask::TEET:
      case (unsigned)llvm::ARM::PredBlockMask::TEEE:
        itRemainingInst = 4;
        break;
      default:
        QBDI_ABORT_PATCH(instPatch, "Unexpected IT mask {}", mask);
    }
    itCond[0] = inst.getOperand(0).getImm();
    for (unsigned i = 1; i < itRemainingInst; i++) {
      if ((mask & (1 << (4 - i))) == 0) {
        itCond[i] = itCond[0];
      } else {
        itCond[i] = llvm::ARMCC::getOppositeCondition(
            (llvm::ARMCC::CondCodes)itCond[0]);
      }
    }
  } else if (itRemainingInst > 0) {
    QBDI_REQUIRE_ABORT_PATCH(instPatch.metadata.archMetadata.cond == itCond[0],
                             instPatch,
                             "Condition doesn't match the last IT condition");
    itRemainingInst--;
    itCond = {itCond[1], itCond[2], itCond[3], 0};
  }

  for (uint32_t j = 0; j < patchRulesThumb.size(); j++) {
    if (patchRulesThumb[j].canBeApplied(instPatch, llvmcpu)) {
      QBDI_DEBUG("Patch Thumb rule {} applied", j);

      patchRulesThumb[j].apply(instPatch, llvmcpu);
      patchList.push_back(std::move(instPatch));
      Patch &patch = patchList.back();

      if (patch.metadata.modifyPC) {
        QBDI_REQUIRE_ABORT_PATCH(itRemainingInst == 0, instPatch,
                                 "Modify PC before the end of ItBlock");
        reset();
        return true;
      } else {
        return false;
      }
    }
  }
  QBDI_ABORT_PATCH(instPatch, "Not PatchRule found");
}

bool PatchRuleAssembly::earlyEnd(const LLVMCPU &llvmcpu,
                                 std::vector<Patch> &patchList) {
  reset();
  return true;
}

} // namespace QBDI
