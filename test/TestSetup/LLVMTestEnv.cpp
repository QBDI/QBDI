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
#include <catch2/catch.hpp>

#include "llvm/ADT/Triple.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInstrInfo.h"
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

#include "Platform.h"

#include "TestSetup/LLVMTestEnv.h"

LLVMTestEnv::~LLVMTestEnv() = default;

LLVMTestEnv::LLVMTestEnv(std::string cpu, std::vector<std::string> mattrs) :
        cpu(cpu), mattrs(mattrs) {
    std::string error;
    std::string featuresStr;

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllDisassemblers();

    // Build features string
    if (mattrs.size()) {
        llvm::SubtargetFeatures features;
        for (unsigned i = 0; i != mattrs.size(); ++i) {
            features.AddFeature(mattrs[i]);
        }
        featuresStr = features.getString();
    }

#ifdef QBDI_PLATFORM_IOS
    // FIXME
    cpu = "swift";
#endif

    // lookup target
    tripleName = llvm::Triple::normalize(
        llvm::sys::getDefaultTargetTriple()
    );
    llvm::Triple process_triple(tripleName);
    processTarget = llvm::TargetRegistry::lookupTarget(tripleName, error);
    llvm::MCTargetOptions options;
    REQUIRE(nullptr != processTarget);
    // Allocate all LLVM classes
    MRI = std::unique_ptr<llvm::MCRegisterInfo>(
        processTarget->createMCRegInfo(tripleName)
    );
    REQUIRE(MRI != nullptr);
    MAI = std::unique_ptr<llvm::MCAsmInfo>(
        processTarget->createMCAsmInfo(*MRI, tripleName, options)
    );
    REQUIRE(MAI != nullptr);
    MOFI = std::make_unique<llvm::MCObjectFileInfo>();
    REQUIRE(MOFI != nullptr);
    MCTX = std::make_unique<llvm::MCContext>(MAI.get(), MRI.get(), MOFI.get());
    REQUIRE(MCTX != nullptr);
    MCII = std::unique_ptr<llvm::MCInstrInfo>(processTarget->createMCInstrInfo());
    REQUIRE(MCII != nullptr);
    MSTI = std::unique_ptr<llvm::MCSubtargetInfo>(
      processTarget->createMCSubtargetInfo(tripleName, cpu, featuresStr)
    );
    REQUIRE(MSTI != nullptr);
    auto MAB = std::unique_ptr<llvm::MCAsmBackend>(
        processTarget->createMCAsmBackend(*MSTI, *MRI, options)
    );
    MCE = std::unique_ptr<llvm::MCCodeEmitter>(
       processTarget->createMCCodeEmitter(*MCII, *MRI, *MCTX)
    );
    REQUIRE(MAB != nullptr);
    assembly = std::make_unique<QBDI::Assembly>(*MCTX, std::move(MAB), *MCII, *processTarget, *MSTI);
    REQUIRE(assembly != nullptr);
}
