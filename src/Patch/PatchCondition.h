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
#ifndef PATCHCONDITION_H
#define PATCHCONDITION_H

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Patch/PatchUtils.h"
#include "Patch/Types.h"

#include "QBDI/Range.h"
#include "QBDI/State.h"

namespace llvm {
class MCInst;
} // namespace llvm

namespace QBDI {

class LLVMCPU;

class PatchCondition {
public:
  using UniquePtr = std::unique_ptr<PatchCondition>;
  using UniquePtrVec = std::vector<std::unique_ptr<PatchCondition>>;

  virtual UniquePtr clone() const = 0;

  virtual bool test(const llvm::MCInst &inst, rword address, rword instSize,
                    const LLVMCPU &llvmcpu) const = 0;

  virtual RangeSet<rword> affectedRange() const {
    RangeSet<rword> r;
    r.add(Range<rword>(0, (rword)-1));
    return r;
  }

  virtual ~PatchCondition() = default;
};

class MnemonicIs : public AutoClone<PatchCondition, MnemonicIs> {
  std::string mnemonic;

public:
  /*! Return true if the mnemonic of the current instruction is equal to
   * Mnemonic.
   *
   * @param[in] mnemonic   A null terminated instruction mnemonic (using LLVM
   * style)
   */
  MnemonicIs(const char *mnemonic) : mnemonic(mnemonic){};

  bool test(const llvm::MCInst &inst, rword address, rword instSize,
            const LLVMCPU &llvmcpu) const override;
};

class OpIs : public AutoClone<PatchCondition, OpIs> {
  unsigned int op;

public:
  /*! Return true if the instruction opcode is equal to op.
   *
   * @param[in] op LLVM instruction opcode ID.
   */
  OpIs(unsigned int op) : op(op){};

  bool test(const llvm::MCInst &inst, rword address, rword instSize,
            const LLVMCPU &llvmcpu) const override;
};

class UseReg : public AutoClone<PatchCondition, UseReg> {
  Reg reg;

public:
  /*!  Return true if the instruction uses reg as one of its operand.
   *
   * @param[in] reg The register to compare with.
   */
  UseReg(Reg reg) : reg(reg){};

  bool test(const llvm::MCInst &inst, rword address, rword instSize,
            const LLVMCPU &llvmcpu) const override;
};

class InstructionInRange
    : public AutoClone<PatchCondition, InstructionInRange> {
  Range<rword> range;

public:
  /*! Return true if the instruction address is in the range [start, end[ (end
   * not included).
   *
   * @param[in] start Start of the range.
   * @param[in] end   End of the range (not included).
   */
  InstructionInRange(Constant start, Constant end) : range(start, end){};

  bool test(const llvm::MCInst &inst, rword address, rword instSize,
            const LLVMCPU &llvmcpu) const override {
    return range.contains(Range<rword>(address, address + instSize));
  }

  RangeSet<rword> affectedRange() const override {
    RangeSet<rword> r;
    r.add(range);
    return r;
  }
};

class AddressIs : public AutoClone<PatchCondition, AddressIs> {
  rword breakpoint;

public:
  /*! Return true if on specified address
   */
  AddressIs(rword breakpoint) : breakpoint(breakpoint){};

  bool test(const llvm::MCInst &inst, rword address, rword instSize,
            const LLVMCPU &llvmcpu) const override {
    return address == breakpoint;
  }

  RangeSet<rword> affectedRange() const override {
    RangeSet<rword> r;
    r.add(Range<rword>(breakpoint, breakpoint + 1));
    return r;
  }
};

class And : public AutoUnique<PatchCondition, And> {
  PatchCondition::UniquePtrVec conditions;

public:
  /*! Return true if every PatchCondition of the list conditions return true
   * (lazy evaluation).
   *
   * @param[in] conditions List of conditions to evaluate.
   */
  And(PatchCondition::UniquePtrVec &&conditions)
      : conditions(std::forward<PatchCondition::UniquePtrVec>(conditions)){};

  bool test(const llvm::MCInst &inst, rword address, rword instSize,
            const LLVMCPU &llvmcpu) const override {
    return std::all_of(conditions.begin(), conditions.end(),
                       [&](const PatchCondition::UniquePtr &cond) {
                         return cond->test(inst, address, instSize, llvmcpu);
                       });
  }

  RangeSet<rword> affectedRange() const override {
    RangeSet<rword> r;
    r.add(Range<rword>(0, (rword)-1));
    for (unsigned int i = 0; i < conditions.size(); i++) {
      r.intersect(conditions[i]->affectedRange());
    }
    return r;
  }

  inline std::unique_ptr<PatchCondition> clone() const override {
    return And::unique(cloneVec(conditions));
  };
};

class Or : public AutoUnique<PatchCondition, Or> {
  PatchCondition::UniquePtrVec conditions;

public:
  /*! Return true if one of the PatchCondition of the list conditions return
   * true (lazy evaluation).
   *
   * @param[in] conditions List of conditions to evaluate.
   */
  Or(PatchCondition::UniquePtrVec &&conditions)
      : conditions(std::forward<PatchCondition::UniquePtrVec>(conditions)){};

  bool test(const llvm::MCInst &inst, rword address, rword instSize,
            const LLVMCPU &llvmcpu) const override {
    return std::any_of(conditions.begin(), conditions.end(),
                       [&](const PatchCondition::UniquePtr &cond) {
                         return cond->test(inst, address, instSize, llvmcpu);
                       });
  }

  RangeSet<rword> affectedRange() const override {
    RangeSet<rword> r;
    for (unsigned int i = 0; i < conditions.size(); i++) {
      r.add(conditions[i]->affectedRange());
    }
    return r;
  }

  inline std::unique_ptr<PatchCondition> clone() const override {
    return Or::unique(cloneVec(conditions));
  };
};

class Not : public AutoUnique<PatchCondition, Not> {
  PatchCondition::UniquePtr condition;

public:
  /*! Return the logical inverse of condition.
   *
   * @param[in] condition Condition to evaluate.
   */
  Not(PatchCondition::UniquePtr &&condition)
      : condition(std::forward<PatchCondition::UniquePtr>(condition)){};

  bool test(const llvm::MCInst &inst, rword address, rword instSize,
            const LLVMCPU &llvmcpu) const override {
    return !condition->test(inst, address, instSize, llvmcpu);
  }

  inline std::unique_ptr<PatchCondition> clone() const override {
    return Not::unique(condition->clone());
  };
};

class True : public AutoClone<PatchCondition, True> {
public:
  /*! Return true.
   */
  True(){};

  bool test(const llvm::MCInst &inst, rword address, rword instSize,
            const LLVMCPU &llvmcpu) const override {
    return true;
  }
};

class DoesReadAccess : public AutoClone<PatchCondition, DoesReadAccess> {
public:
  /*! Return true if the instruction read data from memory.
   */
  DoesReadAccess(){};

  bool test(const llvm::MCInst &inst, rword address, rword instSize,
            const LLVMCPU &llvmcpu) const override;
};

class DoesWriteAccess : public AutoClone<PatchCondition, DoesWriteAccess> {
public:
  /*! Return true if the instruction write data to memory.
   */
  DoesWriteAccess(){};

  bool test(const llvm::MCInst &inst, rword address, rword instSize,
            const LLVMCPU &llvmcpu) const override;
};

} // namespace QBDI

#endif
