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
#include "llvm/MC/MCInstrInfo.h"

#include "ExecBlock/ExecBlock.h"
#include "Patch/MemoryAccess.h"
#include "Patch/Patch.h"
#include "Patch/PatchCondition.h"
#include "Patch/X86_64/InstInfo_X86_64.h"
#include "Patch/X86_64/PatchGenerator_X86_64.h"
#include "Utility/LogSys.h"

#include "Callback.h"

namespace QBDI {

enum MemoryTag : uint16_t {
    MEM_READ_ADDRESS_TAG  = MEMORY_TAG_BEGIN + 0,
    MEM_WRITE_ADDRESS_TAG = MEMORY_TAG_BEGIN + 1,

    MEM_READ_VALUE_TAG    = MEMORY_TAG_BEGIN + 2,
    MEM_WRITE_VALUE_TAG   = MEMORY_TAG_BEGIN + 3,

    MEM_READ_0_BEGIN_ADDRESS_TAG  = MEMORY_TAG_BEGIN + 4,
    MEM_READ_1_BEGIN_ADDRESS_TAG  = MEMORY_TAG_BEGIN + 5,
    MEM_WRITE_BEGIN_ADDRESS_TAG   = MEMORY_TAG_BEGIN + 6,

    MEM_READ_0_END_ADDRESS_TAG    = MEMORY_TAG_BEGIN + 7,
    MEM_READ_1_END_ADDRESS_TAG    = MEMORY_TAG_BEGIN + 8,
    MEM_WRITE_END_ADDRESS_TAG     = MEMORY_TAG_BEGIN + 9,
};

void analyseMemoryAccessAddrValue(const ExecBlock& curExecBlock, llvm::ArrayRef<ShadowInfo>& shadows, std::vector<MemoryAccess>& dest) {
    if (shadows.size() < 1) {
        return;
    }

    auto access = MemoryAccess();
    access.flags = MEMORY_NO_FLAGS;

    uint16_t expectValueTag;
    const llvm::MCInst& inst = curExecBlock.getOriginalMCInst(shadows[0].instID);
    switch (shadows[0].tag) {
        default:
            return;
        case MEM_READ_ADDRESS_TAG:
            access.type = MEMORY_READ;
            access.size = getReadSize(inst);
            expectValueTag = MEM_READ_VALUE_TAG;
            if (isMinSizeRead(inst)) {
                access.flags |= MEMORY_MINIMUM_SIZE;
            }
            break;
        case MEM_WRITE_ADDRESS_TAG:
            access.type = MEMORY_WRITE;
            access.size = getWriteSize(inst);
            expectValueTag = MEM_WRITE_VALUE_TAG;
            if (isMinSizeRead(inst)) {
                access.flags |= MEMORY_MINIMUM_SIZE;
            }
            break;
    }

    access.accessAddress = curExecBlock.getShadow(shadows[0].shadowID);
    access.instAddress = curExecBlock.getInstAddress(shadows[0].instID);

    if (access.size > sizeof(rword)) {
        access.flags |= MEMORY_UNKNOWN_VALUE;
        access.value = 0;
        dest.push_back(std::move(access));
        return;
    }

    size_t index = 0;
    // search the index of MEM_x_VALUE_TAG. For most instruction, it's the next shadow.
    do {
        index += 1;
        if (index >= shadows.size()) {
            LogError(
                "analyseMemoryAccessAddrValue",
                "Not found shadow tag %" PRIu16 " for instruction %" PRIx64,
                expectValueTag, access.instAddress
            );
            return;
        }
        RequireAction("analyseMemoryAccessAddrValue", shadows[0].instID == shadows[index].instID, return);
    } while (shadows[index].tag != expectValueTag);

    access.value = curExecBlock.getShadow(shadows[index].shadowID);

    dest.push_back(std::move(access));
}

void analyseMemoryAccessAddrRange(const ExecBlock& curExecBlock, llvm::ArrayRef<ShadowInfo>& shadows, bool postInst, std::vector<MemoryAccess>& dest) {
    if (shadows.size() < 1) {
        return;
    }
    auto access = MemoryAccess();
    access.flags = MEMORY_NO_FLAGS;

    uint16_t expectValueTag;
    unsigned accessAtomicSize;
    switch (shadows[0].tag) {
        default:
            return;
        case MEM_READ_0_BEGIN_ADDRESS_TAG:
            access.type = MEMORY_READ;
            expectValueTag = MEM_READ_0_END_ADDRESS_TAG;
            accessAtomicSize = getReadSize(curExecBlock.getOriginalMCInst(shadows[0].instID));
            break;
        case MEM_READ_1_BEGIN_ADDRESS_TAG:
            access.type = MEMORY_READ;
            expectValueTag = MEM_READ_1_END_ADDRESS_TAG;
            accessAtomicSize = getReadSize(curExecBlock.getOriginalMCInst(shadows[0].instID));
            break;
        case MEM_WRITE_BEGIN_ADDRESS_TAG:
            access.type = MEMORY_WRITE;
            expectValueTag = MEM_WRITE_END_ADDRESS_TAG;
            accessAtomicSize = getWriteSize(curExecBlock.getOriginalMCInst(shadows[0].instID));
            break;
    }

    access.instAddress = curExecBlock.getInstAddress(shadows[0].instID);
    access.flags |= MEMORY_UNKNOWN_VALUE;
    access.value = 0;

    if (! postInst) {
        access.accessAddress = curExecBlock.getShadow(shadows[0].shadowID);
        access.flags |= MEMORY_UNKNOWN_SIZE;
        access.size = 0;
        dest.push_back(std::move(access));
        return;
    }

    size_t index = 0;
    // search the index of MEM_x_VALUE_TAG. For most instruction, it's the next shadow.
    do {
        index += 1;
        if (index >= shadows.size()) {
            LogError(
                "analyseMemoryAccessAddrRange",
                "Not found shadow tag %" PRIu16 " for instruction %" PRIx64,
                expectValueTag, access.instAddress
            );
            return;
        }
        RequireAction("analyseMemoryAccessAddrRange", shadows[0].instID == shadows[index].instID, return);
    } while (shadows[index].tag != expectValueTag);

    rword beginAddress = curExecBlock.getShadow(shadows[0].shadowID);
    rword endAddress = curExecBlock.getShadow(shadows[index].shadowID);

    if (endAddress >= beginAddress) {
        access.accessAddress = beginAddress;
        access.size = endAddress - beginAddress;
    } else {
        // the endAddress is lesser than the begin address, this may be the case
        // in X86 with REP prefix and DF=1
        // In this case, the memory have been access between [endAddress + accessSize, beginAddress + accessAtomicSize)
        access.accessAddress = endAddress + accessAtomicSize;
        access.size = beginAddress - endAddress;
    }

    dest.push_back(std::move(access));
}

void analyseMemoryAccess(const ExecBlock& curExecBlock, uint16_t instID, bool afterInst, std::vector<MemoryAccess>& dest) {

    llvm::ArrayRef<ShadowInfo> shadows = curExecBlock.getShadowByInst(instID);
    LogDebug("analyseMemoryAccess", "Got %zu shadows for Instruction %" PRIu16, shadows.size(), instID);

    while(! shadows.empty()) {
        Require("analyseMemoryAccess", shadows[0].instID == instID);

        switch (shadows[0].tag) {
            default:
                break;
            case MEM_READ_ADDRESS_TAG:
                analyseMemoryAccessAddrValue(curExecBlock, shadows, dest);
                break;
            case MEM_WRITE_ADDRESS_TAG:
                if (afterInst) {
                    analyseMemoryAccessAddrValue(curExecBlock, shadows, dest);
                }
                break;
            case MEM_READ_0_BEGIN_ADDRESS_TAG:
            case MEM_READ_1_BEGIN_ADDRESS_TAG:
                analyseMemoryAccessAddrRange(curExecBlock, shadows, afterInst, dest);
                break;
            case MEM_WRITE_BEGIN_ADDRESS_TAG:
                if (afterInst) {
                    analyseMemoryAccessAddrRange(curExecBlock, shadows, afterInst, dest);
                }
                break;
        }
        shadows = shadows.drop_front();
    }
}

static PatchGenerator::SharedPtrVec generatePreReadInstrumentPatch(Patch &patch, const llvm::MCInstrInfo* MCII,
                                                                   const llvm::MCRegisterInfo* MRI) {

    // REP prefix
    if (hasREPPrefix(patch.metadata.inst)) {
        if (isDoubleRead(patch.metadata.inst)) {
            return {
                        GetReadAddress(Temp(0), 0),
                        WriteTemp(Temp(0), Shadow(MEM_READ_0_BEGIN_ADDRESS_TAG)),
                        GetReadAddress(Temp(0), 1),
                        WriteTemp(Temp(0), Shadow(MEM_READ_1_BEGIN_ADDRESS_TAG)),
                    };
        } else {
            return {
                        GetReadAddress(Temp(0)),
                        WriteTemp(Temp(0), Shadow(MEM_READ_0_BEGIN_ADDRESS_TAG)),
                    };
        }
    }
    // instruction with double read
    else if (isDoubleRead(patch.metadata.inst)) {
        if (getReadSize(patch.metadata.inst) > sizeof(rword)) {
            return {
                        GetReadAddress(Temp(0), 0),
                        WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                        GetReadAddress(Temp(0), 1),
                        WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                    };
        } else {
            return {
                        GetReadAddress(Temp(0), 0),
                        WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                        GetReadValue(Temp(0), 0),
                        WriteTemp(Temp(0), Shadow(MEM_READ_VALUE_TAG)),
                        GetReadAddress(Temp(0), 1),
                        WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                        GetReadValue(Temp(0), 1),
                        WriteTemp(Temp(0), Shadow(MEM_READ_VALUE_TAG)),
                    };
        }
    } else {
        if (getReadSize(patch.metadata.inst) > sizeof(rword)) {
            return {
                        GetReadAddress(Temp(0)),
                        WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                    };
        } else {
            return {
                        GetReadAddress(Temp(0)),
                        WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                        GetReadValue(Temp(0)),
                        WriteTemp(Temp(0), Shadow(MEM_READ_VALUE_TAG)),
                    };
        }
    }
}

static PatchGenerator::SharedPtrVec generatePostReadInstrumentPatch(Patch &patch, const llvm::MCInstrInfo* MCII,
                                                                   const llvm::MCRegisterInfo* MRI) {

    // REP prefix
    if (hasREPPrefix(patch.metadata.inst)) {
        if (isDoubleRead(patch.metadata.inst)) {
            return {
                        GetReadAddress(Temp(0), 0),
                        WriteTemp(Temp(0), Shadow(MEM_READ_0_END_ADDRESS_TAG)),
                        GetReadAddress(Temp(0), 1),
                        WriteTemp(Temp(0), Shadow(MEM_READ_1_END_ADDRESS_TAG)),
                    };
        } else {
            return {
                        GetReadAddress(Temp(0)),
                        WriteTemp(Temp(0), Shadow(MEM_READ_0_END_ADDRESS_TAG)),
                    };
        }
    } else {
        return {};
    }
}

static PatchGenerator::SharedPtrVec generatePreWriteInstrumentPatch(Patch &patch, const llvm::MCInstrInfo* MCII,
                                                                    const llvm::MCRegisterInfo* MRI) {

    const llvm::MCInstrDesc& desc = MCII->get(patch.metadata.inst.getOpcode());

    if (hasREPPrefix(patch.metadata.inst)) {
        return {
                    GetWriteAddress(Temp(0)),
                    WriteTemp(Temp(0), Shadow(MEM_WRITE_BEGIN_ADDRESS_TAG)),
                };
    }
    // Some instruction need to have the address get before the instruction
    else if (mayChangeWriteAddr(patch.metadata.inst, desc) && !isStackWrite(patch.metadata.inst)) {
        return {
                    GetWriteAddress(Temp(0)),
                    WriteTemp(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                };
    } else {
        return {};
    }
}

static PatchGenerator::SharedPtrVec generatePostWriteInstrumentPatch(Patch &patch, const llvm::MCInstrInfo* MCII,
                                                                     const llvm::MCRegisterInfo* MRI) {

    const llvm::MCInstrDesc& desc = MCII->get(patch.metadata.inst.getOpcode());

    if (hasREPPrefix(patch.metadata.inst)) {
        return {
                    GetWriteAddress(Temp(0)),
                    WriteTemp(Temp(0), Shadow(MEM_WRITE_END_ADDRESS_TAG)),
                };
    }
    // Some instruction need to have the address get before the instruction
    else if (mayChangeWriteAddr(patch.metadata.inst, desc) && !isStackWrite(patch.metadata.inst)) {
        if (getWriteSize(patch.metadata.inst) > sizeof(rword)) {
            return {};
        } else {
            return {
                        ReadTemp(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                        GetWriteValue(Temp(0)),
                        WriteTemp(Temp(0), Shadow(MEM_WRITE_VALUE_TAG)),
                    };
        }
    } else {
        if (getWriteSize(patch.metadata.inst) > sizeof(rword)) {
            return {
                        GetWriteAddress(Temp(0)),
                        WriteTemp(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                    };
        } else {
            return {
                        GetWriteAddress(Temp(0)),
                        WriteTemp(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                        GetWriteValue(Temp(0)),
                        WriteTemp(Temp(0), Shadow(MEM_WRITE_VALUE_TAG)),
                    };
        }
    }
}

std::vector<std::unique_ptr<InstrRule>> getInstrRuleMemAccessRead() {
    return conv_unique<InstrRule>(
        InstrRuleDynamic(
            DoesReadAccess(),
            generatePreReadInstrumentPatch,
            PREINST,
            false,
            0x8000 /* first PREINST */),
        InstrRuleDynamic(
            DoesReadAccess(),
            generatePostReadInstrumentPatch,
            POSTINST,
            false,
            -0x8000 /* first POSTINST */)
        );
}

std::vector<std::unique_ptr<InstrRule>> getInstrRuleMemAccessWrite() {
    return conv_unique<InstrRule>(
        InstrRuleDynamic(
            DoesWriteAccess(),
            generatePreWriteInstrumentPatch,
            PREINST,
            false,
            0x8000 /* first PRETINST */),
        InstrRuleDynamic(
            DoesWriteAccess(),
            generatePostWriteInstrumentPatch,
            POSTINST,
            false,
            -0x8000 /* first POSTINST */)
        );
}


}

