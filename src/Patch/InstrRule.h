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
    bool canBeApplied(const Patch &patch, llvm::MCInstrInfo* MCII) {
        return condition->test(&patch.metadata.inst, patch.metadata.address, patch.metadata.instSize, MCII);
    }

    /*! Instrument a patch by evaluating its generators on the current context. Also handles the
     *  temporary register management for this patch.
     *
     * @param[in] patch  The current patch to instrument.
     * @param[in] MCII   A LLVM::MCInstrInfo classes used for internal architecture specific
     *                   queries.
     * @param[in] MRI    A LLVM::MCRegisterInfo classes used for internal architecture specific
     *                   queries.
    */
    void instrument(Patch &patch, llvm::MCInstrInfo* MCII, llvm::MCRegisterInfo* MRI) {
        /* The instrument function needs to handle several different cases. An instrumentation can 
         * be either prepended or appended to the patch and, in each case, can trigger a break to 
         * host.
        */
        RelocatableInst::SharedPtrVec instru;
        TempManager tempManager(&patch.metadata.inst, MCII, MRI);

        // Generate the instrumentation code from the original instruction context
        for(PatchGenerator::SharedPtr& g : patchGen) {
            append(instru,
                g->generate(&patch.metadata.inst, patch.metadata.address, patch.metadata.instSize, &tempManager, nullptr)
            );
        }

        // In case we break to the host, we need to ensure the value of PC in the context is 
        // correct. This value needs to be set when instrumenting before the instruction or when 
        // instrumenting after an instruction which does not set PC.
        if(breakToHost) {
            if(position == InstPosition::PREINST || patch.metadata.modifyPC == false) {
                switch(position) {
                    // In PREINST PC is set to the current address
                    case InstPosition::PREINST:
                        append(instru,
                               GetConstant(
                                    Temp(0),
                                    Constant(patch.metadata.address)
                               ).generate(
                                    &patch.metadata.inst,
                                    patch.metadata.address,
                                    patch.metadata.instSize,
                                    &tempManager,
                                    nullptr
                               )
                        );
                        break;
                    // In POSTINST PC is set to the next instruction address
                    case InstPosition::POSTINST:
                        append(instru,
                               GetConstant(
                                    Temp(0),
                                    Constant(patch.metadata.address + patch.metadata.instSize)
                               ).generate(
                                    &patch.metadata.inst,
                                    patch.metadata.address,
                                    patch.metadata.instSize,
                                    &tempManager,
                                    nullptr
                               )
                        );
                        break;
                }
                append(instru, SaveReg(tempManager.getRegForTemp(0), Offset(Reg(REG_PC))));
            }
        }

        // The breakToHost code requires one temporary register. If none were allocated by the
        // instrumentation we thus need to add one.
        if(breakToHost && tempManager.getUsedRegisterNumber() == 0) {
            tempManager.getRegForTemp(Temp(0));
        }
        // Prepend the temporary register saving code to the instrumentation
        Reg::Vec usedRegisters = tempManager.getUsedRegisters();
        for(uint32_t i = 0; i < usedRegisters.size(); i++) {
            prepend(instru, SaveReg(usedRegisters[i], Offset(usedRegisters[i])));
        }

        // In the break to host case the first used register is not restored and instead given to
        // the break to host code as a scratch. It will later be restored by the break to host code.
        if(breakToHost) {
            for(uint32_t i = 1; i < usedRegisters.size(); i++) {
                append(instru, LoadReg(usedRegisters[i], Offset(usedRegisters[i])));
            }
            append(instru, getBreakToHost(usedRegisters[0]));
        }
        // Normal case where we append the temporary register restoration code to the instrumentation
        else {
            for(uint32_t i = 0; i < usedRegisters.size(); i++) {
                append(instru, LoadReg(usedRegisters[i], Offset(usedRegisters[i])));
            }
        }

        // The resulting instrumentation is either appended or prepended as per the InstPosition
        if(position == PREINST) {
            patch.prepend(instru);
        }
        else if(position == POSTINST) {
            patch.append(instru);
        }
    }

};

}

#endif
