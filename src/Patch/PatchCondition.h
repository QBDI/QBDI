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
#ifndef PATCHCONDITION_H
#define PATCHCONDITION_H

#include <memory>
#include <vector>
#include <string>

#include "llvm/MC/MCInst.h"

#include "Range.h"
#include "Patch/Types.h"
#include "Patch/PatchUtils.h"

#include "Utility/String.h"

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
#include "Patch/X86_64/InstInfo_X86_64.h"
#elif defined(QBDI_ARCH_ARM)
#include "Patch/ARM/InstInfo_ARM.h"
#endif

namespace QBDI {

class PatchCondition {
public:

    using SharedPtr    = std::shared_ptr<PatchCondition>;
    using SharedPtrVec = std::vector<std::shared_ptr<PatchCondition>>;

    virtual bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) = 0;

    virtual RangeSet<rword> affectedRange() {
        RangeSet<rword> r;
        r.add(Range<rword>(0, (rword) -1));
        return r;
    }

    virtual ~PatchCondition() {};
};

class MnemonicIs : public PatchCondition, public AutoAlloc<PatchCondition, MnemonicIs> {
    std::string mnemonic;

public:

    /*! Return true if the mnemonic of the current instruction is equal to Mnemonic.
     *
     * @param[in] mnemonic   A null terminated instruction mnemonic (using LLVM style)
    */
    MnemonicIs(const char *mnemonic) : mnemonic(mnemonic) {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        return QBDI::String::startsWith(mnemonic.c_str(), MCII->getName(inst->getOpcode()).data());
    }
};

class OpIs : public PatchCondition, public AutoAlloc<PatchCondition, OpIs> {
    unsigned int op;

public:

    /*! Return true if the instruction opcode is equal to op.
     *
     * @param[in] op LLVM instruction opcode ID.
    */
    OpIs(unsigned int op) : op(op) {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) { // refactor all test() add MCII
        return inst->getOpcode() == op;
    }
};

class RegIs : public PatchCondition, public AutoAlloc<PatchCondition, RegIs> {
    Operand opn;
    Reg reg;

public:

    /*! Return true if the instruction operand with index opn is a register equal to reg.
     *
     * @param[in] opn The instruction operand.
     * @param[in] reg The register to compare with.
    */
    RegIs(Operand opn, Reg reg) : opn(opn), reg(reg) {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        return inst->getOperand(opn).isReg() && inst->getOperand(opn).getReg() == (unsigned int) reg;
    }
};

class UseReg : public PatchCondition, public AutoAlloc<PatchCondition, UseReg> {
    Reg reg;

public:

    /*!  Return true if the instruction uses reg as one of its operand.
     *
     * @param[in] reg The register to compare with.
    */
    UseReg(Reg reg) : reg(reg) {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        for(unsigned int i  = 0; i < inst->getNumOperands(); i++) {
            const llvm::MCOperand &op = inst->getOperand(i);
            if(op.isReg() && op.getReg() == (unsigned int) reg) {
                return true;
            }
        }
        return false;
    }
};

class InstructionInRange : public PatchCondition, public AutoAlloc<PatchCondition, InstructionInRange> {
    Range<rword> range;

public:

    /*! Return true if the instruction address is in the range [start, end[ (end not included).
     *
     * @param[in] start Start of the range.
     * @param[in] end   End of the range (not included).
    */
    InstructionInRange(Constant start, Constant end) : range(start, end) {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        if(range.contains(Range<rword>(address, address + instSize))) {
            return true;
        }
        else {
            return false;
        }
    }

    RangeSet<rword> affectedRange() {
        RangeSet<rword> r;
        r.add(range);
        return r;
    }
};

class AddressIs : public PatchCondition, public AutoAlloc<PatchCondition, AddressIs> {
    rword breakpoint;
     
public:

    /*! Return true if on specified address
    */
    AddressIs(rword breakpoint) : breakpoint(breakpoint) {};
    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        return address == breakpoint;
    }

    RangeSet<rword> affectedRange() {
        RangeSet<rword> r;
        r.add(Range<rword>(breakpoint, breakpoint + 1));
        return r;
    }
};

class OperandIsReg : public PatchCondition, public AutoAlloc<PatchCondition, OperandIsReg> {
    Operand opn;

public:

    /*! Return true if the instruction operand opn is a register.
     *
     * @param[in] opn The instruction operand.
    */
    OperandIsReg(Operand opn) : opn(opn) {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        return inst->getOperand(opn).isReg(); }
};

class OperandIsImm : public PatchCondition, public AutoAlloc<PatchCondition, OperandIsImm> {
    Operand opn;

public:

    /*! Return true if the instruction operand opn is an immediate.
     *
     * @param[in] opn The instruction operand.
    */
    OperandIsImm(Operand opn) : opn(opn) {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        return inst->getOperand(opn).isImm();
    }
};

class And : public PatchCondition, public AutoAlloc<PatchCondition, And> {
    PatchCondition::SharedPtrVec conditions;

public:

    /*! Return true if every PatchCondition of the list conditions return true (lazy evaluation).
     *
     * @param[in] conditions List of conditions to evaluate.
    */
    And(PatchCondition::SharedPtrVec conditions) : conditions(conditions) {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        for(unsigned int i = 0; i < conditions.size(); i++) {
            if(conditions[i]->test(inst, address, instSize, MCII) == false) {
                return false;
            }
        }
        return true;
    }

    RangeSet<rword> affectedRange() {
        RangeSet<rword> r;
        r.add(Range<rword>(0, (rword)-1));
        for(unsigned int i = 0; i < conditions.size(); i++) {
            r.intersect(conditions[i]->affectedRange());
        }
        return r;
    }
};

class Or : public PatchCondition, public AutoAlloc<PatchCondition, Or> {
    PatchCondition::SharedPtrVec conditions;

public:

    /*! Return true if one of the PatchCondition of the list conditions return true (lazy evaluation).
     *
     * @param[in] conditions List of conditions to evaluate.
    */
    Or(PatchCondition::SharedPtrVec conditions) : conditions(conditions) {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        for(unsigned int i = 0; i < conditions.size(); i++) {
            if(conditions[i]->test(inst, address, instSize, MCII) == true) {
                return true;
            }
        }
        return false;
    }

    RangeSet<rword> affectedRange() {
        RangeSet<rword> r;
        for(unsigned int i = 0; i < conditions.size(); i++) {
            r.add(conditions[i]->affectedRange());
        }
        return r;
    }
};

class Not : public PatchCondition, public AutoAlloc<PatchCondition, Not> {
    PatchCondition::SharedPtr condition;

public:

    /*! Return the logical inverse of condition.
     *
     * @param[in] condition Condition to evaluate.
    */
    Not(PatchCondition::SharedPtr condition) : condition(condition) {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        return !condition->test(inst, address, instSize, MCII);
    }
};

class True : public PatchCondition, public AutoAlloc<PatchCondition, True> {
public:

    /*! Return true.
    */
    True() {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        return true;
    }
};

class DoesReadAccess : public PatchCondition, public AutoAlloc<PatchCondition, DoesReadAccess> {
public:

    /*! Return true if the instruction read data from memory.
    */
    DoesReadAccess() {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        return getReadSize(inst) > 0;
    }
};

class DoesWriteAccess : public PatchCondition, public AutoAlloc<PatchCondition, DoesWriteAccess> {
public:

    /*! Return true if the instruction write data to memory.
    */
    DoesWriteAccess() {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        return getWriteSize(inst) > 0;
    }
};

class ReadAccessSizeIs : public PatchCondition, public AutoAlloc<PatchCondition, ReadAccessSizeIs> {
public:

    Constant size;

    /*! Return true if the instruction read access size equal to size.
     *
     * @param[in] size Size to compare with.
    */
    ReadAccessSizeIs(Constant size) : size(size) {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        return getReadSize(inst) == (rword) size;
    }
};

class WriteAccessSizeIs : public PatchCondition, public AutoAlloc<PatchCondition, WriteAccessSizeIs> {
public:

    Constant size;

    /*! Return true if the instruction write access size equal to size.
     *
     * @param[in] size Size to compare with.
    */
    WriteAccessSizeIs(Constant size) : size(size) {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        return getWriteSize(inst) == (rword) size;
    }
};

class IsStackRead : public PatchCondition, public AutoAlloc<PatchCondition, IsStackRead> {
public:

    /*! Return true if the instruction is reading data from the stack.
    */
    IsStackRead() {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        return isStackRead(inst);
    }
};

class IsStackWrite : public PatchCondition, public AutoAlloc<PatchCondition, IsStackWrite> {
public:

    /*! Return true if the instruction is writing data to the stack.
    */
    IsStackWrite() {};

    bool test(const llvm::MCInst* inst, rword address, rword instSize, llvm::MCInstrInfo* MCII) {
        return isStackWrite(inst);
    }
};

}

#endif
