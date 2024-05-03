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
#ifndef PATCHGENERATOR_AARCH64_H
#define PATCHGENERATOR_AARCH64_H

#include <memory>
#include <stddef.h>
#include <vector>

#include "QBDI/State.h"
#include "Patch/PatchGenerator.h"
#include "Patch/PatchUtils.h"
#include "Patch/Types.h"

namespace QBDI {

class SimulateLink : public AutoClone<PatchGenerator, SimulateLink> {
  Temp temp;

public:
  /*! Simulate the effects of the link operation performed by BL and BLX
   * instructions: the address of the next instruction is copied into the LR
   * register. A temp and a shadow are needed to compute this address.
   *
   * @param[in] temp   Any unused temporary, overwritten by this generator.
   */
  SimulateLink(Temp temp) : temp(temp) {}

  /*! Output:
   *
   * LDR REG64 temp, MEM64 Shadow(IMM64 (address + 4))
   * MOV REG64 LR, REG64 temp
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;

  inline bool modifyPC() const override { return true; }
};

class GetPCOffset : public AutoClone<PatchGenerator, GetPCOffset> {
  Temp temp;
  Operand opdst;
  Constant cst;
  Operand opsrc;
  enum {
    TempConstantType,
    TempOperandType,
    OperandOperandType,
  } type;

public:
  /*! Interpret a constant as a PC relative offset and copy it in a temporary.
   * It can be used to obtain the current value of PC by using a constant of 0.
   *
   * @param[in] temp     A temporary where the value will be copied.
   * @param[in] cst      The constant to be used.
   */
  GetPCOffset(Temp temp, Constant cst)
      : temp(temp), opdst(0), cst(cst), opsrc(0), type(TempConstantType) {}

  /*! Interpret an operand as a PC relative offset and copy it in a temporary.
   * It can be used to obtain jump/call targets or relative memory access
   * addresses.
   *
   * @param[in] temp     A temporary where the value will be copied.
   * @param[in] op       The operand index (relative to the instruction LLVM
   * MCInst representation) to be used.
   */
  GetPCOffset(Temp temp, Operand op)
      : temp(temp), opdst(0), cst(0), opsrc(op), type(TempOperandType) {}

  /*! Interpret an operand as a PC relative offset and copy it in a temporary.
   * It can be used to obtain jump/call targets or relative memory access
   * addresses.
   *
   * @param[in] opdst    The operand where the value will be copied.
   * @param[in] opsrc    The operand index (relative to the instruction LLVM
   * MCInst representation) to be used.
   */
  GetPCOffset(Operand opdst, Operand opsrc)
      : temp(0), opdst(opdst), cst(0), opsrc(opsrc), type(OperandOperandType) {}

  /*! Output:
   *
   * If cst:
   * LDR REG32 temp, MEM32 Shadow(IMM32 (cst + address + 8))
   *
   * If op is an immediate:
   * LDR REG32 temp, MEM32 Shadow(IMM32 (op + address + 8))
   *
   * If op is a register:
   * LDR REG32 temp, MEM32 Shadow(IMM32 (address + 8))
   * ADD REG32 temp, REG32 op
   *
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class SaveX28IfSet : public AutoClone<PatchGenerator, SaveX28IfSet> {

public:
  SaveX28IfSet() {}

  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class CondExclusifLoad : public AutoClone<PatchGenerator, CondExclusifLoad> {
  Temp tmp;

public:
  CondExclusifLoad(Temp tmp) : tmp(tmp) {}

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
   * @param[in] addr      A temporary whith the address of the access.
   * @param[in] index     Index of access to saved when access size > 64
   */
  GetReadValue(Temp temp, Temp addr, size_t index)
      : temp(temp), addr(addr), index(index) {}

  /*! Output:
   *
   * MOV REG64 temp, MEM64 val
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
   * @param[in] addr      A temporary whith the address of the access.
   * @param[in] index     Index of access to saved when access size > 64
   */
  GetWrittenValue(Temp temp, Temp addr, size_t index)
      : temp(temp), addr(addr), index(index) {}

  /*! Output:
   *
   * MOV REG64 temp, MEM64 val
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class GetReadValueX2 : public AutoClone<PatchGenerator, GetReadValueX2> {

  Temp temp;
  Temp temp2;
  Temp addr;
  size_t index;

public:
  /*! Resolve the memory address where the instructions will read its value and
   * copy the value in a temporary. This PatchGenerator is only guaranteed to
   * work before the instruction has been executed.
   *
   * @param[in] temp      A temporary where the first memory value will be
   * copied.
   * @param[in] temp2     A temporary where the second memory value will be
   * copied.
   * @param[in] addr      A temporary whith the address of the access.
   * @param[in] index     Index of access to saved when access size > 64
   */
  GetReadValueX2(Temp temp, Temp temp2, Temp addr, size_t index)
      : temp(temp), temp2(temp2), addr(addr), index(index) {}

  /*! Output:
   *
   * MOV REG64 temp, MEM64 val
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class GetWrittenValueX2 : public AutoClone<PatchGenerator, GetWrittenValueX2> {

  Temp temp;
  Temp temp2;
  Temp addr;
  size_t index;

public:
  /*! Resolve the memory address where the instructions has written its value
   * and copy back the value in a temporary. This PatchGenerator is only
   * guaranteed to work after the instruction has been executed.
   *
   * @param[in] temp      A temporary where the first memory value will be
   * copied.
   * @param[in] temp2     A temporary where the second memory value will be
   * copied.
   * @param[in] addr      A temporary whith the address of the access.
   * @param[in] index     Index of access to saved when access size > 64
   */
  GetWrittenValueX2(Temp temp, Temp temp2, Temp addr, size_t index)
      : temp(temp), temp2(temp2), addr(addr), index(index) {}

  /*! Output:
   *
   * MOV REG64 temp, MEM64 val
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class FullRegisterRestore
    : public PureEval<AutoClone<PatchGenerator, FullRegisterRestore>> {

  bool restoreX28;

public:
  /*! Restore the ScratchRegister
   *  if restoreX28, restore also X28 and save TPIDR_EL0 in the datablock
   *
   * @param[in] restoreX28   Restore also X28
   */
  FullRegisterRestore(bool restoreX28) : restoreX28(restoreX28) {}

  /*! Output:
   *
   * MOV REG64 temp, MEM64 val
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  genReloc(const LLVMCPU &llvmcpu) const override;
};

class FullRegisterReset
    : public PureEval<AutoClone<PatchGenerator, FullRegisterReset>> {

  bool restoreX28;

public:
  /*! Reset the ScratchRegister to the base address
   *  if restoreX28, restore also X28. TPIDR_EL0 is used to keep all register
   *  value
   *
   * @param[in] restoreX28   Restore also X28
   */
  FullRegisterReset(bool restoreX28) : restoreX28(restoreX28) {}

  /*! Output:
   *
   * MOV REG64 temp, MEM64 val
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  genReloc(const LLVMCPU &llvmcpu) const override;
};

class GetAddrAuth : public AutoClone<PatchGenerator, GetAddrAuth> {

  Temp temp;
  Operand op;
  enum {
    TempType,
    OperandType,
  } type;

  bool bypass;

public:
  /*! Generate a Patch to authenticate a pointer and store it to a temp
   *
   * @param[in] temp    the register where to store the value
   * @param[in] bypass  only drop the authantication tag
   */
  GetAddrAuth(Temp temp, bool bypass)
      : temp(temp), op(0), type(TempType), bypass(bypass) {}

  /*! Generate a Patch to authenticate a pointer and store it to a temp
   *
   * @param[in] op      the operand where to store the value
   * @param[in] bypass  only drop the authantication tag
   */
  GetAddrAuth(Operand op, bool bypass)
      : temp(0), op(op), type(OperandType), bypass(bypass) {}

  /*! Output:
   *
   * MOV dest, autia(op(0), op(1))
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class GenBTI : public PureEval<AutoClone<PatchGenerator, GenBTI>> {
public:
  /*! Add BTI instruction is OPT_ENABLE_BTI is enabled
   */
  GenBTI() {}

  /*! Output:
   *
   * BTI
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  genReloc(const LLVMCPU &llvmcpu) const override;
};

} // namespace QBDI

#endif
