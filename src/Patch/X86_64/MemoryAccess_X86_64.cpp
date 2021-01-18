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
};

void analyseMemoryAccessAddrValue(const ExecBlock& curExecBlock, llvm::ArrayRef<ShadowInfo>& shadows, std::vector<MemoryAccess>& dest) {
    if (shadows.size() < 1) {
        return;
    }
    auto access = MemoryAccess();
    uint16_t expectValueTag;
    switch (shadows[0].tag) {
        default:
            return;
        case MEM_READ_ADDRESS_TAG:
            access.type = MEMORY_READ;
            access.size = getReadSize(curExecBlock.getOriginalMCInst(shadows[0].instID));
            expectValueTag = MEM_READ_VALUE_TAG;
            break;
        case MEM_WRITE_ADDRESS_TAG:
            access.type = MEMORY_WRITE;
            access.size = getWriteSize(curExecBlock.getOriginalMCInst(shadows[0].instID));
            expectValueTag = MEM_WRITE_VALUE_TAG;
            break;
    }

    access.accessAddress = curExecBlock.getShadow(shadows[0].shadowID);
    access.instAddress = curExecBlock.getInstAddress(shadows[0].instID);

    if (access.size > sizeof(rword)) {
        access.flags |= MEMORY_UNKNOWN_VALUE;
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
        }
        shadows = shadows.drop_front();
    }
}

static PatchGenerator::SharedPtrVec generateReadInstrumentPatch(Patch &patch, const llvm::MCInstrInfo* MCII,
                                                                const llvm::MCRegisterInfo* MRI) {

    // instruction with double read
    if (isDoubleRead(patch.metadata.inst)) {
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

static PatchGenerator::SharedPtrVec generatePreWriteInstrumentPatch(Patch &patch, const llvm::MCInstrInfo* MCII,
                                                                    const llvm::MCRegisterInfo* MRI) {

    const llvm::MCInstrDesc& desc = MCII->get(patch.metadata.inst.getOpcode());

    // Some instruction need to have the address get before the instruction
    if (mayChangeWriteAddr(patch.metadata.inst, desc) && !isStackWrite(patch.metadata.inst)) {
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

    // Some instruction need to have the address get before the instruction
    if (mayChangeWriteAddr(patch.metadata.inst, desc) && !isStackWrite(patch.metadata.inst)) {
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
            generateReadInstrumentPatch,
            PREINST,
            false,
            0x8000 /* first PREINST */)
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

