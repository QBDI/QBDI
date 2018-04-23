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
#include "Utility/Assembly.h"
#include "Utility/LogSys.h"
#include "Platform.h"

#include "ExecBlock/ExecBlock.h"

namespace QBDI {

Assembly::Assembly(LLVMCPU* llvmCPU) : llvmCPU(llvmCPU) {
    unsigned int variant = 0;

    null_ostream = std::unique_ptr<llvm::raw_pwrite_stream>(
        new llvm::raw_null_ostream()
    );

    objectWriter = std::unique_ptr<llvm::MCObjectWriter>(
        llvmCPU->getMAB()->createObjectWriter(*null_ostream)
    );

    disassembler = std::unique_ptr<llvm::MCDisassembler>(
        llvmCPU->getTarget()->createMCDisassembler(*llvmCPU->getMSTI(), *llvmCPU->getMCTX())
    );

    assembler = std::unique_ptr<llvm::MCAssembler>(
        new llvm::MCAssembler(*llvmCPU->getMCTX(), *llvmCPU->getMAB(), *llvmCPU->getMCE(), *objectWriter)
    );

    // TODO: find better way to handle variant
    #if defined(QBDI_ARCH_X86) || defined(QBDI_ARCH_X86_64)
    variant = 1; // Force Intel
    #else
    variant = llvmCPU->getMAI()->getAssemblerDialect();
    #endif // __x86_64__

    asmPrinter = std::unique_ptr<llvm::MCInstPrinter>(
        llvmCPU->getTarget()->createMCInstPrinter(llvmCPU->getMSTI()->getTargetTriple(), variant, *llvmCPU->getMAI(), *llvmCPU->getMII(), *llvmCPU->getMRI())
    );
    asmPrinter->setPrintImmHex(true);
    asmPrinter->setPrintImmHex(llvm::HexStyle::C);
}


llvm::MCDisassembler::DecodeStatus Assembly::getInstruction(llvm::MCInst &instr, uint64_t &size, 
                                         llvm::ArrayRef< uint8_t > bytes, uint64_t address) const {
    return disassembler->getInstruction(instr, size, bytes, address, llvm::nulls(), llvm::nulls());
}


void Assembly::writeInstruction(const llvm::MCInst inst, memory_ostream *stream) const {
    // MCCodeEmitter needs a fixups array
    llvm::SmallVector<llvm::MCFixup,4> fixups;

    uint64_t pos = stream->current_pos();
    LogCallback(LogPriority::DEBUG, "Assembly::writeInstruction", [&] (FILE *log) -> void {
        std::string disass;
        llvm::raw_string_ostream disassOs(disass);
        printDisasm(inst, disassOs);
        disassOs.flush();
        fprintf(log, "Assembling %s at 0x%" PRIRWORD, disass.c_str(), (rword) stream->get_ptr() + (rword) pos);
    });
    llvmCPU->getMCE()->encodeInstruction(inst, *stream, fixups, *llvmCPU->getMSTI());
    uint64_t size = stream->current_pos() - pos;

    if(fixups.size() > 0) {
        llvm::MCValue target = llvm::MCValue();
        llvm::MCFixup fixup = fixups.pop_back_val();
        int64_t value;
        if(fixup.getValue()->evaluateAsAbsolute(value)) {
            llvmCPU->getMAB()->applyFixup(*assembler, fixup, target, llvm::MutableArrayRef<char>((char*) stream->get_ptr() + pos, size), (uint64_t) value, true);
        }
        else {
            LogWarning("Assembly::writeInstruction", "Could not evalutate fixup, might crash!");
        }
    }

    LogCallback(LogPriority::DEBUG, "Assembly::writeInstruction", [&] (FILE *log) -> void {
        fprintf(log, "Assembly result at 0x%" PRIRWORD " is:", (rword) stream->get_ptr() + (rword) pos);
        uint8_t* ptr = (uint8_t*) stream->get_ptr();
        for(uint32_t i = 0; i < size; i++) {
            fprintf(log, " %02" PRIx8, ptr[pos + i]);
        }
    });
}


void Assembly::printDisasm(const llvm::MCInst &inst, llvm::raw_ostream &out) const {
    llvm::StringRef   unusedAnnotations;
    asmPrinter->printInst(&inst, out, unusedAnnotations, *llvmCPU->getMSTI());
}

}
