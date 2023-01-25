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
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "QBDI/Callback.h"
#include "QBDI/Errors.h"
#include "QBDI/InstAnalysis.h"
#include "QBDI/Options.h"
#include "QBDI/State.h"
#include "QBDI/VM.h"
#include "QBDI/VM_C.h"
#include "Utility/LogSys.h"

namespace QBDI {

void qbdi_initVM(VMInstanceRef *instance, const char *cpu, const char **mattrs,
                 Options opts) {
  QBDI_REQUIRE_ACTION(instance, return );

  *instance = nullptr;
  std::string cpuStr = "";
  std::vector<std::string> mattrsStr;

  if (cpu != nullptr) {
    cpuStr += cpu;
  }

  if (mattrs != nullptr) {
    for (unsigned i = 0; mattrs[i] != nullptr; i++) {
      mattrsStr.emplace_back(mattrs[i]);
    }
  }

  *instance = static_cast<VMInstanceRef>(new VM(cpuStr, mattrsStr, opts));
}

void qbdi_terminateVM(VMInstanceRef instance) {
  QBDI_REQUIRE_ACTION(instance, return );
  delete static_cast<VM *>(instance);
}

void qbdi_addInstrumentedRange(VMInstanceRef instance, rword start, rword end) {
  QBDI_REQUIRE_ACTION(instance, return );
  static_cast<VM *>(instance)->addInstrumentedRange(start, end);
}

bool qbdi_addInstrumentedModule(VMInstanceRef instance, const char *name) {
  QBDI_REQUIRE_ACTION(instance, return false);
  return static_cast<VM *>(instance)->addInstrumentedModule(std::string(name));
}

bool qbdi_addInstrumentedModuleFromAddr(VMInstanceRef instance, rword addr) {
  QBDI_REQUIRE_ACTION(instance, return false);
  return static_cast<VM *>(instance)->addInstrumentedModuleFromAddr(addr);
}

bool qbdi_instrumentAllExecutableMaps(VMInstanceRef instance) {
  QBDI_REQUIRE_ACTION(instance, return false);
  return static_cast<VM *>(instance)->instrumentAllExecutableMaps();
}

void qbdi_removeInstrumentedRange(VMInstanceRef instance, rword start,
                                  rword end) {
  QBDI_REQUIRE_ACTION(instance, return );
  static_cast<VM *>(instance)->removeInstrumentedRange(start, end);
}

void qbdi_removeAllInstrumentedRanges(VMInstanceRef instance) {
  QBDI_REQUIRE_ACTION(instance, return );
  static_cast<VM *>(instance)->removeAllInstrumentedRanges();
}

bool qbdi_removeInstrumentedModule(VMInstanceRef instance, const char *name) {
  QBDI_REQUIRE_ACTION(instance, return false);
  return static_cast<VM *>(instance)->removeInstrumentedModule(
      std::string(name));
}

bool qbdi_removeInstrumentedModuleFromAddr(VMInstanceRef instance, rword addr) {
  QBDI_REQUIRE_ACTION(instance, return false);
  return static_cast<VM *>(instance)->removeInstrumentedModuleFromAddr(addr);
}

bool qbdi_run(VMInstanceRef instance, rword start, rword stop) {
  QBDI_REQUIRE_ACTION(instance, return false);
  return static_cast<VM *>(instance)->run(start, stop);
}

bool qbdi_call(VMInstanceRef instance, rword *retval, rword function,
               uint32_t argNum, ...) {
  QBDI_REQUIRE_ACTION(instance, return false);
  va_list ap;
  va_start(ap, argNum);
  bool res = static_cast<VM *>(instance)->callV(retval, function, argNum, ap);
  va_end(ap);
  return res;
}

bool qbdi_callV(VMInstanceRef instance, rword *retval, rword function,
                uint32_t argNum, va_list ap) {
  QBDI_REQUIRE_ACTION(instance, return false);
  return static_cast<VM *>(instance)->callV(retval, function, argNum, ap);
}

bool qbdi_callA(VMInstanceRef instance, rword *retval, rword function,
                uint32_t argNum, const rword *args) {
  QBDI_REQUIRE_ACTION(instance, return false);
  return static_cast<VM *>(instance)->callA(retval, function, argNum, args);
}

GPRState *qbdi_getGPRState(VMInstanceRef instance) {
  QBDI_REQUIRE_ACTION(instance, return nullptr);
  return static_cast<VM *>(instance)->getGPRState();
}

FPRState *qbdi_getFPRState(VMInstanceRef instance) {
  QBDI_REQUIRE_ACTION(instance, return nullptr);
  return static_cast<VM *>(instance)->getFPRState();
}

void qbdi_setGPRState(VMInstanceRef instance, GPRState *gprState) {
  QBDI_REQUIRE_ACTION(instance, return );
  static_cast<VM *>(instance)->setGPRState(gprState);
}

void qbdi_setFPRState(VMInstanceRef instance, FPRState *fprState) {
  QBDI_REQUIRE_ACTION(instance, return );
  static_cast<VM *>(instance)->setFPRState(fprState);
}

Options qbdi_getOptions(VMInstanceRef instance) {
  QBDI_REQUIRE_ACTION(instance, return Options::NO_OPT);
  return static_cast<VM *>(instance)->getOptions();
}

void qbdi_setOptions(VMInstanceRef instance, Options options) {
  QBDI_REQUIRE_ACTION(instance, return );
  static_cast<VM *>(instance)->setOptions(options);
}

uint32_t qbdi_addMnemonicCB(VMInstanceRef instance, const char *mnemonic,
                            InstPosition pos, InstCallback cbk, void *data,
                            int priority) {
  QBDI_REQUIRE_ACTION(instance, return VMError::INVALID_EVENTID);
  return static_cast<VM *>(instance)->addMnemonicCB(mnemonic, pos, cbk, data,
                                                    priority);
}

uint32_t qbdi_addCodeCB(VMInstanceRef instance, InstPosition pos,
                        InstCallback cbk, void *data, int priority) {
  QBDI_REQUIRE_ACTION(instance, return VMError::INVALID_EVENTID);
  return static_cast<VM *>(instance)->addCodeCB(pos, cbk, data, priority);
}

uint32_t qbdi_addCodeAddrCB(VMInstanceRef instance, rword address,
                            InstPosition pos, InstCallback cbk, void *data,
                            int priority) {
  QBDI_REQUIRE_ACTION(instance, return VMError::INVALID_EVENTID);
  return static_cast<VM *>(instance)->addCodeAddrCB(address, pos, cbk, data,
                                                    priority);
}

uint32_t qbdi_addCodeRangeCB(VMInstanceRef instance, rword start, rword end,
                             InstPosition pos, InstCallback cbk, void *data,
                             int priority) {
  QBDI_REQUIRE_ACTION(instance, return VMError::INVALID_EVENTID);
  return static_cast<VM *>(instance)->addCodeRangeCB(start, end, pos, cbk, data,
                                                     priority);
}

uint32_t qbdi_addMemAccessCB(VMInstanceRef instance, MemoryAccessType type,
                             InstCallback cbk, void *data, int priority) {
  QBDI_REQUIRE_ACTION(instance, return VMError::INVALID_EVENTID);
  return static_cast<VM *>(instance)->addMemAccessCB(type, cbk, data, priority);
}

uint32_t qbdi_addMemAddrCB(VMInstanceRef instance, rword address,
                           MemoryAccessType type, InstCallback cbk,
                           void *data) {
  QBDI_REQUIRE_ACTION(instance, return VMError::INVALID_EVENTID);
  return static_cast<VM *>(instance)->addMemAddrCB(address, type, cbk, data);
}

uint32_t qbdi_addMemRangeCB(VMInstanceRef instance, rword start, rword end,
                            MemoryAccessType type, InstCallback cbk,
                            void *data) {
  QBDI_REQUIRE_ACTION(instance, return VMError::INVALID_EVENTID);
  return static_cast<VM *>(instance)->addMemRangeCB(start, end, type, cbk,
                                                    data);
}

uint32_t qbdi_addVMEventCB(VMInstanceRef instance, VMEvent mask, VMCallback cbk,
                           void *data) {
  QBDI_REQUIRE_ACTION(instance, return VMError::INVALID_EVENTID);
  return static_cast<VM *>(instance)->addVMEventCB(mask, cbk, data);
}

bool qbdi_deleteInstrumentation(VMInstanceRef instance, uint32_t id) {
  QBDI_REQUIRE_ACTION(instance, return false);
  return static_cast<VM *>(instance)->deleteInstrumentation(id);
}

void qbdi_deleteAllInstrumentations(VMInstanceRef instance) {
  QBDI_REQUIRE_ACTION(instance, return );
  static_cast<VM *>(instance)->deleteAllInstrumentations();
}

const InstAnalysis *qbdi_getInstAnalysis(const VMInstanceRef instance,
                                         AnalysisType type) {
  QBDI_REQUIRE_ACTION(instance, return nullptr);
  return static_cast<const VM *>(instance)->getInstAnalysis(type);
}

const InstAnalysis *qbdi_getCachedInstAnalysis(const VMInstanceRef instance,
                                               rword address,
                                               AnalysisType type) {
  QBDI_REQUIRE_ACTION(instance, return nullptr);
  return static_cast<const VM *>(instance)->getCachedInstAnalysis(address,
                                                                  type);
}

bool qbdi_recordMemoryAccess(VMInstanceRef instance, MemoryAccessType type) {
  QBDI_REQUIRE_ACTION(instance, return false);
  return static_cast<VM *>(instance)->recordMemoryAccess(type);
}

MemoryAccess *qbdi_getInstMemoryAccess(VMInstanceRef instance, size_t *size) {
  QBDI_REQUIRE_ACTION(instance, return nullptr);
  QBDI_REQUIRE_ACTION(size, return nullptr);
  *size = 0;
  std::vector<MemoryAccess> ma_vec =
      static_cast<VM *>(instance)->getInstMemoryAccess();
  // Do not allocate if no shadows
  if (ma_vec.size() == 0) {
    return NULL;
  }
  // Allocate and copy
  *size = ma_vec.size();
  MemoryAccess *ma_arr =
      static_cast<MemoryAccess *>(malloc(*size * sizeof(MemoryAccess)));
  for (size_t i = 0; i < *size; i++) {
    ma_arr[i] = ma_vec[i];
  }
  return ma_arr;
}

MemoryAccess *qbdi_getBBMemoryAccess(VMInstanceRef instance, size_t *size) {
  QBDI_REQUIRE_ACTION(instance, return nullptr);
  QBDI_REQUIRE_ACTION(size, return nullptr);
  *size = 0;
  std::vector<MemoryAccess> ma_vec =
      static_cast<VM *>(instance)->getBBMemoryAccess();
  // Do not allocate if no shadows
  if (ma_vec.size() == 0) {
    return NULL;
  }
  // Allocate and copy
  *size = ma_vec.size();
  MemoryAccess *ma_arr =
      static_cast<MemoryAccess *>(malloc(*size * sizeof(MemoryAccess)));
  for (size_t i = 0; i < *size; i++) {
    ma_arr[i] = ma_vec[i];
  }
  return ma_arr;
}

bool qbdi_precacheBasicBlock(VMInstanceRef instance, rword pc) {
  QBDI_REQUIRE_ACTION(instance, return false);
  return static_cast<VM *>(instance)->precacheBasicBlock(pc);
}

void qbdi_clearAllCache(VMInstanceRef instance) {
  static_cast<VM *>(instance)->clearAllCache();
}

void qbdi_clearCache(VMInstanceRef instance, rword start, rword end) {
  static_cast<VM *>(instance)->clearCache(start, end);
}

uint32_t qbdi_addInstrRule(VMInstanceRef instance, InstrRuleCallbackC cbk,
                           AnalysisType type, void *data) {
  QBDI_REQUIRE_ACTION(instance, return VMError::INVALID_EVENTID);
  return static_cast<VM *>(instance)->addInstrRule(cbk, type, data);
}

uint32_t qbdi_addInstrRuleRange(VMInstanceRef instance, rword start, rword end,
                                InstrRuleCallbackC cbk, AnalysisType type,
                                void *data) {
  QBDI_REQUIRE_ACTION(instance, return VMError::INVALID_EVENTID);
  return static_cast<VM *>(instance)->addInstrRuleRange(start, end, cbk, type,
                                                        data);
}

void qbdi_addInstrRuleData(InstrRuleDataVec cbks, InstPosition position,
                           InstCallback cbk, void *data, int priority) {
  QBDI_REQUIRE_ACTION(cbks, return );
  cbks->emplace_back(position, cbk, data, priority);
}

} // namespace QBDI
