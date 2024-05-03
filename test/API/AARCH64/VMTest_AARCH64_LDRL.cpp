/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
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

#include <catch2/catch.hpp>
#include "API/APITest.h"
#include "QBDI/Memory.hpp"

QBDI_NOINLINE uint64_t ldrlX() {
  uint64_t r;

  asm("  ldr %0, labelX\n"
      "  b targetX\n"
      "labelX:\n"
      "  brk 65535\n"
      "  brk 65535\n"
      "targetX:\n"
      "  nop\n"
      : "=r"(r));

  return r;
}

TEST_CASE_METHOD(APITest, "VMTest_AARCH64_LDRLX") {
  QBDI::rword retval = 0;

  vm.call(&retval, (QBDI::rword)ldrlX, {});
  CHECK(retval == 0xD43FFFE0D43FFFE0ull);

  SUCCEED();
}

QBDI_NOINLINE uint64_t ldrlW_1() {
  uint64_t r;

  asm("  ldr w1, labelW\n"
      "  mov %0, x1\n"
      "  b targetW\n"
      "labelW:\n"
      "  brk 65535\n"
      "targetW:\n"
      "  nop\n"
      : "=r"(r)::"x1");

  return r;
}

TEST_CASE_METHOD(APITest, "VMTest_AARCH64_LDRLW_1") {
  QBDI::rword retval = 0;

  vm.call(&retval, (QBDI::rword)ldrlW_1, {});
  CHECK(retval == 0xD43FFFE0ull);

  SUCCEED();
}

QBDI_NOINLINE uint64_t ldrlW_2() {
  uint64_t r;

  asm("  b targetW2\n"
      "labelW2:\n"
      "  brk 65535\n"
      "targetW2:\n"
      "  ldr w1, labelW2\n"
      "  mov %0, x1\n"
      : "=r"(r)::"x1");

  return r;
}

TEST_CASE_METHOD(APITest, "VMTest_AARCH64_LDRLW_2") {
  QBDI::rword retval = 0;

  vm.call(&retval, (QBDI::rword)ldrlW_2, {});
  CHECK(retval == 0xD43FFFE0ull);

  SUCCEED();
}

QBDI_NOINLINE uint64_t ldrlSW_1() {
  uint64_t r;

  asm("  ldrsw %0, labelSW\n"
      "  b targetSW\n"
      "labelSW:\n"
      "  brk 65535\n"
      "targetSW:\n"
      "  nop\n"
      : "=r"(r));

  return r;
}

TEST_CASE_METHOD(APITest, "VMTest_AARCH64_LDRLSW_1") {
  QBDI::rword retval = 0;

  vm.call(&retval, (QBDI::rword)ldrlSW_1, {});
  CHECK(retval == 0xFFFFFFFFD43FFFE0ull);

  SUCCEED();
}

QBDI_NOINLINE uint64_t ldrlSW_2() {
  uint64_t r;

  asm("  b targetSW2\n"
      "labelSW2:\n"
      "  brk 65535\n"
      "targetSW2:\n"
      "  ldrsw %0, labelSW2\n"
      : "=r"(r));

  return r;
}

TEST_CASE_METHOD(APITest, "VMTest_AARCH64_LDRLSW_2") {
  QBDI::rword retval = 0;

  vm.call(&retval, (QBDI::rword)ldrlSW_2, {});
  CHECK(retval == 0xFFFFFFFFD43FFFE0ull);

  SUCCEED();
}
