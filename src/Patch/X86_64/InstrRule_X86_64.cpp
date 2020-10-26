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

#include "Patch/InstrRule.h"
#include "Patch/Patch.h"
#include "Patch/X86_64/InstInfo_X86_64.h"
#include "Patch/X86_64/PatchGenerator_X86_64.h"
#include "Patch/X86_64/RelocatableInst_X86_64.h"

#include "Config.h"

namespace QBDI {

namespace {

PatchGenerator::SharedPtrVec generatePreReadInstrumentPatch(Patch &patch, const llvm::MCInstrInfo* MCII,
                                                            const llvm::MCRegisterInfo* MRI) {

    uint64_t TsFlags = MCII->get(patch.metadata.inst.getOpcode()).TSFlags;
    if (hasREPPrefix(patch.metadata.inst)) {
        if (isDoubleRead(&patch.metadata.inst)) {
            return {
                        GetReadAddress(Temp(0), 0, TsFlags),
                        WriteTemp(Temp(0), Shadow(MEM_READ_START_ADDRESS_1_TAG)),
                        GetReadAddress(Temp(0), 1, TsFlags),
                        WriteTemp(Temp(0), Shadow(MEM_READ_START_ADDRESS_2_TAG)),
                    };
        } else {
            return {
                        GetReadAddress(Temp(0), 0, TsFlags),
                        WriteTemp(Temp(0), Shadow(MEM_READ_START_ADDRESS_1_TAG)),
                    };
        }
    } else {
        if (isDoubleRead(&patch.metadata.inst)) {
            if (getReadSize(&patch.metadata.inst) > sizeof(rword)) {
                return {
                            GetReadAddress(Temp(0), 0, TsFlags),
                            WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                            GetReadAddress(Temp(0), 1, TsFlags),
                            WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                        };
            } else {
                return {
                            GetReadAddress(Temp(0), 0, TsFlags),
                            WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                            GetReadValue(Temp(0), 0, TsFlags),
                            WriteTemp(Temp(0), Shadow(MEM_VALUE_TAG)),
                            GetReadAddress(Temp(0), 1, TsFlags),
                            WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                            GetReadValue(Temp(0), 1, TsFlags),
                            WriteTemp(Temp(0), Shadow(MEM_VALUE_TAG)),
                        };
            }
        } else {
            if (getReadSize(&patch.metadata.inst) > sizeof(rword)) {
                return {
                            GetReadAddress(Temp(0), 0, TsFlags),
                            WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                        };
            } else {
                return {
                            GetReadAddress(Temp(0), 0, TsFlags),
                            WriteTemp(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
                            GetReadValue(Temp(0), 0, TsFlags),
                            WriteTemp(Temp(0), Shadow(MEM_VALUE_TAG)),
                        };
            }
        }
    }
}

PatchGenerator::SharedPtrVec generatePostReadInstrumentPatch(Patch &patch, const llvm::MCInstrInfo* MCII,
                                                             const llvm::MCRegisterInfo* MRI) {
    uint64_t TsFlags = MCII->get(patch.metadata.inst.getOpcode()).TSFlags;
    if (hasREPPrefix(patch.metadata.inst)) {
        if (isDoubleRead(&patch.metadata.inst)) {
            return {
                        GetReadAddress(Temp(0), 0, TsFlags),
                        WriteTemp(Temp(0), Shadow(MEM_READ_STOP_ADDRESS_1_TAG)),
                        GetReadAddress(Temp(0), 1, TsFlags),
                        WriteTemp(Temp(0), Shadow(MEM_READ_STOP_ADDRESS_2_TAG)),
                    };
        } else {
            return {
                        GetReadAddress(Temp(0), 0, TsFlags),
                        WriteTemp(Temp(0), Shadow(MEM_READ_STOP_ADDRESS_1_TAG)),
                    };
        }
    } else {
        return {};
    }
}

PatchGenerator::SharedPtrVec generatePreWriteInstrumentPatch(Patch &patch, const llvm::MCInstrInfo* MCII,
                                                             const llvm::MCRegisterInfo* MRI) {
    if (hasREPPrefix(patch.metadata.inst)) {
        uint64_t TsFlags = MCII->get(patch.metadata.inst.getOpcode()).TSFlags;
        return {
                    GetWriteAddress(Temp(0), TsFlags),
                    WriteTemp(Temp(0), Shadow(MEM_WRITE_START_ADDRESS_TAG)),
                };
    } else {
        return {};
    }
}

PatchGenerator::SharedPtrVec generatePostWriteInstrumentPatch(Patch &patch, const llvm::MCInstrInfo* MCII,
                                                              const llvm::MCRegisterInfo* MRI) {

    uint64_t TsFlags = MCII->get(patch.metadata.inst.getOpcode()).TSFlags;
    if (hasREPPrefix(patch.metadata.inst)) {
        return {
                    GetWriteAddress(Temp(0), TsFlags),
                    WriteTemp(Temp(0), Shadow(MEM_WRITE_STOP_ADDRESS_TAG)),
                };
    } else {
        if (getWriteSize(&patch.metadata.inst) > sizeof(rword)) {
            return {
                        GetWriteAddress(Temp(0), TsFlags),
                        WriteTemp(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                    };
        } else {
            return {
                        GetWriteAddress(Temp(0), TsFlags),
                        WriteTemp(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                        GetWriteValue(Temp(0), TsFlags),
                        WriteTemp(Temp(0), Shadow(MEM_VALUE_TAG)),
                    };
        }
    }
}


} // namespace anonymous

std::unique_ptr<InstrRule> getMemReadPreInstrRule() {

    return InstrRuleDynamic(
            DoesReadAccess(), generatePreReadInstrumentPatch,
            PREINST,
            false,
            LASTPASS // need to be before user PREINST callback
        );
}

std::unique_ptr<InstrRule> getMemReadPostInstrRule() {
    return InstrRuleDynamic(
            DoesReadAccess(), generatePostReadInstrumentPatch,
            POSTINST,
            false,
            FIRSTPASS
        );
}

std::unique_ptr<InstrRule> getMemWritePreInstrRule() {

    return InstrRuleDynamic(
            DoesWriteAccess(), generatePreWriteInstrumentPatch,
            PREINST,
            false,
            LASTPASS
        );
}

std::unique_ptr<InstrRule> getMemWritePostInstrRule() {

    return InstrRuleDynamic(
            DoesWriteAccess(), generatePostWriteInstrumentPatch,
            POSTINST,
            false,
            FIRSTPASS // need to be before user POSTINST callback
        );
}

} // namespace QBDI
