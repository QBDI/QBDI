/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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

#include "QBDI/Config.h"

namespace QBDI {

class HostPCRel : public AutoClone<RelocatableInst, HostPCRel> {
    unsigned int opn;
    rword        offset;

public:
    HostPCRel(llvm::MCInst&& inst, unsigned int opn, rword offset)
        : AutoClone<RelocatableInst, HostPCRel>(std::forward<llvm::MCInst>(inst)),
        opn(opn), offset(offset) {};

    llvm::MCInst reloc(ExecBlock *exec_block) const override {
        llvm::MCInst res = inst;
        res.getOperand(opn).setImm(offset + exec_block->getCurrentPC());
        return res;
    }
};

class InstId : public AutoClone<RelocatableInst, InstId> {
    unsigned int opn;

public:
    InstId(llvm::MCInst&& inst, unsigned int opn)
        : AutoClone<RelocatableInst, InstId>(std::forward<llvm::MCInst>(inst)), opn(opn) {};

    llvm::MCInst reloc(ExecBlock *exec_block) const override {
        llvm::MCInst res = inst;
        res.getOperand(opn).setImm(exec_block->getNextInstID());
        return res;
    }
};

class TaggedShadow : public AutoClone<RelocatableInst, TaggedShadow> {

    unsigned int opn;
    uint16_t tag;
    rword inst_size;
    bool create;

public:
    TaggedShadow(llvm::MCInst&& inst, unsigned int opn, uint16_t tag, rword inst_size, bool create=true)
        : AutoClone<RelocatableInst, TaggedShadow>(std::forward<llvm::MCInst>(inst)),
        opn(opn), tag(tag), inst_size(inst_size), create(create) {};

    llvm::MCInst reloc(ExecBlock *exec_block) const override {
        uint16_t id;
        if (create) {
            id = exec_block->newShadow(tag);
        } else {
            id = exec_block->getLastShadow(tag);
        }
        llvm::MCInst res = inst;
        res.getOperand(opn).setImm(
            exec_block->getDataBlockOffset() + exec_block->getShadowOffset(id) - inst_size
        );
        return res;
    }
};

class TaggedShadowAbs : public AutoClone<RelocatableInst, TaggedShadowAbs> {

    unsigned int opn;
    uint16_t tag;
    bool create;

public:
    TaggedShadowAbs(llvm::MCInst&& inst, unsigned int opn, uint16_t tag, bool create=true)
        : AutoClone<RelocatableInst, TaggedShadowAbs>(std::forward<llvm::MCInst>(inst)),
        opn(opn), tag(tag), create(create) {};

    llvm::MCInst reloc(ExecBlock *exec_block) const override {
        uint16_t id;
        if (create) {
            id = exec_block->newShadow(tag);
        } else {
            id = exec_block->getLastShadow(tag);
        }
        llvm::MCInst res = inst;
        res.getOperand(opn).setImm(
            exec_block->getDataBlockBase() + exec_block->getShadowOffset(id)
        );
        return res;
    }
};

inline std::unique_ptr<RelocatableInst> DataBlockRelx86(llvm::MCInst&& inst, unsigned int opn, rword offset, rword inst_size) {
    if constexpr(is_x86_64) {
        inst.getOperand(opn /* AddrBaseReg */).setReg(Reg(REG_PC));
        return DataBlockRel::unique(std::forward<llvm::MCInst>(inst), opn + 3 /* AddrDisp */, offset - inst_size);
    } else {
        inst.getOperand(opn /* AddrBaseReg */).setReg(0);
        return DataBlockAbsRel::unique(std::forward<llvm::MCInst>(inst), opn + 3 /* AddrDisp */, offset);
    }
}

inline std::unique_ptr<RelocatableInst> TaggedShadowx86(llvm::MCInst&& inst, unsigned int opn, uint16_t tag, rword inst_size, bool create=true) {
    if constexpr(is_x86_64) {
        inst.getOperand(opn /* AddrBaseReg */).setReg(Reg(REG_PC));
        return TaggedShadow::unique(std::forward<llvm::MCInst>(inst), opn + 3 /* AddrDisp */, tag, inst_size, create);
    } else {
        inst.getOperand(opn /* AddrBaseReg */).setReg(0);
        return TaggedShadowAbs::unique(std::forward<llvm::MCInst>(inst), opn + 3 /* AddrDisp */, tag, create);
    }
}

}

#endif
