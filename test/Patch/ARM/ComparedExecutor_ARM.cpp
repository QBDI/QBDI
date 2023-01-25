/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2023 Quarkslab
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
#include "ComparedExecutor_ARM.h"
#include "QBDI/Config.h"
#include "QBDI/Memory.hpp"
#include "Patch/Utils.h"

const char CPU[] = CPU_CPU;
const std::vector<std::string> MATTRS = {CPU_MATTRS};

InMemoryObject
ComparedExecutor_ARM::compileWithContextSwitch(const char *source) {
  std::ostringstream finalSource;

  finalSource << "push {lr}\n"
              << "add r0, r1, #" << offsetof(QBDI::Context, fprState.vreg.d[0])
              << "\n"
              << "vldmia	r0!, {d0-d15}\n"
#if QBDI_NUM_FPR == 32
              << "vldmia	r0, {d16-d31}\n"
#endif
              << "ldr r0, [r1, #" << offsetof(QBDI::Context, gprState.cpsr)
              << "]\n"
              << "msr APSR_nzcvqg, r0\n"
              // backup SP
              << "str sp, [r1, #" << offsetof(QBDI::Context, hostState.sp)
              << "]\n"
              // set SP LR
              << "add r0, r1, #" << offsetof(QBDI::Context, gprState.sp) << "\n"
              << "ldm r0, {sp,lr}\n"
              // set backup
              << "push {r0-r1}\n"
              // set r0-r12 SP LR
              << "add r0, r1, #" << offsetof(QBDI::Context, gprState.r0) << "\n"
              << "ldm r0, {r0-r12}\n";
  finalSource << source;
  finalSource << "str r0, [sp]\n"
              << "ldr r0, [sp, #4]\n"
              // backup r1-r12 SP LR
              << "add r0, r0, #" << offsetof(QBDI::Context, gprState.r1) << "\n"
              << "stm r0, {r1-r12,sp,lr}\n"
              << "pop {r0-r1}\n"
              // backup r0
              << "str r0, [r1, #" << offsetof(QBDI::Context, gprState.r0)
              << "]\n"
              // restore sp
              << "ldr sp, [r1, #" << offsetof(QBDI::Context, hostState.sp)
              << "]\n"
              << "mrs r0, APSR\n"
              << "str r0, [r1, #" << offsetof(QBDI::Context, gprState.cpsr)
              << "]\n"
              << "add r0, r1, #" << offsetof(QBDI::Context, fprState.vreg.d[0])
              << "\n"
              << "vstmia  r0!, {d0-d15}\n"
#if QBDI_NUM_FPR == 32
              << "vstmia  r0, {d16-d31}\n"
#endif
              << "pop {pc}\n";

  return InMemoryObject(finalSource.str().c_str(), CPU, "arm", MATTRS);
}

QBDI::Context ComparedExecutor_ARM::jitExec(llvm::ArrayRef<uint8_t> code,
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
  outerState.gprState.r1 = (QBDI::rword)ctxBlock.base();

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

QBDI::Context ComparedExecutor_ARM::realExec(llvm::ArrayRef<uint8_t> code,
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
  {
      register uint32_t ctxBlockBase asm("r1") = (uint32_t) ctxBlock.base();
      register uint32_t codeData asm("r2") = (uint32_t) code.data();
      asm volatile inline(
          "push {r0, lr}\n"
          "blx r2\n"
          "pop {r0, lr}\n"
          :
          : "r"(codeData), "r"(ctxBlockBase)
          : "r0", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11",
            "r12", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9",
            "d10", "d11", "d12", "d13", "d14", "d15", "d16", "d17", "d18", "d19",
            "d20", "d21", "d22", "d23", "d24", "d25", "d26", "d27", "d28", "d29",
            "d30", "d31", "memory");
  }
  // Get the output context
  memcpy((void *)&outputState, ctxBlock.base(), sizeof(QBDI::Context));

  llvm::sys::Memory::releaseMappedMemory(ctxBlock);

  return outputState;
}

void ComparedExecutor_ARM::initContext(QBDI::Context &ctx) {
  memset(&ctx, 0, sizeof(QBDI::Context));
  ctx.gprState.r0 = get_random();
  ctx.gprState.r1 = get_random();
  ctx.gprState.r2 = get_random();
  ctx.gprState.r3 = get_random();
  ctx.gprState.r4 = get_random();
  ctx.gprState.r5 = get_random();
  ctx.gprState.r6 = get_random();
  ctx.gprState.r7 = get_random();
  ctx.gprState.r8 = get_random();
  ctx.gprState.r9 = get_random();
  ctx.gprState.r10 = get_random();
  ctx.gprState.r11 = get_random();
  ctx.gprState.r12 = get_random();
  ctx.gprState.lr = get_random();
}

const char *GPRSave_s =
    "    mov r0, #1\n"
    "    mov r1, #2\n"
    "    mov r2, #3\n"
    "    mov r3, #4\n"
    "    mov r4, #5\n"
    "    mov r5, #6\n"
    "    mov r6, #7\n"
    "    mov r7, #8\n"
    "    mov r8, #9\n"
    "    mov r9, #10\n"
    "    mov r10, #11\n"
    "    mov r11, #12\n"
    "    mov r12, #13\n"
    "    mov lr, #14\n";

const char *GPRShuffle_s =
    "    push {r0-r12,lr}\n"
    "    pop {r3}\n"
    "    pop {r11}\n"
    "    pop {r7}\n"
    "    pop {r2}\n"
    "    pop {r12}\n"
    "    pop {r10}\n"
    "    pop {lr}\n"
    "    pop {r1}\n"
    "    pop {r5}\n"
    "    pop {r6}\n"
    "    pop {r0}\n"
    "    pop {r4}\n"
    "    pop {r9}\n"
    "    pop {r8}\n";

const char *RelativeAddressing_s =
    "b start\n"
    "c1:\n"
    "    .word 0x12345678\n"
    "start:\n"
    "    adr r4, c1\n"
    "    ldr r5, [r4]\n"
    "    eor r0, r0, r5\n"
    "    adr r6, c2\n"
    "    ldr r7, [r6]\n"
    "    eor r1, r1, r7\n"
    "    b end\n"
    "c2:\n"
    "    .word 0x87654321\n"
    "end:\n";

const char *ConditionalBranching_s =
    "    push {r0-r3}\n"
    "    mov r12, #0\n"
    "    mov r2, #0\n"
    "    mov r1, #0\n"
    "    mov r0, sp\n"
    "loop:\n"
    "    ldrb r1, [r0], 1\n"
    "    eor r12, r12, r1\n"
    "    ror r12, r12, #12\n"
    "    add r2, r2, #1\n"
    "    cmp r2, #16\n"
    "    blt loop\n"
    "    adr r3, checksum\n"
    "    ldr r4, [r3]\n"
    "    cmp r4, r2\n"
    "    bne bad\n"
    "    mov r0, #1\n"
    "    b end\n"
    "bad:\n"
    "    mov r0, #0\n"
    "    b end\n"
    "checksum:\n"
    "    .word 0xffe8dd7f\n"
    "end:\n"
    "    add sp, sp, 16\n";

const char *FibonacciRecursion_s =
    "    adr r2, fibo\n"
    "    blx r2\n"
    "    b end\n"
    "fibo:\n"
    "    cmp r0, #2\n"
    "    movls r0, #1\n"
    "    bxls lr\n"
    "    push {r0, lr}\n"
    "    sub r0, r0, #1\n"
    "    blx r2\n"
    "    pop {r1}\n"
    "    push {r0}\n"
    "    sub r0, r1, #2\n"
    "    blx r2\n"
    "    pop {r1}\n"
    "    add r0, r0, r1\n"
    "    pop {pc}\n"
    "end:\n";

const char *StackTricks_s =
    "    adr r2, end\n"
    "    adr r3, f1\n"
    "    push {r0, r2}\n"
    "    bx r3\n"
    "f1:\n"
    "    ldr r0, [sp]\n"
    "    adr r2, f2\n"
    "    adr r3, f6\n"
    "    mov r4, #1\n"
    "    cmp r0, #2\n"
    "    movhi r3, r2\n"
    "    push {r3}\n"
    "    pop {pc}\n"
    "f2:\n"
    "    sub r0, r0, #1\n"
    "    adr r2, f4\n"
    "    adr r3, f1\n"
    "    push {r0, r2}\n"
    "    mov lr, r3\n"
    "    bx lr\n"
    "f4:\n"
    "    add r1, r1, r4\n"
    "    sub r0, r0, #1\n"
    "    adr r2, f5\n"
    "    adr r3, f1\n"
    "    push {r0, r2}\n"
    "    blx r3\n"
    "f5:\n"
    "    add r4, r4, r1\n"
    "f6:\n"
    "    pop {r0, pc}\n"
    "end:\n";

const char *STLDMIA_s =
    "    sub r0, sp, #128\n"
    "    adr lr, pos1\n"
    "    stmia r0, {r0-r12,sp,lr,pc}\n"
    "    ldmia r0, {r0-r12,sp,pc}\n"
    "pos1:\n"
    "    sub r0, r0, #128\n"
    "    adr lr, pos2\n"
    "    stmia r0, {r2-r12,sp,lr,pc}\n"
    "    ldmia r0, {r2-r12,sp,pc}\n"
    "pos2:\n"
    "    sub r0, r0, #128\n"
    "    stmiaeq r0, {r0-r12,sp,pc}\n"
    "    ldmiaeq r0, {r0-r12,sp,pc}\n"
    "    sub r0, r0, #128\n"
    "    stmiane r0, {r0-r12,sp,pc}\n"
    "    ldmiane r0, {r0-r12,sp,pc}\n";

const char *STLDMIB_s =
    "    sub r0, sp, #128\n"
    "    adr lr, pos1\n"
    "    stmib r0, {r0-r12,sp,lr,pc}\n"
    "    ldmib r0, {r0-r12,sp,pc}\n"
    "pos1:\n"
    "    sub r0, r0, #128\n"
    "    adr lr, pos2\n"
    "    stmib r0, {r2-r12,sp,lr,pc}\n"
    "    ldmib r0, {r2-r12,sp,pc}\n"
    "pos2:\n"
    "    sub r0, r0, #128\n"
    "    stmibeq r0, {r0-r12,sp,pc}\n"
    "    ldmibeq r0, {r0-r12,sp,pc}\n"
    "    sub r0, r0, #128\n"
    "    stmibne r0, {r0-r12,sp,pc}\n"
    "    ldmibne r0, {r0-r12,sp,pc}\n";

const char *STLDMDA_s =
    "    sub r0, sp, #128\n"
    "    adr lr, pos1\n"
    "    stmda r0, {r0-r12,sp,lr,pc}\n"
    "    sub r0, r0, #4\n"
    "    ldmda r0, {r0-r12,sp,pc}\n"
    "pos1:\n"
    "    sub r0, r0, #128\n"
    "    adr lr, pos2\n"
    "    stmda r0, {r2-r12,sp,lr,pc}\n"
    "    sub r0, r0, #4\n"
    "    ldmda r0, {r2-r12,sp,pc}\n"
    "pos2:\n"
    "    sub r0, r0, #128\n"
    "    stmdaeq r0, {r0-r12,sp,pc}\n"
    "    ldmdaeq r0, {r0-r12,sp,pc}\n"
    "    sub r0, r0, #128\n"
    "    stmdane r0, {r0-r12,sp,pc}\n"
    "    ldmdane r0, {r0-r12,sp,pc}\n";

const char *STLDMDB_s =
    "    sub r0, sp, #128\n"
    "    adr lr, pos1\n"
    "    stmdb r0, {r0-r12,sp,lr,pc}\n"
    "    sub r0, r0, #4\n"
    "    ldmdb r0, {r0-r12,sp,pc}\n"
    "pos1:\n"
    "    sub r0, r0, #128\n"
    "    adr lr, pos2\n"
    "    stmdb r0, {r2-r12,sp,lr,pc}\n"
    "    sub r0, r0, #4\n"
    "    ldmdb r0, {r2-r12,sp,pc}\n"
    "pos2:\n"
    "    sub r0, r0, #128\n"
    "    stmdbeq r0, {r0-r12,sp,pc}\n"
    "    ldmdbeq r0, {r0-r12,sp,pc}\n"
    "    sub r0, r0, #128\n"
    "    stmdbne r0, {r0-r12,sp,pc}\n"
    "    ldmdbne r0, {r0-r12,sp,pc}\n";

const char *STMDB_LDMIA_post_s =
    "    sub r0, sp, #128\n"
    "    adr lr, pos1\n"
    "    stmdb r0!, {r0-r12,sp,lr,pc}\n"
    "    add r0, r0, #4\n"
    "    ldmia r0!, {r1-r12,sp,pc}\n"
    "pos1:\n"
    "    sub r0, r0, #128\n"
    "    adr lr, pos2\n"
    "    stmdb r0!, {r2-r12,sp,lr,pc}\n"
    "    ldmia r0!, {r2-r12,sp,pc}\n"
    "pos2:\n"
    "    sub r0, r0, #128\n"
    "    stmdbeq r0!, {r2-r12,sp,pc}\n"
    "    ldmiaeq r0!, {r2-r12,sp,pc}\n"
    "    sub r0, r0, #128\n"
    "    stmdbne r0!, {r2-r12,sp,pc}\n"
    "    ldmiane r0!, {r2-r12,sp,pc}\n";

const char *STMDA_LDMIB_post_s =
    "    sub r0, sp, #128\n"
    "    adr lr, pos1\n"
    "    stmda r0!, {r0-r12,sp,lr,pc}\n"
    "    add r0, r0, #4\n"
    "    ldmib r0!, {r1-r12,sp,pc}\n"
    "pos1:\n"
    "    sub r0, r0, #128\n"
    "    adr lr, pos2\n"
    "    stmda r0!, {r2-r12,sp,lr,pc}\n"
    "    ldmib r0!, {r2-r12,sp,pc}\n"
    "pos2:\n"
    "    sub r0, r0, #128\n"
    "    stmdaeq r0!, {r2-r12,sp,pc}\n"
    "    ldmibeq r0!, {r2-r12,sp,pc}\n"
    "    sub r0, r0, #128\n"
    "    stmdane r0!, {r2-r12,sp,pc}\n"
    "    ldmibne r0!, {r2-r12,sp,pc}\n";

const char *STMIB_LDMDA_post_s =
    "    sub r0, sp, #128\n"
    "    adr lr, pos1\n"
    "    stmib r0!, {r0-r12,lr,pc}\n"
    "    sub r0, r0, #4\n"
    "    ldmda r0!, {r1-r12,pc}\n"
    "pos1:\n"
    "    sub r0, r0, #128\n"
    "    adr lr, pos2\n"
    "    stmib r0!, {r2-r12,sp,lr,pc}\n"
    "    sub r0, r0, #4\n"
    "    ldmda r0!, {r2-r12,sp,pc}\n"
    "pos2:\n"
    "    sub r0, r0, #128\n"
    "    stmibeq r0!, {r2-r12,sp,pc}\n"
    "    ldmdaeq r0!, {r2-r12,sp,pc}\n"
    "    sub r0, r0, #128\n"
    "    stmibne r0!, {r2-r12,sp,pc}\n"
    "    ldmdane r0!, {r2-r12,sp,pc}\n";

const char *STMIA_LDMDB_post_s =
    "    sub r0, sp, #128\n"
    "    adr lr, pos1\n"
    "    stmia r0!, {r0-r12,lr,pc}\n"
    "    sub r0, r0, #4\n"
    "    ldmdb r0!, {r1-r12,pc}\n"
    "pos1:\n"
    "    sub r0, r0, #128\n"
    "    adr lr, pos2\n"
    "    stmia r0!, {r2-r12,sp,lr,pc}\n"
    "    sub r0, r0, #4\n"
    "    ldmdb r0!, {r2-r12,sp,pc}\n"
    "pos2:\n"
    "    sub r0, r0, #128\n"
    "    stmiaeq r0!, {r2-r12,sp,pc}\n"
    "    ldmdbeq r0!, {r2-r12,sp,pc}\n"
    "    sub r0, r0, #128\n"
    "    stmiane r0!, {r2-r12,sp,pc}\n"
    "    ldmdbne r0!, {r2-r12,sp,pc}\n";

const char *LDREXTest_s =
    "   mov r12, sp\n"
    "   mov r2, #0\n"
    "loopmemset:\n"
    "   strh r2, [r11, r2]\n"
    "   add r2, #2\n"
    "   cmp r2, 4096\n"
    "   bne loopmemset\n"
    // align 4 r11 (to support ldrexd)
    "   orr r11, r11, 0xf\n"
    "   add r11, r11, 1\n"
    // test 1
    "   ldrex r0, [r11]\n"
    "   mov r1, #0xff\n"
    "   strex r2, r1, [r11]\n"
    "   ldr r1, [r11]\n"
    "   push {r0, r1, r2}\n"
    // test 2
    "   add r10, r11, #256\n"
    "   ldrex r0, [r10]\n"
    "   mov r1, #0xfa7\n"
    "   add r10, r11, #256\n"
    "   strex r2, r1, [r10]\n"
    "   ldr r1, [r11, #256]\n"
    "   push {r0, r1, r2}\n"
    // test 3
    "   ldrex r0, [r11]\n"
    "   mov r1, #0xfa8\n"
    "   add r10, r11, #300\n"
    "   strex r2, r1, [r10]\n"
    "   ldr r1, [r10]\n"
    "   push {r0, r1, r2}\n"
    // test 4
    "   add r10, r11, #2048\n"
    "   ldrex r0, [r11]\n"
    "   ldrex r1, [r10]\n"
    "   mov r2, #0xfa8\n"
    "   mov r3, #0xc58\n"
    "   strex r4, r2, [r11]\n"
    "   strex r5, r3, [r10]\n"
    "   ldr r6, [r11]\n"
    "   ldr r7, [r10]\n"
    "   push {r0-r7}\n"
    // test 5
    "   mov r10, #3124\n"
    "   add r10, r11, r10\n"
    "   ldrex r0, [r11]\n"
    "   ldrex r1, [r10]\n"
    "   mov r2, #0x1a4\n"
    "   mov r3, #0x453\n"
    "   strex r4, r2, [r10]\n"
    "   strex r5, r3, [r11]\n"
    "   ldr r6, [r11]\n"
    "   ldr r7, [r10]\n"
    "   push {r0-r7}\n"
    // test 6
    "   mov r0, #0\n"
    "   mov r1, #0\n"
    "   mov r10, #3096\n"
    "   add r10, r11, r10\n"
    "   cmp r8, r9\n"
    "   ldrexle r0, [r11]\n"
    "   ldrexgt r1, [r10]\n"
    "   mov r2, #0x58\n"
    "   mov r3, #0x761\n"
    "   mov r4, #2\n"
    "   mov r5, #2\n"
    "   strexle r5, r3, [r11]\n"
    "   strexgt r4, r2, [r10]\n"
    "   ldr r6, [r11]\n"
    "   ldr r7, [r10]\n"
    "   push {r0-r7}\n"
    // test 7
    "   mov r0, #0\n"
    "   mov r1, #0\n"
    "   mov r10, #3080\n"
    "   add r10, r11, r10\n"
    "   cmp r8, r9\n"
    "   ldrexle r0, [r11]\n"
    "   ldrexgt r1, [r10]\n"
    "   mov r2, #0x146\n"
    "   mov r3, #0x9de\n"
    "   mov r4, #2\n"
    "   mov r5, #2\n"
    "   strexle r4, r2, [r10]\n"
    "   strexgt r5, r3, [r11]\n"
    "   ldr r6, [r11]\n"
    "   ldr r7, [r10]\n"
    "   push {r0-r7}\n"
    // test 8
    "   mov r0, #0\n"
    "   mov r1, #0\n"
    "   mov r10, #3000\n"
    "   add r10, r11, r10\n"
    "   cmp r9, r8\n"
    "   ldrexhle r0, [r11]\n"
    "   ldrexbgt r1, [r10]\n"
    "   mov r2, #0xb5\n"
    "   mov r3, #0xea\n"
    "   mov r4, #2\n"
    "   mov r5, #2\n"
    "   strexhle r5, r3, [r11]\n"
    "   strexbgt r4, r2, [r10]\n"
    "   ldr r6, [r11]\n"
    "   ldr r7, [r10]\n"
    "   push {r0-r7}\n"
    // test 9
    "   mov r0, #0\n"
    "   mov r1, #0\n"
    "   mov r10, #1032\n"
    "   add r10, r11, r10\n"
    "   cmp r9, r8\n"
    "   ldrexdle r8, r9, [r11]\n"
    "   ldrexbgt r1, [r10]\n"
    "   mov r2, #0x78\n"
    "   mov r3, #0x46d\n"
    "   mov r4, #2\n"
    "   mov r5, #2\n"
    "   strexble r5, r3, [r11]\n"
    "   strexdgt r4, r8, r9, [r10]\n"
    "   ldr r6, [r11]\n"
    "   ldr r7, [r10]\n"
    "   push {r0-r9}\n"

    "end:\n"
    "   mov sp, r12\n";
