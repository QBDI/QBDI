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
#include <algorithm>
#include <bitset>

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

#include "Engine/LLVMCPU.h"
#include "Utility/LogSys.h"
#include "Utility/System.h"

#include "spdlog/fmt/bin_to_hex.h"

namespace QBDI {

LLVMCPU::LLVMCPU(const std::string &_cpu,
                 const std::vector<std::string> &_mattrs, Options opts)
    : cpu(_cpu), mattrs(_mattrs), options(opts) {

  std::string error;
  std::string featuresStr;

  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllDisassemblers();

  // Build features string
  if (cpu.empty()) {
    cpu = QBDI::getHostCPUName();
    // If API is broken on ARM, we are facing big problems...
    if constexpr (is_arm) {
      QBDI_REQUIRE(!cpu.empty() && cpu != "generic");
    }
  }
  if (mattrs.empty()) {
    mattrs = getHostCPUFeatures();
  }
  if (!mattrs.empty()) {
    llvm::SubtargetFeatures features;
    for (unsigned i = 0; i != mattrs.size(); ++i) {
      features.AddFeature(mattrs[i]);
    }
    featuresStr = features.getString();
  }

  // lookup target
  tripleName = llvm::Triple::normalize(llvm::sys::getDefaultTargetTriple());
  llvm::Triple processTriple(tripleName);
  target = llvm::TargetRegistry::lookupTarget(tripleName, error);
  QBDI_DEBUG("Initialized LLVM for target {}", tripleName.c_str());

  // Allocate all LLVM classes
  llvm::MCTargetOptions MCOptions;
  MRI = std::unique_ptr<llvm::MCRegisterInfo>(
      target->createMCRegInfo(tripleName));
  MAI = std::unique_ptr<llvm::MCAsmInfo>(
      target->createMCAsmInfo(*MRI, tripleName, MCOptions));
  MOFI = std::make_unique<llvm::MCObjectFileInfo>();
  MCTX = std::make_unique<llvm::MCContext>(MAI.get(), MRI.get(), MOFI.get());
  MOFI->InitMCObjectFileInfo(processTriple, false, *MCTX);
  MCII = std::unique_ptr<llvm::MCInstrInfo>(target->createMCInstrInfo());
  MSTI = std::unique_ptr<llvm::MCSubtargetInfo>(
      target->createMCSubtargetInfo(tripleName, cpu, featuresStr));
  QBDI_DEBUG("Initialized LLVM subtarget with cpu {} and features {}",
             cpu.c_str(), featuresStr.c_str());

  auto MAB = std::unique_ptr<llvm::MCAsmBackend>(
      target->createMCAsmBackend(*MSTI, *MRI, MCOptions));
  MCE = std::unique_ptr<llvm::MCCodeEmitter>(
      target->createMCCodeEmitter(*MCII, *MRI, *MCTX));

  // assembler, disassembler and printer
  null_ostream = std::make_unique<llvm::raw_null_ostream>();

  disassembler = std::unique_ptr<llvm::MCDisassembler>(
      target->createMCDisassembler(*MSTI, *MCTX));

  auto codeEmitter = std::unique_ptr<llvm::MCCodeEmitter>(
      target->createMCCodeEmitter(*MCII, *MRI, *MCTX));

  auto objectWriter = std::unique_ptr<llvm::MCObjectWriter>(
      MAB->createObjectWriter(*null_ostream));

  assembler = std::make_unique<llvm::MCAssembler>(
      *MCTX, std::move(MAB), std::move(codeEmitter), std::move(objectWriter));

  unsigned int variant = 0;
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
  variant = ((options & Options::OPT_ATT_SYNTAX) == 0) ? 1 : 0;
#else
  variant = MAI.getAssemblerDialect();
#endif

  asmPrinter = std::unique_ptr<llvm::MCInstPrinter>(target->createMCInstPrinter(
      MSTI->getTargetTriple(), variant, *MAI, *MCII, *MRI));
  asmPrinter->setPrintImmHex(true);
  asmPrinter->setPrintImmHex(llvm::HexStyle::C);
}

LLVMCPU::~LLVMCPU() = default;

llvm::MCDisassembler::DecodeStatus
LLVMCPU::getInstruction(llvm::MCInst &instr, uint64_t &size,
                        llvm::ArrayRef<uint8_t> bytes, uint64_t address) const {
  return disassembler->getInstruction(instr, size, bytes, address,
                                      llvm::nulls());
}

void LLVMCPU::writeInstruction(const llvm::MCInst inst,
                               memory_ostream *stream) const {
  // MCCodeEmitter needs a fixups array
  llvm::SmallVector<llvm::MCFixup, 4> fixups;

  uint64_t pos = stream->current_pos();
  QBDI_DEBUG_BLOCK({
    uint64_t address = reinterpret_cast<uint64_t>(stream->get_ptr()) + pos;
    std::string disass = showInst(inst, address);
    QBDI_DEBUG("Assembling {} at 0x{:x}", disass.c_str(), address);
  });
  assembler->getEmitter().encodeInstruction(inst, *stream, fixups, *MSTI);
  uint64_t size = stream->current_pos() - pos;

  if (fixups.size() > 0) {
    llvm::MCValue target = llvm::MCValue();
    llvm::MCFixup fixup = fixups.pop_back_val();
    int64_t value;
    if (fixup.getValue()->evaluateAsAbsolute(value)) {
      assembler->getBackend().applyFixup(
          *assembler, fixup, target,
          llvm::MutableArrayRef<char>((char *)stream->get_ptr() + pos, size),
          (uint64_t)value, true, MSTI.get());
    } else {
      QBDI_WARN("Could not evalutate fixup, might crash!");
    }
  }

  QBDI_DEBUG(
      "Assembly result at 0x{:x} is: {:n}",
      reinterpret_cast<uint64_t>(stream->get_ptr()) + pos,
      spdlog::to_hex(reinterpret_cast<uint8_t *>(stream->get_ptr()) + pos,
                     reinterpret_cast<uint8_t *>(stream->get_ptr()) +
                         stream->current_pos()));
}

std::string LLVMCPU::showInst(const llvm::MCInst &inst,
                              uint64_t address) const {
  std::string out;
  llvm::raw_string_ostream rso(out);

  llvm::StringRef unusedAnnotations;
  asmPrinter->printInst(&inst, address, unusedAnnotations, *MSTI, rso);

  rso.flush();
  return out;
}

const char *LLVMCPU::getRegisterName(unsigned int id) const {
  return MRI->getName(id);
}

void LLVMCPU::setOptions(Options opts) {
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
  if (((opts ^ options) & Options::OPT_ATT_SYNTAX) != 0) {
    asmPrinter =
        std::unique_ptr<llvm::MCInstPrinter>(target->createMCInstPrinter(
            MSTI->getTargetTriple(),
            ((opts & Options::OPT_ATT_SYNTAX) == 0) ? 1 : 0, *MAI, *MCII,
            *MRI));
    asmPrinter->setPrintImmHex(true);
    asmPrinter->setPrintImmHex(llvm::HexStyle::C);
  }
#endif
  options = opts;
}

} // namespace QBDI
