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

#include "MemAccessTestUtils_X86_64.h"

#include "QBDI/Config.h"

#include <cstdint>

#if defined(QBDI_PLATFORM_LINUX)
#include <sys/mman.h>
#ifndef MAP_32BIT
#define MAP_32BIT 0x40
#endif
#elif defined(QBDI_PLATFORM_MACOS)
#include <sys/mman.h>
#elif defined(QBDI_PLATFORM_WINDOWS)
#include <windows.h>

#include <memoryapi.h>
#endif

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

namespace {

class Low32BitBuffer {
public:
  static constexpr size_t kSize = 4096;

  Low32BitBuffer() {
#if defined(QBDI_PLATFORM_LINUX)
    void *p = mmap(nullptr, kSize, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
    if (p != MAP_FAILED &&
        (reinterpret_cast<uintptr_t>(p) + kSize) < 0x100000000ULL) {
      ptr_ = static_cast<uint8_t *>(p);
    }
#elif defined(QBDI_PLATFORM_MACOS)
    // no MAP_32BIT on macOS: pass a low hint address without MAP_FIXED,
    // honored on a best-effort basis by the kernel.
    void *hint = reinterpret_cast<void *>(0x10000000ULL);
    void *p = mmap(hint, kSize, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p != MAP_FAILED) {
      if ((reinterpret_cast<uintptr_t>(p) + kSize) < 0x100000000ULL) {
        ptr_ = static_cast<uint8_t *>(p);
      } else {
        munmap(p, kSize);
      }
    }
#elif defined(QBDI_PLATFORM_WINDOWS)
    MEM_ADDRESS_REQUIREMENTS addrReq = {};
    addrReq.LowestStartingAddress = nullptr;
    addrReq.HighestEndingAddress = reinterpret_cast<PVOID>(0xffffffffULL);
    addrReq.Alignment = 0;
    MEM_EXTENDED_PARAMETER param = {};
    param.Type = MemExtendedParameterAddressRequirements;
    param.Pointer = &addrReq;
    void *p = VirtualAlloc2(nullptr, nullptr, kSize, MEM_RESERVE | MEM_COMMIT,
                            PAGE_READWRITE, &param, 1);
    if (p != nullptr &&
        (reinterpret_cast<uintptr_t>(p) + kSize) < 0x100000000ULL) {
      ptr_ = static_cast<uint8_t *>(p);
    }
#endif
  }

  ~Low32BitBuffer() {
#if defined(QBDI_PLATFORM_LINUX) or defined(QBDI_PLATFORM_MACOS)
    if (ptr_ != nullptr) {
      munmap(ptr_, kSize);
    }
#elif defined(QBDI_PLATFORM_WINDOWS)
    if (ptr_ != nullptr) {
      VirtualFree(ptr_, 0, MEM_RELEASE);
    }
#endif
  }

  bool valid() const { return ptr_ != nullptr; }
  uint8_t *data() const { return ptr_; }

private:
  uint8_t *ptr_ = nullptr;
};

} // namespace

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-FXRSTOR") {
  const char source[] = "fxrstor 0x20(%rbx,%rsi,8)\n";
  alignas(16) uint8_t buffer[560] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FXRSTOR", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FXRSTOR", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-FXRSTOR64") {
  const char source[] = "fxrstor64 0x20(%rbx,%rsi,8)\n";
  alignas(16) uint8_t buffer[560] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FXRSTOR64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FXRSTOR64", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-FXSAVE") {
  const char source[] = "fxsave 0x20(%rbx,%rsi,8)\n";
  alignas(16) uint8_t buffer[560] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 512, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FXSAVE", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-FXSAVE64") {
  const char source[] = "fxsave64 0x20(%rbx,%rsi,8)\n";
  alignas(16) uint8_t buffer[560] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 512, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FXSAVE64", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOVDIR64B32") {
  if (!QBDI::isHostCPUFeaturePresent("movdir64b")) {
    return;
  }
  Low32BitBuffer src;
  Low32BitBuffer dst;
  if (!src.valid() or !dst.valid()) {
    WARN("No verified sub-4GB buffer available on this platform: SKIP");
    return;
  }
  const char source[] = "movdir64b 0x20(%ebx,%esi,8), %eax\n";
  for (size_t i = 0; i < Low32BitBuffer::kSize; i++) {
    dst.data()[i] = 0xcc;
  }
  QBDI::rword srcAddr = (QBDI::rword)(src.data() + 64);
  QBDI::rword dstAddr = (QBDI::rword)dst.data();
  ExpectedMemoryAccesses expectedPre = {{
      {srcAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {srcAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
      {dstAddr, 0, 64, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOVDIR64B32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOVDIR64B32", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = dstAddr;
  state->rbx = (QBDI::rword)src.data();
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (size_t i = 64; i < Low32BitBuffer::kSize; i++) {
    CHECK(dst.data()[i] == 0xcc);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOVDIR64B32_EVEX") {
  if (!QBDI::isHostCPUFeaturePresent("movdir64b") or
      !QBDI::isHostCPUFeaturePresent("egpr")) {
    return;
  }
  Low32BitBuffer src;
  Low32BitBuffer dst;
  if (!src.valid() or !dst.valid()) {
    WARN("No verified sub-4GB buffer available on this platform: SKIP");
    return;
  }
  const char source[] = "{evex} movdir64b 0x20(%ebx,%esi,8), %eax\n";
  for (size_t i = 0; i < Low32BitBuffer::kSize; i++) {
    dst.data()[i] = 0xcc;
  }
  QBDI::rword srcAddr = (QBDI::rword)(src.data() + 64);
  QBDI::rword dstAddr = (QBDI::rword)dst.data();
  ExpectedMemoryAccesses expectedPre = {{
      {srcAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {srcAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
      {dstAddr, 0, 64, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOVDIR64B32_EVEX", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("MOVDIR64B32_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = dstAddr;
  state->rbx = (QBDI::rword)src.data();
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (size_t i = 64; i < Low32BitBuffer::kSize; i++) {
    CHECK(dst.data()[i] == 0xcc);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOVDIR64B64") {
  if (!QBDI::isHostCPUFeaturePresent("movdir64b")) {
    return;
  }
  const char source[] = "movdir64b 0x20(%rbx,%rsi,8), %rax\n";
  alignas(64) uint8_t src[192] = {0};
  alignas(64) uint8_t dst[128];
  for (size_t i = 0; i < sizeof(dst); i++) {
    dst[i] = 0xcc;
  }
  QBDI::rword srcAddr = (QBDI::rword)&src[64];
  QBDI::rword dstAddr = (QBDI::rword)&dst[0];
  ExpectedMemoryAccesses expectedPre = {{
      {srcAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {srcAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
      {dstAddr, 0, 64, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOVDIR64B64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOVDIR64B64", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = dstAddr;
  state->rbx = (QBDI::rword)&src[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (size_t i = 64; i < sizeof(dst); i++) {
    CHECK(dst[i] == 0xcc);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOVDIR64B64_EVEX") {
  if (!QBDI::isHostCPUFeaturePresent("movdir64b") or
      !QBDI::isHostCPUFeaturePresent("egpr")) {
    return;
  }
  const char source[] = "{evex} movdir64b 0x20(%rbx,%rsi,8), %rax\n";
  alignas(64) uint8_t src[192] = {0};
  alignas(64) uint8_t dst[128];
  for (size_t i = 0; i < sizeof(dst); i++) {
    dst[i] = 0xcc;
  }
  QBDI::rword srcAddr = (QBDI::rword)&src[64];
  QBDI::rword dstAddr = (QBDI::rword)&dst[0];
  ExpectedMemoryAccesses expectedPre = {{
      {srcAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {srcAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
      {dstAddr, 0, 64, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOVDIR64B64_EVEX", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("MOVDIR64B64_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = dstAddr;
  state->rbx = (QBDI::rword)&src[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (size_t i = 64; i < sizeof(dst); i++) {
    CHECK(dst[i] == 0xcc);
  }
}
