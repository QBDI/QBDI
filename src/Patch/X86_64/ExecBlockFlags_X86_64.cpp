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

#include "Patch/Types.h"
#include "Patch/InstInfo.h"
#include "Patch/ExecBlockFlags.h"

namespace QBDI {

const uint8_t defaultExecuteFlags = ExecBlockFlags::needAVX | ExecBlockFlags::needFPU;

static inline uint8_t getRegisterFlags(unsigned int op) {
    register uint8_t flags = 0;

    if (llvm::X86::YMM0<= op && op <= llvm::X86::YMM15)
        flags |= ExecBlockFlags::needAVX | ExecBlockFlags::needFPU;

    if (    (llvm::X86::XMM0<= op && op <= llvm::X86::XMM15) ||
            (llvm::X86::ST0<= op && op <= llvm::X86::ST7) ||
            (llvm::X86::MM0<= op && op <= llvm::X86::MM7) ||
            llvm::X86::FPSW == op || llvm::X86::FPCW == op)
        flags |= ExecBlockFlags::needFPU;

    return flags;
}

uint8_t getExecBlockFlags(const llvm::MCInst *inst, llvm::MCInstrInfo* MCII, llvm::MCRegisterInfo* MRI) {

    const llvm::MCInstrDesc &desc = MCII->get(inst->getOpcode());
    uint8_t flags = 0;

    // register flag
    for (size_t i=0; i < inst->getNumOperands(); i++) {
        const llvm::MCOperand& op = inst->getOperand(i);
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

    // detect implicit FPU instruction
    if ((desc.TSFlags & llvm::X86II::FPTypeMask) != 0)
        flags |= ExecBlockFlags::needFPU;

    return flags;
}

}
