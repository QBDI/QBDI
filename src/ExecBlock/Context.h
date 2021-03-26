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
#ifndef CONTEXT_H
#define CONTEXT_H

#include <map>
#include "State.h"

namespace QBDI {

extern const unsigned int GPR_ID[];
extern const unsigned int FLAG_ID[];
extern const unsigned int SEG_ID[];
extern const std::map<unsigned int, int16_t> FPR_ID;
extern const unsigned int size_GPR_ID;
extern const unsigned int size_FLAG_ID;
extern const unsigned int size_SEG_ID;

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
// ============================================================================
// X86_64 Context
// ============================================================================

/*! X86 / X86_64 Host context.
 */
struct HostState {
    rword sp;
    rword selector;
    rword callback;
    rword data;
    rword origin;
    rword executeFlags;
};

/*! X86 / X86_64 Execution context.
 */
struct Context {

public:

    // fprState needs to be first for memory alignement reasons
    FPRState fprState;
    GPRState gprState;
    HostState hostState;

};

#elif defined(QBDI_ARCH_ARM)
// ============================================================================
// ARM Context
// ============================================================================


/*! ARM Host context.
*/
struct HostState {
    rword fp;
    rword sp;
    rword selector;
    rword callback;
    rword data;
    rword origin;
    rword executeFlags;
};

/*! ARM Execution context.
*/
struct Context {

public:

    // hostState needs to be first for relative addressing range reasons
    HostState hostState;
    FPRState fprState;
    GPRState gprState;
};

#endif // QBDI_ARCH_ARM

} // QBDI

#endif //CONTEXT_H
