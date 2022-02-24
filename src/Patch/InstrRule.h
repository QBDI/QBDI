/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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
#ifndef INSTRRULE_H
#define INSTRRULE_H

#include <algorithm>
#include <memory>
#include <vector>

#include "Patch/PatchUtils.h"
#include "Patch/Types.h"

#include "QBDI/Callback.h"
#include "QBDI/InstAnalysis.h"
#include "QBDI/Range.h"
#include "QBDI/State.h"

namespace QBDI {

class LLVMCPU;
class Patch;
class PatchCondition;
class PatchGenerator;

using PatchConditionUniquePtr = std::unique_ptr<PatchCondition>;
using PatchGeneratorUniquePtrVec = std::vector<std::unique_ptr<PatchGenerator>>;

/*! An instrumentation rule written in PatchDSL.
 */
class InstrRule {

protected:
  // priority of the rule.
  // The rule with the lesser priority will be applied first
  int priority;

public:
  InstrRule(int priority = PRIORITY_DEFAULT) : priority(priority) {}

  virtual ~InstrRule() = default;

  // virtual copy constructor used to duplicate the object
  virtual std::unique_ptr<InstrRule> clone() const = 0;

  virtual RangeSet<rword> affectedRange() const = 0;

  inline int getPriority() const { return priority; };

  inline void setPriority(int priority) { this->priority = priority; };

  inline virtual void changeVMInstanceRef(VMInstanceRef vminstance){};

  /*! Determine wheter this rule have to be apply on this Path and instrument if
   * needed.
   *
   * @param[in] patch     The current patch to instrument.
   * @param[in] llvmcpu   LLVMCPU object
   */
  virtual bool tryInstrument(Patch &patch, const LLVMCPU &llvmcpu) const = 0;

  /*! Instrument a patch by evaluating its generators on the current context.
   * Also handles the temporary register management for this patch.
   *
   * @param[in] patch       The current patch to instrument.
   * @param[in] patchGen    The list of patchGenerator to apply
   * @param[in] breakToHost Add a break to VM need to be add after the patch
   * @param[in] position    Add the patch before or after the instruction
   * @param[in] priority    The priority of this patch
   * @param[in] tag         The tag for this patch
   */
  void instrument(Patch &patch, const PatchGeneratorUniquePtrVec &patchGen,
                  bool breakToHost, InstPosition position, int priority,
                  RelocatableInstTag tag) const;
};

class InstrRuleBasic : public AutoUnique<InstrRule, InstrRuleBasic> {

  PatchConditionUniquePtr condition;
  PatchGeneratorUniquePtrVec patchGen;
  InstPosition position;
  bool breakToHost;
  RelocatableInstTag tag;

public:
  /*! Allocate a new instrumentation rule with a condition, a list of
   * generators, an instrumentation position and a breakToHost request.
   *
   * @param[in] condition    A PatchCondition which determine wheter or not this
   *                         PatchRule applies.
   * @param[in] patchGen     A vector of PatchGenerator which will produce the
   *                         patch instructions.
   * @param[in] position     An enum indicating wether this instrumentation
   *                         should be positioned before the instruction or
   *                         after it.
   * @param[in] breakToHost  A boolean determining whether this instrumentation
   *                         should end with a break to host (in the case of a
   *                         callback for example).
   * @param[in] priority     Priority of the callback
   * @param[in] tag          A tag for the callback
   */
  InstrRuleBasic(PatchConditionUniquePtr &&condition,
                 PatchGeneratorUniquePtrVec &&patchGen, InstPosition position,
                 bool breakToHost, int priority = PRIORITY_DEFAULT,
                 RelocatableInstTag tag = RelocTagInvalid);

  ~InstrRuleBasic() override;

  std::unique_ptr<InstrRule> clone() const override;

  inline InstPosition getPosition() const { return position; }

  RangeSet<rword> affectedRange() const override;

  /*! Determine wheter this rule applies by evaluating this rule condition on
   * the current context.
   *
   * @param[in] patch     A patch containing the current context.
   * @param[in] llvmcpu   LLVMCPU object
   *
   * @return True if this instrumentation condition evaluate to true on this
   * patch.
   */
  bool canBeApplied(const Patch &patch, const LLVMCPU &llvmcpu) const;

  inline bool tryInstrument(Patch &patch,
                            const LLVMCPU &llvmcpu) const override {
    if (canBeApplied(patch, llvmcpu)) {
      instrument(patch, patchGen, breakToHost, position, priority, tag);
      return true;
    }
    return false;
  }
};

typedef const PatchGeneratorUniquePtrVec &(*PatchGenMethod)(
    Patch &patch, const LLVMCPU &llvmcpu);

class InstrRuleDynamic : public AutoUnique<InstrRule, InstrRuleDynamic> {

  PatchConditionUniquePtr condition;
  PatchGenMethod patchGenMethod;
  InstPosition position;
  bool breakToHost;
  RelocatableInstTag tag;

public:
  /*! Allocate a new instrumentation rule with a condition, a method to generate
   * patch instruction, an instrumentation position and a breakToHost request.
   *
   * @param[in] condition        A PatchCondition which determine wheter or not
   *                             this PatchRule applies.
   * @param[in] patchGenMethod   A Method that will be called to generate the
   *                             patch.
   * @param[in] position         An enum indicating wether this instrumentation
   *                             should be positioned before the instruction or
   *                             after it.
   * @param[in] breakToHost      A boolean determining whether this
   *                             instrumentation should end with a break to
   *                             host (in the case of a callback for example).
   * @param[in] priority         Priority of the callback
   * @param[in] tag              A tag for the callback
   */
  InstrRuleDynamic(PatchConditionUniquePtr &&condition,
                   PatchGenMethod patchGenMethod, InstPosition position,
                   bool breakToHost, int priority = PRIORITY_DEFAULT,
                   RelocatableInstTag tag = RelocTagInvalid);

  ~InstrRuleDynamic() override;

  std::unique_ptr<InstrRule> clone() const override;

  inline InstPosition getPosition() const { return position; }

  RangeSet<rword> affectedRange() const override;

  /*! Determine wheter this rule applies by evaluating this rule condition on
   * the current context.
   *
   * @param[in] patch     A patch containing the current context.
   * @param[in] llvmcpu   LLVMCPU object
   *
   * @return True if this instrumentation condition evaluate to true on this
   * patch.
   */
  bool canBeApplied(const Patch &patch, const LLVMCPU &llvmcpu) const;

  inline bool tryInstrument(Patch &patch,
                            const LLVMCPU &llvmcpu) const override {
    if (canBeApplied(patch, llvmcpu)) {
      instrument(patch, patchGenMethod(patch, llvmcpu), breakToHost, position,
                 priority, tag);
      return true;
    }
    return false;
  }
};

class InstrRuleUser : public AutoClone<InstrRule, InstrRuleUser> {

  InstrRuleCallback cbk;
  AnalysisType analysisType;
  void *cbk_data;
  VMInstanceRef vm;
  RangeSet<rword> range;

public:
  InstrRuleUser(InstrRuleCallback cbk, AnalysisType analysisType,
                void *cbk_data, VMInstanceRef vm, RangeSet<rword> range,
                int priority = 0);

  ~InstrRuleUser() override;

  inline void changeVMInstanceRef(VMInstanceRef vminstance) override {
    vm = vminstance;
  };

  inline RangeSet<rword> affectedRange() const override { return range; }

  bool tryInstrument(Patch &patch, const LLVMCPU &llvmcpu) const override;
};

} // namespace QBDI

#endif
