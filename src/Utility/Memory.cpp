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
#include "llvm/Support/Process.h"

#include "Utility/LogSys.h"
#include "Memory.h"

#define FRAME_LENGTH       16


namespace QBDI {

std::vector<std::string> getModuleNames() {
    std::vector<std::string> modules;

    for(const MemoryMap& m : getCurrentProcessMaps()) {
        if(m.name != "") {
            bool exist = false;

            for(const std::string& s : modules) {
                if(s == m.name) {
                    exist = true;
                }
            }

            if(!exist) {
                modules.push_back(m.name);
            }
        }
    }

    return modules;
}


char** qbdi_getModuleNames(size_t* size) {
    std::vector<std::string> modules = getModuleNames();
    *size = modules.size();
    if(*size == 0) {
        return NULL;
    }
    char** names = (char**) malloc(modules.size() * sizeof(char**));
    for(size_t i = 0; i < modules.size(); i++) {
        names[i] = (char*) malloc((modules[i].length() + 1) * sizeof(char));
        strncpy(names[i], modules[i].c_str(), modules[i].length() + 1);
    }
    return names;
}


bool qbdi_allocateVirtualStack(GPRState *ctx, uint32_t stackSize, uint8_t **stack) {
    (*stack) = (uint8_t*) alignedAlloc(stackSize, 16);
    if(*stack == nullptr) {
        return false;
    }

    QBDI_GPR_SET(ctx, REG_SP, (QBDI::rword) (*stack) + stackSize);
    QBDI_GPR_SET(ctx, REG_BP, QBDI_GPR_GET(ctx, REG_SP));

    return true;
}

void qbdi_simulateCallA(GPRState *ctx, rword returnAddress, uint32_t argNum, const rword* args) {
    uint32_t i = 0;
    uint32_t argsoff = 0;
    uint32_t limit = FRAME_LENGTH;

    // Allocate arguments frame
    QBDI_GPR_SET(ctx, REG_SP, QBDI_GPR_GET(ctx, REG_SP) - FRAME_LENGTH * sizeof(rword));

    // Handle the return address
#if defined(QBDI_ARCH_X86_64)
    ctx->rsp -= sizeof(rword);
    *((rword*) ctx->rsp) = returnAddress;
    argsoff++;
#elif defined(QBDI_ARCH_ARM)
    ctx->lr = returnAddress;
#endif

#define UNSTACK_ARG(REG) if (i < argNum) { ctx->REG = args[i++]; }
#if defined(QBDI_ARCH_X86_64)
 #if defined(QBDI_OS_WIN)
    // Shadow space
    argsoff += 4;
    // Register args
    UNSTACK_ARG(rcx);
    UNSTACK_ARG(rdx);
    UNSTACK_ARG(r8);
    UNSTACK_ARG(r9);
 #else
    // Register args
    UNSTACK_ARG(rdi);
    UNSTACK_ARG(rsi);
    UNSTACK_ARG(rdx);
    UNSTACK_ARG(rcx);
    UNSTACK_ARG(r8);
    UNSTACK_ARG(r9);
 #endif // OS
#elif defined(QBDI_ARCH_ARM)
    UNSTACK_ARG(r0);
    UNSTACK_ARG(r1);
    UNSTACK_ARG(r2);
    UNSTACK_ARG(r3);
#endif // ARCH
#undef UNSTACK_ARG
    limit -= argsoff;

    // Push remaining args on the stack
    rword* frame = (rword*) QBDI_GPR_GET(ctx, REG_SP);
    for (uint32_t j = 0; (i + j) < argNum && j < limit; j++) {
        frame[argsoff + j] = args[i + j];
    }
}

void qbdi_simulateCallV(GPRState *ctx, rword returnAddress, uint32_t argNum, va_list ap) {
    rword* args = new rword[argNum];
    for(uint32_t i = 0; i < argNum; i++) {
        args[i] = va_arg(ap, rword);
    }
    qbdi_simulateCallA(ctx, returnAddress, argNum, args);
    delete[] args;
}

void qbdi_simulateCall(GPRState *ctx, rword returnAddress, uint32_t argNum, ...) {
    va_list  ap;
    // Handle the arguments
    va_start(ap, argNum);
    qbdi_simulateCallV(ctx, returnAddress, argNum, ap);
    va_end(ap);
}

void* qbdi_alignedAlloc(size_t size, size_t align) {
    void* allocated = nullptr;
    // Alignment needs to be a power of 2
    if ((align == 0) || ((align & (align - 1)) != 0)) {
        return nullptr;
    }
#if defined(QBDI_OS_LINUX) || defined(QBDI_OS_ANDROID) || defined(QBDI_OS_DARWIN)
    int ret = posix_memalign(&allocated, align, size);
    if (ret != 0) {
        return nullptr;
    }
#elif defined(QBDI_OS_WIN)
    allocated = _aligned_malloc(size, align);
#endif
    return allocated;
}

void qbdi_alignedFree(void* ptr) {
#if defined(QBDI_OS_LINUX) || defined(QBDI_OS_ANDROID) || defined(QBDI_OS_DARWIN)
    free(ptr);
#elif defined(QBDI_OS_WIN)
    _aligned_free(ptr);
#endif
}

}
