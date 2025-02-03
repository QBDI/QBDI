/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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
#ifndef PATCHRULEASSEMBLY_AARCH64_H
#define PATCHRULEASSEMBLY_AARCH64_H

#include <stdbool.h>

#include "Patch/PatchRuleAssemblyBase.h"

namespace QBDI {
class PatchRule;

class PatchRuleAssembly final : public PatchRuleAssemblyBase {
  std::vector<PatchRule> patchRules;
  Options options;

public:
  PatchRuleAssembly(Options opts);

  ~PatchRuleAssembly();

  bool changeOptions(Options opts) override;

  bool generate(const llvm::MCInst &inst, rword address, uint32_t instSize,
                const LLVMCPU &llvmcpu, std::vector<Patch> &patchList) override;

  bool earlyEnd(const LLVMCPU &llvmcpu, std::vector<Patch> &patchList) override;
};

} // namespace QBDI

#endif
