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
#include "Utility/AARCH64/AssemblyProbe.h"

#include "QBDI/State.h"

namespace {

bool probePAuthOnce() {

  static const QBDI::rword candidates[] = {
      0x0000000012345678ULL,
      0x0000123456789abcULL,
      0x0000cafebabe0000ULL,
      0x0000fedcba987654ULL,
  };
  for (QBDI::rword magic : candidates) {
    QBDI::rword result;
    asm volatile inline(
        "mov x17, %1\n"
        "mov x16, #0\n"
        "hint #8\n" /* pacia1716 */
        "mov %0, x17\n"
        : "=r"(result)
        : "r"(magic)
        : "x16", "x17");
    if (result != magic) {
      return true;
    }
  }
  return false;
}

} // namespace

namespace QBDI {

bool hostSupportsPAuthProbe() {
  // The host's PAuth support never changes during the process lifetime:
  // probe once and cache the result. Local static init is thread-safe
  // per the C++11 standard.
  static const bool hasPauth = probePAuthOnce();
  return hasPauth;
}

} // namespace QBDI
