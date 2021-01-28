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
#include <algorithm>
#include <cstdint>

#include "ExecBlock/ExecBlock.h"
#include "ExecBlock/ExecBlockManager.h"
#include "Patch/Patch.h"
#include "Utility/LogSys.h"

#include "State.h"

namespace QBDI {

ExecBlockManager::ExecBlockManager(const Assembly& assembly, VMInstanceRef vminstance) :
   total_translated_size(1), total_translation_size(1), vminstance(vminstance), assembly(assembly) {
}

ExecBlockManager::~ExecBlockManager() {
    LogCallback(LogPriority::DEBUG, "ExecBlockManager::~ExecBlockManager", [&] (FILE *log) -> void {
        this->printCacheStatistics(log);
    });
    clearCache();
}

void ExecBlockManager::changeVMInstanceRef(VMInstanceRef vminstance) {
    this->vminstance = vminstance;
    for (auto &reg : regions)
        for (auto &block : reg.blocks)
            block->changeVMInstanceRef(vminstance);
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
        fprintf(output, "\t\t[0x%" PRIRWORD ", 0x%" PRIRWORD "]: %zu blocks, %f occupation ratio\n",
                regions[i].covered.start(),
                regions[i].covered.end(),
                regions[i].blocks.size(),
                occupation);
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
                     address, region.blocks[seqLoc->second.blockIdx].get(), seqLoc->second.seqID);
            // Select sequence and return execBlock
            region.blocks[seqLoc->second.blockIdx]->selectSeq(seqLoc->second.seqID);
            return region.blocks[seqLoc->second.blockIdx].get();
        }

        // Attempting instCache resolution
        const std::map<rword, InstLoc>::const_iterator instLoc = region.instCache.find(address);
        if(instLoc != region.instCache.end()) {
            // Retrieving corresponding block and seqLoc
            ExecBlock* block = region.blocks[instLoc->second.blockIdx].get();
            uint16_t existingSeqId = block->getSeqID(instLoc->second.instID);
            const SeqLoc& existingSeqLoc = region.sequenceCache[block->getInstMetadata(block->getSeqStart(existingSeqId)).address];
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

const ExecBlock* ExecBlockManager::getExecBlock(rword address) const {
    LogDebug("ExecBlockManager::getExecBlock", "Looking up address %" PRIRWORD, address);

    size_t r = searchRegion(address);

    if(r < regions.size() && regions[r].covered.contains(address)) {
        const ExecRegion& region = regions[r];

        // Attempting instCache resolution
        const std::map<rword, InstLoc>::const_iterator instLoc = region.instCache.find(address);
        if(instLoc != region.instCache.end()) {
            LogDebug("ExecBlockManager::getExecBlock", "Found address 0x%" PRIRWORD " in ExecBlock %p",
                     address, region.blocks[instLoc->second.blockIdx].get());
            return region.blocks[instLoc->second.blockIdx].get();
        }
    }
    LogDebug("ExecBlockManager::getExecBlock", "Cache miss for address 0x%" PRIRWORD, address);
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

size_t ExecBlockManager::preWriteBasicBlock(const std::vector<Patch>& basicBlock) {
    // prereserve the region in the cache and return the instruction that are already in the cache for this basicBlock

    // Locating an approriate cache region
    const Range<rword> bbRange {basicBlock.front().metadata.address, basicBlock.back().metadata.endAddress()};
    size_t r = findRegion(bbRange);
    ExecRegion& region = regions[r];

    // detect cached instruction
    size_t patchEnd = basicBlock.size();

    while (patchEnd > 0 && region.instCache.count(basicBlock[patchEnd - 1].metadata.address) != 0) {
        patchEnd--;
    }

    return patchEnd;
}


void ExecBlockManager::writeBasicBlock(const std::vector<Patch>& basicBlock, size_t patchEnd) {
    unsigned translated = 0;
    unsigned translation = 0;
    size_t patchIdx = 0;
    const Patch& firstPatch = basicBlock.front();
    const Patch& lastPatch = basicBlock.back();
    rword bbStart = firstPatch.metadata.address;
    rword bbEnd = lastPatch.metadata.endAddress();

    // Locating an approriate cache region
    const Range<rword> bbRange {bbStart, bbEnd};
    size_t r = findRegion(bbRange);
    ExecRegion& region = regions[r];

    // Basic block truncation to prevent dedoubled instruction
    // patchEnd must be the number of instruction that wasn't in the cache for this basicblock
    RequireAction("ExecBlockManager::writeBasicBlock", patchEnd <= basicBlock.size(), abort());
    RequireAction("ExecBlockManager::writeBasicBlock", patchEnd == basicBlock.size() || region.instCache.count(basicBlock[patchEnd].metadata.address) == 1, abort());

    while (patchEnd > 0 && region.instCache.count(basicBlock[patchEnd - 1].metadata.address) != 0) {
        // should never happen if preWriteBasicBlock is used
        patchEnd--;
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
                RequireAction("ExecBlockManager::writeBasicBlock", i < (1<<16), abort());
                region.blocks.emplace_back(std::make_unique<ExecBlock>(assembly, vminstance));
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
                regions[r].sequenceCache[basicBlock[patchIdx].metadata.address] = SeqLoc {
                    static_cast<uint16_t>(i),
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
                LogDebug("ExecBlockManager::writeBasicBlock",
                         "Sequence 0x%" PRIRWORD "-0x%" PRIRWORD " written in ExecBlock %p as seqID %" PRIu16,
                         basicBlock[patchIdx].metadata.address,
                         basicBlock[patchIdx + res.patchWritten - 1].metadata.endAddress(),
                         region.blocks[i].get(), res.seqID);
                // Updating counters
                translated += basicBlock[patchIdx + res.patchWritten - 1].metadata.endAddress() - basicBlock[patchIdx].metadata.address;
                translation += res.bytesWritten;
                patchIdx += res.patchWritten;
                break;
            }
        }
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
        if(regions[idx].covered.start() > address) {
            high = idx;
        }
        else if(regions[idx].covered.end() <= address) {
            low = idx;
        }
        else {
            LogDebug("ExecBlockManager::searchRegion", "Exact match for region %zu [0x%" PRIRWORD ", 0x%" PRIRWORD "]",
                     idx, regions[idx].covered.start(), regions[idx].covered.end());
            return idx;
        }
    }
    LogDebug("ExecBlockManager::searchRegion", "Low match for region %zu [0x%" PRIRWORD ", 0x%" PRIRWORD "]",
             low, regions[low].covered.start(), regions[low].covered.end());
    return low;
}

void ExecBlockManager::mergeRegion(size_t i) {

    RequireAction("ExecBlockManager::mergeRegion", i + 1 < regions.size(), return);
    RequireAction("ExecBlockManager::mergeRegion",
            regions[i].blocks.size() + regions[i+1].blocks.size() < (1<<16), abort());

    LogDebug(
            "ExecBlockManager::mergeRegion",
            "Merge region %zu [0x%" PRIRWORD ", 0x%" PRIRWORD "] and region %zu [0x%" PRIRWORD ", 0x%" PRIRWORD "]",
            i,
            regions[i].covered.start(),
            regions[i].covered.end(),
            i+1,
            regions[i+1].covered.start(),
            regions[i+1].covered.end()
            );
    // SeqLoc
    for (const auto& it: regions[i+1].sequenceCache) {
        regions[i].sequenceCache[it.first] = SeqLoc {
                static_cast<uint16_t>(it.second.blockIdx + regions[i].blocks.size()),
                it.second.seqID,
                it.second.bbStart,
                it.second.bbEnd,
                it.second.seqStart,
                it.second.seqEnd
            };
    }
    // InstLoc
    for (const auto& it: regions[i+1].instCache) {
        regions[i].instCache[it.first] = InstLoc {
                static_cast<uint16_t>(it.second.blockIdx + regions[i].blocks.size()),
                it.second.instID,
            };
    }

    // range
    regions[i].covered.setEnd(regions[i+1].covered.end());

    // ExecBlock
    std::move(regions[i+1].blocks.begin(), regions[i+1].blocks.end(),
              std::back_inserter(regions[i].blocks));
    // flush
    regions[i].toFlush |= regions[i+1].toFlush;

    regions.erase(regions.begin() + i + 1);
}

size_t ExecBlockManager::findRegion(const Range<rword>& codeRange) {
    size_t best_region = regions.size();
    size_t low = searchRegion(codeRange.start());
    unsigned best_cost = 0xFFFFFFFF;

    // when low == 0, three cases:
    //  - no regions => no loop
    //  - codeRange.start is before the first region => 1 round of the loop to
    //      determine if we create a region or create a new one
    //  - codeRange.start is in or after the first region => 2 rounds to
    //      determine if we create a region or use the first or the second one
    size_t limit = 2;
    if(low == 0 and regions.size() != 0 and codeRange.start() < regions[0].covered.start())
        limit = 1;

    for(size_t i = low; i < low + limit && i < regions.size(); i++) {
        unsigned cost = 0;
        // Easy case: the code range is inside one of the region, we can return immediately
        if(regions[i].covered.contains(codeRange)) {
            LogDebug(
                "ExecBlockManager::findRegion",
                "Basic block [0x%" PRIRWORD ", 0x%" PRIRWORD "] assigned to region %zu [0x%" PRIRWORD ", 0x%" PRIRWORD "]",
                codeRange.start(),
                codeRange.end(),
                i,
                regions[i].covered.start(),
                regions[i].covered.end()
            );
            return i;
        }

        // Medium case: the code range overlaps.
        // This case may append when instrument unaligned code
        // In order to avoid two regions to overlaps,
        //  we must use the first region that overlaps
        if(regions[i].covered.overlaps(codeRange)) {
            LogDebug(
                    "ExecBlockManager::findRegion",
                    "Region %zu [0x%" PRIRWORD ", 0x%" PRIRWORD "] overlaps a part of basic block [0x%" PRIRWORD ", 0x%" PRIRWORD "], Try extend",
                    i,
                    regions[i].covered.start(),
                    regions[i].covered.end(),
                    codeRange.start(),
                    codeRange.end()
            );
            // part 1, codeRange.start() must be on the regions[i] to avoid error with searchRegion
            if(codeRange.start() < regions[i].covered.start()) {
                // the previous region mustn't containt codeRange.start
                // (should never happend as the iteration test the previous block first)
                RequireAction("ExecBlockManager::findRegion",
                        i == 0 or regions[i-1].covered.end() <= codeRange.start(),
                        abort());
                regions[i].covered.setStart(codeRange.start());
            }

            // part 2, codeRange.end() SHOULD be in the region.
            if(regions[i].covered.end() < codeRange.end()) {
                if(     i + 1 == regions.size() or
                        codeRange.end() <= regions[i+1].covered.start()) {
                    // extend the current region if no overlaps the next region
                    regions[i].covered.setEnd(codeRange.end());
                } else {
                    // the range overlaps two region, merge them
                    mergeRegion(i);
                }
            }
            LogDebug(
                    "ExecBlockManager::findRegion",
                    "New Region %zu [0x%" PRIRWORD ", 0x%" PRIRWORD "]",
                    i,
                    regions[i].covered.start(),
                    regions[i].covered.end()
            );

            return i;
        }

        // Hard case: it's in the available budget of one the region. Keep the lowest cost.
        // First compute the required cost for the region to cover this extended range.
        if(regions[i].covered.end() < codeRange.end()) {
            cost += (codeRange.end() - regions[i].covered.end());
        }
        if(regions[i].covered.start() > codeRange.start()) {
            cost += (regions[i].covered.start() - codeRange.start());
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
            regions[best_region].covered.start(),
            regions[best_region].covered.end(),
            codeRange.start(),
            codeRange.end()
        );
        if(regions[best_region].covered.end() < codeRange.end()) {
            regions[best_region].covered.setEnd(codeRange.end());
        }
        if(regions[best_region].covered.start() > codeRange.start()) {
            regions[best_region].covered.setStart(codeRange.start());
        }
        return best_region;
    }
    // Else we have to create a new region
    // Find a place to insert it
    size_t insert = low;
    for(; insert < regions.size(); insert++) {
        if(regions[insert].covered.start() > codeRange.start()) {
            break;
        }
    }
    LogDebug(
        "ExecBlockManager::findRegion",
        "Creating new region %zu to cover basic block [0x%" PRIRWORD ", 0x%" PRIRWORD "]",
        insert,
        codeRange.start(),
        codeRange.end()
    );
    regions.insert(regions.begin() + insert, {codeRange, 0, 0, {}});
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
    if(needFlush) {
        LogDebug("ExecBlockManager::flushCommit", "Flushing analysis caches");
        regions.erase(std::remove_if(regions.begin(), regions.end(),
            [](const ExecRegion &r) -> bool {
                if (r.toFlush)
                    LogDebug("ExecBlockManager::flushCommit", "Erasing region [0x%" PRIRWORD ", 0x%" PRIRWORD "]",
                             r.covered.start(), r.covered.end());
                return r.toFlush;
            }), regions.end());
        needFlush = false;
    }
}

void ExecBlockManager::clearCache(Range<rword> range) {
    size_t i = 0;
    LogDebug("ExecBlockManager::clearCache", "Erasing range [0x%" PRIRWORD ", 0x%" PRIRWORD "]", range.start(), range.end());
    for(i = 0; i < regions.size(); i++) {
        if(regions[i].covered.overlaps(range)) {
            regions[i].toFlush = true;
            needFlush = true;
        }
    }
}

void ExecBlockManager::clearCache(bool flushNow) {
    LogDebug("ExecBlockManager::clearCache", "Erasing all cache");
    if (flushNow) {
        regions.clear();
        total_translated_size = 1;
        total_translation_size = 1;
        needFlush = false;
    } else {
        for(auto &r : regions) {
            r.toFlush = true;
            needFlush = true;
        }
    }
}

}
