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
#ifndef QBDITEST_VMTEST_AARCH64_H
#define QBDITEST_VMTEST_AARCH64_H

#include <unordered_map>
#include <vector>

#include "QBDI/Memory.hpp"
#include "QBDI/Platform.h"
#include "QBDI/VM.h"

QBDI_NOINLINE QBDI::rword satanicFun(QBDI::rword arg0);

#define MNEM_COUNT 1u
#define MNEM_VALIDATION 33u

#define MAX_OPERAND 4
#define MNEM_CMP "SUBS*"

struct SizedTestCode {
  std::vector<uint8_t> code;
  std::size_t size;
};

struct TestInst {
  uint32_t instSize;
  uint8_t numOperands;
  bool isCompare;
  QBDI::RegisterAccessType flagsAccess;
  QBDI::OperandAnalysis operands[MAX_OPERAND];
};

extern const struct TestInst TestInsts[MNEM_COUNT];
extern std::unordered_map<std::string, SizedTestCode> TestCode;

#define SKIPTESTASM "nop\nnop\nret\n"

#endif
