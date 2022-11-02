/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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
#include "Platform.h"

#include <mach/host_info.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <mach/machine.h>
#include <mach/vm_map.h>
#include <mutex>

#include "llvm/Support/Host.h"
#include "llvm/Support/Process.h"

#include "Utility/AARCH64/server-iOS-jit-user.h"
#include "Utility/LogSys.h"
#include "Utility/System.h"

#ifndef PT_TRACE_ME
#define PT_TRACE_ME 0
#define PT_DETACH 11
#endif

static mach_port_t frida_jit = MACH_PORT_NULL;

extern "C" {
kern_return_t bootstrap_look_up(mach_port_t bp, const char *service_name,
                                mach_port_t *sp);
int ptrace(int request, pid_t pid, caddr_t addr, int data);
}

namespace QBDI {

void init_jit_server() {
  // connect to frida JIT server
  if (frida_jit == MACH_PORT_NULL) {
    bootstrap_look_up(bootstrap_port, "com.apple.uikit.viewservice.frida",
                      &frida_jit);
    if (frida_jit == MACH_PORT_NULL) {
      QBDI_ERROR("Cannot attach to Frida JIT server !");
    }
  }
  // disable code signature enforcement
  ptrace(PT_TRACE_ME, 0, 0, 0);
  ptrace(PT_DETACH, 0, 0, 0);
}

void terminate_jit_server() {
  if (frida_jit != MACH_PORT_NULL) {
    mach_port_deallocate(mach_task_self(), frida_jit);
    frida_jit = MACH_PORT_NULL;
  }
}

std::once_flag rwxflag;
using PF = llvm::sys::Memory::ProtectionFlags;

bool isRWXSupported() {
  static bool cached_result = false;

  std::call_once(rwxflag, []() {
    bool supported = false;
    bool success = false;
    mach_port_t task;
    vm_address_t page = 0;
    vm_address_t address;
    vm_size_t size = 0;
    natural_t depth = 0;
    struct vm_region_submap_info_64 info;
    mach_msg_type_number_t info_count;
    kern_return_t kr;
    size_t pageSize = llvm::sys::Process::getPageSize();

    task = mach_task_self();

    kr = vm_allocate(task, &page, pageSize, VM_FLAGS_ANYWHERE);
    QBDI_REQUIRE_ACTION(kr == KERN_SUCCESS, return );

    llvm::sys::MemoryBlock block((void *)page, pageSize);

    std::error_code ec = llvm::sys::Memory::protectMappedMemory(
        block, PF::MF_READ | PF::MF_WRITE | PF::MF_EXEC);
    success = ec.value() == 0;

    address = page;
    while (success) {
      info_count = VM_REGION_SUBMAP_INFO_COUNT_64;
      kr = vm_region_recurse_64(task, &address, &size, &depth,
                                (vm_region_recurse_info_t)&info, &info_count);
      if (kr != KERN_SUCCESS)
        break;

      if (info.is_submap) {
        depth++;
        continue;
      } else {
        vm_prot_t requested_prot =
            VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE;
        supported = (info.protection & requested_prot) == requested_prot;
        break;
      }
    }

    vm_deallocate(task, page, pageSize);

    cached_result = supported;
  });

  return cached_result;
}

llvm::sys::MemoryBlock
allocateMappedMemory(size_t numBytes,
                     const llvm::sys::MemoryBlock *const nearBlock,
                     unsigned pFlags, std::error_code &ec) {
  // Set default error code
  ec = std::error_code(ENOMEM, std::system_category());
  // create an empty memory block (in case of failure)
  llvm::sys::MemoryBlock empty = llvm::sys::MemoryBlock();
  if (numBytes == 0) {
    return empty;
  }

  // convert size in bytes to a number of pages
  static const size_t pageSize = llvm::sys::Process::getPageSize();
  const size_t numPages = (numBytes + pageSize - 1) / pageSize;

  // try allocate memory
  kern_return_t kr;
  vm_size_t size = numPages * pageSize;
  vm_address_t address = 0;
  mach_vm_address_t page =
      0; // careful here to use correct type as size can be different
  bool setPerms = true;

  // we want executable memory and our process can't have it...
  // try to use a JIT server (Frida one by default)
  if ((pFlags & llvm::sys::Memory::MF_EXEC) && !isRWXSupported()) {
    // lazily init JIT server
    if (frida_jit == MACH_PORT_NULL) {
      init_jit_server();
    }
    QBDI_REQUIRE_ACTION(frida_jit != MACH_PORT_NULL, return empty);
    kr = frida_jit_alloc(frida_jit, mach_task_self(), &page, size,
                         VM_FLAGS_ANYWHERE);
    address = page;
    // no need to set permissions later
    setPerms = false;
  } else {
    kr = vm_allocate(mach_task_self(), &address, size, VM_FLAGS_ANYWHERE);
  }
  QBDI_REQUIRE_ACTION(kr == KERN_SUCCESS, return empty);
  // everything should be fine, set error code to 0
  ec = std::error_code();

  llvm::sys::MemoryBlock Result((void *)address, size);
  // set permissions on memory range (if needed)
  if (setPerms) {
    ec = llvm::sys::Memory::protectMappedMemory(Result, pFlags);
  }

  // flush instruction cache if we successfully allocated executable memory
  if ((ec.value() == 0) && (pFlags & llvm::sys::Memory::MF_EXEC)) {
    llvm::sys::Memory::InvalidateInstructionCache((const void *)address, size);
  }

  return Result;
}

void releaseMappedMemory(llvm::sys::MemoryBlock &block) {
  vm_deallocate(mach_task_self(), (vm_address_t)block.base(), block.size());
}

} // namespace QBDI
