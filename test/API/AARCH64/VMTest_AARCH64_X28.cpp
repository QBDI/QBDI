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
#include <cstdint>
#include <stdio.h>

#include <catch2/catch_test_macros.hpp>
#include "API/APITest.h"
#include "QBDI/Memory.hpp"

static QBDI::VMAction dummyCB(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
                              QBDI::FPRState *fprState, void *data) {
  return QBDI::VMAction::CONTINUE;
}

QBDI_NOINLINE uint64_t useX28_simple() {
  uint64_t v = 0x20;
  uint64_t r = 0x0;

  asm("  mov  x28, %1\n"
      "  mov %0, x28\n"
      : "=r"(r)
      : "r"(v)
      : "x28");

  return r;
}

TEST_CASE_METHOD(APITest, "VMTest_AARCH64_X28_RegisterSet_simple") {
  QBDI::rword retval = 0;

  state->x28 = 0;
  vm.call(&retval, (QBDI::rword)useX28_simple, {});
  INFO("Case x28 == 0");
  CHECK(retval == (QBDI::rword)0x20);

  state->x28 = 0x3269;
  retval = 0;
  vm.call(&retval, (QBDI::rword)useX28_simple, {});
  INFO("Case x28 == 0x3269");
  CHECK(retval == (QBDI::rword)0x20);

  state->x28 = 0x0;
  retval = 0;
  uint32_t instrId =
      vm.addCodeCB(QBDI::InstPosition::PREINST, dummyCB, nullptr);
  vm.call(&retval, (QBDI::rword)useX28_simple, {});
  INFO("Case With PreInst Callback");
  CHECK(retval == (QBDI::rword)0x20);
  vm.deleteInstrumentation(instrId);

  state->x28 = 0x0;
  retval = 0;
  vm.addCodeCB(QBDI::InstPosition::POSTINST, dummyCB, nullptr);
  vm.call(&retval, (QBDI::rword)useX28_simple, {});
  INFO("Case With PostInst Callback");
  CHECK(retval == (QBDI::rword)0x20);

  state->x28 = 0x0;
  retval = 0;
  vm.addCodeCB(QBDI::InstPosition::PREINST, dummyCB, nullptr);
  vm.call(&retval, (QBDI::rword)useX28_simple, {});
  INFO("Case With PreInst and PostInst Callback");
  CHECK(retval == (QBDI::rword)0x20);

  SUCCEED();
}

QBDI_NOINLINE uint64_t useX28_crossBB() {
  uint64_t v = 0x20;
  uint64_t r = 0x0;

  asm("  mov  x28, %1\n"
      "  b label\n"
      "label:\n"
      "  mov %0, x28\n"
      : "=r"(r)
      : "r"(v)
      : "x28");

  return r;
}

TEST_CASE_METHOD(APITest, "VMTest_AARCH64_X28_RegisterSet_crossBB") {
  QBDI::rword retval = 0;

  state->x28 = 0;
  vm.call(&retval, (QBDI::rword)useX28_crossBB, {});
  INFO("Case x28 == 0");
  CHECK(retval == (QBDI::rword)0x20);

  state->x28 = 0x3269;
  retval = 0;
  vm.call(&retval, (QBDI::rword)useX28_crossBB, {});
  INFO("Case x28 == 0x3269");
  CHECK(retval == (QBDI::rword)0x20);

  state->x28 = 0x0;
  retval = 0;
  uint32_t instrId =
      vm.addCodeCB(QBDI::InstPosition::PREINST, dummyCB, nullptr);
  vm.call(&retval, (QBDI::rword)useX28_crossBB, {});
  INFO("Case With PreInst Callback");
  CHECK(retval == (QBDI::rword)0x20);
  vm.deleteInstrumentation(instrId);

  state->x28 = 0x0;
  retval = 0;
  vm.addCodeCB(QBDI::InstPosition::POSTINST, dummyCB, nullptr);
  vm.call(&retval, (QBDI::rword)useX28_crossBB, {});
  INFO("Case With PostInst Callback");
  CHECK(retval == (QBDI::rword)0x20);

  state->x28 = 0x0;
  retval = 0;
  vm.addCodeCB(QBDI::InstPosition::PREINST, dummyCB, nullptr);
  vm.call(&retval, (QBDI::rword)useX28_crossBB, {});
  INFO("Case With PreInst and PostInst Callback");
  CHECK(retval == (QBDI::rword)0x20);

  SUCCEED();
}
