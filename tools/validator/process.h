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
#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>

#include "QBDI/State.h"

class Process {

public:
  virtual ~Process() = default;

  virtual pid_t getPID() = 0;

  virtual void setBreakpoint(void *address) = 0;

  virtual void unsetBreakpoint() = 0;

  virtual void continueExecution() = 0;

  virtual int waitForStatus() = 0;

  virtual void getProcessGPR(QBDI::GPRState *gprState) = 0;

  virtual void getProcessFPR(QBDI::FPRState *fprState) = 0;
};

bool hasExited(int status);

bool hasStopped(int status);

bool hasCrashed(int status);

#endif // PROCESS_H
