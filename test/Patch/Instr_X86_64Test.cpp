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
#include "Instr_X86_64Test.h"

QBDI::VMAction increment(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    *((uint64_t*) data) = *((uint64_t*) data) + 1;
    return QBDI::VMAction::CONTINUE;
}

TEST_CASE_METHOD(Instr_X86_64Test, "Instr_X86_64Test-GPRSave_IC") {
    uint64_t count1 = 0;
    uint64_t count2 = 0;

    QBDI::Context inputState;
    memset(&inputState, 0, sizeof(QBDI::Context));

    vm.deleteAllInstrumentations();
    vm.addCodeCB(QBDI::PREINST, increment, (void*) &count1);
    vm.addCodeCB(QBDI::POSTINST, increment, (void*) &count2);

    comparedExec(GPRSave_s, inputState, 4096);

    REQUIRE((uint64_t) 0 < count1);
    REQUIRE(count1 == count2);

    INFO("Took " << count1 << " instructions");
}

TEST_CASE_METHOD(Instr_X86_64Test, "Instr_X86_64Test-GPRShuffle_IC") {
    uint64_t count1 = 0;
    uint64_t count2 = 0;

    QBDI::Context inputState;
    memset(&inputState, 0, sizeof(QBDI::Context));
    for(uint32_t i = 0; i < QBDI::AVAILABLE_GPR; i++)
        QBDI_GPR_SET(&inputState.gprState, i, i);

    vm.deleteAllInstrumentations();
    vm.addCodeCB(QBDI::PREINST, increment, (void*) &count1);
    vm.addCodeCB(QBDI::POSTINST, increment, (void*) &count2);

    comparedExec(GPRShuffle_s, inputState, 4096);

    REQUIRE((uint64_t) 0 < count1);
    REQUIRE(count1 == count2);

    INFO("Took " << count1 << " instructions");
}

TEST_CASE_METHOD(Instr_X86_64Test, "Instr_X86_64Test-RelativeAddressing_IC") {
    uint64_t count1 = 0;
    uint64_t count2 = 0;

    QBDI::Context inputState;
    memset(&inputState, 0, sizeof(QBDI::Context));
#if defined(QBDI_ARCH_X86)
    inputState.gprState.eax = (QBDI::rword)rand();
    inputState.gprState.ebx = (QBDI::rword)rand();
#else
    inputState.gprState.rax = ((QBDI::rword)rand() << 32) | rand();
    inputState.gprState.rbx = ((QBDI::rword)rand() << 32) | rand();
#endif

    vm.deleteAllInstrumentations();
    vm.addCodeCB(QBDI::PREINST, increment, (void*) &count1);
    vm.addCodeCB(QBDI::POSTINST, increment, (void*) &count2);

    comparedExec(RelativeAddressing_s, inputState, 4096);

    REQUIRE((uint64_t) 0 < count1);
    REQUIRE(count1 == count2);

    INFO("Took " << count1 << " instructions");
}

TEST_CASE_METHOD(Instr_X86_64Test, "Instr_X86_64Test-ConditionalBranching_IC") {
    uint64_t count1 = 0;
    uint64_t count2 = 0;

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

    vm.deleteAllInstrumentations();
    vm.addCodeCB(QBDI::PREINST, increment, (void*) &count1);
    vm.addCodeCB(QBDI::POSTINST, increment, (void*) &count2);

    comparedExec(ConditionalBranching_s, inputState, 4096);

    REQUIRE((uint64_t) 0 < count1);
    REQUIRE(count1 == count2);

    INFO("Took " << count1 << " instructions");
}

TEST_CASE_METHOD(Instr_X86_64Test, "Instr_X86_64Test-FibonacciRecursion_IC") {
    uint64_t count1 = 0;
    uint64_t count2 = 0;

    QBDI::Context inputState;
    memset(&inputState, 0, sizeof(QBDI::Context));
#if defined(QBDI_ARCH_X86)
    inputState.gprState.eax = (QBDI::rword) (rand() % 20) + 2;
#else
    inputState.gprState.rax = (QBDI::rword) (rand() % 20) + 2;
#endif

    vm.deleteAllInstrumentations();
    vm.addCodeCB(QBDI::PREINST, increment, (void*) &count1);
    vm.addCodeCB(QBDI::POSTINST, increment, (void*) &count2);

    comparedExec(FibonacciRecursion_s, inputState, 4096);

    REQUIRE((uint64_t) 0 < count1);
    REQUIRE(count1 == count2);

    INFO("Took " << count1 << " instructions");
}

TEST_CASE_METHOD(Instr_X86_64Test, "Instr_X86_64Test-StackTricks_IC") {

    uint64_t count1 = 0;
    uint64_t count2 = 0;

    QBDI::Context inputState;
    memset(&inputState, 0, sizeof(QBDI::Context));
#if defined(QBDI_ARCH_X86)
    inputState.gprState.eax = (QBDI::rword) (rand() % 20) + 2;
#else
    inputState.gprState.rax = (QBDI::rword) (rand() % 20) + 2;
#endif

    vm.deleteAllInstrumentations();
    vm.addCodeCB(QBDI::PREINST, increment, (void*) &count1);
    vm.addCodeCB(QBDI::POSTINST, increment, (void*) &count2);

    comparedExec(StackTricks_s, inputState, 4096);

    REQUIRE((uint64_t) 0 < count1);
    REQUIRE(count1 == count2);

    INFO("Took " << count1 << " instructions");
}

#if defined(QBDI_ARCH_X86_64)
// not implemented for X86
TEST_CASE_METHOD(Instr_X86_64Test, "Instr_X86_64Test-UnalignedCodeForward_IC") {
    uint64_t count1 = 0;
    uint64_t count2 = 0;

    QBDI::Context inputState;
    memset(&inputState, 0, sizeof(QBDI::Context));

    vm.deleteAllInstrumentations();
    vm.addCodeCB(QBDI::PREINST, increment, (void*) &count1);
    vm.addCodeCB(QBDI::POSTINST, increment, (void*) &count2);

    comparedExec(UnalignedCodeForward_s, inputState, 4096);

    REQUIRE((uint64_t) 0 < count1);
    REQUIRE(count1 == count2);

    INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_X86_64Test, "Instr_X86_64Test-UnalignedCodeBackward_IC") {
    uint64_t count1 = 0;
    uint64_t count2 = 0;

    QBDI::Context inputState;
    memset(&inputState, 0, sizeof(QBDI::Context));

    vm.deleteAllInstrumentations();
    vm.addCodeCB(QBDI::PREINST, increment, (void*) &count1);
    vm.addCodeCB(QBDI::POSTINST, increment, (void*) &count2);

    comparedExec(UnalignedCodeBackward_s, inputState, 4096);

    REQUIRE((uint64_t) 0 < count1);
    REQUIRE(count1 == count2);

    INFO("Took " << count1 << " instructions\n");
}
#endif
