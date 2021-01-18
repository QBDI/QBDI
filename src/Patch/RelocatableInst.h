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
#ifndef RELOCATABLEINST_H
#define RELOCATABLEINST_H

#include <memory>
#include <vector>

#include "llvm/MC/MCInst.h"

#include "Patch/Types.h"
#include "ExecBlock/ExecBlock.h"
#include "Patch/PatchUtils.h"

namespace QBDI {

class RelocatableInst {
protected:
    llvm::MCInst inst;

public:

    using SharedPtr    = std::shared_ptr<RelocatableInst>;
    using SharedPtrVec = std::vector<std::shared_ptr<RelocatableInst>>;

    RelocatableInst(llvm::MCInst&& inst) : inst(std::move(inst)) {}

    virtual llvm::MCInst reloc(ExecBlock *exec_block) const =0;

    virtual ~RelocatableInst() = default;
};

class NoReloc : public RelocatableInst, public AutoAlloc<RelocatableInst, NoReloc> {
public:

    NoReloc(llvm::MCInst&& inst) : RelocatableInst(std::move(inst)) {}

    llvm::MCInst reloc(ExecBlock *exec_block) const override {
        return inst;
    }
};

class DataBlockRel : public RelocatableInst, public AutoAlloc<RelocatableInst, DataBlockRel> {
    unsigned int opn;
    rword        offset;

public:
    DataBlockRel(llvm::MCInst&& inst, unsigned int opn, rword offset)
        : RelocatableInst(std::move(inst)), opn(opn), offset(offset) {};

    llvm::MCInst reloc(ExecBlock *exec_block) const override {
        llvm::MCInst res = inst;
        res.getOperand(opn).setImm(offset + exec_block->getDataBlockOffset());
        return res;
    }
};

class DataBlockAbsRel : public RelocatableInst, public AutoAlloc<RelocatableInst, DataBlockAbsRel> {
    unsigned int opn;
    rword        offset;

public:
    DataBlockAbsRel(llvm::MCInst&& inst, unsigned int opn, rword offset)
        : RelocatableInst(std::move(inst)), opn(opn), offset(offset) {};

    llvm::MCInst reloc(ExecBlock *exec_block) const override {
        llvm::MCInst res = inst;
        res.getOperand(opn).setImm(exec_block->getDataBlockBase() + offset);
        return res;
    }
};

class EpilogueRel : public RelocatableInst, public AutoAlloc<RelocatableInst, EpilogueRel> {
    unsigned int opn;
    rword        offset;

public:
    EpilogueRel(llvm::MCInst&& inst, unsigned int opn, rword offset)
        : RelocatableInst(std::move(inst)), opn(opn), offset(offset) {};

    llvm::MCInst reloc(ExecBlock *exec_block) const override {
        llvm::MCInst res = inst;
        res.getOperand(opn).setImm(offset + exec_block->getEpilogueOffset());
        return res;
    }
};

}

#endif
