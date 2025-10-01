/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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
#include <mach/host_info.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <mach/machine.h>
#include <mach/vm_map.h>
#include <mutex>

#include "llvm/Support/Process.h"
#include "llvm/TargetParser/Host.h"

#include "Utility/LogSys.h"
#include "Utility/System.h"

#ifndef PT_TRACE_ME
#define PT_TRACE_ME 0
#define PT_DETACH 11
#endif

namespace QBDI {

std::once_flag rwxflag;
using PF = llvm::sys::Memory::ProtectionFlags;

namespace {

vm_prot_t getPageProtection(vm_address_t address_, mach_port_t &task) {
  vm_address_t address = address_;
  vm_size_t size = 0;
  natural_t depth = 0;
  kern_return_t kr;
  struct vm_region_submap_info_64 info;
  mach_msg_type_number_t info_count;
  while (true) {
    info_count = VM_REGION_SUBMAP_INFO_COUNT_64;
    kr = vm_region_recurse_64(task, &address, &size, &depth,
                              (vm_region_recurse_info_t)&info, &info_count);
    if (kr != KERN_SUCCESS)
      break;

    if (info.is_submap) {
      depth++;
      continue;
    } else {
      return (info.protection &
              (VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE));
    }
  }
  return 0;
}

bool cached_RWX_supported = false;
bool cached_RW_RX_supported = false;

void cache_RWXSupported() {
  bool success = false;
  mach_port_t task;
  vm_address_t page = 0;
  kern_return_t kr;
  size_t pageSize = llvm::sys::Process::getPageSize().get();

  task = mach_task_self();

  kr = vm_allocate(task, &page, pageSize, VM_FLAGS_ANYWHERE);
  QBDI_REQUIRE_ACTION(kr == KERN_SUCCESS, return);

  llvm::sys::MemoryBlock block((void *)page, pageSize);

  std::error_code ec = llvm::sys::Memory::protectMappedMemory(
      block, PF::MF_READ | PF::MF_WRITE | PF::MF_EXEC);
  success = ec.value() == 0;

  if (success) {
    cached_RWX_supported = getPageProtection(page, task) ==
                           (VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE);
  } else {
    cached_RWX_supported = false;
  }

  ec =
      llvm::sys::Memory::protectMappedMemory(block, PF::MF_READ | PF::MF_WRITE);
  cached_RW_RX_supported = ec.value() == 0;
  if (cached_RW_RX_supported) {
    cached_RW_RX_supported &=
        getPageProtection(page, task) == (VM_PROT_READ | VM_PROT_WRITE);
  }
  if (cached_RW_RX_supported) {
    ec = llvm::sys::Memory::protectMappedMemory(block,
                                                PF::MF_READ | PF::MF_EXEC);
    cached_RW_RX_supported &= ec.value() == 0;
  }
  if (cached_RW_RX_supported) {
    cached_RW_RX_supported &=
        getPageProtection(page, task) == (VM_PROT_READ | VM_PROT_EXECUTE);
  }

  vm_deallocate(task, page, pageSize);

  QBDI_DEBUG("Support RW_RX pages : {}", cached_RW_RX_supported);
  QBDI_DEBUG("Support RWX pages : {}", cached_RWX_supported);
  if (not(cached_RW_RX_supported or cached_RWX_supported)) {
    QBDI_CRITICAL("Cannot create JIT compatible page");
  }
}

} // anonymous namespace

bool isRWXSupported() {
  std::call_once(rwxflag, cache_RWXSupported);

  return cached_RWX_supported;
}

bool isRWRXSupported() {
  std::call_once(rwxflag, cache_RWXSupported);

  return cached_RW_RX_supported;
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
  static const size_t pageSize = llvm::sys::Process::getPageSize().get();
  const size_t numPages = (numBytes + pageSize - 1) / pageSize;

  // try allocate memory
  kern_return_t kr;
  vm_size_t size = numPages * pageSize;
  vm_address_t address = 0;

  kr = vm_allocate(mach_task_self(), &address, size, VM_FLAGS_ANYWHERE);
  QBDI_REQUIRE_ACTION(kr == KERN_SUCCESS, return empty);
  // everything should be fine, set error code to 0
  ec = std::error_code();

  llvm::sys::MemoryBlock Result((void *)address, size);
  // set permissions on memory range (if needed)
  ec = llvm::sys::Memory::protectMappedMemory(Result, pFlags);

  // flush instruction cache if we successfully allocated executable memory
  if ((ec.value() == 0) && (pFlags & llvm::sys::Memory::MF_EXEC)) {
    llvm::sys::Memory::InvalidateInstructionCache((const void *)address, size);
  }

  return Result;
}

void releaseMappedMemory(llvm::sys::MemoryBlock &block) {
  vm_deallocate(mach_task_self(), (vm_address_t)block.base(),
                block.allocatedSize());
}

} // namespace QBDI
