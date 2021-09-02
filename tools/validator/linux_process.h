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
#ifndef LINUX_PROCESS_H
#define LINUX_PROCESS_H

#include "process.h"

#if defined(QBDI_ARCH_X86_64)
#include "X86_64/linux_X86_64.h"
#elif defined(QBDI_ARCH_X86)
#include "X86/linux_X86.h"
#elif defined(QBDI_ARCH_ARM)
#include "ARM/linux_ARM.h"
#elif defined(QBDI_ARCH_AARCH64)
#include "AARCH64/linux_AARCH64.h"
#endif

class LinuxProcess : public Process {

private:
  pid_t pid;
  void *brk_address;
  long brk_value;

public:
  LinuxProcess(pid_t process)
      : pid(process), brk_address(nullptr), brk_value(0) {}

  pid_t getPID() { return pid; }

  void setBreakpoint(void *address);

  void unsetBreakpoint();

  void continueExecution();

  int waitForStatus();

  void getProcessGPR(QBDI::GPRState *gprState);

  void getProcessFPR(QBDI::FPRState *fprState);
};

#endif // LINUX_PROCESS_H
