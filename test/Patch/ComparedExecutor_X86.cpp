/*
 * This file is part of QBDI.
 *
 * Copyright 2017 Quarkslab
 *
 * Licensed under the Apache License, Veesion 2.0 (the "License");
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
#include "ComparedExecutor_X86.h"

#if defined(_M_IX86)

extern "C" void runRealExec(const uint8_t* code, void* ctxBlock);

#endif


InMemoryObject ComparedExecutor_X86::compileWithContextSwitch(const char* source) {
    std::ostringstream      finalSource;

    // Assemble the sources
    finalSource << "mov " << offsetof(QBDI::Context, gprState.eflags) << "(%edi), %eax\n"
                   "push %eax\n"
                   "popf\n"
                   "mov " << offsetof(QBDI::Context, gprState.eax) << "(%edi), %eax\n"
                   "mov " << offsetof(QBDI::Context, gprState.ebx) << "(%edi), %ebx\n"
                   "mov " << offsetof(QBDI::Context, gprState.ecx) << "(%edi), %ecx\n"
                   "mov " << offsetof(QBDI::Context, gprState.edx) << "(%edi), %edx\n"
                   "mov " << offsetof(QBDI::Context, gprState.esi) << "(%edi), %esi\n"
                   "mov %ebp, " << offsetof(QBDI::Context, hostState.bp) << "(%edi)\n"
                   "mov %esp, " << offsetof(QBDI::Context, hostState.sp) << "(%edi)\n"
                   "mov " << offsetof(QBDI::Context, gprState.ebp) << "(%edi), %ebp\n"
                   "mov " << offsetof(QBDI::Context, gprState.esp) << "(%edi), %esp\n"
                   "push %edi\n"
                   "mov " << offsetof(QBDI::Context, gprState.edi) << "(%edi), %edi\n";
    finalSource << source;
    finalSource << "pop %esp\n"
                   "mov %eax, " << offsetof(QBDI::Context, gprState.eax) << "(%esp)\n"
                   "mov %ebx, " << offsetof(QBDI::Context, gprState.ebx) << "(%esp)\n"
                   "mov %ecx, " << offsetof(QBDI::Context, gprState.ecx) << "(%esp)\n"
                   "mov %edx, " << offsetof(QBDI::Context, gprState.edx) << "(%esp)\n"
                   "mov %esi, " << offsetof(QBDI::Context, gprState.esi) << "(%esp)\n"
                   "mov %edi, " << offsetof(QBDI::Context, gprState.edi) << "(%esp)\n"
                   "mov %ebp, " << offsetof(QBDI::Context, gprState.ebp) << "(%esp)\n"
                   "mov " << offsetof(QBDI::Context, hostState.bp) << "(%esp), %ebp\n"
                   "mov %esp, %edi\n"
                   "mov " << offsetof(QBDI::Context, hostState.sp) << "(%esp), %esp\n"
                   "pushf\n"
                   "pop %eax\n"
                   "mov %eax, " << offsetof(QBDI::Context, gprState.eflags) << "(%edi)\n"
                   "ret\n";

    return InMemoryObject(finalSource.str().c_str());
}

QBDI::Context ComparedExecutor_X86::jitExec(llvm::ArrayRef<uint8_t> code, QBDI::Context &inputState,
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
    inputState.gprState.ebp = (QBDI::rword) stack.base() + stack.allocatedSize();
    inputState.gprState.esp = (QBDI::rword) stack.base() + stack.allocatedSize();

    memcpy((void*)ctxBlock.base(), (void*)&inputState, sizeof(QBDI::Context));
    // Prepare the outerState to fake the realExec() action
    outerState.gprState.ebp = (QBDI::rword) outerStack.base() + outerStack.allocatedSize();
    outerState.gprState.esp = (QBDI::rword) outerStack.base() + outerStack.allocatedSize() - sizeof(QBDI::rword);
    *((QBDI::rword*)outerState.gprState.esp) = (QBDI::rword) 0;
    outerState.gprState.edi = (QBDI::rword) ctxBlock.base();

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

QBDI::Context ComparedExecutor_X86::realExec(llvm::ArrayRef<uint8_t> code,
                                             QBDI::Context &inputState,
                                             llvm::sys::MemoryBlock &stack) {

    QBDI::Context           outputState;
    std::error_code         ec;
    llvm::sys::MemoryBlock  ctxBlock;

    ctxBlock = llvm::sys::Memory::allocateMappedMemory(4096, nullptr,
                                                       PF::MF_READ | PF::MF_WRITE, ec);

    // Put the inputState on the stack
    QBDI_GPR_SET(&inputState.gprState, QBDI::REG_BP, (QBDI::rword) stack.base() + stack.allocatedSize());
    QBDI_GPR_SET(&inputState.gprState, QBDI::REG_SP, (QBDI::rword) stack.base() + stack.allocatedSize());

    // Copy the input context
    memcpy(ctxBlock.base(), (void*) &inputState, sizeof(QBDI::Context));
    // Execute
    #if defined(_M_IX86)
    runRealExec(code.data(), ctxBlock.base());
    #else
    __asm__ volatile(
        "mov %1, %%edi;"
        "call *%0;"
        :
        :"rm"(code.data()), "rm" (ctxBlock.base())
        :"eax", "ebx", "ecx", "edx", "edi", "esi"
    );
    #endif
    // Get the output context
    memcpy((void*) &outputState, ctxBlock.base(), sizeof(QBDI::Context));

    llvm::sys::Memory::releaseMappedMemory(ctxBlock);

    return outputState;
}

const char* GPRSave_s =
        "    mov $0x1, %eax\n"
        "    mov $0x2, %ebx\n"
        "    mov $0x3, %ecx\n"
        "    mov $0x4, %edx\n"
        "    mov $0x5, %esi\n"
        "    mov $0x6, %edi\n";

const char* GPRShuffle_s =
        "    pushal\n"
        "    popal\n"
        "    push %eax\n"
        "    push %ebx\n"
        "    push %ecx\n"
        "    push %edx\n"
        "    push %esi\n"
        "    push %edi\n"
        "    pop %esi\n"
        "    pop %edx\n"
        "    pop %ecx\n"
        "    pop %ebx\n"
        "    pop %eax\n"
        "    pop %edi\n";

const char* RelativeAddressing_s =
        "jmp start\n"
        "c1:\n"
        "    .long 0x12345678\n"
        "start:\n"
        "    push %ebp\n"
        "    call L1\n"
        "L1:\n"
        "    pop %ebp\n"
        "    lea c1-L1(%ebp), %esi\n"
        "    mov c1-L1(%ebp), %ecx\n"
        "    xor %ecx, %eax\n"
        "    xor %esi, %esi\n"
        "    lea c2-L1(%ebp), %edi\n"
        "    mov c2-L1(%ebp), %edx\n"
        "    xor %edx, %ebx\n"
        "    xor %edi, %ebx\n"
        "    jmp end\n"
        "c2:\n"
        "    .long 0x0fedcba9\n"
        "end:\n"
        "    pop %ebp\n"
        "    movl $0x666, -4(%esp)\n";

const char* ConditionalBranching_s =
        "    push %edx\n"
        "    push %ecx\n"
        "    push %ebx\n"
        "    push %eax\n"
        "    call L1\n"
        "L1:\n"
        "    pop %ebx\n"
        "    xor %ecx, %ecx\n"
        "    xor %edx, %edx\n"
        "loop:\n"
        "    movzx (%esp), %ax\n"
        "    inc %esp\n"
        "    xor %al, %dl\n"
        "    ror $12, %edx\n"
        "    inc %ecx\n"
        "    cmpl $16, %ecx\n"
        "    jb loop\n"
        "    lea cheksum-L1(%ebx), %esi\n"
        "    mov (%esi), %edi\n"
        "    cmp %edx, %edi\n"
        "    jne bad\n"
        "    mov $1, %eax\n"
        "    jmp end\n"
        "bad:\n"
        "    mov $0, %eax\n"
        "    jmp end\n"
        "checksum:\n"
        "    .long 0x32253676\n"
        "end:\n";

const char* FibonacciRecursion_s =
        "   call L1\n"
        "L1:\n"
        "   pop %esi\n"
        "   lea fibo-L1(%esi), %ebx\n"
        "   push %ebx\n"
        "   call *0x0(%esp)\n"
        "   jmp end\n"
        "fibo:\n"
        "   sub $0x8, %esp\n"
        "   cmp $2, %eax\n"
        "   ja fibo1\n"
        "   mov $1, %eax\n"
        "   jmp fibo2\n"
        "fibo1:\n"
        "   dec %eax\n"
        "   mov %eax, 0x4(%esp)\n"
        "   call fibo\n"
        "   mov %eax, 0x0(%esp)\n"
        "   mov 0x4(%esp), %eax\n"
        "   dec %eax\n"
        "   lea fibo-L1(%esi), %ebx\n"
        "   call *%ebx\n"
        "   add 0x0(%esp), %eax\n"
        "fibo2:\n"
        "   add $0x8, %esp\n"
        "   ret\n"
        "end:\n"
        "   pop %ebx\n";

const char* StackTricks_s =
        "   push %ebp\n"
        "   call L1\n"
        "L1:\n"
        "   pop %ebp\n"
        "   lea end-L1(%ebp), %ecx\n"
        "   lea f1-L1(%ebp), %edx\n"
        "   push %eax\n"
        "   push %ecx\n"
        "   push %edx\n"
        "   ret\n"
        "f1:\n"
        "   mov 0x4(%esp), %eax\n"
        "   lea f2-L1(%ebp), %ecx\n"
        "   lea f6-L1(%ebp), %edx\n"
        "   mov $1, %esi\n"
        "   cmp $2, %eax\n"
        "   cmova %ecx, %edx\n"
        "   push %edx\n"
        "   ret\n"
        "f2:\n"
        "   dec %eax\n"
        "   lea f4-L1(%ebp), %ecx\n"
        "   lea f1-L1(%ebp), %edx\n"
        "   push %eax\n"
        "   push %ecx\n"
        "   push %edx\n"
        "   ret\n"
        "f4:\n"
        "   add %esi, %ebx\n"
        "   dec %eax\n"
        "   lea f5-L1(%ebp), %ecx\n"
        "   lea f1-L1(%ebp), %edx\n"
        "   push %eax\n"
        "   push %ecx\n"
        "   push %edx\n"
        "   ret\n"
        "f5:\n"
        "   add %ebx, %esi\n"
        "f6:\n"
        "   mov 0x4(%esp), %eax\n"
        "   ret $0x4\n"
        "end:\n"
        "   pop %ebp\n"
        "   movl $0x666, -4(%esp)\n";
