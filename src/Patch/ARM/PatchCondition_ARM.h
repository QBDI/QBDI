/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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
#ifndef PATCHCONDITION_ARM_H
#define PATCHCONDITION_ARM_H

#include "Patch/PatchCondition.h"

namespace QBDI {

class HasCond : public AutoClone<PatchCondition, HasCond> {

public:
  /*! Return true if the instruction has a condition and may be execute as a NOP
   * depending of the flags
   */
  HasCond(){};

  bool test(const Patch &patch, const LLVMCPU &llvmcpu) const override;
};

class OperandIs : public AutoClone<PatchCondition, OperandIs> {
  unsigned int position;
  RegLLVM reg;
  Constant imm;

  enum { RegType, ImmType } type;

public:
  /*! Return true if the operand is the expected register
   */
  OperandIs(unsigned int position, RegLLVM reg)
      : position(position), reg(reg), imm(0), type(RegType){};

  /*! Return true if the operand is the expected immediate
   */
  OperandIs(unsigned int position, Constant imm)
      : position(position), reg(0), imm(imm), type(ImmType){};

  bool test(const Patch &patch, const LLVMCPU &llvmcpu) const override;
};

class InITBlock : public AutoClone<PatchCondition, InITBlock> {

public:
  /*! Return true if the instruction is in a ITBlock
   */
  InITBlock(){};

  bool test(const Patch &patch, const LLVMCPU &llvmcpu) const override;
};

class LastInITBlock : public AutoClone<PatchCondition, LastInITBlock> {

public:
  /*! Return true if the instruction is in a ITBlock and the last instruction of
   * the block.
   *
   * Return false if the instruction is outside of an ITBlock, or inside but not
   * the last instruction.
   */
  LastInITBlock(){};

  bool test(const Patch &patch, const LLVMCPU &llvmcpu) const override;
};

} // namespace QBDI

#endif
