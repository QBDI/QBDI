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
#include "Engine/LLVMCPU.h"
#include "Utility/LogSys.h"
#include "Utility/System.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCParser/MCAsmParser.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Object/ELFObjectFile.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/TargetParser/SubtargetFeature.h"
#include "llvm/TargetParser/Triple.h"

void writeResult(const char *source,
                 const llvm::SmallVector<char, 1024> &objectVector) {

  if (getenv("DUMP_TEST_ASM") != nullptr) {
    std::string filename_source(getenv("DUMP_TEST_ASM"));
    std::string filename_code(getenv("DUMP_TEST_ASM"));

    filename_source += ".txt";
    filename_code += ".bin";

    std::ofstream source_file(filename_source, std::ios_base::trunc);
    source_file << source << '\n';
    source_file.close();

    std::ofstream code_file(filename_code,
                            std::ios_base::trunc | std::ios_base::binary);
    code_file.write(objectVector.data(), objectVector.size());
    code_file.close();
  }
}

InMemoryObject::InMemoryObject(const char *source, const char *cpu,
                               const char *arch,
                               const std::vector<std::string> mattrs) {
  std::unique_ptr<llvm::MCAsmInfo> MAI;
  std::unique_ptr<llvm::MCContext> MCTX;
  std::unique_ptr<llvm::MCInstrInfo> MCII;
  std::unique_ptr<llvm::MCObjectFileInfo> MOFI;
  std::unique_ptr<llvm::MCRegisterInfo> MRI;
  std::unique_ptr<llvm::MCSubtargetInfo> MSTI;
  std::unique_ptr<llvm::MCStreamer> mcStr;
  const llvm::Target *processTarget;
  llvm::SourceMgr SrcMgr;
  std::string tripleName;
  std::error_code ec;
  llvm::SmallVector<char, 1024> objectVector;

  llvm::SubtargetFeatures features;
  for (const auto &e : mattrs) {
    features.AddFeature(e);
  }
  std::string featuresStr = features.getString();

  std::string error;
  // lookup target
  llvm::Triple processTriple{llvm::sys::getDefaultTargetTriple()};
  // make sure we don't accidentally use a weak object file format
  processTriple.setObjectFormat(llvm::Triple::ObjectFormatType::ELF);
  processTriple.setOS(llvm::Triple::OSType::Linux);
  tripleName = llvm::Triple::normalize(processTriple.str());
  processTarget =
      llvm::TargetRegistry::lookupTarget(arch, processTriple, error);

  llvm::MCTargetOptions options;
  // Allocate all LLVM classes
  MRI = std::unique_ptr<llvm::MCRegisterInfo>(
      processTarget->createMCRegInfo(tripleName));
  MAI = std::unique_ptr<llvm::MCAsmInfo>(
      processTarget->createMCAsmInfo(*MRI, tripleName, options));
  MCII = std::unique_ptr<llvm::MCInstrInfo>(processTarget->createMCInstrInfo());
  MSTI = std::unique_ptr<llvm::MCSubtargetInfo>(
      processTarget->createMCSubtargetInfo(tripleName, cpu, featuresStr));
  MCTX = std::make_unique<llvm::MCContext>(processTriple, MAI.get(), MRI.get(),
                                           MSTI.get(), &SrcMgr);
  MOFI = std::unique_ptr<llvm::MCObjectFileInfo>(
      processTarget->createMCObjectFileInfo(*MCTX, false));
  MCTX->setObjectFileInfo(MOFI.get());
  auto MAB = std::unique_ptr<llvm::MCAsmBackend>(
      processTarget->createMCAsmBackend(*MSTI, *MRI, options));
  auto MCE = std::unique_ptr<llvm::MCCodeEmitter>(
      processTarget->createMCCodeEmitter(*MCII, *MCTX));

  // Wrap output object into raw_ostream
  // raw_pwrite_string_ostream rpsos(objectStr);
  llvm::raw_svector_ostream rsos(objectVector);
  auto objectWriter =
      std::unique_ptr<llvm::MCObjectWriter>(MAB->createObjectWriter(rsos));

  // Add input to the SourceMgr
  SrcMgr.AddNewSourceBuffer(
      llvm::MemoryBuffer::getMemBuffer(llvm::StringRef(source)), llvm::SMLoc());
  // Set MCStreamer as a MCObjectStreamer
  mcStr.reset(processTarget->createMCObjectStreamer(
      MSTI->getTargetTriple(), *MCTX, std::move(MAB), std::move(objectWriter),
      std::move(MCE), *MSTI, true, false, false));
  // Create the assembly parsers with --no-deprecated-warn
  llvm::MCTargetOptions parserOpts;
  parserOpts.MCNoDeprecatedWarn = true;
  parserOpts.MCNoWarn = (getenv("TEST_WARN") == nullptr);
  llvm::MCAsmParser *parser =
      llvm::createMCAsmParser(SrcMgr, *MCTX, *mcStr, *MAI);
  llvm::MCTargetAsmParser *tap =
      processTarget->createMCAsmParser(*MSTI, *parser, *MCII, parserOpts);
  parser->setTargetParser(*tap);
  // Finally do something we care about
  mcStr->initSections(false, *MSTI);
  REQUIRE_FALSE(parser->Run(true));
  delete parser;
  delete tap;
  // Copy object into new page and make it executable
  unsigned mFlags = PF::MF_READ | PF::MF_WRITE;
  if constexpr (QBDI::is_ios)
    mFlags |= PF::MF_EXEC;
  objectBlock =
      QBDI::allocateMappedMemory(objectVector.size(), nullptr, mFlags, ec);
  memcpy(objectBlock.base(), objectVector.data(), objectVector.size());

  // debug export result
  writeResult(source, objectVector);

  // LLVM Insanity Oriented Programming
  auto obj_ptr = llvm::object::ObjectFile::createObjectFile(
      llvm::MemoryBufferRef(llvm::StringRef((const char *)objectBlock.base(),
                                            objectBlock.allocatedSize()),
                            ""));
  if (not obj_ptr) {
    QBDI_ABORT("Failed to load input file");
  }

  // Finding the .text section of the object
  llvm::object::ObjectFile *object = obj_ptr.get().get();
  llvm::StringRef text_section_content;

  for (auto sit = object->sections().begin(); sit != object->sections().end();
       ++sit) {
    // llvm::errs() << "sections name : " << sit->getName().get() << '\n';
    if (sit->isText()) {
      if (auto E = sit->getContents()) {
        // verify that only one Text section
        REQUIRE(text_section_content.empty());
        text_section_content = *E;
        REQUIRE_FALSE(text_section_content.empty());
      } else {
        llvm::handleAllErrors(
            E.takeError(), [](const llvm::ErrorInfoBase &EIB) {
              QBDI_ABORT("Failed to load text section: {}", EIB.message());
            });
        return;
      }
    }
  }
  REQUIRE_FALSE(text_section_content.empty());
  code = llvm::ArrayRef<uint8_t>((const uint8_t *)text_section_content.data(),
                                 text_section_content.size());

  // perform relocation if needed
  QBDI::LLVMCPUs llvmcpus(cpu, mattrs);
  perform_reloc(object, llvmcpus);

  // Finally, set the page executable
  if constexpr (not QBDI::is_ios)
    llvm::sys::Memory::protectMappedMemory(objectBlock,
                                           PF::MF_READ | PF::MF_EXEC);
}

InMemoryObject::~InMemoryObject() { QBDI::releaseMappedMemory(objectBlock); }
