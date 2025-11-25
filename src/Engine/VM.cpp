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
#include <algorithm>
#include <cstdint>
#include <memory>
#include <stdarg.h>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>

#include "QBDI/Callback.h"
#include "QBDI/Config.h"
#include "QBDI/Errors.h"
#include "QBDI/InstAnalysis.h"
#include "QBDI/Memory.hpp"
#include "QBDI/Options.h"
#include "QBDI/PtrAuth.h"
#include "QBDI/Range.h"
#include "QBDI/State.h"
#include "QBDI/VM.h"

#include "Engine/Engine.h"
#include "Engine/VM_internal.h"
#include "ExecBlock/ExecBlock.h"
#include "Patch/InstrRule.h"
#include "Patch/InstrRules.h"
#include "Patch/MemoryAccess.h"
#include "Patch/PatchCondition.h"
#include "Patch/PatchGenerator.h"
#include "Patch/PatchUtils.h"
#include "Utility/LogSys.h"
#include "Utility/StackSwitch.h"

// Mask to identify Virtual Callback events
#define EVENTID_VIRTCB_MASK (1UL << 31)

namespace QBDI {

VMAction memReadGate(VMInstanceRef vm, GPRState *gprState, FPRState *fprState,
                     void *data) {
  std::vector<std::pair<uint32_t, MemCBInfo>> &memCBInfos =
      *static_cast<std::vector<std::pair<uint32_t, MemCBInfo>> *>(data);
  std::vector<MemoryAccess> memAccesses = vm->getInstMemoryAccess();
  RangeSet<rword> readRange;
  for (const MemoryAccess &memAccess : memAccesses) {
    if (memAccess.type & MEMORY_READ) {
      Range<rword> accessRange(memAccess.accessAddress,
                               memAccess.accessAddress + memAccess.size,
                               real_addr_t());
      readRange.add(accessRange);
    }
  }

  VMAction action = VMAction::CONTINUE;
  for (const auto &p : memCBInfos) {
    // Check access type and range
    if (p.second.type == MEMORY_READ and readRange.overlaps(p.second.range)) {
      // Forward to virtual callback
      VMAction ret = p.second.cbk(vm, gprState, fprState, p.second.data);
      // Always keep the most extreme action as the return
      if (ret > action) {
        action = ret;
      }
    }
  }
  return action;
}

VMAction memWriteGate(VMInstanceRef vm, GPRState *gprState, FPRState *fprState,
                      void *data) {
  std::vector<std::pair<uint32_t, MemCBInfo>> &memCBInfos =
      *static_cast<std::vector<std::pair<uint32_t, MemCBInfo>> *>(data);
  std::vector<MemoryAccess> memAccesses = vm->getInstMemoryAccess();
  RangeSet<rword> readRange;
  RangeSet<rword> writeRange;
  for (const MemoryAccess &memAccess : memAccesses) {
    Range<rword> accessRange(memAccess.accessAddress,
                             memAccess.accessAddress + memAccess.size,
                             real_addr_t());
    if (memAccess.type & MEMORY_READ) {
      readRange.add(accessRange);
    }
    if (memAccess.type & MEMORY_WRITE) {
      writeRange.add(accessRange);
    }
  }

  VMAction action = VMAction::CONTINUE;
  for (const auto &p : memCBInfos) {
    // Check accessCB
    // 1. has MEMORY_WRITE and write range overlaps
    // 2. is MEMORY_READ_WRITE and read range overlaps
    // note: the case with MEMORY_READ only is managed by memReadGate
    if (((p.second.type & MEMORY_WRITE) and
         writeRange.overlaps(p.second.range)) or
        (p.second.type == MEMORY_READ_WRITE and
         readRange.overlaps(p.second.range))) {
      // Forward to virtual callback
      VMAction ret = p.second.cbk(vm, gprState, fprState, p.second.data);
      // Always keep the most extreme action as the return
      if (ret > action) {
        action = ret;
      }
    }
  }
  return action;
}

std::vector<InstrRuleDataCBK>
InstrCBGateC(VMInstanceRef vm, const InstAnalysis *inst, void *_data) {
  InstrCBInfo *data = static_cast<InstrCBInfo *>(_data);
  std::vector<InstrRuleDataCBK> vec{};
  data->cbk(vm, inst, &vec, data->data);
  return vec;
}

VMAction VMCBLambdaProxy(VMInstanceRef vm, const VMState *vmState,
                         GPRState *gprState, FPRState *fprState, void *_data) {
  VMCbLambda &data = *static_cast<VMCbLambda *>(_data);
  return data(vm, vmState, gprState, fprState);
}

VMAction InstCBLambdaProxy(VMInstanceRef vm, GPRState *gprState,
                           FPRState *fprState, void *_data) {
  InstCbLambda &data = *static_cast<InstCbLambda *>(_data);
  return data(vm, gprState, fprState);
}

std::vector<InstrRuleDataCBK>
InstrRuleCBLambdaProxy(VMInstanceRef vm, const InstAnalysis *ana, void *_data) {
  InstrRuleCbLambda &data = *static_cast<InstrRuleCbLambda *>(_data);
  return data(vm, ana);
}

VMAction stopCallback(VMInstanceRef vm, GPRState *gprState, FPRState *fprState,
                      void *data) {
  return VMAction::STOP;
}

// constructor

VM::VM(const std::string &cpu, const std::vector<std::string> &mattrs,
       Options opts)
    : memoryLoggingLevel(0), memCBID(0),
      memReadGateCBID(VMError::INVALID_EVENTID),
      memWriteGateCBID(VMError::INVALID_EVENTID) {
#if defined(_QBDI_ASAN_ENABLED_)
  opts |= Options::OPT_DISABLE_FPR;
#endif
  engine = std::make_unique<Engine>(cpu, mattrs, opts, this);
  memCBInfos = std::make_unique<std::vector<std::pair<uint32_t, MemCBInfo>>>();
  instrCBInfos = std::make_unique<
      std::vector<std::pair<uint32_t, std::unique_ptr<InstrCBInfo>>>>();
}

// destructor

VM::~VM() = default;

// move constructor

VM::VM(VM &&vm)
    : engine(std::move(vm.engine)), memoryLoggingLevel(vm.memoryLoggingLevel),
      memCBInfos(std::move(vm.memCBInfos)), memCBID(vm.memCBID),
      memReadGateCBID(vm.memReadGateCBID),
      memWriteGateCBID(vm.memWriteGateCBID),
      instrCBInfos(std::move(vm.instrCBInfos)),
      vmCBData(std::move(vm.vmCBData)), instCBData(std::move(vm.instCBData)),
      instrRuleCBData(std::move(vm.instrRuleCBData)) {

  engine->changeVMInstanceRef(this);
}

// move operator

VM &VM::operator=(VM &&vm) {
  engine = std::move(vm.engine);
  memoryLoggingLevel = vm.memoryLoggingLevel;
  memCBInfos = std::move(vm.memCBInfos);
  memCBID = vm.memCBID;
  memReadGateCBID = vm.memReadGateCBID;
  memWriteGateCBID = vm.memWriteGateCBID;
  instrCBInfos = std::move(vm.instrCBInfos);
  vmCBData = std::move(vm.vmCBData);
  instCBData = std::move(vm.instCBData);
  instrRuleCBData = std::move(vm.instrRuleCBData);

  engine->changeVMInstanceRef(this);

  return *this;
}

// copy constructor

VM::VM(const VM &vm)
    : engine(std::make_unique<Engine>(*vm.engine)),
      memoryLoggingLevel(vm.memoryLoggingLevel),
      memCBInfos(std::make_unique<std::vector<std::pair<uint32_t, MemCBInfo>>>(
          *vm.memCBInfos)),
      memCBID(vm.memCBID), memReadGateCBID(vm.memReadGateCBID),
      memWriteGateCBID(vm.memWriteGateCBID), vmCBData(vm.vmCBData),
      instCBData(vm.instCBData), instrRuleCBData(vm.instrRuleCBData) {

  engine->changeVMInstanceRef(this);
  instrCBInfos = std::make_unique<
      std::vector<std::pair<uint32_t, std::unique_ptr<InstrCBInfo>>>>();
  for (const auto &p : *vm.instrCBInfos) {
    engine->deleteInstrumentation(p.first);
    addInstrRuleRange(p.second->range.start(), p.second->range.end(),
                      p.second->cbk, p.second->type, p.second->data);
  }

  if (memReadGateCBID != VMError::INVALID_EVENTID) {
    InstrRule *rule = engine->getInstrRule(memReadGateCBID);
    QBDI_REQUIRE_ABORT(rule != nullptr, "VM copy internal error");
    QBDI_REQUIRE_ABORT(rule->changeDataPtr(memCBInfos.get()),
                       "VM copy internal error");
  }

  if (memWriteGateCBID != VMError::INVALID_EVENTID) {
    InstrRule *rule = engine->getInstrRule(memWriteGateCBID);
    QBDI_REQUIRE_ABORT(rule != nullptr, "VM copy internal error");
    QBDI_REQUIRE_ABORT(rule->changeDataPtr(memCBInfos.get()),
                       "VM copy internal error");
  }

  for (auto &p : vmCBData) {
    engine->setVMEventCB(p.first, VMCBLambdaProxy, &p.second);
  }

  for (std::pair<uint32_t, InstCbLambda> &p : instCBData) {
    if (p.first & EVENTID_VIRTCB_MASK) {
      uint32_t id = p.first;
      auto it = std::find_if(memCBInfos->begin(), memCBInfos->end(),
                             [id](const std::pair<uint32_t, MemCBInfo> &el) {
                               return id == el.first;
                             });
      QBDI_REQUIRE_ABORT(it != memCBInfos->end(), "VM copy internal error");
      it->second.data = &p.second;
    } else {
      InstrRule *rule = engine->getInstrRule(p.first);
      QBDI_REQUIRE_ABORT(rule != nullptr, "VM copy internal error");
      QBDI_REQUIRE_ABORT(rule->changeDataPtr(&p.second),
                         "VM copy internal error");
    }
  }

  for (std::pair<uint32_t, InstrRuleCbLambda> &p : instrRuleCBData) {
    InstrRule *rule = engine->getInstrRule(p.first);
    QBDI_REQUIRE_ABORT(rule != nullptr, "VM copy internal error");
    QBDI_REQUIRE_ABORT(rule->changeDataPtr(&p.second),
                       "VM copy internal error");
  }
}

// Copy operator

VM &VM::operator=(const VM &vm) {
  *engine = *vm.engine;
  *memCBInfos = *vm.memCBInfos;

  memoryLoggingLevel = vm.memoryLoggingLevel;
  memCBID = vm.memCBID;
  memReadGateCBID = vm.memReadGateCBID;
  memWriteGateCBID = vm.memWriteGateCBID;

  instrCBInfos = std::make_unique<
      std::vector<std::pair<uint32_t, std::unique_ptr<InstrCBInfo>>>>();
  for (const auto &p : *vm.instrCBInfos) {
    engine->deleteInstrumentation(p.first);
    addInstrRuleRange(p.second->range.start(), p.second->range.end(),
                      p.second->cbk, p.second->type, p.second->data);
  }

  if (memReadGateCBID != VMError::INVALID_EVENTID) {
    InstrRule *rule = engine->getInstrRule(memReadGateCBID);
    QBDI_REQUIRE_ABORT(rule != nullptr, "VM copy internal error");
    QBDI_REQUIRE_ABORT(rule->changeDataPtr(memCBInfos.get()),
                       "VM copy internal error");
  }

  if (memWriteGateCBID != VMError::INVALID_EVENTID) {
    InstrRule *rule = engine->getInstrRule(memWriteGateCBID);
    QBDI_REQUIRE_ABORT(rule != nullptr, "VM copy internal error");
    QBDI_REQUIRE_ABORT(rule->changeDataPtr(memCBInfos.get()),
                       "VM copy internal error");
  }

  vmCBData = vm.vmCBData;
  for (auto &p : vmCBData) {
    engine->setVMEventCB(p.first, VMCBLambdaProxy, &p.second);
  }

  instCBData = vm.instCBData;
  for (std::pair<uint32_t, InstCbLambda> &p : instCBData) {
    if (p.first & EVENTID_VIRTCB_MASK) {
      uint32_t id = p.first;
      auto it = std::find_if(memCBInfos->begin(), memCBInfos->end(),
                             [id](const std::pair<uint32_t, MemCBInfo> &el) {
                               return id == el.first;
                             });
      QBDI_REQUIRE_ABORT(it != memCBInfos->end(), "VM copy internal error");
      it->second.data = &p.second;
    } else {
      InstrRule *rule = engine->getInstrRule(p.first);
      QBDI_REQUIRE_ABORT(rule != nullptr, "VM copy internal error");
      QBDI_REQUIRE_ABORT(rule->changeDataPtr(&p.second),
                         "VM copy internal error");
    }
  }

  instrRuleCBData = vm.instrRuleCBData;
  for (std::pair<uint32_t, InstrRuleCbLambda> &p : instrRuleCBData) {
    InstrRule *rule = engine->getInstrRule(p.first);
    QBDI_REQUIRE_ABORT(rule != nullptr, "VM copy internal error");
    QBDI_REQUIRE_ABORT(rule->changeDataPtr(&p.second),
                       "VM copy internal error");
  }

  engine->changeVMInstanceRef(this);

  return *this;
}

// getGPRState

GPRState *VM::getGPRState() const { return engine->getGPRState(); }

// getFPRState

FPRState *VM::getFPRState() const { return engine->getFPRState(); }

// setGPRState

void VM::setGPRState(const GPRState *gprState) {
  QBDI_REQUIRE_ACTION(gprState != nullptr, return);
  engine->setGPRState(gprState);
}

// setFPRState

void VM::setFPRState(const FPRState *fprState) {
  QBDI_REQUIRE_ACTION(fprState != nullptr, return);
  engine->setFPRState(fprState);
}

// getOptions

Options VM::getOptions() const { return engine->getOptions(); }

// setOptions

void VM::setOptions(Options options) {
#if defined(_QBDI_ASAN_ENABLED_)
  options |= Options::OPT_DISABLE_FPR;
#endif
  engine->setOptions(options);
}

// addInstrumentedRange

void VM::addInstrumentedRange(rword start, rword end) {
  QBDI_REQUIRE_ACTION(strip_ptrauth(start) < strip_ptrauth(end), return);
  engine->addInstrumentedRange(strip_ptrauth(start), strip_ptrauth(end));
}

// addInstrumentedModule

bool VM::addInstrumentedModule(const std::string &name) {
  return engine->addInstrumentedModule(name);
}

// addInstrumentedModuleFromAddr

bool VM::addInstrumentedModuleFromAddr(rword addr) {
  return engine->addInstrumentedModuleFromAddr(strip_ptrauth(addr));
}

// instrumentAllExecutableMaps

bool VM::instrumentAllExecutableMaps() {
  return engine->instrumentAllExecutableMaps();
}

// removeInstrumentedRange

void VM::removeInstrumentedRange(rword start, rword end) {
  QBDI_REQUIRE_ACTION(strip_ptrauth(start) < strip_ptrauth(end), return);
  engine->removeInstrumentedRange(strip_ptrauth(start), strip_ptrauth(end));
}

// removeAllInstrumentedRanges

void VM::removeAllInstrumentedRanges() {
  engine->removeAllInstrumentedRanges();
}

// removeInstrumentedModule

bool VM::removeInstrumentedModule(const std::string &name) {
  return engine->removeInstrumentedModule(name);
}

// removeInstrumentedModuleFromAddr

bool VM::removeInstrumentedModuleFromAddr(rword addr) {
  return engine->removeInstrumentedModuleFromAddr(strip_ptrauth(addr));
}

// run

bool VM::run(rword start, rword stop) {
  uint32_t stopCB = addCodeAddrCB(strip_ptrauth(stop), InstPosition::PREINST,
                                  stopCallback, nullptr);
  bool ret = engine->run(strip_ptrauth(start), strip_ptrauth(stop));
  deleteInstrumentation(stopCB);
  return ret;
}

// callA

#define FAKE_RET_ADDR 42

bool VM::callA(rword *retval, rword function, uint32_t argNum,
               const rword *args) {
  GPRState *state = getGPRState();
  QBDI_REQUIRE_ABORT(state != nullptr, "Fail to get VM GPRState");

  // a stack pointer must be set in state
  if (QBDI_GPR_GET(state, REG_SP) == 0) {
    return false;
  }
  // push arguments in current context
  simulateCallA(state, FAKE_RET_ADDR, argNum, args);
  // call function
  bool res = run(function, FAKE_RET_ADDR);
  // get return value from current state
  if (retval != nullptr) {
    *retval = QBDI_GPR_GET(state, REG_RETURN);
  }
  return res;
}

// call

bool VM::call(rword *retval, rword function, const std::vector<rword> &args) {
  return this->callA(retval, function, args.size(), args.data());
}

// callV

bool VM::callV(rword *retval, rword function, uint32_t argNum, va_list ap) {
  std::vector<rword> args(argNum);
  for (uint32_t i = 0; i < argNum; i++) {
    args[i] = va_arg(ap, rword);
  }

  bool res = this->callA(retval, function, argNum, args.data());

  return res;
}

// switchStackAndCallA

bool VM::switchStackAndCallA(rword *retval, rword function, uint32_t argNum,
                             const rword *args, uint32_t stackSize) {

  QBDI_REQUIRE_ACTION(stackSize > 0x10000, return false);

  uint8_t *fakestack = static_cast<uint8_t *>(alignedAlloc(stackSize, 16));
  if (fakestack == nullptr) {
    return false;
  }

  bool res =
      switchStack(fakestack + stackSize - sizeof(rword), [&](rword stackPtr) {
        // add a space of one integer
        rword sp = stackPtr - sizeof(rword);
        // align SP to 16
        sp &= ~0xf;
        QBDI_GPR_SET(this->getGPRState(), REG_SP, sp);
        return this->callA(retval, function, argNum, args);
      });

  QBDI::alignedFree(fakestack);
  return res;
}

// switchStackAndCall

bool VM::switchStackAndCall(rword *retval, rword function,
                            const std::vector<rword> &args,
                            uint32_t stackSize) {
  return this->switchStackAndCallA(retval, function, args.size(), args.data(),
                                   stackSize);
}

// switchStackAndCallV

bool VM::switchStackAndCallV(rword *retval, rword function, uint32_t argNum,
                             va_list ap, uint32_t stackSize) {
  std::vector<rword> args(argNum);
  for (uint32_t i = 0; i < argNum; i++) {
    args[i] = va_arg(ap, rword);
  }

  bool res = this->switchStackAndCallA(retval, function, argNum, args.data(),
                                       stackSize);

  return res;
}

// addInstrRule

uint32_t VM::addInstrRule(InstrRuleCallback cbk, AnalysisType type,
                          void *data) {
  RangeSet<rword> r;
  r.add(Range<rword>(0, (rword)-1, real_addr_t()));
  return engine->addInstrRule(
      InstrRuleUser::unique(cbk, type, data, this, std::move(r)));
}

uint32_t VM::addInstrRule(InstrRuleCallbackC cbk, AnalysisType type,
                          void *data) {
  InstrCBInfo *_data = new InstrCBInfo{
      Range<rword>(0, (rword)-1, real_addr_t()), cbk, type, data};
  uint32_t id = addInstrRule(InstrCBGateC, type, _data);
  instrCBInfos->emplace_back(id, _data);
  return id;
}

uint32_t VM::addInstrRule(const InstrRuleCbLambda &cbk, AnalysisType type) {
  auto &el = instrRuleCBData.emplace_front(0xffffffff, cbk);
  uint32_t id = addInstrRule(InstrRuleCBLambdaProxy, type, &el.second);
  el.first = id;
  return id;
}

uint32_t VM::addInstrRule(InstrRuleCbLambda &&cbk, AnalysisType type) {
  auto &el = instrRuleCBData.emplace_front(0xffffffff, std::move(cbk));
  uint32_t id = addInstrRule(InstrRuleCBLambdaProxy, type, &el.second);
  el.first = id;
  return id;
}

// addInstrRuleRange

uint32_t VM::addInstrRuleRange(rword start, rword end, InstrRuleCallback cbk,
                               AnalysisType type, void *data) {
  RangeSet<rword> r;
  r.add(Range<rword>(start, end, auth_addr_t()));
  return engine->addInstrRule(InstrRuleUser::unique(cbk, type, data, this, r));
}

uint32_t VM::addInstrRuleRange(rword start, rword end, InstrRuleCallbackC cbk,
                               AnalysisType type, void *data) {
  InstrCBInfo *_data =
      new InstrCBInfo{Range<rword>(start, end, auth_addr_t()), cbk, type, data};
  uint32_t id = addInstrRuleRange(start, end, InstrCBGateC, type, _data);
  instrCBInfos->emplace_back(id, _data);
  return id;
}

uint32_t VM::addInstrRuleRange(rword start, rword end,
                               const InstrRuleCbLambda &cbk,
                               AnalysisType type) {
  auto &el = instrRuleCBData.emplace_front(0xffffffff, cbk);
  uint32_t id =
      addInstrRuleRange(start, end, InstrRuleCBLambdaProxy, type, &el.second);
  el.first = id;
  return id;
}

uint32_t VM::addInstrRuleRange(rword start, rword end, InstrRuleCbLambda &&cbk,
                               AnalysisType type) {
  auto &el = instrRuleCBData.emplace_front(0xffffffff, std::move(cbk));
  uint32_t id =
      addInstrRuleRange(start, end, InstrRuleCBLambdaProxy, type, &el.second);
  el.first = id;
  return id;
}

// addInstrRuleRangeSet

uint32_t VM::addInstrRuleRangeSet(RangeSet<rword> range, InstrRuleCallback cbk,
                                  AnalysisType type, void *data) {
  return engine->addInstrRule(
      InstrRuleUser::unique(cbk, type, data, this, std::move(range)));
}

uint32_t VM::addInstrRuleRangeSet(RangeSet<rword> range,
                                  const InstrRuleCbLambda &cbk,
                                  AnalysisType type) {
  auto &el = instrRuleCBData.emplace_front(0xffffffff, cbk);
  uint32_t id = addInstrRuleRangeSet(std::move(range), InstrRuleCBLambdaProxy,
                                     type, &el.second);
  el.first = id;
  return id;
}

uint32_t VM::addInstrRuleRangeSet(RangeSet<rword> range,
                                  InstrRuleCbLambda &&cbk, AnalysisType type) {
  auto &el = instrRuleCBData.emplace_front(0xffffffff, std::move(cbk));
  uint32_t id = addInstrRuleRangeSet(std::move(range), InstrRuleCBLambdaProxy,
                                     type, &el.second);
  el.first = id;
  return id;
}

// addMnemonicCB

uint32_t VM::addMnemonicCB(const char *mnemonic, InstPosition pos,
                           InstCallback cbk, void *data, int priority) {
  QBDI_REQUIRE_ACTION(mnemonic != nullptr, return VMError::INVALID_EVENTID);
  QBDI_REQUIRE_ACTION(cbk != nullptr, return VMError::INVALID_EVENTID);
  return engine->addInstrRule(InstrRuleBasicCBK::unique(
      MnemonicIs::unique(mnemonic), cbk, data, pos, true, priority,
      (pos == PREINST) ? RelocTagPreInstStdCBK : RelocTagPostInstStdCBK));
}

uint32_t VM::addMnemonicCB(const char *mnemonic, InstPosition pos,
                           const InstCbLambda &cbk, int priority) {
  auto &el = instCBData.emplace_front(0xffffffff, cbk);
  uint32_t id =
      addMnemonicCB(mnemonic, pos, InstCBLambdaProxy, &el.second, priority);
  el.first = id;
  return id;
}

uint32_t VM::addMnemonicCB(const char *mnemonic, InstPosition pos,
                           InstCbLambda &&cbk, int priority) {
  auto &el = instCBData.emplace_front(0xffffffff, std::move(cbk));
  uint32_t id =
      addMnemonicCB(mnemonic, pos, InstCBLambdaProxy, &el.second, priority);
  el.first = id;
  return id;
}

// addCodeCB

uint32_t VM::addCodeCB(InstPosition pos, InstCallback cbk, void *data,
                       int priority) {
  QBDI_REQUIRE_ACTION(cbk != nullptr, return VMError::INVALID_EVENTID);
  return engine->addInstrRule(InstrRuleBasicCBK::unique(
      True::unique(), cbk, data, pos, true, priority,
      (pos == PREINST) ? RelocTagPreInstStdCBK : RelocTagPostInstStdCBK));
}

uint32_t VM::addCodeCB(InstPosition pos, const InstCbLambda &cbk,
                       int priority) {
  auto &el = instCBData.emplace_front(0xffffffff, cbk);
  uint32_t id = addCodeCB(pos, InstCBLambdaProxy, &el.second, priority);
  el.first = id;
  return id;
}

uint32_t VM::addCodeCB(InstPosition pos, InstCbLambda &&cbk, int priority) {
  auto &el = instCBData.emplace_front(0xffffffff, std::move(cbk));
  uint32_t id = addCodeCB(pos, InstCBLambdaProxy, &el.second, priority);
  el.first = id;
  return id;
}

// addCodeAddrCB

uint32_t VM::addCodeAddrCB(rword address, InstPosition pos, InstCallback cbk,
                           void *data, int priority) {
  QBDI_REQUIRE_ACTION(cbk != nullptr, return VMError::INVALID_EVENTID);
  return engine->addInstrRule(InstrRuleBasicCBK::unique(
      AddressIs::unique(strip_ptrauth(address)), cbk, data, pos, true, priority,
      (pos == PREINST) ? RelocTagPreInstStdCBK : RelocTagPostInstStdCBK));
}

uint32_t VM::addCodeAddrCB(rword address, InstPosition pos,
                           const InstCbLambda &cbk, int priority) {
  auto &el = instCBData.emplace_front(0xffffffff, cbk);
  uint32_t id =
      addCodeAddrCB(address, pos, InstCBLambdaProxy, &el.second, priority);
  el.first = id;
  return id;
}

uint32_t VM::addCodeAddrCB(rword address, InstPosition pos, InstCbLambda &&cbk,
                           int priority) {
  auto &el = instCBData.emplace_front(0xffffffff, std::move(cbk));
  uint32_t id =
      addCodeAddrCB(address, pos, InstCBLambdaProxy, &el.second, priority);
  el.first = id;
  return id;
}

// addCodeRangeCB

uint32_t VM::addCodeRangeCB(rword start, rword end, InstPosition pos,
                            InstCallback cbk, void *data, int priority) {
  QBDI_REQUIRE_ACTION(start < end, return VMError::INVALID_EVENTID);
  QBDI_REQUIRE_ACTION(cbk != nullptr, return VMError::INVALID_EVENTID);
  return engine->addInstrRule(InstrRuleBasicCBK::unique(
      InstructionInRange::unique(strip_ptrauth(start), strip_ptrauth(end)), cbk,
      data, pos, true, priority,
      (pos == PREINST) ? RelocTagPreInstStdCBK : RelocTagPostInstStdCBK));
}

uint32_t VM::addCodeRangeCB(rword start, rword end, InstPosition pos,
                            const InstCbLambda &cbk, int priority) {
  auto &el = instCBData.emplace_front(0xffffffff, cbk);
  uint32_t id =
      addCodeRangeCB(start, end, pos, InstCBLambdaProxy, &el.second, priority);
  el.first = id;
  return id;
}

uint32_t VM::addCodeRangeCB(rword start, rword end, InstPosition pos,
                            InstCbLambda &&cbk, int priority) {
  auto &el = instCBData.emplace_front(0xffffffff, std::move(cbk));
  uint32_t id =
      addCodeRangeCB(start, end, pos, InstCBLambdaProxy, &el.second, priority);
  el.first = id;
  return id;
}

// addMemAccessCB

uint32_t VM::addMemAccessCB(MemoryAccessType type, InstCallback cbk, void *data,
                            int priority) {
  QBDI_REQUIRE_ACTION(cbk != nullptr, return VMError::INVALID_EVENTID);
  recordMemoryAccess(type);
  switch (type) {
    case MEMORY_READ:
      return engine->addInstrRule(InstrRuleBasicCBK::unique(
          DoesReadAccess::unique(), cbk, data, InstPosition::PREINST, true,
          priority, RelocTagPreInstStdCBK));
    case MEMORY_WRITE:
      return engine->addInstrRule(InstrRuleBasicCBK::unique(
          DoesWriteAccess::unique(), cbk, data, InstPosition::POSTINST, true,
          priority, RelocTagPostInstStdCBK));
    case MEMORY_READ_WRITE:
      return engine->addInstrRule(InstrRuleBasicCBK::unique(
          Or::unique(conv_unique<PatchCondition>(DoesReadAccess::unique(),
                                                 DoesWriteAccess::unique())),
          cbk, data, InstPosition::POSTINST, true, priority,
          RelocTagPostInstStdCBK));
    default:
      return VMError::INVALID_EVENTID;
  }
}

uint32_t VM::addMemAccessCB(MemoryAccessType type, const InstCbLambda &cbk,
                            int priority) {
  auto &el = instCBData.emplace_front(0xffffffff, cbk);
  uint32_t id = addMemAccessCB(type, InstCBLambdaProxy, &el.second, priority);
  el.first = id;
  return id;
}

uint32_t VM::addMemAccessCB(MemoryAccessType type, InstCbLambda &&cbk,
                            int priority) {
  auto &el = instCBData.emplace_front(0xffffffff, std::move(cbk));
  uint32_t id = addMemAccessCB(type, InstCBLambdaProxy, &el.second, priority);
  el.first = id;
  return id;
}

// addMemAddrCB

uint32_t VM::addMemAddrCB(rword address, MemoryAccessType type,
                          InstCallback cbk, void *data) {
  QBDI_REQUIRE_ACTION(cbk != nullptr, return VMError::INVALID_EVENTID);
  return addMemRangeCB(address, address + 1, type, cbk, data);
}

uint32_t VM::addMemAddrCB(rword address, MemoryAccessType type,
                          const InstCbLambda &cbk) {
  auto &el = instCBData.emplace_front(0xffffffff, cbk);
  uint32_t id = addMemAddrCB(address, type, InstCBLambdaProxy, &el.second);
  el.first = id;
  return id;
}

uint32_t VM::addMemAddrCB(rword address, MemoryAccessType type,
                          InstCbLambda &&cbk) {
  auto &el = instCBData.emplace_front(0xffffffff, std::move(cbk));
  uint32_t id = addMemAddrCB(address, type, InstCBLambdaProxy, &el.second);
  el.first = id;
  return id;
}

// addMemRangeCB

uint32_t VM::addMemRangeCB(rword start_, rword end_, MemoryAccessType type,
                           InstCallback cbk, void *data) {
  rword start = strip_ptrauth(start_);
  rword end = strip_ptrauth(end_);
  QBDI_REQUIRE_ACTION(start < end, return VMError::INVALID_EVENTID);
  QBDI_REQUIRE_ACTION(type & MEMORY_READ_WRITE,
                      return VMError::INVALID_EVENTID);
  QBDI_REQUIRE_ACTION(cbk != nullptr, return VMError::INVALID_EVENTID);
  recordMemoryAccess(type);
  if ((type == MEMORY_READ) && memReadGateCBID == VMError::INVALID_EVENTID) {
    memReadGateCBID = engine->addInstrRule(InstrRuleBasicCBK::unique(
        DoesReadAccess::unique(), memReadGate, memCBInfos.get(),
        InstPosition::PREINST, true, 0, RelocTagPreInstStdCBK));
  }
  if ((type & MEMORY_WRITE) && memWriteGateCBID == VMError::INVALID_EVENTID) {
    // memWriteGate manage MEMORY_WRITE and MEMORY_READ_WRITE callback
    memWriteGateCBID = engine->addInstrRule(InstrRuleBasicCBK::unique(
        Or::unique(conv_unique<PatchCondition>(DoesReadAccess::unique(),
                                               DoesWriteAccess::unique())),
        memWriteGate, memCBInfos.get(), InstPosition::POSTINST, true, 0,
        RelocTagPostInstStdCBK));
  }
  uint32_t id = memCBID++;
  QBDI_REQUIRE_ACTION(id < EVENTID_VIRTCB_MASK,
                      return VMError::INVALID_EVENTID);
  memCBInfos->emplace_back(
      id | EVENTID_VIRTCB_MASK,
      MemCBInfo{type, {start, end, real_addr_t()}, cbk, data});
  return id | EVENTID_VIRTCB_MASK;
}

uint32_t VM::addMemRangeCB(rword start, rword end, MemoryAccessType type,
                           const InstCbLambda &cbk) {
  auto &el = instCBData.emplace_front(0xffffffff, cbk);
  uint32_t id = addMemRangeCB(start, end, type, InstCBLambdaProxy, &el.second);
  el.first = id;
  return id;
}

uint32_t VM::addMemRangeCB(rword start, rword end, MemoryAccessType type,
                           InstCbLambda &&cbk) {
  auto &el = instCBData.emplace_front(0xffffffff, std::move(cbk));
  uint32_t id = addMemRangeCB(start, end, type, InstCBLambdaProxy, &el.second);
  el.first = id;
  return id;
}

// addVMEventCB

uint32_t VM::addVMEventCB(VMEvent mask, VMCallback cbk, void *data) {
  QBDI_REQUIRE_ACTION(mask != 0, return VMError::INVALID_EVENTID);
  QBDI_REQUIRE_ACTION(cbk != nullptr, return VMError::INVALID_EVENTID);
  return engine->addVMEventCB(mask, cbk, data);
}

uint32_t VM::addVMEventCB(VMEvent mask, const VMCbLambda &cbk) {
  auto &el = vmCBData.emplace_front(0xffffffff, cbk);
  uint32_t id = addVMEventCB(mask, VMCBLambdaProxy, &el.second);
  el.first = id;
  return id;
}

uint32_t VM::addVMEventCB(VMEvent mask, VMCbLambda &&cbk) {
  auto &el = vmCBData.emplace_front(0xffffffff, std::move(cbk));
  uint32_t id = addVMEventCB(mask, VMCBLambdaProxy, &el.second);
  el.first = id;
  return id;
}

// deleteInstrumentation

bool VM::deleteInstrumentation(uint32_t id) {
  if (id & EVENTID_VIRTCB_MASK) {
    auto found = std::remove_if(memCBInfos->begin(), memCBInfos->end(),
                                [id](const std::pair<uint32_t, MemCBInfo> &el) {
                                  return id == el.first;
                                });
    if (found == memCBInfos->end()) {
      return false;
    }

    memCBInfos->erase(found, memCBInfos->end());
    instCBData.remove_if([id](const std::pair<uint32_t, InstCbLambda> &x) {
      return x.first == id;
    });
    return true;
  } else {
    instrCBInfos->erase(
        std::remove_if(
            instrCBInfos->begin(), instrCBInfos->end(),
            [id](const std::pair<uint32_t, std::unique_ptr<InstrCBInfo>> &x) {
              return x.first == id;
            }),
        instrCBInfos->end());
    vmCBData.remove_if([id](const std::pair<uint32_t, VMCbLambda> &x) {
      return x.first == id;
    });
    instCBData.remove_if([id](const std::pair<uint32_t, InstCbLambda> &x) {
      return x.first == id;
    });
    instrRuleCBData.remove_if(
        [id](const std::pair<uint32_t, InstrRuleCbLambda> &x) {
          return x.first == id;
        });
    return engine->deleteInstrumentation(id);
  }
}

// deleteAllInstrumentations

void VM::deleteAllInstrumentations() {
  engine->deleteAllInstrumentations();
  memReadGateCBID = VMError::INVALID_EVENTID;
  memWriteGateCBID = VMError::INVALID_EVENTID;
  memCBInfos->clear();
  instrCBInfos->clear();
  vmCBData.clear();
  instCBData.clear();
  instrRuleCBData.clear();
  memoryLoggingLevel = 0;
}

// getInstAnalysis

const InstAnalysis *VM::getInstAnalysis(AnalysisType type) const {
  const ExecBlock *curExecBlock = engine->getCurExecBlock();
  QBDI_REQUIRE_ACTION(curExecBlock != nullptr, return nullptr);
  uint16_t curInstID = curExecBlock->getCurrentInstID();
  return curExecBlock->getInstAnalysis(curInstID, type);
}

// getCachedInstAnalysis

const InstAnalysis *VM::getCachedInstAnalysis(rword address,
                                              AnalysisType type) const {
  return engine->getInstAnalysis(strip_ptrauth(address), type);
}

// getJITInstAnalysis
const InstAnalysis *VM::getJITInstAnalysis(rword jitAddress,
                                           AnalysisType type) const {
  auto info = engine->getPatchInfoOfJit(strip_ptrauth(jitAddress));
  if (info and info->second != NOT_FOUND) {
    return info->first->getInstAnalysis(info->second, type);
  } else {
    return nullptr;
  }
}

// recordMemoryAccess

bool VM::recordMemoryAccess(MemoryAccessType type) {
  if (type & MEMORY_READ && !(memoryLoggingLevel & MEMORY_READ)) {
    memoryLoggingLevel |= MEMORY_READ;
    for (auto &r : getInstrRuleMemAccessRead()) {
      engine->addInstrRule(std::move(r));
    }
  }
  if (type & MEMORY_WRITE && !(memoryLoggingLevel & MEMORY_WRITE)) {
    memoryLoggingLevel |= MEMORY_WRITE;
    for (auto &r : getInstrRuleMemAccessWrite()) {
      engine->addInstrRule(std::move(r));
    }
  }
  return true;
}

// getInstMemoryAccess

std::vector<MemoryAccess> VM::getInstMemoryAccess() const {
  const ExecBlock *curExecBlock = engine->getCurExecBlock();
  if (curExecBlock == nullptr) {
    return {};
  }
  uint16_t instID = curExecBlock->getCurrentInstID();
  std::vector<MemoryAccess> memAccess;
  analyseMemoryAccess(*curExecBlock, instID, !engine->isPreInst(), memAccess);
  return memAccess;
}

// getBBMemoryAccess

std::vector<MemoryAccess> VM::getBBMemoryAccess() const {
  const ExecBlock *curExecBlock = engine->getCurExecBlock();
  if (curExecBlock == nullptr) {
    return {};
  }
  uint16_t bbID = curExecBlock->getCurrentSeqID();
  uint16_t instID = curExecBlock->getCurrentInstID();
  std::vector<MemoryAccess> memAccess;
  QBDI_DEBUG(
      "Search MemoryAccess for Basic Block {:x} stopping at Instruction {:x}",
      bbID, instID);

  uint16_t endInstID = curExecBlock->getSeqEnd(bbID);
  for (uint16_t itInstID = curExecBlock->getSeqStart(bbID);
       itInstID <= std::min(endInstID, instID); itInstID++) {

    analyseMemoryAccess(*curExecBlock, itInstID,
                        itInstID != instID || !engine->isPreInst(), memAccess);
  }
  return memAccess;
}

// precacheBasicBlock

bool VM::precacheBasicBlock(rword pc) {
  return engine->precacheBasicBlock(strip_ptrauth(pc));
}

// clearAllCache

void VM::clearAllCache() { engine->clearAllCache(); }

// clearCache

void VM::clearCache(rword start, rword end) {
  engine->clearCache(strip_ptrauth(start), strip_ptrauth(end));
}

// getNbExecBlock

uint32_t VM::getNbExecBlock() const { return engine->getNbExecBlock(); }

// reduceCacheTo

void VM::reduceCacheTo(uint32_t nb) { engine->reduceCacheTo(nb); }

} // namespace QBDI
