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

#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/TargetRegistry.h"

#include "Utility/Assembly.h"
#include "Utility/LogSys.h"

#include "Platform.h"
#include "State.h"

namespace QBDI {

Assembly::Assembly(llvm::MCContext &MCTX, std::unique_ptr<llvm::MCAsmBackend> MAB, llvm::MCInstrInfo &MCII,
                   const llvm::Target &target, llvm::MCSubtargetInfo &MSTI)
    : MCII(MCII), MRI(*MCTX.getRegisterInfo()), MAI(*MCTX.getAsmInfo()), MSTI(MSTI) {

    unsigned int variant = 0;

    null_ostream = std::make_unique<llvm::raw_null_ostream>();

    disassembler = std::unique_ptr<llvm::MCDisassembler>(
        target.createMCDisassembler(MSTI, MCTX)
    );

    auto codeEmitter = std::unique_ptr<llvm::MCCodeEmitter>(
        target.createMCCodeEmitter(MCII, MRI, MCTX)
    );

    auto objectWriter = std::unique_ptr<llvm::MCObjectWriter>(
        MAB->createObjectWriter(*null_ostream)
    );

    assembler = std::make_unique<llvm::MCAssembler>(
        MCTX, std::move(MAB), std::move(codeEmitter), std::move(objectWriter)
    );

    // TODO: find better way to handle variant
    if constexpr(is_x86_64 or is_x86)
        variant = 1; // Force Intel
    else
        variant = MAI.getAssemblerDialect();

    asmPrinter = std::unique_ptr<llvm::MCInstPrinter>(
        target.createMCInstPrinter(MSTI.getTargetTriple(), variant, MAI, MCII, MRI)
    );
    asmPrinter->setPrintImmHex(true);
    asmPrinter->setPrintImmHex(llvm::HexStyle::C);
}

Assembly::~Assembly() = default;

llvm::MCDisassembler::DecodeStatus Assembly::getInstruction(llvm::MCInst &instr, uint64_t &size,
                                         llvm::ArrayRef< uint8_t > bytes, uint64_t address) const {
    return disassembler->getInstruction(instr, size, bytes, address, llvm::nulls());
}


void Assembly::writeInstruction(const llvm::MCInst inst, memory_ostream *stream) const {
    // if the instruction is a pseudo instruction, don't generate it
    if (MCII.get(inst.getOpcode()).isPseudo())
        return;

    // MCCodeEmitter needs a fixups array
    llvm::SmallVector<llvm::MCFixup,4> fixups;

    uint64_t pos = stream->current_pos();
    LogCallback(LogPriority::DEBUG, "Assembly::writeInstruction", [&] (FILE *log) -> void {
        std::string disass;
        llvm::raw_string_ostream disassOs(disass);
        uint64_t address = reinterpret_cast<uint64_t>(stream->get_ptr()) + pos;
        printDisasm(inst, address, disassOs);
        disassOs.flush();
        fprintf(log, "Assembling %s at 0x%" PRIx64, disass.c_str(), address);
    });
    assembler->getEmitter().encodeInstruction(inst, *stream, fixups, MSTI);
    uint64_t size = stream->current_pos() - pos;

    if(fixups.size() > 0) {
        llvm::MCValue target = llvm::MCValue();
        llvm::MCFixup fixup = fixups.pop_back_val();
        int64_t value;
        if(fixup.getValue()->evaluateAsAbsolute(value)) {
            assembler->getBackend().applyFixup(*assembler, fixup, target, llvm::MutableArrayRef<char>((char*) stream->get_ptr() + pos, size), (uint64_t) value, true, &MSTI);
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


void Assembly::printDisasm(const llvm::MCInst &inst, uint64_t address, llvm::raw_ostream &out) const {
    llvm::StringRef   unusedAnnotations;
    asmPrinter->printInst(&inst, address, unusedAnnotations, MSTI, out);
}

}
