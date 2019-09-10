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

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
#include "Patch/X86_64/InstrRules_X86_64.h"
#elif defined(QBDI_ARCH_ARM)
#include "Patch/ARM/InstrRules_ARM.h"
#endif

namespace QBDI {

/*! An instrumentation rule written in PatchDSL.
*/
class InstrRule : public AutoAlloc<InstrRule, InstrRule> {

    PatchCondition::SharedPtr     condition;
    PatchGenerator::SharedPtrVec  patchGen;
    InstPosition                  position;
    bool                          breakToHost;

public:

    using SharedPtr    = std::shared_ptr<InstrRule>;
    using SharedPtrVec = std::vector<std::shared_ptr<InstrRule>>;


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
    InstrRule(PatchCondition::SharedPtr condition, PatchGenerator::SharedPtrVec patchGen,
              InstPosition position, bool breakToHost) : condition(condition),
              patchGen(patchGen), position(position), breakToHost(breakToHost) {}

    InstPosition getPosition() { return position; }

    RangeSet<rword> affectedRange() const {
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

    /*! Instrument a patch by evaluating its generators on the current context. Also handles the
     *  temporary register management for this patch.
     *
     * @param[in] patch  The current patch to instrument.
     * @param[in] MCII   A LLVM::MCInstrInfo classes used for internal architecture specific
     *                   queries.
     * @param[in] MRI    A LLVM::MCRegisterInfo classes used for internal architecture specific
     *                   queries.
    */
    void instrument(Patch &patch, llvm::MCInstrInfo* MCII, llvm::MCRegisterInfo* MRI);
};

}

#endif
