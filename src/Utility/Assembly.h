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

#include "llvm/ADT/APInt.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/Support/raw_ostream.h"

#include "Options.h"
#include "Utility/memory_ostream.h"

namespace llvm {
  class MCContext;
  class MCAsmBackend;
  class Target;
  class MCInstrInfo;
  class MCAsmInfo;
  class MCSubtargetInfo;
  class MCAssembler;
  class MCInstPrinter;
  class MCInst;
}

namespace QBDI {

class Assembly {
protected:

    const llvm::Target                       *target;
    llvm::MCInstrInfo                        &MCII;
    const llvm::MCRegisterInfo               &MRI;
    const llvm::MCAsmInfo                    &MAI;
    llvm::MCSubtargetInfo                    &MSTI;
    std::unique_ptr<llvm::MCAssembler>       assembler;
    std::unique_ptr<llvm::MCDisassembler>    disassembler;
    std::unique_ptr<llvm::MCInstPrinter>     asmPrinter;
    std::unique_ptr<llvm::raw_pwrite_stream> null_ostream;

    Options                                  options;

public:
    Assembly(llvm::MCContext &context, std::unique_ptr<llvm::MCAsmBackend> MAB, llvm::MCInstrInfo &MCII,
             const llvm::Target *target, llvm::MCSubtargetInfo &MSTI, Options options);

    ~Assembly();

    void writeInstruction(llvm::MCInst inst, memory_ostream* stream) const;

    llvm::MCDisassembler::DecodeStatus getInstruction(llvm::MCInst &inst, uint64_t &size,
                                            llvm::ArrayRef<uint8_t> bytes, uint64_t address) const;

    void printDisasm(const llvm::MCInst &inst, uint64_t address, llvm::raw_ostream &out = llvm::errs()) const;

    const char* getRegisterName(unsigned int id) const {return MRI.getName(id); }

    inline const llvm::MCInstrInfo& getMCII() const { return MCII; }

    inline const llvm::MCRegisterInfo& getMRI() const { return MRI; }

    Options getOptions() const {return options;}
    void setOptions(Options opts);
};

}

#endif // ASSEMBLY_H
