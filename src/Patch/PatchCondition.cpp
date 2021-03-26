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

#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"

#include "Patch/InstInfo.h"
#include "Patch/PatchCondition.h"
#include "Utility/String.h"

namespace QBDI {

bool MnemonicIs::test(const llvm::MCInst &inst, rword address, rword instSize,
                      const llvm::MCInstrInfo *MCII) const {
  return QBDI::String::startsWith(mnemonic.c_str(),
                                  MCII->getName(inst.getOpcode()).data());
}

bool OpIs::test(const llvm::MCInst &inst, rword address, rword instSize,
                const llvm::MCInstrInfo *MCII) const {
  return inst.getOpcode() == op;
}

bool RegIs::test(const llvm::MCInst &inst, rword address, rword instSize,
                 const llvm::MCInstrInfo *MCII) const {
  return inst.getOperand(opn).isReg() &&
         inst.getOperand(opn).getReg() == (unsigned int)reg;
}

bool UseReg::test(const llvm::MCInst &inst, rword address, rword instSize,
                  const llvm::MCInstrInfo *MCII) const {
  for (unsigned int i = 0; i < inst.getNumOperands(); i++) {
    const llvm::MCOperand &op = inst.getOperand(i);
    if (op.isReg() && op.getReg() == (unsigned int)reg) {
      return true;
    }
  }
  return false;
}

bool OperandIsReg::test(const llvm::MCInst &inst, rword address, rword instSize,
                        const llvm::MCInstrInfo *MCII) const {
  return inst.getOperand(opn).isReg();
}

bool OperandIsImm::test(const llvm::MCInst &inst, rword address, rword instSize,
                        const llvm::MCInstrInfo *MCII) const {
  return inst.getOperand(opn).isImm();
}

bool DoesReadAccess::test(const llvm::MCInst &inst, rword address,
                          rword instSize, const llvm::MCInstrInfo *MCII) const {
  return getReadSize(inst) > 0;
}

bool DoesWriteAccess::test(const llvm::MCInst &inst, rword address,
                           rword instSize,
                           const llvm::MCInstrInfo *MCII) const {
  return getWriteSize(inst) > 0;
}

bool ReadAccessSizeIs::test(const llvm::MCInst &inst, rword address,
                            rword instSize,
                            const llvm::MCInstrInfo *MCII) const {
  return getReadSize(inst) == (rword)size;
}

bool WriteAccessSizeIs::test(const llvm::MCInst &inst, rword address,
                             rword instSize,
                             const llvm::MCInstrInfo *MCII) const {
  return getWriteSize(inst) == (rword)size;
}

bool IsStackRead::test(const llvm::MCInst &inst, rword address, rword instSize,
                       const llvm::MCInstrInfo *MCII) const {
  return isStackRead(inst);
}

bool IsStackWrite::test(const llvm::MCInst &inst, rword address, rword instSize,
                        const llvm::MCInstrInfo *MCII) const {
  return isStackWrite(inst);
}

} // namespace QBDI
