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
#ifndef PATCHRULES_H
#define PATCHRULES_H

#include <memory>
#include <vector>

#include "Options.h"
#include "State.h"

namespace QBDI {

class PatchRule;
class RelocatableInst;

std::vector<std::unique_ptr<RelocatableInst>> getExecBlockPrologue(Options opts);

std::vector<std::unique_ptr<RelocatableInst>> getExecBlockEpilogue(Options opts);

std::vector<std::unique_ptr<RelocatableInst>> getTerminator(rword address);

std::vector<PatchRule> getDefaultPatchRules();

}

#endif

