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

#include "ExecBlock/ExecBlock.h"
#include "Patch/InstInfo.h"
#include "Patch/MemoryAccess.h"
#include "Patch/PatchCondition.h"
#include "Patch/PatchGenerator.h"
#include "Utility/LogSys.h"

#include "Callback.h"

namespace QBDI {

void analyseMemoryAccess(const ExecBlock& curExecBlock, uint16_t instID, bool afterInst, std::vector<MemoryAccess>& dest) {

    llvm::ArrayRef<ShadowInfo> shadows = curExecBlock.getShadowByInst(instID);
    LogDebug("analyseMemoryAccess", "Got %zu shadows for Instruction %" PRIu16, shadows.size(), instID);

    size_t i = 0;
    while(i < shadows.size()) {
        Require("analyseMemoryAccess", shadows[i].instID == instID);

        auto access = MemoryAccess();
        if(shadows[i].tag == MEM_READ_ADDRESS_TAG) {
            access.type = MEMORY_READ;
            access.size = getReadSize(curExecBlock.getOriginalMCInst(instID));
        }
        else if(afterInst && shadows[i].tag == MEM_WRITE_ADDRESS_TAG) {
            access.type = MEMORY_WRITE;
            access.size = getWriteSize(curExecBlock.getOriginalMCInst(instID));
        }
        else {
            i += 1;
            continue;
        }
        access.accessAddress = curExecBlock.getShadow(shadows[i].shadowID);
        access.instAddress = curExecBlock.getInstAddress(instID);
        i += 1;

        if(i >= shadows.size()) {
            LogError(
                "analyseMemoryAccess",
                "An address shadow is not followed by a shadow for instruction at address %" PRIx64,
                access.instAddress
            );
            continue;
        }
        Require("analyseMemoryAccess", shadows[i].instID == instID);

        if(shadows[i].tag == MEM_VALUE_TAG) {
            access.value = curExecBlock.getShadow(shadows[i].shadowID);
        }
        else {
            LogError(
                "analyseMemoryAccess",
                "An address shadow is not followed by a value shadow for instruction at address %" PRIx64,
                access.instAddress
            );
            continue;
        }

        // we found our access and its value, record access
        dest.push_back(access);
        i += 1;
    }
}

std::vector<std::unique_ptr<InstrRule>> getInstrRuleMemAccessRead() {
    return conv_unique<InstrRule>(
        InstrRuleBasic(
            DoesReadAccess(),
            {
                GetReadAddress(Temp(0)),
                WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                GetReadValue(Temp(0)),
                WriteTemp(Temp(0), Shadow(MEM_VALUE_TAG)),
            },
            PREINST,
            false,
            0x8000 /* first PREINST */)
        );
}

std::vector<std::unique_ptr<InstrRule>> getInstrRuleMemAccessWrite() {
    return conv_unique<InstrRule>(
        InstrRuleBasic(
            DoesWriteAccess(),
            {
                GetWriteAddress(Temp(0)),
                WriteTemp(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                GetWriteValue(Temp(0)),
                WriteTemp(Temp(0), Shadow(MEM_VALUE_TAG)),
            },
            POSTINST,
            false,
            -0x8000 /* first POSTINST */)
        );
}


}

