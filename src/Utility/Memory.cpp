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
#include "llvm/Support/Process.h"

#include <set>
#include "QBDI/Memory.h"
#include "QBDI/Memory.hpp"
#include "Utility/LogSys.h"

#define FRAME_LENGTH 16

namespace QBDI {

// C++ method
std::vector<std::string> getModuleNames() {
  std::set<std::string> modules;

  for (const MemoryMap &m : getCurrentProcessMaps(false))
    if (!m.name.empty())
      modules.insert(m.name);

  return {std::begin(modules), std::end(modules)};
}

void *alignedAlloc(size_t size, size_t align) {
  void *allocated = nullptr;
  // Alignment needs to be a power of 2
  if ((align == 0) || ((align & (align - 1)) != 0)) {
    return nullptr;
  }
#if defined(QBDI_PLATFORM_LINUX) || defined(QBDI_PLATFORM_ANDROID) || \
    defined(QBDI_PLATFORM_OSX)
  int ret = posix_memalign(&allocated, align, size);
  if (ret != 0) {
    return nullptr;
  }
#elif defined(QBDI_PLATFORM_WINDOWS)
  allocated = _aligned_malloc(size, align);
#endif
  return allocated;
}

void alignedFree(void *ptr) {
#if defined(QBDI_PLATFORM_LINUX) || defined(QBDI_PLATFORM_ANDROID) || \
    defined(QBDI_PLATFORM_OSX)
  free(ptr);
#elif defined(QBDI_PLATFORM_WINDOWS)
  _aligned_free(ptr);
#endif
}

bool allocateVirtualStack(GPRState *ctx, uint32_t stackSize, uint8_t **stack) {
  (*stack) = (uint8_t *)alignedAlloc(stackSize, 16);
  if (*stack == nullptr) {
    return false;
  }

  QBDI_GPR_SET(ctx, REG_SP, (QBDI::rword)(*stack) + stackSize);
  QBDI_GPR_SET(ctx, REG_BP, QBDI_GPR_GET(ctx, REG_SP));

  return true;
}

void simulateCall(GPRState *ctx, rword returnAddress,
                  const std::vector<rword> &args) {
  simulateCallA(ctx, returnAddress, args.size(), args.data());
}

void simulateCallV(GPRState *ctx, rword returnAddress, uint32_t argNum,
                   va_list ap) {
  std::vector<rword> args(argNum);
  for (uint32_t i = 0; i < argNum; i++) {
    args[i] = va_arg(ap, rword);
  }
  simulateCallA(ctx, returnAddress, argNum, args.data());
}

void simulateCallA(GPRState *ctx, rword returnAddress, uint32_t argNum,
                   const rword *args) {
  uint32_t i = 0;
  uint32_t argsoff = 0;
  uint32_t limit = FRAME_LENGTH;

  // Allocate arguments frame
  QBDI_GPR_SET(ctx, REG_SP,
               QBDI_GPR_GET(ctx, REG_SP) - FRAME_LENGTH * sizeof(rword));

  // Handle the return address
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
  QBDI_GPR_SET(ctx, REG_SP, QBDI_GPR_GET(ctx, REG_SP) - sizeof(rword));
  *(rword *)(QBDI_GPR_GET(ctx, REG_SP)) = returnAddress;
  argsoff++;
#elif defined(QBDI_ARCH_ARM)
  ctx->lr = returnAddress;
#endif

#define UNSTACK_ARG(REG)  \
  if (i < argNum) {       \
    ctx->REG = args[i++]; \
  }
#if defined(QBDI_ARCH_X86_64)
#if defined(QBDI_PLATFORM_WINDOWS)
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
#elif defined(QBDI_ARCH_X86)
  // no register used
#elif defined(QBDI_ARCH_ARM)
  UNSTACK_ARG(r0);
  UNSTACK_ARG(r1);
  UNSTACK_ARG(r2);
  UNSTACK_ARG(r3);
#endif // ARCH
#undef UNSTACK_ARG
  limit -= argsoff;

  // Push remaining args on the stack
  rword *frame = (rword *)QBDI_GPR_GET(ctx, REG_SP);
  for (uint32_t j = 0; (i + j) < argNum && j < limit; j++) {
    frame[argsoff + j] = args[i + j];
  }
}

// C method
qbdi_MemoryMap *convert_MemoryMap_to_C(std::vector<MemoryMap> maps,
                                       size_t *size) {
  if (size == NULL)
    return NULL;
  *size = maps.size();
  if (*size == 0) {
    return NULL;
  }
  qbdi_MemoryMap *cmaps =
      (qbdi_MemoryMap *)malloc(*size * sizeof(qbdi_MemoryMap));
  QBDI_REQUIRE_ACTION(cmaps != NULL, abort());
  for (size_t i = 0; i < *size; i++) {
    cmaps[i].start = maps[i].range.start();
    cmaps[i].end = maps[i].range.end();
    cmaps[i].permission = static_cast<qbdi_Permission>(maps[i].permission);
    cmaps[i].name = strdup(maps[i].name.c_str());
  }
  return cmaps;
}

qbdi_MemoryMap *qbdi_getRemoteProcessMaps(rword pid, bool full_path,
                                          size_t *size) {
  if (size == NULL)
    return NULL;
  return convert_MemoryMap_to_C(getRemoteProcessMaps(pid, full_path), size);
}

qbdi_MemoryMap *qbdi_getCurrentProcessMaps(bool full_path, size_t *size) {
  if (size == NULL)
    return NULL;
  return convert_MemoryMap_to_C(getCurrentProcessMaps(full_path), size);
}

void qbdi_freeMemoryMapArray(qbdi_MemoryMap *arr, size_t size) {
  for (size_t i = 0; i < size; i++) {
    if (arr[i].name) {
      free(arr[i].name);
    }
  }
  free(arr);
}

char **qbdi_getModuleNames(size_t *size) {
  if (size == NULL)
    return NULL;
  std::vector<std::string> modules = getModuleNames();
  *size = modules.size();
  if (*size == 0) {
    return NULL;
  }
  char **names = (char **)malloc(modules.size() * sizeof(char *));
  QBDI_REQUIRE_ACTION(names != NULL, abort());
  for (size_t i = 0; i < modules.size(); i++) {
    names[i] = strdup(modules[i].c_str());
  }
  return names;
}

void *qbdi_alignedAlloc(size_t size, size_t align) {
  return alignedAlloc(size, align);
}

void qbdi_alignedFree(void *ptr) { alignedFree(ptr); }

bool qbdi_allocateVirtualStack(GPRState *ctx, uint32_t stackSize,
                               uint8_t **stack) {
  return allocateVirtualStack(ctx, stackSize, stack);
}

void qbdi_simulateCall(GPRState *ctx, rword returnAddress, uint32_t argNum,
                       ...) {
  va_list ap;
  // Handle the arguments
  va_start(ap, argNum);
  simulateCallV(ctx, returnAddress, argNum, ap);
  va_end(ap);
}
void qbdi_simulateCallV(GPRState *ctx, rword returnAddress, uint32_t argNum,
                        va_list ap) {
  simulateCallV(ctx, returnAddress, argNum, ap);
}

void qbdi_simulateCallA(GPRState *ctx, rword returnAddress, uint32_t argNum,
                        const rword *args) {
  simulateCallA(ctx, returnAddress, argNum, args);
}

} // namespace QBDI
