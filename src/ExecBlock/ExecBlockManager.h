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
#ifndef EXECBLOCKMANAGER_H
#define EXECBLOCKMANAGER_H

#include <algorithm>
#include <map>
#include <vector>

#include "Callback.h"
#include "Range.h"
#include "State.h"

namespace llvm {
  class MCInstrInfo;
  class MCRegisterInfo;
}

namespace QBDI {

class Assembly;
class ExecBlock;
class InstMetadata;
class Patch;
class RelocatableInst;

struct InstLoc {
    uint16_t blockIdx;
    uint16_t instID;
};

struct SeqLoc {
    uint16_t blockIdx;
    uint16_t seqID;
    rword bbStart;
    rword bbEnd;
    rword seqStart;
    rword seqEnd;
};

struct ExecRegion {
    Range<rword>                             covered;
    unsigned                                 translated;
    unsigned                                 available;
    std::vector<std::unique_ptr<ExecBlock>>  blocks;
    std::map<rword, SeqLoc>                  sequenceCache;
    std::map<rword, InstLoc>                 instCache;
    bool                                     toFlush = false;

    ExecRegion(ExecRegion&&) = default;
    ExecRegion& operator=(ExecRegion&&) = default;
};

class ExecBlockManager {
private:
    std::vector<ExecRegion>            regions;
    rword                              total_translated_size;
    rword                              total_translation_size;
    bool                               needFlush;

    VMInstanceRef              vminstance;
    Assembly&                  assembly;

    size_t searchRegion(rword start) const;

    void mergeRegion(size_t i);

    size_t findRegion(const Range<rword>& codeRange);

    void updateRegionStat(size_t r, rword translated);

    float getExpansionRatio() const;

public:

    ExecBlockManager(Assembly& assembly, VMInstanceRef vminstance = nullptr);

    ~ExecBlockManager();

    ExecBlockManager(const ExecBlockManager&) = delete;

    void changeVMInstanceRef(VMInstanceRef vminstance);

    void printCacheStatistics(FILE* output) const;

    ExecBlock* getProgrammedExecBlock(rword address);

    const ExecBlock* getExecBlock(rword address) const;

    const SeqLoc* getSeqLoc(rword address) const;

    void writeBasicBlock(const std::vector<Patch>& basicBlock);

    bool isFlushPending() { return needFlush; }

    void flushCommit();

    void clearCache(bool flushNow=true);

    void clearCache(Range<rword> range);

    void clearCache(RangeSet<rword> rangeSet);
};

}

#endif
