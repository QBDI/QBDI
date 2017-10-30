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
#include "Patch/ARM/InstrRules_ARM.h"

namespace QBDI {

/* Genreate a series of RelocatableInst which when appended to an instrumentation code trigger a 
 * break to host. It receive in argument a temporary reg which will be used for computations then 
 * finally restored.
*/
RelocatableInst::SharedPtrVec getBreakToHost(Reg temp) {
    RelocatableInst::SharedPtrVec breakToHost;

    // Use the temporary register to compute PC + 16 which is the address which will follow this 
    // patch and where the execution needs to be resumed
    breakToHost.push_back(HostPCRel(ldri12(temp, Reg(REG_PC), 0), 2, 16));
    // Set the selector to this address so the execution can be resumed when the exec block will be 
    // reexecuted
    append(breakToHost, SaveReg(temp, Offset(offsetof(Context, hostState.selector))));
    // Restore the temporary register
    append(breakToHost, LoadReg(temp, Offset(temp)));
    // Jump to the epilogue to break to the host
    append(breakToHost, JmpEpilogue());

    return breakToHost;
}

}
