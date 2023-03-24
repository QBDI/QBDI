/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
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

#include <memory>
#include <stdint.h>
#include <vector>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/Memory.h"

#include "Patch/InstMetadata.h"
#include "Patch/Types.h"

#include "QBDI/Callback.h"
#include "QBDI/Config.h"
#include "QBDI/InstAnalysis.h"
#include "QBDI/State.h"

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
#include "ExecBlock/X86_64/ScratchRegisterInfo_X86_64.h"
#elif defined(QBDI_ARCH_ARM)
#include "ExecBlock/ARM/ScratchRegisterInfo_ARM.h"
#elif defined(QBDI_ARCH_AARCH64)
#include "ExecBlock/AARCH64/ScratchRegisterInfo_AARCH64.h"
#else
#error "No ScratchRegisterInfo for this architecture"
#endif

namespace llvm {
class MCInst;
}

namespace QBDI {

class LLVMCPUs;
class LLVMCPU;
class RelocatableInst;
class Patch;

struct Context;

struct InstInfo {
  uint16_t seqID;
  uint16_t offset;
  uint16_t offsetSkip;
  uint16_t shadowOffset;
  uint16_t shadowSize;
  uint16_t tagOffset;
  uint16_t tagSize;
  ScratchRegisterSeqInfo sr;
};

struct SeqInfo {
  uint16_t startInstID;
  uint16_t endInstID;
  uint8_t executeFlags;
  CPUMode cpuMode;
  ScratchRegisterSeqInfo sr;
};

struct SeqWriteResult {
  uint16_t seqID;
  unsigned bytesWritten;
  unsigned patchWritten;
};

struct ShadowInfo {
  uint16_t instID;
  uint16_t tag;
  uint16_t shadowID;
};

struct TagInfo {
  uint16_t tag;
  uint16_t offset;
};

static const uint16_t EXEC_BLOCK_FULL = 0xFFFF;

/*! Manages the concept of an exec block made of two contiguous memory blocks
 * (one for the code, the other for the data) used to store and execute
 * instrumented basic blocks.
 */
class ExecBlock {
private:
  using PF = llvm::sys::Memory::ProtectionFlags;

  enum PageState { RX, RW };

  VMInstanceRef vminstance;
  llvm::sys::MemoryBlock codeBlock;
  llvm::sys::MemoryBlock dataBlock;
  unsigned codeBlockPosition;
  unsigned codeBlockMaxSize;
  const LLVMCPUs &llvmCPUs;
  Context *context;
  rword *shadows;
  std::vector<ShadowInfo> shadowRegistry;
  std::vector<TagInfo> tagRegistry;
  uint16_t shadowIdx;
  std::vector<InstMetadata> instMetadata;
  std::vector<InstInfo> instRegistry;
  std::vector<SeqInfo> seqRegistry;
  PageState pageState;
  uint16_t currentSeq;
  uint16_t currentInst;
  uint32_t epilogueSize;
  bool isFull;
  ScratchRegisterInfo srInfo;

  /*! Verify if the code block is in read execute mode.
   *
   * @return Return true if the code block is in read execute mode.
   */
  inline bool isRX() const { return pageState == RX; }

  /*! Verify if the code block is in read write mode.
   *
   * @return Return true if the code block is in read write mode.
   */
  inline bool isRW() const { return pageState == RW; }

  /*! Changes the code block permissions to RX.
   */
  void makeRX();

  /*! Changes the code block permissions to RW.
   */
  void makeRW();

  void initScratchRegisterForPatch(std::vector<Patch>::const_iterator seqStart,
                                   std::vector<Patch>::const_iterator seqEnd);

  bool writePatch(std::vector<Patch>::const_iterator seqCurrent,
                  std::vector<Patch>::const_iterator seqEnd,
                  const LLVMCPU &llvmcpu);

  bool writeCodeByte(const llvm::ArrayRef<char> &);

  bool
  applyRelocatedInst(const std::vector<std::unique_ptr<RelocatableInst>> &reloc,
                     std::vector<TagInfo> *tags, const LLVMCPU &llvmcpu,
                     unsigned limit = 0);

  void finalizeScratchRegisterForPatch();

public:
  /*! Construct a new ExecBlock
   *
   * @param[in] llvmCPUs           LLVMCPU used to assemble instructions in the
   *                               ExecBlock.
   * @param[in] vminstance         Pointer to public engine interface
   * @param[in] execBlockPrologue  cached prologue of ExecManager
   * @param[in] execBlockEpilogue  cached epilogue of ExecManager
   * @param[in] epilogueSize       size in bytes of the epilogue (0 is not know)
   */
  ExecBlock(
      const LLVMCPUs &llvmCPUs, VMInstanceRef vminstance,
      const std::vector<std::unique_ptr<RelocatableInst>> *execBlockPrologue =
          nullptr,
      const std::vector<std::unique_ptr<RelocatableInst>> *execBlockEpilogue =
          nullptr,
      uint32_t epilogueSize = 0);

  ~ExecBlock();

  ExecBlock(const ExecBlock &) = delete;
  ExecBlock &operator=(const ExecBlock &) = delete;

  /*! Change vminstance when VM object is moved
   */
  void changeVMInstanceRef(VMInstanceRef vminstance);

  /*! Display the content of an exec block to stderr.
   */
  void show() const;

  /* Low level run function. Does not take care of the callbacks.
   */
  void run();

  /*! Execute the sequence currently programmed in the selector of the exec
   * block. Take care of the callbacks handling.
   */
  VMAction execute();

  /*! Write a new sequence in the exec block. This function does not guarantee
   * that the sequence will be written in its entierty and might stop before the
   * end using an architecture specific terminator. Return 0 if the exec block
   * was full and no instruction was written.
   *
   * @param seqStart [in] Iterator to the start of a list of patches.
   * @param seqEnd   [in] Iterator to the end of a list of patches.
   *
   * @return A structure detailling the write operation result.
   */
  SeqWriteResult writeSequence(std::vector<Patch>::const_iterator seqStart,
                               std::vector<Patch>::const_iterator seqEnd);

  /*! Split an existing sequence at instruction instID to create a new sequence.
   *
   * @param instID  [in] ID of the instruction where to split the sequence at.
   *
   * @return The new sequence ID.
   */
  uint16_t splitSequence(uint16_t instID);

  /*! Get the address of the DataBlock
   *
   * @return The DataBlock offset.
   */
  rword getDataBlockBase() const {
    return reinterpret_cast<rword>(dataBlock.base());
  }

  /*! Compute the offset between the current code stream position and the start
   * of the data block. Used for pc relative memory access to the data block.
   *
   * @return The computed offset.
   */
  rword getDataBlockOffset() const {
    return codeBlock.allocatedSize() - codeBlockPosition;
  }

  /*! Compute the offset between the current code stream position and the start
   * of the exec block epilogue code. Used for computing the remaining code
   * space left or jumping to the exec block epilogue at the end of a sequence.
   *
   * @return The computed offset.
   */
  rword getEpilogueOffset() const {
    return codeBlock.allocatedSize() - epilogueSize - codeBlockPosition;
  }

  /*! Get the size of the epilogue
   *
   * @return The size of the epilogue.
   */
  uint32_t getEpilogueSize() const { return epilogueSize; }

  /*! Obtain the value of the PC where the ExecBlock is currently writing
   * instructions.
   *
   * @return The PC value.
   */
  rword getCurrentPC() const {
    return reinterpret_cast<rword>(codeBlock.base()) + codeBlockPosition;
  }

  /*! Obtain the current instruction ID.
   *
   * @return The current instruction ID.
   */
  uint16_t getNextInstID() const {
    return static_cast<uint16_t>(instMetadata.size());
  }

  /*! Obtain the instruction ID for a specific address (the address must exactly
   * match the start of the instruction).
   *
   * @param address The address of the start of the instruction.
   * @param cpumode The mode of the instruction
   *
   * @return The instruction ID or NOT_FOUND.
   */
  uint16_t getInstID(rword address, CPUMode cpumode) const;

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
  const InstMetadata &getInstMetadata(uint16_t instID) const;

  /*! Obtain the instruction address for a specific instruction ID.
   *
   * @param instID The instruction ID.
   *
   * @return The real address of the instruction.
   */
  rword getInstAddress(uint16_t instID) const;

  /*! Obtain the instrumented address for a specific instruction ID.
   *
   * @param instID The instruction ID.
   *
   * @return The address in the BasicBlock of the instruction.
   */
  rword getInstInstrumentedAddress(uint16_t instID) const;

  /*! Obtain the original MCInst for a specific instruction ID.
   *
   * @param instID The instruction ID.
   *
   * @return The original MCInst of the instruction.
   */
  const llvm::MCInst &getOriginalMCInst(uint16_t instID) const;

  /*! Obtain the analysis of an instruction. Analysis results are
   * cached in the InstAnalysis. The validity of the returned pointer is only
   * guaranteed until the end of the callback, else a deepcopy of the structure
   * is required.
   *
   * @param[in] instID  The ID of the instruction to analyse.
   * @param[in] type    Properties to retrieve during analysis.
   *
   * @return A InstAnalysis structure containing the analysis result.
   */
  const InstAnalysis *getInstAnalysis(uint16_t instID, AnalysisType type) const;

  /*! Obtain the next sequence ID.
   *
   * @return The next sequence ID.
   */
  uint16_t getNextSeqID() const {
    return static_cast<uint16_t>(seqRegistry.size());
  }

  /*! Obtain the sequence ID for a specific address (the address must exactly
   * match the start of the sequence).
   *
   * @param address The address of the start of the sequence.
   *
   * @return The sequence ID or NOT_FOUND.
   */
  uint16_t getSeqID(rword address, CPUMode cpumode) const;

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

  /*! Obtain the sequence start address for a specific sequence ID.
   *
   * @param seqID The sequence ID.
   *
   * @return The start address of the sequence.
   */
  uint16_t getSeqStart(uint16_t seqID) const;

  /*! Obtain the instruction id of the sequence end address for a specific
   * sequence ID.
   *
   * @param seqID The sequence ID.
   *
   * @return The end address of the sequence.
   */
  uint16_t getSeqEnd(uint16_t seqID) const;

  /*! Set the selector of the exec block to a specific sequence offset. Used to
   * program the execution of a specific sequence within the exec block.
   *
   *  @param seqID [in] Basic block ID within the exec block.
   */
  void selectSeq(uint16_t seqID);

  /*! Get a pointer to the context structure stored in the data block.
   *
   * @return The context pointer.
   */
  Context *getContext() const { return context; }

  /*! Allocate a new shadow within the data block. Used by relocation to load or
   * store data from the instrumented code.
   *
   * @param tag The tag associated with the registration, 0xFFFF is reserved for
   * unregistered shadows.
   *
   *  @return The shadow id (which is its index within the shadow array).
   */
  uint16_t newShadow(uint16_t tag = ShadowReservedTag::Untagged);

  /*! Search the last Shadow with the tag for the current instruction.
   *  Used by relocation to load or store data from the instrumented code.
   *
   *  @param tag The tag associated with the registration
   *
   *  @return The shadow id (which is its index within the shadow array).
   */
  uint16_t getLastShadow(uint16_t tag);

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

  /* Get all registered shadows for an instruction
   *
   * @param instID  The id of the instruction in the ExecBlock
   *
   * @return a vector of shadowID matching the query
   */
  const llvm::ArrayRef<ShadowInfo> getShadowByInst(uint16_t instID) const;

  /* Query registered shadows and returns a vector of matching shadowID
   *
   * @param
   *
   * @return a vector of shadowID matching the query
   */
  std::vector<ShadowInfo> queryShadowByInst(uint16_t instID,
                                            uint16_t tag) const;

  /* Query registered shadows and returns a vector of matching shadowID
   *
   * @param
   *
   * @return a vector of ShadowInfo matching the query
   */
  std::vector<ShadowInfo> queryShadowBySeq(uint16_t seqID, uint16_t tag) const;

  /* Get all registered tag for an instruction
   *
   * @param instID  The id of the instruction in the ExecBlock
   *
   * @return a vector of tag matching the query
   */
  const llvm::ArrayRef<TagInfo> getTagByInst(uint16_t instID) const;

  /* Query registered tag and returns a vector of matching tagID
   *
   * @param instID  The id of the instruction in the ExecBlock
   * @param tag     The tag to search
   *
   * @return a vector of tagID matching the query
   */
  std::vector<TagInfo> queryTagByInst(uint16_t instID, uint16_t tag) const;

  /* Get TagInfo JITTed address
   *
   * @param tinfo A TagInfo of this basicblock
   *
   * @return The address of the Tag in this ExecBlock
   */
  inline rword getAddressTag(const TagInfo &tinfo) const {
    return reinterpret_cast<rword>(codeBlock.base()) + tinfo.offset;
  }

  /* Compute the occupation ratio of the ExecBlock.
   *
   * @return the occupation ratio.
   */
  float occupationRatio() const;

  const ScratchRegisterInfo &getScratchRegisterInfo() const { return srInfo; }

  /* Get the LLVMCPU used by an instruction
   *
   * @param instID  The id of the instruction in the ExecBlock
   *
   * @return the LLVMCPU for the instruction
   */
  const LLVMCPU &getLLVMCPUByInst(uint16_t instID) const;
};

} // namespace QBDI

#endif // EXECBLOCK_H
