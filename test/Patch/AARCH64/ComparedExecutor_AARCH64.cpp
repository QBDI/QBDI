/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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
#include "ComparedExecutor_AARCH64.h"
#include "QBDI/Config.h"
#include "QBDI/Memory.hpp"
#include "Patch/Utils.h"

const char CPU[] = CPU_CPU;
const std::vector<std::string> MATTRS = {CPU_MATTRS};

InMemoryObject
ComparedExecutor_AARCH64::compileWithContextSwitch(const char *source) {
  std::ostringstream finalSource;

  finalSource << "stp x29, lr, [sp, #-16]!\n"
              << "add x7, x1, #" << offsetof(QBDI::Context, fprState.v0)
              << "\n";
  for (uint32_t i = 0; i < QBDI_NUM_FPR; i = i + 4) {
    finalSource << "ld1 {v" << i << ".2d-v" << i + 3 << ".2d}, [x7], #"
                << sizeof(__uint128_t) * 4 << "\n";
  }
  finalSource
      << "ldr x0, [x1, #" << offsetof(QBDI::Context, gprState.nzcv) << "]\n"
      << "msr nzcv, x0\n"
      << "ldr x0, [x1, #" << offsetof(QBDI::Context, fprState.fpcr) << "]\n"
      << "msr fpcr, x0\n"
      << "ldr x0, [x1, #" << offsetof(QBDI::Context, fprState.fpsr) << "]\n"
      << "msr fpsr, x0\n"
      << "ldr x2, [x1, #" << offsetof(QBDI::Context, gprState.x2) << "]\n"
      << "ldr x3, [x1, #" << offsetof(QBDI::Context, gprState.x3) << "]\n"
      << "ldr x4, [x1, #" << offsetof(QBDI::Context, gprState.x4) << "]\n"
      << "ldr x5, [x1, #" << offsetof(QBDI::Context, gprState.x5) << "]\n"
      << "ldr x6, [x1, #" << offsetof(QBDI::Context, gprState.x6) << "]\n"
      << "ldr x7, [x1, #" << offsetof(QBDI::Context, gprState.x7) << "]\n"
      << "ldr x8, [x1, #" << offsetof(QBDI::Context, gprState.x8) << "]\n"
      << "ldr x9, [x1, #" << offsetof(QBDI::Context, gprState.x9) << "]\n"
      << "ldr x10, [x1, #" << offsetof(QBDI::Context, gprState.x10) << "]\n"
      << "ldr x11, [x1, #" << offsetof(QBDI::Context, gprState.x11) << "]\n"
      << "ldr x12, [x1, #" << offsetof(QBDI::Context, gprState.x12) << "]\n"
      << "ldr x13, [x1, #" << offsetof(QBDI::Context, gprState.x13) << "]\n"
      << "ldr x14, [x1, #" << offsetof(QBDI::Context, gprState.x14) << "]\n"
      << "ldr x15, [x1, #" << offsetof(QBDI::Context, gprState.x15) << "]\n"
      << "ldr x16, [x1, #" << offsetof(QBDI::Context, gprState.x16) << "]\n"
      << "ldr x17, [x1, #" << offsetof(QBDI::Context, gprState.x17) << "]\n"
#if not(defined(QBDI_PLATFORM_OSX) || defined(QBDI_PLATFORM_IOS))
      << "ldr x18, [x1, #" << offsetof(QBDI::Context, gprState.x18) << "]\n"
#endif
      << "ldr x19, [x1, #" << offsetof(QBDI::Context, gprState.x19) << "]\n"
      << "ldr x20, [x1, #" << offsetof(QBDI::Context, gprState.x20) << "]\n"
      << "ldr x21, [x1, #" << offsetof(QBDI::Context, gprState.x21) << "]\n"
      << "ldr x22, [x1, #" << offsetof(QBDI::Context, gprState.x22) << "]\n"
      << "ldr x23, [x1, #" << offsetof(QBDI::Context, gprState.x23) << "]\n"
      << "ldr x24, [x1, #" << offsetof(QBDI::Context, gprState.x24) << "]\n"
      << "ldr x25, [x1, #" << offsetof(QBDI::Context, gprState.x25) << "]\n"
      << "ldr x26, [x1, #" << offsetof(QBDI::Context, gprState.x26) << "]\n"
      << "ldr x27, [x1, #" << offsetof(QBDI::Context, gprState.x27) << "]\n"
      << "ldr x28, [x1, #" << offsetof(QBDI::Context, gprState.x28) << "]\n"
      << "ldr x29, [x1, #" << offsetof(QBDI::Context, gprState.x29) << "]\n"
      << "ldr x30, [x1, #" << offsetof(QBDI::Context, gprState.lr)
      << "]\n"
      // save and replace sp
      << "mov x0, sp\n"
      << "str x0, [x1, #" << offsetof(QBDI::Context, hostState.sp) << "]\n"
      << "ldr x0, [x1, #" << offsetof(QBDI::Context, gprState.sp) << "]\n"
      << "mov sp, x0\n"
      << "stp xzr, x1, [sp, #-16]!\n" // stack must be align
      << "ldr x0, [x1, #" << offsetof(QBDI::Context, gprState.x0) << "]\n"
      << "ldr x1, [x1, #" << offsetof(QBDI::Context, gprState.x1) << "]\n";
  finalSource << source;
  finalSource
      << "str x1, [sp]\n"
      << "ldr x1, [sp, #8]\n"
      << "str x0, [x1, #" << offsetof(QBDI::Context, gprState.x0) << "]\n"
      << "ldp x0, x1, [sp], #16\n"
      << "str x0, [x1, #" << offsetof(QBDI::Context, gprState.x1) << "]\n"
      << "mrs x0, nzcv\n"
      << "str x0, [x1, #" << offsetof(QBDI::Context, gprState.nzcv) << "]\n"
      << "mrs x0, fpcr\n"
      << "str x0, [x1, #" << offsetof(QBDI::Context, fprState.fpcr) << "]\n"
      << "mrs x0, fpsr\n"
      << "str x0, [x1, #" << offsetof(QBDI::Context, fprState.fpsr) << "]\n"
      << "str x2, [x1, #" << offsetof(QBDI::Context, gprState.x2) << "]\n"
      << "str x3, [x1, #" << offsetof(QBDI::Context, gprState.x3) << "]\n"
      << "str x4, [x1, #" << offsetof(QBDI::Context, gprState.x4) << "]\n"
      << "str x5, [x1, #" << offsetof(QBDI::Context, gprState.x5) << "]\n"
      << "str x6, [x1, #" << offsetof(QBDI::Context, gprState.x6) << "]\n"
      << "str x7, [x1, #" << offsetof(QBDI::Context, gprState.x7) << "]\n"
      << "str x8, [x1, #" << offsetof(QBDI::Context, gprState.x8) << "]\n"
      << "str x9, [x1, #" << offsetof(QBDI::Context, gprState.x9) << "]\n"
      << "str x10, [x1, #" << offsetof(QBDI::Context, gprState.x10) << "]\n"
      << "str x11, [x1, #" << offsetof(QBDI::Context, gprState.x11) << "]\n"
      << "str x12, [x1, #" << offsetof(QBDI::Context, gprState.x12) << "]\n"
      << "str x13, [x1, #" << offsetof(QBDI::Context, gprState.x13) << "]\n"
      << "str x14, [x1, #" << offsetof(QBDI::Context, gprState.x14) << "]\n"
      << "str x15, [x1, #" << offsetof(QBDI::Context, gprState.x15) << "]\n"
      << "str x16, [x1, #" << offsetof(QBDI::Context, gprState.x16) << "]\n"
      << "str x17, [x1, #" << offsetof(QBDI::Context, gprState.x17) << "]\n"
#if not(defined(QBDI_PLATFORM_OSX) || defined(QBDI_PLATFORM_IOS))
      << "str x18, [x1, #" << offsetof(QBDI::Context, gprState.x18) << "]\n"
#endif
      << "str x19, [x1, #" << offsetof(QBDI::Context, gprState.x19) << "]\n"
      << "str x20, [x1, #" << offsetof(QBDI::Context, gprState.x20) << "]\n"
      << "str x21, [x1, #" << offsetof(QBDI::Context, gprState.x21) << "]\n"
      << "str x22, [x1, #" << offsetof(QBDI::Context, gprState.x22) << "]\n"
      << "str x23, [x1, #" << offsetof(QBDI::Context, gprState.x23) << "]\n"
      << "str x24, [x1, #" << offsetof(QBDI::Context, gprState.x24) << "]\n"
      << "str x25, [x1, #" << offsetof(QBDI::Context, gprState.x25) << "]\n"
      << "str x26, [x1, #" << offsetof(QBDI::Context, gprState.x26) << "]\n"
      << "str x27, [x1, #" << offsetof(QBDI::Context, gprState.x27) << "]\n"
      << "str x28, [x1, #" << offsetof(QBDI::Context, gprState.x28) << "]\n"
      << "str x29, [x1, #" << offsetof(QBDI::Context, gprState.x29) << "]\n"
      << "str x30, [x1, #" << offsetof(QBDI::Context, gprState.lr)
      << "]\n"
      // restore sp
      << "ldr x0, [x1, #" << offsetof(QBDI::Context, hostState.sp) << "]\n"
      << "mov sp, x0\n"
      << "add x1, x1, #" << offsetof(QBDI::Context, fprState.v0) << "\n";
  for (uint32_t i = 0; i < QBDI_NUM_FPR; i = i + 4) {
    finalSource << "st1 {v" << i << ".2d-v" << i + 3 << ".2d}, [x1], #"
                << sizeof(__uint128_t) * 4 << "\n";
  }
  finalSource << "ldp x29, lr, [sp], 16\n";
  finalSource << "ret\n";

  return InMemoryObject(finalSource.str().c_str(), CPU, "aarch64", MATTRS);
}

QBDI::Context ComparedExecutor_AARCH64::jitExec(llvm::ArrayRef<uint8_t> code,
                                                QBDI::Context &inputState,
                                                llvm::sys::MemoryBlock &stack) {
  QBDI::Context outputState;
  QBDI::Context outerState;
  llvm::sys::MemoryBlock ctxBlock;
  llvm::sys::MemoryBlock outerStack;
  std::error_code ec;

  ctxBlock = llvm::sys::Memory::allocateMappedMemory(
      4096, nullptr, PF::MF_READ | PF::MF_WRITE, ec);
  outerStack = llvm::sys::Memory::allocateMappedMemory(
      4096, nullptr, PF::MF_READ | PF::MF_WRITE, ec);
  memset((void *)&outerState, 0, sizeof(QBDI::Context));
  // Put the inputState on the stack
  inputState.gprState.sp = (QBDI::rword)stack.base() + stack.allocatedSize();

  memcpy((void *)ctxBlock.base(), (void *)&inputState, sizeof(QBDI::Context));
  // Prepare the outerState to fake the realExec() action
  outerState.gprState.sp =
      (QBDI::rword)outerStack.base() + outerStack.allocatedSize();
  outerState.gprState.lr = (QBDI::rword)0;
  outerState.gprState.x1 = (QBDI::rword)ctxBlock.base();

  vm.setGPRState(&outerState.gprState);
  vm.setFPRState(&outerState.fprState);
  vm.addInstrumentedRange((QBDI::rword)code.data(),
                          (QBDI::rword)code.data() + code.size());
  vm.run((QBDI::rword)code.data(), 0);
  vm.removeInstrumentedRange((QBDI::rword)code.data(),
                             (QBDI::rword)code.data() + code.size());

  memcpy((void *)&outputState, (void *)ctxBlock.base(), sizeof(QBDI::Context));

  llvm::sys::Memory::releaseMappedMemory(ctxBlock);
  llvm::sys::Memory::releaseMappedMemory(outerStack);

  return outputState;
}

QBDI::Context
ComparedExecutor_AARCH64::realExec(llvm::ArrayRef<uint8_t> code,
                                   QBDI::Context &inputState,
                                   llvm::sys::MemoryBlock &stack) {

  QBDI::Context outputState;
  std::error_code ec;
  llvm::sys::MemoryBlock ctxBlock;

  ctxBlock = llvm::sys::Memory::allocateMappedMemory(
      4096, nullptr, PF::MF_READ | PF::MF_WRITE, ec);

  // Put the inputState on the stack
  inputState.gprState.sp = (QBDI::rword)stack.base() + stack.allocatedSize();

  // Assemble the sources
  // Copy the input context
  memcpy(ctxBlock.base(), (void *)&inputState, sizeof(QBDI::Context));
  // Execute
  asm volatile inline(
      "mov x1, %1\n"
      "mov x2, %0\n"
      "stp x0, lr, [SP, #-16]!\n"
      "blr x2\n"
      "ldp x0, lr, [SP], #16\n"
      :
      : "r"(code.data()), "r"(ctxBlock.base())
      : "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11",
        "x12", "x13", "x14", "x15", "x16", "x17", "x19", "x20", "x21", "x22",
        "x23", "x24", "x25", "x26", "x27", "x28", "v0", "v1", "v2", "v3", "v4",
        "v5", "v6", "v7", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15",
        "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25",
        "v26", "v27", "v28", "v29", "v30", "v31",
#if not(defined(QBDI_PLATFORM_OSX) || defined(QBDI_PLATFORM_IOS))
        "x18",
#endif
        "memory");
  // Get the output context
  memcpy((void *)&outputState, ctxBlock.base(), sizeof(QBDI::Context));

  llvm::sys::Memory::releaseMappedMemory(ctxBlock);

  return outputState;
}

void ComparedExecutor_AARCH64::initContext(QBDI::Context &ctx) {
  memset(&ctx, 0, sizeof(QBDI::Context));
  ctx.gprState.x0 = get_random();
  ctx.gprState.x1 = get_random();
  ctx.gprState.x2 = get_random();
  ctx.gprState.x3 = get_random();
  ctx.gprState.x4 = get_random();
  ctx.gprState.x5 = get_random();
  ctx.gprState.x6 = get_random();
  ctx.gprState.x7 = get_random();
  ctx.gprState.x8 = get_random();
  ctx.gprState.x9 = get_random();
  ctx.gprState.x10 = get_random();
  ctx.gprState.x11 = get_random();
  ctx.gprState.x12 = get_random();
  ctx.gprState.x13 = get_random();
  ctx.gprState.x14 = get_random();
  ctx.gprState.x15 = get_random();
  ctx.gprState.x16 = get_random();
  ctx.gprState.x17 = get_random();
  ctx.gprState.x18 = get_random();
  ctx.gprState.x19 = get_random();
  ctx.gprState.x20 = get_random();
  ctx.gprState.x21 = get_random();
  ctx.gprState.x22 = get_random();
  ctx.gprState.x23 = get_random();
  ctx.gprState.x24 = get_random();
  ctx.gprState.x25 = get_random();
  ctx.gprState.x26 = get_random();
  ctx.gprState.x27 = get_random();
  ctx.gprState.x28 = get_random();
  ctx.gprState.x29 = get_random();
  ctx.gprState.lr = get_random();
}

const char *GPRSave_s =
    "    mov x0, #1\n"
    "    mov x1, #2\n"
    "    mov x2, #3\n"
    "    mov x3, #4\n"
    "    mov x4, #5\n"
    "    mov x5, #6\n"
    "    mov x6, #7\n"
    "    mov x7, #8\n"
    "    mov x8, #9\n"
    "    mov x9, #10\n"
    "    mov x10, #11\n"
    "    mov x11, #12\n"
    "    mov x12, #13\n"
    "    mov x13, #14\n"
    "    mov x14, #15\n"
    "    mov x15, #16\n"
    "    mov x16, #17\n"
    "    mov x17, #18\n"
#if not(defined(QBDI_PLATFORM_OSX) || defined(QBDI_PLATFORM_IOS))
    "    mov x18, #19\n"
#endif
    "    mov x19, #20\n"
    "    mov x20, #21\n"
    "    mov x21, #22\n"
    "    mov x22, #23\n"
    "    mov x23, #24\n"
    "    mov x24, #25\n"
    "    mov x25, #26\n"
    "    mov x26, #27\n"
    "    mov x27, #28\n"
    "    mov x28, #29\n"
    "    mov x29, #30\n";

const char *GPRShuffle_s =
    "    stp x0,  x1,   [sp, #-16]!\n"
    "    stp x2,  x3,   [sp, #-16]!\n"
    "    stp x4,  x5,   [sp, #-16]!\n"
    "    stp x6,  x7,   [sp, #-16]!\n"
    "    stp x8,  x9,   [sp, #-16]!\n"
    "    stp x10, x11,  [sp, #-16]!\n"
    "    stp x12, x13,  [sp, #-16]!\n"
    "    stp x14, x15,  [sp, #-16]!\n"
    "    stp x16, x17,  [sp, #-16]!\n"
#if not(defined(QBDI_PLATFORM_OSX) || defined(QBDI_PLATFORM_IOS))
    "    stp x18, x19,  [sp, #-16]!\n"
#else
    "    stp xzr, x19,  [sp, #-16]!\n"
#endif
    "    stp x20, x21,  [sp, #-16]!\n"
    "    stp x22, x23,  [sp, #-16]!\n"
    "    stp x24, x25,  [sp, #-16]!\n"
    "    stp x26, x27,  [sp, #-16]!\n"
    "    stp x28, x29,  [sp, #-16]!\n"
    "    ldp x1,  x0,   [sp], 16\n"
    "    ldp x21, x20,  [sp], 16\n"
    "    ldp x5,  x11,  [sp], 16\n"
    "    ldp x10, x27,  [sp], 16\n"
    "    ldp x9,  x28,  [sp], 16\n"
    "    ldp x25, x3,   [sp], 16\n"
    "    ldp x4,  x22,  [sp], 16\n"
    "    ldp x26, x6,   [sp], 16\n"
    "    ldp x24, x2,   [sp], 16\n"
    "    ldp x17, x29,  [sp], 16\n"
    "    ldp x15, x13,  [sp], 16\n"
    "    ldp x8,  x12,  [sp], 16\n"
    "    ldp x23, x14,  [sp], 16\n"
    "    ldp x19, x7,   [sp], 16\n"
#if not(defined(QBDI_PLATFORM_OSX) || defined(QBDI_PLATFORM_IOS))
    "    ldp x16, x18,  [sp], 16\n";
#else
    "    ldp x16, xzr,  [sp], 16\n";
#endif

const char *RelativeAddressing_s =
    "b start\n"
    "c1:\n"
    "    .quad 0x123456789abcdef0\n"
    "start:\n"
    "    adr x4, c1\n"
    "    ldr x5, [x4]\n"
    "    eor x0, x0, x5\n"
    "    adr x6, c2\n"
    "    ldr x7, [x6]\n"
    "    eor x1, x1, x7\n"
    "    b end\n"
    "c2:\n"
    "    .quad 0x0fedcba987654321\n"
    "end:\n";

const char *ConditionalBranching_s =
    "    stp x2,  x3,   [sp, #-16]!\n"
    "    stp x0,  x1,   [sp, #-16]!\n"
    "    mov x12, #0\n"
    "    mov x2, #0\n"
    "    mov x1, #0\n"
    "    mov x0, sp\n"
    "loop:\n"
    "    ldrb w1, [x0], 1\n"
    "    eor x12, x12, x1\n"
    "    ror x12, x12, #12\n"
    "    add x2, x2, #1\n"
    "    cmp x2, #32\n"
    "    blt loop\n"
    "    adr x3, checksum\n"
    "    ldr x4, [x3]\n"
    "    cmp x4, x2\n"
    "    bne bad\n"
    "    mov x0, #1\n"
    "    b end\n"
    "bad:\n"
    "    mov x0, #0\n"
    "    b end\n"
    "checksum:\n"
    "    .quad 0x32253676ffe8dd7f\n"
    "end:\n"
    "    add sp, sp, 32\n";

const char *FibonacciRecursion_s =
    "    adr x1, fibo\n"
    "    stp x1, x28, [sp, #-16]!\n"
    "    blr x1\n"
    "    b end\n"
    "fibo:\n"
    "    stp lr, x0, [sp, #-16]!\n"
    "    cmp x0, #2\n"
    "    bhi fibo1\n"
    "    mov x0, #1\n"
    "    b fibo2\n"
    "fibo1:\n"
    "    sub x0, x0, #1\n"
    "    str x0, [sp, #8]\n"
    "    bl fibo\n"
    "    ldr x2, [sp, #8]\n"
    "    str x0, [sp, #8]\n"
    "    sub x0, x2, #1\n"
    "    adr x1, fibo\n"
    "    blr x1\n"
    "    ldr x1, [sp, #8]\n"
    "    add x0, x0, x1\n"
    "fibo2:\n"
    "    ldp lr, x1, [sp], 16\n"
    "    ret\n"
    "end:\n"
    "    ldp x1, x28, [sp], 16\n";

const char *StackTricks_s =
    "    adr x2, end\n"
    "    adr x3, f1\n"
    "    stp x2, x0, [sp, #-16]!\n"
    "    mov lr, x3\n"
    "    ret\n"
    "f1:\n"
    "    ldr x0, [sp, #8]\n"
    "    adr x2, f2\n"
    "    adr x3, f6\n"
    "    mov x4, #1\n"
    "    cmp x0, #2\n"
    "    csel x3, x2, x3, hi\n"
    "    mov lr, x3\n"
    "    ret\n"
    "f2:\n"
    "    sub x0, x0, #1\n"
    "    adr x2, f4\n"
    "    adr x3, f1\n"
    "    stp x2, x0, [sp, #-16]!\n"
    "    mov lr, x3\n"
    "    ret\n"
    "f4:\n"
    "    add x1, x1, x4\n"
    "    sub x0, x0, #1\n"
    "    adr x2, f5\n"
    "    adr x3, f1\n"
    "    stp x2, x0, [sp, #-16]!\n"
    "    mov lr, x3\n"
    "    ret\n"
    "f5:\n"
    "    add x4, x4, x1\n"
    "f6:\n"
    "    add x0, sp, #8\n"
    "    ldp lr, x0, [sp], 16\n"
    "    ret\n"
    "end:\n";
