/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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
#ifndef QBDI_VM_INTERNAL_H_
#define QBDI_VM_INTERNAL_H_

#include "QBDI/VM.h"

namespace QBDI {

struct MemCBInfo {
  MemoryAccessType type;
  Range<rword> range;
  InstCallback cbk;
  void *data;
};

struct InstrCBInfo {
  Range<rword> range;
  InstrRuleCallbackC cbk;
  AnalysisType type;
  void *data;
};

VMAction memReadGate(VMInstanceRef vm, GPRState *gprState, FPRState *fprState,
                     void *data);

VMAction memWriteGate(VMInstanceRef vm, GPRState *gprState, FPRState *fprState,
                      void *data);

std::vector<InstrRuleDataCBK>
InstrCBGateC(VMInstanceRef vm, const InstAnalysis *inst, void *_data);

VMAction VMCBLambdaProxy(VMInstanceRef vm, const VMState *vmState,
                         GPRState *gprState, FPRState *fprState, void *_data);
VMAction InstCBLambdaProxy(VMInstanceRef vm, GPRState *gprState,
                           FPRState *fprState, void *_data);

std::vector<InstrRuleDataCBK>
InstrRuleCBLambdaProxy(VMInstanceRef vm, const InstAnalysis *ana, void *_data);

VMAction stopCallback(VMInstanceRef vm, GPRState *gprState, FPRState *fprState,
                      void *data);

} // namespace QBDI

#endif // QBDI_VM_INTERNAL_H_
