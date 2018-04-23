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
#ifndef PATCHGENERATOR_H
#define PATCHGENERATOR_H

#include <memory>
#include <vector>

#include "llvm/MC/MCInst.h"

#include "Patch/Patch.h"
#include "Patch/Types.h"
#include "Patch/PatchUtils.h"
#include "Patch/RelocatableInst.h"
#include "Patch/InstTransform.h"

namespace QBDI {

class PatchGenerator {
public:

    using SharedPtr    = std::shared_ptr<PatchGenerator>;
    using SharedPtrVec = std::vector<std::shared_ptr<PatchGenerator>>;

    virtual RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst, rword address,
        rword instSize, CPUMode cpuMode, TempManager *temp_manager, const Patch *toMerge) = 0;

    virtual ~PatchGenerator() {};

    virtual bool modifyPC() { return false; }

    virtual bool doNotInstrument() { return false; }
};

class ModifyInstruction : public PatchGenerator, public AutoAlloc<PatchGenerator, ModifyInstruction>
{
    InstTransform::SharedPtrVec transforms;

public:

    /*! Apply a list of InstTransform to the current instruction and output the result.
     *
     * @param[in] transforms Vector of InstTransform to be applied.
    */
    ModifyInstruction(InstTransform::SharedPtrVec transforms) : transforms(transforms) {};

    /*!
     * Output:
     *   (depends on the current instructions and transforms)
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst, rword address, rword instSize,
        CPUMode cpuMode, TempManager *temp_manager, const Patch *toMerge) {

        llvm::MCInst a(*inst);
        for(auto t: transforms) {
            t->transform(a, address, instSize, temp_manager);
        }

        RelocatableInst::SharedPtrVec out;
        if(toMerge != nullptr) {
            append(out, toMerge->insts);
        }
        out.push_back(NoReloc(a));
        return out;
    }
};

class DoNotInstrument : public PatchGenerator, public AutoAlloc<PatchGenerator, DoNotInstrument>
{

public:

    /*! Adds a "do not instrument" flag to the resulting patch which allows it to skip the
     * instrumentation process of the engine.
    */
    DoNotInstrument() {};

    /*!
     * Output:
     *   (none)
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst, rword address, rword instSize,
        CPUMode cpuMode, TempManager *temp_manager, const Patch *toMerge) {
        return {};
    }

    bool doNotInstrument() { return true; }
};

}

#endif
