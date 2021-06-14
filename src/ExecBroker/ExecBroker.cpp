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
#include "ExecBroker/ExecBroker.h"
#include "Patch/ExecBlockFlags.h"
#include "Utility/LogSys.h"

namespace QBDI {

ExecBroker::ExecBroker(std::unique_ptr<ExecBlock> _transferBlock,
                       VMInstanceRef vminstance)
    : transferBlock(std::move(_transferBlock)) {
  pageSize = llvm::expectedToOptional(llvm::sys::Process::getPageSize())
                 .getValueOr(4096);
}

void ExecBroker::changeVMInstanceRef(VMInstanceRef vminstance) {
  transferBlock->changeVMInstanceRef(vminstance);
}

void ExecBroker::addInstrumentedRange(const Range<rword> &r) {
  QBDI_DEBUG("Adding instrumented range [{:x}, {:x}]", r.start(), r.end());
  instrumented.add(r);
}

void ExecBroker::removeInstrumentedRange(const Range<rword> &r) {
  QBDI_DEBUG("Removing instrumented range [{:x}, {:x}]", r.start(), r.end());
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
  bool instrumented = false;

  for (const MemoryMap &m : getCurrentProcessMaps()) {
    if (m.range.contains(addr)) {
      instrumented = addInstrumentedModule(m.name);
      break;
    }
  }
  return instrumented;
}

bool ExecBroker::removeInstrumentedModule(const std::string &name) {
  bool removed = false;

  for (const MemoryMap &m : getCurrentProcessMaps()) {
    if ((m.name == name) && (m.permission & QBDI::PF_EXEC)) {
      removeInstrumentedRange(m.range);
      removed = true;
    }
  }
  return removed;
}

bool ExecBroker::removeInstrumentedModuleFromAddr(rword addr) {
  bool removed = false;

  for (const MemoryMap &m : getCurrentProcessMaps()) {
    if (m.range.contains(addr)) {
      removed = removeInstrumentedModule(m.name);
      break;
    }
  }
  return removed;
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

bool ExecBroker::transferExecution(rword addr, GPRState *gprState,
                                   FPRState *fprState) {
  rword hookedAddress = 0;
  rword hook = 0;
  rword *ptr = NULL;

  ptr = getReturnPoint(gprState);
  if (!ptr)
    return false;

  // Backup / Patch return address
  hookedAddress = *ptr;
  hook = transferBlock->getCurrentPC() + transferBlock->getEpilogueOffset();
  *ptr = hook;
  QBDI_DEBUG("Patched 0x{:x} hooking return address 0x{:x} with 0x{:x}",
             reinterpret_cast<uintptr_t>(ptr), hookedAddress, *ptr);

  // Write transfer state
  transferBlock->getContext()->gprState = *gprState;
  transferBlock->getContext()->fprState = *fprState;
  transferBlock->getContext()->hostState.selector = addr;
  transferBlock->getContext()->hostState.executeFlags = defaultExecuteFlags;
  // Execute transfer
  QBDI_DEBUG("Transfering execution to 0x{:x} using transferBlock 0x{:x}", addr,
             reinterpret_cast<uintptr_t>(&transferBlock));
  transferBlock->run();
  // Restore original return
  QBDI_GPR_SET(&transferBlock->getContext()->gprState, REG_PC, hookedAddress);
#if defined(QBDI_ARCH_ARM)
  // Under ARM, also reset the LR register
  if (QBDI_GPR_GET(&transferBlock->getContext()->gprState, REG_LR) == hook) {
    QBDI_GPR_SET(&transferBlock->getContext()->gprState, REG_LR, hookedAddress);
  }
#endif
  // Read transfer result
  *gprState = transferBlock->getContext()->gprState;
  *fprState = transferBlock->getContext()->fprState;

  return true;
}

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)

rword *ExecBroker::getReturnPoint(GPRState *gprState) const {
  static int SCAN_DISTANCE = 3;
  rword *ptr = (rword *)QBDI_GPR_GET(gprState, REG_SP);

  for (int i = 0; i < SCAN_DISTANCE; i++) {
    if (isInstrumented(ptr[i])) {
      QBDI_DEBUG("Found instrumented return address on the stack at 0x{:x}",
                 reinterpret_cast<uintptr_t>(&(ptr[i])));
      return &(ptr[i]);
    }
  }
  QBDI_DEBUG("No instrumented return address found on the stack");
  return NULL;
}

#elif defined(QBDI_ARCH_ARM)

rword *ExecBroker::getReturnPoint(GPRState *gprState) const {
  static int SCAN_DISTANCE = 2;
  rword *ptr = (rword *)gprState->sp;

  if (isInstrumented(gprState->lr)) {
    QBDI_DEBUG("Found instrumented return address in LR register");
    return &(gprState->lr);
  }
  for (int i = 0; i < SCAN_DISTANCE; i++) {
    if (isInstrumented(ptr[i])) {
      QBDI_DEBUG("Found instrumented return address on the stack at 0x{:x}",
                 reinterpret_cast<uintptr_t>(&(ptr[i])));
      return &(ptr[i]);
    }
  }

  QBDI_DEBUG("LR register does not contain an instrumented return address");
  return NULL;
}

#endif

} // namespace QBDI
