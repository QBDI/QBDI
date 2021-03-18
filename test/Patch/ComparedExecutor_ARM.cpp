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
#include "ComparedExecutor_ARM.h"

const char CPU[] = CPU_CPU;
const char* MATTRS[] = {CPU_MATTRS, nullptr};

InMemoryObject ComparedExecutor_ARM::compileWithContextSwitch(const char* source) {
    std::ostringstream      finalSource;

    finalSource << "push {lr}\n";
    finalSource << "push {r7}\n"; // can't trust globbers for ARM frame pointer
    for(uint32_t i = 0; i < QBDI_NUM_FPR; i++) {
        finalSource << "vldr s" << i << ", [r1, #" <<
                      offsetof(QBDI::Context, fprState) + i*sizeof(float) << "]\n";
    }
    finalSource << "ldr r0, [r1, #" << offsetof(QBDI::Context, gprState.cpsr) << "]\n"
                   "msr cpsr, r0\n"
                   "ldr r0, [r1, #" << offsetof(QBDI::Context, gprState.r0) << "]\n"
                   "ldr r2, [r1, #" << offsetof(QBDI::Context, gprState.r2) << "]\n"
                   "ldr r3, [r1, #" << offsetof(QBDI::Context, gprState.r3) << "]\n"
                   "ldr r4, [r1, #" << offsetof(QBDI::Context, gprState.r4) << "]\n"
                   "ldr r5, [r1, #" << offsetof(QBDI::Context, gprState.r5) << "]\n"
                   "ldr r6, [r1, #" << offsetof(QBDI::Context, gprState.r6) << "]\n"
                   "ldr r7, [r1, #" << offsetof(QBDI::Context, gprState.r7) << "]\n"
                   "ldr r8, [r1, #" << offsetof(QBDI::Context, gprState.r8) << "]\n"
                   "ldr r9, [r1, #" << offsetof(QBDI::Context, gprState.r9) << "]\n"
                   "ldr r10, [r1, #" << offsetof(QBDI::Context, gprState.r10) << "]\n"
                   "ldr r12, [r1, #" << offsetof(QBDI::Context, gprState.r12) << "]\n"
                   "str fp, [r1, #" << offsetof(QBDI::Context, hostState.fp) << "]\n"
                   "str sp, [r1, #" << offsetof(QBDI::Context, hostState.sp) << "]\n"
                   "ldr fp, [r1, #" << offsetof(QBDI::Context, gprState.fp) << "]\n"
                   "ldr sp, [r1, #" << offsetof(QBDI::Context, gprState.sp) << "]\n"
                   "push {r1}\n"
                   "ldr r1, [r1, #" << offsetof(QBDI::Context, gprState.r1) << "]\n";
    finalSource << source;
    finalSource << "pop {fp}\n"
                   "str r0, [fp, #" << offsetof(QBDI::Context, gprState.r0) << "]\n"
                   "mrs r0, cpsr\n"
                   "str r0, [fp, #" << offsetof(QBDI::Context, gprState.cpsr) << "]\n"
                   "str r1, [fp, #" << offsetof(QBDI::Context, gprState.r1) << "]\n"
                   "str r2, [fp, #" << offsetof(QBDI::Context, gprState.r2) << "]\n"
                   "str r3, [fp, #" << offsetof(QBDI::Context, gprState.r3) << "]\n"
                   "str r4, [fp, #" << offsetof(QBDI::Context, gprState.r4) << "]\n"
                   "str r5, [fp, #" << offsetof(QBDI::Context, gprState.r5) << "]\n"
                   "str r6, [fp, #" << offsetof(QBDI::Context, gprState.r6) << "]\n"
                   "str r7, [fp, #" << offsetof(QBDI::Context, gprState.r7) << "]\n"
                   "str r8, [fp, #" << offsetof(QBDI::Context, gprState.r8) << "]\n"
                   "str r9, [fp, #" << offsetof(QBDI::Context, gprState.r9) << "]\n"
                   "str r10, [fp, #" << offsetof(QBDI::Context, gprState.r10) << "]\n"
                   "str r12, [fp, #" << offsetof(QBDI::Context, gprState.r12) << "]\n"
                   "mov r1, fp\n"
                   "ldr fp, [r1, #" << offsetof(QBDI::Context, hostState.fp) << "]\n"
                   "ldr sp, [r1, #" << offsetof(QBDI::Context, hostState.sp) << "]\n";
    for(uint32_t i = 0; i < QBDI_NUM_FPR; i++) {
        finalSource << "vstr s" << i << ", [r1, #" <<
                    offsetof(QBDI::Context, fprState) + i*sizeof(float) << "]\n";
    }
    finalSource << "pop {r7}\n";
    finalSource << "pop {pc}\n";

    return InMemoryObject(finalSource.str().c_str(), CPU, MATTRS);
}

QBDI::Context ComparedExecutor_ARM::jitExec(llvm::ArrayRef<uint8_t> code, QBDI::Context &inputState,
                       llvm::sys::MemoryBlock &stack) {
    QBDI::Context           outputState;
    QBDI::Context           outerState;
    llvm::sys::MemoryBlock  ctxBlock;
    llvm::sys::MemoryBlock  outerStack;
    std::error_code         ec;

    ctxBlock = llvm::sys::Memory::allocateMappedMemory(4096, nullptr,
                                                       PF::MF_READ | PF::MF_WRITE, ec);
    outerStack = llvm::sys::Memory::allocateMappedMemory(4096, nullptr,
                                                         PF::MF_READ | PF::MF_WRITE, ec);
    memset((void*)&outerState, 0, sizeof(QBDI::Context));
    // Put the inputState on the stack
    inputState.gprState.fp = (QBDI::rword) stack.base() + stack.allocatedSize();
    inputState.gprState.sp = (QBDI::rword) stack.base() + stack.allocatedSize();

    memcpy((void*)ctxBlock.base(), (void*)&inputState, sizeof(QBDI::Context));
    // Prepare the outerState to fake the realExec() action
    outerState.gprState.fp = (QBDI::rword) outerStack.base() + outerStack.allocatedSize();
    outerState.gprState.sp = (QBDI::rword) outerStack.base() + outerStack.allocatedSize();
    outerState.gprState.r1 = (QBDI::rword) ctxBlock.base();
    outerState.gprState.lr = (QBDI::rword) 0;

    vm.setGPRState(&outerState.gprState);
    vm.setFPRState(&outerState.fprState);
    vm.addInstrumentedRange((QBDI::rword) code.data(), (QBDI::rword) code.data() + code.size());
    vm.run((QBDI::rword) code.data(), 0);
    vm.removeInstrumentedRange((QBDI::rword) code.data(), (QBDI::rword) code.data() + code.size());

    memcpy((void*)&outputState, (void*)ctxBlock.base(), sizeof(QBDI::Context));

    llvm::sys::Memory::releaseMappedMemory(ctxBlock);
    llvm::sys::Memory::releaseMappedMemory(outerStack);

    return outputState;
}

QBDI::Context ComparedExecutor_ARM::realExec(llvm::ArrayRef<uint8_t> code,
                                                    QBDI::Context &inputState,
                                                    llvm::sys::MemoryBlock &stack) {

    QBDI::Context           outputState;
    std::error_code         ec;
    llvm::sys::MemoryBlock  ctxBlock;

    ctxBlock = llvm::sys::Memory::allocateMappedMemory(4096, nullptr,
                                                       PF::MF_READ | PF::MF_WRITE, ec);

    // Put the inputState on the stack
    inputState.gprState.fp = (QBDI::rword) stack.base() + stack.allocatedSize();
    inputState.gprState.sp = (QBDI::rword) stack.base() + stack.allocatedSize();

    // Assemble the sources
    // Copy the input context
    memcpy(ctxBlock.base(), (void*) &inputState, sizeof(QBDI::Context));
    // Execute
    const void* ctxBlockPtr = ctxBlock.base();
    __asm__ volatile(
        "ldr r1, %1;"
        "blx %0;"
        :
        :"r"(code.data()), "m" (ctxBlockPtr)
        :"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r12"
    );
    // Get the output context
    memcpy((void*) &outputState, ctxBlock.base(), sizeof(QBDI::Context));

    llvm::sys::Memory::releaseMappedMemory(ctxBlock);

    return outputState;
}

const char* GPRSave_s =
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
        "    mov r12, #12\n";

const char* GPRShuffle_s =
        "    push {r0}\n"
        "    push {r1}\n"
        "    push {r2}\n"
        "    push {r3}\n"
        "    push {r4}\n"
        "    push {r5}\n"
        "    push {r6}\n"
        "    push {r7}\n"
        "    push {r8}\n"
        "    push {r9}\n"
        "    push {r10}\n"
        "    push {r12}\n"
        "    pop {r10}\n"
        "    pop {r9}\n"
        "    pop {r8}\n"
        "    pop {r7}\n"
        "    pop {r6}\n"
        "    pop {r5}\n"
        "    pop {r4}\n"
        "    pop {r3}\n"
        "    pop {r2}\n"
        "    pop {r1}\n"
        "    pop {r0}\n"
        "    pop {r12}\n";

const char* RelativeAddressing_s =
        "b start\n"
        "c1:\n"
        "    .long 0x12345678\n"
        "start:\n"
        "    adr r4, c1\n"
        "    ldr r5, [r4]\n"
        "    eor r0, r5\n"
        "    adr r6, c2\n"
        "    ldr r7, [r6]\n"
        "    eor r1, r1, r7\n"
        "    b end\n"
        "c2:\n"
        "    .long 0x89abcdef\n"
        "end:\n";

const char* ConditionalBranching_s =
        "    push {r7}\n"
        "    push {r6}\n"
        "    push {r5}\n"
        "    push {r4}\n"
        "    push {r3}\n"
        "    push {r2}\n"
        "    push {r1}\n"
        "    push {r0}\n"
        "    mov r12, #0\n"
        "    mov r2, #0\n"
        "loop:\n"
        "    ldrb r1, [sp]\n"
        "    add sp, #1\n"
        "    eor r12, r1\n"
        "    ror r12, #12\n"
        "    add r2, #1\n"
        "    cmp r2, #32\n"
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
        "    .word 0x1fbddc9c\n"
        "end:\n";
