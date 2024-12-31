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

#include <catch2/catch_test_macros.hpp>
#include "API/APITest.h"
#include "QBDI/Memory.hpp"

QBDI_NOINLINE uint64_t sinpleStore() {
  uint64_t v = 0x20;

  asm("ldxr x1, [%0]\n"
      "add   x1, x1, #1\n"
      "stxr  w0, x1, [%0]\n"
      :
      : "r"(&v)
      : "x0", "x1", "memory");

  return v;
}

TEST_CASE_METHOD(APITest, "VMTest_AARCH64_LocalMonitor-sinpleStore") {
  QBDI::rword retval;

  vm.call(&retval, (QBDI::rword)sinpleStore, {});
  REQUIRE(retval == (QBDI::rword)0x21);

  SUCCEED();
}

QBDI_NOINLINE uint64_t clearMonitor() {
  uint64_t v = 0x20;

  asm("ldxr x1, [%0]\n"
      "add   x1, x1, #1\n"
      "clrex\n"
      "stxr  w0, x1, [%0]\n"
      :
      : "r"(&v)
      : "x0", "x1", "memory");

  return v;
}

TEST_CASE_METHOD(APITest, "VMTest_AARCH64_LocalMonitor-clearMonitor") {
  QBDI::rword retval;

  vm.call(&retval, (QBDI::rword)clearMonitor, {});
  REQUIRE(retval == (QBDI::rword)0x20);

  SUCCEED();
}

QBDI_NOINLINE uint64_t doubleStackStore() {
  uint64_t v[2] = {0x20, 0x30};

  asm("ldxr x1, [%0]\n"
      "add   x1, x1, #1\n"

      "ldxr x2, [%1]\n"
      "add   x2, x2, #1\n"
      "stxr  w0, x2, [%1]\n"

      "stxr  w0, x1, [%0]\n"
      :
      : "r"(&v[0]), "r"(&v[1])
      : "x0", "x1", "x2", "memory");

  return v[0] | (v[1] << 16);
}

TEST_CASE_METHOD(APITest, "VMTest_AARCH64_LocalMonitor-doubleStackStore") {
  QBDI::rword retval;

  vm.call(&retval, (QBDI::rword)doubleStackStore, {});
  REQUIRE(retval == (QBDI::rword)0x310020);

  SUCCEED();
}

QBDI_NOINLINE uint64_t doubleStore(uint64_t *arg) {
  uint64_t v = 0x20;

  asm("ldxr x1, [%0]\n"
      "add   x1, x1, #1\n"

      "ldxr x2, [%1]\n"
      "stxr  w0, x1, [%0]\n"
      "add   x2, x2, #1\n"
      "stxr  w0, x2, [%1]\n"

      :
      : "r"(&v), "r"(arg)
      : "x0", "x1", "x2", "memory");

  return v;
}

TEST_CASE_METHOD(APITest, "VMTest_AARCH64_LocalMonitor-doubleStore") {
  QBDI::rword retval;
  // stack and heap isn't in the same exclusive range
  uint64_t *allocated_val = new uint64_t;
  *allocated_val = 0x30;

  vm.call(&retval, (QBDI::rword)doubleStore, {(QBDI::rword)allocated_val});
  REQUIRE(retval == (QBDI::rword)0x20);
  REQUIRE(*allocated_val == 0x30);

  delete allocated_val;
  SUCCEED();
}
