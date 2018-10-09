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
public:

    using SharedPtr    = std::shared_ptr<RelocatableInst>;
    using SharedPtrVec = std::vector<std::shared_ptr<RelocatableInst>>;

    llvm::MCInst inst;

    RelocatableInst(llvm::MCInst inst) {
        this->inst = inst;
    }

    virtual llvm::MCInst reloc(ExecBlock *exec_block) {
        return inst;
    }

    virtual ~RelocatableInst() {};
};

class NoReloc : public RelocatableInst, public AutoAlloc<RelocatableInst, NoReloc> {
public:

    NoReloc(llvm::MCInst inst) : RelocatableInst(inst) {}

    llvm::MCInst reloc(ExecBlock *exec_block) {
        return inst;
    }
};

class DataBlockRel : public RelocatableInst, public AutoAlloc<RelocatableInst, DataBlockRel> {
    unsigned int opn;
    rword        offset;

public:
    DataBlockRel(llvm::MCInst inst, unsigned int opn, rword offset)
        : RelocatableInst(inst), opn(opn), offset(offset) {};

    llvm::MCInst reloc(ExecBlock *exec_block) {
        inst.getOperand(opn).setImm(offset + exec_block->getDataBlockOffset());
        return inst;
    }
};

class DataBlockAbsRel : public RelocatableInst, public AutoAlloc<RelocatableInst, DataBlockAbsRel> {
    unsigned int opn;
    rword        offset;

public:
    DataBlockAbsRel(llvm::MCInst inst, unsigned int opn, rword offset)
        : RelocatableInst(inst), opn(opn), offset(offset) {};

    llvm::MCInst reloc(ExecBlock *exec_block) {
        inst.getOperand(opn).setImm(exec_block->getDataBlockBase() + offset);
        return inst;
    }
};

class EpilogueRel : public RelocatableInst, public AutoAlloc<RelocatableInst, EpilogueRel> {
    unsigned int opn;
    rword        offset;

public:
    EpilogueRel(llvm::MCInst inst, unsigned int opn, rword offset)
        : RelocatableInst(inst), opn(opn), offset(offset) {};

    llvm::MCInst reloc(ExecBlock *exec_block) {
        inst.getOperand(opn).setImm(offset + exec_block->getEpilogueOffset());
        return inst;
    }
};

}

#endif
