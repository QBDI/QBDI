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
#ifndef INSTTRANSFORM_H
#define INSTTRANSFORM_H

#include <memory>
#include <vector>

#include "Patch/Types.h"
#include "Patch/PatchUtils.h"

namespace llvm {
  class MCInst;
}

namespace QBDI {

class InstTransform {
public:

    using SharedPtr    = std::shared_ptr<InstTransform>;
    using SharedPtrVec = std::vector<std::shared_ptr<InstTransform>>;
    using UniqPtr      = std::unique_ptr<PatchGenerator>;
    using UniqPtrVec   = std::vector<std::unique_ptr<PatchGenerator>>;

    virtual void transform(llvm::MCInst &inst, rword address, size_t instSize, TempManager *temp_manager) const = 0;

    virtual ~InstTransform() = default;
};

class SetOperand : public InstTransform, public AutoAlloc<InstTransform, SetOperand> {
    Operand opn;
    enum {
        TempOperandType,
        RegOperandType,
        ImmOperandType
    } type;
    // Not working VS 2015
    // union {
        Temp temp;
        Reg reg;
        Constant imm;
    // };

public:

    /*! Set the operand opn of the instruction as the Temp temp.
     *
     * @param[in] opn   Operand index in the LLVM MCInst representation.
     * @param[in] temp  Temporary register which will be set as the new operand
    */
    SetOperand(Operand opn, Temp temp) : opn(opn), type(TempOperandType), temp(temp), reg(0), imm(0) {}

    /*! Set the operand opn of the instruction as the Reg reg.
     *
     * @param[in] opn  Operand index in the LLVM MCInst representation.
     * @param[in] reg  Register which will be set as the new operand.
    */
    SetOperand(Operand opn, Reg reg) : opn(opn), type(RegOperandType), temp(0), reg(reg), imm(0) {}

    /*! Set the operand opn of the instruction as the immediate imm.
     *
     * @param[in] opn  Operand index in the LLVM MCInst representation.
     * @param[in] imm  Constant which will be set as the new immediate operand.
    */
    SetOperand(Operand opn, Constant imm) : opn(opn), type(ImmOperandType), temp(0), reg(0), imm(imm) {}

    void transform(llvm::MCInst &inst, rword address, size_t instSize, TempManager *temp_manager) const override;
};

class SubstituteWithTemp : public InstTransform,
                           public AutoAlloc<InstTransform, SubstituteWithTemp> {
    Reg  reg;
    Temp temp;

public:

    /*! Substitute every reference to reg in the operands of the instruction with temp.
     *
     * @param[in] reg   Register which will be substituted.
     * @param[in] temp  Temporary register which will be substituted with.
    */
    SubstituteWithTemp(Reg reg, Temp temp) : reg(reg), temp(temp) {};

    void transform(llvm::MCInst &inst, rword address, size_t instSize, TempManager *temp_manager) const override;
};

class AddOperand : public InstTransform, public AutoAlloc<InstTransform, AddOperand> {

    Operand opn;
    enum {
        TempOperandType,
        RegOperandType,
        ImmOperandType
    } type;
    // Not working under VS2015
    // union {
        Temp temp;
        Reg reg;
        Constant imm;
    // };

public:

    /*! Add a new temporary register operand to the instruction by inserting it at operand index opn.
     *
     * @param[in] opn   Operand index in LLVM MCInst representation.
     * @param[in] temp  Temp to be inserted as a new operand.
    */
    AddOperand(Operand opn, Temp temp) : opn(opn), type(TempOperandType), temp(temp), reg(0), imm(0) {}

    /*! Add a new register operand to the instruction by inserting it at operand index opn.
     *
     * @param[in] opn  Operand index in LLVM MCInst representation.
     * @param[in] reg  Register to be inserted as a new operand.
    */
    AddOperand(Operand opn, Reg reg) : opn(opn), type(RegOperandType), temp(0), reg(reg), imm(0) {}

    /*! Add a new immediate operand to the instruction by inserting it at operand index opn.
     *
     * @param[in] opn  Operand index in LLVM MCInst representation.
     * @param[in] imm  Constant to be inserted as a new immediate operand.
    */
    AddOperand(Operand opn, Constant imm) : opn(opn), type(ImmOperandType), temp(0), reg(0), imm(imm) {}

    void transform(llvm::MCInst &inst, rword address, size_t instSize, TempManager *temp_manager) const override;
};

class RemoveOperand : public InstTransform, public AutoAlloc<InstTransform, RemoveOperand> {

    Reg reg;

public:

    /*! Remove the first occurence of reg in the operands of the instruction.
     *
     * @param[in] reg Register to remove from the operand list.
    */
    RemoveOperand(Reg reg) : reg(reg) {}

    void transform(llvm::MCInst &inst, rword address, size_t instSize, TempManager *temp_manager) const override;
};


class SetOpcode : public InstTransform, public AutoAlloc<InstTransform, SetOpcode> {

    unsigned int opcode;

public:

    /*! Set the opcode of the instruction.
     *
     * @param[in] opcode New opcode to set as the instruction opcode.
    */
    SetOpcode(unsigned int opcode) : opcode(opcode) {}

    void transform(llvm::MCInst &inst, rword address, size_t instSize, TempManager *temp_manager) const override;
};

}

#endif
