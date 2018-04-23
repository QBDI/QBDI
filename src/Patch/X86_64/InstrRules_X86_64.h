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
#ifndef INSTRRULES_X86_64_H
#define INSTRRULES_X86_64_H

#include <memory>
#include <vector>

#include "llvm/MC/MCInst.h"

#include "Callback.h"
#include "Patch/PatchUtils.h"
#include "Patch/X86_64/PatchGenerator_X86_64.h"

namespace QBDI {

class InstrRule;

RelocatableInst::SharedPtrVec getBreakToHost(Reg temp, CPUMode cpuMode);

std::vector<std::shared_ptr<InstrRule>> getMemAccessInstrRules();

}

#endif
