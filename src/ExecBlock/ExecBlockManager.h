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
#ifndef EXECBLOCKMANAGER_H
#define EXECBLOCKMANAGER_H

#include <algorithm>
#include <map>
#include <memory>
#include <stddef.h>
#include <stdint.h>
#include <vector>

#include "QBDI/Callback.h"
#include "QBDI/Range.h"
#include "QBDI/State.h"

namespace QBDI {

class ExecBlock;
class ExecBroker;
class LLVMCPUs;
class Patch;
class RelocatableInst;

struct InstLoc {
  uint16_t blockIdx;
  uint16_t instID;
};

struct SeqLoc {
  uint16_t blockIdx;
  uint16_t seqID;
  rword bbEnd;
  rword seqStart;
  rword seqEnd;
};

struct ExecRegion {
  Range<rword> covered;
  unsigned translated;
  unsigned available;
  std::vector<std::unique_ptr<ExecBlock>> blocks;
  // Note for sequenceCache instCache
  // The key must be generate with getExecRegionKey
  std::map<rword, SeqLoc> sequenceCache;
  std::map<rword, InstLoc> instCache;
  bool toFlush = false;

  // lambda ptr for user callback set with addInstrRule
  // These pointers should be remove at the same time as the region
  std::vector<std::unique_ptr<InstCbLambda>> userInstCB;

  ExecRegion(ExecRegion &&) = default;
  ExecRegion &operator=(ExecRegion &&) = default;
};

class ExecBlockManager {
private:
  std::unique_ptr<ExecBroker> execBroker;
  std::vector<ExecRegion> regions;
  rword total_translated_size;
  rword total_translation_size;
  bool needFlush;

  VMInstanceRef vminstance;
  const LLVMCPUs &llvmCPUs;

  // cache ExecBlock prologue and epilogue
  uint32_t epilogueSize;
  const std::vector<std::unique_ptr<RelocatableInst>> execBlockPrologue;
  const std::vector<std::unique_ptr<RelocatableInst>> execBlockEpilogue;

  size_t searchRegion(rword start) const;

  void mergeRegion(size_t i);

  size_t findRegion(const Range<rword> &codeRange);

  void updateRegionStat(size_t r, rword translated);

  float getExpansionRatio() const;

public:
  ExecBlockManager(const LLVMCPUs &llvmCPUs,
                   VMInstanceRef vminstance = nullptr);

  ~ExecBlockManager();

  ExecBlockManager(const ExecBlockManager &) = delete;

  void changeVMInstanceRef(VMInstanceRef vminstance);

  inline ExecBroker *getExecBroker() { return execBroker.get(); }

  void printCacheStatistics() const;

  ExecBlock *getProgrammedExecBlock(rword address, CPUMode cpumode,
                                    SeqLoc *programmedSeqLock = nullptr);

  const ExecBlock *getExecBlock(rword address, CPUMode cpumode) const;

  size_t preWriteBasicBlock(const std::vector<Patch> &basicBlock);

  void writeBasicBlock(std::vector<Patch> &&basicBlock, size_t patchEnd);

  bool isFlushPending() { return needFlush; }

  void flushCommit();

  void clearCache(bool flushNow = true);

  void clearCache(Range<rword> range);

  void clearCache(RangeSet<rword> rangeSet);
};

} // namespace QBDI

#endif
