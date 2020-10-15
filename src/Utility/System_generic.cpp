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
#include "Platform.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/Process.h"

#include "Utility/LogSys.h"
#include "System.h"


namespace QBDI {

bool isRWXSupported() {
  return false;
}


llvm::sys::MemoryBlock allocateMappedMemory(size_t numBytes,
                                            const llvm::sys::MemoryBlock *const nearBlock,
                                            unsigned pFlags,
                                            std::error_code &ec) {
    // forward to llvm function
    return llvm::sys::Memory::allocateMappedMemory(numBytes, nearBlock, pFlags, ec);
}


void releaseMappedMemory(llvm::sys::MemoryBlock& block) {
    llvm::sys::Memory::releaseMappedMemory(block);
}


const std::string getHostCPUName() {
    const std::string& cpuname = llvm::sys::getHostCPUName();
    // set default ARM CPU
    if constexpr(is_arm)
        if (cpuname.empty() || cpuname == "generic")
            return std::string("cortex-a8");
    return cpuname;
}

}
