/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2026 Quarkslab
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
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <iostream>

#include "TestSetup/InMemoryAssembler.h"

#include "QBDI/Platform.h"
#include "Engine/LLVMCPU.h"
#include "Utility/LogSys.h"
#include "Utility/System.h"

#include "MCTargetDesc/AArch64MCTargetDesc.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Object/ELFObjectFile.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/TargetParser/Triple.h"

void InMemoryObject::perform_reloc(llvm::object::ObjectFile *object,
                                   const QBDI::LLVMCPUs &llvmcpus) {

  for (auto sit = object->sections().begin(); sit != object->sections().end();
       ++sit) {
    auto relocatedSectionExpect = sit->getRelocatedSection();
    if (!relocatedSectionExpect)
      continue;

    auto relocatedSection = relocatedSectionExpect.get();
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
        continue;
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

      uint64_t targetAddr = reinterpret_cast<uint64_t>(code.data()) + address;

      uint32_t offset = relocIt->getOffset() - relocatedSection->getAddress();
      QBDI_REQUIRE_ABORT(relocatedSectionPtr == code.data(),
                         "Wrong buffer pointer");

      switch (relocIt->getType()) {
        case llvm::ELF::R_AARCH64_ADR_PREL_PG_HI21: {
          QBDI_REQUIRE_ABORT(offset + 4 <= relocatedSection->getSize(),
                             "Symbol instruction out of the target section");
          uint8_t *instAddr = relocatedSectionPtr + offset;
          uint64_t instrAddr = reinterpret_cast<uint64_t>(instAddr);

          patchInstructionOperand(
              llvmcpus.getCPU(QBDI::CPUMode::AARCH64), instAddr, 4,
              [&](llvm::MCInst &inst) {
                QBDI_REQUIRE_ABORT(inst.getOpcode() == llvm::AArch64::ADRP,
                                   "Unexpected opcode {}", inst.getOpcode());
                QBDI_REQUIRE_ABORT(inst.getNumOperands() == 2,
                                   "Unexpected operand number");
                QBDI_REQUIRE_ABORT(inst.getOperand(1).isImm(),
                                   "Unexpected operand type");

                int64_t pageDelta =
                    (static_cast<int64_t>(targetAddr & ~0xFFFULL) -
                     static_cast<int64_t>(instrAddr & ~0xFFFULL)) >>
                    12;
                inst.getOperand(1).setImm(pageDelta);
              });

          QBDI_DEBUG("Relocated instruction 0x{:x} : 0x{:x}", offset,
                     *reinterpret_cast<uint32_t *>(instAddr));
          break;
        }
        case llvm::ELF::R_AARCH64_ADD_ABS_LO12_NC: {
          QBDI_REQUIRE_ABORT(offset + 4 <= relocatedSection->getSize(),
                             "Symbol instruction out of the target section");
          uint8_t *instAddr = relocatedSectionPtr + offset;

          patchInstructionOperand(
              llvmcpus.getCPU(QBDI::CPUMode::AARCH64), instAddr, 4,
              [&](llvm::MCInst &inst) {
                QBDI_REQUIRE_ABORT(inst.getOpcode() == llvm::AArch64::ADDXri,
                                   "Unexpected opcode {} (only plain 'add "
                                   "xd, xn, :lo12:label' is supported)",
                                   inst.getOpcode());
                QBDI_REQUIRE_ABORT(inst.getNumOperands() == 4,
                                   "Unexpected operand number");
                QBDI_REQUIRE_ABORT(inst.getOperand(2).isImm(),
                                   "Unexpected operand type");

                inst.getOperand(2).setImm(targetAddr & 0xFFF);
              });

          QBDI_DEBUG("Relocated instruction 0x{:x} : 0x{:x}", offset,
                     *reinterpret_cast<uint32_t *>(instAddr));
          break;
        }
        case llvm::ELF::R_AARCH64_CALL26:
        case llvm::ELF::R_AARCH64_JUMP26: {
          QBDI_REQUIRE_ABORT(offset + 4 <= relocatedSection->getSize(),
                             "Symbol instruction out of the target section");
          uint8_t *instAddr = relocatedSectionPtr + offset;
          uint64_t instrAddr = reinterpret_cast<uint64_t>(instAddr);
          bool isCall = relocIt->getType() == llvm::ELF::R_AARCH64_CALL26;

          patchInstructionOperand(
              llvmcpus.getCPU(QBDI::CPUMode::AARCH64), instAddr, 4,
              [&](llvm::MCInst &inst) {
                unsigned expectedOpcode =
                    isCall ? llvm::AArch64::BL : llvm::AArch64::B;
                QBDI_REQUIRE_ABORT(inst.getOpcode() == expectedOpcode,
                                   "Unexpected opcode {} (expected {})",
                                   inst.getOpcode(), expectedOpcode);
                QBDI_REQUIRE_ABORT(inst.getNumOperands() == 1,
                                   "Unexpected operand number");
                QBDI_REQUIRE_ABORT(inst.getOperand(0).isImm(),
                                   "Unexpected operand type");

                int64_t byteDelta = static_cast<int64_t>(targetAddr) -
                                    static_cast<int64_t>(instrAddr);
                QBDI_REQUIRE_ABORT(byteDelta % 4 == 0,
                                   "Unaligned branch target 0x{:x}",
                                   targetAddr);
                inst.getOperand(0).setImm(byteDelta / 4);
              });

          QBDI_DEBUG("Relocated instruction 0x{:x} : 0x{:x}", offset,
                     *reinterpret_cast<uint32_t *>(instAddr));
          break;
        }
        case llvm::ELF::R_AARCH64_ABS64: {
          QBDI_REQUIRE_ABORT(offset + 8 <= relocatedSection->getSize(),
                             "Symbol data out of the target section");
          uint64_t *dataAddr =
              reinterpret_cast<uint64_t *>(relocatedSectionPtr + offset);
          *dataAddr = targetAddr;

          QBDI_DEBUG("Relocated value 0x{:x} : 0x{:x}", offset, *dataAddr);
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
