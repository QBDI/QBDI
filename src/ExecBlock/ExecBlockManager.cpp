/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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
#include <iterator>
#include <stdlib.h>
#include <utility>

#include "Engine/LLVMCPU.h"
#include "ExecBlock/ExecBlock.h"
#include "ExecBlock/ExecBlockManager.h"
#include "ExecBroker/ExecBroker.h"
#include "Patch/ExecBlockPatch.h"
#include "Patch/InstMetadata.h"
#include "Patch/Patch.h"
#include "Patch/RelocatableInst.h"
#include "Utility/LogSys.h"

namespace QBDI {

namespace {

inline rword getExecRegionKey(rword address, CPUMode cpumode) {
  if constexpr (is_arm) {
    if (cpumode != CPUMode::DEFAULT) {
      address |= 1;
    } else {
      address &= (~1);
    }
  }
  return address;
}

} // namespace

ExecBlockManager::ExecBlockManager(const LLVMCPUs &llvmCPUs,
                                   VMInstanceRef vminstance)
    : total_translated_size(1), total_translation_size(1),
      vminstance(vminstance), llvmCPUs(llvmCPUs),
      execBlockPrologue(
          getExecBlockPrologue(llvmCPUs.getCPU(CPUMode::DEFAULT))),
      execBlockEpilogue(
          getExecBlockEpilogue(llvmCPUs.getCPU(CPUMode::DEFAULT))) {

  auto execBrokerBlock = std::make_unique<ExecBlock>(
      llvmCPUs, vminstance, &execBlockPrologue, &execBlockEpilogue, 0);
  epilogueSize = execBrokerBlock->getEpilogueSize();
  execBroker = std::make_unique<ExecBroker>(std::move(execBrokerBlock),
                                            llvmCPUs, vminstance);
}

ExecBlockManager::~ExecBlockManager() {
  QBDI_DEBUG_BLOCK({ this->printCacheStatistics(); });
  clearCache();
}

void ExecBlockManager::changeVMInstanceRef(VMInstanceRef vminstance) {
  this->vminstance = vminstance;
  execBroker->changeVMInstanceRef(vminstance);
  for (auto &reg : regions) {
    for (auto &block : reg.blocks) {
      block->changeVMInstanceRef(vminstance);
    }
  }
}

float ExecBlockManager::getExpansionRatio() const {
  QBDI_DEBUG("{} / {}", total_translation_size, total_translated_size);
  return static_cast<float>(total_translation_size) /
         static_cast<float>(total_translated_size);
}

void ExecBlockManager::printCacheStatistics() const {
  float mean_occupation = 0.0;
  size_t region_overflow = 0;
  QBDI_DEBUG("\tCache made of {} regions:", regions.size());
  for (size_t i = 0; i < regions.size(); i++) {
    float occupation = 0.0;
    for (size_t j = 0; j < regions[i].blocks.size(); j++) {
      occupation += regions[i].blocks[j]->occupationRatio();
    }
    if (regions[i].blocks.size() > 1) {
      region_overflow += 1;
    }
    if (regions[i].blocks.size() > 0) {
      occupation /= regions[i].blocks.size();
    }
    mean_occupation += occupation;
    QBDI_DEBUG("\t\t[0x{:x}, 0x{:x}]: {} blocks, {} occupation ratio",
               regions[i].covered.start(), regions[i].covered.end(),
               regions[i].blocks.size(), occupation);
  }
  if (regions.size() > 0) {
    mean_occupation /= regions.size();
  }
  QBDI_DEBUG("\tMean occupation ratio: {}", mean_occupation);
  QBDI_DEBUG("\tRegion overflow count: {}", region_overflow);
}

ExecBlock *ExecBlockManager::getProgrammedExecBlock(rword address,
                                                    CPUMode cpumode,
                                                    SeqLoc *programmedSeqLock) {
  QBDI_DEBUG("Looking up sequence at address {:x} mode {}", address, cpumode);

  size_t r = searchRegion(address);

  if (r < regions.size() && regions[r].covered.contains(address)) {
    ExecRegion &region = regions[r];
    const auto target = getExecRegionKey(address, cpumode);

    // Attempting sequenceCache resolution
    const auto seqLoc = region.sequenceCache.find(target);
    if (seqLoc != region.sequenceCache.end()) {
      QBDI_DEBUG("Found sequence 0x{:x} ({}) in ExecBlock 0x{:x} as seqID {:x}",
                 address, cpumode,
                 reinterpret_cast<uintptr_t>(
                     region.blocks[seqLoc->second.blockIdx].get()),
                 seqLoc->second.seqID);
      // copy current sequence info
      if (programmedSeqLock != nullptr) {
        *programmedSeqLock = seqLoc->second;
      }
      // Select sequence and return execBlock
      region.blocks[seqLoc->second.blockIdx]->selectSeq(seqLoc->second.seqID);
      return region.blocks[seqLoc->second.blockIdx].get();
    }

    // Attempting instCache resolution
    const auto instLoc = region.instCache.find(target);
    if (instLoc != region.instCache.end()) {
      // Retrieving corresponding block and seqLoc
      ExecBlock *block = region.blocks[instLoc->second.blockIdx].get();
      uint16_t existingSeqId = block->getSeqID(instLoc->second.instID);
      const SeqLoc &existingSeqLoc = region.sequenceCache[getExecRegionKey(
          block->getInstMetadata(block->getSeqStart(existingSeqId)).address,
          cpumode)];
      // Creating a new sequence at that instruction and
      // saving it in the sequenceCache
      uint16_t newSeqID = block->splitSequence(instLoc->second.instID);
      region.sequenceCache[target] = SeqLoc{
          instLoc->second.blockIdx, newSeqID, existingSeqLoc.bbEnd, address,
          existingSeqLoc.seqEnd,
      };
      QBDI_DEBUG(
          "Splitted seqID {:x} at instID {:x} in ExecBlock 0x{:x} as new "
          "sequence with seqID {:x}",
          existingSeqId, instLoc->second.instID,
          reinterpret_cast<uintptr_t>(block), newSeqID);
      // copy current sequence info
      if (programmedSeqLock != nullptr) {
        *programmedSeqLock = regions[r].sequenceCache[target];
      }
      block->selectSeq(newSeqID);
      return block;
    }
  }
  QBDI_DEBUG("Cache miss for sequence 0x{:x} ({})", address, cpumode);
  return nullptr;
}

const ExecBlock *ExecBlockManager::getExecBlock(rword address,
                                                CPUMode cpumode) const {
  QBDI_DEBUG("Looking up address {:x} ({})", address, cpumode);

  size_t r = searchRegion(address);

  if (r < regions.size() && regions[r].covered.contains(address)) {
    const ExecRegion &region = regions[r];

    // Attempting instCache resolution
    const auto instLoc =
        region.instCache.find(getExecRegionKey(address, cpumode));
    if (instLoc != region.instCache.end()) {
      QBDI_DEBUG("Found address 0x{:x} ({}) in ExecBlock 0x{:x}", address,
                 cpumode,
                 reinterpret_cast<uintptr_t>(
                     region.blocks[instLoc->second.blockIdx].get()));
      return region.blocks[instLoc->second.blockIdx].get();
    }
  }
  QBDI_DEBUG("Cache miss for address 0x{:x} ({})", address, cpumode);
  return nullptr;
}

size_t
ExecBlockManager::preWriteBasicBlock(const std::vector<Patch> &basicBlock) {
  // prereserve the region in the cache and return the instruction that are
  // already in the cache for this basicBlock

  // Locating an approriate cache region
  const Range<rword> bbRange{basicBlock.front().metadata.address,
                             basicBlock.back().metadata.endAddress()};
  size_t r = findRegion(bbRange);
  ExecRegion &region = regions[r];

  // detect cached instruction
  size_t patchEnd = basicBlock.size();

  while (patchEnd > 0 && region.instCache.count(getExecRegionKey(
                             basicBlock[patchEnd - 1].metadata.address,
                             basicBlock[patchEnd - 1].metadata.cpuMode)) != 0) {
    patchEnd--;
  }

  return patchEnd;
}

void ExecBlockManager::writeBasicBlock(std::vector<Patch> &&basicBlock,
                                       size_t patchEnd) {
  unsigned translated = 0;
  unsigned translation = 0;
  size_t patchIdx = 0;
  const Patch &firstPatch = basicBlock.front();
  const Patch &lastPatch = basicBlock.back();
  rword bbStart = firstPatch.metadata.address;
  rword bbEnd = lastPatch.metadata.endAddress();

  // Locating an approriate cache region
  const Range<rword> bbRange{bbStart, bbEnd};
  size_t r = findRegion(bbRange);
  ExecRegion &region = regions[r];

  // Basic block truncation to prevent dedoubled instruction
  // patchEnd must be the number of instruction that wasn't in the cache for
  // this basicblock
  QBDI_REQUIRE_ABORT(patchEnd <= basicBlock.size(), "Internal error");
  QBDI_REQUIRE_ABORT(patchEnd == basicBlock.size() ||
                         region.instCache.count(getExecRegionKey(
                             basicBlock[patchEnd].metadata.address,
                             basicBlock[patchEnd].metadata.cpuMode)) == 1,
                     "Internal error, basicBlock end not found in the cache");

  while (patchEnd > 0 && region.instCache.count(getExecRegionKey(
                             basicBlock[patchEnd - 1].metadata.address,
                             basicBlock[patchEnd - 1].metadata.cpuMode)) != 0) {
    // should never happen if preWriteBasicBlock is used
    patchEnd--;
  }

  // Cache integrity safeguard, should never happen
  if (patchEnd == 0) {
    QBDI_DEBUG("Cache hit, basic block 0x{:x} already exist",
               firstPatch.metadata.address);
    return;
  }
  QBDI_DEBUG("Writting new basic block 0x{:x}", firstPatch.metadata.address);

  // Writing the basic block as one or more sequences
  while (patchIdx < patchEnd) {
    // Attempting to find an ExecBlock in the region
    for (size_t i = 0; true; i++) {
      // If the region doesn't have enough space in its ExecBlocks, we add one.
      // Optimally, a region should only have one ExecBlocks but misspredictions
      // or oversized basic blocks can cause overflows.
      if (i >= region.blocks.size()) {
        QBDI_REQUIRE_ABORT(i < (1 << 16),
                           "Too many ExecBlock in the same region");
        region.blocks.emplace_back(std::make_unique<ExecBlock>(
            llvmCPUs, vminstance, &execBlockPrologue, &execBlockEpilogue,
            epilogueSize));
      }
      // Write sequence
      SeqWriteResult res = region.blocks[i]->writeSequence(
          basicBlock.begin() + patchIdx, basicBlock.begin() + patchEnd);
      // Successful write
      if (res.seqID != EXEC_BLOCK_FULL) {
        // Saving sequence in the sequence cache
        regions[r].sequenceCache[getExecRegionKey(
            basicBlock[patchIdx].metadata.address,
            basicBlock[patchIdx].metadata.cpuMode)] = SeqLoc{
            static_cast<uint16_t>(i),
            res.seqID,
            bbEnd,
            basicBlock[patchIdx].metadata.address,
            basicBlock[patchIdx + res.patchWritten - 1].metadata.endAddress(),
        };
        // Generate instruction mapping cache
        uint16_t startID = region.blocks[i]->getSeqStart(res.seqID);
        for (size_t j = 0; j < res.patchWritten; j++) {
          region.instCache[getExecRegionKey(
              basicBlock[patchIdx + j].metadata.address,
              basicBlock[patchIdx + j].metadata.cpuMode)] = InstLoc{
              static_cast<uint16_t>(i), static_cast<uint16_t>(startID + j)};
          std::move(basicBlock[patchIdx + j].userInstCB.begin(),
                    basicBlock[patchIdx + j].userInstCB.end(),
                    std::back_inserter(region.userInstCB));
          basicBlock[patchIdx + j].userInstCB.clear();
        }
        QBDI_DEBUG(
            "Sequence 0x{:x}-0x{:x} written in ExecBlock 0x{:x} as seqID {:x}",
            basicBlock[patchIdx].metadata.address,
            basicBlock[patchIdx + res.patchWritten - 1].metadata.endAddress(),
            reinterpret_cast<uintptr_t>(region.blocks[i].get()), res.seqID);
        // Updating counters
        translated +=
            basicBlock[patchIdx + res.patchWritten - 1].metadata.endAddress() -
            basicBlock[patchIdx].metadata.address;
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
  if (regions.size() == 0) {
    return 0;
  }
  QBDI_DEBUG("Searching for address 0x{:x}", address);
  // Binary search of the first region to look at
  while (low + 1 != high) {
    size_t idx = (low + high) / 2;
    if (regions[idx].covered.start() > address) {
      high = idx;
    } else if (regions[idx].covered.end() <= address) {
      low = idx;
    } else {
      QBDI_DEBUG("Exact match for region {} [0x{:x}, 0x{:x}]", idx,
                 regions[idx].covered.start(), regions[idx].covered.end());
      return idx;
    }
  }
  QBDI_DEBUG("Low match for region {} [0x{:x}, 0x{:x}]", low,
             regions[low].covered.start(), regions[low].covered.end());
  return low;
}

void ExecBlockManager::mergeRegion(size_t i) {

  QBDI_REQUIRE_ACTION(i + 1 < regions.size(), return );
  QBDI_REQUIRE_ABORT(regions[i].blocks.size() + regions[i + 1].blocks.size() <
                         (1 << 16),
                     "Too many ExecBlock in the same region");

  QBDI_DEBUG("Merge region {} [0x{:x}, 0x{:x}] and region {} [0x{:x}, 0x{:x}]",
             i, regions[i].covered.start(), regions[i].covered.end(), i + 1,
             regions[i + 1].covered.start(), regions[i + 1].covered.end());
  // SeqLoc
  for (const auto &it : regions[i + 1].sequenceCache) {
    regions[i].sequenceCache[it.first] = SeqLoc{
        static_cast<uint16_t>(it.second.blockIdx + regions[i].blocks.size()),
        it.second.seqID, it.second.bbEnd, it.second.seqStart, it.second.seqEnd};
  }
  // InstLoc
  for (const auto &it : regions[i + 1].instCache) {
    regions[i].instCache[it.first] = InstLoc{
        static_cast<uint16_t>(it.second.blockIdx + regions[i].blocks.size()),
        it.second.instID,
    };
  }

  // range
  regions[i].covered.setEnd(regions[i + 1].covered.end());

  // ExecBlock
  std::move(regions[i + 1].blocks.begin(), regions[i + 1].blocks.end(),
            std::back_inserter(regions[i].blocks));
  // flush
  regions[i].toFlush |= regions[i + 1].toFlush;

  regions.erase(regions.begin() + i + 1);
}

size_t ExecBlockManager::findRegion(const Range<rword> &codeRange) {
  size_t best_region = regions.size();
  size_t low = searchRegion(codeRange.start());
  unsigned best_cost = 0xFFFFFFFF;

  // when low == 0, three cases:
  //  - no regions => no loop
  //  - codeRange.start is before the first region => 1 round of the loop to
  //    determine if we create a region or create a new one
  //  - codeRange.start is in or after the first region => 2 rounds to
  //    determine if we create a region or use the first or the second one
  size_t limit = 2;
  if (low == 0 and regions.size() != 0 and
      codeRange.start() < regions[0].covered.start())
    limit = 1;

  for (size_t i = low; i < low + limit && i < regions.size(); i++) {
    unsigned cost = 0;
    // Easy case: the code range is inside one of the region, we can return
    // immediately
    if (regions[i].covered.contains(codeRange)) {
      QBDI_DEBUG(
          "Basic block [0x{:x}, 0x{:x}] assigned to region {} [0x{:x}, 0x{:x}]",
          codeRange.start(), codeRange.end(), i, regions[i].covered.start(),
          regions[i].covered.end());
      return i;
    }

    // Medium case: the code range overlaps.
    // This case may append when instrument unaligned code
    // In order to avoid two regions to overlaps,
    //  we must use the first region that overlaps
    if (regions[i].covered.overlaps(codeRange)) {
      QBDI_DEBUG(
          "Region {} [0x{:x}, 0x{:x}] overlaps a part of basic block [0x{:x}, "
          "0x{:x}], Try extend",
          i, regions[i].covered.start(), regions[i].covered.end(),
          codeRange.start(), codeRange.end());
      // part 1, codeRange.start() must be on the regions[i] to avoid error with
      // searchRegion
      if (codeRange.start() < regions[i].covered.start()) {
        // the previous region mustn't containt codeRange.start
        // (should never happend as the iteration test the previous block first)
        QBDI_REQUIRE_ABORT(i == 0 or regions[i - 1].covered.end() <=
                                         codeRange.start(),
                           "Internal Error");
        regions[i].covered.setStart(codeRange.start());
      }

      // part 2, codeRange.end() SHOULD be in the region.
      if (regions[i].covered.end() < codeRange.end()) {
        if (i + 1 == regions.size() or
            codeRange.end() <= regions[i + 1].covered.start()) {
          // extend the current region if no overlaps the next region
          regions[i].covered.setEnd(codeRange.end());
        } else {
          // the range overlaps two region, merge them
          mergeRegion(i);
        }
      }
      QBDI_DEBUG("New Region {} [0x{:x}, 0x{:x}]", i,
                 regions[i].covered.start(), regions[i].covered.end());

      return i;
    }

    // Hard case: it's in the available budget of one the region. Keep the
    // lowest cost. First compute the required cost for the region to cover this
    // extended range.
    if (regions[i].covered.end() < codeRange.end()) {
      cost += (codeRange.end() - regions[i].covered.end());
    }
    if (regions[i].covered.start() > codeRange.start()) {
      cost += (regions[i].covered.start() - codeRange.start());
    }
    // Make sure that such cost is available and that it's better than previous
    // candidates
    if (cost < regions[i].available &&
        (cost == 0 ||
         getExpansionRatio() < static_cast<float>(regions[i].available) /
                                   static_cast<float>(cost)) &&
        cost < best_cost) {
      best_cost = cost;
      best_region = i;
    }
  }
  // We found an extension candidate
  if (best_region != regions.size()) {
    QBDI_DEBUG(
        "Extending region {} [0x{:x}, 0x{:x}] to cover basic block [0x{:x}, "
        "0x{:x}]",
        best_region, regions[best_region].covered.start(),
        regions[best_region].covered.end(), codeRange.start(), codeRange.end());
    if (regions[best_region].covered.end() < codeRange.end()) {
      regions[best_region].covered.setEnd(codeRange.end());
    }
    if (regions[best_region].covered.start() > codeRange.start()) {
      regions[best_region].covered.setStart(codeRange.start());
    }
    return best_region;
  }
  // Else we have to create a new region
  // Find a place to insert it
  size_t insert = low;
  for (; insert < regions.size(); insert++) {
    if (regions[insert].covered.start() > codeRange.start()) {
      break;
    }
  }
  QBDI_DEBUG("Creating new region {} to cover basic block [0x{:x}, 0x{:x}]",
             insert, codeRange.start(), codeRange.end());
  regions.insert(regions.begin() + insert, {codeRange, 0, 0, {}});
  return insert;
}

void ExecBlockManager::updateRegionStat(size_t r, rword translated) {
  regions[r].translated += translated;
  // Remaining code block space
  regions[r].available = regions[r].blocks[0]->getEpilogueOffset();
  // Space which needs to be reserved for the non translated part of the covered
  // region
  unsigned reserved = static_cast<unsigned>(
      (static_cast<float>(regions[r].covered.size() - regions[r].translated)) *
      getExpansionRatio());
  QBDI_DEBUG(
      "Region {} has {} bytes available of which {} are reserved for {} bytes "
      "of untranslated code",
      r, regions[r].available, reserved,
      (regions[r].covered.size() - regions[r].translated));
  if (reserved > regions[r].available) {
    regions[r].available = 0;
  } else {
    regions[r].available -= reserved;
  }
}

void ExecBlockManager::clearCache(RangeSet<rword> rangeSet) {
  const std::vector<Range<rword>> &ranges = rangeSet.getRanges();
  for (Range<rword> r : ranges) {
    clearCache(r);
  }
  // Probably comming from an instrumentation change, reset translation counters
  total_translated_size = 1;
  total_translation_size = 1;
}

void ExecBlockManager::flushCommit() {
  // It needs to be erased from last to first to preserve index validity
  if (needFlush) {
    QBDI_DEBUG("Flushing analysis caches");
    regions.erase(std::remove_if(regions.begin(), regions.end(),
                                 [](const ExecRegion &r) -> bool {
                                   if (r.toFlush)
                                     QBDI_DEBUG(
                                         "Erasing region [0x{:x}, 0x{:x}]",
                                         r.covered.start(), r.covered.end());
                                   return r.toFlush;
                                 }),
                  regions.end());
    needFlush = false;
  }
}

void ExecBlockManager::clearCache(Range<rword> range) {
  size_t i = 0;
  QBDI_DEBUG("Erasing range [0x{:x}, 0x{:x}]", range.start(), range.end());
  for (i = 0; i < regions.size(); i++) {
    if (regions[i].covered.overlaps(range)) {
      regions[i].toFlush = true;
      needFlush = true;
    }
  }
}

void ExecBlockManager::clearCache(bool flushNow) {
  QBDI_DEBUG("Erasing all cache");
  if (flushNow) {
    regions.clear();
    total_translated_size = 1;
    total_translation_size = 1;
    needFlush = false;
  } else {
    for (auto &r : regions) {
      r.toFlush = true;
      needFlush = true;
    }
  }
}

} // namespace QBDI
