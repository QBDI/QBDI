/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2026 Quarkslab
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

#include <catch2/catch_test_macros.hpp>
#include "API/APITest.h"

static constexpr QBDI::rword ZF = (1 << 6);

static constexpr char JE_SHORT_SRC[] =
    "je je_short_landed\n"
    "movq $0x1111, %rax\n"
    "jmp je_short_end\n"
    "je_short_landed:\n"
    "movq $0x2222, %rax\n"
    "je_short_end:\n";

static constexpr char JE_NEAR_SRC[] =
    "je je_near_landed\n"
    "movq $0x1111, %rax\n"
    "jmp je_near_end\n"
    ".fill 200, 1, 0x90\n"
    "je_near_landed:\n"
    "movq $0x2222, %rax\n"
    "je_near_end:\n";

static constexpr char JRCXZ_SRC[] =
    "jrcxz jrcxz_landed\n"
    "movq $0x1111, %rax\n"
    "jmp jrcxz_end\n"
    "jrcxz_landed:\n"
    "movq $0x2222, %rax\n"
    "jrcxz_end:\n";

static constexpr char JECXZ_SRC[] =
    "jecxz jecxz_landed\n"
    "movq $0x1111, %rax\n"
    "jmp jecxz_end\n"
    "jecxz_landed:\n"
    "movq $0x2222, %rax\n"
    "jecxz_end:\n";

static constexpr char LOOP_TAKEN_SRC[] =
    "movq $0, %rdx\n"
    "loop_top:\n"
    "incq %rdx\n"
    "loop loop_top\n"
    "movq %rdx, %rax\n";

static constexpr char LOOP_NOT_TAKEN_SRC[] =
    "loop loop_nt_after\n"
    "movq $0x1111, %rax\n"
    "jmp loop_nt_end\n"
    "loop_nt_after:\n"
    "movq $0x2222, %rax\n"
    "loop_nt_end:\n";

static constexpr char LOOPE_TAKEN_SRC[] =
    "movq $0, %rdx\n"
    "cmpq %rdx, %rdx\n"
    "loope_top:\n"
    "leaq 1(%rdx), %rdx\n"
    "loope loope_top\n"
    "movq %rdx, %rax\n";

static constexpr char LOOPE_NOT_TAKEN_SRC[] =
    "movq $1, %rdx\n"
    "cmpq $0, %rdx\n"
    "loope loope_nt_after\n"
    "movq $0x1111, %rax\n"
    "jmp loope_nt_end\n"
    "loope_nt_after:\n"
    "movq $0x2222, %rax\n"
    "loope_nt_end:\n";

static constexpr char LOOPNE_TAKEN_SRC[] =
    "movq $1, %rdx\n"
    "cmpq $0, %rdx\n"
    "loopne_top:\n"
    "leaq 1(%rdx), %rdx\n"
    "loopne loopne_top\n"
    "movq %rdx, %rax\n";

static constexpr char LOOPNE_NOT_TAKEN_SRC[] =
    "movq $0, %rdx\n"
    "cmpq %rdx, %rdx\n"
    "loopne loopne_nt_after\n"
    "movq $0x1111, %rax\n"
    "jmp loopne_nt_end\n"
    "loopne_nt_after:\n"
    "movq $0x2222, %rax\n"
    "loopne_nt_end:\n";

static void checkCondJump(APITest &fixture, const char *mnemonic,
                          const char *source, bool taken) {
  bool seen = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB(mnemonic, QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB(mnemonic, QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  if (taken) {
    state->eflags |= ZF;
  } else {
    state->eflags &= ~ZF;
  }
  fixture.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkZeroRegJump(APITest &fixture, const char *mnemonic,
                             const char *source, QBDI::rword rcxTaken,
                             QBDI::rword rcxNotTaken, bool taken) {
  bool seen = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB(mnemonic, QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB(mnemonic, QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->rcx = taken ? rcxTaken : rcxNotTaken;
  fixture.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkLoopFamily(APITest &fixture, const char *mnemonic,
                            const char *takenSource, const char *notTakenSource,
                            QBDI::rword rcxTaken, QBDI::rword rcxNotTaken,
                            QBDI::rword rcxEndTaken, QBDI::rword rcxEndNotTaken,
                            QBDI::rword retvalTaken, QBDI::rword retvalNotTaken,
                            bool taken) {
  size_t seenCount = 0;
  size_t seenCountPost = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB(mnemonic, QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenCount++;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB(mnemonic, QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenCountPost++;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->rcx = taken ? rcxTaken : rcxNotTaken;
  fixture.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, taken ? takenSource : notTakenSource);

  CHECK(ran);
  CHECK(seenCount == (taken ? 3u : 1u));
  CHECK(seenCountPost == (taken ? 3u : 1u));
  CHECK(state->rcx == (taken ? rcxEndTaken : rcxEndNotTaken));
  CHECK(retval == (taken ? retvalTaken : retvalNotTaken));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-je_short_taken") {
  checkCondJump(*this, "JCC_1", JE_SHORT_SRC, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-je_short_not_taken") {
  checkCondJump(*this, "JCC_1", JE_SHORT_SRC, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-je_near_taken") {
  checkCondJump(*this, "JCC_4", JE_NEAR_SRC, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-je_near_not_taken") {
  checkCondJump(*this, "JCC_4", JE_NEAR_SRC, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-loop_taken") {
  checkLoopFamily(*this, "LOOP", LOOP_TAKEN_SRC, LOOP_NOT_TAKEN_SRC, 3, 1, 0, 0,
                  3, 0x1111, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-loop_not_taken") {
  checkLoopFamily(*this, "LOOP", LOOP_TAKEN_SRC, LOOP_NOT_TAKEN_SRC, 3, 1, 0, 0,
                  3, 0x1111, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-jrcxz_taken") {
  checkZeroRegJump(*this, "JRCXZ", JRCXZ_SRC, 0, 5, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-jrcxz_not_taken") {
  checkZeroRegJump(*this, "JRCXZ", JRCXZ_SRC, 0, 5, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-loope_taken") {
  checkLoopFamily(*this, "LOOPE", LOOPE_TAKEN_SRC, LOOPE_NOT_TAKEN_SRC, 3, 5, 0,
                  4, 3, 0x1111, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-loope_not_taken") {
  checkLoopFamily(*this, "LOOPE", LOOPE_TAKEN_SRC, LOOPE_NOT_TAKEN_SRC, 3, 5, 0,
                  4, 3, 0x1111, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-loopne_taken") {
  checkLoopFamily(*this, "LOOPNE", LOOPNE_TAKEN_SRC, LOOPNE_NOT_TAKEN_SRC, 3, 5,
                  0, 4, 4, 0x1111, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-loopne_not_taken") {
  checkLoopFamily(*this, "LOOPNE", LOOPNE_TAKEN_SRC, LOOPNE_NOT_TAKEN_SRC, 3, 5,
                  0, 4, 4, 0x1111, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-jecxz_taken") {
  checkZeroRegJump(*this, "JECXZ", JECXZ_SRC, 0xdeadbeef00000000ULL, 5, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-jecxz_not_taken") {
  checkZeroRegJump(*this, "JECXZ", JECXZ_SRC, 0xdeadbeef00000000ULL, 5, false);
}
