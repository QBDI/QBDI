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
#include <cerrno>
#include <csignal>
#include <cstdint>
#include <sys/wait.h>
#include <unistd.h>

#include "ARMInstrInfo.h"

#include "Engine/LLVMCPU.h"
#include "Patch/ARM/InstInfo_ARM.h"
#include "Patch/ARM/Layer2_ARM.h"
#include "Patch/ARM/MemoryAccess_ARM.h"
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

bool isJazelleTrivialImplementation() {
  static const bool trivial = [] {
    pid_t pid = fork();
    if (pid < 0) {
      return false;
    }
    if (pid == 0) {
      // single-threaded child: safe to install a SIGILL handler
      struct sigaction newAction{};
      newAction.sa_handler = [](int) { _exit(1); };
      sigemptyset(&newAction.sa_mask);
      newAction.sa_flags = 0;

      if (sigaction(SIGILL, &newAction, nullptr) != 0) {
        _exit(1);
      }

      uint32_t jidr = 0;
      asm volatile("mrc p14, 7, %0, c0, c0, 0" : "=r"(jidr));

      constexpr uint32_t implementerAndSubarchMask = 0x0FFFF000;
      _exit((jidr & implementerAndSubarchMask) == 0 ? 0 : 1);
    }

    int status = 0;
    pid_t waited;
    do {
      waited = waitpid(pid, &status, 0);
    } while (waited == -1 and errno == EINTR);

    if (waited != pid) {
      return false;
    }

    return WIFEXITED(status) and WEXITSTATUS(status) == 0;
  }();
  return trivial;
}

namespace A32 {

// instruction where PC can be the first (dest) operand only
using PCInst1OpDest =
    OpIsIn<llvm::ARM::LDR_PRE_IMM, llvm::ARM::LDR_POST_IMM, llvm::ARM::MOVi,
           llvm::ARM::MOVi16, llvm::ARM::MVNi>;

// instruction where PC can be the first (src) operand
using PCInst1OpSrc =
    OpIsIn<llvm::ARM::CMNri, llvm::ARM::CMPri, llvm::ARM::PLDWi12,
           llvm::ARM::PLDWrs, llvm::ARM::PLDi12, llvm::ARM::PLDrs,
           llvm::ARM::TEQri, llvm::ARM::TSTri, llvm::ARM::VLDMDIA,
           llvm::ARM::VLDMSIA, llvm::ARM::FLDMXIA, llvm::ARM::VSTMDIA,
           llvm::ARM::VSTMSIA, llvm::ARM::FSTMXIA>;

// instruction where PC can be the second (src) operand
using PCInst1OpSrcOff1 =
    OpIsIn<llvm::ARM::LDRBi12, llvm::ARM::LDRBrs, llvm::ARM::LDRH,
           llvm::ARM::LDRSB, llvm::ARM::LDRSH, llvm::ARM::STRBi12,
           llvm::ARM::STRBrs, llvm::ARM::STRH, llvm::ARM::STRT_POST_IMM,
           llvm::ARM::VLDRD, llvm::ARM::VLDRH, llvm::ARM::VLDRS,
           llvm::ARM::VSTRD, llvm::ARM::VSTRH, llvm::ARM::VSTRS>;

// instruction where PC can be the third (src) operand
using PCInst1OpSrcOff2 =
    OpIsIn<llvm::ARM::LDRD, llvm::ARM::STC_OFFSET, llvm::ARM::STRD,
           llvm::ARM::LDC2L_OFFSET, llvm::ARM::LDC2L_OPTION,
           llvm::ARM::LDC2L_POST, llvm::ARM::LDC2L_PRE, llvm::ARM::LDC2_OFFSET,
           llvm::ARM::LDC2_OPTION, llvm::ARM::LDC2_POST, llvm::ARM::LDC2_PRE,
           llvm::ARM::LDCL_OFFSET, llvm::ARM::LDCL_OPTION, llvm::ARM::LDCL_POST,
           llvm::ARM::LDCL_PRE, llvm::ARM::LDC_OFFSET, llvm::ARM::LDC_OPTION,
           llvm::ARM::LDC_POST, llvm::ARM::LDC_PRE>;

// instruction where PC can be the first/second (src) operands
using PCInst2OpSrc =
    OpIsIn<llvm::ARM::CMNzrr, llvm::ARM::CMNzrsi, llvm::ARM::CMPrr,
           llvm::ARM::STRi12, llvm::ARM::STRrs, llvm::ARM::TEQrr,
           llvm::ARM::TSTrr>;

// instruction where PC can be the first (dest) or/and the second (src)
// operands only
using PCInst2Op = OpIsIn<llvm::ARM::ADCri, llvm::ARM::ADDri, llvm::ARM::ANDri,
                         llvm::ARM::BICri, llvm::ARM::EORri, llvm::ARM::LDRi12,
                         llvm::ARM::LDRrs, llvm::ARM::MOVr, llvm::ARM::MOVsi,
                         llvm::ARM::MVNr, llvm::ARM::ORRri, llvm::ARM::RSBri,
                         llvm::ARM::RSCri, llvm::ARM::SBCri, llvm::ARM::SUBri>;

// instruction where PC can be the first (dest) or/and the second/third (src)
// operands only
using PCInst3Op = OpIsIn<
    llvm::ARM::ADCrr, llvm::ARM::ADCrsi, llvm::ARM::ADDrr, llvm::ARM::ADDrsi,
    llvm::ARM::ANDrr, llvm::ARM::ANDrsi, llvm::ARM::BICrr, llvm::ARM::BICrsi,
    llvm::ARM::EORrr, llvm::ARM::EORrsi, llvm::ARM::ORRrr, llvm::ARM::ORRrsi,
    llvm::ARM::RSBrr, llvm::ARM::RSBrsi, llvm::ARM::RSCrr, llvm::ARM::RSCrsi,
    llvm::ARM::SBCrr, llvm::ARM::SBCrsi, llvm::ARM::SUBrr, llvm::ARM::SUBrsi>;

std::vector<PatchRule> getARMPatchRules(Options opts) {
  std::vector<PatchRule> rules;

  // ARM instruction
  // ===============

  // Warning: All instructions can be conditionnal, Each patchRule must keep the
  // conditionnal behavior

  /* Rule #0: BX lr | mov pc, lr without condition
   *
   *  str lr, <offset PC>
   *  setExchange
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OpIsIn<llvm::ARM::BX_RET, llvm::ARM::MOVPCLR>::unique(),
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
      And::unique(conv_unique<PatchCondition>(
          OpIsIn<llvm::ARM::BX_RET, llvm::ARM::MOVPCLR>::unique(),
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
          OpIsIn<llvm::ARM::BX, llvm::ARM::BX_pred, llvm::ARM::BXJ>::unique(),
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
          OpIsIn<llvm::ARM::BX, llvm::ARM::BX_pred, llvm::ARM::BXJ>::unique(),
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
          OpIsIn<llvm::ARM::BX, llvm::ARM::BX_pred, llvm::ARM::BXJ>::unique(),
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
          OpIsIn<llvm::ARM::BX, llvm::ARM::BX_pred, llvm::ARM::BXJ>::unique(),
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
      OpIsIn<llvm::ARM::BLX, llvm::ARM::BLX_pred>::unique(),
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
      OpIsIn<llvm::ARM::BL, llvm::ARM::BL_pred>::unique(),
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
        OpIs::unique(llvm::ARM::CLREX),
        conv_unique<PatchGenerator>(
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable)))));

    /* Rule #12: Clear local monitor state
     */
    rules.emplace_back(
        OpIs::unique(llvm::ARM::SVC),
        conv_unique<PatchGenerator>(
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            // for SVC, we need to backup the value of Temp(0) after the syscall
            SaveTemp::unique(Temp(0), true),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable)))));

    /* Rule #13: exclusive load 1 register
     */
    rules.emplace_back(
        OpIsIn<llvm::ARM::LDREX, llvm::ARM::LDREXB, llvm::ARM::LDREXD,
               llvm::ARM::LDREXH, llvm::ARM::LDAEX, llvm::ARM::LDAEXB,
               llvm::ARM::LDAEXD, llvm::ARM::LDAEXH>::unique(),
        conv_unique<PatchGenerator>(
            GetConstantMap::unique(Temp(0),
                                   std::map<unsigned, Constant>({
                                       {llvm::ARM::LDREXB, Constant(1)},
                                       {llvm::ARM::LDREXH, Constant(2)},
                                       {llvm::ARM::LDREX, Constant(4)},
                                       {llvm::ARM::LDREXD, Constant(8)},
                                       {llvm::ARM::LDAEXB, Constant(1)},
                                       {llvm::ARM::LDAEXH, Constant(2)},
                                       {llvm::ARM::LDAEX, Constant(4)},
                                       {llvm::ARM::LDAEXD, Constant(8)},
                                   })),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            WriteOperandCC::unique(
                Operand(1),
                Offset(offsetof(Context, gprState.localMonitor.addr))),
            ModifyInstruction::unique(InstTransform::UniquePtrVec())));

    /* Rule #14: exclusive store register(s)
     */
    rules.emplace_back(
        OpIsIn<llvm::ARM::STREXB, llvm::ARM::STREXH, llvm::ARM::STREX,
               llvm::ARM::STREXD, llvm::ARM::STLEXB, llvm::ARM::STLEXH,
               llvm::ARM::STLEX, llvm::ARM::STLEXD>::unique(),
        conv_unique<PatchGenerator>(
            CondExclusifLoad::unique(Temp(0), Temp(1), Temp(2)),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            WriteOperandCC::unique(Operand(0),
                                   Shadow(MEM_EXCLUSIVE_STATUS_TAG)),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable)))));
  } else {

    /* Rule #14b: exclusive store register(s), local monitor disabled
     */
    rules.emplace_back(
        OpIsIn<llvm::ARM::STREXB, llvm::ARM::STREXH, llvm::ARM::STREX,
               llvm::ARM::STREXD, llvm::ARM::STLEXB, llvm::ARM::STLEXH,
               llvm::ARM::STLEX, llvm::ARM::STLEXD>::unique(),
        conv_unique<PatchGenerator>(
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            WriteOperandCC::unique(Operand(0),
                                   Shadow(MEM_EXCLUSIVE_STATUS_TAG))));
  }

  // Instruction without PC
  // ======================

  /* Rule #15: instruction to skip (barrier, preload)
   */
  rules.emplace_back(
      OpIsIn<llvm::ARM::PLDWi12, llvm::ARM::PLDWrs, llvm::ARM::PLDi12,
             llvm::ARM::PLDrs, llvm::ARM::PLIi12, llvm::ARM::PLIrs>::unique(),
      PatchGenerator::UniquePtrVec());

  /* Rule #16: all other instruction without PC
   *
   * Note: This patch should be at the end of the list. However, as many
   * instruction doesn't used PC, we place it here to apply it early on
   * intructions without PC.
   */
  rules.emplace_back(
      Not::unique(Or::unique(conv_unique<PatchCondition>(
          UseReg::unique(Reg(REG_PC)),
          // ADR is decoded as ADDri; SETEND is an unsupported instruction;
          OpIsIn<llvm::ARM::ADR, llvm::ARM::SETEND>::unique()))),
      conv_unique<PatchGenerator>(
          ModifyInstruction::unique(InstTransform::UniquePtrVec())));

  // Instruction with PC : special case LDM / STM
  // ============================================

  /* Rule #17: LDM with PC
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          UseReg::unique(Reg(REG_PC)),
          OpIsIn<llvm::ARM::LDMIA, llvm::ARM::LDMIB, llvm::ARM::LDMDA,
                 llvm::ARM::LDMDB, llvm::ARM::LDMIA_UPD, llvm::ARM::LDMIB_UPD,
                 llvm::ARM::LDMDA_UPD, llvm::ARM::LDMDB_UPD>::unique())),
      conv_unique<PatchGenerator>(LDMPatchGen::unique(Temp(0)),
                                  SetExchange::unique(Temp(0))));

  /* Rule #18: STM with PC
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          UseReg::unique(Reg(REG_PC)),
          OpIsIn<llvm::ARM::STMIA, llvm::ARM::STMIB, llvm::ARM::STMDA,
                 llvm::ARM::STMDB, llvm::ARM::STMIA_UPD, llvm::ARM::STMIB_UPD,
                 llvm::ARM::STMDA_UPD, llvm::ARM::STMDB_UPD>::unique())),
      conv_unique<PatchGenerator>(STMPatchGen::unique(Temp(0))));

  // Instruction with PC as source
  // =============================

  /* Rule #19: ADD/SUB/... with PC as source only:
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
              Not::unique(OperandIs::unique(0, Reg(REG_PC))),
              OperandIs::unique(1, Reg(REG_PC)),
              OperandIs::unique(2, Reg(REG_PC)), PCInst3Op::unique())),
          And::unique(conv_unique<PatchCondition>(
              OperandIs::unique(0, Reg(REG_PC)),
              OperandIs::unique(1, Reg(REG_PC)),
              Not::unique(OperandIs::unique(2, Reg(REG_PC))),
              PCInst2OpSrc::unique())),
          And::unique(conv_unique<PatchCondition>(
              OperandIs::unique(0, Reg(REG_PC)),
              Not::unique(OperandIs::unique(1, Reg(REG_PC))),
              Not::unique(OperandIs::unique(2, Reg(REG_PC))),
              OpIsInUnion<PCInst2OpSrc, PCInst1OpSrc>::unique())),
          And::unique(conv_unique<PatchCondition>(
              Not::unique(OperandIs::unique(0, Reg(REG_PC))),
              OperandIs::unique(1, Reg(REG_PC)),
              Not::unique(OperandIs::unique(2, Reg(REG_PC))),
              OpIsInUnion<PCInst1OpSrcOff1, PCInst2OpSrc, PCInst2Op,
                          PCInst3Op>::unique())),
          And::unique(conv_unique<PatchCondition>(
              Not::unique(OperandIs::unique(0, Reg(REG_PC))),
              Not::unique(OperandIs::unique(1, Reg(REG_PC))),
              OperandIs::unique(2, Reg(REG_PC)),
              OpIsInUnion<PCInst1OpSrcOff2, PCInst3Op>::unique())))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0))))));

  // Instruction with PC 3 times and dest
  // ====================================

  /* Rule #20: LDR/ADD/SUB/... with PC in first (dst) and second/third (src)
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
          OperandIs::unique(2, Reg(REG_PC)), PCInst3Op::unique(),
          Not::unique(HasCond::unique()))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #21: LDR/ADD/SUB/... with PC in first (dst) and second/third (src)
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
          OperandIs::unique(2, Reg(REG_PC)), PCInst3Op::unique(),
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

  /* Rule #22: ADD/SUB/... with PC in first (dst) and third (src) operand and
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
          OperandIs::unique(2, Reg(REG_PC)), PCInst3Op::unique(),
          Not::unique(HasCond::unique()))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #23: ADD/SUB/... with PC in first (dst) and third (src) operand and
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
          OperandIs::unique(2, Reg(REG_PC)), PCInst3Op::unique(),
          HasCond::unique())),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          GetPCOffset::unique(Temp(1), Constant(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(0), Temp(0)),
              SetOperand::unique(Operand(2), Temp(1)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #24: LDR/ADD/SUB/... with PC in first (dst) and second (src) operand
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
          OpIsInUnion<PCInst3Op, PCInst2Op>::unique(),
          Not::unique(HasCond::unique()))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #25: LDR/ADD/SUB/... with PC in first (dst) and second (src) operand
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
          OpIsInUnion<PCInst3Op, PCInst2Op>::unique(), HasCond::unique())),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          GetPCOffset::unique(Temp(1), Constant(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(0), Temp(0)),
              SetOperand::unique(Operand(1), Temp(1)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  // Instruction with PC 1 times and dst
  // ===================================

  /* Rule #26: LDR/ADD/SUB/... with PC in first (dst) operand and without cond
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
          OpIsInUnion<PCInst1OpDest, PCInst2Op, PCInst3Op>::unique())),
      conv_unique<PatchGenerator>(
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  /* Rule #27: LDR/ADD/SUB/... with PC in first (dst) operand and with cond
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
          OpIsInUnion<PCInst1OpDest, PCInst2Op, PCInst3Op>::unique(),
          HasCond::unique())),
      conv_unique<PatchGenerator>(
          GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)))),
          WritePC::unique(Temp(0)), SetExchange::unique(Temp(0))));

  return rules;
}

bool isSupportedInstruction(const Patch &patch, const char *&reason) {
  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;
  switch (inst.getOpcode()) {
    case llvm::ARM::ADR:
      reason = "Expect ADR to be decoded as ADDRi";
      return false;
    case llvm::ARM::SETEND:
      reason = "SETEND is currently not supported by QBDI.";
      return false;
    case llvm::ARM::BXJ:
      if (not isJazelleTrivialImplementation()) {
        reason =
            "BXJ is not supported by QBDI: this device implements "
            "Jazelle hardware bytecode execution.";
        return false;
      }
      return true;
    case llvm::ARM::ERET:
      reason =
          "ERET is a privileged instruction (EL1+ only), not supported "
          "by QBDI.";
      return false;
    case llvm::ARM::HVC:
      reason =
          "HVC is a privileged instruction (routes to EL2), not "
          "supported by QBDI.";
      return false;
    case llvm::ARM::BL:
    case llvm::ARM::BLX:
    case llvm::ARM::BLX_pred:
    case llvm::ARM::BLXi:
    case llvm::ARM::BL_pred:
    case llvm::ARM::BX:
    case llvm::ARM::BX_RET:
    case llvm::ARM::BX_pred:
    case llvm::ARM::MOVPCLR:
    case llvm::ARM::Bcc:
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
      return true;
    default:
      break;
  }
  bool usePC = false;
  bool isPC0 = false;
  bool isPC1 = false;
  bool isPC2 = false;
  for (unsigned int i = 0; i < inst.getNumOperands(); i++) {
    const llvm::MCOperand &op = inst.getOperand(i);
    if (op.isReg() && op.getReg() == llvm::ARM::PC) {
      usePC = true;
      if (i == 0) {
        isPC0 = true;
      } else if (i == 1) {
        isPC1 = true;
      } else if (i == 2) {
        isPC2 = true;
      } else {
        reason = "No instruction support PC as operand greater than 2.";
        return false;
      }
    }
  }
  if (not usePC) {
    return true;
  }
  if (PCInst3Op().test(patch, llvmcpu)) {
    return true;
  }
  if (OpIsInUnion<PCInst2Op, PCInst2OpSrc>().test(patch, llvmcpu)) {
    if (isPC2) {
      reason = "PC is not supported as operand 2.";
      return false;
    } else {
      return true;
    }
  }
  if (PCInst1OpSrcOff2().test(patch, llvmcpu)) {
    if (isPC0) {
      reason = "PC is not supported as operand 0.";
      return false;
    } else if (isPC1) {
      reason = "PC is not supported as operand 1.";
      return false;
    } else {
      return true;
    }
  }
  if (PCInst1OpSrcOff1().test(patch, llvmcpu)) {
    if (isPC0) {
      reason = "PC is not supported as operand 0.";
      return false;
    } else if (isPC2) {
      reason = "PC is not supported as operand 2.";
      return false;
    } else {
      return true;
    }
  }
  if (OpIsInUnion<PCInst1OpDest, PCInst1OpSrc>().test(patch, llvmcpu)) {
    if (isPC1) {
      reason = "PC is not supported as operand 1.";
      return false;
    } else if (isPC2) {
      reason = "PC is not supported as operand 2.";
      return false;
    } else {
      return true;
    }
  }

  reason = "PC register is not supported for this instruction";
  return false;
}

} // namespace A32

namespace Thumb {

// instruction where PC can be the first (dest) operand only
using PCInst1OpDest =
    OpIsIn<llvm::ARM::t2LDRi8, llvm::ARM::t2LDR_PRE, llvm::ARM::t2LDR_POST,
           llvm::ARM::t2LDRi12, llvm::ARM::t2LDRs>;

// instruction where PC can be the second (src) operand
using PCInst1OpSrcOff1 =
    OpIsIn<llvm::ARM::VLDRD, llvm::ARM::VLDRH, llvm::ARM::VLDRS>;

// instruction where PC can be the third (src) operand
using PCInst1OpSrcOff2 =
    OpIsIn<llvm::ARM::t2LDC2L_OFFSET, llvm::ARM::t2LDC2_OFFSET,
           llvm::ARM::t2LDCL_OFFSET, llvm::ARM::t2LDC_OFFSET,
           llvm::ARM::t2LDRDi8, llvm::ARM::tADDspr>;

// instruction where PC can be the first (dest) or/and the second (src)
// operands only
using PCInst2Op = OpIsIn<llvm::ARM::tMOVr>;

// instruction where PC can be the first (dest) AND the third (src) operands
// the first and third operand is always the same
using PCInst1Dst3Src = OpIsIn<llvm::ARM::tADDrSP>;

// instruction where PC can be the first|second (dest/src) OR the third (src)
// operands
// the first and second operand is always the same
using PCInst1Dst2SrcOR3Src = OpIsIn<llvm::ARM::tADDhirr>;

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

  // Thumb instruction
  // =================

  /* Rule #0: Adr <rx>, <imm>
   */
  rules.emplace_back(OpIsIn<llvm::ARM::tADR, llvm::ARM::t2ADR>::unique(),
                     conv_unique<PatchGenerator>(GetPCOffset::unique(
                         Operand(0), Operand(1), /* keepCond */ true)));

  /* Rule #1: Bcc <imm>
   *
   *    no SetExchange, as Bcc doesn't perform the change of mode
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(
          And::unique(conv_unique<PatchCondition>(
              OpIsIn<llvm::ARM::tBcc, llvm::ARM::t2Bcc>::unique(),
              Not::unique(InITBlock::unique()))),
          And::unique(conv_unique<PatchCondition>(
              OpIsIn<llvm::ARM::tB, llvm::ARM::t2B>::unique(),
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
          OpIsIn<llvm::ARM::tB, llvm::ARM::t2B>::unique(),
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

  /* Rule #8: BXcc <reg> | BXJcc <reg>
   */
  rules.emplace_back(And::unique(conv_unique<PatchCondition>(
                         OpIsIn<llvm::ARM::tBX, llvm::ARM::t2BXJ>::unique(),
                         LastInITBlock::unique())),
                     conv_unique<PatchGenerator>(
                         GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
                         GetOperandCC::unique(Temp(0), Operand(0)),
                         WritePC::unique(Temp(0)),
                         SetExchange::unique(Temp(0))));

  /* Rule #9: BX <reg> | BXJ <reg>
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          OpIsIn<llvm::ARM::tBX, llvm::ARM::t2BXJ>::unique(),
          Not::unique(InITBlock::unique()))),
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
          OpIsIn<llvm::ARM::tCBNZ, llvm::ARM::tCBZ>::unique(),
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
          OpIsIn<llvm::ARM::t2TBB, llvm::ARM::t2TBH>::unique(),
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
          OpIsIn<llvm::ARM::t2LDRpci, llvm::ARM::t2LDRHpci,
                 llvm::ARM::t2LDRBpci, llvm::ARM::t2LDRSHpci,
                 llvm::ARM::t2LDRSBpci>::unique(),
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
                  OperandIs::unique(2, Reg(REG_PC)), PCInst1Dst3Src::unique())),
              And::unique(conv_unique<PatchCondition>(
                  OperandIs::unique(1, Reg(REG_PC)),
                  Not::unique(OperandIs::unique(2, Reg(REG_PC))),
                  OpIsInUnion<PCInst1Dst2SrcOR3Src, PCInst2Op>::unique())))))),
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
                  OperandIs::unique(2, Reg(REG_PC)), PCInst1Dst3Src::unique())),
              And::unique(conv_unique<PatchCondition>(
                  OperandIs::unique(1, Reg(REG_PC)),
                  Not::unique(OperandIs::unique(2, Reg(REG_PC))),
                  OpIsInUnion<PCInst1Dst2SrcOR3Src, PCInst2Op>::unique())))))),
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
  rules.emplace_back(And::unique(conv_unique<PatchCondition>(
                         OperandIs::unique(0, Reg(REG_PC)),
                         Not::unique(OperandIs::unique(1, Reg(REG_PC))),
                         Not::unique(OperandIs::unique(2, Reg(REG_PC))),
                         Not::unique(InITBlock::unique()),
                         OpIsInUnion<PCInst2Op, PCInst1OpDest>::unique())),
                     conv_unique<PatchGenerator>(
                         ModifyInstruction::unique(conv_unique<InstTransform>(
                             SetOperand::unique(Operand(0), Temp(0)))),
                         WritePC::unique(Temp(0)),
                         SetExchange::unique(Temp(0))));

  /* Rule #21: ADD... with PC in the first (dest)
   * Last in ITBlock
   */
  rules.emplace_back(And::unique(conv_unique<PatchCondition>(
                         OperandIs::unique(0, Reg(REG_PC)),
                         Not::unique(OperandIs::unique(1, Reg(REG_PC))),
                         Not::unique(OperandIs::unique(2, Reg(REG_PC))),
                         LastInITBlock::unique(),
                         OpIsInUnion<PCInst2Op, PCInst1OpDest>::unique())),
                     conv_unique<PatchGenerator>(
                         GetNextInstAddr::unique(Temp(0), /* keepCond */ false),
                         ItPatch::unique(false),
                         ModifyInstruction::unique(conv_unique<InstTransform>(
                             SetOperand::unique(Operand(0), Temp(0)))),
                         WritePC::unique(Temp(0)),
                         SetExchange::unique(Temp(0))));

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
                  OpIsInUnion<PCInst2Op, PCInst1OpSrcOff1>::unique())),
              And::unique(conv_unique<PatchCondition>(
                  Not::unique(OperandIs::unique(1, Reg(REG_PC))),
                  OperandIs::unique(2, Reg(REG_PC)),
                  OpIsInUnion<PCInst1Dst2SrcOR3Src,
                              PCInst1OpSrcOff2>::unique())))))),
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
          OpIsIn<llvm::ARM::t2LDMIA, llvm::ARM::t2LDMDB, llvm::ARM::t2LDMIA_UPD,
                 llvm::ARM::t2LDMDB_UPD>::unique())),
      conv_unique<PatchGenerator>(T2LDMPatchGen::unique(Temp(0), true),
                                  SetExchange::unique(Temp(0))));
  /* Rule #24: LDM without PC
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(
          Not::unique(UseReg::unique(Reg(REG_PC))),
          OpIsIn<llvm::ARM::t2LDMIA, llvm::ARM::t2LDMDB, llvm::ARM::t2LDMIA_UPD,
                 llvm::ARM::t2LDMDB_UPD>::unique())),
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
          OpIsIn<llvm::ARM::t2STMIA, llvm::ARM::t2STMDB, llvm::ARM::t2STMIA_UPD,
                 llvm::ARM::t2STMDB_UPD>::unique())),
      conv_unique<PatchGenerator>(T2STMPatchGen::unique(Temp(0))));

  // local monitor
  // =============

  if ((opts & Options::OPT_DISABLE_LOCAL_MONITOR) == 0) {

    /* Rule #27: Clear local monitor state
     */
    rules.emplace_back(
        OpIs::unique(llvm::ARM::t2CLREX),
        conv_unique<PatchGenerator>(
            ItPatch::unique(false),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable)))));

    /* Rule #28: Clear local monitor state on svc
     */
    rules.emplace_back(
        OpIs::unique(llvm::ARM::tSVC),
        conv_unique<PatchGenerator>(
            ItPatch::unique(false),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            // for SVC, we need to backup the value of Temp(0) after the syscall
            SaveTemp::unique(Temp(0), true),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable)))));

    /* Rule #29: exclusive load 1 register
     */
    rules.emplace_back(
        OpIsIn<llvm::ARM::t2LDREXB, llvm::ARM::t2LDREXH, llvm::ARM::t2LDAEXB,
               llvm::ARM::t2LDAEXH, llvm::ARM::t2LDAEX>::unique(),
        conv_unique<PatchGenerator>(
            GetConstantMap::unique(Temp(0),
                                   std::map<unsigned, Constant>({
                                       {llvm::ARM::t2LDREXB, Constant(1)},
                                       {llvm::ARM::t2LDREXH, Constant(2)},
                                       {llvm::ARM::t2LDAEXB, Constant(1)},
                                       {llvm::ARM::t2LDAEXH, Constant(2)},
                                       {llvm::ARM::t2LDAEX, Constant(4)},
                                   })),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable))),
            WriteOperandCC::unique(
                Operand(1),
                Offset(offsetof(Context, gprState.localMonitor.addr))),
            ItPatch::unique(false),
            ModifyInstruction::unique(InstTransform::UniquePtrVec())));

    /* Rule #30: exclusive load 1 register + offset
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

    /* Rule #31: exclusive load 2 registers
     */
    rules.emplace_back(
        OpIsIn<llvm::ARM::t2LDREXD, llvm::ARM::t2LDAEXD>::unique(),
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

    /* Rule #32: exclusive store register(s)
     */
    rules.emplace_back(
        OpIsIn<llvm::ARM::t2STREXB, llvm::ARM::t2STREXH, llvm::ARM::t2STREX,
               llvm::ARM::t2STREXD, llvm::ARM::t2STLEXB, llvm::ARM::t2STLEXH,
               llvm::ARM::t2STLEX, llvm::ARM::t2STLEXD>::unique(),
        conv_unique<PatchGenerator>(
            CondExclusifLoad::unique(Temp(0), Temp(1), Temp(2)),
            ItPatch::unique(false),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            WriteOperandCC::unique(Operand(0),
                                   Shadow(MEM_EXCLUSIVE_STATUS_TAG)),
            GetConstant::unique(Temp(0), Constant(0)),
            WriteTempCC::unique(
                Temp(0),
                Offset(offsetof(Context, gprState.localMonitor.enable)))));
  } else {

    /* Rule #32b: exclusive store register(s), local monitor disabled
     */
    rules.emplace_back(
        OpIsIn<llvm::ARM::t2STREXB, llvm::ARM::t2STREXH, llvm::ARM::t2STREX,
               llvm::ARM::t2STREXD, llvm::ARM::t2STLEXB, llvm::ARM::t2STLEXH,
               llvm::ARM::t2STLEX, llvm::ARM::t2STLEXD>::unique(),
        conv_unique<PatchGenerator>(
            ItPatch::unique(false),
            ModifyInstruction::unique(InstTransform::UniquePtrVec()),
            WriteOperandCC::unique(Operand(0),
                                   Shadow(MEM_EXCLUSIVE_STATUS_TAG))));
  }

  // Instruction with no PC
  // ======================

  /* Rule #33: instruction to skip (it, barrier, preload)
   */
  rules.emplace_back(
      OpIsIn<llvm::ARM::t2PLDWi12, llvm::ARM::t2PLDWi8, llvm::ARM::t2PLDWs,
             llvm::ARM::t2PLDi12, llvm::ARM::t2PLDi8, llvm::ARM::t2PLDpci,
             llvm::ARM::t2PLDs, llvm::ARM::t2PLIi12, llvm::ARM::t2PLIi8,
             llvm::ARM::t2PLIpci, llvm::ARM::t2PLIs, llvm::ARM::t2IT>::unique(),
      PatchGenerator::UniquePtrVec());

  /* Rule #34: all other
   */
  rules.emplace_back(
      Not::unique(Or::unique(conv_unique<PatchCondition>(
          UseReg::unique(Reg(REG_PC)),
          // tSETEND is an unsupported instruction; the others are operands
          // invalid with the current ITBlock
          OpIsIn<llvm::ARM::tSETEND, llvm::ARM::t2B, llvm::ARM::t2BXAUT,
                 llvm::ARM::t2Bcc, llvm::ARM::t2TBB, llvm::ARM::t2TBH,
                 llvm::ARM::tB, llvm::ARM::tBL, llvm::ARM::tBLXi,
                 llvm::ARM::tBLXr, llvm::ARM::tBX, llvm::ARM::tBcc,
                 llvm::ARM::tCBNZ, llvm::ARM::tCBZ,
                 llvm::ARM::tLDRpci>::unique()))),
      conv_unique<PatchGenerator>(
          ItPatch::unique(false),
          ModifyInstruction::unique(InstTransform::UniquePtrVec())));

  return rules;
}

unsigned getITBlockLength(unsigned mask, const Patch &instPatch) {
  switch (mask) {
    case (unsigned)llvm::ARM::PredBlockMask::T:
      return 1;
    case (unsigned)llvm::ARM::PredBlockMask::TE:
    case (unsigned)llvm::ARM::PredBlockMask::TT:
      return 2;
    case (unsigned)llvm::ARM::PredBlockMask::TTT:
    case (unsigned)llvm::ARM::PredBlockMask::TTE:
    case (unsigned)llvm::ARM::PredBlockMask::TET:
    case (unsigned)llvm::ARM::PredBlockMask::TEE:
      return 3;
    case (unsigned)llvm::ARM::PredBlockMask::TTTT:
    case (unsigned)llvm::ARM::PredBlockMask::TTTE:
    case (unsigned)llvm::ARM::PredBlockMask::TTET:
    case (unsigned)llvm::ARM::PredBlockMask::TTEE:
    case (unsigned)llvm::ARM::PredBlockMask::TETT:
    case (unsigned)llvm::ARM::PredBlockMask::TETE:
    case (unsigned)llvm::ARM::PredBlockMask::TEET:
    case (unsigned)llvm::ARM::PredBlockMask::TEEE:
      return 4;
    default:
      QBDI_ABORT("Unexpected IT mask {} {}", mask, instPatch);
  }
}

bool isSupportedInstruction(const Patch &patch, const char *&reason) {
  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;
  const unsigned posITblock = patch.metadata.archMetadata.posITblock;
  // outside an IT block, or the last instruction of one (not strictly mid
  // an IT block)
  const bool notMidITBlock = (posITblock == 0 || posITblock == 1);

  switch (inst.getOpcode()) {
    case llvm::ARM::MVE_LETP:
    case llvm::ARM::MVE_WLSTP_8:
    case llvm::ARM::MVE_WLSTP_16:
    case llvm::ARM::MVE_WLSTP_32:
    case llvm::ARM::MVE_WLSTP_64:
    case llvm::ARM::t2LE:
    case llvm::ARM::t2LEUpdate:
    case llvm::ARM::t2WLS:
      reason =
          "M-profile (Cortex-M) instruction, not supported by QBDI "
          "(A-profile target only).";
      return false;
    case llvm::ARM::tBLXNSr:
    case llvm::ARM::tBXNS:
      reason =
          "ARMv8-M Security Extension instruction, not supported by "
          "QBDI (A-profile target only).";
      return false;
    case llvm::ARM::t2RFEIA:
    case llvm::ARM::t2RFEIAW:
    case llvm::ARM::t2RFEDB:
    case llvm::ARM::t2RFEDBW:
      reason = "RFE is a privileged instruction, not supported by QBDI.";
      return false;
    case llvm::ARM::t2HVC:
      reason =
          "HVC is a privileged instruction (routes to EL2), not "
          "supported by QBDI.";
      return false;
    case llvm::ARM::t2SMC:
      reason =
          "SMC is a privileged instruction (EL1+ only), not supported "
          "by QBDI.";
      return false;
    case llvm::ARM::t2SUBS_PC_LR:
      reason =
          "SUBS pc, lr is a privileged interrupt-return instruction, "
          "not supported by QBDI.";
      return false;
    case llvm::ARM::t2BXJ:
      if (not isJazelleTrivialImplementation()) {
        reason =
            "BXJ is not supported by QBDI: this device implements "
            "Jazelle hardware bytecode execution.";
        return false;
      }
      if (not notMidITBlock) {
        reason =
            "This instruction is not supported in the middle of an IT "
            "block";
        return false;
      }
      return true;
    // no PC operand, no IT-block restriction
    case llvm::ARM::tADR:
    case llvm::ARM::t2ADR:
    case llvm::ARM::tLDRpci:
    case llvm::ARM::t2CLREX:
    case llvm::ARM::tSVC:
    case llvm::ARM::t2LDREXB:
    case llvm::ARM::t2LDREXH:
    case llvm::ARM::t2LDAEXB:
    case llvm::ARM::t2LDAEXH:
    case llvm::ARM::t2LDAEX:
    case llvm::ARM::t2LDREX:
    case llvm::ARM::t2LDREXD:
    case llvm::ARM::t2LDAEXD:
    case llvm::ARM::t2STREXB:
    case llvm::ARM::t2STREXH:
    case llvm::ARM::t2STREX:
    case llvm::ARM::t2STREXD:
    case llvm::ARM::t2STLEXB:
    case llvm::ARM::t2STLEXH:
    case llvm::ARM::t2STLEX:
    case llvm::ARM::t2STLEXD:
    case llvm::ARM::t2PLDWi12:
    case llvm::ARM::t2PLDWi8:
    case llvm::ARM::t2PLDWs:
    case llvm::ARM::t2PLDi12:
    case llvm::ARM::t2PLDi8:
    case llvm::ARM::t2PLDpci:
    case llvm::ARM::t2PLDs:
    case llvm::ARM::t2PLIi12:
    case llvm::ARM::t2PLIi8:
    case llvm::ARM::t2PLIpci:
    case llvm::ARM::t2PLIs:
    case llvm::ARM::t2IT:
      return true;
    case llvm::ARM::tBcc:
    case llvm::ARM::t2Bcc:
      if (posITblock != 0) {
        reason = "Bcc is only supported outside an IT block";
        return false;
      }
      return true;
    case llvm::ARM::tB:
    case llvm::ARM::t2B:
    case llvm::ARM::tBL:
    case llvm::ARM::tBLXi:
    case llvm::ARM::tBLXr:
    case llvm::ARM::tBX:
    case llvm::ARM::t2BXAUT:
    case llvm::ARM::t2TBB:
    case llvm::ARM::t2TBH:
      if (not notMidITBlock) {
        reason =
            "This instruction is not supported in the middle of an IT "
            "block";
        return false;
      }
      return true;
    case llvm::ARM::tCBNZ:
    case llvm::ARM::tCBZ:
      if (posITblock != 0) {
        reason = "CBZ/CBNZ is not supported inside an IT block";
        return false;
      }
      return true;
    case llvm::ARM::t2LDMIA:
    case llvm::ARM::t2LDMDB:
    case llvm::ARM::t2LDMIA_UPD:
    case llvm::ARM::t2LDMDB_UPD:
    case llvm::ARM::tPOP:
      if (UseReg(Reg(REG_PC)).test(patch, llvmcpu) and not notMidITBlock) {
        reason =
            "LDM/POP with pc is not supported in the middle of an IT "
            "block";
        return false;
      }
      return true;
    case llvm::ARM::t2STMIA:
    case llvm::ARM::t2STMDB:
    case llvm::ARM::t2STMIA_UPD:
    case llvm::ARM::t2STMDB_UPD:
      if (UseReg(Reg(REG_PC)).test(patch, llvmcpu)) {
        reason = "STM with pc is not supported in Thumb mode";
        return false;
      }
      return true;
    default:
      break;
  }

  bool usePC = false;
  bool isPC0 = false;
  bool isPC1 = false;
  bool isPC2 = false;
  for (unsigned int i = 0; i < inst.getNumOperands(); i++) {
    const llvm::MCOperand &op = inst.getOperand(i);
    if (op.isReg() && op.getReg() == llvm::ARM::PC) {
      usePC = true;
      if (i == 0) {
        isPC0 = true;
      } else if (i == 1) {
        isPC1 = true;
      } else if (i == 2) {
        isPC2 = true;
      } else {
        reason = "No instruction support PC as operand greater than 2.";
        return false;
      }
    }
  }
  if (not usePC) {
    return true;
  }

  if (inst.getOpcode() == llvm::ARM::t2LDRpci) {
    if (isPC1) {
      reason = "PC is not supported as operand 1.";
      return false;
    }
    if (isPC2) {
      reason = "PC is not supported as operand 2.";
      return false;
    }
    if (not notMidITBlock) {
      reason =
          "This instruction is not supported in the middle of an IT "
          "block";
      return false;
    }
    return true;
  }

  if (PCInst1Dst3Src().test(patch, llvmcpu)) {
    if (isPC1) {
      reason = "PC is not supported as operand 1.";
      return false;
    }
    if (isPC0 and isPC2) {
      if (not notMidITBlock) {
        reason =
            "This instruction is not supported in the middle of an IT "
            "block";
        return false;
      }
      return true;
    }
    reason = isPC0 ? "PC is not supported as operand 0 without operand 2."
                   : "PC is not supported as operand 2 without operand 0.";
    return false;
  }

  if (PCInst1Dst2SrcOR3Src().test(patch, llvmcpu)) {
    if (isPC0 and isPC1 and not isPC2) {
      if (not notMidITBlock) {
        reason =
            "This instruction is not supported in the middle of an IT "
            "block";
        return false;
      }
      return true;
    }
    if (isPC2 and not isPC0 and not isPC1) {
      return true;
    }
    if (isPC2) {
      reason =
          "PC is not supported as operand 2 when operand 0/1 is also "
          "pc.";
      return false;
    }
    reason = isPC0 ? "PC is not supported as operand 0 without operand 1."
                   : "PC is not supported as operand 1 without operand 0.";
    return false;
  }

  if (PCInst2Op().test(patch, llvmcpu)) {
    if (isPC2) {
      reason = "PC is not supported as operand 2.";
      return false;
    } else if (isPC0 and not notMidITBlock) {
      reason =
          "This instruction is not supported in the middle of an IT "
          "block";
      return false;
    }
    return true;
  }

  if (PCInst1OpDest().test(patch, llvmcpu)) {
    if (isPC1) {
      reason = "PC is not supported as operand 1.";
      return false;
    } else if (isPC2) {
      reason = "PC is not supported as operand 2.";
      return false;
    }
    if (not notMidITBlock) {
      reason = "This instruction is not supported in the middle of an IT block";
      return false;
    }
    return true;
  }

  if (PCInst1OpSrcOff1().test(patch, llvmcpu)) {
    if (isPC0) {
      reason = "PC is not supported as operand 0.";
      return false;
    } else if (isPC2) {
      reason = "PC is not supported as operand 2.";
      return false;
    }
    return true;
  }

  if (PCInst1OpSrcOff2().test(patch, llvmcpu)) {
    if (isPC0) {
      reason = "PC is not supported as operand 0.";
      return false;
    } else if (isPC1) {
      reason = "PC is not supported as operand 1.";
      return false;
    }
    return true;
  }

  reason = "PC register is not supported for this instruction";
  return false;
}

} // namespace Thumb

} // namespace

PatchRuleAssembly::PatchRuleAssembly(Options opts)
    : patchRulesARM(A32::getARMPatchRules(opts)),
      patchRulesThumb(Thumb::getThumbPatchRules(opts)), options(opts),
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
    patchRulesARM = A32::getARMPatchRules(opts);
    patchRulesThumb = Thumb::getThumbPatchRules(opts);
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

  QBDI_REQUIRE_ABORT(0 < patch.metadata.inst.getNumOperands(),
                     "Invalid instruction {}", patch);
  QBDI_REQUIRE_ABORT(patch.metadata.inst.getOperand(0).isReg(),
                     "Unexpected operand type {}", patch);

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

PatchRuleResult PatchRuleAssembly::generate(const llvm::MCInst &inst,
                                            rword address, uint32_t instSize,
                                            const LLVMCPU &llvmcpu,
                                            std::vector<Patch> &patchList,
                                            const char *&unsupportedReason) {
  if (llvmcpu.getCPUMode() == CPUMode::ARM) {
    return generateARM(inst, address, instSize, llvmcpu, patchList,
                       unsupportedReason);
  } else {
    return generateThumb(inst, address, instSize, llvmcpu, patchList,
                         unsupportedReason);
  }
}

PatchRuleResult PatchRuleAssembly::generateARM(const llvm::MCInst &inst,
                                               rword address, uint32_t instSize,
                                               const LLVMCPU &llvmcpu,
                                               std::vector<Patch> &patchList,
                                               const char *&unsupportedReason) {

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

  if (not A32::isSupportedInstruction(instPatch, unsupportedReason)) {
    return PatchRuleResult::UNSUPPORTED;
  }

  for (uint32_t j = 0; j < patchRulesARM.size(); j++) {
    if (patchRulesARM[j].canBeApplied(instPatch, llvmcpu)) {
      QBDI_DEBUG("Patch ARM rule {} applied", j);

      patchRulesARM[j].apply(instPatch, llvmcpu);
      patchList.push_back(std::move(instPatch));
      Patch &patch = patchList.back();

      if (patch.metadata.modifyPC) {
        reset();
        return PatchRuleResult::VALID_END_BB;
      } else {
        return PatchRuleResult::VALID;
      }
    }
  }
  return PatchRuleResult::UNSUPPORTED;
}

PatchRuleResult
PatchRuleAssembly::generateThumb(const llvm::MCInst &inst, rword address,
                                 uint32_t instSize, const LLVMCPU &llvmcpu,
                                 std::vector<Patch> &patchList,
                                 const char *&unsupportedReason) {

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
    QBDI_REQUIRE_ABORT(itRemainingInst == 0,
                       "IT instruction cannot be inside another IT block {}",
                       instPatch);
    QBDI_REQUIRE_ABORT(2 == inst.getNumOperands(), "Invalid instruction {}",
                       instPatch);
    QBDI_REQUIRE_ABORT(inst.getOperand(0).isImm(), "Unexpected operand type {}",
                       instPatch);
    QBDI_REQUIRE_ABORT(inst.getOperand(1).isImm(), "Unexpected operand type {}",
                       instPatch);
    unsigned mask = inst.getOperand(1).getImm();
    itRemainingInst = Thumb::getITBlockLength(mask, instPatch);
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
    QBDI_REQUIRE_ABORT(instPatch.metadata.archMetadata.cond == itCond[0],
                       "Condition doesn't match the last IT condition {}",
                       instPatch);
  }
  if (not Thumb::isSupportedInstruction(instPatch, unsupportedReason)) {
    return PatchRuleResult::UNSUPPORTED;
  }

  for (uint32_t j = 0; j < patchRulesThumb.size(); j++) {
    if (patchRulesThumb[j].canBeApplied(instPatch, llvmcpu)) {
      QBDI_DEBUG("Patch Thumb rule {} applied", j);

      if (inst.getOpcode() != llvm::ARM::t2IT && itRemainingInst > 0) {
        itRemainingInst--;
        itCond = {itCond[1], itCond[2], itCond[3], 0};
      }

      patchRulesThumb[j].apply(instPatch, llvmcpu);
      patchList.push_back(std::move(instPatch));
      Patch &patch = patchList.back();

      if (patch.metadata.modifyPC) {
        QBDI_REQUIRE_ABORT(itRemainingInst == 0,
                           "Modify PC before the end of ItBlock {}", instPatch);
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
  if (llvmcpu.getCPUMode() == CPUMode::Thumb) {
    if (itRemainingInst > 0) {
      const unsigned maxPop = 4 + 1 - itRemainingInst;
      unsigned popped = 0;
      bool foundIT = false;
      while (popped < maxPop && patchList.size() > 0 && not foundIT) {
        const llvm::MCInst &lastInst = patchList.back().metadata.inst;
        foundIT = (lastInst.getOpcode() == llvm::ARM::t2IT);
        unsigned itBlockLength = 0;
        if (foundIT) {
          itBlockLength = Thumb::getITBlockLength(
              lastInst.getOperand(1).getImm(), patchList.back());
        }
        patchList.pop_back();
        popped++;
        if (foundIT && (popped + itRemainingInst != itBlockLength + 1)) {
          QBDI_ERROR(
              "Inconsistent IT block state: popped {} instruction(s) + {} "
              "remaining != block length {} + 1",
              popped, itRemainingInst, itBlockLength);
          return false;
        }
      }
      if (not foundIT) {
        QBDI_ERROR(
            "Failed to find the IT instruction of the current incomplete IT "
            "block within the last {} instructions",
            maxPop);
        return false;
      }
    }
  }
  reset();
  return true;
}

} // namespace QBDI
