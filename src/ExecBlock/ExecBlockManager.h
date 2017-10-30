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

#include "Context.h"
#include "InstAnalysis.h"
#include "Range.h"
#include "Utility/Assembly.h"
#include "ExecBlock/ExecBlock.h"


namespace QBDI {

class RelocatableInst;

struct InstLoc {
    uint16_t blockIdx;
    uint16_t instID;
};

struct SeqLoc {
    ExecBlock *execBlock;
    uint16_t seqID;
    uint16_t bbIdx;
};

struct BBInfo {
    rword start;
    rword end;
};

struct ExecRegion {
    Range<rword>                    covered;
    unsigned                        translated; 
    unsigned                        available;
    std::vector<ExecBlock*>         blocks;
    std::vector<BBInfo>             bbRegistry;
    std::map<rword, SeqLoc>         sequenceCache;
    std::map<rword, InstLoc>        instCache;
    std::map<rword, InstAnalysis*>  analysisCache;
};

class ExecBlockManager {
private:

    std::vector<ExecRegion>         regions;
    std::map<rword, InstAnalysis*>  analysisCache;
    std::vector<size_t>             flushList;
    rword                           total_translated_size;
    rword                           total_translation_size;

    VMInstanceRef              vminstance;
    llvm::MCInstrInfo&         MCII;
    llvm::MCRegisterInfo&      MRI;
    Assembly&                  assembly;

    void eraseRegion(size_t r);

    size_t searchRegion(rword start) const;

    size_t findRegion(Range<rword> codeRange);

    SeqLoc getSeqLoc(rword address);

    void updateRegionStat(size_t r, rword translated);

    float getExpansionRatio() const;


public:

    ExecBlockManager(llvm::MCInstrInfo& MCII, llvm::MCRegisterInfo& MRI, Assembly& assembly, VMInstanceRef vminstance = nullptr);

    ~ExecBlockManager();

    void printCacheStatistics(FILE* output) const;

    ExecBlock* getExecBlock(rword address);

    const BBInfo* getBBInfo(rword address) const;

    void writeBasicBlock(const std::vector<Patch>& basicBlock);

    const InstAnalysis* analyzeInstMetadata(const InstMetadata* instMetadata, AnalysisType type);

    bool isFlushPending() { return this->flushList.size() > 0; }

    void flushCommit();

    void clearCache();

    void clearCache(Range<rword> range);

    void clearCache(RangeSet<rword> rangeSet);
};

}

#endif
