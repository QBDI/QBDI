/*
 * This file is part of QBDI.
 *
 * Copyright 2018 Quarkslab
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

#include "Engine/LLVMCPU.h"
#include "Utility/LogSys.h"
#include "Utility/System.h"

namespace QBDI {


LLVMCPU::LLVMCPU(const std::string& _cpu, const std::string& _arch, const std::vector<std::string>& _mattrs)
                :  cpu(_cpu), arch(_arch), mattrs(_mattrs) {
    std::string          error;
    std::string          featuresStr;

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllDisassemblers();

    // Handle CPU
    if (cpu.empty()) {
        cpu = getHostCPUName();
        // If API is broken on ARM, we are facing big problems...
        #if defined(QBDI_ARCH_ARM)
        Require("LLVMCPU::LLVMCPU", !cpu.empty() && cpu != "generic");
        #endif
    }
    // Handle CPU
    if (!arch.empty()) {
    }
    // Build features string
    if (mattrs.empty()) {
        mattrs = getHostCPUFeatures();
    }
    if (!mattrs.empty()) {
        llvm::SubtargetFeatures features;
        for (unsigned i = 0; i != mattrs.size(); ++i) {
            features.AddFeature(mattrs[i]);
        }
        featuresStr = features.getString();
    }

    // lookup target
    llvm::Triple processTriple(llvm::Triple::normalize(llvm::sys::getDefaultTargetTriple()));
    target = llvm::TargetRegistry::lookupTarget(arch, processTriple, error);
    tripleName = processTriple.getTriple();
    LogDebug("LLVMCPU::LLVMCPU", "Initialized LLVM for target %s", tripleName.c_str());

    // Allocate all LLVM classes
    MOFI = std::unique_ptr<llvm::MCObjectFileInfo>(
        new llvm::MCObjectFileInfo()
    );
    MRI = std::unique_ptr<llvm::MCRegisterInfo>(
        target->createMCRegInfo(tripleName)
    );
    MAI = std::unique_ptr<llvm::MCAsmInfo>(
        target->createMCAsmInfo(*MRI, tripleName)
    );
    MCTX = std::unique_ptr<llvm::MCContext>(
        new llvm::MCContext(MAI.get(), MRI.get(), MOFI.get())
    );

    MOFI->InitMCObjectFileInfo(processTriple, llvm::Reloc::Static, llvm::CodeModel::Default, *MCTX);

    MII = std::unique_ptr<llvm::MCInstrInfo>(
        target->createMCInstrInfo()
    );
    MSTI = std::unique_ptr<llvm::MCSubtargetInfo>(
      target->createMCSubtargetInfo(tripleName, cpu, featuresStr)
    );
    LogDebug("LLVMCPU::LLVMCPU", "Initialized LLVM subtarget with cpu %s and features %s", cpu.c_str(), featuresStr.c_str());
    MAB = std::unique_ptr<llvm::MCAsmBackend>(
        target->createMCAsmBackend(*MRI, tripleName, cpu, llvm::MCTargetOptions())
    );
    MCE = std::unique_ptr<llvm::MCCodeEmitter>(
        target->createMCCodeEmitter(*MII, *MRI, *MCTX)
    );
}

}
