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

#include "Utility/Assembly.h"


class LLVMTestEnv : public ::testing::Test {
protected:

    std::unique_ptr<llvm::MCAsmInfo>         MAI;
    std::unique_ptr<llvm::MCCodeEmitter>     MCE;
    std::unique_ptr<llvm::MCContext>         MCTX;
    std::unique_ptr<llvm::MCInstrInfo>       MCII;
    std::unique_ptr<llvm::MCObjectFileInfo>  MOFI;
    std::unique_ptr<llvm::MCRegisterInfo>    MRI;
    std::unique_ptr<llvm::MCSubtargetInfo>   MSTI;
    std::unique_ptr<QBDI::Assembly>          assembly;
    const llvm::Target*                      processTarget;
    std::string                              tripleName;
    std::string                              cpu;
    std::vector<std::string>                 mattrs;

    virtual void SetUp();

public:

    LLVMTestEnv(std::string cpu = "", std::vector<std::string> mattrs = {}): 
        cpu(cpu), mattrs(mattrs) {}
};

#endif
