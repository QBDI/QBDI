/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2023 Quarkslab
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
#ifndef PATCHGENERATOR_ARM_H
#define PATCHGENERATOR_ARM_H

#include <array>
#include <memory>
#include <stddef.h>
#include <vector>

#include "QBDI/State.h"
#include "Patch/PatchGenerator.h"
#include "Patch/PatchUtils.h"
#include "Patch/Types.h"

namespace QBDI {

class SetDataBlockAddress
    : public PureEval<AutoClone<PatchGenerator, SetDataBlockAddress>> {
  Reg reg;
  bool setScratchRegister;

public:
  /* Set reg to the address of the Datablock
   */
  SetDataBlockAddress(Reg reg) : reg(reg), setScratchRegister(false) {}

  /* Set the stratch register to the address of the Datablock
   */
  SetDataBlockAddress() : reg(0), setScratchRegister(true) {}

  std::vector<std::unique_ptr<RelocatableInst>>
  genReloc(const LLVMCPU &llvmcpu) const override;
};

class WritePC : public AutoClone<PatchGenerator, WritePC> {
  Temp temp;
  bool dropCond;

public:
  /*! Write PC in the DataBlock. Manage to add or remove the LSB when the
   *  depending of the instruction behavior and the CPU behavior selected.
   *
   * @param[in] temp   The register that containt the new value of PC.
   */
  WritePC(Temp temp, bool dropCond = false) : temp(temp), dropCond(dropCond) {}

  /*! Output:
   *
   * LDR REG32 temp, MEM32 Shadow(IMM32 1)
   * STR REG32 temp, MEM32 DSataBlock[Offset(hostState.exchange)]
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;

  inline bool modifyPC() const override { return true; }
};

class SetExchange : public AutoClone<PatchGenerator, SetExchange> {
  Temp temp;

public:
  /*!
   *
   * @param[in] temp   Any unused temporary, overwritten by this generator.
   */
  SetExchange(Temp temp) : temp(temp) {}

  /*! Output:
   *
   * LDR REG32 temp, MEM32 Shadow(IMM32 1)
   * STR REG32 temp, MEM32 DSataBlock[Offset(hostState.exchange)]
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class GetPCOffset : public AutoClone<PatchGenerator, GetPCOffset> {

  Temp temp;
  Operand opDest;
  Constant cst;
  Operand op;
  enum {
    TmpConstantType,
    TmpOperandType,
    OpOperandType,
  } type;

  bool keepCond;

public:
  /*! Interpret a constant as a RIP relative offset and copy it in a temporary.
   * It can be used to obtain the current value of RIP by using a constant of 0.
   *
   * @param[in] temp     A temporary where the value will be copied.
   * @param[in] cst      The constant to be used.
   * @param[in] keepCond keep the condition of the instruction. If false, use AL
   */
  GetPCOffset(Temp temp, Constant cst, bool keepCond = false)
      : temp(temp), opDest(0), cst(cst), op(0), type(TmpConstantType),
        keepCond(keepCond) {}

  /*! Interpret an operand as a RIP relative offset and copy it in a temporary.
   * It can be used to obtain jump/call targets or relative memory access
   * addresses.
   *
   * @param[in] temp     A temporary where the value will be copied.
   * @param[in] op       The  operand index (relative to the instruction
   *                     LLVM MCInst representation) to be used.
   * @param[in] keepCond keep the condition of the instruction. If false, use AL
   */
  GetPCOffset(Temp temp, Operand op, bool keepCond = false)
      : temp(temp), opDest(0), cst(0), op(op), type(TmpOperandType),
        keepCond(keepCond) {}

  /*! Interpret an operand as a RIP relative offset and copy it in an operand.
   * It can be used to obtain jump/call targets or relative memory access
   * addresses.
   *
   * @param[in] opDest   An operand where the value will be copied.
   * @param[in] op       The operand index (relative to the instruction
   *                     LLVM MCInst representation) to be used.
   * @param[in] keepCond keep the condition of the instruction. If false, use AL
   */
  GetPCOffset(Operand opDest, Operand op, bool keepCond = false)
      : temp(0), opDest(opDest), cst(0), op(op), type(OpOperandType),
        keepCond(keepCond) {}

  /*! Output:
   *
   * If cst:
   * MOV REG64 temp, IMM64 (cst + address)

   * If op is an immediate:
   * MOV REG64 temp, IMM64 (op + address)
   *
  */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class GetNextInstAddr : public AutoClone<PatchGenerator, GetNextInstAddr> {
  Temp temp;
  Reg reg;
  bool keepCond;
  bool invCond;
  enum {
    TmpType,
    RegType,
  } type;

public:
  /*! Get the address of the next instruction
   * It can be used to obtain the current value of RIP by using a constant of 0.
   *
   * @param[in] temp            A temporary where the value will be copied.
   * @param[in] keepCond        keep the condition of the instruction. If false,
   *                            use AL
   * @param[in] invCond         inverse the condition of the instruction.
   *                            an ItPatch
   */
  GetNextInstAddr(Temp temp, bool keepCond = false, bool invCond = false)
      : temp(temp), reg(0), keepCond(keepCond), invCond(invCond),
        type(TmpType) {}

  /*! Get the address of the next instruction
   * It can be used to obtain the current value of RIP by using a constant of 0.
   *
   * @param[in] reg             The register where the value will be copied.
   * @param[in] keepCond        keep the condition of the instruction. If false,
   *                            use AL
   * @param[in] invCond         inverse the condition of the instruction.
   *                            an ItPatch
   */
  GetNextInstAddr(Reg reg, bool keepCond = false, bool invCond = false)
      : temp(0), reg(reg), keepCond(keepCond), invCond(invCond), type(RegType) {
  }

  /*! Output:
   *
   * MOV REG64 temp, IMM64 (EndAddress)
   *
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class GetOperandCC : public AutoClone<PatchGenerator, GetOperandCC> {
  Temp temp;
  Reg reg;
  Operand op;

  enum { TmpType, RegType } type;

public:
  /*! Obtain the value of the operand op and copy it's value in a temporary. If
   * op is an immediate the immediate value is copied, if op is a register the
   * register value is copied.
   *
   * @param[in] temp   A temporary where the value will be copied.
   * @param[in] op     The operand index (relative to the instruction
   *                   LLVM MCInst representation) to be copied.
   */
  GetOperandCC(Temp temp, Operand op)
      : temp(temp), reg(0), op(op), type(TmpType) {}

  /*! Obtain the value of the operand op and copy it's value in a register. If
   * op is an immediate the immediate value is copied, if op is a register the
   * register value is copied.
   *
   * @param[in] reg    The register where the value will be copied.
   * @param[in] op     The operand index (relative to the instruction
   *                   LLVM MCInst representation) to be copied.
   */
  GetOperandCC(Reg reg, Operand op)
      : temp(0), reg(reg), op(op), type(RegType) {}

  /*!
   * Output:
   *   MOV REG64 temp, IMM64/REG64 op
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class CopyRegCC : public AutoClone<PatchGenerator, CopyRegCC> {
  Reg src;
  Reg destReg;
  Temp destTemp;

  enum {
    Reg2Temp,
    Reg2Reg,
  } type;

public:
  /*! Copy a register in a temporary.
   *
   * @param[in] dest   A temporary where the register will be copied.
   * @param[in] src    The register which will be copied
   */
  CopyRegCC(Temp dest, Reg src)
      : src(src), destReg(0), destTemp(dest), type(Reg2Temp) {}

  /*! Copy a register in a register.
   *
   * @param[in] src    The register which will be copied
   * @param[in] dest   A register where the register will be copied.
   */
  CopyRegCC(Reg dest, Reg src)
      : src(src), destReg(dest), destTemp(0), type(Reg2Reg) {}

  /*! Output:
   *
   * MOV REG64 dest, REG64 src
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class WriteTempCC : public AutoClone<PatchGenerator, WriteTempCC> {

  Temp temp;
  Offset offset;

public:
  /*! Write a temporary value in the data block at the specified offset if the
   * condition of the instruction is reached. This can be used to overwrite
   * register values in the context part of the data block.
   *
   * @param[in] temp    A temporary which will be written.
   *                    The value of the temporary is unchanged.
   * @param[in] offset  The offset in the data block where the temporary
   *                    will be written.
   */
  WriteTempCC(Temp temp, Offset offset) : temp(temp), offset(offset) {}

  /*! Output:
   *
   * MOV MEM64 DataBlock[offset], REG64 temp
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;

  inline bool modifyPC() const override {
    return offset == Offset(Reg(REG_PC));
  }
};

class WriteOperandCC : public AutoClone<PatchGenerator, WriteOperandCC> {
  Operand op;
  Offset offset;

public:
  /*! Obtain the value of the operand op and copy it's value to the Datablock
   * Only Register operand is supported
   *
   * @param[in] op      The operand index (relative to the instruction
   *                    LLVM MCInst representation) to be copied.
   * @param[in] offset  The offset in the data block where the operand
   *                    will be written.
   */
  WriteOperandCC(Operand op, Offset offset) : op(op), offset(offset) {}

  /*!
   * Output:
   *   MOV REG64 temp, IMM64/REG64 op
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;

  inline bool modifyPC() const override {
    return offset == Offset(Reg(REG_PC));
  }
};

class CopyTempCC : public AutoClone<PatchGenerator, CopyTempCC> {
  Temp src;
  Temp destTemp;
  Reg destReg;

  enum {
    Temp2Temp,
    Temp2Reg,
  } type;

public:
  /*! Copy a temporary in a temporary.
   *
   * @param[in] src    The temporary which will be copied
   * @param[in] dest   A temporary where the temporary will be copied.
   */
  CopyTempCC(Temp dest, Temp src)
      : src(src), destTemp(dest), destReg(0), type(Temp2Temp) {}

  /*! Copy a temporary in a register.
   *
   * @param[in] src    The temporary which will be copied
   * @param[in] dest   A register where the temporary will be copied.
   */
  CopyTempCC(Reg dest, Temp src)
      : src(src), destTemp(0), destReg(dest), type(Temp2Reg) {}

  /*! Output:
   *
   * MOV REG64 dest, REG64 src
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class AddOperandToTemp : public AutoClone<PatchGenerator, AddOperandToTemp> {
  Temp temp;
  Operand op;
  Operand op2;

public:
  /*! Add the value of two operands and store the result in the temp register
   *
   * @param[in] temp   A temporary where the value will be copied.
   * @param[in] op     The register operand
   * @param[in] op2     The immediate operand
   */
  AddOperandToTemp(Temp temp, Operand op, Operand op2)
      : temp(temp), op(op), op2(op2) {}

  /*!
   * Output:
   *   MOV REG64 temp, IMM64/REG64 op
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class LDMPatchGen : public AutoClone<PatchGenerator, LDMPatchGen> {
  Temp temp;

public:
  /*! A patchGenerator for LDM instruction that used PC
   *
   * Only one Temp can be used for all PatchGenerator that used the same
   * TempManager.
   */
  LDMPatchGen(Temp temp) : temp(temp) {}

  /*! Output:
   *
   * MOV REG64 temp, REG64 reg
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;

  inline bool modifyPC() const override { return true; }
};

class STMPatchGen : public AutoClone<PatchGenerator, STMPatchGen> {
  Temp temp;

public:
  /*! A patchGenerator for STM instruction that used PC
   *
   * Only one Temp can be used for all PatchGenerator that used the same
   * TempManager.
   */
  STMPatchGen(Temp temp) : temp(temp) {}

  /*! Output:
   *
   * MOV REG64 temp, REG64 reg
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class GetReadAddress : public AutoClone<PatchGenerator, GetReadAddress> {

  Temp temp;

public:
  /*! Resolve the memory address where the instructions will read its value and
   * copy the address in a temporary. This PatchGenerator is only guaranteed to
   * work before the instruction has been executed.
   *
   * @param[in] temp      A temporary where the memory address will be copied.
   */
  GetReadAddress(Temp temp) : temp(temp) {}

  /*! Output:
   * MOV REG64 temp, REG64 addr
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class GetWrittenAddress : public AutoClone<PatchGenerator, GetWrittenAddress> {

  Temp temp;

public:
  /*! Resolve the memory address where the instructions will write its value and
   * copy the address in a temporary. This PatchGenerator is only guaranteed to
   * work before the instruction has been executed.
   *
   * @param[in] temp      A temporary where the memory address will be copied.
   */
  GetWrittenAddress(Temp temp) : temp(temp) {}

  /*! Output:
   *
   * if stack access:
   * MOV REG64 temp, REG64 addr
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class GetReadValue : public AutoClone<PatchGenerator, GetReadValue> {

  Temp temp;
  Temp addr;
  size_t index;

public:
  /*! Resolve the memory address where the instructions will read its value and
   * copy the value in a temporary. This PatchGenerator is only guaranteed to
   * work before the instruction has been executed.
   *
   * @param[in] temp      A temporary where the memory value will be copied.
   * @param[in] addr      A temporary with the address of the access. If the
   *                      register is != temp, the address is incremented.
   *                      (except with size == 3)
   * @param[in] index     Index of access to saved when access size > 32
   */
  GetReadValue(Temp temp, Temp addr, size_t index)
      : temp(temp), addr(addr), index(index) {}

  /*! Output:
   *
   * MOV REG32 temp, MEM32 val
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class GetWrittenValue : public AutoClone<PatchGenerator, GetWrittenValue> {

  Temp temp;
  Temp addr;

  size_t index;

public:
  /*! Resolve the memory address where the instructions has written its value
   * and copy back the value in a temporary. This PatchGenerator is only
   * guaranteed to work after the instruction has been executed.
   *
   * @param[in] temp      A temporary where the memory value will be copied.
   * @param[in] addr      A temporary with the address of the access. If the
   *                      register is != temp, the address is incremented.
   *                      (except with size == 3)
   * @param[in] index     Index of access to saved when access size > 32
   */
  GetWrittenValue(Temp temp, Temp addr, size_t index)
      : temp(temp), addr(addr), index(index) {}

  /*! Output:
   *
   * MOV REG32 temp, MEM32 val
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class BackupValueX2 : public AutoClone<PatchGenerator, BackupValueX2> {

  Temp temp;
  Temp temp2;
  Temp addr;
  Shadow shadow;
  Shadow shadow2;

public:
  /*! Resolve the memory address where the instructions will read its value and
   * copy the value in a temporary. This PatchGenerator is only guaranteed to
   * work before the instruction has been executed.
   *
   * @param[in] temp      A temporary where the first memory value will be
   *                      copied.
   * @param[in] temp2     A temporary where the second memory value will be
   *                      copied.
   * @param[in] addr      A temporary with the address of the access. (the
   *                      address is incremented by 8)
   */
  BackupValueX2(Temp temp, Temp temp2, Temp addr, Shadow shadow, Shadow shadow2)
      : temp(temp), temp2(temp2), addr(addr), shadow(shadow), shadow2(shadow2) {
  }

  /*! Output:
   *
   * MOV REG32 temp, MEM32 val
   * LDMIA addr!, {temp, temp2}
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class CondExclusifLoad : public AutoClone<PatchGenerator, CondExclusifLoad> {
  Temp temp;
  Temp temp2;

public:
  CondExclusifLoad(Temp temp, Temp temp2) : temp(temp), temp2(temp2) {}

  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class SetCondReachAndJump
    : public AutoUnique<PatchGenerator, SetCondReachAndJump> {

  Temp temp;
  Shadow tag;
  PatchGenerator::UniquePtrVec patchVec;

public:
  /*! This patch allows to jump over PatchGenerator when an instruction has a
   * condition and the condition isn't reach at runtime.
   * If a tag (!= Untagged) is given, the patch will store 0 in the shadow if
   * the condition isn't reach, else 1. If the instruction isn't conditionnal,
   * not shadow will be write and no jump will be inserted
   *
   * @param[in] temp      A temporary
   * @param[in] temp2     A temporary where the second memory value will be
   *                      copied.
   * @param[in] addr      A temporary with the address of the access. (the
   *                      address is incremented by 8)
   */
  SetCondReachAndJump(Temp temp, Shadow tag,
                      PatchGenerator::UniquePtrVec &&patchVec)
      : temp(temp), tag(tag), patchVec(std::move(patchVec)) {}

  PatchGenerator::UniquePtr clone() const override;

  /*! Output:
   *
   *   b.<not <cond>> target
   *   <patchVec>
   * target:
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class ItPatch : public AutoClone<PatchGenerator, ItPatch> {

  std::array<bool, 4> cond;
  unsigned nbcond;

public:
  /*! Add the conditionnal instruction it. The condition will be the one of the
   * current instruction.
   *
   * /!\ If used invert a condition, the instruction should never be within a IT
   * block with the condition AL
   *
   * @param[in] cond0   Invert the condition for the first instruction.
   * @param[in] cond1   Invert the condition for the second instruction.
   *                    If not given, the ItPatch will cover only 1 instruction.
   * @param[in] cond2   Invert the condition for the third instruction.
   *                    If not given, the ItPatch will cover only 2
   *                    instructions.
   * @param[in] cond3   Invert the condition for the fourth instruction.
   *                    If not given, the ItPatch will cover only 3
   * instructions.
   */
  ItPatch(bool cond0) : cond({cond0, false, false, false}), nbcond(1) {}
  ItPatch(bool cond0, bool cond1)
      : cond({cond0, cond1, false, false}), nbcond(2) {}
  ItPatch(bool cond0, bool cond1, bool cond2)
      : cond({cond0, cond1, cond2, false}), nbcond(3) {}
  ItPatch(bool cond0, bool cond1, bool cond2, bool cond3)
      : cond({cond0, cond1, cond2, cond3}), nbcond(4) {}

  /*! Output:
   *
   * It<te|te|te> <cond|invcond>
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class TPopPatchGen : public AutoClone<PatchGenerator, TPopPatchGen> {
  Temp temp;

public:
  /*! A patchGenerator for thumb POP (with PC) instruction
   *
   * Only one Temp can be used for all PatchGenerator that used the same
   * TempManager.
   */
  TPopPatchGen(Temp temp) : temp(temp) {}

  /*! Output:
   *
   * MOV REG64 temp, REG64 reg
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;

  inline bool modifyPC() const override { return true; }
};

class T2LDMPatchGen : public AutoClone<PatchGenerator, T2LDMPatchGen> {
  Temp temp;
  bool writePC;

public:
  /*! A patchGenerator for thumb2 LDM instruction
   *
   * Only one Temp can be used for all PatchGenerator that used the same
   * TempManager.
   */
  T2LDMPatchGen(Temp temp, bool writePC) : temp(temp), writePC(writePC) {}

  /*! Output:
   *
   * MOV REG64 temp, REG64 reg
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;

  inline bool modifyPC() const override { return writePC; }
};

class T2STMPatchGen : public AutoClone<PatchGenerator, T2STMPatchGen> {
  Temp temp;

public:
  /*! A patchGenerator for thumb2 STM instruction
   *
   * Only one Temp can be used for all PatchGenerator that used the same
   * TempManager.
   */
  T2STMPatchGen(Temp temp) : temp(temp) {}

  /*! Output:
   *
   * MOV REG64 temp, REG64 reg
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class T2TBBTBHPatchGen : public AutoClone<PatchGenerator, T2TBBTBHPatchGen> {
  Temp temp1;
  Temp temp2;

public:
  /*! A patchGenerator for thumb2 TBB/TBH instruction
   *
   * Only one Temp can be used for all PatchGenerator that used the same
   * TempManager.
   */
  T2TBBTBHPatchGen(Temp temp1, Temp temp2) : temp1(temp1), temp2(temp2) {}

  /*! Output:
   *
   * MOV REG64 temp, REG64 reg
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;

  inline bool modifyPC() const override { return true; }
};

class T2BXAUTPatchGen : public AutoClone<PatchGenerator, T2BXAUTPatchGen> {

public:
  /*! A patchGenerator for thumb2 BXAUT instruction to perform AUTG operation
   */
  T2BXAUTPatchGen() {}

  /*! Output:
   *
   * AUTG
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

} // namespace QBDI

#endif
