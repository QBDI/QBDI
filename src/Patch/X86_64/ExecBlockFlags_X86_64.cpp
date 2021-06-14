/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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

#include "Engine/LLVMCPU.h"
#include "Patch/ExecBlockFlags.h"
#include "Patch/InstInfo.h"
#include "Patch/Types.h"
#include "Utility/LogSys.h"

namespace QBDI {
namespace {

struct ExecBlockFlagsArray {
  uint8_t arr[llvm::X86::NUM_TARGET_REGS];

  constexpr ExecBlockFlagsArray() : arr() {
    for (unsigned i = 0; i < llvm::X86::NUM_TARGET_REGS; i++) {
      if (llvm::X86::YMM0 <= i && i <= llvm::X86::YMM15) {
        arr[i] = ExecBlockFlags::needAVX | ExecBlockFlags::needFPU;
      } else if ((llvm::X86::XMM0 <= i && i <= llvm::X86::XMM15) ||
                 (llvm::X86::ST0 <= i && i <= llvm::X86::ST7) ||
                 (llvm::X86::MM0 <= i && i <= llvm::X86::MM7) ||
                 llvm::X86::FPSW == i || llvm::X86::FPCW == i) {
        arr[i] = ExecBlockFlags::needFPU;
      } else {
        arr[i] = 0;
      }
    }
  }

  inline uint8_t get(size_t reg) const {
    if (reg < llvm::X86::NUM_TARGET_REGS)
      return arr[reg];

    QBDI_ERROR("No register {}", reg);
    return 0;
  }
};

} // namespace

const uint8_t defaultExecuteFlags =
    ExecBlockFlags::needAVX | ExecBlockFlags::needFPU;

uint8_t getExecBlockFlags(const llvm::MCInst &inst,
                          const QBDI::LLVMCPU &llvmcpu) {
  static constexpr ExecBlockFlagsArray cache;

  const llvm::MCInstrDesc &desc = llvmcpu.getMCII().get(inst.getOpcode());
  uint8_t flags = 0;

  // register flag
  for (size_t i = 0; i < inst.getNumOperands(); i++) {
    const llvm::MCOperand &op = inst.getOperand(i);
    if (op.isReg()) {
      flags |= cache.get(op.getReg());
    }
  }

  const uint16_t *implicitRegs = desc.getImplicitDefs();
  for (; implicitRegs && *implicitRegs; implicitRegs++) {
    flags |= cache.get(*implicitRegs);
  }
  implicitRegs = desc.getImplicitUses();
  for (; implicitRegs && *implicitRegs; implicitRegs++) {
    flags |= cache.get(*implicitRegs);
  }

  // detect implicit FPU instruction
  if ((desc.TSFlags & llvm::X86II::FPTypeMask) != 0) {
    if ((desc.TSFlags & llvm::X86II::FPTypeMask) != llvm::X86II::SpecialFP or
        ((not desc.isReturn()) and (not desc.isCall()))) {
      flags |= ExecBlockFlags::needFPU;
    }
  }

  if ((flags & ExecBlockFlags::needAVX) != 0)
    flags |= ExecBlockFlags::needFPU;

  return flags;
}

} // namespace QBDI
