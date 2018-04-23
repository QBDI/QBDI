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
#ifndef PATCHRULES_X86_64_H
#define PATCHRULES_X86_64_H

#include <memory>
#include <vector>

#include "Patch/X86_64/PatchGenerator_X86_64.h"

namespace QBDI {

class PatchRule;

static const uint32_t MINIMAL_BLOCK_SIZE = 64;

RelocatableInst::SharedPtrVec getExecBlockPrologue();

RelocatableInst::SharedPtrVec getExecBlockEpilogue();

RelocatableInst::SharedPtrVec getTerminator(rword address, CPUMode cpuMode);

std::vector<std::shared_ptr<PatchRule>> getDefaultPatchRules();

}

#endif
