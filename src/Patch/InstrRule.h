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

#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"

#include "Patch/PatchUtils.h"
#include "Patch/PatchGenerator.h"
#include "Patch/PatchCondition.h"
#include "Patch/Patch.h"
#include "Platform.h"
#include "Callback.h"

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
#include "Patch/X86_64/InstrRules_X86_64.h"
#elif defined(QBDI_ARCH_ARM)
#include "Patch/ARM/InstrRules_ARM.h"
#endif

namespace QBDI {

typedef enum : uint8_t {
    FIRSTPASS = 0, // the instrumentation rule in the first pass will be near the Patched instruction
    MIDDLEPASS,
    LASTPASS, // the instrumentation rule in the last pass will be the first or the last instruction of the Patch
} InstPass;


/*! An instrumentation rule written in PatchDSL.
*/
class InstrRule {

protected:

    InstPass pass;

public:

    InstrRule(InstPass pass) : pass(pass) {}

    using SharedPtr    = std::shared_ptr<InstrRule>;
    using SharedPtrVec = std::vector<std::shared_ptr<InstrRule>>;

    virtual operator std::shared_ptr<InstrRule>() =0;

    InstPass getPass() { return pass; }

    virtual RangeSet<rword> affectedRange() const =0;

    /*! Determine wheter this rule have to be apply on this Path and instrument if needed.
     *
     * @param[in] patch     The current patch to instrument.
     * @param[in] MCII      A LLVM::MCInstrInfo classes used for internal architecture specific
     *                      queries.
     * @param[in] MRI       A LLVM::MCRegisterInfo classes used for internal architecture specific
     *                      queries.
     * @param[in] assemby   Assembly object to generate InstAnalysis
    */
    virtual bool tryInstrument(Patch &patch, llvm::MCInstrInfo* MCII, llvm::MCRegisterInfo* MRI,
                               Assembly* assembly) =0;

    /*! Instrument a patch by evaluating its generators on the current context. Also handles the
     *  temporary register management for this patch.
     *
     * @param[in] patch  The current patch to instrument.
     * @param[in] MCII   A LLVM::MCInstrInfo classes used for internal architecture specific
     *                   queries.
     * @param[in] MRI    A LLVM::MCRegisterInfo classes used for internal architecture specific
     *                   queries.
    */
    void instrument(Patch &patch, llvm::MCInstrInfo* MCII, llvm::MCRegisterInfo* MRI,
                    PatchGenerator::SharedPtrVec patchGen, bool breakToHost, InstPosition position);
};

class InstrRuleBasic : public InstrRule {

    PatchCondition::SharedPtr     condition;
    PatchGenerator::SharedPtrVec  patchGen;
    InstPosition                  position;
    bool                          breakToHost;

public:

    operator std::shared_ptr<InstrRule>() override {
        return std::shared_ptr<InstrRule>(new InstrRuleBasic(*static_cast<InstrRuleBasic*>(this)));
    }

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
    InstrRuleBasic(PatchCondition::SharedPtr condition, PatchGenerator::SharedPtrVec patchGen,
                   InstPosition position, bool breakToHost, InstPass pass) : InstrRule(pass),
        condition(condition), patchGen(patchGen), position(position), breakToHost(breakToHost) {}

    RangeSet<rword> affectedRange() const override {
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
    bool canBeApplied(const Patch &patch, llvm::MCInstrInfo* MCII);

    bool tryInstrument(Patch &patch, llvm::MCInstrInfo* MCII, llvm::MCRegisterInfo* MRI,
                       Assembly* assembly) override {
        if (canBeApplied(patch, MCII)) {
            instrument(patch, MCII, MRI, patchGen, breakToHost, position);
            return true;
        }
        return false;
    }
};

typedef PatchGenerator::SharedPtrVec (*PatchGenMethod)(Patch &patch, llvm::MCInstrInfo* MCII,
                                                       llvm::MCRegisterInfo* MRI);

class InstrRuleDynamic : public InstrRule {

    PatchCondition::SharedPtr     condition;
    PatchGenMethod                patchGenMethod;
    InstPosition                  position;
    bool                          breakToHost;

public:

    operator std::shared_ptr<InstrRule>() override {
        return std::shared_ptr<InstrRule>(new InstrRuleDynamic(*static_cast<InstrRuleDynamic*>(this)));
    }

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
    InstrRuleDynamic(PatchCondition::SharedPtr condition, PatchGenMethod patchGenMethod,
                     InstPosition position, bool breakToHost, InstPass pass) : InstrRule(pass),
        condition(condition), patchGenMethod(patchGenMethod), position(position), breakToHost(breakToHost) {}

    RangeSet<rword> affectedRange() const override {
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
    bool canBeApplied(const Patch &patch, llvm::MCInstrInfo* MCII);

    bool tryInstrument(Patch &patch, llvm::MCInstrInfo* MCII, llvm::MCRegisterInfo* MRI,
                       Assembly* assembly) override {
        if (canBeApplied(patch, MCII)) {
            PatchGenerator::SharedPtrVec patchGen = patchGenMethod(patch, MCII, MRI);
            instrument(patch, MCII, MRI, patchGen, breakToHost, position);
            return true;
        }
        return false;
    }
};

class InstrRuleUser : public InstrRule {

    InstrumentCallback cbk;
    AnalysisType analysisType;
    void* cbk_data;
    VMInstanceRef vm;
    RangeSet<rword> range;

public:

    InstrRuleUser(InstrumentCallback cbk, AnalysisType analysisType,
                     void* cbk_data, VMInstanceRef vm, RangeSet<rword> range, InstPass pass) :
        InstrRule(pass), cbk(cbk), analysisType(analysisType), cbk_data(cbk_data), vm(vm), range(range) {}

    operator std::shared_ptr<InstrRule>() override {
        return std::shared_ptr<InstrRule>(new InstrRuleUser(*static_cast<InstrRuleUser*>(this)));
    }

    RangeSet<rword> affectedRange() const override {
        return range;
    }

    bool tryInstrument(Patch &patch, llvm::MCInstrInfo* MCII, llvm::MCRegisterInfo* MRI,
                       Assembly* assembly) override;

};

}

#endif
