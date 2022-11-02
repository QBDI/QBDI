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
#ifndef PATCH_MEMORYACCESS_ARM_H
#define PATCH_MEMORYACCESS_ARM_H

#include <memory>

#include "Patch/MemoryAccess.h"
#include "Patch/Patch.h"

namespace QBDI {
class RelocatableInst;
class LLVMCPU;

/* Generate the patch to retrive the address of the access from an instruction
 *
 * @param[in] patch          The current patch of the instruction
 * @param[in] writteneAccess If true, get the address of the written access
 *                           If false, get the address of the read access
 * @param[in] dest           The register to store the address
 *
 * Each instruction of InstInfo that does a memory access must have an
 * associated AddressGenFn
 */
std::vector<std::unique_ptr<RelocatableInst>>
generateAddressPatch(const Patch &patch, bool writeAccess, Reg dest);
} // namespace QBDI

#endif
