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
#ifndef PATCH_MEMORYACCESS_H
#define PATCH_MEMORYACCESS_H

#include "Patch/InstrRule.h"

#include "Callback.h"

namespace QBDI {

class ExecBlock;

void analyseMemoryAccess(const ExecBlock& currentExecBlock, uint16_t instID, bool afterInst, std::vector<MemoryAccess>& dest);


std::vector<std::unique_ptr<InstrRule>> getInstrRuleMemAccessRead();

std::vector<std::unique_ptr<InstrRule>> getInstrRuleMemAccessWrite();

}

#endif
