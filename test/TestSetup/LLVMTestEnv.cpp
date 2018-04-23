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
#include "TestSetup/LLVMTestEnv.h"
#include "Platform.h"

void LLVMTestEnv::SetUp() {
    unsigned i = 0;

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllDisassemblers();

    #ifdef QBDI_OS_IOS
    // FIXME
    cpu = "swift";
    #endif

    #if defined(QBDI_ARCH_ARM)
    llvmCPU[QBDI::CPUMode::ARM]     = new QBDI::LLVMCPU(cpu, "arm", mattrs);
    llvmCPU[QBDI::CPUMode::Thumb]   = new QBDI::LLVMCPU(cpu, "thumb", mattrs);
    #elif defined(QBDI_ARCH_X86_64)
    llvmCPU[QBDI::CPUMode::X86_64] = new QBDI::LLVMCPU(cpu, "", mattrs);
    #endif
    // Allocate corresponding Assembly
    for(i = 0; i < QBDI::CPUMode::COUNT; i++) {
        assembly[i] = new QBDI::Assembly(llvmCPU[i]);
    }
}
