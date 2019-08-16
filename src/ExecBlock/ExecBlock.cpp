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
#include "llvm/Support/Format.h"
#include "Patch/PatchRule.h"
#include "ExecBlock.h"
#include "Patch/Patch.h"
#include "Platform.h"
#include "Memory.hpp"
#include "Utility/LogSys.h"
#include "Utility/System.h"

#if defined(QBDI_OS_WIN)
    #if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
        extern "C" void qbdi_runCodeBlockSSE(void *codeBlock, QBDI::rword execflags);
        extern "C" void qbdi_runCodeBlockAVX(void *codeBlock, QBDI::rword execflags);
    #else
        extern "C" void qbdi_runCodeBlock(void *codeBlock, QBDI::rword execflags);
    #endif
#else
    #if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
        extern void qbdi_runCodeBlockSSE(void *codeBlock, QBDI::rword execflags) asm ("__qbdi_runCodeBlockSSE");
        extern void qbdi_runCodeBlockAVX(void *codeBlock, QBDI::rword execflags) asm ("__qbdi_runCodeBlockAVX");
    #else
        extern void qbdi_runCodeBlock(void *codeBlock, QBDI::rword execflags) asm ("__qbdi_runCodeBlock");
    #endif
#endif

namespace QBDI {

uint32_t ExecBlock::epilogueSize = 0;
RelocatableInst::SharedPtrVec ExecBlock::execBlockPrologue = RelocatableInst::SharedPtrVec();
RelocatableInst::SharedPtrVec ExecBlock::execBlockEpilogue = RelocatableInst::SharedPtrVec();
void (*ExecBlock::runCodeBlockFct)(void*, rword) = NULL;

ExecBlock::ExecBlock(Assembly &assembly, VMInstanceRef vminstance) : vminstance(vminstance), assembly(assembly) {
    // Allocate memory blocks
    std::error_code ec;
#ifdef QBDI_OS_IOS
    // iOS now use 16k superpages, but as JIT mecanisms are totally differents
    // on this platform, we can enforce a 4k "virtual" page size
    uint64_t pageSize = 4096;
#else
    uint64_t pageSize = llvm::sys::Process::getPageSize();
#endif
    unsigned mflags =  PF::MF_READ | PF::MF_WRITE;
#ifdef QBDI_OS_IOS
             mflags |= PF::MF_EXEC;
#endif

    // Allocate 2 pages block
    codeBlock = QBDI::allocateMappedMemory(2*pageSize, nullptr, mflags, ec);
    RequireAction("ExecBlock::ExecBlock", codeBlock.base() != nullptr, abort());
    // Split it in two blocks
    dataBlock = llvm::sys::MemoryBlock(reinterpret_cast<void*>(reinterpret_cast<uint64_t>(codeBlock.base()) + pageSize), pageSize);
    codeBlock = llvm::sys::MemoryBlock(codeBlock.base(), pageSize);
    LogDebug("ExecBlock::ExecBlock", "codeBlock @ 0x%" PRIRWORD " | dataBlock @ 0x%" PRIRWORD, reinterpret_cast<rword>(codeBlock.base()), reinterpret_cast<rword>(dataBlock.base()));

    // Other initializations
    context = static_cast<Context*>(dataBlock.base());
    shadows = reinterpret_cast<rword*>(reinterpret_cast<rword>(dataBlock.base()) + sizeof(Context));
    shadowIdx = 0;
    currentSeq = 0;
    currentInst = 0;
    codeStream = new memory_ostream(codeBlock);
    pageState = RW;

    // Epilogue and prologue management.
    // If epilogueSize == 0 then static members are not yet initialized
    if(epilogueSize == 0) {
        execBlockPrologue = getExecBlockPrologue();
        execBlockEpilogue = getExecBlockEpilogue();
        // Only way to know the epilogue size is to JIT is somewhere
        for(auto &inst: execBlockEpilogue) {
            assembly.writeInstruction(inst->reloc(this), codeStream);
        }
        epilogueSize = codeStream->current_pos();
        codeStream->seek(0);
        // runCodeBlock variant selection
        #if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
        if(isHostCPUFeaturePresent("avx")) {
            LogDebug("ExecBlock::ExecBlock", "AVX support enabled in host context switches");
            runCodeBlockFct = qbdi_runCodeBlockAVX;
        }
        else {
            runCodeBlockFct = qbdi_runCodeBlockSSE;
        }
        #else
        runCodeBlockFct = qbdi_runCodeBlock;
        #endif
    }
    // JIT prologue and epilogue
    codeStream->seek(codeBlock.size() - epilogueSize);
    for(auto &inst: execBlockEpilogue) {
        assembly.writeInstruction(inst->reloc(this), codeStream);
    }
    codeStream->seek(0);
    for(auto &inst: execBlockPrologue) {
        assembly.writeInstruction(inst->reloc(this), codeStream);
    }
}

ExecBlock::~ExecBlock() {
    // Reunite the 2 blocks before freeing them
    codeBlock = llvm::sys::MemoryBlock(codeBlock.base(), codeBlock.size() + dataBlock.size());
    QBDI::releaseMappedMemory(codeBlock);
    delete codeStream;
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
        RequireAction("ExecBlock::show", dstatus != llvm::MCDisassembler::Fail,
            break
        );

        llvm::raw_string_ostream disassOs(disass);
        assembly.printDisasm(inst, disassOs);
        disassOs.flush();
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
    Require("ExecBlock::selectSeq", seqID < seqRegistry.size());
    currentSeq = seqID;
    currentInst = seqRegistry[currentSeq].startInstID;
    context->hostState.selector = reinterpret_cast<rword>(codeBlock.base()) + static_cast<rword>(instRegistry[seqRegistry[seqID].startInstID].offset);
    context->hostState.executeFlags = seqRegistry[currentSeq].executeFlags;
}

void ExecBlock::run() {
    // Pages are RWX on iOS
#ifndef QBDI_OS_IOS
    makeRX();
#else
    llvm::sys::Memory::InvalidateInstructionCache(codeBlock.base(), codeBlock.size());
#endif // QBDI_OS_IOS
    runCodeBlockFct(codeBlock.base(), context->hostState.executeFlags);
}

VMAction ExecBlock::execute() {
    LogDebug("ExecBlock::execute", "Executing ExecBlock %p programmed with selector at 0x%" PRIRWORD,
             this, context->hostState.selector);
    do {
        context->hostState.callback = (rword) 0;
        context->hostState.data = (rword) 0;

        LogDebug("ExecBlock::execute", "Execution of ExecBlock %p resumed at 0x%" PRIRWORD,
                 this, context->hostState.selector);
        run();

        if(context->hostState.callback != 0) {
            currentInst = context->hostState.origin;

            LogDebug("ExecBlock::execute", "Callback request by ExecBlock %p for callback 0x%" PRIRWORD,
                     this, context->hostState.callback);
            Require("ExecBlock::execute", currentInst < instMetadata.size());

            VMAction r = (reinterpret_cast<InstCallback>(context->hostState.callback))(
                vminstance,
                &context->gprState, &context->fprState,
               (void*) context->hostState.data
            );

            switch(r) {
                case CONTINUE:
                    LogDebug("ExecBlock::execute", "Callback 0x%" PRIRWORD" returned CONTINUE", context->hostState.callback);
                    break;
                case BREAK_TO_VM:
                    LogDebug("ExecBlock::execute", "Callback 0x%" PRIRWORD" returned BREAK_TO_VM", context->hostState.callback);
                    return BREAK_TO_VM;
                case STOP:
                    LogDebug("ExecBlock::execute", "Callback 0x%" PRIRWORD" returned STOP", context->hostState.callback);
                    return STOP;
            }
        }
    } while(context->hostState.callback != 0);
    currentInst = seqRegistry[currentSeq].endInstID;

    return CONTINUE;
}

SeqWriteResult ExecBlock::writeSequence(std::vector<Patch>::const_iterator seqIt, std::vector<Patch>::const_iterator seqEnd, SeqType seqType) {
    rword startOffset = (rword)codeStream->current_pos();
    uint16_t startInstID = getNextInstID();
    uint16_t seqID = getNextSeqID();
    uint8_t executeFlags = 0;
    unsigned patchWritten = 0;

    // Refuse to write empty sequence
    if(seqIt == seqEnd) {
        LogWarning("ExecBlock::writeSequence", "Attempting to write empty sequence");
        return {EXEC_BLOCK_FULL, 0, 0};
    }

    // Check if there's enough space left
    if(getEpilogueOffset() <= MINIMAL_BLOCK_SIZE) {
        LogDebug("ExecBlock::writeBasicBlock", "ExecBlock %p is full", this);
        return {EXEC_BLOCK_FULL, 0, 0};
    }
    LogDebug("ExecBlock::writeBasicBlock", "Attempting to write %zu patches to ExecBlock %p", std::distance(seqIt, seqEnd), this);
    // Pages are RWX on iOS
#ifndef QBDI_OS_IOS
    // Ensure code block is RW
    makeRW();
#endif // QBDI_OS_IOS
    // JIT the basic block instructions patch per patch
    // A patch correspond to an original instruction and should be written in its entierty
    while(seqIt != seqEnd) {
        bool rollback = false;
        rword rollbackOffset = codeStream->current_pos();
        uint32_t rollbackShadowIdx = shadowIdx;
        size_t rollbackShadowRegistry = shadowRegistry.size();

        LogDebug("ExecBlock::writeBasicBlock", "Attempting to write patch of %zu RelocatableInst to ExecBlock %p", seqIt->metadata.patchSize, this);
        // Attempt to write a complete patch. If not, rollback to the last complete patch written
        for(const RelocatableInst::SharedPtr& inst : seqIt->insts) {
            if(getEpilogueOffset() > MINIMAL_BLOCK_SIZE) {
                assembly.writeInstruction(inst->reloc(this), codeStream);
            }
            else {
                // Not enough space left, rollback
                rollback = true;
                break;
            }
        }

        if(rollback) {
            LogDebug("ExecBlock::writeBasicBlock", "Not enough space left, rolling back to offset 0x%" PRIRWORD, rollbackOffset);
            // Seek to the last complete patch written and terminate it with a terminator
            codeStream->seek(rollbackOffset);
            // free shadows allocated by the rollbacked code
            shadowIdx = rollbackShadowIdx;
            shadowRegistry.resize(rollbackShadowRegistry);
            // It's a NULL rollback, don't terminate it
            if(rollbackOffset == startOffset) {
                LogDebug("ExecBlock::writeBasicBlock", "NULL rollback, nothing written to ExecBlock %p", this);
                return {EXEC_BLOCK_FULL, 0, 0};
            }
            // Because we didn't wrote the full sequence, only keep the Entry bit
            seqType = static_cast<SeqType>(seqType & SeqType::Entry);
            break;
        } else {
            // Complete instruction was written, we add the metadata
            instMetadata.push_back(seqIt->metadata);
            // Register instruction
            instRegistry.push_back(InstInfo {seqID, static_cast<uint16_t>(rollbackOffset)});
            // Update indexes
            executeFlags |= seqIt->metadata.execblockFlags;
            seqIt++;
            patchWritten += 1;
        }
    }
    // If it's a rollback or a non-exit sequence, add a terminator
    if((seqType & SeqType::Exit) == 0) {
        LogDebug("ExecBlock::writeBasicBlock", "Writting terminator to ExecBlock %p to finish non-exit sequence", this);
        RelocatableInst::SharedPtrVec terminator = getTerminator(seqIt->metadata.address);
        for(RelocatableInst::SharedPtr &inst : terminator) {
            assembly.writeInstruction(inst->reloc(this), codeStream);
        }
    }
    // JIT the jump to epilogue
    RelocatableInst::SharedPtrVec jmpEpilogue = JmpEpilogue();
    for(RelocatableInst::SharedPtr &inst : jmpEpilogue) {
        assembly.writeInstruction(inst->reloc(this), codeStream);
    }
    // Register sequence
    uint16_t endInstID = getNextInstID() - 1;
    seqRegistry.push_back(SeqInfo {startInstID, endInstID, seqType, executeFlags});
    // Return write results
    unsigned bytesWritten = static_cast<unsigned>(codeStream->current_pos() - startOffset);
    LogDebug("ExecBlock::writeBasicBlock", "End write sequence in basicblock %p with execFlags : %x", this, executeFlags);
    return SeqWriteResult {seqID, bytesWritten, patchWritten};
}

uint16_t ExecBlock::splitSequence(uint16_t instID) {
    Require("ExecBlock::splitSequence", instID < instRegistry.size());
    uint16_t seqID = instRegistry[instID].seqID;
    seqRegistry.push_back(SeqInfo {
        instID,
        seqRegistry[seqID].endInstID,
        static_cast<SeqType>(SeqType::Entry | seqRegistry[seqID].type),
        seqRegistry[seqID].executeFlags
    });
    return getNextSeqID() - 1;
}

void ExecBlock::makeRX() {
    LogDebug("ExecBlock::makeRX", "Making ExecBlock %p RX", this);
    if(pageState != RX) {
        RequireAction(
            "ExecBlock::makeRX",
            !llvm::sys::Memory::protectMappedMemory(codeBlock, PF::MF_READ | PF::MF_EXEC),
            abort()
        );
        pageState = RX;
    }
}

void ExecBlock::makeRW() {
    LogDebug("ExecBlock::makeRW", "Making ExecBlock %p RW", this);
    if(pageState != RW) {
        RequireAction(
            "ExecBlock::makeRW",
            !llvm::sys::Memory::protectMappedMemory(codeBlock, PF::MF_READ | PF::MF_WRITE),
            abort()
        );
        pageState = RW;
    }
}

uint16_t ExecBlock::newShadow(uint16_t tag) {
    uint16_t id = shadowIdx++;
    RequireAction("ExecBlock::newShadow", id * sizeof(rword) < dataBlock.size() - sizeof(Context), abort());
    if(tag != NO_REGISTRATION) {
        LogDebug("ExecBlock::newShadow", "Registering new tagged shadow %" PRIu16 " for instID %" PRIu16 " wih tag %" PRIu16, id, getNextInstID(), tag);
        shadowRegistry.push_back({
            getNextSeqID(),
            getNextInstID(),
            tag,
            id
        });
    }
    return id;
}

void ExecBlock::setShadow(uint16_t id, rword v) {
    RequireAction("ExecBlock::setShadow", id * sizeof(rword) < dataBlock.size() - sizeof(Context), abort());
    shadows[id] = v;
}

rword ExecBlock::getShadow(uint16_t id) const {
    RequireAction("ExecBlock::getShadow", id * sizeof(rword) < dataBlock.size() - sizeof(Context), abort());
    return shadows[id];
}

rword ExecBlock::getShadowOffset(uint16_t id) const {
    rword offset = sizeof(Context) + id*sizeof(rword);
    RequireAction("ExecBlock::getShadowOffset", offset < dataBlock.size(), abort());
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

const InstMetadata* ExecBlock::getInstMetadata(uint16_t instID) const {
    Require("ExecBlock::getInstMetadata", instID < instMetadata.size());
    return &instMetadata[instID];
}

rword ExecBlock::getInstAddress(uint16_t instID) const {
    Require("ExecBlock::getInstAddress", instID < instMetadata.size());
    return instMetadata[instID].address;
}

const llvm::MCInst* ExecBlock::getOriginalMCInst(uint16_t instID) const {
    Require("ExecBlock::getOriginalMCInst", instID < instMetadata.size());
    return &instMetadata[instID].inst;
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
    Require("ExecBlock::getSeqID", instID < instRegistry.size());
    return instRegistry[instID].seqID;
}

SeqType ExecBlock::getSeqType(uint16_t seqID) const {
    Require("ExecBlock::getSeqType", seqID < seqRegistry.size());
    return seqRegistry[seqID].type;
}

uint16_t ExecBlock::getSeqStart(uint16_t seqID) const {
    Require("ExecBlock::getSeqStart", seqID < seqRegistry.size());
    return seqRegistry[seqID].startInstID;
}

uint16_t ExecBlock::getSeqEnd(uint16_t seqID) const {
    Require("ExecBlock::getSeqStart", seqID < seqRegistry.size());
    return seqRegistry[seqID].endInstID;
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

    for(const auto& reg: shadowRegistry) {
        if((seqID == ANY || reg.seqID  == seqID)  &&
           (tag   == ANY || reg.tag   == tag)) {
            result.push_back(reg);
        }
    }

    return result;
}

float ExecBlock::occupationRatio() const {
    return static_cast<float>(codeBlock.size() - getEpilogueOffset()) / static_cast<float>(codeBlock.size());
}

const CachedEdge* ExecBlock::getCachedEdge(rword addr) {
    std::map<rword, CachedEdge>::const_iterator it = cacheEdge.find(addr);
    if (it != cacheEdge.end()) {
        return &(it->second);
    }
    return nullptr;
}

void ExecBlock::setCachedEdge(rword addr, ExecBlock* nextBlock, uint16_t nextSeqId, uint16_t previousEndInstID) {
    std::map<rword, CachedEdge>::iterator it = cacheEdge.find(addr);
    if (it != cacheEdge.end()) {
        it->second.previousInstID.insert(previousEndInstID);
    } else {
        cacheEdge[addr] = {nextBlock, nextSeqId, {previousEndInstID}};
    }
}

void ExecBlock::clearCachedEdge(const Range<rword>& range) {
    std::map<rword, CachedEdge>::iterator it = cacheEdge.begin();
    while (it != cacheEdge.end()) {
        if (range.contains(it->first)) {
            it = cacheEdge.erase(it);
        } else {
            it++;
        }
    }
}

}
