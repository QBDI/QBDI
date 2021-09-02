/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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
#ifndef INSTANALYSISPRIVE_H
#define INSTANALYSISPRIVE_H

#include <memory>

#include "QBDI/InstAnalysis.h"

namespace llvm {
class MCInstrDesc;
class MCInstrInfo;
class MCRegisterInfo;
} // namespace llvm

namespace QBDI {

class InstMetadata;
class LLVMCPU;

struct InstAnalysisDestructor {
  void operator()(InstAnalysis *ptr) const;
};

using InstAnalysisPtr = std::unique_ptr<InstAnalysis, InstAnalysisDestructor>;

const InstAnalysis *analyzeInstMetadata(const InstMetadata &instMetadata,
                                        AnalysisType type,
                                        const LLVMCPU &llvmcpu);
namespace InstructionAnalysis {

void analyseRegister(OperandAnalysis &opa, unsigned int regNo,
                     const llvm::MCRegisterInfo &MRI);
void tryMergeCurrentRegister(InstAnalysis *instAnalysis);

// Arch specific
// =============

void analyseCondition(InstAnalysis *instAnalysis, const llvm::MCInst &inst,
                      const llvm::MCInstrDesc &desc);
bool isFlagOperand(unsigned opcode, unsigned opNum, unsigned operandType);
unsigned getBias(const llvm::MCInstrDesc &desc);

// Missing operand in LLVM Description
unsigned getAdditionnalOperandNumber(const llvm::MCInst &inst,
                                     const llvm::MCInstrDesc &desc);
void getAdditionnalOperand(InstAnalysis *instAnalysis, const llvm::MCInst &inst,
                           const llvm::MCInstrDesc &desc,
                           const llvm::MCRegisterInfo &MRI);

} // namespace InstructionAnalysis
} // namespace QBDI

#endif
