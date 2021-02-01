/*
 * This file is part of QBDI.
 *
 * Copyright 2020 Quarkslab
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

#include "Patch/PatchGenerator.h"
#include "Patch/PatchRule.h"

namespace QBDI {

PatchRule::PatchRule(PatchCondition::UniqPtr&& condition, std::vector<std::unique_ptr<PatchGenerator>>&& generators)
        : condition(std::move(condition)), generators(std::move(generators)) {};

PatchRule::~PatchRule() = default;

PatchRule::PatchRule(PatchRule&&) = default;

Patch PatchRule::generate(const llvm::MCInst &inst, rword address,
    rword instSize, const llvm::MCInstrInfo* MCII, const llvm::MCRegisterInfo* MRI, const Patch* toMerge) const {

    Patch patch(inst, address, instSize);
    if(toMerge != nullptr) {
        patch.metadata.address = toMerge->metadata.address;
        patch.metadata.instSize += toMerge->metadata.instSize;
    }
    TempManager temp_manager(inst, MCII, MRI);
    bool modifyPC = false;
    bool merge = false;

    for(const auto &g : generators) {
        patch.append(g->generate(&inst, address, instSize, MCII, &temp_manager, toMerge));
        modifyPC |= g->modifyPC();
        merge |= g->doNotInstrument();
    }
    patch.setMerge(merge);
    patch.setModifyPC(modifyPC);

    Reg::Vec used_registers = temp_manager.getUsedRegisters();

    for(unsigned int i = 0; i < used_registers.size(); i++) {
        patch.prepend(SaveReg(used_registers[i], Offset(used_registers[i])));
    }

    for(unsigned int i = 0; i < used_registers.size(); i++) {
        patch.append(LoadReg(used_registers[i], Offset(used_registers[i])));
    }

    return patch;
}

} // namespace QBDI
