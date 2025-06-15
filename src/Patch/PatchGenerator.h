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
#ifndef PATCHGENERATOR_H
#define PATCHGENERATOR_H

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "QBDI/State.h"
#include "Patch/InstTransform.h"
#include "Patch/PatchUtils.h"
#include "Patch/Types.h"

namespace llvm {
class MCInst;
}

namespace QBDI {
class InstTransform;
class LLVMCPU;
class Patch;
class RelocatableInst;
class TempManager;

/* Flags value for PatchGenerator
 */
enum PatchGeneratorFlags {
  None = 0,
  PatchRuleBegin = 0x1,
  PatchRuleEnd = 0x2,
  ModifyInstructionBeginFlags = 0x3,
  ModifyInstructionEndFlags = 0x4,
  ArchSpecificFlags = 0x80
};

class PatchGenerator {
public:
  using UniquePtr = std::unique_ptr<PatchGenerator>;
  using UniquePtrVec = std::vector<std::unique_ptr<PatchGenerator>>;

  virtual std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const = 0;

  virtual ~PatchGenerator() = default;

  virtual std::unique_ptr<PatchGenerator> clone() const = 0;

  virtual inline bool modifyPC() const { return false; }

  virtual inline uint32_t getPreFlags() const {
    return PatchGeneratorFlags::None;
  }
  virtual inline uint32_t getPostFlags() const {
    return PatchGeneratorFlags::None;
  }
};

template <typename T>
class PureEval : public T {
public:
  virtual std::vector<std::unique_ptr<RelocatableInst>>
  genReloc(const LLVMCPU &llvmcpu) const = 0;

  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const final;
};

// Generic Generator available for any target

class ModifyInstruction : public AutoUnique<PatchGenerator, ModifyInstruction> {
  std::vector<std::unique_ptr<InstTransform>> transforms;

public:
  /*! Apply a list of InstTransform to the current instruction and output the
   * result.
   *
   * @param[in] transforms Vector of InstTransform to be applied.
   */
  ModifyInstruction(std::vector<std::unique_ptr<InstTransform>> &&transforms);

  std::unique_ptr<PatchGenerator> clone() const override;

  /*!
   * Output:
   *   (depends on the current instructions and transforms)
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;

  inline uint32_t getPreFlags() const override {
    return PatchGeneratorFlags::ModifyInstructionBeginFlags;
  }
  inline uint32_t getPostFlags() const override {
    return PatchGeneratorFlags::ModifyInstructionEndFlags;
  }
};

class PatchGenFlags
    : public PureEval<AutoClone<PatchGenerator, PatchGenFlags>> {
  uint32_t flags;

public:
  PatchGenFlags(uint32_t flags) : flags(flags){};

  /*!
   * Output:
   *   (none)
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  genReloc(const LLVMCPU &llvmcpu) const override;

  inline uint32_t getPreFlags() const override { return flags; }
};

class GetOperand : public AutoClone<PatchGenerator, GetOperand> {
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
  GetOperand(Temp temp, Operand op)
      : temp(temp), reg(0), op(op), type(TmpType) {}

  /*! Obtain the value of the operand op and copy it's value in a register. If
   * op is an immediate the immediate value is copied, if op is a register the
   * register value is copied.
   *
   * @param[in] reg    The register where the value will be copied.
   * @param[in] op     The operand index (relative to the instruction
   *                   LLVM MCInst representation) to be copied.
   */
  GetOperand(Reg reg, Operand op) : temp(0), reg(reg), op(op), type(RegType) {}

  /*!
   * Output:
   *   MOV REG64 temp, IMM64/REG64 op
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class WriteOperand : public AutoClone<PatchGenerator, WriteOperand> {
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
  WriteOperand(Operand op, Offset offset) : op(op), offset(offset) {}

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

class GetConstant : public AutoClone<PatchGenerator, GetConstant> {
  Temp temp;
  Constant cst;

public:
  /*! Copy a constant in a temporary.
   *
   * @param[in] temp   A temporary where the value will be copied.
   * @param[in] cst    The constant to copy.
   */
  GetConstant(Temp temp, Constant cst) : temp(temp), cst(cst) {}

  /*! Output:
   *
   * MOV REG64 temp, IMM64 cst
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class GetConstantMap : public AutoClone<PatchGenerator, GetConstantMap> {
  Temp temp;
  std::map<unsigned, Constant> opcodeMap;

public:
  /*! Set the opcode of the instruction.
   *
   * @param[in] opcode New opcode to set as the instruction opcode.
   */
  GetConstantMap(Temp temp, std::map<unsigned, Constant> opcodeMap)
      : temp(temp), opcodeMap(opcodeMap) {}

  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class ReadTemp : public AutoClone<PatchGenerator, ReadTemp> {

  Temp temp;
  enum { OffsetType, ShadowType } type;

  Offset offset;
  Shadow shadow;

public:
  /*! Read a temporary value in the data block at the specified offset.
   *
   * @param[in] temp      A temporary to store the shadow value.
   * @param[in] offset    The offset in the data block where the temporary
   *                      will be read.
   */
  ReadTemp(Temp temp, Offset offset)
      : temp(temp), type(OffsetType), offset(offset), shadow(0) {}

  /*! Read a temporary value from the last shadow with the same tag for this
   * instruction.
   *
   * @param[in] temp      A temporary to store the shadow value.
   * @param[in] shadow    The shadow in the data block where the temporary
   *                      will be read.
   */
  ReadTemp(Temp temp, Shadow shadow)
      : temp(temp), type(ShadowType), offset(0), shadow(shadow) {}

  /*! Output:
   *
   * MOV REG64 temp, MEM64 DataBlock[offset]
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class WriteTemp : public AutoClone<PatchGenerator, WriteTemp> {

  Temp temp;
  Offset offset;
  Shadow shadow;
  Operand operand;
  enum { OffsetType, ShadowType, OperandType } type;

public:
  /*! Write a temporary value in the data block at the specified offset. This
   * can be used to overwrite register values in the context part of the data
   * block.
   *
   * @param[in] temp    A temporary which will be written.
   *                    The value of the temporary is unchanged.
   * @param[in] offset  The offset in the data block where the temporary
   *                    will be written.
   */
  WriteTemp(Temp temp, Offset offset)
      : temp(temp), offset(offset), shadow(0), operand(0), type(OffsetType) {}

  /*! Write a temporary value in a shadow in the data block.
   *
   * @param[in] temp      A temporary which will be written.
   *                      The value of the temporary is unchanged.
   * @param[in] shadow    The shadow use to store the value.
   */
  WriteTemp(Temp temp, Shadow shadow)
      : temp(temp), offset(0), shadow(shadow), operand(0), type(ShadowType) {}

  /*! Write a temporary value another register (based on a operand)
   *
   * @param[in] temp      A temporary which will be written.
   *                      The value of the temporary is unchanged.
   * @param[in] operand   The shadow use to store the value.
   */
  WriteTemp(Temp temp, Operand operand)
      : temp(temp), offset(0), shadow(0), operand(operand), type(OperandType) {}

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

class LoadReg : public PureEval<AutoClone<PatchGenerator, LoadReg>> {

  Reg reg;
  Offset offset;

public:
  /*! Load a register from the data block at the specified offset. This can be
   * used to load register values from the context part of the data block.
   *
   * @param[in] reg     A register where the value will be loaded.
   * @param[in] offset  The offset in the data block from where the value will
   * be loaded.
   */
  LoadReg(Reg reg, Offset offset) : reg(reg), offset(offset) {}

  /*! Output:
   *
   * MOV REG64 reg, MEM64 DataBlock[offset]
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  genReloc(const LLVMCPU &llvmcpu) const override;
};

class SaveTemp : public AutoClone<PatchGenerator, SaveTemp> {
  Temp temp;

public:
  /*! Save the value of the temporary register in the datablock.
   * This can be used when an instruction can change the value of the register
   * used as a temp
   *
   * @param[in] temp     A temp to save.
   */
  SaveTemp(Temp temp) : temp(temp) {}

  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class SaveReg : public PureEval<AutoClone<PatchGenerator, SaveReg>> {

  Reg reg;
  Offset offset;

public:
  /*! Save a register in the data block at the specified offset. This can be
   * used to save register values in the context part of the data block.
   *
   * @param[in] reg     A register which will be saved.
   * @param[in] offset  The offset in the data block where the register will be
   * written.
   */
  SaveReg(Reg reg, Offset offset) : reg(reg), offset(offset) {}

  /*! Output:
   *
   * MOV MEM64 DataBlock[offset], REG64 reg
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  genReloc(const LLVMCPU &llvmcpu) const override;

  inline bool modifyPC() const override {
    return offset == Offset(Reg(REG_PC));
  }
};

class CopyReg : public AutoClone<PatchGenerator, CopyReg> {
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
  CopyReg(Temp dest, Reg src)
      : src(src), destReg(0), destTemp(dest), type(Reg2Temp) {}

  /*! Copy a register in a register.
   *
   * @param[in] src    The register which will be copied
   * @param[in] dest   A register where the register will be copied.
   */
  CopyReg(Reg dest, Reg src)
      : src(src), destReg(dest), destTemp(0), type(Reg2Reg) {}

  /*! Output:
   *
   * MOV REG64 temp, REG64 reg
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class CopyTemp : public AutoClone<PatchGenerator, CopyTemp> {
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
  CopyTemp(Temp dest, Temp src)
      : src(src), destTemp(dest), destReg(0), type(Temp2Temp) {}

  /*! Copy a temporary in a register.
   *
   * @param[in] src    The temporary which will be copied
   * @param[in] dest   A register where the temporary will be copied.
   */
  CopyTemp(Reg dest, Temp src)
      : src(src), destTemp(0), destReg(dest), type(Temp2Reg) {}

  /*! Output:
   *
   * MOV REG64 temp, REG64 reg
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

class GetInstId : public AutoClone<PatchGenerator, GetInstId> {

  Temp temp;

public:
  /*! Copy an ExecBlock specific id for the current instruction in a temporary.
   * This id is used to identify the instruction responsible for a callback in
   * the engine and is only meant for internal use.
   *
   * @param[in] temp   A temporary where the id will be copied.
   */
  GetInstId(Temp temp) : temp(temp) {}

  /*! Output:
   *
   * MOV REG64 temp, IMM64 instID
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;
};

// Generic PatchGenerator that must be implemented by each target

class TargetPrologue : public AutoClone<PatchGenerator, TargetPrologue> {

public:
  /*! Generate a label where the prologue can jump
   */
  TargetPrologue() {}

  /*! Output:
   *
   * label
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  generate(const Patch &patch, TempManager &temp_manager) const override;

  std::vector<std::unique_ptr<RelocatableInst>>
  genReloc(const Patch &patch) const;
};

class JmpEpilogue : public PureEval<AutoClone<PatchGenerator, JmpEpilogue>> {

public:
  /*! Generate a jump instruction which target the epilogue of the ExecBlock.
   */
  JmpEpilogue() {}

  /*! Output:
   *
   * JMP Offset(Epilogue)
   */
  std::vector<std::unique_ptr<RelocatableInst>>
  genReloc(const LLVMCPU &llvmcpu) const override;
};

} // namespace QBDI

#endif
