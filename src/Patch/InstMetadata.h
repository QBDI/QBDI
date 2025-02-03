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
#ifndef INSTMETADATA_H
#define INSTMETADATA_H

#include "llvm/MC/MCInst.h"

#include "Utility/InstAnalysis_prive.h"

#include "QBDI/State.h"

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
#include "Patch/X86_64/InstMetadata_X86_64.h"
#elif defined(QBDI_ARCH_ARM)
#include "Patch/ARM/InstMetadata_ARM.h"
#elif defined(QBDI_ARCH_AARCH64)
#include "Patch/AARCH64/InstMetadata_AARCH64.h"
#endif

namespace QBDI {

class InstMetadata {
public:
  llvm::MCInst inst;
  rword address;
  uint32_t instSize;
  uint32_t patchSize;
  CPUMode cpuMode;
  bool modifyPC;
  uint8_t execblockFlags;
  mutable InstAnalysisPtr analysis;
  InstMetadataArch archMetadata;
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
  // prefix for X86_64 instruction like ``lock``
  std::vector<llvm::MCInst> prefix;
#endif

  InstMetadata(const llvm::MCInst &inst, rword address, uint32_t instSize,
               uint32_t patchSize, CPUMode cpuMode, bool modifyPC,
               uint8_t execblockFlags, InstAnalysisPtr analysis)
      : inst(inst), address(address), instSize(instSize), patchSize(patchSize),
        cpuMode(cpuMode), modifyPC(modifyPC), execblockFlags(execblockFlags),
        analysis(std::move(analysis)) {}

  InstMetadata(const llvm::MCInst &inst, rword address, uint32_t instSize,
               CPUMode cpuMode, uint8_t execblockFlags)
      : inst(inst), address(address), instSize(instSize), patchSize(0),
        cpuMode(cpuMode), modifyPC(false), execblockFlags(execblockFlags),
        analysis(nullptr) {}

  inline rword endAddress() const { return address + instSize; }

  inline InstMetadata lightCopy() const {
    InstMetadata cpy{inst,    address,  instSize,       patchSize,
                     cpuMode, modifyPC, execblockFlags, nullptr};
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
    cpy.prefix = prefix;
#endif
    cpy.archMetadata = archMetadata;
    return cpy;
  }
};

} // namespace QBDI

#endif // INSTMETADATA_H
