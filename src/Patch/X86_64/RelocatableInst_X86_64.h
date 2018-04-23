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
#ifndef RELOCATABLEINST_X86_64_H
#define RELOCATABLEINST_X86_64_H

#include "Patch/RelocatableInst.h"

namespace QBDI {

class DataBlockRel : public RelocatableInst, public AutoAlloc<RelocatableInst, DataBlockRel> {
    unsigned int opn;
    rword        offset;

public:
    DataBlockRel(llvm::MCInst inst, unsigned int opn, rword offset)
        : RelocatableInst(inst), opn(opn), offset(offset) {};

    llvm::MCInst reloc(ExecBlock *exec_block, CPUMode cpuMode) {
        inst.getOperand(opn).setImm(offset + exec_block->getDataBlockOffset());
        return inst;
    }
};

class EpilogueRel : public RelocatableInst, public AutoAlloc<RelocatableInst, EpilogueRel> {
    unsigned int opn;
    rword        offset;

public:
    EpilogueRel(llvm::MCInst inst, unsigned int opn, rword offset)
        : RelocatableInst(inst), opn(opn), offset(offset) {};

    llvm::MCInst reloc(ExecBlock *exec_block, CPUMode cpuMode) {
        inst.getOperand(opn).setImm(offset + exec_block->getEpilogueOffset());
        return inst;
    }
};

class HostPCRel : public RelocatableInst, public AutoAlloc<RelocatableInst, HostPCRel> {
    unsigned int opn;
    rword        offset;

public:
  HostPCRel(llvm::MCInst inst, unsigned int opn, rword offset)
        : RelocatableInst(inst), opn(opn), offset(offset) {};

    llvm::MCInst reloc(ExecBlock *exec_block, CPUMode cpuMode) {
        inst.getOperand(opn).setImm(offset + exec_block->getCurrentPC());
        return inst;
    }
};

class InstId : public RelocatableInst, public AutoAlloc<RelocatableInst, InstId> {
    unsigned int opn;

public:
    InstId(llvm::MCInst inst, unsigned int opn)
        : RelocatableInst(inst), opn(opn) {};

    llvm::MCInst reloc(ExecBlock *exec_block, CPUMode cpuMode) {
        inst.getOperand(opn).setImm(exec_block->getNextInstID());
        return inst;
    }
};

class TaggedShadow : public RelocatableInst, public AutoAlloc<RelocatableInst, TaggedShadow> {

    unsigned int opn;
    uint16_t tag;

public:
    TaggedShadow(llvm::MCInst inst, unsigned int opn, uint16_t tag)
        : RelocatableInst(inst), opn(opn), tag(tag) {};

    llvm::MCInst reloc(ExecBlock *exec_block, CPUMode cpuMode) {
        uint16_t id = exec_block->newShadow(tag);
        inst.getOperand(opn).setImm(
            exec_block->getDataBlockOffset() + exec_block->getShadowOffset(id) - 7
        );
        return inst;
    }
};

}

#endif
