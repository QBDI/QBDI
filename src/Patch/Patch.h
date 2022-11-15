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
#ifndef PATCH_H
#define PATCH_H

#include <array>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "Patch/InstMetadata.h"
#include "Patch/Register.h"
#include "Patch/Types.h"

#include "QBDI/Callback.h"
#include "QBDI/State.h"

namespace llvm {
class MCInst;
}

namespace QBDI {
class LLVMCPU;
class RelocatableInst;

struct InstrPatch {
  InstPosition position;
  int priority;
  std::vector<std::unique_ptr<RelocatableInst>> insts;
};

class Patch {
private:
  std::vector<InstrPatch> instsPatchs;

public:
  InstMetadata metadata;
  std::vector<std::unique_ptr<RelocatableInst>> insts;
  // flags generate by the PatchRule
  std::vector<std::pair<unsigned int, int>> patchGenFlags;
  int patchGenFlagsOffset = 0;
  // InstCbLambda to register in the ExecBlockManager
  std::vector<std::unique_ptr<InstCbLambda>> userInstCB;
  // Registers Used and Defs by the instruction
  std::array<RegisterUsage, NUM_GPR> regUsage;
  std::map<RegLLVM, RegisterUsage> regUsageExtra;
  // Registers used by the TempRegister for this patch
  std::set<RegLLVM> tempReg;
  const LLVMCPU *llvmcpu;
  bool finalize = false;

  using Vec = std::vector<Patch>;

  Patch(const llvm::MCInst &inst, rword address, uint32_t instSize,
        const LLVMCPU &llvmcpu);

  Patch(Patch &&);
  Patch &operator=(Patch &&);

  ~Patch();

  void setModifyPC(bool modifyPC);

  void append(std::unique_ptr<RelocatableInst> &&r);
  void append(std::vector<std::unique_ptr<RelocatableInst>> v);

  void prepend(std::unique_ptr<RelocatableInst> &&r);
  void prepend(std::vector<std::unique_ptr<RelocatableInst>> v);

  void insertAt(unsigned position,
                std::vector<std::unique_ptr<RelocatableInst>> v);

  void addInstsPatch(InstPosition position, int priority,
                     std::vector<std::unique_ptr<RelocatableInst>> v);

  void finalizeInstsPatch();
};

} // namespace QBDI

#endif // PATCH_H
