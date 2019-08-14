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

#include "X86InstrInfo.h"

#include "Patch/Types.h"
#include "Patch/InstInfo.h"
#include "Patch/ExecBlockFlags.h"

namespace QBDI {

const uint8_t defaultExecuteFlags = ExecBlockFlags::needAVX;

static inline uint8_t getRegisterFlags(unsigned int op) {
    uint8_t flags = 0;

    if (llvm::X86::YMM0 <= op && op <= llvm::X86::YMM15)
        flags |= ExecBlockFlags::needAVX;

    return flags;
}

uint8_t getExecBlockFlags(const llvm::MCInst& inst, const llvm::MCInstrInfo* MCII, const llvm::MCRegisterInfo* MRI) {

    const llvm::MCInstrDesc &desc = MCII->get(inst.getOpcode());
    uint8_t flags = 0;

    for (size_t i=0; i < inst.getNumOperands(); i++) {
        const llvm::MCOperand& op = inst.getOperand(i);
        if (op.isReg()) {
            flags |= getRegisterFlags(op.getReg());
        }
    }

    const uint16_t* implicitRegs = desc.getImplicitDefs();
    for (; implicitRegs && *implicitRegs; implicitRegs++) {
        flags |= getRegisterFlags(*implicitRegs);
    }
    implicitRegs = desc.getImplicitUses();
    for (; implicitRegs && *implicitRegs; implicitRegs++) {
        flags |= getRegisterFlags(*implicitRegs);
    }

    return flags;
}

}
