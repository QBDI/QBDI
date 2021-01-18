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

#include "Patch/Types.h"
#include "Patch/PatchUtils.h"

namespace QBDI {
class RelocatableInst;
class InstTransform;
class Patch;

template<typename T> class PureEval {
public:

    operator std::vector<std::shared_ptr<RelocatableInst>>() {
        return (static_cast<T*>(this))->generate(nullptr, 0, 0, nullptr, nullptr, nullptr);
    }
};

class PatchGenerator {
public:

    using SharedPtr    = std::shared_ptr<PatchGenerator>;
    using SharedPtrVec = std::vector<std::shared_ptr<PatchGenerator>>;
    using UniqPtr      = std::unique_ptr<PatchGenerator>;
    using UniqPtrVec   = std::vector<std::unique_ptr<PatchGenerator>>;

    virtual std::vector<std::shared_ptr<RelocatableInst>> generate(const llvm::MCInst *inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const = 0;

    virtual ~PatchGenerator() = default;

    virtual bool modifyPC() const { return false; }

    virtual bool doNotInstrument() const { return false; }
};

class ModifyInstruction : public PatchGenerator, public AutoAlloc<PatchGenerator, ModifyInstruction>
{
    std::vector<std::shared_ptr<InstTransform>> transforms;

public:

    /*! Apply a list of InstTransform to the current instruction and output the result.
     *
     * @param[in] transforms Vector of InstTransform to be applied.
    */
    ModifyInstruction(std::vector<std::shared_ptr<InstTransform>> transforms) : transforms(transforms) {};

    /*!
     * Output:
     *   (depends on the current instructions and transforms)
    */
    std::vector<std::shared_ptr<RelocatableInst>> generate(const llvm::MCInst *inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const override;
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
    std::vector<std::shared_ptr<RelocatableInst>> generate(const llvm::MCInst *inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const override {
        return {};
    }

    bool doNotInstrument() const override { return true; }
};


// Target Specific Generator that must be available for each target

class CopyReg : public PatchGenerator, public AutoAlloc<PatchGenerator, CopyReg> {
    Temp dst;
    Reg src;

public:

    /*! Copy a register in a temporary.
     *
     * @param[in] dst    A temporary where the register will be copied.
     * @param[in] src    The register which will be copied
    */
    CopyReg(Temp dst, Reg src) : dst(dst), src(src) {}

    /*! Output:
     *
     * MOV REG64 temp, REG64 reg
    */
    std::vector<std::shared_ptr<RelocatableInst>> generate(const llvm::MCInst* inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const override;
};

class GetOperand : public PatchGenerator, public AutoAlloc<PatchGenerator, GetOperand> {
    Temp temp;
    Operand op;

public:

    /*! Obtain the value of the operand op and copy it's value in a temporary. If op is an immediate
     * the immediate value is copied, if op is a register the register value is copied.
     *
     * @param[in] temp   A temporary where the value will be copied.
     * @param[in] op     The operand index (relative to the instruction LLVM MCInst representation)
     *                   to be copied.
    */
    GetOperand(Temp temp, Operand op): temp(temp), op(op) {}

    /*!
     * Output:
     *   MOV REG64 temp, IMM64/REG64 op
    */
    std::vector<std::shared_ptr<RelocatableInst>> generate(const llvm::MCInst* inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const override;
};


class GetConstant : public PatchGenerator, public AutoAlloc<PatchGenerator, GetConstant> {
    Temp temp;
    Constant cst;

public:

    /*! Copy a constant in a temporary.
     *
     * @param[in] temp   A temporary where the value will be copied.
     * @param[in] cst    The constant to copy.
    */
    GetConstant(Temp temp, Constant cst): temp(temp), cst(cst) {}

    /*! Output:
     *
     * MOV REG64 temp, IMM64 cst
    */
    std::vector<std::shared_ptr<RelocatableInst>> generate(const llvm::MCInst* inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const override;
};

class ReadTemp : public PatchGenerator, public AutoAlloc<PatchGenerator, ReadTemp> {

    Temp  temp;
    enum {
        OffsetType,
        ShadowType
    } type;

    Offset offset;
    Shadow shadow;

public:

    /*! Write a temporary value in the data block at the specified offset. This can be used to overwrite
     * register values in the context part of the data block.
     *
     * @param[in] temp      A temporary to store the shadow value.
     * @param[in] offset    The offset in the data block where the temporary will be read.
    */
    ReadTemp(Temp temp, Offset offset): temp(temp), type(OffsetType), offset(offset), shadow(0) {}

    /*! Read a temporary value from the last shadow with the same tag for this instruction.
     *
     * @param[in] temp      A temporary to store the shadow value.
     * @param[in] shadow    The shadow in the data block where the temporary will be read.
    */
    ReadTemp(Temp temp, Shadow shadow): temp(temp), type(ShadowType), offset(0), shadow(shadow) {}

    /*! Output:
     *
     * MOV REG64 temp, MEM64 DataBlock[offset]
    */
    std::vector<std::shared_ptr<RelocatableInst>> generate(const llvm::MCInst* inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const override;
};

class WriteTemp : public PatchGenerator, public AutoAlloc<PatchGenerator, WriteTemp> {

    Temp  temp;
    enum {
        OffsetType,
        ShadowType
    } type;
    // Not working VS 2015
    // union {
        Offset offset;
        Shadow shadow;
    // }

public:

    /*! Write a temporary value in the data block at the specified offset. This can be used to overwrite
     * register values in the context part of the data block.
     *
     * @param[in] temp    A temporary which will be written.  The value of the temporary is unchanged.
     * @param[in] offset  The offset in the data block where the temporary will be written.
    */
    WriteTemp(Temp temp, Offset offset): temp(temp), type(OffsetType), offset(offset), shadow(0) {}

    /*! Write a temporary value in a shadow in the data block.
     *
     * @param[in] temp      A temporary which will be written. The value of the temporary is unchanged.
     * @param[in] shadow    The shadow use to store the value.
    */
    WriteTemp(Temp temp, Shadow shadow): temp(temp), type(ShadowType), offset(0), shadow(shadow) {}

    /*! Output:
     *
     * MOV MEM64 DataBlock[offset], REG64 temp
    */
    std::vector<std::shared_ptr<RelocatableInst>> generate(const llvm::MCInst* inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const override;

    bool modifyPC() const override {return offset == Offset(Reg(REG_PC));}
};

class SaveReg : public PatchGenerator, public AutoAlloc<PatchGenerator, SaveReg>,
    public PureEval<SaveReg> {

    Reg   reg;
    Offset offset;

public:

    /*! Save a register in the data block at the specified offset. This can be used to save
     * register values in the context part of the data block.
     *
     * @param[in] reg     A register which will be saved.
     * @param[in] offset  The offset in the data block where the register will be written.
    */
    SaveReg(Reg reg, Offset offset): reg(reg), offset(offset) {}

    /*! Output:
     *
     * MOV MEM64 DataBlock[offset], REG64 reg
    */
    std::vector<std::shared_ptr<RelocatableInst>> generate(const llvm::MCInst* inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const override;
};

class LoadReg : public PatchGenerator, public AutoAlloc<PatchGenerator, LoadReg>,
    public PureEval<LoadReg> {

    Reg   reg;
    Offset offset;

public:

    /*! Load a register from the data block at the specified offset. This can be used to load
     * register values from the context part of the data block.
     *
     * @param[in] reg     A register where the value will be loaded.
     * @param[in] offset  The offset in the data block from where the value will be loaded.
    */
    LoadReg(Reg reg, Offset offset): reg(reg), offset(offset) {}

    /*! Output:
     *
     * MOV REG64 reg, MEM64 DataBlock[offset]
    */
    std::vector<std::shared_ptr<RelocatableInst>> generate(const llvm::MCInst* inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const override;
};

class GetInstId : public PatchGenerator, public AutoAlloc<PatchGenerator, GetInstId> {

    Temp temp;

public:

    /*! Copy an ExecBlock specific id for the current instruction in a temporary. This id is used to
     * identify the instruction responsible for a callback in the engine and is only meant for
     * internal use.
     *
     * @param[in] temp   A temporary where the id will be copied.
    */
    GetInstId(Temp temp): temp(temp) {}

    /*! Output:
     *
     * MOV REG64 temp, IMM64 instID
    */
    std::vector<std::shared_ptr<RelocatableInst>> generate(const llvm::MCInst* inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const override;
};

class JmpEpilogue : public PatchGenerator, public AutoAlloc<PatchGenerator, JmpEpilogue>,
    public PureEval<JmpEpilogue> {

public:

    /*! Generate a jump instruction which target the epilogue of the ExecBlock.
    */
    JmpEpilogue() {}

    /*! Output:
     *
     * JMP Offset(Epilogue)
    */
    std::vector<std::shared_ptr<RelocatableInst>> generate(const llvm::MCInst* inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const override;
};


}

#endif
