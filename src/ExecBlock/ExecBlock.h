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
#ifndef EXECBLOCK_H
#define EXECBLOCK_H

#include <iterator>
#include <memory>
#include <vector>

#include "llvm/MC/MCInst.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/Memory.h"

#include "Callback.h"
#include "Context.h"
#include "Patch/Types.h"
#include "Utility/memory_ostream.h"
#include "Utility/Assembly.h"

namespace QBDI {

class RelocatableInst;
class Patch;

enum SeqType {
    Entry = 1,
    Exit  = 1<<1,
};

struct InstInfo {
    uint16_t seqID;
    uint16_t offset;
};

struct SeqInfo {
    uint16_t startInstID;
    uint16_t endInstID;
    SeqType  type;
    CPUMode  cpuMode;
};

struct SeqWriteResult {
    uint16_t seqID;
    unsigned bytesWritten;
    unsigned patchWritten;
};

struct ShadowInfo {
    uint16_t seqID;
    uint16_t instID;
    uint16_t tag;
    uint16_t shadowID;
};

static const uint16_t EXEC_BLOCK_FULL = 0xFFFF;

/*! Manages the concept of an exec block made of two contiguous memory blocks (one for the code, 
 *  the other for the data) used to store and execute instrumented basic blocks.
 */
class ExecBlock {
private:

    using PF = llvm::sys::Memory::ProtectionFlags;

    enum PageState {RX, RW};

    static uint32_t                                      epilogueSize;
    static std::vector<std::shared_ptr<RelocatableInst>> execBlockPrologue;
    static std::vector<std::shared_ptr<RelocatableInst>> execBlockEpilogue;
    static void (*runCodeBlockFct)(void*);

    VMInstanceRef               vminstance;
    llvm::sys::MemoryBlock      codeBlock;
    llvm::sys::MemoryBlock      dataBlock;
    memory_ostream*             codeStream;
    Assembly*                   assembly[CPUMode::COUNT];
    Context*                    context;
    rword*                      shadows;
    std::vector<ShadowInfo>     shadowRegistry;
    uint16_t                    shadowIdx;
    std::vector<InstMetadata>   instMetadata;
    std::vector<InstInfo>       instRegistry;
    std::vector<SeqInfo>        seqRegistry;
    PageState                   pageState;
    uint16_t                    currentSeq;
    uint16_t                    currentInst;

    /*! Verify if the code block is in read execute mode.
     *
     * @return Return true if the code block is in read execute mode.
     */
    bool isRX() const {return pageState == RX;}
    
    /*! Verify if the code block is in read write mode.
     *
     * @return Return true if the code block is in read write mode.
     */
    bool isRW() const {return pageState == RW;}

    /*! Changes the code block permissions to RX.
     */
    void makeRX();

    /*! Changes the code block permissions to RW.
     */
    void makeRW();

public:

    /*! Construct a new ExecBlock
     *
     * @param[in] assembly    Assembly used to assemble instructions in the ExecBlock.
     * @param[in] vminstance  Pointer to public engine interface
     */
    ExecBlock(Assembly* assembly[CPUMode::COUNT], VMInstanceRef vminstance = nullptr);

    ~ExecBlock();

    /*! Display the content of an exec block to stderr.
     */
    void show() const;
    
    /* Low level run function. Does not take care of the callbacks.
    */
    void run();

    /*! Execute the sequence currently programmed in the selector of the exec block. Take care 
     *  of the callbacks handling.
     */
    VMAction execute();

    /*! Write a new sequence in the exec block. This function does not guarantee that the 
     *  sequence will be written in its entierty and might stop before the end using an
     *  architecture specific terminator. Return 0 if the exec block was full and no instruction was
     *  written.
     *  
     * @param seqStart [in] Iterator to the start of a list of patches.
     * @param seqEnd   [in] Iterator to the end of a list of patches.
     * @param seqType  [in] Type of the sequence.
     *
     * @return A structure detailling the write operation result.
     */
    SeqWriteResult writeSequence(std::vector<Patch>::const_iterator seqStart, std::vector<Patch>::const_iterator seqEnd, SeqType seqType);

    /*! Split an existing sequence at instruction instID to create a new sequence.
     *
     * @param instID  [in] ID of the instruction where to split the sequence at.
     *
     * @return The new sequence ID.
     */
    uint16_t splitSequence(uint16_t instID);

    /*! Compute the offset between the current code stream position and the start of the data block.
     *  Used for pc relative memory access to the data block.
     *  
     * @return The computed offset.
     */
    rword getDataBlockOffset() const {
        return (rword) dataBlock.base() - (rword) codeBlock.base() - codeStream->current_pos();
    }

    /*! Compute the offset between the current code stream position and the start of the 
     *  exec block epilogue code. Used for computing the remaining code space left or jumping to 
     *  the exec block epilogue at the end of a sequence.
     *  
     * @return The computed offset.
     */
    rword getEpilogueOffset() const {
        return codeBlock.size() - epilogueSize - codeStream->current_pos();
    }

    /*! Obtain the value of the PC where the ExecBlock is currently writing instructions.
     *
     * @return The PC value.
     */
    rword getCurrentPC() const {
        return (rword) codeBlock.base() + codeStream->current_pos();
    }

    /*! Obtain the current instruction ID.
     *
     * @return The current instruction ID.
     */
    uint16_t getNextInstID() const {
        return (uint16_t) instMetadata.size();
    }

    /*! Obtain the instruction ID for a specific address (the address must exactly match the start 
     *  of the instruction).
     *
     * @param address The address of the start of the instruction.
     *
     * @return The instruction ID or NOT_FOUND.
     */
    uint16_t getInstID(rword address) const;

    /*! Obtain the current instruction ID.
     *
     * @return The ID of the current instruction.
     */
    uint16_t getCurrentInstID() const { return currentInst; }

    /*! Obtain the instruction metadata for a specific instruction ID.
     *
     * @param instID The instruction ID.
     *
     * @return The metadata of the instruction.
     */
    const InstMetadata* getInstMetadata(uint16_t instID) const;

    /*! Obtain the instruction address for a specific instruction ID.
     *
     * @param instID The instruction ID.
     *
     * @return The address of the instruction.
     */
    rword getInstAddress(uint16_t instID) const;

    /*! Obtain the original MCInst for a specific instruction ID.
     *
     * @param instID The instruction ID.
     *
     * @return The original MCInst of the instruction.
     */
    const llvm::MCInst* getOriginalMCInst(uint16_t instID) const;

    /*! Obtain the next sequence ID.
     *
     * @return The next sequence ID.
     */
    uint16_t getNextSeqID() const {
        return (uint16_t) seqRegistry.size();
    }

    /*! Obtain the sequence ID for a specific address (the address must exactly match the start 
     *  of the sequence).
     *
     * @param address The address of the start of the sequence.
     *
     * @return The sequence ID or NOT_FOUND.
     */
    uint16_t getSeqID(rword address) const;

    /*! Obtain the sequence ID containing a specific instruction ID.
     *
     * @param instID The instruction ID.
     *
     * @return The sequence ID or NOT_FOUND.
     */
    uint16_t getSeqID(uint16_t instID) const;

    /*! Obtain the current sequence ID.
     *
     * @return The ID of the current sequence.
     */
    uint16_t getCurrentSeqID() const { return currentSeq; }

    /* Obtain the sequence type for a specific sequence ID.
     *
     * @param seqID The sequence ID.
     *
     * @return The type of the sequence.
     */
    SeqType getSeqType(uint16_t seqID) const;

    /*! Obtain the sequence start address for a specific sequence ID.
     *
     * @param seqID The sequence ID.
     *
     * @return The start address of the sequence.
     */
    uint16_t getSeqStart(uint16_t seqID) const;

    /*! Obtain the instruction id of the sequence end address for a specific sequence ID.
     *
     * @param seqID The sequence ID.
     *
     * @return The end address of the sequence.
     */
    uint16_t getSeqEnd(uint16_t seqID) const;

    /*! Set the selector of the exec block to a specific sequence offset. Used to program the
     *  execution of a specific sequence within the exec block.
     *
     *  @param seqID [in] Basic block ID within the exec block.
     */
    void selectSeq(uint16_t seqID);

    /*! Get a pointer to the context structure stored in the data block.
     *  
     * @return The context pointer.
     */
    Context* getContext() const {return context;}

    /*! Allocate a new shadow within the data block. Used by relocation to load or store data from
     *  the instrumented code.
     *
     * @param tag The tag associated with the registration, 0xFFFF is reserved for unregistered shadows.
     *
     *  @return The shadow id (which is its index within the shadow array).
     */
    uint16_t newShadow(uint16_t tag = NO_REGISTRATION);

    /*! Set the value of a shadow.
     *
     *  @param id [in] ID of the shadow to set.
     *  @param v  [in] Value to assigne to the shadow.
     */
    void setShadow(uint16_t id, rword v);

    /*! Get the value of a shadow.
     *
     *  @param id [in] ID of the shadow.
     *
     *  @return Value of the shadow.
     */
    rword getShadow(uint16_t id) const;

    /*! Get the offset of a shadow within the data block.
     *
     *  @param id [in] ID of the shadow.
     *
     *  @return Offset of the shadow.
     */
    rword getShadowOffset(uint16_t id) const;

    /* Query registered shadows and returns a vector of matching shadowID
     *
     * @param
     *
     * @return a vector of shadowID matching the query
     */
    std::vector<ShadowInfo> queryShadowByInst(uint16_t instID, uint16_t tag) const;

    /* Query registered shadows and returns a vector of matching shadowID
     *
     * @param
     *
     * @return a vector of ShadowInfo matching the query
     */
    std::vector<ShadowInfo> queryShadowBySeq(uint16_t seqID, uint16_t tag) const;

    /* Compute the occupation ratio of the ExecBlock.
     *
     * @return the occupation ratio.
    */
    float occupationRatio() const;
};

}

#endif //EXECBLOCK_H
