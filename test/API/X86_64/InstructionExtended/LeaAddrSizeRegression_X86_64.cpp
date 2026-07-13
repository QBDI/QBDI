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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-LEA64r_addr64") {
  const char source[] = "movl 0x20(%rbx,%rsi,8), %eax\n";
  alignas(8) uint8_t buffer[64] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[0x28]);
  *target = 0x11223344;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11223344, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(retval == 0x11223344);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-LEA64r_addr32") {
  Low32BitBuffer buf;
  if (!buf.valid()) {
    WARN("No verified sub-4GB buffer available on this platform: SKIP");
    return;
  }
  const char source[] = "movl 0x20(%ebx,%esi,8), %eax\n";
  uint32_t *target = reinterpret_cast<uint32_t *>(buf.data() + 0x28);
  *target = 0x11223344;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11223344, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)buf.data();
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(retval == 0x11223344);
  CHECK(expectedPost.see);
}
