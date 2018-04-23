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
#ifndef LLVMTESTENV_H
#define LLVMTESTENV_H

#include <gtest/gtest.h>

#include "llvm/ADT/Triple.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"

#include "Engine/Engine.h"
#include "Engine/LLVMCPU.h"
#include "Utility/Assembly.h"


class LLVMTestEnv : public ::testing::Test {
protected:

    std::string                              cpu;
    std::vector<std::string>                 mattrs;
    QBDI::LLVMCPU*                           llvmCPU[QBDI::CPUMode::COUNT];
    QBDI::Assembly*                          assembly[QBDI::CPUMode::COUNT];

    virtual void SetUp();

public:

    LLVMTestEnv(std::string cpu = "", std::vector<std::string> mattrs = {}): 
        cpu(cpu), mattrs(mattrs) {}

    ~LLVMTestEnv() {
        unsigned i = 0;

        for(i = 0; i < QBDI::CPUMode::COUNT; i++) {
            delete assembly[i];
            delete llvmCPU[i];
        }
    }
};

#endif
