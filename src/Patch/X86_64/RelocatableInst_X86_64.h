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

#include "Config.h"

namespace QBDI {

class RelocatablePseudoInst : public RelocatableInst {

public:
    RelocatablePseudoInst();

    virtual llvm::MCInst reloc(ExecBlock *exec_block) const =0;

    virtual ~RelocatablePseudoInst() = default;
};

class HostPCRel : public RelocatableInst, public AutoAlloc<RelocatableInst, HostPCRel> {
    unsigned int opn;
    rword        offset;

public:
    HostPCRel(llvm::MCInst&& inst, unsigned int opn, rword offset)
        : RelocatableInst(std::move(inst)), opn(opn), offset(offset) {};

    llvm::MCInst reloc(ExecBlock *exec_block) const override {
        llvm::MCInst res = inst;
        res.getOperand(opn).setImm(offset + exec_block->getCurrentPC());
        return res;
    }
};

class InstId : public RelocatableInst, public AutoAlloc<RelocatableInst, InstId> {
    unsigned int opn;

public:
    InstId(llvm::MCInst&& inst, unsigned int opn)
        : RelocatableInst(std::move(inst)), opn(opn) {};

    llvm::MCInst reloc(ExecBlock *exec_block) const override {
        llvm::MCInst res = inst;
        res.getOperand(opn).setImm(exec_block->getNextInstID());
        return res;
    }
};

class CreateShadowInst : public RelocatablePseudoInst, public AutoAlloc<RelocatableInst, CreateShadowInst> {

    uint16_t tag;

public:
    CreateShadowInst(uint16_t tag)
        : RelocatablePseudoInst(), tag(tag) {};

    llvm::MCInst reloc(ExecBlock *exec_block) const override {
        exec_block->newShadow(tag);
        return inst;
    }
};

class TaggedShadow : public RelocatableInst, public AutoAlloc<RelocatableInst, TaggedShadow> {

    unsigned int opn;
    uint16_t tag;
    rword inst_size;

public:
    TaggedShadow(llvm::MCInst&& inst, unsigned int opn, uint16_t tag, rword inst_size)
        : RelocatableInst(std::move(inst)), opn(opn), tag(tag), inst_size(inst_size) {};

    llvm::MCInst reloc(ExecBlock *exec_block) const override {
        uint16_t id = exec_block->getLastShadow(tag);
        llvm::MCInst res = inst;
        res.getOperand(opn).setImm(
            exec_block->getDataBlockOffset() + exec_block->getShadowOffset(id) - inst_size
        );
        return res;
    }
};

class TaggedShadowAbs : public RelocatableInst, public AutoAlloc<RelocatableInst, TaggedShadowAbs> {

    unsigned int opn;
    uint16_t tag;

public:
    TaggedShadowAbs(llvm::MCInst&& inst, unsigned int opn, uint16_t tag)
        : RelocatableInst(std::move(inst)), opn(opn), tag(tag) {};

    llvm::MCInst reloc(ExecBlock *exec_block) const override {
        uint16_t id = exec_block->getLastShadow(tag);
        llvm::MCInst res = inst;
        res.getOperand(opn).setImm(
            exec_block->getDataBlockBase() + exec_block->getShadowOffset(id)
        );
        return res;
    }
};

inline std::shared_ptr<RelocatableInst> DataBlockRelx86(llvm::MCInst&& inst, unsigned int opn, rword offset, rword inst_size) {
    if constexpr(is_x86_64) {
        inst.getOperand( opn /* AddrBaseReg*/).setReg(Reg(REG_PC));
        return DataBlockRel(std::move(inst), opn + 3 /* AddrDisp */, offset - inst_size);
    } else {
        inst.getOperand( opn /* AddrBaseReg*/).setReg(0);
        return DataBlockAbsRel(std::move(inst), opn + 3 /* AddrDisp */, offset);
    }
}

inline std::shared_ptr<RelocatableInst> TaggedShadowx86(llvm::MCInst&& inst, unsigned int opn, uint16_t tag, rword inst_size) {
    if constexpr(is_x86_64) {
        inst.getOperand( opn /* AddrBaseReg*/).setReg(Reg(REG_PC));
        return TaggedShadow(std::move(inst), opn + 3 /* AddrDisp */, tag, inst_size);
    } else {
        inst.getOperand( opn /* AddrBaseReg*/).setReg(0);
        return TaggedShadowAbs(std::move(inst), opn + 3 /* AddrDisp */, tag);
    }

}

}

#endif
