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
#ifndef EXECBROKER_H
#define EXECBROKER_H

#include <string>

#include "llvm/Support/Process.h"

#include "Memory.hpp"
#include "Range.h"
#include "State.h"
#include "ExecBlock/ExecBlock.h"

namespace QBDI {
class Assembly;

class ExecBroker {

private:

    RangeSet<rword>        instrumented;
    ExecBlock              transferBlock;
    rword                  pageSize;

    using PF = llvm::sys::Memory::ProtectionFlags;

    // ARCH dependant method
    rword *getReturnPoint(GPRState* gprState) const;

public:

    ExecBroker(const Assembly& assembly, VMInstanceRef vminstance = nullptr);

    void changeVMInstanceRef(VMInstanceRef vminstance);

    bool isInstrumented(rword addr) const { return instrumented.contains(addr);}

    void setInstrumentedRange(const RangeSet<rword>& r) {
      instrumented = r;
    }

    const RangeSet<rword>& getInstrumentedRange() const {
      return instrumented;
    }

    void addInstrumentedRange(const Range<rword>& r);
    bool addInstrumentedModule(const std::string& name);
    bool addInstrumentedModuleFromAddr(rword addr);

    void removeInstrumentedRange(const Range<rword>& r);
    bool removeInstrumentedModule(const std::string& name);
    bool removeInstrumentedModuleFromAddr(rword addr);
    void removeAllInstrumentedRanges();

    bool instrumentAllExecutableMaps();

    bool canTransferExecution(GPRState* gprState) const;

    bool transferExecution(rword addr, GPRState *gprState, FPRState *fprState);
};

}

#endif // EXECBROKER_H
