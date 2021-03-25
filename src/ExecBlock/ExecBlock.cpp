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

#include "llvm/MC/MCInst.h"
#include "llvm/Support/Format.h"

#include "ExecBlock/ExecBlock.h"
#include "Patch/ExecBlockFlags.h"
#include "Patch/Patch.h"
#include "Patch/PatchGenerator.h"
#include "Patch/PatchRule.h"
#include "Patch/PatchRules_Target.h"
#include "Patch/RelocatableInst.h"
#include "Utility/Assembly.h"
#include "Utility/InstAnalysis_prive.h"
#include "Utility/LogSys.h"
#include "Utility/System.h"

#include "QBDI/Memory.hpp"
#include "QBDI/Options.h"
#include "QBDI/Platform.h"

#if defined(QBDI_PLATFORM_WINDOWS)
    extern "C" void qbdi_runCodeBlock(void *codeBlock, QBDI::rword execflags);
#else
    extern void qbdi_runCodeBlock(void *codeBlock, QBDI::rword execflags) asm ("__qbdi_runCodeBlock");
#endif

namespace QBDI {

ExecBlock::ExecBlock(const Assembly& assembly, VMInstanceRef vminstance,
                     const std::vector<std::unique_ptr<RelocatableInst>>* execBlockPrologue,
                     const std::vector<std::unique_ptr<RelocatableInst>>* execBlockEpilogue,
                     uint32_t epilogueSize_)
      : vminstance(vminstance), assembly(assembly), epilogueSize(epilogueSize_) {

    // Allocate memory blocks
    std::error_code ec;
    // iOS now use 16k superpages, but as JIT mecanisms are totally differents
    // on this platform, we can enforce a 4k "virtual" page size
    uint64_t pageSize = is_ios ? 4096 :
        llvm::expectedToOptional(llvm::sys::Process::getPageSize()).getValueOr(4096);
    unsigned mflags =  PF::MF_READ | PF::MF_WRITE;

    if constexpr(is_ios)
        mflags |= PF::MF_EXEC;

    // Allocate 2 pages block
    codeBlock = QBDI::allocateMappedMemory(2*pageSize, nullptr, mflags, ec);
    QBDI_REQUIRE_ACTION(codeBlock.base() != nullptr, abort());
    // Split it in two blocks
    dataBlock = llvm::sys::MemoryBlock(reinterpret_cast<void*>(reinterpret_cast<uint64_t>(codeBlock.base()) + pageSize), pageSize);
    codeBlock = llvm::sys::MemoryBlock(codeBlock.base(), pageSize);
    QBDI_DEBUG("codeBlock @ 0x{:x} | dataBlock @ 0x{:x}", reinterpret_cast<rword>(codeBlock.base()), reinterpret_cast<rword>(dataBlock.base()));

    // Other initializations
    context = static_cast<Context*>(dataBlock.base());
    shadows = reinterpret_cast<rword*>(reinterpret_cast<rword>(dataBlock.base()) + sizeof(Context));
    shadowIdx = 0;
    currentSeq = 0;
    currentInst = 0;
    codeStream = std::make_unique<memory_ostream>(codeBlock);
    pageState = RW;

    std::vector<std::unique_ptr<RelocatableInst>> execBlockPrologue_;
    std::vector<std::unique_ptr<RelocatableInst>> execBlockEpilogue_;

    if (execBlockPrologue == nullptr) {
        execBlockPrologue_ = getExecBlockPrologue(assembly.getOptions());
        execBlockPrologue = &execBlockPrologue_;
    }
    if (execBlockEpilogue == nullptr) {
        execBlockEpilogue_ = getExecBlockEpilogue(assembly.getOptions());
        execBlockEpilogue = &execBlockEpilogue_;
    }
    if(epilogueSize == 0) {
        // Only way to know the epilogue size is to JIT is somewhere
        for(const auto &inst: *execBlockEpilogue) {
            assembly.writeInstruction(inst->reloc(this), codeStream.get());
        }
        epilogueSize = codeStream->current_pos();
        codeStream->seek(0);
        QBDI_DEBUG("Detect Epilogue size: {}", epilogueSize);
    }
    // JIT prologue and epilogue
    codeStream->seek(codeBlock.allocatedSize() - epilogueSize);
    for(const auto &inst: *execBlockEpilogue) {
        assembly.writeInstruction(inst->reloc(this), codeStream.get());
    }
    QBDI_REQUIRE_ACTION(codeStream->current_pos() == codeBlock.allocatedSize() && "Wrong Epilogue Size", abort());

    codeStream->seek(0);
    for(const auto &inst: *execBlockPrologue) {
        assembly.writeInstruction(inst->reloc(this), codeStream.get());
    }
}

ExecBlock::~ExecBlock() {
    // Reunite the 2 blocks before freeing them
    codeBlock = llvm::sys::MemoryBlock(codeBlock.base(), codeBlock.allocatedSize() + dataBlock.allocatedSize());
    QBDI::releaseMappedMemory(codeBlock);
}

void ExecBlock::changeVMInstanceRef(VMInstanceRef vminstance) {
    this->vminstance = vminstance;
}

void ExecBlock::show() const {
    rword i;
    uint64_t instSize;
    llvm::MCDisassembler::DecodeStatus dstatus;
    llvm::ArrayRef<uint8_t> jitCode(static_cast<const uint8_t*>(codeBlock.base()), codeStream->current_pos());

    fprintf(stderr, "---- JIT CODE ----\n");
    for (i = 0; i < jitCode.size(); i += instSize)
    {
        llvm::MCInst inst;
        std::string disass;

        dstatus = assembly.getInstruction(inst, instSize, jitCode.slice(i), i);
        QBDI_REQUIRE_ACTION(dstatus != llvm::MCDisassembler::Fail,
            break
        );

        disass = assembly.showInst(inst, reinterpret_cast<uint64_t>(codeBlock.base()) + i);
        fprintf(stderr, "%s\n", disass.c_str());
    }

    fprintf(stderr, "---- CONTEXT ----\n");
    for (i = 0; i < NUM_GPR; i++) {
        fprintf(stderr, "%s=0x%016" PRIRWORD " ", assembly.getRegisterName(GPR_ID[i]), QBDI_GPR_GET(&context->gprState, i));
        if(i % 4 == 0) fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "---- SHADOWS ----\n[");
    for (i = 0; i+1 < shadowIdx; i ++)
        fprintf(stderr, "0x%016" PRIRWORD ", ", shadows[i]);
    if(shadowIdx > 0)
        fprintf(stderr, "0x%016" PRIRWORD, shadows[i-1]);
    fprintf(stderr, "]\n");
}

void ExecBlock::selectSeq(uint16_t seqID) {
    QBDI_REQUIRE(seqID < seqRegistry.size());
    currentSeq = seqID;
    currentInst = seqRegistry[currentSeq].startInstID;
    context->hostState.selector = reinterpret_cast<rword>(codeBlock.base()) + static_cast<rword>(instRegistry[seqRegistry[seqID].startInstID].offset);
    context->hostState.executeFlags = seqRegistry[currentSeq].executeFlags;
}

void ExecBlock::run() {
    // Pages are RWX on iOS
    if constexpr(is_ios)
        llvm::sys::Memory::InvalidateInstructionCache(codeBlock.base(), codeBlock.allocatedSize());
    else
        makeRX();
    qbdi_runCodeBlock(codeBlock.base(), context->hostState.executeFlags);
}

VMAction ExecBlock::execute() {
    QBDI_DEBUG("Executing ExecBlock 0x{:x} programmed with selector at 0x{:x}",
             reinterpret_cast<uintptr_t>(this), context->hostState.selector);
    do {
        context->hostState.callback = (rword) 0;
        context->hostState.data = (rword) 0;

        QBDI_DEBUG("Execution of ExecBlock 0x{:x} resumed at 0x{:x}",
                 reinterpret_cast<uintptr_t>(this), context->hostState.selector);
        run();

        if(context->hostState.callback != 0) {
            currentInst = context->hostState.origin;

            QBDI_DEBUG("Callback request by ExecBlock 0x{:x} for callback 0x{:x}",
                     reinterpret_cast<uintptr_t>(this), context->hostState.callback);
            QBDI_REQUIRE(currentInst < instMetadata.size());

            VMAction r = (reinterpret_cast<InstCallback>(context->hostState.callback))(
                vminstance,
                &context->gprState, &context->fprState,
               (void*) context->hostState.data
            );

            switch(r) {
                case CONTINUE:
                    QBDI_DEBUG("Callback 0x{:x}"" returned CONTINUE", context->hostState.callback);
                    break;
                case BREAK_TO_VM:
                    QBDI_DEBUG("Callback 0x{:x}"" returned BREAK_TO_VM", context->hostState.callback);
                    return BREAK_TO_VM;
                case STOP:
                    QBDI_DEBUG("Callback 0x{:x}"" returned STOP", context->hostState.callback);
                    return STOP;
            }
        }
    } while(context->hostState.callback != 0);
    currentInst = seqRegistry[currentSeq].endInstID;

    return CONTINUE;
}

SeqWriteResult ExecBlock::writeSequence(std::vector<Patch>::const_iterator seqIt, std::vector<Patch>::const_iterator seqEnd) {
    rword startOffset = (rword)codeStream->current_pos();
    uint16_t startInstID = getNextInstID();
    uint16_t seqID = getNextSeqID();
    uint8_t executeFlags = 0;
    unsigned patchWritten = 0;

    // Refuse to write empty sequence
    if(seqIt == seqEnd) {
        QBDI_WARN("Attempting to write empty sequence");
        return {EXEC_BLOCK_FULL, 0, 0};
    }

    // Check if there's enough space left
    if(getEpilogueOffset() <= MINIMAL_BLOCK_SIZE) {
        QBDI_DEBUG("ExecBlock 0x{:x} is full", reinterpret_cast<uintptr_t>(this));
        return {EXEC_BLOCK_FULL, 0, 0};
    }
    QBDI_DEBUG("Attempting to write {} patches to ExecBlock 0x{:x}", std::distance(seqIt, seqEnd), reinterpret_cast<uintptr_t>(this));
    // Pages are RWX on iOS
    // Ensure code block is RW
    if constexpr(not is_ios) {
        makeRW();
    }
    bool needTerminator = true;
    // JIT the basic block instructions patch per patch
    // A patch correspond to an original instruction and should be written in its entierty
    while(seqIt != seqEnd) {
        bool rollback = false;
        rword rollbackOffset = codeStream->current_pos();
        uint32_t rollbackShadowIdx = shadowIdx;
        size_t rollbackShadowRegistry = shadowRegistry.size();

        QBDI_DEBUG_BLOCK({
            std::string disass =  assembly.showInst(seqIt->metadata.inst, seqIt->metadata.address);
            QBDI_DEBUG("Attempting to write patch of {} RelocatableInst to ExecBlock 0x{:x} for instruction {:x}: {}",
                    seqIt->metadata.patchSize, reinterpret_cast<uintptr_t>(this), seqIt->metadata.address, disass.c_str());
        });
        // Attempt to write a complete patch. If not, rollback to the last complete patch written
        for(const RelocatableInst::UniquePtr& inst : seqIt->insts) {
            if(getEpilogueOffset() > MINIMAL_BLOCK_SIZE) {
                assembly.writeInstruction(inst->reloc(this), codeStream.get());
            }
            else {
                // Not enough space left, rollback
                rollback = true;
                break;
            }
        }

        if(rollback) {
            QBDI_DEBUG("Not enough space left, rolling back to offset 0x{:x}", rollbackOffset);
            // Seek to the last complete patch written and terminate it with a terminator
            codeStream->seek(rollbackOffset);
            // free shadows allocated by the rollbacked code
            shadowIdx = rollbackShadowIdx;
            shadowRegistry.resize(rollbackShadowRegistry);
            // It's a NULL rollback, don't terminate it
            if(rollbackOffset == startOffset) {
                QBDI_DEBUG("NULL rollback, nothing written to ExecBlock 0x{:x}", reinterpret_cast<uintptr_t>(this));
                return {EXEC_BLOCK_FULL, 0, 0};
            }
            needTerminator = true;
            break;
        } else {
            // Complete instruction was written, we add the metadata
            // Move the analysis of the instruction in the cached metadata
            instMetadata.push_back(seqIt->metadata.lightCopy());
            instMetadata.back().analysis.reset(seqIt->metadata.analysis.release());
            // Register instruction
            instRegistry.push_back(InstInfo {seqID, static_cast<uint16_t>(rollbackOffset),
                                             static_cast<uint16_t>(rollbackShadowRegistry),
                                             static_cast<uint16_t>(shadowRegistry.size() - rollbackShadowRegistry)});
            // Update indexes
            needTerminator = not seqIt->metadata.modifyPC;
            executeFlags |= seqIt->metadata.execblockFlags;
            seqIt++;
            patchWritten += 1;
        }
    }
    // The last instruction of the sequence doesn't end with a change of RIP/PC, add a Terminator
    if (needTerminator) {
        QBDI_DEBUG("Writting terminator to ExecBlock 0x{:x} to finish non-exit sequence", reinterpret_cast<uintptr_t>(this));
        RelocatableInst::UniquePtrVec terminator = getTerminator(instMetadata.back().endAddress());
        for(const RelocatableInst::UniquePtr &inst : terminator) {
            assembly.writeInstruction(inst->reloc(this), codeStream.get());
        }
    }
    // JIT the jump to epilogue
    RelocatableInst::UniquePtrVec jmpEpilogue = JmpEpilogue();
    for(const RelocatableInst::UniquePtr &inst : jmpEpilogue) {
        assembly.writeInstruction(inst->reloc(this), codeStream.get());
    }
    // change the flag of the basicblock
    if (assembly.getOptions() & Options::OPT_DISABLE_FPR) {
        executeFlags = 0;
    } else if (assembly.getOptions() & Options::OPT_DISABLE_OPTIONAL_FPR) {
        executeFlags = defaultExecuteFlags;
    }
    // Register sequence
    uint16_t endInstID = getNextInstID() - 1;
    seqRegistry.push_back(SeqInfo {startInstID, endInstID, executeFlags});
    // Return write results
    unsigned bytesWritten = static_cast<unsigned>(codeStream->current_pos() - startOffset);
    QBDI_DEBUG("End write sequence in basicblock 0x{:x} with execFlags : {:x}", reinterpret_cast<uintptr_t>(this), executeFlags);
    return SeqWriteResult {seqID, bytesWritten, patchWritten};
}

uint16_t ExecBlock::splitSequence(uint16_t instID) {
    QBDI_REQUIRE(instID < instRegistry.size());
    uint16_t seqID = instRegistry[instID].seqID;
    seqRegistry.push_back(SeqInfo {
        instID,
        seqRegistry[seqID].endInstID,
        seqRegistry[seqID].executeFlags
    });
    return getNextSeqID() - 1;
}

void ExecBlock::makeRX() {
    QBDI_DEBUG("Making ExecBlock 0x{:x} RX", reinterpret_cast<uintptr_t>(this));
    if(pageState != RX) {
        QBDI_REQUIRE_ACTION(
            !llvm::sys::Memory::protectMappedMemory(codeBlock, PF::MF_READ | PF::MF_EXEC),
            abort()
        );
        pageState = RX;
    }
}

void ExecBlock::makeRW() {
    QBDI_DEBUG("Making ExecBlock 0x{:x} RW", reinterpret_cast<uintptr_t>(this));
    if(pageState != RW) {
        QBDI_REQUIRE_ACTION(
            !llvm::sys::Memory::protectMappedMemory(codeBlock, PF::MF_READ | PF::MF_WRITE),
            abort()
        );
        pageState = RW;
    }
}

uint16_t ExecBlock::newShadow(uint16_t tag) {
    uint16_t id = shadowIdx++;
    QBDI_REQUIRE_ACTION(id * sizeof(rword) < dataBlock.allocatedSize() - sizeof(Context), abort());
    if(tag != ShadowReservedTag::Untagged) {
        QBDI_DEBUG("Registering new tagged shadow {:x} for instID {:x} wih tag {:x}", id, getNextInstID(), tag);
        shadowRegistry.push_back({
            getNextInstID(),
            tag,
            id
        });
    }
    return id;
}

uint16_t ExecBlock::getLastShadow(uint16_t tag) {
    uint16_t nextInstID = getNextInstID();

    for (auto it = shadowRegistry.crbegin(); it != shadowRegistry.crend(); ++it)  {
        if (it->instID == nextInstID && it->tag == tag) {
            return it->shadowID;
        }
    }
    QBDI_ERROR("Cannot found shadow tag {:x} for the current instruction", tag);
    abort();
}

void ExecBlock::setShadow(uint16_t id, rword v) {
    QBDI_REQUIRE_ACTION(id * sizeof(rword) < dataBlock.allocatedSize() - sizeof(Context), abort());
    shadows[id] = v;
}

rword ExecBlock::getShadow(uint16_t id) const {
    QBDI_REQUIRE_ACTION(id * sizeof(rword) < dataBlock.allocatedSize() - sizeof(Context), abort());
    return shadows[id];
}

rword ExecBlock::getShadowOffset(uint16_t id) const {
    rword offset = sizeof(Context) + id*sizeof(rword);
    QBDI_REQUIRE_ACTION(offset < dataBlock.allocatedSize(), abort());
    return offset;
}

uint16_t ExecBlock::getInstID(rword address) const {
    for(size_t i = 0; i < instMetadata.size(); i++) {
        if(instMetadata[i].address == address) {
            return (uint16_t) i;
        }
    }
    return NOT_FOUND;
}

const InstMetadata& ExecBlock::getInstMetadata(uint16_t instID) const {
    QBDI_REQUIRE(instID < instMetadata.size());
    return instMetadata[instID];
}

rword ExecBlock::getInstAddress(uint16_t instID) const {
    QBDI_REQUIRE(instID < instMetadata.size());
    return instMetadata[instID].address;
}

const llvm::MCInst& ExecBlock::getOriginalMCInst(uint16_t instID) const {
    QBDI_REQUIRE(instID < instMetadata.size());
    return instMetadata[instID].inst;
}

const InstAnalysis* ExecBlock::getInstAnalysis(uint16_t instID, AnalysisType type) const {
    QBDI_REQUIRE(instID < instMetadata.size());
    return analyzeInstMetadata(instMetadata[instID], type, assembly);
}

uint16_t ExecBlock::getSeqID(rword address) const {
    for(size_t i = 0; i < seqRegistry.size(); i++) {
        if(instMetadata[seqRegistry[i].startInstID].address == address) {
            return (uint16_t) i;
        }
    }
    return NOT_FOUND;
}

uint16_t ExecBlock::getSeqID(uint16_t instID) const {
    QBDI_REQUIRE(instID < instRegistry.size());
    return instRegistry[instID].seqID;
}

uint16_t ExecBlock::getSeqStart(uint16_t seqID) const {
    QBDI_REQUIRE(seqID < seqRegistry.size());
    return seqRegistry[seqID].startInstID;
}

uint16_t ExecBlock::getSeqEnd(uint16_t seqID) const {
    QBDI_REQUIRE(seqID < seqRegistry.size());
    return seqRegistry[seqID].endInstID;
}

const llvm::ArrayRef<ShadowInfo> ExecBlock::getShadowByInst(uint16_t instID) const {
    QBDI_REQUIRE(instID < instRegistry.size());
    QBDI_REQUIRE(instRegistry[instID].shadowOffset <= shadowRegistry.size());
    QBDI_REQUIRE(instRegistry[instID].shadowOffset + instRegistry[instID].shadowSize <= shadowRegistry.size());

    if (instRegistry[instID].shadowOffset == shadowRegistry.size()) {
        return llvm::ArrayRef<ShadowInfo> {};
    } else {
        return llvm::ArrayRef<ShadowInfo> { &shadowRegistry[instRegistry[instID].shadowOffset], instRegistry[instID].shadowSize };
    }
}

std::vector<ShadowInfo> ExecBlock::queryShadowByInst(uint16_t instID, uint16_t tag) const {
    std::vector<ShadowInfo> result;

    for(const auto& reg: shadowRegistry) {
        if((instID == ANY || reg.instID == instID) &&
           (tag    == ANY || reg.tag    == tag)) {
            result.push_back(reg);
        }
    }

    return result;
}

std::vector<ShadowInfo> ExecBlock::queryShadowBySeq(uint16_t seqID, uint16_t tag) const {
    std::vector<ShadowInfo> result;

    if (seqID == ANY) {
        for(const auto& reg: shadowRegistry) {
            if(tag == ANY || reg.tag == tag) {
                result.push_back(reg);
            }
        }
    } else {
        uint16_t firstInstID = getSeqStart(seqID);
        uint16_t lastInstID = getSeqEnd(seqID);
        for(const auto& reg: shadowRegistry) {
            if(firstInstID <= reg.instID && reg.instID <= lastInstID &&
               (tag == ANY || reg.tag == tag)) {
                result.push_back(reg);
            }
        }

    }

    return result;
}

float ExecBlock::occupationRatio() const {
    return static_cast<float>(codeBlock.allocatedSize() - getEpilogueOffset()) / static_cast<float>(codeBlock.allocatedSize());
}

}
