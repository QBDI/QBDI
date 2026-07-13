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
#ifndef QBDI_TEST_MEMACCESSTESTUTILS_X86_64_H
#define QBDI_TEST_MEMACCESSTESTUTILS_X86_64_H

#include <catch2/catch_test_macros.hpp>
#include "API/APITest.h"

#include <cstdio>
#include <vector>

#include "QBDI/Memory.hpp"
#include "QBDI/Platform.h"
#include "QBDI/Range.h"
#include "QBDI/State.h"

#include "Utility/System.h"

namespace QBDITestBatch2 {

static bool checkFeature(const char *f) {
  if (!QBDI::isHostCPUFeaturePresent(f)) {
    // WARN("Host doesn't support " << f << " feature: SKIP");
    return false;
  }
  return true;
}

struct ExpectedMemoryAccess {
  QBDI::rword address;
  QBDI::rword value;
  uint16_t size;
  QBDI::MemoryAccessType type;
  QBDI::MemoryAccessFlags flags;
  bool see = false;
};

struct ExpectedMemoryAccesses {
  std::vector<ExpectedMemoryAccess> accesses;
};

static QBDI::VMAction checkAccess(QBDI::VMInstanceRef vm,
                                  QBDI::GPRState *gprState,
                                  QBDI::FPRState *fprState, void *data) {
  ExpectedMemoryAccesses *info = static_cast<ExpectedMemoryAccesses *>(data);
  std::vector<QBDI::MemoryAccess> memaccesses = vm->getInstMemoryAccess();
  REQUIRE(memaccesses.size() == info->accesses.size());
  for (size_t i = 0; i < info->accesses.size(); i++) {
    auto &memaccess = memaccesses[i];
    auto &expect = info->accesses[i];
    CHECK(memaccess.accessAddress == expect.address);
    CHECK(memaccess.value == expect.value);
    CHECK(memaccess.size == expect.size);
    CHECK(memaccess.type == expect.type);
    expect.see = true;
  }
  return QBDI::VMAction::CONTINUE;
}

static QBDI::VMAction checkEmptyAccess(QBDI::VMInstanceRef vm,
                                       QBDI::GPRState *gprState,
                                       QBDI::FPRState *fprState, void *data) {
  bool *seen = static_cast<bool *>(data);
  CHECK(vm->getInstMemoryAccess().empty());
  *seen = true;
  return QBDI::VMAction::CONTINUE;
}

} // namespace QBDITestBatch2

#endif // QBDI_TEST_MEMACCESSTESTUTILS_X86_64_H
