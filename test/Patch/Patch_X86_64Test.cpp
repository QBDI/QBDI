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
#include <catch2/catch.hpp>
#include "Patch/Patch_X86_64Test.h"

TEST_CASE_METHOD(Patch_X86_64Test, "Patch_X86_64Test-EmptyFunction") {
    QBDI::Context inputState;

    memset(&inputState, 0, sizeof(QBDI::Context));
    comparedExec("", inputState, 4096);
}

TEST_CASE_METHOD(Patch_X86_64Test, "Patch_X86_64Test-GPRSave") {
    QBDI::Context inputState;

    memset(&inputState, 0, sizeof(QBDI::Context));
    comparedExec(GPRSave_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_X86_64Test, "Patch_X86_64Test-GPRShuffle") {
    QBDI::Context inputState;

    memset(&inputState, 0, sizeof(QBDI::Context));
    for(uint32_t i = 0; i < QBDI::AVAILABLE_GPR; i++)
        QBDI_GPR_SET(&inputState.gprState, i, i);
    comparedExec(GPRShuffle_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_X86_64Test, "Patch_X86_64Test-RelativeAddressing") {
    QBDI::Context inputState;

    memset(&inputState, 0, sizeof(QBDI::Context));
#if defined(QBDI_ARCH_X86)
    inputState.gprState.eax = (QBDI::rword)rand();
    inputState.gprState.ebx = (QBDI::rword)rand();
#else
    inputState.gprState.rax = ((QBDI::rword)rand() << 32) | rand();
    inputState.gprState.rbx = ((QBDI::rword)rand() << 32) | rand();
#endif
    comparedExec(RelativeAddressing_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_X86_64Test, "Patch_X86_64Test-ConditionalBranching") {
    QBDI::Context inputState;

    memset(&inputState, 0, sizeof(QBDI::Context));
#if defined(QBDI_ARCH_X86)
    inputState.gprState.eax = (QBDI::rword)rand();
    inputState.gprState.ebx = (QBDI::rword)rand();
    inputState.gprState.ecx = (QBDI::rword)rand();
    inputState.gprState.edx = (QBDI::rword)rand();
#else
    inputState.gprState.rax = ((QBDI::rword)rand() << 32) | rand();
    inputState.gprState.rbx = ((QBDI::rword)rand() << 32) | rand();
    inputState.gprState.rcx = ((QBDI::rword)rand() << 32) | rand();
    inputState.gprState.rdx = ((QBDI::rword)rand() << 32) | rand();
#endif
    comparedExec(ConditionalBranching_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_X86_64Test, "Patch_X86_64Test-FibonacciRecursion") {
    QBDI::Context inputState;

    memset(&inputState, 0, sizeof(QBDI::Context));
#if defined(QBDI_ARCH_X86)
    inputState.gprState.eax = (QBDI::rword) (rand() % 20) + 2;
#else
    inputState.gprState.rax = (QBDI::rword) (rand() % 20) + 2;
#endif
    comparedExec(FibonacciRecursion_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_X86_64Test, "Patch_X86_64Test-StackTricks") {
    QBDI::Context inputState;

    memset(&inputState, 0, sizeof(QBDI::Context));
#if defined(QBDI_ARCH_X86)
    inputState.gprState.eax = (QBDI::rword) (rand() % 20) + 2;
#else
    inputState.gprState.rax = (QBDI::rword) (rand() % 20) + 2;
#endif
    comparedExec(StackTricks_s, inputState, 4096);
}

#if defined(QBDI_ARCH_X86_64)
// not implemented for X86
TEST_CASE_METHOD(Patch_X86_64Test, "Patch_X86_64Test-UnalignedCodeForward") {
    QBDI::Context inputState;

    memset(&inputState, 0, sizeof(QBDI::Context));
    comparedExec(UnalignedCodeForward_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_X86_64Test, "Patch_X86_64Test-UnalignedCodeBackward") {
    QBDI::Context inputState;

    memset(&inputState, 0, sizeof(QBDI::Context));
    comparedExec(UnalignedCodeBackward_s, inputState, 4096);
}
#endif
