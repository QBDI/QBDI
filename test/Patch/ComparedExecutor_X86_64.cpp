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
#include "ComparedExecutor_X86_64.h"

#if defined(_M_X64)

extern "C" void runRealExec(const uint8_t* code, void* ctxBlock);

#endif


InMemoryObject ComparedExecutor_X86_64::compileWithContextSwitch(const char* source) {
    std::ostringstream      finalSource;

    // Assemble the sources
    finalSource << "mov " << offsetof(QBDI::Context, gprState.eflags) << "(%rdi), %rax\n"
                   "push %rax\n"
                   "popfq\n"
                   "mov " << offsetof(QBDI::Context, gprState.rax) << "(%rdi), %rax\n"
                   "mov " << offsetof(QBDI::Context, gprState.rbx) << "(%rdi), %rbx\n"
                   "mov " << offsetof(QBDI::Context, gprState.rcx) << "(%rdi), %rcx\n"
                   "mov " << offsetof(QBDI::Context, gprState.rdx) << "(%rdi), %rdx\n"
                   "mov " << offsetof(QBDI::Context, gprState.rsi) << "(%rdi), %rsi\n"
                   "mov " << offsetof(QBDI::Context, gprState.r8) << "(%rdi), %r8\n"
                   "mov " << offsetof(QBDI::Context, gprState.r9) << "(%rdi), %r9\n"
                   "mov " << offsetof(QBDI::Context, gprState.r10) << "(%rdi), %r10\n"
                   "mov " << offsetof(QBDI::Context, gprState.r11) << "(%rdi), %r11\n"
                   "mov " << offsetof(QBDI::Context, gprState.r12) << "(%rdi), %r12\n"
                   "mov " << offsetof(QBDI::Context, gprState.r13) << "(%rdi), %r13\n"
                   "mov " << offsetof(QBDI::Context, gprState.r14) << "(%rdi), %r14\n"
                   "mov " << offsetof(QBDI::Context, gprState.r15) << "(%rdi), %r15\n"
                   "mov %rbp, " << offsetof(QBDI::Context, hostState.bp) << "(%rdi)\n"
                   "mov %rsp, " << offsetof(QBDI::Context, hostState.sp) << "(%rdi)\n"
                   "mov " << offsetof(QBDI::Context, gprState.rbp) << "(%rdi), %rbp\n"
                   "mov " << offsetof(QBDI::Context, gprState.rsp) << "(%rdi), %rsp\n"
                   "push %rdi\n"
                   "mov " << offsetof(QBDI::Context, gprState.rdi) << "(%rdi), %rdi\n";
    finalSource << source;
    finalSource << "pop %rsp\n"
                   "mov %rax, " << offsetof(QBDI::Context, gprState.rax) << "(%rsp)\n"
                   "mov %rbx, " << offsetof(QBDI::Context, gprState.rbx) << "(%rsp)\n"
                   "mov %rcx, " << offsetof(QBDI::Context, gprState.rcx) << "(%rsp)\n"
                   "mov %rdx, " << offsetof(QBDI::Context, gprState.rdx) << "(%rsp)\n"
                   "mov %rsi, " << offsetof(QBDI::Context, gprState.rsi) << "(%rsp)\n"
                   "mov %rdi, " << offsetof(QBDI::Context, gprState.rdi) << "(%rsp)\n"
                   "mov %r8, " << offsetof(QBDI::Context, gprState.r8) << "(%rsp)\n"
                   "mov %r9, " << offsetof(QBDI::Context, gprState.r9) << "(%rsp)\n"
                   "mov %r10, " << offsetof(QBDI::Context, gprState.r10) << "(%rsp)\n"
                   "mov %r11, " << offsetof(QBDI::Context, gprState.r11) << "(%rsp)\n"
                   "mov %r12, " << offsetof(QBDI::Context, gprState.r12) << "(%rsp)\n"
                   "mov %r13, " << offsetof(QBDI::Context, gprState.r13) << "(%rsp)\n"
                   "mov %r14, " << offsetof(QBDI::Context, gprState.r14) << "(%rsp)\n"
                   "mov %r15, " << offsetof(QBDI::Context, gprState.r15) << "(%rsp)\n"
                   "mov %rbp, " << offsetof(QBDI::Context, gprState.rbp) << "(%rsp)\n"
                   "mov " << offsetof(QBDI::Context, hostState.bp) << "(%rsp), %rbp\n"
                   "mov %rsp, %rdi\n"
                   "mov " << offsetof(QBDI::Context, hostState.sp) << "(%rsp), %rsp\n"
                   "pushfq\n"
                   "pop %rax\n"
                   "mov %rax, " << offsetof(QBDI::Context, gprState.eflags) << "(%rdi)\n"
                   "ret\n";

    return InMemoryObject(finalSource.str().c_str());
}

QBDI::Context ComparedExecutor_X86_64::jitExec(llvm::ArrayRef<uint8_t> code, QBDI::Context &inputState, 
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
    inputState.gprState.rbp = (QBDI::rword) stack.base() + stack.size();
    inputState.gprState.rsp = (QBDI::rword) stack.base() + stack.size();

    memcpy((void*)ctxBlock.base(), (void*)&inputState, sizeof(QBDI::Context));
    // Prepare the outerState to fake the realExec() action
    outerState.gprState.rbp = (QBDI::rword) outerStack.base() + outerStack.size();
    outerState.gprState.rsp = (QBDI::rword) outerStack.base() + outerStack.size() - sizeof(QBDI::rword);
    *((QBDI::rword*)outerState.gprState.rsp) = (QBDI::rword) 0;
    outerState.gprState.rdi = (QBDI::rword) ctxBlock.base();

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

QBDI::Context ComparedExecutor_X86_64::realExec(llvm::ArrayRef<uint8_t> code, 
                                                    QBDI::Context &inputState, 
                                                    llvm::sys::MemoryBlock &stack) {

    QBDI::Context           outputState;
    std::error_code         ec;
    llvm::sys::MemoryBlock  ctxBlock;

    ctxBlock = llvm::sys::Memory::allocateMappedMemory(4096, nullptr, 
                                                       PF::MF_READ | PF::MF_WRITE, ec);

    // Put the inputState on the stack
    QBDI_GPR_SET(&inputState.gprState, QBDI::REG_BP, (QBDI::rword) stack.base() + stack.size());
    QBDI_GPR_SET(&inputState.gprState, QBDI::REG_SP, (QBDI::rword) stack.base() + stack.size());

    // Copy the input context
    memcpy(ctxBlock.base(), (void*) &inputState, sizeof(QBDI::Context));
    // Execute
    #if defined(_M_X64)
    runRealExec(code.data(), ctxBlock.base());
    #else
    __asm__ volatile(
        "mov %1, %%rdi;"
        "call *%0;"
        :
        :"rm"(code.data()), "rm" (ctxBlock.base())
        :"rax", "rbx", "rcx", "rdx", "rdi", "rsi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
    );
    #endif
    // Get the output context
    memcpy((void*) &outputState, ctxBlock.base(), sizeof(QBDI::Context));

    llvm::sys::Memory::releaseMappedMemory(ctxBlock);

    return outputState;
}

const char* GPRSave_s =
        "    mov $0x1, %rax\n"
        "    mov $0x2, %rbx\n"
        "    mov $0x3, %rcx\n"
        "    mov $0x4, %rdx\n"
        "    mov $0x5, %rsi\n"
        "    mov $0x6, %rdi\n"
        "    mov $0x7, %r8\n"
        "    mov $0x8, %r9\n"
        "    mov $0x9, %r10\n"
        "    mov $0xa, %r11\n"
        "    mov $0xb, %r12\n"
        "    mov $0xc, %r13\n"
        "    mov $0xd, %r14\n"
        "    mov $0xe, %r15\n";

const char* GPRShuffle_s =
        "    push %rax\n"
        "    push %rbx\n"
        "    push %rcx\n"
        "    push %rdx\n"
        "    push %rsi\n"
        "    push %rdi\n"
        "    push %r8\n"
        "    push %r9\n"
        "    push %r10\n"
        "    push %r11\n"
        "    push %r12\n"
        "    push %r13\n"
        "    push %r14\n"
        "    push %r15\n"
        "    pop %r14\n"
        "    pop %r13\n"
        "    pop %r12\n"
        "    pop %r11\n"
        "    pop %r10\n"
        "    pop %r9\n"
        "    pop %r8\n"
        "    pop %rdi\n"
        "    pop %rsi\n"
        "    pop %rdx\n"
        "    pop %rcx\n"
        "    pop %rbx\n"
        "    pop %rax\n"
        "    pop %r15\n";

const char* RelativeAddressing_s =
        "jmp start\n"
        "c1:\n"
        "    .quad 0x123456789abcdef0\n"
        "start:\n"
        "    leaq c1(%rip), %rsi\n"
        "    movq c1(%rip), %rcx\n"
        "    xor %rcx, %rax\n"
        "    xor %rsi, %rsi\n"
        "    leaq c2(%rip), %rdi\n"
        "    movq c2(%rip), %rdx\n"
        "    xor %rdx, %rbx\n"
        "    xor %rdi, %rbx\n"
        "    jmp end\n"
        "c2:\n"
        "    .quad 0x0fedcba987654321\n"
        "end:\n";

const char* ConditionalBranching_s =
        "    push %rdx\n"
        "    push %rcx\n"
        "    push %rbx\n"
        "    push %rax\n"
        "    xor %rcx, %rcx\n"
        "    xor %rdx, %rdx\n"
        "loop:\n"
        "    movzx (%rsp), %ax\n"
        "    inc %rsp\n"
        "    xor %al, %dl\n"
        "    ror $12, %rdx\n"
        "    inc %rcx\n"
        "    cmpq $32, %rcx\n"
        "    jb loop\n"
        "    lea cheksum(%rip), %rsi\n"
        "    mov (%rsi), %rdi\n"
        "    cmp %rdx, %rdi\n"
        "    jne bad\n"
        "    mov $1, %rax\n"
        "    jmp end\n"
        "bad:\n"
        "    mov $0, %rax\n"
        "    jmp end\n"
        "checksum:\n"
        "    .quad 0x32253676ffe8dd7f\n"
        "end:\n";

const char* FibonacciRecursion_s = 
        "   lea fibo(%rip), %rbx\n"
        "   push %rbx\n"
        "   call *0x0(%rsp)\n"
        "   jmp end\n"
        "fibo:\n"
        "   sub $0x10, %rsp\n"
        "   cmp $2, %rax\n"
        "   ja fibo1\n"
        "   mov $1, %rax\n"
        "   jmp fibo2\n"
        "fibo1:\n"
        "   dec %rax\n"
        "   mov %rax, 0x8(%rsp)\n"
        "   call fibo\n"
        "   mov %rax, 0x0(%rsp)\n"
        "   mov 0x8(%rsp), %rax\n"
        "   dec %rax\n"
        "   lea fibo(%rip), %rbx\n"
        "   call *%rbx\n"
        "   add 0x0(%rsp), %rax\n"
        "fibo2:\n"
        "   add $0x10, %rsp\n"
        "   ret\n"
        "end:\n"
        "   pop %rbx\n";

const char* StackTricks_s =
        "   lea end(%rip), %rcx\n"
        "   lea f1(%rip), %rdx\n"
        "   push %rax\n"
        "   push %rcx\n"
        "   push %rdx\n"
        "   ret\n"
        "f1:\n"
        "   mov 0x8(%rsp), %rax\n"
        "   lea f2(%rip), %rcx\n"
        "   lea f6(%rip), %rdx\n"
        "   mov $1, %r8\n"
        "   cmp $2, %rax\n"
        "   cmova %rcx, %rdx\n"
        "   push %rdx\n"
        "   ret\n"   
        "f2:\n"
        "   dec %rax\n"
        "   lea f4(%rip), %rcx\n"
        "   lea f1(%rip), %rdx\n"
        "   push %rax\n"
        "   push %rcx\n"
        "   push %rdx\n"
        "   ret\n"
        "f4:\n"
        "   add %r8, %rbx\n"
        "   dec %rax\n"
        "   lea f5(%rip), %rcx\n"
        "   lea f1(%rip), %rdx\n"
        "   push %rax\n"
        "   push %rcx\n"
        "   push %rdx\n"
        "   ret\n"
        "f5:\n"
        "   add %rbx, %r8\n"
        "f6:\n"
        "   mov 0x8(%rsp), %rax\n"
        "   ret $0x8\n"
        "end:\n";
