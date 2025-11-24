/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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
#include <utility>

#include "llvm/Support/Error.h"
#include "llvm/Support/Process.h"

#include "QBDI/Memory.hpp"
#include "ExecBroker/ExecBroker.h"
#include "Utility/LogSys.h"

namespace QBDI {

ExecBroker::ExecBroker(std::unique_ptr<ExecBlock> _transferBlock,
                       const LLVMCPUs &llvmCPUs, VMInstanceRef vminstance)
    : transferBlock(std::move(_transferBlock)) {
  pageSize = llvm::expectedToOptional(llvm::sys::Process::getPageSize())
                 .value_or(4096);
  initExecBrokerSequences(llvmCPUs);
}

void ExecBroker::changeVMInstanceRef(VMInstanceRef vminstance) {
  transferBlock->changeVMInstanceRef(vminstance);
}

void ExecBroker::addInstrumentedRange(const Range<rword> &r) {
  QBDI_DEBUG("Adding instrumented range [0x{:x}, 0x{:x}]", r.start(), r.end());
  instrumented.add(r);
}

void ExecBroker::removeInstrumentedRange(const Range<rword> &r) {
  QBDI_DEBUG("Removing instrumented range [0x{:x}, 0x{:x}]", r.start(),
             r.end());
  instrumented.remove(r);
}

void ExecBroker::removeAllInstrumentedRanges() { instrumented.clear(); }

bool ExecBroker::addInstrumentedModule(const std::string &name) {
  bool instrumented = false;
  if (name.empty()) {
    return false;
  }

  for (const MemoryMap &m : getCurrentProcessMaps()) {
    if ((m.name == name) && (m.permission & QBDI::PF_EXEC)) {
      addInstrumentedRange(m.range);
      instrumented = true;
    }
  }
  return instrumented;
}

bool ExecBroker::addInstrumentedModuleFromAddr(rword addr) {
  for (const MemoryMap &m : getCurrentProcessMaps()) {
    if (m.range.contains(addr)) {
      if (not m.name.empty()) {
        return addInstrumentedModule(m.name);
      } else if (m.permission & QBDI::PF_EXEC) {
        addInstrumentedRange(m.range);
        return true;
      } else {
        return false;
      }
    }
  }
  return false;
}

bool ExecBroker::removeInstrumentedModule(const std::string &name) {
  bool removed = false;

  for (const MemoryMap &m : getCurrentProcessMaps()) {
    if (m.name == name) {
      removeInstrumentedRange(m.range);
      removed = true;
    }
  }
  return removed;
}

bool ExecBroker::removeInstrumentedModuleFromAddr(rword addr) {
  for (const MemoryMap &m : getCurrentProcessMaps()) {
    if (m.range.contains(addr)) {
      removeInstrumentedRange(m.range);
      if (not m.name.empty()) {
        removeInstrumentedModule(m.name);
      }
      return true;
    }
  }
  return false;
}

bool ExecBroker::instrumentAllExecutableMaps() {
  bool instrumented = false;

  for (const MemoryMap &m : getCurrentProcessMaps()) {
    if (m.permission & QBDI::PF_EXEC) {
      addInstrumentedRange(m.range);
      instrumented = true;
    }
  }
  return instrumented;
}

bool ExecBroker::canTransferExecution(GPRState *gprState) const {
  return getReturnPoint(gprState) ? true : false;
}

} // namespace QBDI
