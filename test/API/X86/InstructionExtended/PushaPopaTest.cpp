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

#include <vector>

#include "QBDI/Memory.hpp"
#include "QBDI/Platform.h"
#include "QBDI/Range.h"

namespace {

struct AggregateAccessCtx {
  QBDI::rword preEsp = 0;
  QBDI::rword postEsp = 0;
  uint16_t size = 0;
  QBDI::MemoryAccessType type = QBDI::MEMORY_READ;
  bool seen = false;
};

QBDI::VMAction captureEsp(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
                          QBDI::FPRState *fprState, void *data) {
  AggregateAccessCtx *ctx = static_cast<AggregateAccessCtx *>(data);
  ctx->preEsp = gprState->esp;
  return QBDI::VMAction::CONTINUE;
}

QBDI::VMAction checkAggregateAccess(QBDI::VMInstanceRef vm,
                                    QBDI::GPRState *gprState,
                                    QBDI::FPRState *fprState, void *data) {
  AggregateAccessCtx *ctx = static_cast<AggregateAccessCtx *>(data);
  std::vector<QBDI::MemoryAccess> accesses = vm->getInstMemoryAccess();
  REQUIRE(accesses.size() == 1);
  QBDI::rword expectedAddr = (ctx->type == QBDI::MEMORY_WRITE)
                                 ? (ctx->preEsp - ctx->size)
                                 : ctx->preEsp;
  CHECK(accesses[0].accessAddress == expectedAddr);
  CHECK(accesses[0].size == ctx->size);
  CHECK(accesses[0].type == ctx->type);
  CHECK((accesses[0].flags & QBDI::MEMORY_UNKNOWN_VALUE) != 0);
  ctx->postEsp = gprState->esp;
  ctx->seen = true;
  return QBDI::VMAction::CONTINUE;
}

} // anonymous namespace

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-PUSHA32") {
  const char source[] =
      "pusha\n"
      "addl $32, %esp\n";

  AggregateAccessCtx ctx;
  ctx.size = 32;
  ctx.type = QBDI::MEMORY_WRITE;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSHA32", QBDI::PREINST, captureEsp, &ctx);
  vm.addMnemonicCB("PUSHA32", QBDI::POSTINST, checkAggregateAccess, &ctx);

  QBDI::GPRState *state = vm.getGPRState();
  state->eax = 0x11111111;
  state->ecx = 0x22222222;
  state->edx = 0x33333333;
  state->ebx = 0x44444444;
  state->ebp = 0x66666666;
  state->esi = 0x77777777;
  state->edi = 0x88888888;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(ctx.seen);

  const uint32_t *stack = reinterpret_cast<const uint32_t *>(ctx.preEsp - 32);
  CHECK(stack[7] == 0x11111111); // EAX
  CHECK(stack[6] == 0x22222222); // ECX
  CHECK(stack[5] == 0x33333333); // EDX
  CHECK(stack[4] == 0x44444444); // EBX
  CHECK(stack[3] == ctx.preEsp); // original ESP
  CHECK(stack[2] == 0x66666666); // EBP
  CHECK(stack[1] == 0x77777777); // ESI
  CHECK(stack[0] == 0x88888888); // EDI
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-POPA32") {
  const char source[] =
      "subl $32, %esp\n"
      "movl $0x11111111, 28(%esp)\n"
      "movl $0x22222222, 24(%esp)\n"
      "movl $0x33333333, 20(%esp)\n"
      "movl $0x44444444, 16(%esp)\n"
      "movl $0xdead0005, 12(%esp)\n"
      "movl $0x66666666, 8(%esp)\n"
      "movl $0x77777777, 4(%esp)\n"
      "movl $0x88888888, 0(%esp)\n"
      "popa\n";

  AggregateAccessCtx ctx;
  ctx.size = 32;
  ctx.type = QBDI::MEMORY_READ;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POPA32", QBDI::PREINST, captureEsp, &ctx);
  vm.addMnemonicCB("POPA32", QBDI::POSTINST, checkAggregateAccess, &ctx);

  QBDI::GPRState *state = vm.getGPRState();
  state->eax = 0;
  state->ecx = 0;
  state->edx = 0;
  state->ebx = 0;
  state->ebp = 0;
  state->esi = 0;
  state->edi = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(ctx.seen);
  CHECK(state->eax == 0x11111111);
  CHECK(state->ecx == 0x22222222);
  CHECK(state->edx == 0x33333333);
  CHECK(state->ebx == 0x44444444);
  CHECK(state->ebp == 0x66666666);
  CHECK(state->esi == 0x77777777);
  CHECK(state->edi == 0x88888888);
  CHECK(ctx.postEsp == ctx.preEsp + 32);
  CHECK(ctx.postEsp != 0xdead0005);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-PUSHA16") {
  const char source[] =
      "pushaw\n"
      "addl $16, %esp\n";

  AggregateAccessCtx ctx;
  ctx.size = 16;
  ctx.type = QBDI::MEMORY_WRITE;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSHA16", QBDI::PREINST, captureEsp, &ctx);
  vm.addMnemonicCB("PUSHA16", QBDI::POSTINST, checkAggregateAccess, &ctx);

  QBDI::GPRState *state = vm.getGPRState();
  state->eax = 0xffff1111;
  state->ecx = 0xeeee2222;
  state->edx = 0xdddd3333;
  state->ebx = 0xcccc4444;
  state->ebp = 0xaaaa6666;
  state->esi = 0x99997777;
  state->edi = 0x88888888;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(ctx.seen);

  const uint16_t *stack = reinterpret_cast<const uint16_t *>(ctx.preEsp - 16);
  CHECK(stack[7] == 0x1111);                          // AX
  CHECK(stack[6] == 0x2222);                          // CX
  CHECK(stack[5] == 0x3333);                          // DX
  CHECK(stack[4] == 0x4444);                          // BX
  CHECK(stack[3] == (uint16_t)(ctx.preEsp & 0xffff)); // original SP
  CHECK(stack[2] == 0x6666);                          // BP
  CHECK(stack[1] == 0x7777);                          // SI
  CHECK(stack[0] == 0x8888);                          // DI
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-POPA16") {
  const char source[] =
      "subl $16, %esp\n"
      "movw $0x1111, 14(%esp)\n"
      "movw $0x2222, 12(%esp)\n"
      "movw $0x3333, 10(%esp)\n"
      "movw $0x4444, 8(%esp)\n"
      "movw $0xd005, 6(%esp)\n"
      "movw $0x6666, 4(%esp)\n"
      "movw $0x7777, 2(%esp)\n"
      "movw $0x8888, 0(%esp)\n"
      "popaw\n";

  AggregateAccessCtx ctx;
  ctx.size = 16;
  ctx.type = QBDI::MEMORY_READ;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POPA16", QBDI::PREINST, captureEsp, &ctx);
  vm.addMnemonicCB("POPA16", QBDI::POSTINST, checkAggregateAccess, &ctx);

  QBDI::GPRState *state = vm.getGPRState();
  state->eax = 0xaaaa0000;
  state->ecx = 0xbbbb0000;
  state->edx = 0xcccc0000;
  state->ebx = 0xdddd0000;
  state->ebp = 0xeeee0000;
  state->esi = 0xffff0000;
  state->edi = 0x12340000;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(ctx.seen);
  CHECK(state->eax == 0xaaaa1111);
  CHECK(state->ecx == 0xbbbb2222);
  CHECK(state->edx == 0xcccc3333);
  CHECK(state->ebx == 0xdddd4444);
  CHECK(state->ebp == 0xeeee6666);
  CHECK(state->esi == 0xffff7777);
  CHECK(state->edi == 0x12348888);
  CHECK(ctx.postEsp == ctx.preEsp + 16);
  CHECK(ctx.postEsp != 0xd005);
}
