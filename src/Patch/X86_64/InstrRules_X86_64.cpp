/*
 * This file is part of QBDI.
 *
 * Copyright 2017 Quarkslab
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
#include "Patch/X86_64/InstrRules_X86_64.h"

namespace QBDI {

/* Genreate a series of RelocatableInst which when appended to an instrumentation code trigger a 
 * break to host. It receive in argument a temporary reg which will be used for computations then 
 * finally restored.
*/
RelocatableInst::SharedPtrVec getBreakToHost(Reg temp, CPUMode cpuMode) {
    RelocatableInst::SharedPtrVec breakToHost;

    // Use the temporary register to compute RIP + 29 which is the address which will follow this 
    // patch and where the execution needs to be resumed 
    breakToHost.push_back(HostPCRel(mov64ri(temp, 0), 1, 29));
    // Set the selector to this address so the execution can be resumed when the exec block will be 
    // reexecuted
    append(breakToHost, SaveReg(temp, Offset(offsetof(Context, hostState.selector))).generate(cpuMode));
    // Restore the temporary register
    append(breakToHost, LoadReg(temp, Offset(temp)).generate(cpuMode));
    // Jump to the epilogue to break to the host
    append(breakToHost, JmpEpilogue().generate(cpuMode));

    return breakToHost;
}

std::vector<std::shared_ptr<InstrRule>> getMemAccessInstrRules() {
    // TODO: Insert here memory access rules
    return {};
}

}
