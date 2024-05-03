/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
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
#ifndef RELOCATABLEINST_AARCH64_H
#define RELOCATABLEINST_AARCH64_H

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

class SetScratchRegister
    : public AutoClone<RelocatableInst, SetScratchRegister> {
  llvm::MCInst inst;
  Operand opn;

public:
  SetScratchRegister(llvm::MCInst &&inst, Operand opn)
      : AutoClone<RelocatableInst, SetScratchRegister>(),
        inst(std::forward<llvm::MCInst>(inst)), opn(opn) {}

  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override { return 4; }
};

class EpilogueAddrRel : public AutoClone<RelocatableInst, EpilogueAddrRel> {
  llvm::MCInst inst;
  Operand opn;
  rword offset;

public:
  EpilogueAddrRel(llvm::MCInst &&inst, Operand opn, rword offset)
      : AutoClone<RelocatableInst, EpilogueAddrRel>(),
        inst(std::forward<llvm::MCInst>(inst)), opn(opn), offset(offset) {}

  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override { return 4; }
};

class RestoreScratchRegister
    : public AutoClone<RelocatableInst, RestoreScratchRegister> {

public:
  RestoreScratchRegister() {}

  // Restore the original value of the scratch register
  // /!\ After this instruction, only use NoReloc
  // until the scratch register is restored with ResetScratchRegister
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override { return 4; }
};

class ResetScratchRegister
    : public AutoClone<RelocatableInst, ResetScratchRegister> {

public:
  ResetScratchRegister() {}

  // Set the Datablock address in the ScratchRegister
  // The value of the scratch register is lost
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override { return 4; }
};

class SetBaseAddress : public AutoClone<RelocatableInst, SetBaseAddress> {
  RegLLVM reg;

public:
  SetBaseAddress(RegLLVM reg) : reg(reg) {}

  // Set the Datablock address in the ScratchRegister
  // The value of the scratch register is lost
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override { return 4; }
};

class LoadDataBlockX2 : public AutoClone<RelocatableInst, LoadDataBlockX2> {
  RegLLVM reg;
  RegLLVM reg2;
  int64_t offset;

public:
  LoadDataBlockX2(RegLLVM reg, RegLLVM reg2, int64_t offset)
      : AutoClone<RelocatableInst, LoadDataBlockX2>(), reg(reg), reg2(reg2),
        offset(offset) {}

  // Load a value from the specified offset of the datablock
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override { return 4; }
};

class StoreDataBlockX2 : public AutoClone<RelocatableInst, StoreDataBlockX2> {
  RegLLVM reg;
  RegLLVM reg2;
  int64_t offset;

public:
  StoreDataBlockX2(RegLLVM reg, RegLLVM reg2, int64_t offset)
      : AutoClone<RelocatableInst, StoreDataBlockX2>(), reg(reg), reg2(reg2),
        offset(offset) {}

  // Store a value to the specified offset of the datablock
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override { return 4; }
};

} // namespace QBDI

#endif
