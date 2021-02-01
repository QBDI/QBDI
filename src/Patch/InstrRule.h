/*
 * This file is part of QBDI.
 *
 * Copyright 2017 Quarkslab
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

#include <memory>
#include <vector>

#include "Patch/PatchCondition.h"
#include "Patch/PatchGenerator.h"
#include "Patch/PatchUtils.h"

#include "Callback.h"

namespace llvm {
  class MCInstrInfo;
  class MCRegisterInfo;
}

namespace QBDI {

class Assembly;
class Patch;
class PatchGenerator;

/*! An instrumentation rule written in PatchDSL.
*/
class InstrRule {

    protected:

    // priority of the rule.
    // The rule with the lesser priority will be applied first
    int priority;

    public:

    InstrRule(int priority = 0) : priority(priority) {}

    virtual ~InstrRule() = default;

    // virtual copy constructor used to duplicate the object
    virtual std::unique_ptr<InstrRule> clone() const =0;

    virtual RangeSet<rword> affectedRange() const =0;

    inline int getPriority() const { return priority; };

    inline void setPriority(int priority) { this->priority = priority; };

    inline virtual void changeVMInstanceRef(VMInstanceRef vminstance) {};

    /*! Determine wheter this rule have to be apply on this Path and instrument if needed.
     *
     * @param[in] patch     The current patch to instrument.
     * @param[in] MCII      A LLVM::MCInstrInfo classes used for internal architecture specific
     *                      queries.
     * @param[in] MRI       A LLVM::MCRegisterInfo classes used for internal architecture specific
     *                      queries.
     * @param[in] assemby   Assembly object to generate InstAnalysis
    */
    virtual bool tryInstrument(Patch &patch, const llvm::MCInstrInfo* MCII, const llvm::MCRegisterInfo* MRI,
                               const Assembly* assembly) const =0;

    /*! Instrument a patch by evaluating its generators on the current context. Also handles the
     *  temporary register management for this patch.
     *
     * @param[in] patch  The current patch to instrument.
     * @param[in] MCII   A LLVM::MCInstrInfo classes used for internal architecture specific
     *                   queries.
     * @param[in] MRI    A LLVM::MCRegisterInfo classes used for internal architecture specific
     *                   queries.
    */
    void instrument(Patch &patch, const llvm::MCInstrInfo* MCII, const llvm::MCRegisterInfo* MRI,
                    const PatchGenerator::UniquePtrVec& patchGen, bool breakToHost, InstPosition position) const;
};


class InstrRuleBasic : public AutoUnique<InstrRule, InstrRuleBasic> {

    PatchCondition::UniquePtr     condition;
    PatchGenerator::UniquePtrVec  patchGen;
    InstPosition    position;
    bool            breakToHost;

    public:

    /*! Allocate a new instrumentation rule with a condition, a list of generators, an
     *  instrumentation position and a breakToHost request.
     *
     * @param[in] condition    A PatchCondition which determine wheter or not this PatchRule
     *                         applies.
     * @param[in] patchGen     A vector of PatchGenerator which will produce the patch instructions.
     * @param[in] position     An enum indicating wether this instrumentation should be positioned
     *                         before the instruction or after it.
     * @param[in] breakToHost  A boolean determining whether this instrumentation should end with
     *                         a break to host (in the case of a callback for example).
    */
    InstrRuleBasic(PatchCondition::UniquePtr&& condition, PatchGenerator::UniquePtrVec&& patchGen,
                   InstPosition position, bool breakToHost, int priority = 0)
        : AutoUnique<InstrRule, InstrRuleBasic>(priority), condition(std::forward<PatchCondition::UniquePtr>(condition)),
        patchGen(std::forward<PatchGenerator::UniquePtrVec>(patchGen)), position(position), breakToHost(breakToHost) {}

    ~InstrRuleBasic() override = default;

    inline std::unique_ptr<InstrRule> clone() const override {
        return InstrRuleBasic::unique(condition->clone(), cloneVec(patchGen), position, breakToHost, priority);
    };

    inline InstPosition getPosition() const { return position; }

    inline RangeSet<rword> affectedRange() const override {
        return condition->affectedRange();
    }

    /*! Determine wheter this rule applies by evaluating this rule condition on the current
     *  context.
     *
     * @param[in] patch  A patch containing the current context.
     * @param[in] MCII   An LLVM MC instruction info context.
     *
     * @return True if this instrumentation condition evaluate to true on this patch.
    */
    bool canBeApplied(const Patch &patch, const llvm::MCInstrInfo* MCII) const;

    inline bool tryInstrument(Patch &patch, const llvm::MCInstrInfo* MCII, const llvm::MCRegisterInfo* MRI,
                              const Assembly* assembly) const override {
        if (canBeApplied(patch, MCII)) {
            instrument(patch, MCII, MRI, patchGen, breakToHost, position);
            return true;
        }
        return false;
    }
};

typedef const std::vector<std::unique_ptr<PatchGenerator>>& (*PatchGenMethod)(Patch &patch, const llvm::MCInstrInfo* MCII,
                                                            const llvm::MCRegisterInfo* MRI);

class InstrRuleDynamic : public AutoUnique<InstrRule, InstrRuleDynamic> {

    PatchCondition::UniquePtr     condition;
    PatchGenMethod                patchGenMethod;
    InstPosition                  position;
    bool                          breakToHost;

public:

    /*! Allocate a new instrumentation rule with a condition, a method to generate patch instruction, an
     *  instrumentation position and a breakToHost request.
     *
     * @param[in] condition        A PatchCondition which determine wheter or not this PatchRule
     *                             applies.
     * @param[in] patchGenMethod   A Method that will be called to generate the patch.
     * @param[in] position         An enum indicating wether this instrumentation should be positioned
     *                             before the instruction or after it.
     * @param[in] breakToHost      A boolean determining whether this instrumentation should end with
     *                             a break to host (in the case of a callback for example).
    */
    InstrRuleDynamic(PatchCondition::UniquePtr&& condition, PatchGenMethod patchGenMethod,
                     InstPosition position, bool breakToHost, int priority = 0)
        : AutoUnique<InstrRule, InstrRuleDynamic>(priority), condition(std::forward<PatchCondition::UniquePtr>(condition)),
        patchGenMethod(patchGenMethod), position(position), breakToHost(breakToHost) {}

    ~InstrRuleDynamic() override = default;

    inline std::unique_ptr<InstrRule> clone() const override {
        return InstrRuleDynamic::unique(condition->clone(), patchGenMethod, position, breakToHost, priority);
    };

    inline InstPosition getPosition() const { return position; }

    inline RangeSet<rword> affectedRange() const override {
        return condition->affectedRange();
    }

    /*! Determine wheter this rule applies by evaluating this rule condition on the current
     *  context.
     *
     * @param[in] patch  A patch containing the current context.
     * @param[in] MCII   An LLVM MC instruction info context.
     *
     * @return True if this instrumentation condition evaluate to true on this patch.
    */
    bool canBeApplied(const Patch &patch, const llvm::MCInstrInfo* MCII) const;

    inline bool tryInstrument(Patch &patch, const llvm::MCInstrInfo* MCII, const llvm::MCRegisterInfo* MRI,
                              const Assembly* assembly) const override {
        if (canBeApplied(patch, MCII)) {
            instrument(patch, MCII, MRI, patchGenMethod(patch, MCII, MRI), breakToHost, position);
            return true;
        }
        return false;
    }
};

class InstrRuleUser : public AutoClone<InstrRule, InstrRuleUser> {

    InstrumentCallback cbk;
    AnalysisType analysisType;
    void* cbk_data;
    VMInstanceRef vm;
    RangeSet<rword> range;

    public:

    InstrRuleUser(InstrumentCallback cbk, AnalysisType analysisType,
                  void* cbk_data, VMInstanceRef vm, RangeSet<rword> range,
                  int priority = 0)
        : AutoClone<InstrRule, InstrRuleUser>(priority), cbk(cbk),
        analysisType(analysisType), cbk_data(cbk_data), vm(vm), range(range) {}

    ~InstrRuleUser() override = default;

    inline void changeVMInstanceRef(VMInstanceRef vminstance) override {
        vm = vminstance;
    };

    inline RangeSet<rword> affectedRange() const override {
        return range;
    }

    bool tryInstrument(Patch &patch, const llvm::MCInstrInfo* MCII, const llvm::MCRegisterInfo* MRI,
                       const Assembly* assembly) const override;
};

}

#endif
