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
#ifndef SYSTEM_H
#define SYSTEM_H

#include <stddef.h>
#include <string>
#include <system_error>
#include <vector>

#include "llvm/Support/Memory.h"

namespace QBDI {
bool isRWXSupported();
bool isRWRXSupported();
llvm::sys::MemoryBlock
allocateMappedMemory(size_t NumBytes,
                     const llvm::sys::MemoryBlock *const NearBlock,
                     unsigned PFlags, std::error_code &EC);
void releaseMappedMemory(llvm::sys::MemoryBlock &block);
const std::string getHostCPUName();
const std::vector<std::string> getHostCPUFeatures();
bool isHostCPUFeaturePresent(const char *f);
} // namespace QBDI

#endif // SYSTEM_H
