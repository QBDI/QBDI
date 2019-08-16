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
#include "Platform.h"
#include "ExecBlock/ExecBlockManager.h"
#include "Patch/PatchRule.h"
#include "Patch/RegisterSize.h"
#include "Utility/LogSys.h"

#include <cstdint>
#include <algorithm>

namespace QBDI {

ExecBlockManager::ExecBlockManager(llvm::MCInstrInfo& MCII, llvm::MCRegisterInfo& MRI, Assembly& assembly, VMInstanceRef vminstance) :
   total_translated_size(1), total_translation_size(1), vminstance(vminstance), MCII(MCII), MRI(MRI), assembly(assembly) {
}

ExecBlockManager::~ExecBlockManager() {
    LogCallback(LogPriority::DEBUG, "ExecBlockManager::~ExecBlockManager", [&] (FILE *log) -> void {
        this->printCacheStatistics(log);
    });
    clearCache();
}

float ExecBlockManager::getExpansionRatio() const {
    LogDebug("ExecBlockManager::getExpansionRatio", "%zu / %zu", total_translation_size, total_translated_size);
    return static_cast<float>(total_translation_size) / static_cast<float>(total_translated_size);
}

void ExecBlockManager::printCacheStatistics(FILE* output) const {
    float mean_occupation = 0.0;
    size_t region_overflow = 0;
    fprintf(output, "\tCache made of %zu regions:\n", regions.size());
    for(size_t i = 0; i < regions.size(); i++) {
        float occupation = 0.0;
        for(size_t j = 0; j < regions[i].blocks.size(); j++) {
            occupation += regions[i].blocks[j]->occupationRatio();
        }
        if(regions[i].blocks.size() > 1) {
            region_overflow += 1;
        }
        if(regions[i].blocks.size() > 0) {
            occupation /= regions[i].blocks.size();
        }
        mean_occupation += occupation;
        fprintf(output, "\t\t[0x%" PRIRWORD ", 0x%" PRIRWORD "]: %zu blocks, %f occupation ratio\n", regions[i].covered.start, regions[i].covered.end, regions[i].blocks.size(), occupation);
    }
    if(regions.size() > 0) {
        mean_occupation /= regions.size();
    }
    fprintf(output, "\tMean occupation ratio: %f\n", mean_occupation);
    fprintf(output, "\tRegion overflow count: %zu\n", region_overflow);
}

ExecBlock* ExecBlockManager::getProgrammedExecBlock(rword address) {
    LogDebug("ExecBlockManager::getProgrammedExecBlock", "Looking up sequence at address %" PRIRWORD, address);

    size_t r = searchRegion(address);

    if(r < regions.size() && regions[r].covered.contains(address)) {
        ExecRegion& region = regions[r];

        // Attempting sequenceCache resolution
        const std::map<rword, SeqLoc>::const_iterator seqLoc = region.sequenceCache.find(address);
        if(seqLoc != region.sequenceCache.end()) {
            LogDebug("ExecBlockManager::getProgrammedExecBlock", "Found sequence 0x%" PRIRWORD " in ExecBlock %p as seqID %" PRIu16,
                     address, region.blocks[seqLoc->second.blockIdx], seqLoc->second.seqID);
            // Select sequence and return execBlock
            region.blocks[seqLoc->second.blockIdx]->selectSeq(seqLoc->second.seqID);
            return region.blocks[seqLoc->second.blockIdx];
        }

        // Attempting instCache resolution
        const std::map<rword, InstLoc>::const_iterator instLoc = region.instCache.find(address);
        if(instLoc != region.instCache.end()) {
            // Retrieving corresponding block and seqLoc
            ExecBlock* block = region.blocks[instLoc->second.blockIdx];
            uint16_t existingSeqId = block->getSeqID(instLoc->second.instID);
            const SeqLoc& existingSeqLoc = region.sequenceCache[block->getInstMetadata(block->getSeqStart(existingSeqId))->address];
            // Creating a new sequence at that instruction and saving it in the sequenceCache
            uint16_t newSeqID = block->splitSequence(instLoc->second.instID);
            regions[r].sequenceCache[address] = SeqLoc {
                instLoc->second.blockIdx,
                newSeqID,
                address,
                existingSeqLoc.bbEnd,
                address,
                existingSeqLoc.seqEnd,
            };
            LogDebug("ExecBlockManager::getProgrammedExecBlock", "Splitted seqID %" PRIu16 " at instID %" PRIu16 " in ExecBlock %p as new sequence with seqID %" PRIu16,
                     existingSeqId, instLoc->second.instID, block, newSeqID);
            block->selectSeq(newSeqID);
            return block;
        }
    }
    LogDebug("ExecBlockManager::getProgrammedExecBlock", "Cache miss for sequence 0x%" PRIRWORD, address);
    return nullptr;
}

const SeqLoc* ExecBlockManager::getSeqLoc(rword address) const {
    size_t r = searchRegion(address);
    if(r < regions.size() && regions[r].covered.contains(address)) {
        const std::map<rword, SeqLoc>::const_iterator seqLoc = regions[r].sequenceCache.find(address);
        if(seqLoc != regions[r].sequenceCache.end()) {
            return &(seqLoc->second);
        }
    }
    return nullptr;
}

void ExecBlockManager::writeBasicBlock(const std::vector<Patch>& basicBlock) {
    unsigned translated = 0;
    unsigned translation = 0;
    size_t patchIdx = 0, patchEnd = basicBlock.size();
    const Patch& firstPatch = basicBlock.front();
    const Patch& lastPatch = basicBlock.back();
    rword bbStart = firstPatch.metadata.address;
    rword bbEnd = lastPatch.metadata.endAddress();
    ExecBlock* previousExecBlock = nullptr;
    uint16_t previousLastInstID = 0;

    // Locating an approriate cache region
    size_t r = findRegion(Range<rword>(bbStart, bbEnd));
    ExecRegion& region = regions[r];

    // Basic block truncation to prevent dedoubled sequence
    const SeqLoc* endSequenceCached = nullptr;
    for(size_t i = 0; i < basicBlock.size(); i++) {
        std::map<rword, SeqLoc>::const_iterator it = region.sequenceCache.find(basicBlock[i].metadata.address);
        if(it != region.sequenceCache.end()) {
            patchEnd = i;
            endSequenceCached = &(it->second);
            break;
        }
    }
    // Cache integrity safeguard, should never happen
    if(patchEnd == 0) {
        LogDebug("ExecBlockManager::writeBasicBlock", "Cache hit, basic block 0x%" PRIRWORD " already exist", firstPatch.metadata.address);
        return;
    }
    LogDebug("ExecBlockManager::writeBasicBlock", "Writting new basic block 0x%" PRIRWORD, firstPatch.metadata.address);

    // Writing the basic block as one or more sequences
    while(patchIdx < patchEnd) {
        // Attempting to find an ExecBlock in the region
        for(size_t i = 0; true; i++) {
            // If the region doesn't have enough space in its ExecBlocks, we add one.
            // Optimally, a region should only have one ExecBlocks but misspredictions or oversized
            // basic blocks can cause overflows.
            if(i >= region.blocks.size()) {
                region.blocks.push_back(new ExecBlock(assembly, vminstance));
            }
            // Determine sequence type
            SeqType seqType = (SeqType) 0;
            if(patchIdx == 0) seqType = static_cast<SeqType>(seqType | SeqType::Entry);
            if(patchEnd == basicBlock.size()) seqType = static_cast<SeqType>(seqType | SeqType::Exit);
            // Write sequence
            SeqWriteResult res = region.blocks[i]->writeSequence(basicBlock.begin() + patchIdx, basicBlock.begin() + patchEnd, seqType);
            // Successful write
            if(res.seqID != EXEC_BLOCK_FULL) {
                // Saving sequence in the sequence cache
                region.sequenceCache[basicBlock[patchIdx].metadata.address] = SeqLoc {
                    (uint16_t) i,
                    res.seqID,
                    bbStart,
                    bbEnd,
                    basicBlock[patchIdx].metadata.address,
                    basicBlock[patchIdx + res.patchWritten - 1].metadata.endAddress(),
                };
                // Generate instruction mapping cache
                uint16_t startID = region.blocks[i]->getSeqStart(res.seqID);
                for(size_t j = 0; j < res.patchWritten; j++) {
                    region.instCache[basicBlock[patchIdx + j].metadata.address] = InstLoc {static_cast<uint16_t>(i), static_cast<uint16_t>(startID + j)};
                }
                // generate CachedEdge the basic block is plit
                if (previousExecBlock != nullptr) {
                    previousExecBlock->setCachedEdge(basicBlock[patchIdx].metadata.address, region.blocks[i], res.seqID, previousLastInstID);
                }
                previousExecBlock = region.blocks[i];
                previousLastInstID = basicBlock[patchIdx + res.patchWritten - 1].metadata.address;
                LogDebug("ExecBlockManager::writeBasicBlock",
                         "Sequence 0x%" PRIRWORD "-0x%" PRIRWORD " written in ExecBlock %p as seqID %" PRIu16,
                         basicBlock[patchIdx].metadata.address,
                         basicBlock[patchIdx + res.patchWritten - 1].metadata.endAddress(),
                         region.blocks[i], res.seqID);
                // Updating counters
                translated += basicBlock[patchIdx + res.patchWritten - 1].metadata.endAddress() - basicBlock[patchIdx].metadata.address;
                translation += res.bytesWritten;
                patchIdx += res.patchWritten;
                break;
            }
        }
    }
    if (endSequenceCached && previousExecBlock) {
        // cached edge to an already existed end
        previousExecBlock->setCachedEdge(basicBlock[patchEnd].metadata.address,
                                         region.blocks[endSequenceCached->blockIdx],
                                         endSequenceCached->seqID,
                                         previousLastInstID);
    }
    // Updating stats
    total_translation_size += translation;
    total_translated_size += translated;
    updateRegionStat(r, translated);
}

size_t ExecBlockManager::searchRegion(rword address) const {
    size_t low = 0;
    size_t high = regions.size();
    if(regions.size() == 0) {
        return 0;
    }
    LogDebug("ExecBlockManager::searchRegion", "Searching for address 0x%" PRIRWORD, address);
    // Binary search of the first region to look at
    while(low + 1 != high) {
        size_t idx = (low + high) / 2;
        if(regions[idx].covered.start > address) {
            high = idx;
        }
        else if(regions[idx].covered.end <= address) {
            low = idx;
        }
        else {
            LogDebug("ExecBlockManager::searchRegion", "Exact match for region %zu [0x%" PRIRWORD ", 0x%" PRIRWORD "]",
                     idx, regions[idx].covered.start, regions[idx].covered.end);
            return idx;
        }
    }
    LogDebug("ExecBlockManager::searchRegion", "Low match for region %zu [0x%" PRIRWORD ", 0x%" PRIRWORD "]",
             low, regions[low].covered.start, regions[low].covered.end);
    return low;
}

size_t ExecBlockManager::findRegion(Range<rword> codeRange) {
    size_t best_region = regions.size();
    size_t low = searchRegion(codeRange.start);
    unsigned best_cost = 0xFFFFFFFF;
    for(size_t i = low; i < low + 2 && i < regions.size(); i++) {
        unsigned cost = 0;
        // Easy case: the code range is inside one of the region, we can return immediately
        if(regions[i].covered.contains(codeRange)) {
            LogDebug(
                "ExecBlockManager::findRegion",
                "Basic block [0x%" PRIRWORD ", 0x%" PRIRWORD "] assigned to region %zu [0x%" PRIRWORD ", 0x%" PRIRWORD "]",
                codeRange.start,
                codeRange.end,
                i,
                regions[i].covered.start,
                regions[i].covered.end
            );
            return i;
        }
        // Hard case: it's in the available budget of one the region. Keep the lowest cost.
        // First compute the required cost for the region to cover this extended range.
        if(regions[i].covered.end < codeRange.end) {
            cost += (codeRange.end - regions[i].covered.end);
        }
        if(regions[i].covered.start > codeRange.start) {
            cost += (regions[i].covered.start - codeRange.start);
        }
        // Make sure that such cost is available and that it's better than previous candidates
        if(static_cast<unsigned>(cost * getExpansionRatio()) < regions[i].available && cost < best_cost) {
            best_cost = cost;
            best_region = i;
        }
    }
    // We found an extension candidate
    if(best_region != regions.size()) {
        LogDebug(
            "ExecBlockManager::findRegion",
            "Extending region %zu [0x%" PRIRWORD ", 0x%" PRIRWORD "] to cover basic block [0x%" PRIRWORD ", 0x%" PRIRWORD "]",
            best_region,
            regions[best_region].covered.start,
            regions[best_region].covered.end,
            codeRange.start,
            codeRange.end
        );
        if(regions[best_region].covered.end < codeRange.end) {
            regions[best_region].covered.end = codeRange.end;
        }
        if(regions[best_region].covered.start > codeRange.start) {
            regions[best_region].covered.start = codeRange.start;
        }
        return best_region;
    }
    // Else we have to create a new region
    // Find a place to insert it
    size_t insert = low;
    for(; insert < regions.size(); insert++) {
        if(regions[insert].covered.start > codeRange.start) {
            break;
        }
    }
    LogDebug(
        "ExecBlockManager::findRegion",
        "Creating new region %zu to cover basic block [0x%" PRIRWORD ", 0x%" PRIRWORD "]",
        insert,
        codeRange.start,
        codeRange.end
    );
    regions.insert(regions.begin() + insert, ExecRegion {codeRange, 0, 0, std::vector<ExecBlock*>()});
    return insert;
}

void ExecBlockManager::updateRegionStat(size_t r, rword translated) {
    regions[r].translated += translated;
    // Remaining code block space
    regions[r].available = regions[r].blocks[0]->getEpilogueOffset();
    // Space which needs to be reserved for the non translated part of the covered region
    unsigned reserved = static_cast<unsigned>((static_cast<float>(regions[r].covered.size() - regions[r].translated)) * getExpansionRatio());
    LogDebug(
        "ExecBlockManager::updateRegionStat",
        "Region %zu has %zu bytes available of which %u are reserved for %zu bytes of untranslated code",
        r,
        regions[r].available,
        reserved,
        (regions[r].covered.size() - regions[r].translated)
    );
    if(reserved > regions[r].available) {
        regions[r].available = 0;
    }
    else {
        regions[r].available -= reserved;
    }
}

const InstAnalysis* ExecBlockManager::analyzeInstMetadata(const InstMetadata* instMetadata, AnalysisType type) {
    InstAnalysis* instAnalysis = nullptr;
    RequireAction("Engine::analyzeInstMetadata", instMetadata, return nullptr);

    size_t r = searchRegion(instMetadata->address);

    // Attempt to locate it in the sequenceCache
    if(r < regions.size() && regions[r].covered.contains(instMetadata->address) &&
       regions[r].analysisCache.count(instMetadata->address) == 1) {
        LogDebug("ExecBlockManager::analyzeInstMetadata", "Analysis of instruction 0x%" PRIRWORD " found in sequenceCache of region %zu", instMetadata->address, r);
        instAnalysis = regions[r].analysisCache[instMetadata->address];
    }
    // Do we have anything we want inside the cache ?
    if ((instAnalysis != nullptr) &&
        ((instAnalysis->analysisType & type) != type)) {
        LogDebug("ExecBlockManager::analyzeInstMetadata", "Analysis of instruction 0x%" PRIRWORD " need to be rebuilt", instMetadata->address);
        // Free current cache because we want more data
        freeInstAnalysis(instAnalysis);
        instAnalysis = nullptr;
    }
    // We have a usable cached analysis
    if (instAnalysis != nullptr) {
        return instAnalysis;
    }
    // Cache miss
    instAnalysis = analyzeInstMetadataUncached(instMetadata, type, MCII, MRI, assembly);

    // If its part of a region, put in in the region cache
    if(r < regions.size() && regions[r].covered.contains(instMetadata->address)) {
        LogDebug("ExecBlockManager::analyzeInstMetadata", "Analysis of instruction 0x%" PRIRWORD " cached in region %zu", instMetadata->address, r);
        regions[r].analysisCache[instMetadata->address] = instAnalysis;
    }
    // Put it in the global cache. Should never happen under normal usage
    else {
        LogDebug("ExecBlockManager::analyzeInstMetadata", "Analysis of instruction 0x%" PRIRWORD " cached in global cache", instMetadata->address);
        analysisCache[instMetadata->address] = instAnalysis;
    }
    return instAnalysis;
}


void ExecBlockManager::eraseRegion(size_t r) {
    LogDebug("ExecBlockManager::eraseRegion", "Erasing region %zu [0x%" PRIRWORD ", 0x%" PRIRWORD "]",
             r, regions[r].covered.start, regions[r].covered.end);
    // Delete cached blocks
    for(ExecBlock* block: regions[r].blocks) {
        LogDebug("ExecBlockManager::eraseRegion", "Dropping ExecBlock %p", block);
        delete block;
    }
    // Delete cached analysis
    for(std::pair<rword, InstAnalysis*> analysis: regions[r].analysisCache) {
        freeInstAnalysis(analysis.second);
    }
    regions.erase(regions.begin() + r);
}

void ExecBlockManager::clearCache(RangeSet<rword> rangeSet) {
    const std::vector<Range<rword>>& ranges = rangeSet.getRanges();
    for(Range<rword> r: ranges) {
        clearCache(r);
    }
    // Probably comming from an instrumentation change, reset translation counters
    total_translated_size = 1;
    total_translation_size = 1;
}

void ExecBlockManager::flushCommit() {
    // It needs to be erased from last to first to preserve index validity
    if(flushList.size() > 0) {
        LogDebug("ExecBlockManager::flushCommit", "Flushing analysis caches");
        std::sort(flushList.begin(), flushList.end(), std::greater<size_t>());
        // Remove duplicates
        flushList.erase(std::unique(flushList.begin(), flushList.end()), flushList.end());
        for(size_t r: flushList) {
            eraseRegion(r);
        }
        flushList.clear();
        // Clear global cache
        for(std::pair<rword, InstAnalysis*> analysis: analysisCache) {
            freeInstAnalysis(analysis.second);
        }
        analysisCache.clear();
    }
}

void ExecBlockManager::clearCache(Range<rword> range) {
    size_t i = 0;
    LogDebug("ExecBlockManager::clearCache", "Erasing range [0x%" PRIRWORD ", 0x%" PRIRWORD "]", range.start, range.end);
    for(i = 0; i < regions.size(); i++) {
        if(regions[i].covered.overlaps(range)) {
            flushList.push_back(i);
        } else {
            for (ExecBlock* cbk : regions[i].blocks) {
                cbk->clearCachedEdge(range);
            }
        }
    }
}

void ExecBlockManager::clearCache() {
    LogDebug("ExecBlockManager::clearCache", "Erasing all cache");
    while(regions.size() > 0) {
        eraseRegion(regions.size() - 1);
    }
}

}
