/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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
#include <fstream>
#include <iostream>

#include "TestSetup/InMemoryAssembler.h"

#include "QBDI/Platform.h"
#include "Engine/LLVMCPU.h"
#include "Utility/LogSys.h"
#include "Utility/System.h"
#include "Utility/memory_ostream.h"

#include "Target/ARM/ARMSubtarget.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/ADT/Twine.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Object/ELFObjectFile.h"
#include "llvm/Object/ObjectFile.h"

void InMemoryObject::perform_reloc(llvm::object::ObjectFile *object,
                                   const QBDI::LLVMCPUs &llvmcpus) {

  for (auto sit = object->sections().begin(); sit != object->sections().end();
       ++sit) {
    if (!sit->getRelocatedSection())
      continue;

    auto relocatedSection = *sit->getRelocatedSection();
    if (relocatedSection == object->sections().end())
      continue;
    // only relocated the text section, our bytes code
    if (not relocatedSection->isText()) {
      QBDI_ABORT("Found unexpected relocation sections for {}",
                 relocatedSection->getName()->str());
      continue;
    }

    uint8_t *relocatedSectionPtr =
        static_cast<uint8_t *>(objectBlock.base()) +
        llvm::object::ELFSectionRef(*relocatedSection).getOffset();

    for (auto relocIt = sit->relocation_begin();
         relocIt != sit->relocation_end(); ++relocIt) {
      if (relocIt->getOffset() < relocatedSection->getAddress() or
          relocatedSection->getAddress() + relocatedSection->getSize() <=
              relocIt->getOffset()) {
        QBDI_ABORT("Symbols not in the target sections");
      }

      auto sym = relocIt->getSymbol();
      if (sym == object->symbol_end()) {
        QBDI_ABORT("Relocation without symbol");
      }

      if (!sym->getType() or !sym->getName() or !sym->getAddress() or
          !sym->getFlags()) {
        QBDI_ABORT("Error when parsing symbol");
      }

      QBDI_REQUIRE_ABORT(
          (*sym->getFlags() &
           llvm::object::BasicSymbolRef::Flags::SF_Undefined) == 0,
          "Relocation to the undefined symbol {}", sym->getName()->str());

      int64_t address = *sym->getAddress();

      if (auto AddendOrErr =
              llvm::object::ELFRelocationRef(*relocIt).getAddend())
        address += *AddendOrErr;
      else
        llvm::consumeError(AddendOrErr.takeError());

      switch (relocIt->getType()) {
        case llvm::ELF::R_ARM_CALL: {
          QBDI_REQUIRE_ABORT(*sym->getFlags() &
                                 llvm::object::BasicSymbolRef::Flags::SF_Thumb,
                             "The target symbol isn't a thumb method");
          const QBDI::LLVMCPU &llvmcpu = llvmcpus.getCPU(QBDI::CPUMode::ARM);

          uint32_t offset =
              relocIt->getOffset() - relocatedSection->getAddress();
          QBDI_REQUIRE_ABORT(offset + 4 <= relocatedSection->getSize(),
                             "Symbol instruction out of the target section");
          QBDI_REQUIRE_ABORT(relocatedSectionPtr == code.data(),
                             "Wrong buffer pointer");
          uint8_t *instAddr = relocatedSectionPtr + offset;

          llvm::MCInst inst;
          uint64_t instSize;
          bool dstatus = llvmcpu.getInstruction(
              inst, instSize, llvm::ArrayRef<uint8_t>(instAddr, 4),
              reinterpret_cast<uint64_t>(instAddr));
          QBDI_REQUIRE_ABORT(dstatus, "Fail parse the instruction");

          QBDI_REQUIRE_ABORT(inst.getOpcode() == llvm::ARM::BLXi,
                             "Unexpected OPcode");
          QBDI_REQUIRE_ABORT(inst.getNumOperands() == 1,
                             "Unexpected operand number");
          QBDI_REQUIRE_ABORT(inst.getOperand(0).isImm(),
                             "Unexpected operand type");

          inst.getOperand(0).setImm(inst.getOperand(0).getImm() + address -
                                    relocIt->getOffset());

          llvm::sys::MemoryBlock mblock(instAddr, 4);
          QBDI::memory_ostream codeStream(mblock);
          llvmcpu.writeInstruction(inst, codeStream);

          QBDI_DEBUG("Relocated instruction 0x{:x} : 0x{:x}", offset,
                     *reinterpret_cast<uint32_t *>(instAddr));
          break;
        }
        case llvm::ELF::R_ARM_THM_CALL: {
          QBDI_REQUIRE_ABORT((*sym->getFlags() &
                              llvm::object::BasicSymbolRef::Flags::SF_Thumb) ==
                                 0,
                             "The target symbol isn't an arm method");
          const QBDI::LLVMCPU &llvmcpu = llvmcpus.getCPU(QBDI::CPUMode::Thumb);

          uint32_t offset =
              relocIt->getOffset() - relocatedSection->getAddress();
          QBDI_REQUIRE_ABORT(offset + 4 <= relocatedSection->getSize(),
                             "Symbol instruction out of the target section");
          QBDI_REQUIRE_ABORT(relocatedSectionPtr == code.data(),
                             "Wrong buffer pointer");
          uint8_t *instAddr = relocatedSectionPtr + offset;

          llvm::MCInst inst;
          uint64_t instSize;
          bool dstatus = llvmcpu.getInstruction(
              inst, instSize, llvm::ArrayRef<uint8_t>(instAddr, 4),
              reinterpret_cast<uint64_t>(instAddr));
          QBDI_REQUIRE_ABORT(dstatus, "Fail parse the instruction");

          QBDI_REQUIRE_ABORT(inst.getOpcode() == llvm::ARM::tBLXi,
                             "Unexpected OPcode");
          QBDI_REQUIRE_ABORT(inst.getNumOperands() >= 3,
                             "Unexpected operand number");
          QBDI_REQUIRE_ABORT(inst.getOperand(2).isImm(),
                             "Unexpected operand type");

          uint32_t value =
              inst.getOperand(2).getImm() + address - relocIt->getOffset();
          if (value % 4 != 0) {
            value += 2;
          }
          QBDI_REQUIRE_ABORT((value & 1) == 0, "Invalid Address 0x{:x}", value);
          inst.getOperand(2).setImm(value);

          llvm::sys::MemoryBlock mblock(instAddr, 4);
          QBDI::memory_ostream codeStream(mblock);
          llvmcpu.writeInstruction(inst, codeStream);

          QBDI_DEBUG("Relocated instruction 0x{:x} : 0x{:x}", offset,
                     *reinterpret_cast<uint32_t *>(instAddr));
          break;
        }
        case llvm::ELF::R_ARM_ABS32: {
          uint32_t offset =
              relocIt->getOffset() - relocatedSection->getAddress();
          QBDI_REQUIRE_ABORT(offset + 4 <= relocatedSection->getSize(),
                             "Symbol instruction out of the target section");
          QBDI_REQUIRE_ABORT(relocatedSectionPtr == code.data(),
                             "Wrong buffer pointer");
          uint32_t *instAddr =
              reinterpret_cast<uint32_t *>(relocatedSectionPtr + offset);

          const bool symbolIsThumb =
              *sym->getFlags() & llvm::object::BasicSymbolRef::Flags::SF_Thumb;

          int32_t finalAddr =
              address + reinterpret_cast<QBDI::rword>(code.data());
          if (symbolIsThumb) {
            finalAddr |= 1;
          }
          *instAddr += finalAddr;

          QBDI_DEBUG("Relocated value 0x{:x} : 0x{:x}", offset, *instAddr);
          break;
        }
        default: {
          llvm::SmallVector<char> relocName;
          relocIt->getTypeName(relocName);
          relocName.emplace_back('\0');
          QBDI_ABORT("Cannot handle relocation type {} to {}",
                     relocName.begin(), sym->getName()->str());
        }
      }
    }
  }
}
