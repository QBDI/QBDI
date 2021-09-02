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
#ifndef QBDI_EXECBROKER_H
#define QBDI_EXECBROKER_H

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "llvm/Support/Memory.h"

#include "QBDI/Callback.h"
#include "QBDI/Config.h"
#include "QBDI/Range.h"
#include "QBDI/State.h"
#include "ExecBlock/ExecBlock.h"

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
#include "ExecBroker/X86_64/ExecBroker_X86_64.h"
#elif defined(QBDI_ARCH_ARM)
#include "ExecBroker/ARM/ExecBroker_ARM.h"
#elif defined(QBDI_ARCH_AARCH64)
#include "ExecBroker/AARCH64/ExecBroker_AARCH64.h"
#else
#error "No Implementation of ExecBroker for the current ARCH"
#endif

namespace QBDI {
class LLVMCPUs;

class ExecBroker {

private:
  RangeSet<rword> instrumented;
  std::unique_ptr<ExecBlock> transferBlock;
  rword pageSize;

  using PF = llvm::sys::Memory::ProtectionFlags;

  // ARCH specific method
  ExecBrokerArchData archData;

  void initExecBrokerSequences(const LLVMCPUs &llvmCPUs);
  rword *getReturnPoint(GPRState *gprState) const;

public:
  ExecBroker(std::unique_ptr<ExecBlock> transferBlock, const LLVMCPUs &llvmCPUs,
             VMInstanceRef vminstance = nullptr);

  void changeVMInstanceRef(VMInstanceRef vminstance);

  bool isInstrumented(rword addr) const { return instrumented.contains(addr); }

  void setInstrumentedRange(const RangeSet<rword> &r) { instrumented = r; }

  const RangeSet<rword> &getInstrumentedRange() const { return instrumented; }

  void addInstrumentedRange(const Range<rword> &r);
  bool addInstrumentedModule(const std::string &name);
  bool addInstrumentedModuleFromAddr(rword addr);

  void removeInstrumentedRange(const Range<rword> &r);
  bool removeInstrumentedModule(const std::string &name);
  bool removeInstrumentedModuleFromAddr(rword addr);
  void removeAllInstrumentedRanges();

  bool instrumentAllExecutableMaps();

  bool canTransferExecution(GPRState *gprState) const;

  bool transferExecution(rword addr, GPRState *gprState, FPRState *fprState);
};

} // namespace QBDI

#endif // QBDI_EXECBROKER_H
