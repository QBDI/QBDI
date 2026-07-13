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
#include <cstdio>

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
using QBDITestBatch2::checkedSnprintf;
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

template <size_t N>
void encodeAddr32LE(uint32_t addr, char (&out)[N]) {
  checkedSnprintf(out, "0x%02x,0x%02x,0x%02x,0x%02x", addr & 0xff,
                  (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
}

} // namespace

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOV8ao32") {
  Low32BitBuffer buf;
  if (!buf.valid()) {
    WARN("No verified sub-4GB buffer available on this platform: SKIP");
    return;
  }
  uint8_t *target = buf.data() + 0x10;
  *target = 0x2a;
  QBDI::rword targetAddr = (QBDI::rword)target;
  char addrBytes[32];
  encodeAddr32LE((uint32_t)(uintptr_t)target, addrBytes);
  char source[128];
  // mov 0x<target>, %al (67 = addr32 prefix, a0 = moffs8->AL opcode,
  // MOV8ao32; <target> is the runtime address of `target`, little-endian)
  checkedSnprintf(source, ".byte 0x67,0xa0,%s\n", addrBytes);
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x2a, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2a, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV8ao32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOV8ao32", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK((vm.getGPRState()->rax & 0xff) == 0x2a);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOV8o32a") {
  Low32BitBuffer buf;
  if (!buf.valid()) {
    WARN("No verified sub-4GB buffer available on this platform: SKIP");
    return;
  }
  uint8_t *target = buf.data() + 0x10;
  *target = 0;
  QBDI::rword targetAddr = (QBDI::rword)target;
  char addrBytes[32];
  encodeAddr32LE((uint32_t)(uintptr_t)target, addrBytes);
  char source[128];
  // mov %al, 0x<target> (67 = addr32 prefix, a2 = AL->moffs8 opcode,
  // MOV8o32a; <target> is the runtime address of `target`, little-endian)
  checkedSnprintf(source, ".byte 0x67,0xa2,%s\n", addrBytes);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2a, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV8o32a", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOV8o32a", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0x2a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2a);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOV16ao32") {
  Low32BitBuffer buf;
  if (!buf.valid()) {
    WARN("No verified sub-4GB buffer available on this platform: SKIP");
    return;
  }
  uint16_t *target = reinterpret_cast<uint16_t *>(buf.data() + 0x10);
  *target = 0x2a2a;
  QBDI::rword targetAddr = (QBDI::rword)target;
  char addrBytes[32];
  encodeAddr32LE((uint32_t)(uintptr_t)target, addrBytes);
  char source[128];
  // mov 0x<target>, %ax (67 = addr32 prefix, 66 = 16-bit operand-size
  // prefix, a1 = moffs->AX opcode, MOV16ao32; <target> is the runtime
  // address of `target`, little-endian)
  checkedSnprintf(source, ".byte 0x67,0x66,0xa1,%s\n", addrBytes);
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x2a2a, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2a2a, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV16ao32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOV16ao32", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK((vm.getGPRState()->rax & 0xffff) == 0x2a2a);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOV16o32a") {
  Low32BitBuffer buf;
  if (!buf.valid()) {
    WARN("No verified sub-4GB buffer available on this platform: SKIP");
    return;
  }
  uint16_t *target = reinterpret_cast<uint16_t *>(buf.data() + 0x10);
  *target = 0;
  QBDI::rword targetAddr = (QBDI::rword)target;
  char addrBytes[32];
  encodeAddr32LE((uint32_t)(uintptr_t)target, addrBytes);
  char source[128];
  // mov %ax, 0x<target> (67 = addr32 prefix, 66 = 16-bit operand-size
  // prefix, a3 = AX->moffs opcode, MOV16o32a; <target> is the runtime
  // address of `target`, little-endian)
  checkedSnprintf(source, ".byte 0x67,0x66,0xa3,%s\n", addrBytes);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2a2a, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV16o32a", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOV16o32a", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0x2a2a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2a2a);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOV32ao32") {
  Low32BitBuffer buf;
  if (!buf.valid()) {
    WARN("No verified sub-4GB buffer available on this platform: SKIP");
    return;
  }
  uint32_t *target = reinterpret_cast<uint32_t *>(buf.data() + 0x10);
  *target = 0x2a2a2a2a;
  QBDI::rword targetAddr = (QBDI::rword)target;
  char addrBytes[32];
  encodeAddr32LE((uint32_t)(uintptr_t)target, addrBytes);
  char source[128];
  // mov 0x<target>, %eax (67 = addr32 prefix, a1 = moffs->EAX opcode,
  // MOV32ao32; <target> is the runtime address of `target`, little-endian)
  checkedSnprintf(source, ".byte 0x67,0xa1,%s\n", addrBytes);
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x2a2a2a2a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2a2a2a2a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV32ao32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOV32ao32", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK((vm.getGPRState()->rax & 0xffffffff) == 0x2a2a2a2a);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOV32o32a") {
  Low32BitBuffer buf;
  if (!buf.valid()) {
    WARN("No verified sub-4GB buffer available on this platform: SKIP");
    return;
  }
  uint32_t *target = reinterpret_cast<uint32_t *>(buf.data() + 0x10);
  *target = 0;
  QBDI::rword targetAddr = (QBDI::rword)target;
  char addrBytes[32];
  encodeAddr32LE((uint32_t)(uintptr_t)target, addrBytes);
  char source[128];
  // mov %eax, 0x<target> (67 = addr32 prefix, a3 = EAX->moffs opcode,
  // MOV32o32a; <target> is the runtime address of `target`, little-endian)
  checkedSnprintf(source, ".byte 0x67,0xa3,%s\n", addrBytes);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2a2a2a2a, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV32o32a", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOV32o32a", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0x2a2a2a2a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2a2a2a2a);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOV64ao32") {
  Low32BitBuffer buf;
  if (!buf.valid()) {
    WARN("No verified sub-4GB buffer available on this platform: SKIP");
    return;
  }
  uint64_t *target = reinterpret_cast<uint64_t *>(buf.data() + 0x10);
  *target = 0x2a2a2a2a2a2a2a2a;
  QBDI::rword targetAddr = (QBDI::rword)target;
  char addrBytes[32];
  encodeAddr32LE((uint32_t)(uintptr_t)target, addrBytes);
  char source[128];
  // mov 0x<target>, %rax (67 = addr32 prefix, 48 = REX.W, a1 =
  // moffs->RAX opcode, MOV64ao32; <target> is the runtime address of
  // `target`, little-endian)
  checkedSnprintf(source, ".byte 0x67,0x48,0xa1,%s\n", addrBytes);
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x2a2a2a2a2a2a2a2a, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2a2a2a2a2a2a2a2a, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV64ao32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOV64ao32", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(vm.getGPRState()->rax == 0x2a2a2a2a2a2a2a2a);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOV64o32a") {
  Low32BitBuffer buf;
  if (!buf.valid()) {
    WARN("No verified sub-4GB buffer available on this platform: SKIP");
    return;
  }
  uint64_t *target = reinterpret_cast<uint64_t *>(buf.data() + 0x10);
  *target = 0;
  QBDI::rword targetAddr = (QBDI::rword)target;
  char addrBytes[32];
  encodeAddr32LE((uint32_t)(uintptr_t)target, addrBytes);
  char source[128];
  // mov %rax, 0x<target> (67 = addr32 prefix, 48 = REX.W, a3 =
  // RAX->moffs opcode, MOV64o32a; <target> is the runtime address of
  // `target`, little-endian)
  checkedSnprintf(source, ".byte 0x67,0x48,0xa3,%s\n", addrBytes);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2a2a2a2a2a2a2a2a, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV64o32a", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOV64o32a", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0x2a2a2a2a2a2a2a2a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2a2a2a2a2a2a2a2a);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
