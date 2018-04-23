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
#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include <memory>

#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/TargetRegistry.h"

#include "Engine/LLVMCPU.h"
#include "ExecBlock/Context.h"
#include "Utility/memory_ostream.h"

namespace QBDI {

class Assembly {
protected:

    LLVMCPU*                                 llvmCPU;
    std::unique_ptr<llvm::MCAssembler>       assembler;
    std::unique_ptr<llvm::MCDisassembler>    disassembler;
    std::unique_ptr<llvm::MCInstPrinter>     asmPrinter;
    std::unique_ptr<llvm::MCObjectWriter>    objectWriter;
    std::unique_ptr<llvm::raw_pwrite_stream> null_ostream;

public:
    Assembly(LLVMCPU* llvmCPU);

    void writeInstruction(llvm::MCInst inst, memory_ostream* stream) const;

    llvm::MCDisassembler::DecodeStatus getInstruction(llvm::MCInst &inst, uint64_t &size,
                           llvm::ArrayRef<uint8_t> bytes, uint64_t address) const;

    void printDisasm(const llvm::MCInst &inst, llvm::raw_ostream &out = llvm::errs()) const;

    const char* getRegisterName(unsigned int id) const {return llvmCPU->getMRI()->getName(id); }
};

}

#endif // ASSEMBLY_H
