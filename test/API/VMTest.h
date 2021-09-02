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
#ifndef QBDITEST_VMTEST_H
#define QBDITEST_VMTEST_H

#include <memory>

#include "QBDI/VM.h"

#define STACK_SIZE 4096
#define FAKE_RET_ADDR 0x666

class VMTest {
public:
  VMTest();
  ~VMTest();

  std::unique_ptr<QBDI::VM> vm;
  QBDI::GPRState *state;
  uint8_t *fakestack;
};

#if defined(QBDI_ARCH_X86)
#include "X86/VMTest_X86.h"
#elif defined(QBDI_ARCH_X86_64)
#include "X86_64/VMTest_X86_64.h"
#elif defined(QBDI_ARCH_ARM)
#include "ARM/VMTest_ARM.h"
#elif defined(QBDI_ARCH_AARCH64)
#include "AARCH64/VMTest_AARCH64.h"
#else
#error "Architecture not supported"
#endif

#endif /* QBDITEST_VMTEST_H */
