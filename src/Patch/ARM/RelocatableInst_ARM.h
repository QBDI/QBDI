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
#ifndef RELOCATABLEINST_ARM_H
#define RELOCATABLEINST_ARM_H

#include "Patch/RelocatableInst.h"

namespace QBDI {

class MemoryConstant: public RelocatableInst, public AutoAlloc<RelocatableInst, MemoryConstant> {
    unsigned int opn;
    rword        value;

public:

    MemoryConstant(llvm::MCInst inst, unsigned int opn, rword value)
        : RelocatableInst(inst), opn(opn), value(value) {};

    llvm::MCInst reloc(ExecBlock *exec_block) {
        uint16_t id = exec_block->newShadow();
        exec_block->setShadow(id, value);
        inst.getOperand(opn).setImm(
            exec_block->getDataBlockOffset() + exec_block->getShadowOffset(id) - 8
        );
        return inst;
    }
};

class HostPCRel : public RelocatableInst, public AutoAlloc<RelocatableInst, HostPCRel> {
    unsigned int opn;
    rword        offset;

public:
  HostPCRel(llvm::MCInst inst, unsigned int opn, rword offset)
        : RelocatableInst(inst), opn(opn), offset(offset) {};

    llvm::MCInst reloc(ExecBlock *exec_block) {
        uint16_t id = exec_block->newShadow();
        exec_block->setShadow(id, offset + exec_block->getCurrentPC());
        inst.getOperand(opn).setImm(
            exec_block->getDataBlockOffset() + exec_block->getShadowOffset(id) - 8
        );
        return inst;
    }
};

class InstId : public RelocatableInst, public AutoAlloc<RelocatableInst, InstId> {
    unsigned int opn;

public:
    InstId(llvm::MCInst inst, unsigned int opn)
        : RelocatableInst(inst), opn(opn) {};

    llvm::MCInst reloc(ExecBlock *exec_block) {
        uint16_t id = exec_block->newShadow();
        exec_block->setShadow(id, exec_block->getNextInstID());
        inst.getOperand(opn).setImm(
            exec_block->getDataBlockOffset() + exec_block->getShadowOffset(id) - 8
        );
        return inst;
    }
};


}

#endif
