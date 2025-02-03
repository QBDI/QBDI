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
#ifndef SHELLCODETESTER_H
#define SHELLCODETESTER_H

#include <stdlib.h>
#include <string.h>

#include "QBDI/VM.h"
#include "ExecBlock/Context.h"

#include "TestSetup/InMemoryAssembler.h"

class ShellcodeTester {
protected:
  using PF = llvm::sys::Memory::ProtectionFlags;

  QBDI::VM vm;

public:
  ShellcodeTester(const std::string &cpu = "",
                  const std::vector<std::string> &mattrs = {})
      : vm(cpu, mattrs) {}

  virtual llvm::sys::MemoryBlock allocateStack(QBDI::rword size);

  virtual void freeStack(llvm::sys::MemoryBlock &memoryBlock);

  virtual InMemoryObject compileWithContextSwitch(const char *source) = 0;

  virtual QBDI::Context jitExec(llvm::ArrayRef<uint8_t> code,
                                QBDI::Context &inputCtx,
                                llvm::sys::MemoryBlock &stack) = 0;

  virtual QBDI::Context realExec(llvm::ArrayRef<uint8_t> code,
                                 QBDI::Context &inputCtx,
                                 llvm::sys::MemoryBlock &stack) = 0;

  virtual void comparedExec(const char *source, QBDI::Context &inputCtx,
                            QBDI::rword stackSize);
};

#endif
