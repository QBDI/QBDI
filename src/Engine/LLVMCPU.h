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
#ifndef LLVMCPU_H
#define LLVMCPU_H

#include <memory>
#include <vector>

#include "llvm/ADT/APInt.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"

#include "QBDI/Options.h"
#include "Utility/memory_ostream.h"

namespace llvm {
class MCAsmBackend;
class MCAsmInfo;
class MCAssembler;
class MCCodeEmitter;
class MCContext;
class MCInst;
class MCInstPrinter;
class MCInstrInfo;
class MCObjectFileInfo;
class MCRegisterInfo;
class MCSubtargetInfo;
class Target;
class raw_pwrite_stream;
} // namespace llvm

namespace QBDI {

class LLVMCPU {

protected:
  std::string tripleName;
  std::string cpu;
  std::vector<std::string> mattrs;
  const llvm::Target *target;
  Options options;

  std::unique_ptr<llvm::MCAsmInfo> MAI;
  std::unique_ptr<llvm::MCCodeEmitter> MCE;
  std::unique_ptr<llvm::MCContext> MCTX;
  std::unique_ptr<llvm::MCInstrInfo> MCII;
  std::unique_ptr<llvm::MCObjectFileInfo> MOFI;
  std::unique_ptr<llvm::MCRegisterInfo> MRI;
  std::unique_ptr<llvm::MCSubtargetInfo> MSTI;

  std::unique_ptr<llvm::MCAssembler> assembler;
  std::unique_ptr<llvm::MCDisassembler> disassembler;
  std::unique_ptr<llvm::MCInstPrinter> asmPrinter;
  std::unique_ptr<llvm::raw_pwrite_stream> null_ostream;

public:
  LLVMCPU(const std::string &cpu = "",
          const std::vector<std::string> &mattrs = {},
          Options opts = Options::NO_OPT);

  ~LLVMCPU();

  LLVMCPU(const LLVMCPU &&) = delete;
  LLVMCPU &operator=(const LLVMCPU &&) = delete;

  LLVMCPU(const LLVMCPU &) = delete;
  LLVMCPU &operator=(const LLVMCPU &) = delete;

  void writeInstruction(llvm::MCInst inst, memory_ostream *stream) const;

  llvm::MCDisassembler::DecodeStatus
  getInstruction(llvm::MCInst &inst, uint64_t &size,
                 llvm::ArrayRef<uint8_t> bytes, uint64_t address) const;

  std::string showInst(const llvm::MCInst &inst, uint64_t address) const;

  const char *getRegisterName(unsigned int id) const;

  inline const bool isSameCPU(const LLVMCPU &other) const {
    return cpu == other.cpu and mattrs == other.mattrs;
  }

  inline const std::string &getCPU() const { return cpu; }

  inline const std::vector<std::string> &getMattrs() const { return mattrs; }

  inline const llvm::MCInstrInfo &getMCII() const { return *MCII; }

  inline const llvm::MCRegisterInfo &getMRI() const { return *MRI; }

  Options getOptions() const { return options; }
  void setOptions(Options opts);
};

} // namespace QBDI

#endif // LLVMCPU_H
