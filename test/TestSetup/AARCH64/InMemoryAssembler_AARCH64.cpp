/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2023 Quarkslab
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
#include "Utility/LogSys.h"
#include "Utility/System.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Object/ELFObjectFile.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/TargetParser/Triple.h"

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

    // uint8_t* relocatedSectionPtr = static_cast<uint8_t*>(objectBlock.base())
    // +
    //   llvm::object::ELFSectionRef(*relocatedSection).getOffset();

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

      // int64_t address = *sym->getAddress();

      // if (auto AddendOrErr =
      // llvm::object::ELFRelocationRef(*relocIt).getAddend())
      //   address += *AddendOrErr;
      // else
      //   llvm::consumeError(AddendOrErr.takeError());

      switch (relocIt->getType()) {
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
