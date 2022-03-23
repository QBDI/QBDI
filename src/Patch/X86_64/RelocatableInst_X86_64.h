/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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
#ifndef RELOCATABLEINST_X86_64_H
#define RELOCATABLEINST_X86_64_H

#include <memory>
#include <utility>

#include "llvm/MC/MCInst.h"

#include "QBDI/Config.h"
#include "QBDI/State.h"
#include "Patch/PatchUtils.h"
#include "Patch/RelocatableInst.h"
#include "Patch/Types.h"

namespace QBDI {
class ExecBlock;

class EpilogueRel : public AutoClone<RelocatableInst, EpilogueRel> {
  llvm::MCInst inst;
  unsigned int opn;
  rword offset;

public:
  EpilogueRel(llvm::MCInst &&inst, unsigned int opn, rword offset)
      : AutoClone<RelocatableInst, EpilogueRel>(),
        inst(std::forward<llvm::MCInst>(inst)), opn(opn), offset(offset) {}

  // Set an operand to epilogueOffset + offset
  llvm::MCInst reloc(ExecBlock *exec_block) const override;
};

class HostPCRel : public AutoClone<RelocatableInst, HostPCRel> {
  llvm::MCInst inst;
  unsigned int opn;
  rword offset;

public:
  HostPCRel(llvm::MCInst &&inst, unsigned int opn, rword offset)
      : AutoClone<RelocatableInst, HostPCRel>(),
        inst(std::forward<llvm::MCInst>(inst)), opn(opn), offset(offset) {}

  // set a an operand at currentPC + offset
  llvm::MCInst reloc(ExecBlock *exec_block) const override;
};

class DataBlockRel : public AutoClone<RelocatableInst, DataBlockRel> {
  llvm::MCInst inst;
  unsigned int opn;
  rword offset;

public:
  DataBlockRel(llvm::MCInst &&inst, unsigned int opn, rword offset)
      : AutoClone<RelocatableInst, DataBlockRel>(),
        inst(std::forward<llvm::MCInst>(inst)), opn(opn), offset(offset) {}

  llvm::MCInst reloc(ExecBlock *exec_block) const override;
};

class DataBlockAbsRel : public AutoClone<RelocatableInst, DataBlockAbsRel> {
  llvm::MCInst inst;
  unsigned int opn;
  rword offset;

public:
  DataBlockAbsRel(llvm::MCInst &&inst, unsigned int opn, rword offset)
      : AutoClone<RelocatableInst, DataBlockAbsRel>(),
        inst(std::forward<llvm::MCInst>(inst)), opn(opn), offset(offset) {}

  llvm::MCInst reloc(ExecBlock *exec_block) const override;
};

inline std::unique_ptr<RelocatableInst> DataBlockRelx86(llvm::MCInst &&inst,
                                                        unsigned int opn,
                                                        rword offset,
                                                        rword inst_size) {
  if constexpr (is_x86_64) {
    inst.getOperand(opn /* AddrBaseReg */).setReg(Reg(REG_PC));
    return DataBlockRel::unique(std::forward<llvm::MCInst>(inst),
                                opn + 3 /* AddrDisp */, offset - inst_size);
  } else {
    inst.getOperand(opn /* AddrBaseReg */).setReg(0);
    return DataBlockAbsRel::unique(std::forward<llvm::MCInst>(inst),
                                   opn + 3 /* AddrDisp */, offset);
  }
}

} // namespace QBDI

#endif
