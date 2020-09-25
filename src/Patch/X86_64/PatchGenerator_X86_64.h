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
#ifndef PATCHGENERATOR_X86_64_H
#define PATCHGENERATOR_X86_64_H

#include "Patch/PatchGenerator.h"
#include "Patch/InstInfo.h"

namespace QBDI {

class GetPCOffset : public PatchGenerator, public AutoAlloc<PatchGenerator, GetPCOffset> {

    Temp temp;
    Constant cst;
    Operand op;
    enum {
        ConstantType,
        OperandType,
    } type;

public:

    /*! Interpret a constant as a RIP relative offset and copy it in a temporary. It can be used to
     * obtain the current value of RIP by using a constant of 0.
     *
     * @param[in] temp     A temporary where the value will be copied.
     * @param[in] cst      The constant to be used.
    */
    GetPCOffset(Temp temp, Constant cst): temp(temp), cst(cst), op(0), type(ConstantType) {}

    /*! Interpret an operand as a RIP relative offset and copy it in a temporary. It can be used to
     * obtain jump/call targets or relative memory access addresses.
     *
     * @param[in] temp     A temporary where the value will be copied.
     * @param[in] op       The  operand index (relative to the instruction LLVM MCInst
     *                     representation) to be used.
    */
    GetPCOffset(Temp temp, Operand op): temp(temp), cst(0), op(op), type(OperandType) {}

    /*! Output:
     *
     * If cst:
     * MOV REG64 temp, IMM64 (cst + address + instSize)

     * If op is an immediate:
     * MOV REG64 temp, IMM64 (op + address + instSize)
     *
    */
    std::vector<std::shared_ptr<RelocatableInst>> generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) const override;
};

class SimulateCall : public PatchGenerator, public AutoAlloc<PatchGenerator, SimulateCall> {

    Temp temp;

public:

    /*! Simulate the effects of a call to the address stored in a temporary. The target address
     * overwrites the stored value of RIP in the context part of the data block and the return address
     * is pushed onto the stack. This generator signals a PC modification and triggers and end of basic
     * block.
     *
     * @param[in] temp   Stores the call target address. Overwritten by this generator.
    */
    SimulateCall(Temp temp) : temp(temp) {}

    /*! Output:
     *
     * MOV MEM64 DataBlock[Offset(RIP)], REG64 temp
     * MOV REG64 temp, IMM64 (address + InstSize)
     * PUSH REG64 temp
    */
    std::vector<std::shared_ptr<RelocatableInst>> generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) const override;

    bool modifyPC() const override {return true;}
};

class SimulateRet : public PatchGenerator, public AutoAlloc<PatchGenerator, SimulateRet> {

    Temp temp;

public:

    /*! Simulate the effects of a return instruction. First the return address is popped from the stack
     * into a temporary, then an optional stack deallocation is performed and finally the return
     * address is written in the stored value of RIP in the context part of the data block. This
     * generator signals a PC modification and triggers an end of basic block.
     *
     * The optional deallocation is performed if the current instruction has one single immediate
     * operand (which is the case of RETIQ and RETIW).
     *
     * @param[in] temp   Any unused temporary, overwritten by this generator.
    */
    SimulateRet(Temp temp) : temp(temp) {}

    /*! Output:
     *
     * POP REG64 temp
     * ADD RSP, IMM64 deallocation # Optional
     * MOV MEM64 DataBlock[Offset(RIP)], REG64 temp
    */
    std::vector<std::shared_ptr<RelocatableInst>> generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge) const override;

    bool modifyPC() const override {return true;}
};


}

#endif
