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
#ifndef COMPAREDEXECUTOR_ARM_H
#define COMPAREDEXECUTOR_ARM_H

#include <string.h>
#include <string>
#include <sstream>

#include "TestSetup/ShellcodeTester.h"
#include "TestSetup/InMemoryAssembler.h"

#define CPU_CPU "cortex-a9"
#define CPU_MATTRS "vfp2"

extern const char* GPRSave_s;
extern const char* GPRShuffle_s;
extern const char* RelativeAddressing_s;
extern const char* ConditionalBranching_s;


class ComparedExecutor_ARM : public ShellcodeTester {

public:
    ComparedExecutor_ARM() : ShellcodeTester(CPU_CPU, {CPU_MATTRS}) {}

    QBDI::Context jitExec(llvm::ArrayRef<uint8_t> code, QBDI::Context &inputCtx,
                          llvm::sys::MemoryBlock &stack);

    QBDI::Context realExec(llvm::ArrayRef<uint8_t> code, QBDI::Context &inputCtx,
                           llvm::sys::MemoryBlock &stack);

    InMemoryObject compileWithContextSwitch(const char* source);
};

#endif
